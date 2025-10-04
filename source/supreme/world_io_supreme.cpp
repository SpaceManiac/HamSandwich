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

struct io_terrain_t
{
	word flags;
	word next;
};

static terrain_t LoadOneTerrain(io_terrain_t io_terrain)
{
	terrain_t r = { io_terrain.flags, static_cast<word>(io_terrain.next & 0x3ff) };
	if (io_terrain.next & (1 << 15))
		r.flags |= TF_SHADOWLESS;
	return r;
}

static io_terrain_t SaveOneTerrain(terrain_t terrain)
{
	io_terrain_t r = { static_cast<word>(terrain.flags & 0xffffff), terrain.next };
	if (terrain.flags & TF_SHADOWLESS)
		r.next |= (1 << 15);
	return r;
}

// SERIALIZED.
struct saveTile_t
{
	word floor;
	word wall;
	byte item;
	char light;
};

// ----------------------------------------------------------------------------

void LoadTerrain(world_t *world, const char *fname, SDL_RWops *f)
{
	for (terrain_t &terrain : world->Terrain())
	{
		io_terrain_t io_terrain;
		SDL_RWread(f, &io_terrain, sizeof(io_terrain_t), 1);
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

	SDL_RWread(f,&s->x,1,sizeof(byte));
	SDL_RWread(f,&s->y,1,sizeof(byte));
	SDL_RWread(f,&s->uses,1,sizeof(byte));

	SDL_RWread(f,&b,1,sizeof(byte));	// read a combined number indicating #trigs & #effs

	numTrig=b%8;
	numEff=b/8;

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
	for(; i < numSpecials && i < list.size(); i++)
		LoadSpecial(f, &list[i]);
	for(; i < list.size(); i++)
		list[i].x = 255;
}

static void LoadMapData(SDL_RWops *f, Map *me)
{
	int pos,i;
	int8_t readRun;
	saveTile_t *mapCopy;

	mapCopy=new saveTile_t[me->width*me->height];

	pos=0;
	while(pos<me->width*me->height)
	{
		SDL_RWread(f,&readRun,1,sizeof(int8_t));
		if(readRun<0)	// repeat run
		{
			readRun=-readRun;
			SDL_RWread(f,&mapCopy[pos],1,sizeof(saveTile_t));
			for(i=1;i<readRun;i++)
				memcpy(&mapCopy[pos+i],&mapCopy[pos],sizeof(saveTile_t));
			pos+=readRun;
		}
		else	// unique run
		{
			SDL_RWread(f,&mapCopy[pos],readRun,sizeof(saveTile_t));
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
	byte count;

	byte width, height;
	SDL_RWread(f,&width,1,sizeof(byte));
	SDL_RWread(f,&height,1,sizeof(byte));

	Map *me = new Map(width, height, "");

	SDL_RWread(f,me->name,32,sizeof(char));
	SDL_RWread(f,me->song,32,sizeof(char));

	SDL_RWread(f,&count,1,sizeof(byte));	// num badguys
	me->badguy.fill({});
	for (byte i = 0; i < count; ++i)
	{
		byte temp;
		SDL_RWread(f,&me->badguy[i].x, 1, sizeof(byte));
		SDL_RWread(f,&me->badguy[i].y, 1, sizeof(byte));
		SDL_RWread(f,&temp, 1, sizeof(byte));
		me->badguy[i].type = temp;
		SDL_RWread(f,&me->badguy[i].item, 1, sizeof(byte));
	}

	LoadSpecials(f, me->special);

	SDL_RWread(f,&me->flags,1,sizeof(word));
	SDL_RWread(f,&me->numBrains,1,sizeof(word));
	SDL_RWread(f,&me->numCandles,1,sizeof(word));
	SDL_RWread(f,&me->itemDrops,1,sizeof(word));

	LoadMapData(f, me);

	return me;
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
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i].x != 255)
		{
			if (numSpecials == UINT8_MAX)
			{
				LogError("WARNING: in Supreme save, can't save more than 255 specials");
				break;
			}
			numSpecials++;
		}
	}

	SDL_RWwrite(f,&numSpecials,1,sizeof(byte));	// num specials

	// Now, write those out.
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i].x != 255)
		{
			special_t copy = list[i];
			for (int j=0;j<NUM_EFFECTS;j++)
			{
				if (copy.effect[j].type==EFF_SUMMON && copy.effect[j].value2>ITM_RANDOM)
				{
					copy.effect[j].value2=ITM_NONE;
				}
			}
			SaveSpecial(f, &copy);

			if (--numSpecials == 0)
				break;  // Everything we could write has been written.
		}
	}
}

