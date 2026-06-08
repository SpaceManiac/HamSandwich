#include "world_io_supreme.h"
#include <SDL3/SDL_iostream.h>
#include "appdata.h"
#include "world.h"
#include "map.h"
#include "tile.h"
#include "sound.h"
#include "log.h"
#include "monster.h"
#include "worldstitch.h"

bool Supreme_GetWorldName(SDL_IOStream *f, StringDestination name, StringDestination author)
{
	char authbuffer[32], namebuffer[32];

	if (SDL_SeekIO(f, 8, SDL_IO_SEEK_SET) < 0)
		return false;
	if (SDL_ReadIO(f, authbuffer, 32) < 32)
		return false;
	if (SDL_ReadIO(f, namebuffer, 32) < 32)
		return false;

	authbuffer[31] = 0;
	author.assign(authbuffer);
	namebuffer[31] = 0;
	name.assign(namebuffer);

	return true;
}

// ----------------------------------------------------------------------------

// SERIALIZED.
struct IoTerrain
{
	word flags;
	word next;
};
static_assert(sizeof(IoTerrain) == 4);

static terrain_t LoadOneTerrain(IoTerrain io_terrain)
{
	terrain_t r = { (TileFlags)io_terrain.flags, static_cast<word>(io_terrain.next & 0x3ff) };
	if (io_terrain.next & (1 << 15))
		r.flags |= TF_SHADOWLESS;
	return r;
}

static IoTerrain SaveOneTerrain(terrain_t terrain)
{
	IoTerrain r = { static_cast<word>(terrain.flags & 0xffffff), terrain.next };
	if (terrain.flags & TF_SHADOWLESS)
		r.next |= (1 << 15);
	return r;
}

// SERIALIZED.
struct IoTile
{
	word floor;
	word wall;
	byte item;
	char light;
};
static_assert(sizeof(IoTile) == 6);

// SERIALIZED.
struct IoSoundDesc
{
	word num;
	char name[32];
	byte theme;
};
static_assert(sizeof(IoSoundDesc) == 36);

// ----------------------------------------------------------------------------

static void LoadTerrain(world_t *world, const char *fname, SDL_IOStream *f)
{
	for (terrain_t &terrain : world->Terrain())
	{
		IoTerrain io_terrain;
		SDL_ReadIO(f, &io_terrain, sizeof(IoTerrain));
		terrain = LoadOneTerrain(io_terrain);
	}

	// In 2012, Shadowless Wall was added as an additional meaning of the
	// Transparent Roof flag. However, only about 10 worlds since then have
	// used the feature, while about 150 worlds from before that point are at
	// some risk of unexpected appearance due to the change. Therefore, with
	// the flags being split in 2023, only autofill the new flag for those
	// specific worlds, indicated by the presence of this marker file.
	std::string buf = fname;
	buf.append(".shadowless");
	if (AppdataOpen(buf.c_str()))
	{
		for (terrain_t &terrain : world->Terrain())
		{
			if (terrain.flags & TF_TRANS)
				terrain.flags |= TF_SHADOWLESS;
		}
	}
}

static void LoadSpecial(SDL_IOStream *f, special_t *s)
{
	byte numTrig,numEff,b;

	memset(s,0,sizeof(special_t));

	static_assert(sizeof(s->x) == 1);
	static_assert(sizeof(s->y) == 1);
	static_assert(sizeof(s->uses) == 1);
	SDL_ReadIO(f,&s->x,1);
	SDL_ReadIO(f,&s->y,1);
	SDL_ReadIO(f,&s->uses,1);

	SDL_ReadIO(f,&b,1);	// read a combined number indicating #trigs & #effs

	numTrig=b%8;
	numEff=b/8;

	static_assert(sizeof(trigger_t) == 12);
	static_assert(sizeof(effect_t) == 44);
	if(numTrig>0)
		SDL_ReadIO(f,s->trigger,numTrig*sizeof(trigger_t));
	if(numEff>0)
		SDL_ReadIO(f,s->effect,numEff*sizeof(effect_t));

	for (int i = 0; i < numEff; ++i)
	{
		if (s->effect[i].type == EFF_SOUND)
			s->effect[i].value = DescIndexToSound(s->effect[i].value);
	}
}

