#include "world.h"
#include "player.h"
#include "editor.h"
#include "highscore.h"
#include "appdata.h"

byte NewWorld(world_t *world,MGLDraw *mgl)
{
	int i;

	world->version=WORLD_VERSION;
	world->numMaps=1;
	strcpy(world->tileName,"basicTiles.bmp");
	mgl->LoadBMP("graphics/basicTiles.bmp");
	SetTiles(mgl->GetScreen(),0);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	world->map[0]=new Map(MAP_SIZE,MAP_SIZE,"New Level");

	return 1;
}

byte LoadWorld(world_t *world,const char *fname,MGLDraw *mgl)
{
	int i;
	char hdr[5];

	auto f = AppdataOpen(fname);
	if(!f)
		return 0;

	SDL_RWread(f,hdr,4,sizeof(char));
	hdr[4]='\0';
	if(strcmp(hdr,"STBY"))
	{
		return 0;
	}

	SDL_RWread(f,&world->version,1,sizeof(byte));
	if(world->version!=WORLD_VERSION)
	{
		return 0;
	}

	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWread(f,&world->tileName,64,sizeof(char));
	SDL_RWread(f,&world->setname,WORLD_DESC_LINELEN,sizeof(char));
	SDL_RWread(f,&world->desc,WORLD_DESC_LINELEN*WORLD_DESC_LINES,sizeof(char));

	// legacy code, keeps it from crashing to load and save this useless data
	SDL_RWread(f,world->terrain,200,sizeof(terrain_t));

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

	f.reset();
	GetWorldTiles(world,mgl);

	return 1;
}

byte SaveWorld(world_t *world,const char *fname)
{
	int i;
	char hdr[5]="STBY";

	auto f = AppdataOpen_Write(fname);
	if(!f)
		return 0;

	SDL_RWwrite(f,hdr,4,sizeof(char));
	SDL_RWwrite(f,&world->version,1,sizeof(byte));

	SDL_RWwrite(f,&world->numMaps,1,1);
	SDL_RWwrite(f,&world->tileName,64,sizeof(char));
	SDL_RWwrite(f,&world->setname,WORLD_DESC_LINELEN,sizeof(char));
	SDL_RWwrite(f,&world->desc,WORLD_DESC_LINELEN*WORLD_DESC_LINES,sizeof(char));

	// legacy code, keeps it from crashing to load and save this useless data
	SDL_RWwrite(f,world->terrain,200,sizeof(terrain_t));

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f.get());

	f.reset();
	AppdataSync();
	return 1;
}

byte GetWorldTiles(world_t *world,MGLDraw *mgl)
{
	char name[128];

	sprintf(name,"graphics/%s",world->tileName);
	if(mgl->LoadBMP(name, nullptr))
	{
		SetTiles(mgl->GetScreen(),0);
		return 1;
	}
	return 0;
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

void GetWorldName(const char *fname,char *buf,char *auth)
{
	char fname2[60];

	if(fname[0]=='\0')
		return;

	sprintf(fname2,"levels/%s",fname);
	auto f = AppdataOpen(fname2);
	if(!f)
		return;

	// this fseeks past:
	//	 4 bytes='STBY', 1 byte=version, 1 byte numMaps, 64 bytes=tilesetName

	SDL_RWseek(f,4+1+1+64,RW_SEEK_SET);
	// read the name
	SDL_RWread(f,buf,1,32);
	buf[31]='\0';
	// read author
	SDL_RWread(f,auth,1,32);
	auth[31]='\0';
}

void MoveLevel(world_t *world,byte num,char d)
{
	Map *tmp;
	int other;

	if(world->numMaps==1)
		return;

	tmp=world->map[num];

	if(num+d<0)
		other=world->numMaps-1;
	else if(num+d>=world->numMaps)
		other=0;
	else
		other=num+d;

	world->map[num]=world->map[other];
	world->map[other]=tmp;

	SwapGoldScores(num,other);
	EditorSelectMap(other);
}

byte WorldNewLevel(world_t *world,Map *copyMe)
{
	if(world->numMaps==MAX_MAPS)
		return 0;

	if(copyMe==NULL)
		world->map[world->numMaps]=new Map(MAP_SIZE,MAP_SIZE,"New Level");
	else
		world->map[world->numMaps]=new Map(copyMe);

	world->numMaps++;
	return 1;
}

void WorldDeleteLevel(world_t *world,byte num)
{
	int i;

	if(world->numMaps==1)
	{
		delete world->map[0];
		world->map[0]=new Map(MAP_SIZE,MAP_SIZE,"New Level");
	}
	else
	{
		if(num>=world->numMaps)
			num=world->numMaps-1;
		delete world->map[num];
		for(i=num;i<world->numMaps-1;i++)
		{
			world->map[i]=world->map[i+1];
			SwapGoldScores(i,i+1);
		}
		world->map[world->numMaps-1]=NULL;
		world->numMaps--;
	}
}
