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
	FILE *f;
	int i;
	char hdr[5];

	f=AppdataOpen_Stdio(fname);
	if(!f)
		return 0;

	fread(hdr,4,sizeof(char),f);
	hdr[4]='\0';
	if(strcmp(hdr,"STBY"))
	{
		fclose(f);
		return 0;
	}

	fread(&world->version,1,sizeof(byte),f);
	if(world->version!=WORLD_VERSION)
	{
		fclose(f);
		return 0;
	}

	fread(&world->numMaps,1,1,f);
	fread(&world->tileName,64,sizeof(char),f);
	fread(&world->setname,WORLD_DESC_LINELEN,sizeof(char),f);
	fread(&world->desc,WORLD_DESC_LINELEN*WORLD_DESC_LINES,sizeof(char),f);

	// legacy code, keeps it from crashing to load and save this useless data
	fread(world->terrain,200,sizeof(terrain_t),f);

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

	fclose(f);
	GetWorldTiles(world,mgl);

	return 1;
}

byte SaveWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;
	char hdr[5]="STBY";

	f=AppdataOpen_Write_Stdio(fname);
	if(!f)
		return 0;

	fwrite(hdr,4,sizeof(char),f);
	fwrite(&world->version,1,sizeof(byte),f);

	fwrite(&world->numMaps,1,1,f);
	fwrite(&world->tileName,64,sizeof(char),f);
	fwrite(&world->setname,WORLD_DESC_LINELEN,sizeof(char),f);
	fwrite(&world->desc,WORLD_DESC_LINELEN*WORLD_DESC_LINES,sizeof(char),f);

	// legacy code, keeps it from crashing to load and save this useless data
	fwrite(world->terrain,200,sizeof(terrain_t),f);

	for(i=0;i<world->numMaps;i++)
		world->map[i]->Save(f);

	fclose(f);
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
	FILE *f;
	char fname2[60];

	if(fname[0]=='\0')
		return;

	sprintf(fname2,"levels/%s",fname);
	f=AppdataOpen_Stdio(fname2);
	if(!f)
		return;

	// this fseeks past:
	//	 1 byte=version, 64 bytes=tilesetName, 1 byte numMaps

	fseek(f,4+2+64,SEEK_SET);
	// read the name
	fread(buf,1,32,f);
	buf[31]='\0';
	// read author
	fread(auth,1,32,f);
	auth[31]='\0';

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
	f=AppdataOpen_Stdio(fname2);
	if(!f)
		return 100;

	// skip over the byte
	fread(&i,1,1,f);
	// read the int totalPoints
	fread(&i,1,4,f);
	fclose(f);
	return i;
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
