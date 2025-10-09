#include "world_io_supreme.h"
#include <SDL_rwops.h>
#include "appdata.h"
#include "world.h"
#include "map.h"
#include "tile.h"
#include "sound.h"
#include "log.h"
#include "monster.h"

bool Supreme_GetWorldName(SDL_RWops *f, StringDestination name, StringDestination author)
{
	char authbuffer[32], namebuffer[32];

	if (SDL_RWseek(f, 8, RW_SEEK_SET) < 0)
		return false;
	if (SDL_RWread(f, authbuffer, 32, 1) < 1)
		return false;
	if (SDL_RWread(f, namebuffer, 32, 1) < 1)
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

void LoadTerrain(world_t *world, const char *fname, SDL_RWops *f)
{
	for (terrain_t &terrain : world->Terrain())
	{
		IoTerrain io_terrain;
		SDL_RWread(f, &io_terrain, sizeof(IoTerrain), 1);
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

static void LoadSpecial(SDL_RWops *f, special_t *s)
{
	byte numTrig,numEff,b;

	memset(s,0,sizeof(special_t));

	static_assert(sizeof(s->x) == 1);
	static_assert(sizeof(s->y) == 1);
	static_assert(sizeof(s->uses) == 1);
	SDL_RWread(f,&s->x,1,sizeof(byte));
	SDL_RWread(f,&s->y,1,sizeof(byte));
	SDL_RWread(f,&s->uses,1,sizeof(byte));

	SDL_RWread(f,&b,1,sizeof(byte));	// read a combined number indicating #trigs & #effs

	numTrig=b%8;
	numEff=b/8;

	static_assert(sizeof(trigger_t) == 12);
	static_assert(sizeof(effect_t) == 44);
	if(numTrig>0)
		SDL_RWread(f,s->trigger,numTrig,sizeof(trigger_t));
	if(numEff>0)
		SDL_RWread(f,s->effect,numEff,sizeof(effect_t));

	for (int i = 0; i < numEff; ++i)
	{
		if (s->effect[i].type == EFF_SOUND)
			s->effect[i].value = DescIndexToSound(s->effect[i].value);
	}
}

static void LoadSpecials(SDL_RWops *f, span<special_t> list)
{
	byte numSpecials;
	SDL_RWread(f, &numSpecials, 1, sizeof(byte));	// num specials

	size_t i = 0;
	for(; i < numSpecials && i < list.size(); ++i)
		LoadSpecial(f, &list[i]);
	// TODO: could blow up here if numSpecials < list.size()
	for(; i < list.size(); ++i)
		list[i].x = 255;
}

static void LoadMapData(SDL_RWops *f, Map *me)
{
	int pos,i;
	int8_t readRun;
	IoTile *mapCopy;

	mapCopy=new IoTile[me->width*me->height];

	pos=0;
	while(pos<me->width*me->height)
	{
		SDL_RWread(f,&readRun,1,sizeof(int8_t));
		if(readRun<0)	// repeat run
		{
			readRun=-readRun;
			SDL_RWread(f,&mapCopy[pos],1,sizeof(IoTile));
			for(i=1;i<readRun;i++)
				memcpy(&mapCopy[pos+i],&mapCopy[pos],sizeof(IoTile));
			pos+=readRun;
		}
		else	// unique run
		{
			SDL_RWread(f,&mapCopy[pos],readRun,sizeof(IoTile));
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

Map *LoadMap(SDL_RWops *f)
{
	byte width, height;
	SDL_RWread(f,&width,1,sizeof(byte));
	SDL_RWread(f,&height,1,sizeof(byte));

	Map *me = new Map(width, height, "");

	static_assert(sizeof(me->name) == 32);
	static_assert(sizeof(me->song) == 32);
	SDL_RWread(f,me->name,32,sizeof(char));
	SDL_RWread(f,me->song,32,sizeof(char));

	byte count;
	SDL_RWread(f,&count,1,sizeof(byte));	// num badguys
	me->badguy.fill({});
	// TODO: blows up if count > MAX_MAPMONS
	for (byte i = 0; i < count; ++i)
	{
		static_assert(sizeof(me->badguy[i].x) == 1);
		static_assert(sizeof(me->badguy[i].y) == 1);
		static_assert(sizeof(me->badguy[i].item) == 1);

		byte temp;
		SDL_RWread(f,&me->badguy[i].x, 1, sizeof(byte));
		SDL_RWread(f,&me->badguy[i].y, 1, sizeof(byte));
		SDL_RWread(f,&temp, 1, sizeof(byte));
		me->badguy[i].type = temp;
		SDL_RWread(f,&me->badguy[i].item, 1, sizeof(byte));
	}

	LoadSpecials(f, me->special);

	static_assert(sizeof(me->flags) == 2);
	static_assert(sizeof(me->numBrains) == 2);
	static_assert(sizeof(me->numCandles) == 2);
	static_assert(sizeof(me->itemDrops) == 2);
	SDL_RWread(f,&me->flags,1,sizeof(word));
	SDL_RWread(f,&me->numBrains,1,sizeof(word));
	SDL_RWread(f,&me->numCandles,1,sizeof(word));
	SDL_RWread(f,&me->itemDrops,1,sizeof(word));

	LoadMapData(f, me);

	return me;
}

static void LoadItems(SDL_RWops *f)
{
	ExitItems();
	InitItems();

	word changedItems;
	SDL_RWread(f,&changedItems,1,sizeof(word));

	int i = 0;
	for (; i < changedItems; ++i)
	{
		byte curItem = 0;
		SDL_RWread(f, &curItem, 1, sizeof(byte));
		if (curItem == 255)
		{
			break;
		}
		SDL_RWread(f, GetItem(curItem), 1, sizeof(item_t));
	}
	for (; i < changedItems; ++i)
	{
		byte curItem = NewItem();
		SDL_RWread(f, GetItem(curItem), 1, sizeof(item_t));
	}
}

static void LoadCustomSounds(SDL_RWops *f)
{
	ClearCustomSounds();

	int numCustom;
	SDL_RWread(f,&numCustom,1,sizeof(int));
	for(int i=0;i<numCustom;i++)
	{
		IoSoundDesc ioDesc;
		int32_t size;
		byte *data;

		SDL_RWread(f,&ioDesc,1,sizeof(IoSoundDesc));
		SDL_RWread(f,&size,sizeof(int32_t),1);
		data=(byte *)malloc(size);
		SDL_RWread(f,data,sizeof(byte),size);

		soundDesc_t *desc = AddCustomSound(data, size);
		desc->num = CUSTOM_SND_START + i; // ioDesc.num always equals this in conforming .dlw files.
		ham_strcpy(desc->name, ioDesc.name);
		desc->theme = ST_CUSTOM; // ioDesc.theme always equals ST_CUSTOM in conforming .dlw files.
		// Exception: rainbow.dlw has some extraneous sounds (with invalid data)
		// which were previously hidden from the editor. Those show now.
	}
}

bool Supreme_LoadWorld(world_t *world, const char *fname, SDL_RWops *f)
{
	char code[9];
	SDL_RWread(f, code, 8, 1);
	code[8] = '\0';
	if (strcmp(code, "SUPREME!"))
		return false;

	SDL_RWread(f,&world->author,sizeof(char),32);
	SDL_RWseek(f, 32, RW_SEEK_CUR);  // name of the world, not needed here
	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWseek(f, sizeof(int), RW_SEEK_CUR);  // totalPoints
	SDL_RWread(f,&world->numTiles,1,sizeof(word));	// tile count
	SetNumTiles(world->numTiles);

	LoadTiles(f);
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

	LoadItems(f);
	LoadCustomSounds(f);
	SetupRandomItems();
	return true;
}

// ----------------------------------------------------------------------------

static void SaveSpecial(SDL_RWops *f, const special_t *s)
{
	byte numTrig,numEff,b;
	int i;

	SDL_RWwrite(f,&s->x,1,sizeof(byte));
	SDL_RWwrite(f,&s->y,1,sizeof(byte));
	SDL_RWwrite(f,&s->uses,1,sizeof(byte));

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
	SDL_RWwrite(f,&b,1,sizeof(byte));	// write a combined number indicating #trigs & #effs

	if(numTrig>0)
		SDL_RWwrite(f,s->trigger,numTrig,sizeof(trigger_t));
	for (int i = 0; i < numEff; ++i)
	{
		effect_t eff = s->effect[i];
		if (eff.type == EFF_SOUND)
			eff.value = SoundToDescIndex(eff.value);
		SDL_RWwrite(f,&eff,sizeof(effect_t),1);
	}
}

static void SaveSpecials(SDL_RWops *f, span<const special_t> list)
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

	SDL_RWwrite(f,&numSpecials,1,sizeof(byte));	// num specials

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

static void SaveMapData(SDL_RWops *f, const Map *me)
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
				SDL_RWwrite(f,&writeRun,1,sizeof(char));
				SDL_RWwrite(f,src,1,sizeof(IoTile));
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
			}
			else if(sameLength==2 && runStart!=sameStart)	// 2 in a row the same, time to start this as a same run
			{
				writeRun=sameStart-runStart;
				SDL_RWwrite(f,&writeRun,1,sizeof(char));
				SDL_RWwrite(f,&mapCopy[runStart],writeRun,sizeof(IoTile));
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
					SDL_RWwrite(f,&writeRun,1,sizeof(char));
					SDL_RWwrite(f,&mapCopy[runStart],127,sizeof(IoTile));
					diffLength=1;
					runStart=pos;
				}
			}
			else	// end a same run, then start a diff run
			{
				writeRun=-sameLength;
				SDL_RWwrite(f,&writeRun,1,sizeof(char));
				SDL_RWwrite(f,src,1,sizeof(IoTile));
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
		SDL_RWwrite(f,&writeRun,1,sizeof(char));
		SDL_RWwrite(f,src,1,sizeof(IoTile));
	}
	else
	{
		// either it's one lonely unique tile, or a different run, either way...
		writeRun=diffLength;
		SDL_RWwrite(f,&writeRun,1,sizeof(char));
		SDL_RWwrite(f,&mapCopy[runStart],writeRun,sizeof(IoTile));
	}
	delete[] mapCopy;
}

static void SaveMap(SDL_RWops *f, const Map* me)
{
	SDL_RWwrite(f,&me->width,1,sizeof(byte));
	SDL_RWwrite(f,&me->height,1,sizeof(byte));
	SDL_RWwrite(f,me->name,32,sizeof(char));
	SDL_RWwrite(f,me->song,32,sizeof(char));

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

	SDL_RWwrite(f,&count,1,sizeof(byte));	// num badguys
	for (const mapBadguy_t &guy : me->badguy)
	{
		if (guy.type && guy.type <= UINT8_MAX)
		{
			static_assert(sizeof(guy.x) == 1);
			static_assert(sizeof(guy.y) == 1);
			static_assert(sizeof(guy.item) == 1);

			byte temp = guy.type;
			SDL_RWwrite(f, &guy.x, 1, sizeof(byte));
			SDL_RWwrite(f, &guy.y, 1, sizeof(byte));
			SDL_RWwrite(f, &temp, 1, sizeof(byte));
			SDL_RWwrite(f, &guy.item, 1, sizeof(byte));

			if (--count == 0)
			{
				break; // Wrote all that we could.
			}
		}
	}

	SaveSpecials(f, span{me->special.data(), me->special.size()});

	static_assert(sizeof(me->flags) == 2);
	static_assert(sizeof(me->numBrains) == 2);
	static_assert(sizeof(me->numCandles) == 2);
	static_assert(sizeof(me->itemDrops) == 2);
	SDL_RWwrite(f,&me->flags,1,sizeof(word));
	SDL_RWwrite(f,&me->numBrains,1,sizeof(word));
	SDL_RWwrite(f,&me->numCandles,1,sizeof(word));
	SDL_RWwrite(f,&me->itemDrops,1,sizeof(word));

	SaveMapData(f, me);
}

static void SaveItems(SDL_RWops *f)
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

	SDL_RWwrite(f,&changedItems,1,sizeof(word));
	for(i=0;i<NUM_ORIGINAL_ITEMS;i++)
	{
		if(memcmp(GetItem(i),GetBaseItem(i),sizeof(item_t)))
		{
			// this item is changed, write it out
			b=(byte)i;
			SDL_RWwrite(f,&b,1,sizeof(byte));	// write the item number
			item_t item = *GetItem(i);
			item.sound = SoundToDescIndex(item.sound);
			SDL_RWwrite(f,&item,1,sizeof(item_t));
		}
	}
	if(numItems>NUM_ORIGINAL_ITEMS)
	{
		b=255;	// indicating custom items are beginning here
		SDL_RWwrite(f,&b,1,sizeof(byte));
		for(i=NUM_ORIGINAL_ITEMS;i<numItems;i++)
		{
			item_t item = *GetItem(i);
			item.sound = SoundToDescIndex(item.sound);
			SDL_RWwrite(f,&item,1,sizeof(item_t));
		}
	}
}

static void SaveCustomSounds(SDL_RWops *f)
{
	int numCustom = GetNumCustomSounds();
	SDL_RWwrite(f,&numCustom,1,sizeof(int));
	for(int i=0;i<numCustom;i++)
	{
		IoSoundDesc ioDesc;
		ioDesc.num = CUSTOM_SND_START + i;
		ham_strcpy(ioDesc.name, GetSoundInfo(CUSTOM_SND_START + i)->name);
		ioDesc.theme = ST_CUSTOM;
		span<const byte> sound = GetCustomSound(i);

		uint32_t size = sound.size();
		SDL_RWwrite(f,&ioDesc,1,sizeof(IoSoundDesc));	// write out the descriptor
		SDL_RWwrite(f,&size,sizeof(int32_t),1);	// write out the data length
		SDL_RWwrite(f,sound.data(),sizeof(byte),size);	// write out the data
	}
}

bool Supreme_SaveWorld(const world_t *world, SDL_RWops *f)
{
	int totalPoints=0;
	for(int i = 1; i < MAX_MAPS; i++)
		if(world->map[i] && (!(world->map[i]->flags&MAP_HUB)))
			totalPoints+=100;	// each level is worth 100 points except hubs which is worth nothing

	char code[9]="SUPREME!";
	SDL_RWwrite(f,code,8,sizeof(char));	// identifier code

	SDL_RWwrite(f,&world->author,sizeof(char),32);
	SDL_RWwrite(f,&world->map[0]->name,sizeof(char),32);
	SDL_RWwrite(f,&world->numMaps,1,1);
	SDL_RWwrite(f,&totalPoints,1,sizeof(int));
	SDL_RWwrite(f,&world->numTiles,1,sizeof(word));

	SaveTiles(f);

	for (const terrain_t &terrain : world->Terrain())
	{
		IoTerrain io_terrain = SaveOneTerrain(terrain);
		SDL_RWwrite(f, &io_terrain, sizeof(IoTerrain), 1);
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
	// If sizeof changes, break down load, save, and here by field to keep things working.
	static_assert(sizeof(soundDesc_t) == 36);

	return true;
}