static void LoadSpecials(SDL_IOStream *f, std::span<special_t> list)
{
	byte numSpecials;
	SDL_ReadIO(f, &numSpecials, 1);	// num specials

	size_t i = 0;
	for(; i < numSpecials && i < list.size(); ++i)
		LoadSpecial(f, &list[i]);
	// TODO: could blow up here if numSpecials < list.size()
	for(; i < list.size(); ++i)
		list[i].x = 255;
}

static void LoadMapData(SDL_IOStream *f, Map *me)
{
	int pos,i;
	int8_t readRun;
	IoTile *mapCopy;

	mapCopy=new IoTile[me->width*me->height];

	pos=0;
	while(pos<me->width*me->height)
	{
		SDL_ReadIO(f,&readRun,1);
		if(readRun<0)	// repeat run
		{
			readRun=-readRun;
			SDL_ReadIO(f,&mapCopy[pos],sizeof(IoTile));
			for(i=1;i<readRun;i++)
				mapCopy[pos+i] = mapCopy[pos];
			pos+=readRun;
		}
		else	// unique run
		{
			SDL_ReadIO(f,&mapCopy[pos],readRun*sizeof(IoTile));
			pos+=readRun;
		}
	}

	for(pos=0;pos<me->width*me->height;pos++)
	{
		me->map[pos].floor=mapCopy[pos].floor;
		me->map[pos].wall=mapCopy[pos].wall;
		me->map[pos].item=mapCopy[pos].item;
		me->map[pos].light=mapCopy[pos].light;
		me->map[pos].select=1;
		me->map[pos].opaque=0;
		me->map[pos].templight=0;
	}
	delete[] mapCopy;
}

static Map *LoadMap(SDL_IOStream *f)
{
	byte width, height;
	SDL_ReadIO(f,&width,1);
	SDL_ReadIO(f,&height,1);

	Map *me = new Map(width, height, "");

	static_assert(sizeof(me->name) == 32);
	static_assert(sizeof(me->song) == 32);
	SDL_ReadIO(f,me->name,32);
	SDL_ReadIO(f,me->song,32);

	byte count;
	SDL_ReadIO(f,&count,1);	// num badguys
	me->badguy.fill({});
	// TODO: blows up if count > MAX_MAPMONS
	for (byte i = 0; i < count; ++i)
	{
		static_assert(sizeof(me->badguy[i].x) == 1);
		static_assert(sizeof(me->badguy[i].y) == 1);
		static_assert(sizeof(me->badguy[i].item) == 1);

		byte temp;
		SDL_ReadIO(f,&me->badguy[i].x, 1);
		SDL_ReadIO(f,&me->badguy[i].y, 1);
		SDL_ReadIO(f,&temp, 1);
		me->badguy[i].type = temp;
		SDL_ReadIO(f,&me->badguy[i].item, 1);
	}

	LoadSpecials(f, me->special);

	static_assert(sizeof(me->flags) == 2);
	static_assert(sizeof(me->numBrains) == 2);
	static_assert(sizeof(me->numCandles) == 2);
	static_assert(sizeof(me->itemDrops) == 2);
	SDL_ReadIO(f,&me->flags,sizeof(word));
	SDL_ReadIO(f,&me->numBrains,sizeof(word));
	SDL_ReadIO(f,&me->numCandles,sizeof(word));
	SDL_ReadIO(f,&me->itemDrops,sizeof(word));

	LoadMapData(f, me);

	return me;
}

