#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "log.h"
#include "appdata.h"
#include "world_io_legacy.h"
#include "world_io_supreme.h"
#include "world_io_ham1.h"

byte keyChainInLevel[MAX_MAPS];

byte NewWorld(world_t *world,MGLDraw *mgl)
{
	int i;

	ClearCustomSounds();
	world->numMaps=1;
	mgl->LoadBMP("tilegfx/tiles.bmp");
	world->numTiles=400;
	SetNumTiles(400);
	SetTiles(mgl->GetScreen());

	// reset all the terrain
	memset(world->terrain,0,sizeof(terrain_t)*NUMTILES);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	world->map[0]=new Map(0,"New World");
	strcpy(world->author,"Nobody");

	ExitItems();
	InitItems();
	if(!world->map[0])
		return 0;
	return 1;
}

bool LoadWorld(world_t *world,const char *fname)
{
	char code[32];

	auto f = AppdataOpen(fname);
	if(!f)
		return false;

	SDL_RWread(f, code, sizeof(char), 8);
	SDL_RWseek(f, 0, RW_SEEK_SET);
	code[8]='\0';

	if(!strcmp(code, "HAMSWCH!"))
	{
		f.reset();
		return Ham_LoadWorld(world, fname);
	}
	else if (!strcmp(code, "SUPREME!"))
	{
		return Supreme_LoadWorld(world, fname, f.get());
	}
	else
	{
		ClearCustomSounds();
		return Legacy_LoadWorld(world, f.get());
	}
}

byte BeginAppendWorld(world_t *world,const char *fname)
{
	int i;
	char code[32];

	auto f = AppdataOpen(fname);
	if(!f)
	{
		SetStitchError("File Not Found");
		return 0;
	}

	SDL_RWread(f,code,sizeof(char),8);
	code[8]='\0';
	if(strcmp(code,"SUPREME!"))
	{
		SetStitchError("Must be a Supreme world.");
		return 0;
	}

	SDL_RWread(f,&world->author,sizeof(char),32);
	SDL_RWseek(f, 32, RW_SEEK_CUR);  // name of the world, not needed here
	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWseek(f, sizeof(int), RW_SEEK_CUR);  // totalPoints
	SDL_RWread(f,&world->numTiles,1,sizeof(word));	// tile count

	if(world->numTiles+GetNumTiles()>NUMTILES)
	{
		SetStitchError("Too many tiles!");
		return 0;
	}

	stitchTileOffset=GetNumTiles();
	SetNumTiles(world->numTiles+stitchTileOffset);

	AppendTiles(stitchTileOffset, f.get());
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
			return 0;
		}
	}

	if(!AppendItems(f.get()))
	{
		SetStitchError("Too many custom items!");
		return 0;
	}
	stitchSoundOffset=AppendCustomSounds(f.get());
	if(stitchSoundOffset==-1)
	{
		SetStitchError("Too many custom sounds!");
		return 0;
	}
	SetupRandomItems();
	return 1;
}

#define YES_IF(X) if(X) { return true; }
// Checks for if a world *must* be saved as a HamSandwich format world.
// The intent is to prefer the Supreme format when possible, and use the
// newer HamSandwich format only for worlds which absolutely require it.
// It might take a little work, but worlds which do not exceed the limits of
// the relevant integer types should still "fit" in the Supreme format.

// There are some lines checking sizeof() for simplicity. If you change them,
// you can tweak the SaveWorld and LoadWorld functions to save/load the old
// size and tweak those checks.

