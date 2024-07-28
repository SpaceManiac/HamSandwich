#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "appdata.h"
#include "string_extras.h"

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
	int i;
	char code[32];

	auto f = AppdataOpen(fname);
	if(!f)
		return 0;

	SDL_RWread(f,code,sizeof(char),8);
	code[8]='\0';
	if(strcmp(code,WORLDCODE))
	{
		return 0;
	}

	SDL_RWread(f,&world->author,sizeof(char),32);
	SDL_RWread(f,&code,sizeof(char),32);	// name of the world, not needed here
	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWread(f,&world->totalPoints,1,sizeof(int));
	SDL_RWread(f,&world->numTiles,1,sizeof(word));	// tile count
	SetNumTiles(world->numTiles);

	LoadTiles(f.get());

	SDL_RWread(f,world->terrain,world->numTiles,sizeof(terrain_t));

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
	if(strcmp(code,WORLDCODE))
	{
		SetStitchError("Must be a Hollow world.");
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

	AppendTiles(stitchTileOffset,f.get());

	SDL_RWread(f,world->terrain,world->numTiles,sizeof(terrain_t));

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(f.get());
		if(!world->map[i])
		{
			f.reset();
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

byte SaveWorld(world_t *world,const char *fname)
{
	int i;
	char code[9]=WORLDCODE;

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

	SDL_RWwrite(f,world->terrain,world->numTiles,sizeof(terrain_t));

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f.get());

	SaveItems(f.get());
	SaveCustomSounds(f.get());

	f.reset();
	AppdataSync();
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

	auto f = AppdataOpen_Write("req_files.txt");
	SDL_RWprintf(f.get(),"World: %s\n",w->map[0]->name);

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
						SDL_RWprintf(f.get(),"user/%s\n",w->map[i]->special[j].effect[k].text);
					}
				}
			}
		}
	}
	f.reset();
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