static void LoadItems(SDL_IOStream *f)
{
	word changedItems;
	SDL_ReadIO(f,&changedItems,sizeof(word));

	int i = 0;
	for (; i < changedItems; ++i)
	{
		byte curItem = 0;
		SDL_ReadIO(f, &curItem, 1);
		if (curItem == 255)
		{
			break;
		}
		SDL_ReadIO(f, GetItem(curItem), sizeof(item_t));
	}
	for (; i < changedItems; ++i)
	{
		int curItem = NewItem();
		SDL_ReadIO(f, GetItem(curItem), sizeof(item_t));
	}
}

static bool AppendItems(SDL_IOStream *f)
{
	word changedItems;
	SDL_ReadIO(f,&changedItems,sizeof(word));

	int i = 0;
	for (; i < changedItems; ++i)
	{
		byte curItem = 0;
		SDL_ReadIO(f, &curItem, 1);
		if (curItem == 255)
		{
			break;
		}
		// throw away any mods of regular items
		item_t garbage;
		SDL_ReadIO(f, &garbage, sizeof(item_t));
	}
	for (; i < changedItems; ++i)
	{
		int curItem = NewItem();
		if (curItem == -1)
		{
			return false;
		}
		SDL_ReadIO(f, GetItem(curItem), sizeof(item_t));
	}
	return true;
}

static bool LoadCustomSounds(SDL_IOStream *f)
{
	int numCustom;
	SDL_ReadIO(f,&numCustom,sizeof(int));
	for (int i = 0; i < numCustom; ++i)
	{
		IoSoundDesc ioDesc;
		int32_t size;
		byte *data;

		static_assert(sizeof(IoSoundDesc) == 36);
		SDL_ReadIO(f,&ioDesc,sizeof(IoSoundDesc));
		SDL_ReadIO(f,&size,sizeof(int32_t));
		data=(byte *)malloc(size);
		SDL_ReadIO(f,data,size);

		SoundDesc *desc = AddCustomSound(data, size);
		if (!desc)
		{
			return false;
		}
		//desc->num = CUSTOM_SND_START + i; // ioDesc.num always equals this in conforming .dlw files.
		ham_strcpy(desc->name, ioDesc.name);
		desc->theme = ST_CUSTOM; // ioDesc.theme always equals ST_CUSTOM in conforming .dlw files.
		// Exception: rainbow.dlw has some extraneous sounds (with invalid data)
		// which were previously hidden from the editor. Those show now.
	}
	return true;
}

bool Supreme_LoadWorld(world_t *world, const char *fname, SDL_IOStream *f)
{
	char code[9];
	SDL_ReadIO(f, code, 8);
	code[8] = '\0';
	if (strcmp(code, "SUPREME!"))
		return false;

	SDL_ReadIO(f,&world->author,32);
	SDL_SeekIO(f, 32, SDL_IO_SEEK_CUR);  // name of the world, not needed here
	SDL_ReadIO(f,&world->numMaps,1);
	SDL_SeekIO(f, sizeof(int), SDL_IO_SEEK_CUR);  // totalPoints
	SDL_ReadIO(f,&world->numTiles,sizeof(word));	// tile count

	world->tilegfx.LoadTiles(f, world->numTiles);

	LoadTerrain(world, fname, f);

	for(int i = 0; i < MAX_MAPS; i++)
		world->map[i]=NULL;
	for(int i = 0; i < world->numMaps; i++)
	{
		world->map[i] = LoadMap(f);
		if(!world->map[i])
		{
			return false;
		}
	}

	ExitItems();
	InitItems();
	LoadItems(f);

	ClearCustomSounds();
	LoadCustomSounds(f);

	SetupRandomItems();
	return true;
}

