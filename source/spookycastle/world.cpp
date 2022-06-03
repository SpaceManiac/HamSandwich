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
	FILE *f;
	int i;

	f=AssetOpen(fname);
	if(!f)
		return 0;

	fread(&world->numMaps,1,1,f);

	LoadTiles(f);

	fread(world->terrain,200,sizeof(terrain_t),f);

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

	f=AssetOpen_Write(fname);
	if(!f)
		return 0;

	fwrite(&world->numMaps,1,1,f);

	SaveTiles(f);

	fwrite(world->terrain,200,sizeof(terrain_t),f);

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
	FILE *f;
	char fname2[60];

	sprintf(fname2,"worlds/%s",fname);
	f=AssetOpen(fname2);
	if(!f)
		return;

	// this fseeks past:
	//   the byte nummaps, the 400 32x24 tiles,
	//   the 200 terrain types, the width&height of map 0, and bam there it is at the name
	//   of map 0.

	fseek(f,1+400*32*24+200*sizeof(terrain_t)+2*sizeof(int),SEEK_SET);
	// read the name
	fread(buf,1,32,f);
	fclose(f);
}