static void SaveMapData(SDL_RWops *f, const Map *me)
{
	int pos;
	byte sameLength,diffLength;
	int runStart,sameStart;
	saveTile_t *m,*src;
	char writeRun;
	saveTile_t *mapCopy;

	mapCopy=new saveTile_t[me->width*me->height];

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
		if(!memcmp(m,src,sizeof(saveTile_t)))	// the tiles are identical
		{
			sameLength++;
			if(sameLength==128)	// max run length is 127
			{
				// write out the run, and start a new run with this last one
				writeRun=-127;
				SDL_RWwrite(f,&writeRun,1,sizeof(char));
				SDL_RWwrite(f,src,1,sizeof(saveTile_t));
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
			}
			else if(sameLength==2 && runStart!=sameStart)	// 2 in a row the same, time to start this as a same run
			{
				writeRun=sameStart-runStart;
				SDL_RWwrite(f,&writeRun,1,sizeof(char));
				SDL_RWwrite(f,&mapCopy[runStart],writeRun,sizeof(saveTile_t));
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
					SDL_RWwrite(f,&mapCopy[runStart],127,sizeof(saveTile_t));
					diffLength=1;
					runStart=pos;
				}
			}
			else	// end a same run, then start a diff run
			{
				writeRun=-sameLength;
				SDL_RWwrite(f,&writeRun,1,sizeof(char));
				SDL_RWwrite(f,src,1,sizeof(saveTile_t));
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
		SDL_RWwrite(f,src,1,sizeof(saveTile_t));
	}
	else
	{
		// either it's one lonely unique tile, or a different run, either way...
		writeRun=diffLength;
		SDL_RWwrite(f,&writeRun,1,sizeof(char));
		SDL_RWwrite(f,&mapCopy[runStart],writeRun,sizeof(saveTile_t));
	}
	delete[] mapCopy;
}

static bool SaveMap(SDL_RWops *f, const Map* me)
{
	int i;
	byte count;

	SDL_RWwrite(f,&me->width,1,sizeof(byte));
	SDL_RWwrite(f,&me->height,1,sizeof(byte));
	SDL_RWwrite(f,me->name,32,sizeof(char));
	SDL_RWwrite(f,me->song,32,sizeof(char));

	count=0;
	for(i=0;i<MAX_MAPMONS;i++)
		if(me->badguy[i].type)
			count=i+1;
	SDL_RWwrite(f,&count,1,sizeof(byte));	// num badguys
	for (byte i = 0; i < count; ++i)
	{
		byte temp = me->badguy[i].type;
		if (me->badguy[i].type > UINT8_MAX)
		{
			LogError(
				"WARNING: in Supreme save, can't save monster '%s' with ID %u > 255",
				MonsterName(me->badguy[i].type),
				me->badguy[i].type
			);
			temp = 0;
		}
		SDL_RWwrite(f, &me->badguy[i].x, 1, sizeof(byte));
		SDL_RWwrite(f, &me->badguy[i].y, 1, sizeof(byte));
		SDL_RWwrite(f, &temp, 1, sizeof(byte));
		SDL_RWwrite(f, &me->badguy[i].item, 1, sizeof(byte));
	}
	SaveSpecials(f, span{me->special.data(), me->special.size()});

	SDL_RWwrite(f,&me->flags,1,sizeof(word));
	SDL_RWwrite(f,&me->numBrains,1,sizeof(word));
	SDL_RWwrite(f,&me->numCandles,1,sizeof(word));
	SDL_RWwrite(f,&me->itemDrops,1,sizeof(word));

	SaveMapData(f, me);
	return true;
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
		io_terrain_t io_terrain = SaveOneTerrain(terrain);
		SDL_RWwrite(f, &io_terrain, sizeof(io_terrain_t), 1);
	}

	for (Map *map : world->Maps())
		SaveMap(f, map);

	SaveItems(f);
	SaveCustomSounds(f);

	return true;
}