bool BeginAppendWorld(world_t *world, const char *fname)
{
	int i;
	char code[32];

	auto f = AppdataOpen(fname);
	if(!f)
	{
		SetStitchError("File Not Found");
		return false;
	}

	SDL_ReadIO(f,code,8);
	code[8]='\0';
	if(strcmp(code,"SUPREME!"))
	{
		SetStitchError("Must be a Supreme world.");
		return false;
	}

	SDL_ReadIO(f,&world->author,32);
	SDL_SeekIO(f, 32, SDL_IO_SEEK_CUR);  // name of the world, not needed here
	SDL_ReadIO(f,&world->numMaps,1);
	SDL_SeekIO(f, sizeof(int), SDL_IO_SEEK_CUR);  // totalPoints
	SDL_ReadIO(f,&world->numTiles,sizeof(word));	// tile count

	world->tilegfx.LoadTiles(f.get(), world->numTiles);
	LoadTerrain(world, fname, f.get());

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i] = LoadMap(f.get());
		if(!world->map[i])
		{
			SetStitchError("Unable to load a level.");
			for(int j=0;j<i;j++)
				delete world->map[j];
			return false;
		}
	}

	if(!AppendItems(f.get()))
	{
		SetStitchError("Too many custom items!");
		return false;
	}
	if(!LoadCustomSounds(f.get()))
	{
		SetStitchError("Too many custom sounds!");
		return false;
	}
	CalculateItemRenderExtents();
	SetupRandomItems();
	return true;
}

// ----------------------------------------------------------------------------

static void SaveSpecial(SDL_IOStream *f, const special_t *s)
{
	byte numTrig,numEff,b;
	int i;

	SDL_WriteIO(f,&s->x,1);
	SDL_WriteIO(f,&s->y,1);
	SDL_WriteIO(f,&s->uses,1);

	numTrig=0;
	numEff=0;
	for(i=0;i<NUM_TRIGGERS;i++)
		if(s->trigger[i].type)
		{
			numTrig=i+1;
			if(s->trigger[i].type==TRG_EQUATION || s->trigger[i].type==TRG_EQUVAR)
				numEff=i+1;
		}
	for(i=0;i<NUM_EFFECTS;i++)
		if(s->effect[i].type && numEff<i+1)
			numEff=i+1;

	b=numTrig+numEff*8;
	SDL_WriteIO(f,&b,1);	// write a combined number indicating #trigs & #effs

	if(numTrig>0)
		SDL_WriteIO(f,s->trigger,numTrig*sizeof(trigger_t));
	for (int i = 0; i < numEff; ++i)
	{
		effect_t eff = s->effect[i];
		if (eff.type == EFF_SOUND)
			eff.value = SoundToDescIndex(eff.value);
		SDL_WriteIO(f,&eff,sizeof(effect_t));
	}
}

static void SaveSpecials(SDL_IOStream *f, std::span<const special_t> list)
{
	// First, count specials that actually exist.
	byte numSpecials = 0;
	for (const special_t &spcl : list)
	{
		if (spcl.x != 255)
		{
			if (numSpecials == UINT8_MAX)
			{
				// Supreme_CanSaveMap should be false if this is true, but this
				// code is still called when saving the .dlw_old backup.
				LogError("WARNING: in Supreme save, can't save more than 255 specials");
				break;
			}
			numSpecials++;
		}
	}

	SDL_WriteIO(f,&numSpecials,1);	// num specials

	// Now, write those out.
	for (special_t spcl : list) // Intentional copy.
	{
		if (spcl.x != 255)
		{
			for (effect_t &eff : spcl.effect)
			{
				if (eff.type==EFF_SUMMON && eff.value2>ITM_RANDOM)
				{
					eff.value2=ITM_NONE;
				}
			}
			SaveSpecial(f, &spcl);

			if (--numSpecials == 0)
			{
				break;  // Everything we could write has been written.
			}
		}
	}
}

