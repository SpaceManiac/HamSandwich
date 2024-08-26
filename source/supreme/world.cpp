#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "log.h"
#include "appdata.h"
#include "world_io_legacy.h"
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

static void LoadTerrain(world_t *world, const char *fname, SDL_RWops *f)
{
	for (int i = 0; i < world->numTiles; ++i)
	{
		io_terrain_t io_terrain;
		SDL_RWread(f, &io_terrain, sizeof(io_terrain_t), 1);
		world->terrain[i] = LoadOneTerrain(io_terrain);
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
		for (int i = 0; i < world->numTiles; ++i)
		{
			if (world->terrain[i].flags & TF_TRANS)
				world->terrain[i].flags |= TF_SHADOWLESS;
		}
	}
}

byte LoadWorld(world_t *world,const char *fname)
{
	int i;
	char code[32];

	auto f = AppdataOpen(fname);
	if(!f)
		return 0;

	SDL_RWread(f,code,sizeof(char),8);
	code[8]='\0';

	if(!strcmp(code, "HAMSWCH!"))
	{
		f.reset();
		return Ham_LoadWorld(world, fname);
	}
	if(strcmp(code,"SUPREME!"))
	{
		f.reset();
		ClearCustomSounds();
		return Legacy_LoadWorld(world,fname);
	}

	SDL_RWread(f,&world->author,sizeof(char),32);
	SDL_RWread(f,&code,sizeof(char),32);	// name of the world, not needed here
	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWread(f,&world->totalPoints,1,sizeof(int));
	SDL_RWread(f,&world->numTiles,1,sizeof(word));	// tile count
	SetNumTiles(world->numTiles);

	LoadTiles(f.get());
	LoadTerrain(world, fname, f.get());

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f.get());
		if(!world->map[i])
		{
			return 0;
		}
	}

	LoadItems(f.get());
	LoadCustomSounds(f.get());
	SetupRandomItems();
	return 1;
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
	SDL_RWread(f,&code,sizeof(char),32);	// name of the world, not needed here
	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWread(f,&world->totalPoints,1,sizeof(int));
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
		world->map[i]=new Map(f.get());
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
	for (int i = 0; i < MAX_MAPMONS; ++i)
	{
		if (map->badguy[i].type)
		{
			++count;
			YES_IF(count > UINT8_MAX);
			YES_IF(map->badguy[i].x > UINT8_MAX);
			YES_IF(map->badguy[i].y > UINT8_MAX);
			YES_IF(map->badguy[i].type > UINT8_MAX);
			YES_IF(map->badguy[i].item > UINT8_MAX);
		}
	}

	for (int i = 0; i < MAX_SPECIAL; ++i)
	{
		const auto& spcl = map->special[i];
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

	for (int i = 0, max = map->width * map->height; i < max; ++i)
	{
		YES_IF(map->map[i].floor > UINT16_MAX);
		YES_IF(map->map[i].wall > UINT16_MAX);
		YES_IF(map->map[i].item > UINT8_MAX);
		YES_IF(map->map[i].light < INT8_MIN || map->map[i].light > INT8_MAX);
	}

	return false;
}

bool MustBeHamSandwichWorld(const world_t *world)
{
	// If any "legacy" limits are exceeded.
	YES_IF(strlen(world->author) > 31);
	YES_IF(world->numMaps > UINT8_MAX);
	YES_IF(world->numTiles > UINT16_MAX);

	YES_IF(sizeof(io_terrain_t) != 4);

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

byte SaveWorld(world_t *world, const char *fname)
{
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

	int i;
	char code[9]="SUPREME!";

	world->map[0]->flags|=MAP_HUB;
	world->totalPoints=0;
	for(i=1;i<MAX_MAPS;i++)
		if(world->map[i] && (!(world->map[i]->flags&MAP_HUB)))
			world->totalPoints+=100;	// each level is worth 100 points except hubs which is worth nothing

	auto f = AppdataOpen_Write(fname);
	if(!f)
		return 0;

	SDL_RWwrite(f,code,8,sizeof(char));	// identifier code
	SDL_RWwrite(f,&world->author,sizeof(char),32);
	SDL_RWwrite(f,&world->map[0]->name,sizeof(char),32);
	SDL_RWwrite(f,&world->numMaps,1,1);
	SDL_RWwrite(f,&world->totalPoints,1,sizeof(int));
	SDL_RWwrite(f,&world->numTiles,1,sizeof(word));

	SaveTiles(f.get());

	for(i = 0; i < world->numTiles; ++i)
	{
		io_terrain_t io_terrain = SaveOneTerrain(world->terrain[i]);
		SDL_RWwrite(f, &io_terrain, sizeof(io_terrain_t), 1);
	}

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f.get());

	SaveItems(f.get());
	SaveCustomSounds(f.get());

	f.reset();
	AppdataSync();

	return 1;
}

bool GetWorldName(const char *fname,char *buffer,char *authbuffer)
{
	char code[9];

	owned::SDL_RWops f = AppdataOpen(fname);
	if(!f)
		return 0;

	SDL_RWread(f,code,sizeof(char),8);
	code[8]='\0';
	if(!strcmp(code,"HAMSWCH!"))
	{
		f.reset();
		return Ham_GetWorldName(fname, buffer, authbuffer);
	}
	else if(strcmp(code,"SUPREME!"))
	{
		f.reset();

		strcpy(authbuffer,"Unknown Author");
		return Legacy_GetWorldName(fname,buffer);
	}

	SDL_RWread(f,authbuffer,sizeof(char),32);
	SDL_RWread(f,buffer,sizeof(char),32);

	return 1;
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

void RepairTileToTile(world_t *w)
{
	int i;

	for(i=0;i<NUMTILES;i++)
		w->terrain[i].next=GetSwap(w->terrain[i].next);
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
	static terrain_t tmp={0,0};

	if(tile>799)
		return &tmp;
	else
		return &w->terrain[tile];
}
