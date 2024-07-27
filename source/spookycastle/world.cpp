#include "world.h"
#include "player.h"
#include "appdata.h"

byte NewWorld(world_t *world,MGLDraw *mgl)
{
	int i;

	world->numMaps=1;
#ifdef SHAREWARE
	mgl->LoadBMP("graphics/cavernTiles.bmp");
#else
	mgl->LoadBMP("graphics/basicTiles.bmp");
#endif
	SetTiles(mgl->GetScreen());

	// reset all the terrain
	memset(world->terrain,0,sizeof(terrain_t)*200);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	world->map[0]=new Map(0,"New World");

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

	SDL_RWread(f,&world->numMaps,1,1);

	LoadTiles(f.get());

	SDL_RWread(f,world->terrain,200,sizeof(terrain_t));

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f.get());
		if(!world->map[i])
			return 0;
	}

	f.reset();
	return 1;
}

byte SaveWorld(world_t *world,const char *fname)
{
	int i;

	auto f = AppdataOpen_Write(fname);
	if(!f)
		return 0;

	SDL_RWwrite(f,&world->numMaps,1,1);

	SaveTiles(f.get());

	SDL_RWwrite(f,world->terrain,200,sizeof(terrain_t));

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f.get());

	f.reset();
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
	int i;
	int complete;

	complete=0;

	for(i=1;i<MAX_MAPS;i++)
		if(world->map[i])
			complete+=100;		// each level is worth 100 points except the hub which is worth nothing

	PlayerSetWorldWorth(worldNum,complete);
}

void GetWorldName(const char *fname,char *buf)
{
	char fname2[60];

	sprintf(fname2,"worlds/%s",fname);
	auto f = AppdataOpen(fname2);
	if(!f)
		return;

	// this fseeks past:
	//   the byte nummaps, the 400 32x24 tiles,
	//   the 200 terrain types, the width&height of map 0, and bam there it is at the name
	//   of map 0.

	SDL_RWseek(f,1+400*32*24+200*sizeof(terrain_t)+2*sizeof(int),RW_SEEK_SET);
	// read the name
	SDL_RWread(f,buf,1,32);
}