static void SaveMapData(SDL_IOStream *f, const Map *me)
{
	int pos;
	byte sameLength,diffLength;
	int runStart,sameStart;
	IoTile *m,*src;
	char writeRun;
	IoTile *mapCopy;

	mapCopy=new IoTile[me->width*me->height];

	for(pos=0;pos<me->width*me->height;pos++)
	{
		mapCopy[pos].floor=me->map[pos].floor;
		mapCopy[pos].wall=me->map[pos].wall;
		mapCopy[pos].item=me->map[pos].item;
		mapCopy[pos].light=me->map[pos].light;
	}

	runStart=0;
	sameStart=0;
	sameLength=1;
	diffLength=1;
	pos=1;
	src=&mapCopy[0];

	while(pos<me->width*me->height)
	{
		m=&mapCopy[pos];
		if(!memcmp(m,src,sizeof(IoTile)))	// the tiles are identical
		{
			sameLength++;
			if(sameLength==128)	// max run length is 127
			{
				// write out the run, and start a new run with this last one
				writeRun=-127;
				SDL_WriteIO(f,&writeRun,1);
				SDL_WriteIO(f,src,sizeof(IoTile));
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
			}
			else if(sameLength==2 && runStart!=sameStart)	// 2 in a row the same, time to start this as a same run
			{
				writeRun=sameStart-runStart;
				SDL_WriteIO(f,&writeRun,1);
				SDL_WriteIO(f,&mapCopy[runStart],writeRun*sizeof(IoTile));
				runStart=sameStart;
				diffLength=1;
			}
		}
		else // this one is different
		{
			if(sameLength<2)
			{
				// just continuing the non-same run
				sameLength=1;
				sameStart=pos;
				src=m;
				diffLength++;
				if(diffLength==128)	// max run length is 127
				{
					writeRun=127;
					SDL_WriteIO(f,&writeRun,1);
					SDL_WriteIO(f,&mapCopy[runStart],127*sizeof(IoTile));
					diffLength=1;
					runStart=pos;
				}
			}
			else	// end a same run, then start a diff run
			{
				writeRun=-sameLength;
				SDL_WriteIO(f,&writeRun,1);
				SDL_WriteIO(f,src,sizeof(IoTile));
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
				src=&mapCopy[pos];
			}
		}

		pos++;
	}

	// write out the final leftovers
	if(sameLength>1)
	{
		// final run is same run
		writeRun=-sameLength;
		SDL_WriteIO(f,&writeRun,1);
		SDL_WriteIO(f,src,sizeof(IoTile));
	}
	else
	{
		// either it's one lonely unique tile, or a different run, either way...
		writeRun=diffLength;
		SDL_WriteIO(f,&writeRun,1);
		SDL_WriteIO(f,&mapCopy[runStart],writeRun*sizeof(IoTile));
	}
	delete[] mapCopy;
}

static void SaveMap(SDL_IOStream *f, const Map* me)
{
	SDL_WriteIO(f,&me->width,1);
	SDL_WriteIO(f,&me->height,1);
	SDL_WriteIO(f,me->name,32);
	SDL_WriteIO(f,me->song,32);

	byte count = 0;
	for (const mapBadguy_t &guy : me->badguy)
	{
		if (guy.type > UINT8_MAX)
		{
			LogError(
				"WARNING: in Supreme save, can't save monster '%s' with ID %u > 255",
				MonsterName(guy.type),
				guy.type
			);
		}
		else if (guy.type)
		{
			if (count == UINT8_MAX)
			{
				LogError("Warning: in Supreme save, can't save more than 255 guys");
			}
			++count;
		}
	}

	SDL_WriteIO(f,&count,1);	// num badguys
	for (const mapBadguy_t &guy : me->badguy)
	{
		if (guy.type && guy.type <= UINT8_MAX)
		{
			static_assert(sizeof(guy.x) == 1);
			static_assert(sizeof(guy.y) == 1);
			static_assert(sizeof(guy.item) == 1);

			byte temp = guy.type;
			SDL_WriteIO(f, &guy.x, 1);
			SDL_WriteIO(f, &guy.y, 1);
			SDL_WriteIO(f, &temp, 1);
			SDL_WriteIO(f, &guy.item, 1);

			if (--count == 0)
			{
				break; // Wrote all that we could.
			}
		}
	}

	SaveSpecials(f, std::span{me->special.data(), me->special.size()});

	static_assert(sizeof(me->flags) == 2);
	static_assert(sizeof(me->numBrains) == 2);
	static_assert(sizeof(me->numCandles) == 2);
	static_assert(sizeof(me->itemDrops) == 2);
	SDL_WriteIO(f,&me->flags,sizeof(word));
	SDL_WriteIO(f,&me->numBrains,sizeof(word));
	SDL_WriteIO(f,&me->numCandles,sizeof(word));
	SDL_WriteIO(f,&me->itemDrops,sizeof(word));

	SaveMapData(f, me);
}

