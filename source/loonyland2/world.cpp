#include "world.h"
#include "player.h"

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
	FILE *f;
	int i;

	f=fopen(fname,"rb");
	if(!f)
		return 0;

	fread(&world->numMaps,1,1,f);
	fread(&world->totalPoints,1,4,f);

	LoadTiles(f);

	fread(world->terrain,NUMTILES,sizeof(terrain_t),f);
	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f);
		if(!world->map[i])
			return 0;
	}

	fclose(f);
	return 1;
}

byte SaveWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;

	world->totalPoints=0;
	for(i=1;i<MAX_MAPS;i++)
		if(world->map[i])
			world->totalPoints+=100;	// each level is worth 100 points except the hub which is worth nothing

	f=fopen(fname,"wb");
	if(!f)
		return 0;

	fwrite(&world->numMaps,1,1,f);
	fwrite(&world->totalPoints,1,sizeof(int),f);

	SaveTiles(f);

	fwrite(world->terrain,NUMTILES,sizeof(terrain_t),f);

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f);

	fclose(f);
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
	FILE *f;
	char fname2[60];

	if(fname[0]=='\0')
		return;

	sprintf(fname2,"worlds/%s",fname);
	f=fopen(fname2,"rb");
	if(!f)
		return;

	// this fseeks past:
	//   the byte nummaps, the int totalpoints, the 800 32x24 tiles,
	//   the 800 terrain types, the width&height of map 0, and bam there it is at the name
	//   of map 0.

	fseek(f,1+sizeof(int)+NUMTILES*32*24+NUMTILES*sizeof(terrain_t)+2*sizeof(int),SEEK_SET);
	// read the name
	fread(buf,1,32,f);
	fclose(f);
}

int GetWorldPoints(const char *fname)
{
	FILE *f;
	char fname2[60];
	int i;

	if(fname[0]=='\0')
		return 100;

	sprintf(fname2,"worlds/%s",fname);
	f=fopen(fname2,"rb");
	if(!f)
		return 100;

	// skip over the byte
	fread(&i,1,1,f);
	// read the int totalPoints
	fread(&i,1,4,f);
	fclose(f);
	return i;
}
