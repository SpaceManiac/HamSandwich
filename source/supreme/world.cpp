#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "log.h"

byte keyChainInLevel[MAX_MAPS];
static swc_world_t *swcWorld;

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

byte Ham_GetWorldName(const char *fname, char *buffer, char *authbuffer);
byte Ham_LoadWorld(world_t *world, const char *fname);
byte Ham_SaveWorld(world_t *world, const char *fname);

byte LoadWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;
	char code[32];

	f=fopen(fname,"rb");
	if(!f)
		return 0;

	fread(code,sizeof(char),8,f);
	code[8]='\0';

	if(!strcmp(code,"HAMSWCH!"))
	{
		fclose(f);
		return Ham_LoadWorld(world, fname);
	}
	else if(strcmp(code,"SUPREME!"))
	{
		fclose(f);
		ClearCustomSounds();
		return Legacy_LoadWorld(world,fname);
	}
	
	swcWorld=new swc_world_t;

	fread(&world->author,sizeof(char),32,f);
	fread(&code,sizeof(char),32,f);	// name of the world, not needed here
	fread(&world->numMaps,1,1,f);
	fread(&world->totalPoints,1,sizeof(int),f);
	fread(&world->numTiles,1,sizeof(word),f);	// tile count
	SetNumTiles(world->numTiles);

	LoadTiles(f);

	//LogError("SWC sizeof - %i , HAM sizeof - %i", sizeof(swc_terrain_t), sizeof(terrain_t));
	fread(swcWorld->terrain,world->numTiles,sizeof(swc_terrain_t),f);
	
	for(i=0;i<world->numTiles;i++)
	{
		world->terrain[i].flags=(dword)swcWorld->terrain[i].flags;
		world->terrain[i].next=swcWorld->terrain[i].next;
	}

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

	delete swcWorld;
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

	f=fopen(fname,"rb");
	if(!f)
	{
		SetStitchError("File Not Found");
		return 0;
	}

	fread(code,sizeof(char),8,f);
	code[8]='\0';
	if(strcmp(code,"SUPREME!"))
	{
		fclose(f);
		SetStitchError("Must be a Supreme world.");
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

	if(strcmp(code,"SUPREME!"))
		fread(world->terrain,world->numTiles,sizeof(terrain_t),f);
	else
		fread(swcWorld->terrain,swcWorld->numTiles,sizeof(swc_terrain_t),f);

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

byte SaveWorld(world_t *world, const char *fname)
{
	// Save new world
	Ham_SaveWorld(world, fname);
	std::string name = fname;
	
	name.erase(name.end()-3,name.end());
	name.append("dlw");
	fname = name.c_str();

	// Save old world as backup
	FILE *f;
	int i;
	char code[9]="SUPREME!";

	world->map[0]->flags|=MAP_HUB;
	world->totalPoints=0;
	for(i=1;i<MAX_MAPS;i++)
		if(world->map[i] && (!(world->map[i]->flags&MAP_HUB)))
			world->totalPoints+=100;	// each level is worth 100 points except hubs which is worth nothing

	f=fopen(fname,"wb");
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

	return 1;
}

byte GetWorldName(const char *fname,char *buffer,char *authbuffer)
{
	FILE *f;
	char code[9];

	f=fopen(fname,"rb");
	if(!f)
		return 0;

	fread(code,sizeof(char),8,f);
	code[8]='\0';
	if(!strcmp(code,"HAMSWCH!"))
	{
		fclose(f);
		return Ham_GetWorldName(fname, buffer, authbuffer);
	}
	else if(strcmp(code,"SUPREME!"))
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

	f=fopen("req_files.txt","wt");
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
}

terrain_t *GetTerrain(world_t *w,word tile)
{
	static terrain_t tmp={0,0};

	if(tile>799)
		return &tmp;
	else
		return &w->terrain[tile];
}