static void SaveItems(SDL_IOStream *f)
{
	int i;
	word changedItems;
	byte b;
	word numItems = NumItems();

	changedItems=numItems-NUM_ORIGINAL_ITEMS;
	for(i=0;i<NUM_ORIGINAL_ITEMS;i++)
	{
		// has this item been modified in some way?
		if(memcmp(GetItem(i),GetBaseItem(i),sizeof(item_t)))
			changedItems++;
	}

	SDL_WriteIO(f,&changedItems,sizeof(word));
	for(i=0;i<NUM_ORIGINAL_ITEMS;i++)
	{
		if(memcmp(GetItem(i),GetBaseItem(i),sizeof(item_t)))
		{
			// this item is changed, write it out
			b=(byte)i;
			SDL_WriteIO(f,&b,1);	// write the item number
			item_t item = *GetItem(i);
			item.sound = SoundToDescIndex(item.sound);
			SDL_WriteIO(f,&item,sizeof(item_t));
		}
	}
	if(numItems>NUM_ORIGINAL_ITEMS)
	{
		b=255;	// indicating custom items are beginning here
		SDL_WriteIO(f,&b,1);
		for(i=NUM_ORIGINAL_ITEMS;i<numItems;i++)
		{
			item_t item = *GetItem(i);
			item.sound = SoundToDescIndex(item.sound);
			SDL_WriteIO(f,&item,sizeof(item_t));
		}
	}
}

static void SaveCustomSounds(SDL_IOStream *f)
{
	int numCustom = GetNumCustomSounds();
	SDL_WriteIO(f,&numCustom,sizeof(int));
	for(int i=0;i<numCustom;i++)
	{
		IoSoundDesc ioDesc;
		ioDesc.num = CUSTOM_SND_START + i;
		ham_strcpy(ioDesc.name, GetSoundInfo(CUSTOM_SND_START + i)->name);
		ioDesc.theme = ST_CUSTOM;
		std::span<const byte> sound = GetCustomSound(i);

		uint32_t size = sound.size();
		SDL_WriteIO(f,&ioDesc,sizeof(IoSoundDesc));	// write out the descriptor
		SDL_WriteIO(f,&size,sizeof(int32_t));	// write out the data length
		SDL_WriteIO(f,sound.data(),size);	// write out the data
	}
}

bool Supreme_SaveWorld(const world_t *world, SDL_IOStream *f)
{
	int totalPoints=0;
	for(int i = 1; i < MAX_MAPS; i++)
		if(world->map[i] && (!(world->map[i]->flags&MAP_HUB)))
			totalPoints+=100;	// each level is worth 100 points except hubs which is worth nothing

	char code[9]="SUPREME!";
	SDL_WriteIO(f,code,8);	// identifier code

	SDL_WriteIO(f,&world->author,32);
	SDL_WriteIO(f,&world->map[0]->name,32);
	SDL_WriteIO(f,&world->numMaps,1);
	SDL_WriteIO(f,&totalPoints,sizeof(int));
	SDL_WriteIO(f,&world->numTiles,sizeof(word));

	world->tilegfx.SaveTiles(f);

	for (const terrain_t &terrain : world->Terrain())
	{
		IoTerrain io_terrain = SaveOneTerrain(terrain);
		SDL_WriteIO(f, &io_terrain, sizeof(IoTerrain));
	}

	for (Map *map : world->Maps())
		SaveMap(f, map);

	SaveItems(f);
	SaveCustomSounds(f);

	return true;
}


