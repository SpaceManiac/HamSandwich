#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "appdata.h"

#define WORLDCODE "SHOLLOW!"

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

byte LoadWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;
	char code[32];

	f=AssetOpen(fname);
	if(!f)
		return 0;

	fread(code,sizeof(char),8,f);
	code[8]='\0';
	if(strcmp(code,WORLDCODE))
	{
		fclose(f);
		return 0;
	}

	fread(&world->author,sizeof(char),32,f);
	fread(&code,sizeof(char),32,f);	// name of the world, not needed here
	fread(&world->numMaps,1,1,f);
	fread(&world->totalPoints,1,sizeof(int),f);
	fread(&world->numTiles,1,sizeof(word),f);	// tile count
	SetNumTiles(world->numTiles);

	LoadTiles(f);

	fread(world->terrain,world->numTiles,sizeof(terrain_t),f);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f);
		if(!world->map[i])
		{
			fclose(f);
			return 0;
		}
	}

	LoadItems(f);
	LoadCustomSounds(f);
	SetupRandomItems();
	fclose(f);
	return 1;
}

byte BeginAppendWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;
	char code[32];

	f=AssetOpen(fname);
	if(!f)
	{
		SetStitchError("File Not Found");
		return 0;
	}

	fread(code,sizeof(char),8,f);
	code[8]='\0';
	if(strcmp(code,WORLDCODE))
	{
		fclose(f);
		SetStitchError("Must be a Hollow world.");
		return 0;
	}

	fread(&world->author,sizeof(char),32,f);
	fread(&code,sizeof(char),32,f);	// name of the world, not needed here
	fread(&world->numMaps,1,1,f);
	fread(&world->totalPoints,1,sizeof(int),f);
	fread(&world->numTiles,1,sizeof(word),f);	// tile count

	if(world->numTiles+GetNumTiles()>NUMTILES)
	{
		fclose(f);
		SetStitchError("Too many tiles!");
		return 0;
	}

	stitchTileOffset=GetNumTiles();
	SetNumTiles(world->numTiles+stitchTileOffset);

	AppendTiles(stitchTileOffset,f);

	fread(world->terrain,world->numTiles,sizeof(terrain_t),f);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f);
		if(!world->map[i])
		{
			fclose(f);
			SetStitchError("Unable to load a level.");
			for(int j=0;j<i;j++)
				delete world->map[j];
			return 0;
		}
	}

	if(!AppendItems(f))
	{
		fclose(f);
		SetStitchError("Too many custom items!");
		return 0;
	}
	stitchSoundOffset=AppendCustomSounds(f);
	if(stitchSoundOffset==-1)
	{
		fclose(f);
		SetStitchError("Too many custom sounds!");
		return 0;
	}
	SetupRandomItems();
	fclose(f);
	return 1;
}

byte SaveWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;
	char code[9]=WORLDCODE;

	world->map[0]->flags|=MAP_HUB;
	world->totalPoints=0;
	for(i=1;i<MAX_MAPS;i++)
		if(world->map[i] && (!(world->map[i]->flags&MAP_HUB)))
			world->totalPoints+=100;	// each level is worth 100 points except hubs which is worth nothing

	f=AssetOpen_Write(fname);
	if(!f)
		return 0;

	fwrite(code,8,sizeof(char),f);	// identifier code
	fwrite(&world->author,sizeof(char),32,f);
	fwrite(&world->map[0]->name,sizeof(char),32,f);
	fwrite(&world->numMaps,1,1,f);
	fwrite(&world->totalPoints,1,sizeof(int),f);
	fwrite(&world->numTiles,1,sizeof(word),f);

	SaveTiles(f);

	fwrite(world->terrain,world->numTiles,sizeof(terrain_t),f);

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f);

	SaveItems(f);
	SaveCustomSounds(f);

	fclose(f);
	AppdataSync();

	return 1;
}

byte GetWorldName(const char *fname,char *buffer,char *authbuffer)
{
	FILE *f;
	char code[9];

	f=AssetOpen(fname);
	if(!f)
		return 0;

	fread(code,sizeof(char),8,f);
	code[8]='\0';
	if(strcmp(code,WORLDCODE))
	{
		fclose(f);

		strcpy(authbuffer,"Unknown Author");
		return Legacy_GetWorldName(fname,buffer);
	}

	fread(authbuffer,sizeof(char),32,f);
	fread(buffer,sizeof(char),32,f);

	fclose(f);
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

void LogRequirements(world_t *w)
{
	int i,j,k;
	FILE *f;

	f=AppdataOpen_Write("req_files.txt");
	fprintf(f,"World: %s\n",w->map[0]->name);

	for(i=0;i<w->numMaps;i++)
	{
		for(j=0;j<MAX_SPECIAL;j++)
		{
			if(w->map[i]->special[j].x!=255)
			{
				for(k=0;k<NUM_EFFECTS;k++)
				{
					byte type = w->map[i]->special[j].effect[k].type;
					if(type==EFF_PICTURE || type==EFF_MONSGRAPHICS || type==EFF_ITEMGRAPHICS)
					{
						fprintf(f,"user/%s\n",w->map[i]->special[j].effect[k].text);
					}
				}
			}
		}
	}
	fclose(f);
	AppdataSync();
}

terrain_t *GetTerrain(world_t *w,word tile)
{
	static terrain_t tmp={0,0};

	if(tile>799)
		return &tmp;
	else
		return &w->terrain[tile];
}