static bool MustBeHamSandwichMap(const Map *map)
{
	YES_IF(map->width > 250);  // x=255 is special, so we need HSW in that case
	YES_IF(map->height > 250);
	YES_IF(strlen(map->name) > 31);
	YES_IF(strlen(map->song) > 31);

	int count = 0;
	for (const mapBadguy_t &badguy : map->badguy)
	{
		if (badguy.type)
		{
			++count;
			YES_IF(count > UINT8_MAX);
			YES_IF(badguy.x > UINT8_MAX);
			YES_IF(badguy.y > UINT8_MAX);
			YES_IF(badguy.type > UINT8_MAX);
			YES_IF(badguy.item > UINT8_MAX);
		}
	}

	for (int i = 0; i < MAX_SPECIAL; ++i)
	{
		const special_t& spcl = map->special[i];
		if (spcl.x != 255)
		{
			YES_IF(i > UINT8_MAX);
			YES_IF(spcl.x > UINT8_MAX);
			YES_IF(spcl.y > UINT8_MAX);
			YES_IF(spcl.uses > UINT8_MAX);

			for (int j = 0; j < NUM_TRIGGERS; ++j)
			{
				if (spcl.trigger[j].type)
				{
					YES_IF(j > 7);
				}
			}
			for (int j = 0; j < NUM_EFFECTS; ++j)
			{
				if (spcl.effect[j].type)
				{
					YES_IF(j > 31);
				}
			}

			YES_IF(sizeof(trigger_t) != 12);
			YES_IF(sizeof(effect_t) != 44);
		}
	}

	YES_IF(map->flags > UINT16_MAX);
	YES_IF(map->numBrains > UINT16_MAX);
	YES_IF(map->numCandles > UINT16_MAX);
	YES_IF(map->itemDrops > UINT16_MAX);

	for (const mapTile_t& tile : map->Tiles())
	{
		YES_IF(tile.floor > UINT16_MAX);
		YES_IF(tile.wall > UINT16_MAX);
		YES_IF(tile.item > UINT8_MAX);
		YES_IF(tile.light < INT8_MIN || tile.light > INT8_MAX);
	}

	return false;
}

bool MustBeHamSandwichWorld(const world_t *world)
{
	// If any "legacy" limits are exceeded.
	YES_IF(strlen(world->author) > 31);
	YES_IF(world->numMaps > UINT8_MAX);
	YES_IF(world->numTiles > UINT16_MAX);

	for(int i = 0; i < world->numMaps; ++i)
	{
		YES_IF(MustBeHamSandwichMap(world->map[i]));
	}

	// simply crossing our fingers that there are <65535 custom items...
	YES_IF(sizeof(item_t) != 124);

	YES_IF(sizeof(soundDesc_t) != 36);

	return false;
}
#undef YES_IF

bool SaveWorld(const world_t *world, const char *fname)
{
	world->map[0]->flags|=MAP_HUB;

	std::string namebuf;
	if (MustBeHamSandwichWorld(world))
	{
		// Save HamSandwich world.
		printf("Saving HamSandwich world: %s\n", fname);
		Ham_SaveWorld(world, fname);
		// Save old world as backup.
		namebuf = fname;
		namebuf.append("_old");
		fname = namebuf.c_str();
	}
	printf("Saving Supreme world: %s\n", fname);

	auto f = AppdataOpen_Write(fname);
	if(!f)
		return false;

	Supreme_SaveWorld(world, f.get());

	f.reset();
	AppdataSync();

	return true;
}

bool GetWorldName(const char *fname, StringDestination name, StringDestination author)
{
	char code[9];

	owned::SDL_RWops f = AppdataOpen(fname);
	if(!f)
		return false;

	SDL_RWread(f,code,sizeof(char),8);
	code[8]='\0';

	if(!strcmp(code,"HAMSWCH!"))
	{
		f.reset();
		return Ham_GetWorldName(fname, name, author);
	}
	else if(!strcmp(code,"SUPREME!"))
	{
		return Supreme_GetWorldName(f.get(), name, author);
	}
	else
	{
		author.assign("Unknown Author");
		return Legacy_GetWorldName(f.get(), name);
	}
}

void FreeWorld(world_t *world)
{
	int i;

	ClearCustomSounds();
	for(i=0;i<MAX_MAPS;i++)
		if(world->map[i])
			delete world->map[i];
}

void InitWorld(world_t *world)
{
}

void RepairTileToTile(world_t *w, const SwapTable &table)
{
	int i;

	for(i=0;i<NUMTILES;i++)
		w->terrain[i].next=table.GetSwap(w->terrain[i].next);
}

void LocateKeychains(world_t *w)
{
	int i;

	for(i=0;i<MAX_MAPS;i++)
	{
		if(i>=w->numMaps)
			keyChainInLevel[i]=0;
		else
		{
			keyChainInLevel[i]=w->map[i]->Keychains();
		}
	}
}

terrain_t *GetTerrain(world_t *w,word tile)
{
	if (tile < NUMTILES)
		return &w->terrain[tile];

	static terrain_t fake;
	fake = {{}, 0};
	return &fake;
}