#define NO_IF(X) if(X) { return false; }
// Checks for if a world *must* be saved as a HamSandwich format world.
// The intent is to prefer the Supreme format when possible, and use the
// newer HamSandwich format only for worlds which absolutely require it.
// It might take a little work, but worlds which do not exceed the limits of
// the relevant integer types should still "fit" in the Supreme format.

// There are some lines checking sizeof() for simplicity. If you change them,
// you can tweak the SaveWorld and LoadWorld functions to save/load the old
// size and tweak those checks.

static bool Supreme_CanSaveMap(const Map *map)
{
	NO_IF(map->width > 250);  // x=255 is special, so we need HSW in that case
	NO_IF(map->height > 250);
	NO_IF(strlen(map->name) > 31);
	NO_IF(strlen(map->song) > 31);

	int i = 0;
	for (const mapBadguy_t &badguy : map->badguy)
	{
		if (badguy.type)
		{
			++i;
			NO_IF(i > UINT8_MAX);
			NO_IF(badguy.x > UINT8_MAX);
			NO_IF(badguy.y > UINT8_MAX);
			NO_IF(badguy.type > UINT8_MAX);
			NO_IF(badguy.item > UINT8_MAX);
		}
	}

	i = 0;
	for (const special_t &spcl : map->special)
	{
		if (spcl.x != 255)
		{
			++i;
			NO_IF(i > UINT8_MAX);
			NO_IF(spcl.x > UINT8_MAX);
			NO_IF(spcl.y > UINT8_MAX);
			NO_IF(spcl.uses > UINT8_MAX);

			int j = 0;
			for (const trigger_t &trg : spcl.trigger)
			{
				++j;
				if (trg.type)
				{
					NO_IF(j > 7);
				}
			}
			j = 0;
			for (const effect_t &eff : spcl.effect)
			{
				++j;
				if (eff.type)
				{
					NO_IF(j > 31);
				}
			}

			// If sizeof changes, break down load, save, and here by field to keep things working.
			static_assert(sizeof(trigger_t) == 12);
			static_assert(sizeof(effect_t) == 44);
		}
	}

	NO_IF(map->flags > UINT16_MAX);
	NO_IF(map->numBrains > UINT16_MAX);
	NO_IF(map->numCandles > UINT16_MAX);
	NO_IF(map->itemDrops > UINT16_MAX);

	for (const mapTile_t& tile : map->Tiles())
	{
		NO_IF(tile.floor > UINT16_MAX);
		NO_IF(tile.wall > UINT16_MAX);
		NO_IF(tile.item > UINT8_MAX);
		NO_IF(tile.light < INT8_MIN || tile.light > INT8_MAX);
	}

	return true;
}

// Returns false if any limits of the `SUPREME!` .dlw format are exceeded.
bool Supreme_CanSaveWorld(const world_t *world)
{
	// Basics
	NO_IF(strlen(world->author) > 31);
	NO_IF(strlen(world->map[0]->name) > 31);
	NO_IF(world->numMaps > UINT8_MAX);
	NO_IF(world->numTiles > UINT16_MAX);

	// No conditions on tilegfx.

	// Terrain
	for (const terrain_t &terrain : world->Terrain())
	{
		NO_IF(terrain.flags >= (1 << 22)); // See comments in TileFlags enum.
		NO_IF(terrain.next > UINT16_MAX);
	}

	// Maps
	for (const Map *map : world->Maps())
	{
		NO_IF(!Supreme_CanSaveMap(map));
	}

	// Items
	NO_IF(NumItems() > UINT8_MAX);
	// If sizeof changes, break down load, save, and here by field to keep things working.
	static_assert(sizeof(item_t) == 124);

	// Custom sounds
	NO_IF(GetNumCustomSounds() > 64); // Classic MAX_CUSTOM_SOUNDS.

	return true;
}
