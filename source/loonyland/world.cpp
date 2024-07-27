#include "world.h"
#include "player.h"
#include "editor.h"
#include "appdata.h"

byte NewWorld(world_t *world,MGLDraw *mgl)
{
	int i;

	world->numMaps=1;
	EditorLoadTiles("graphics/Tiles1.bmp", 0);
	EditorLoadTiles("graphics/tiles2.bmp", 1);
	EditorLoadTiles("graphics/Tiles3.bmp", 2);

	// reset all the terrain
	memset(world->terrain,0,sizeof(terrain_t)*NUMTILES);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	world->map[0]=new Map(25,25,"New World");

	if(!world->map[1])
		return 0;
	return 1;
}

byte LoadWorld(world_t *world,const char *fname)
{
	int i;

	auto f = AppdataOpen(fname);
	if(!f)
		return 0;

	SDL_RWread(f, &world->numMaps,1,1);
	SDL_RWread(f, &world->totalPoints,1,4);

	LoadTiles(f.get());

	SDL_RWread(f, world->terrain,NUMTILES,sizeof(terrain_t));

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f.get());
		if(!world->map[i])
			return 0;
	}

	return 1;
}

byte SaveWorld(world_t *world,const char *fname)
{
	int i;

	world->totalPoints=0;
	for(i=1;i<MAX_MAPS;i++)
		if(world->map[i])
			world->totalPoints+=100;	// each level is worth 100 points except the hub which is worth nothing

	auto f = AppdataOpen_Write(fname);
	if(!f)
		return 0;

	SDL_RWwrite(f, &world->numMaps,1,1);
	SDL_RWwrite(f, &world->totalPoints,1,sizeof(int));

	SaveTiles(f.get());

	SDL_RWwrite(f, world->terrain,NUMTILES,sizeof(terrain_t));

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f.get());

	f.reset();
	AppdataSync();
	return 1;
}

void FreeWorld(world_t *world)
{
	int i;

	for(i=0;i<MAX_MAPS;i++)
		if(world->map[i])
			delete world->map[i];
}

void InitWorld(world_t *world,byte worldNum)
{
}
