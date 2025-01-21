#include "world.h"
#include "player.h"
#include "appdata.h"

byte NewWorld(world_t *world,MGLDraw *mgl)
{
	int i;

	world->numMaps=1;
	mgl->LoadBMP("addons/tiles1.bmp");
	SetTiles(mgl->GetScreen(),0);
	mgl->LoadBMP("addons/tiles2.bmp");
	SetTiles(mgl->GetScreen(),1);

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

	static_assert(sizeof(terrain_t) == 8);
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

	SDL_RWwrite(f,&world->numMaps,1,1);
	SDL_RWwrite(f,&world->totalPoints,1,sizeof(int));

	SaveTiles(f.get());

	static_assert(sizeof(terrain_t) == 8);
	SDL_RWwrite(f,world->terrain,NUMTILES,sizeof(terrain_t));

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

void GetWorldName(const char *fname,char *buf)
{
	char fname2[60];

	if(fname[0]=='\0')
		return;

	sprintf(fname2,"worlds/%s",fname);
	auto f = AppdataOpen(fname2);
	if(!f)
		return;

	// this fseeks past:
	//   the byte nummaps, the int totalpoints, the 800 32x24 tiles,
	//   the 800 terrain types, the width&height of map 0, and bam there it is at the name
	//   of map 0.

	SDL_RWseek(f,1+sizeof(int)+NUMTILES*32*24+NUMTILES*sizeof(terrain_t)+2*sizeof(int),RW_SEEK_SET);
	// read the name
	SDL_RWread(f,buf,1,32);
}
