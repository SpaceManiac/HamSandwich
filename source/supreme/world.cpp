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
	world->tilegfx.numTiles = world->numTiles;
	world->tilegfx.SetTiles(mgl->GetScreen(), mgl->GetWidth(), mgl->GetHeight());

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

bool SaveWorld(const world_t *world, const char *fname)
{
	world->map[0]->flags|=MAP_HUB;

	std::string namebuf;
	if (!Supreme_CanSaveWorld(world))
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
	SetCurrentTilegfx(&world->tilegfx);
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
