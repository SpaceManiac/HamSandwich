#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "log.h"
#include "appdata.h"

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

	f=AssetOpen(fname);
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

	f=AssetOpen(fname);
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

	YES_IF(sizeof(terrain_t) != 4);

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

	FILE *f;
	int i;
	char code[9]="SUPREME!";

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
	SDL_RWops *f;
	char code[9];

	f=AssetOpen_SDL(fname);
	if(!f)
		return 0;

	SDL_RWread(f,code,sizeof(char),8);
	code[8]='\0';
	if(!strcmp(code,"HAMSWCH!"))
	{
		SDL_RWclose(f);
		return Ham_GetWorldName(fname, buffer, authbuffer);
	}
	else if(strcmp(code,"SUPREME!"))
	{
		SDL_RWclose(f);

		strcpy(authbuffer,"Unknown Author");
		return Legacy_GetWorldName(fname,buffer);
	}

	SDL_RWread(f,authbuffer,sizeof(char),32);
	SDL_RWread(f,buffer,sizeof(char),32);

	SDL_RWclose(f);
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
