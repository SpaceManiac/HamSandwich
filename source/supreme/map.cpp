#include "winpch.h"
#include "map.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "guy.h"
#include "config.h"
#include "log.h"

#define NUM_STARS 400

static world_t *world;

word starX[NUM_STARS],starY[NUM_STARS];
byte starCol[NUM_STARS];
static byte dottedLineOfs;

Map::Map(FILE *f)
{
	byte count;

	fread(&width,1,sizeof(byte),f);
	fread(&height,1,sizeof(byte),f);
	fread(name,32,sizeof(char),f);
	fread(song,32,sizeof(char),f);

	fread(&count,1,sizeof(byte),f);	// num badguys
	memset(badguy,0,sizeof(mapBadguy_t)*MAX_MAPMONS);
	for (byte i = 0; i < count; ++i)
	{
		byte temp;
		fread(&badguy[i].x, 1, sizeof(byte), f);
		fread(&badguy[i].y, 1, sizeof(byte), f);
		fread(&temp, 1, sizeof(byte), f);
		badguy[i].type = temp;
		fread(&badguy[i].item, 1, sizeof(byte), f);
	}

	LoadSpecials(f,special);

	fread(&flags,1,sizeof(word),f);
	fread(&numBrains,1,sizeof(word),f);
	fread(&numCandles,1,sizeof(word),f);
	fread(&itemDrops,1,sizeof(word),f);

	map=(mapTile_t *)malloc(sizeof(mapTile_t)*width*height);

	LoadMapData(f);
}

Map::Map(byte size,const char *name)
{
	int i;

	switch(size)
	{
		case 0:
			width=20;
			height=20;
			break;
		case 1:
			width=64;
			height=64;
			break;
		case 2:
			width=128;
			height=128;
			break;
	}

	strcpy(this->name,name);

	map=(mapTile_t *)calloc(sizeof(mapTile_t)*width*height,1);
	for(i=0;i<width*height;i++)
		map[i].select=1;

	strcpy(song,"");
	flags=0;
	memset(badguy,0,sizeof(mapBadguy_t)*MAX_MAPMONS);
	memset(special,0,sizeof(special_t)*MAX_SPECIAL);
	InitSpecials(special);
	numBrains=0;
	numCandles=0;
	itemDrops=5*FIXAMT;
}

Map::Map(Map *m)
{
	width=m->width;
	height=m->height;
	strcpy(song,m->song);
	flags=m->flags;
	numBrains=m->numBrains;
	numCandles=m->numCandles;
	itemDrops=m->itemDrops;
	strcpy(this->name,m->name);

	map=(mapTile_t *)calloc(sizeof(mapTile_t)*width*height,1);
	memcpy(map,m->map,sizeof(mapTile_t)*width*height);
	memcpy(badguy,m->badguy,sizeof(mapBadguy_t)*MAX_MAPMONS);
	memcpy(special,m->special,sizeof(special_t)*MAX_SPECIAL);
}

Map::~Map(void)
{
	free(map);
}

void Map::SaveMapData(FILE *f)
{
	int pos;
	byte sameLength,diffLength;
	int runStart,sameStart;
	saveTile_t *m,*src;
	char writeRun;
	saveTile_t *mapCopy;

	mapCopy=new saveTile_t[width*height];

	for(pos=0;pos<width*height;pos++)
	{
		mapCopy[pos].floor=map[pos].floor;
		mapCopy[pos].wall=map[pos].wall;
		mapCopy[pos].item=map[pos].item;
		mapCopy[pos].light=map[pos].light;
	}

	runStart=0;
	sameStart=0;
	sameLength=1;
	diffLength=1;
	pos=1;
	src=&mapCopy[0];

	while(pos<width*height)
	{
		m=&mapCopy[pos];
		if(!memcmp(m,src,sizeof(saveTile_t)))	// the tiles are identical
		{
			sameLength++;
			if(sameLength==128)	// max run length is 127
			{
				// write out the run, and start a new run with this last one
				writeRun=-127;
				fwrite(&writeRun,1,sizeof(char),f);
				fwrite(src,1,sizeof(saveTile_t),f);
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
			}
			else if(sameLength==2 && runStart!=sameStart)	// 2 in a row the same, time to start this as a same run
			{
				writeRun=sameStart-runStart;
				fwrite(&writeRun,1,sizeof(char),f);
				fwrite(&mapCopy[runStart],writeRun,sizeof(saveTile_t),f);
				runStart=sameStart;
				diffLength=1;
			}
		}
		else // this one is different
		{
			if(sameLength<2)
			{
				// just continuing the non-same run
				sameLength=1;
				sameStart=pos;
				src=m;
				diffLength++;
				if(diffLength==128)	// max run length is 127
				{
					writeRun=127;
					fwrite(&writeRun,1,sizeof(char),f);
					fwrite(&mapCopy[runStart],127,sizeof(saveTile_t),f);
					diffLength=1;
					runStart=pos;
				}
			}
			else	// end a same run, then start a diff run
			{
				writeRun=-sameLength;
				fwrite(&writeRun,1,sizeof(char),f);
				fwrite(src,1,sizeof(saveTile_t),f);
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
				src=&mapCopy[pos];
			}
		}

		pos++;
	}

	// write out the final leftovers
	if(sameLength>1)
	{
		// final run is same run
		writeRun=-sameLength;
		fwrite(&writeRun,1,sizeof(char),f);
		fwrite(src,1,sizeof(saveTile_t),f);
	}
	else
	{
		// either it's one lonely unique tile, or a different run, either way...
		writeRun=diffLength;
		fwrite(&writeRun,1,sizeof(char),f);
		fwrite(&mapCopy[runStart],writeRun,sizeof(saveTile_t),f);
	}
	delete[] mapCopy;
}

void Map::LoadMapData(FILE *f)
{
	int pos,i;
	char readRun;
	saveTile_t *mapCopy;

	mapCopy=new saveTile_t[width*height];

	pos=0;
	while(pos<width*height)
	{
		fread(&readRun,1,sizeof(char),f);
		if(readRun<0)	// repeat run
		{
			readRun=-readRun;
			fread(&mapCopy[pos],1,sizeof(saveTile_t),f);
			for(i=1;i<readRun;i++)
				memcpy(&mapCopy[pos+i],&mapCopy[pos],sizeof(saveTile_t));
			pos+=readRun;
		}
		else	// unique run
		{
			fread(&mapCopy[pos],readRun,sizeof(saveTile_t),f);
			pos+=readRun;
		}
	}

	for(pos=0;pos<width*height;pos++)
	{
		map[pos].floor=mapCopy[pos].floor;
		map[pos].wall=mapCopy[pos].wall;
		map[pos].item=mapCopy[pos].item;
		map[pos].light=mapCopy[pos].light;
		map[pos].select=1;
		map[pos].opaque=0;
		map[pos].templight=0;
	}
	delete[] mapCopy;
}

byte Map::Save(FILE *f)
{
	int i;
	byte count;

	fwrite(&width,1,sizeof(byte),f);
	fwrite(&height,1,sizeof(byte),f);
	fwrite(name,32,sizeof(char),f);
	fwrite(song,32,sizeof(char),f);

	count=0;
	for(i=0;i<MAX_MAPMONS;i++)
		if(badguy[i].type)
			count=i+1;
	fwrite(&count,1,sizeof(byte),f);	// num badguys
	for (byte i = 0; i < count; ++i)
	{
		byte temp = badguy[i].type;
		if (badguy[i].type > 0xff) {
			LogError("WARNING: in legacy save, can't save monster '%s' with ID %u > 255", MonsterName(badguy[i].type), badguy[i].type);
			temp = 0;
		}
		fwrite(&badguy[i].x, 1, sizeof(byte), f);
		fwrite(&badguy[i].y, 1, sizeof(byte), f);
		fwrite(&temp, 1, sizeof(byte), f);
		fwrite(&badguy[i].item, 1, sizeof(byte), f);
	}
	GetSpecialsFromMap(this->special);
	SaveSpecials(f);

	fwrite(&flags,1,sizeof(word),f);
	fwrite(&numBrains,1,sizeof(word),f);
	fwrite(&numCandles,1,sizeof(word),f);
	fwrite(&itemDrops,1,sizeof(word),f);

	SaveMapData(f);
	return 1;
}

void InitStars()
{
	for (int i=0; i<NUM_STARS; i++)
	{
		starX[i]=Random(SCRWID);
		starY[i]=Random(SCRHEI);
		starCol[i]=(byte)Random(32);
	}
}

void Map::Init(world_t *wrld)
{
	int i;
	InitStars();

	for(i=0;i<width*height;i++)
	{
		if(map[i].item==ITM_KEYCH1 && PlayerKeyChain(0))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_KEYCH2 && PlayerKeyChain(1))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_KEYCH3 && PlayerKeyChain(2))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_KEYCH4 && PlayerKeyChain(3))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_LOONYKEY && PlayerKeyChain(4))	// loonykeys only appear once
			map[i].item=0;
		map[i].templight=-32;	// make it all black so it'll fade in
		map[i].select=1;
	}

	world=wrld;
	// pop in all the badguys
	AddMapGuys(this);

	dottedLineOfs=0;
}

void Map::SmoothLight(int x,int y)
{
	int v,c;

	c=1;
	v=map[x+y*width].light;
	if(x>0)
	{
		c++;
		v+=map[x-1+y*width].light;
	}
	if(x<width-1)
	{
		c++;
		v+=map[x+1+y*width].light;
	}
	if(y>0)
	{
		c++;
		v+=map[x+(y-1)*width].light;
	}
	if(y<height-1)
	{
		c++;
		v+=map[x+(y+1)*width].light;
	}
	v/=c;
	map[x+y*width].light=v;
}

void Map::SmoothLights(void)
{
	int i,j;

	for(i=0;i<width;i++)
		for(j=0;j<height;j++)
			SmoothLight(i,j);
}

void Map::Update(byte mode,world_t *world)
{
	int i;
	static byte timeToReset=0;
	static byte timeToAnim=0;

	dottedLineOfs++;
	if(dottedLineOfs>7)
		dottedLineOfs=0;

	timeToReset++;
	if(timeToReset<2)
		return;

	timeToAnim++;

	timeToReset=0;

	for(i=0;i<width*height;i++)
	{
		map[i].opaque=0;
		if(mode==UPDATE_FADEIN)
		{
			// make the lights approach what they are supposed to be
			if(map[i].templight>map[i].light)
				map[i].templight--;
			else if(map[i].templight<map[i].light-1)
				map[i].templight+=2;
		}
		else if(mode==UPDATE_GAME)
		{
			// make the lights approach what they are supposed to be
			if(player.hammerFlags&HMR_LIGHT)
			{
				if(map[i].templight>0)
					map[i].templight--;
				else if(map[i].templight<0)
					map[i].templight++;
			}
			else
			{
				if(map[i].templight>map[i].light)
					map[i].templight--;
				else if(map[i].templight<map[i].light)
					map[i].templight++;
			}
		}
		else if(mode==UPDATE_EDIT)
		{
			// make the lights right immediately
			map[i].templight=map[i].light;
		}
		else if(mode==UPDATE_FADE)
		{
			// just plain continuously darken
			if(map[i].templight>-32)
				map[i].templight-=2;
		}

		if(mode!=UPDATE_EDIT) // edit mode doesn't animate since it would screw it up
		{
			// check for animation
			if(timeToAnim==2)
			{
				if(GetTerrain(world,map[i].floor)->flags&TF_ANIM)
					map[i].floor=GetTerrain(world,map[i].floor)->next;
				if(map[i].wall!=0 && GetTerrain(world,map[i].wall)->flags&TF_ANIM)
					map[i].wall=GetTerrain(world,map[i].wall)->next;
				if(map[i].item!=ITM_NONE)
					UpdateItem(&map[i],width,i);
			}
		}
	}
	for(i=0;i<width*height;i++)
	{
		if(map[i].opaque==1)	// a movable item wants to move here
		{
			map[i].opaque=0;
			MoveMovableItem(i%width,i/width,this,world);
		}
	}
	if(timeToAnim==2)
		timeToAnim=0;
}

void Map::LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y)
{
	int xdist,ydist;

	xdist=curx-x;
	ydist=cury-y;

	if(xdist<0)
	{
		xdist=-xdist;	// for abs. value needed later
		*p1x=curx+1;
	}
	else if(xdist>0)
		*p1x=curx-1;
	else
		*p1x=curx;

	if(ydist<0)
	{
		ydist=-ydist;	// for abs. value needed later
		*p1y=cury+1;
	}
	else if(ydist>0)
		*p1y=cury-1;
	else
		*p1y=cury;

	if(ydist>xdist)	// need abs. value here
	{
		*p2x=curx;
		*p2y=*p1y;
	}
	else if(ydist<xdist)	// and here
	{
		*p2x=*p1x;
		*p2y=cury;
	}
	else
	{
		*p2x=*p1x;
		*p2y=*p1y;
	}
}

byte Map::LOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *))
{
	int p1x,p1y,p2x,p2y;
	int i,curx,cury;

	if(x<0 || x>=width || y<0 || y>=height)
		return 0;

	// do whatever you are doing at x,y
	if(!DoIt(x,y,x,y,value,this))
		return 1; // DoIt returns zero if it wants you to quit

	map[x+y*width].opaque=0;

	for(i=1;i<radius;i++)	// i is the radius of the square you are working with
	{
		for(cury=y-i;cury<=y+i;cury+=i*2)
			for(curx=x-i;curx<=x+i;curx++)
			{
				// check to be sure point is legal
				if(curx<0 || curx>=width || cury<0 || cury>=height)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);
				if(map[p1x+p1y*width].opaque+
					map[p2x+p2y*width].opaque>=2)
				{
					map[curx+cury*width].opaque=1;
				}
				else
				{
					if(map[curx+cury*width].wall)	// there's a wall here, so opaque
						map[curx+cury*width].opaque=1;
					else
						map[curx+cury*width].opaque=0;
					// do what you have to, it's in sight
					if(!DoIt(curx,cury,x,y,value,this))
						return 1;	// DoIt returns zero if it wants you to quit
				}
			}
		for(curx=x-i;curx<=x+i;curx+=i*2)
			for(cury=y-i+1;cury<=y+i-1;cury++)
			{
				// check to be sure point is legal
				if(curx<0 || curx>=width || cury<0 || cury>=height)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);
				if(map[p1x+p1y*width].opaque+
					map[p2x+p2y*width].opaque>=2)
				{
					map[curx+cury*width].opaque=1;
				}
				else
				{
					if(map[curx+cury*width].wall)	// there's a wall here, so opaque
						map[curx+cury*width].opaque=1;
					else
						map[curx+cury*width].opaque=0;
					// do what you have to, it's in sight
					if(!DoIt(curx,cury,x,y,value,this))
						return 1;
				}
			}
	}
	return 0;
}

byte Map::TightLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *))
{
	int p1x,p1y,p2x,p2y;
	int i,curx,cury;

	if(x<0 || x>=width || y<0 || y>=height)
		return 0;

	// do whatever you are doing at x,y
	if(!DoIt(x,y,x,y,value,this))
		return 1; // DoIt returns zero if it wants you to quit

	map[x+y*width].opaque=0;

	for(i=1;i<radius;i++)	// i is the radius of the square you are working with
	{
		for(cury=y-i;cury<=y+i;cury+=i*2)
			for(curx=x-i;curx<=x+i;curx++)
			{
				// check to be sure point is legal
				if(curx<0 || curx>=width || cury<0 || cury>=height)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);
				if(map[p1x+p1y*width].opaque+
					map[p2x+p2y*width].opaque>=1)
				{
					map[curx+cury*width].opaque=1;
				}
				else
				{
					if(map[curx+cury*width].wall ||
						(GetItem(map[curx+cury*width].item)->flags&IF_BULLETPROOF))	// there's a wall here, so opaque
						map[curx+cury*width].opaque=1;
					else
						map[curx+cury*width].opaque=0;
					// do what you have to, it's in sight
					if(!DoIt(curx,cury,x,y,value,this))
						return 1;	// DoIt returns zero if it wants you to quit
				}
			}
		for(curx=x-i;curx<=x+i;curx+=i*2)
			for(cury=y-i+1;cury<=y+i-1;cury++)
			{
				// check to be sure point is legal
				if(curx<0 || curx>=width || cury<0 || cury>=height)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);
				if(map[p1x+p1y*width].opaque+
					map[p2x+p2y*width].opaque>=1)
				{
					map[curx+cury*width].opaque=1;
				}
				else
				{
					if(map[curx+cury*width].wall ||
						(GetItem(map[curx+cury*width].item)->flags&IF_BULLETPROOF))	// there's a wall here, so opaque
						map[curx+cury*width].opaque=1;
					else
						map[curx+cury*width].opaque=0;
					// do what you have to, it's in sight
					if(!DoIt(curx,cury,x,y,value,this))
						return 1;
				}
			}
	}
	return 0;
}

byte Map::TightestLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *))
{
	int p1x,p1y,p2x,p2y;
	int i,curx,cury;

	if(x<0 || x>=width || y<0 || y>=height)
		return 0;

	// do whatever you are doing at x,y
	if(!DoIt(x,y,x,y,value,this))
		return 1; // DoIt returns zero if it wants you to quit

	map[x+y*width].opaque=0;

	for(i=1;i<radius;i++)	// i is the radius of the square you are working with
	{
		for(cury=y-i;cury<=y+i;cury+=i*2)
			for(curx=x-i;curx<=x+i;curx++)
			{
				// check to be sure point is legal
				if(curx<0 || curx>=width || cury<0 || cury>=height)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);
				if(map[p1x+p1y*width].opaque+
					map[p2x+p2y*width].opaque>=1)
				{
					map[curx+cury*width].opaque=1;
				}
				else
				{
					if(map[curx+cury*width].wall ||
						(GetItem(map[curx+cury*width].item)->flags&(IF_SOLID|IF_BULLETPROOF)))	// there's a wall here, so opaque
						map[curx+cury*width].opaque=1;
					else
						map[curx+cury*width].opaque=0;
					// do what you have to, it's in sight
					if(!DoIt(curx,cury,x,y,value,this))
						return 1;	// DoIt returns zero if it wants you to quit
				}
			}
		for(curx=x-i;curx<=x+i;curx+=i*2)
			for(cury=y-i+1;cury<=y+i-1;cury++)
			{
				// check to be sure point is legal
				if(curx<0 || curx>=width || cury<0 || cury>=height)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);
				if(map[p1x+p1y*width].opaque+
					map[p2x+p2y*width].opaque>=1)
				{
					map[curx+cury*width].opaque=1;
				}
				else
				{
					if(map[curx+cury*width].wall ||
						(GetItem(map[curx+cury*width].item)->flags&(IF_SOLID|IF_BULLETPROOF)))	// there's a wall here, so opaque
						map[curx+cury*width].opaque=1;
					else
						map[curx+cury*width].opaque=0;
					// do what you have to, it's in sight
					if(!DoIt(curx,cury,x,y,value,this))
						return 1;
				}
			}
	}
	return 0;
}

byte PlaceItemCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	if(map->GetTile(x,y)->item)
		return 1;
	if(map->GetTile(x,y)->wall)
		return 1;
	if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))
		return 1;

	map->GetTile(x,y)->item=(byte)value;
	if(value!=ITM_BRAIN && (GetItem(value)->flags&IF_PICKUP))
		MakeSound(SND_ITEMDROP,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,500);
	return 0;	// all done, you placed the item
}

byte FindGuyCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	Guy *g;

	if(GetItem(map->GetTile(x,y)->item)->flags&IF_BULLETPROOF)
		return 1;	// can't see through it

	if(map->GetTile(x,y)->wall)
		return 1;

	g=GetGuy(value);
	if(g && g->mapx==x && g->mapy==y)
		return 0;

	return 1;	// failed
}

byte TorchCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y))/2;
	b=value-b;

	if(b>16)
	{
		b/=4;
	}
	else if(b>=0)
		b=0;

	if(b<0)
		return 1; // not bright enough

	if(map->GetTile(x,y)->light<b)
		map->GetTile(x,y)->light=b;
	if(map->GetTile(x,y)->light>MAX_LIGHT)
		map->GetTile(x,y)->light=MAX_LIGHT;
	return 1;
}

byte DarkTorchCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y))/2;
	b=value+b;

	if(b<=-16)
	{
		b/=4;
	}
	else if(b>=0)
		b=0;

	if(b>0)
		return 1; // not dark enough

	if(map->GetTile(x,y)->light>b)
		map->GetTile(x,y)->light=b;
	if(map->GetTile(x,y)->light<MIN_LIGHT)
		map->GetTile(x,y)->light=MIN_LIGHT;
	return 1;
}

byte TempTorchCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b,desiredLight;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y));
	b=value-b;

	if(b<0)
		return 1;

	if(b<16)
	{
		desiredLight=b-17;
	}
	else if(b<value-4)
		desiredLight=0;
	else
		desiredLight=b-(value-4);

	if(map->GetTile(x,y)->templight<desiredLight-1)
		map->GetTile(x,y)->templight+=2;
	else if(map->GetTile(x,y)->templight==desiredLight-1)
		map->GetTile(x,y)->templight=desiredLight;

	return 1;
}

byte TempDimCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b,desiredLight;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y));
	b=value-b;

	if(b<0)
		return 1;

	if(b<16)
	{
		desiredLight=b-17;
	}
	else if(b<value-4)
		desiredLight=0;
	else
		desiredLight=b-(value-4);

	if(desiredLight>1)
		desiredLight=1;

	if(map->GetTile(x,y)->templight<desiredLight-1)
		map->GetTile(x,y)->templight+=2;
	else if(map->GetTile(x,y)->templight==desiredLight-1)
		map->GetTile(x,y)->templight=desiredLight;

	return 1;
}

byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y))*3;
	b=value-b;

	if(b<0)
		return 1;
	b+=map->GetTile(x,y)->light;
	if(b>MAX_LIGHT)
		b=MAX_LIGHT;

	if(map->GetTile(x,y)->templight<b)
		map->GetTile(x,y)->templight=b;

	return 1;
}

byte TempDarkCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y))*3;
	b=value+b;

	if(b>0)
		return 1;
	b+=map->GetTile(x,y)->light;
	if(b>MAX_LIGHT)
		b=MAX_LIGHT;
	if(b<MIN_LIGHT)
		b=MIN_LIGHT;

	if(map->GetTile(x,y)->templight>b)
		map->GetTile(x,y)->templight=b;
	return 1;
}

byte GlowCursorCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y));
	b=value-b;


	map->GetTile(x,y)->templight=10;

	return 1;
}

byte Map::DropItem(int x,int y,byte itm)
{
	return LOS(x,y,10,itm,PlaceItemCallback);
}

void Map::PermaTorch(int x,int y,char brt)
{
	LOS(x,y,brt/2,brt,TorchCallback);
}

void Map::TempTorch(int x,int y,char brt)
{
	LOS(x,y,brt/4,brt,TempTorchCallback);
}

void Map::BrightTorch(int x,int y,char brt,byte size)
{
	if(brt>=0)
		LOS(x,y,size,brt,TempBrightCallback);
	else
		LOS(x,y,size,brt,TempDarkCallback);
}

void Map::GlowCursor(int x,int y,char brt,byte size)
{
	TightestLOS(x,y,size,brt,GlowCursorCallback);
}

void Map::DimTorch(int x,int y,byte size)
{
	LOS(x,y,size,120,TempDimCallback);
}

void Map::LightSpecial(int x,int y,char brt,byte size)
{
	if(brt>=0)
		LOS(x,y,size,brt,TorchCallback);
	else
		LOS(x,y,size,brt,DarkTorchCallback);
}

byte Map::FindGuy(int x,int y,byte size,Guy *g)
{
	return TightLOS(x,y,size,g->ID,FindGuyCallback);
}

void Map::Swap(int sx,int sy,int blkwidth,int blkheight,int dx,int dy)
{
	mapTile_t *tempMap;
	int i;

	if(sx<0 || sx>=width || sy<0 || sy>=height ||
		(sx+blkwidth)<sx || (sx+blkwidth)>width ||
		(sy+blkheight)<sy || (sy+blkheight)>height ||
		dx<0 || dx>=width || dy<0 || dy>=height ||
		(dx+blkwidth)<dx || (dx+blkwidth)>width ||
		(dy+blkheight)<dy || (dy+blkheight)>height)
		return;	// illegal swap

	tempMap=(mapTile_t *)malloc(blkwidth*blkheight*sizeof(mapTile_t));
	if(!tempMap)
		return;	// can't swap, not enough memory

	// copy destination to temp
	for(i=0;i<blkheight;i++)
	{
		memcpy(&tempMap[i*blkwidth],&map[(i+dy)*width+dx],sizeof(mapTile_t)*blkwidth);
	}

	// copy source to dest
	for(i=0;i<blkheight;i++)
	{
		memcpy(&map[(i+dy)*width+dx],&map[(i+sy)*width+sx],sizeof(mapTile_t)*blkwidth);
	}

	// and copy temp to source
	for(i=0;i<blkheight;i++)
	{
		memcpy(&map[(i+sy)*width+sx],&tempMap[i*blkwidth],sizeof(mapTile_t)*blkwidth);
	}

	free(tempMap);

	// move all specials that are in the target zone
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(special[i].x>=sx && special[i].y>=sy && special[i].x<sx+blkwidth && special[i].y<sy+blkheight)
		{
			special[i].x=special[i].x-sx+dx;
			special[i].y=special[i].y-sy+dy;
			AdjustSpecialCoords(&special[i],-sx+dx,-sy+dy);
		}
		else if(special[i].x>=dx && special[i].y>=dy && special[i].x<dx+blkwidth && special[i].y<dy+blkheight)
		{
			special[i].x=special[i].x+sx-dx;
			special[i].y=special[i].y+sy-dy;
			AdjustSpecialCoords(&special[i],sx-dx,sy-dy);
		}
	}

	// and move all guys in the target
	GuySwap(sx,sy,blkwidth,blkheight,dx,dy);
	// don't forget bullets
	BulletSwap(sx,sy,blkwidth,blkheight,dx,dy);
}

void Map::Copy(int sx,int sy,int blkwidth,int blkheight,int dx,int dy)
{
	int i;

	if(sx<0 || sx>=width || sy<0 || sy>=height ||
		(sx+blkwidth)<sx || (sx+blkwidth)>width ||
		(sy+blkheight)<sy || (sy+blkheight)>height ||
		dx<0 || dx>=width || dy<0 || dy>=height ||
		(dx+blkwidth)<dx || (dx+blkwidth)>width ||
		(dy+blkheight)<dy || (dy+blkheight)>height)
		return;	// illegal coords

	// copy source to dest
	for(i=0;i<blkheight;i++)
	{
		// Has to be memmove instead of memcpy because the source and destination might overlap.
		memmove(&map[(i+dy)*width+dx],&map[(i+sy)*width+sx],sizeof(mapTile_t)*blkwidth);
	}

	// move all specials that are in the target zone
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(special[i].x>=sx && special[i].y>=sy && special[i].x<sx+blkwidth && special[i].y<sy+blkheight)
		{
			special[i].x=special[i].x-sx+dx;
			special[i].y=special[i].y-sy+dy;
		}
	}
}

int Map::ItemCount(byte itm)
{
	int i,cnt;

	cnt=0;

	for(i=0;i<width*height;i++)
		if(map[i].item==itm)
			cnt++;

	return cnt;
}

int Map::ItemCountInRect(byte itm,int x,int y,int x2,int y2)
{
	int i,j,cnt;

	cnt=0;

	for(i=x;i<=x2;i++)
		for(j=y;j<=y2;j++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				if(map[i+j*width].item==itm)
					cnt++;
			}
		}

	return cnt;
}

void Map::RenderStars(int camX, int camY)
{
	int w = GetDisplayMGL()->GetWidth(), h = GetDisplayMGL()->GetHeight();
	for(int i=0; i<NUM_STARS; i++)
	{
		for (int x = starX[i]; x < w; x += SCRWID)
		{
			for (int y = starY[i]; y < h; y += SCRHEI)
			{
				auto dtx = div(x + camX, TILE_WIDTH);
				auto dty = div(y + camY, TILE_HEIGHT);

				if((x+camX)<0 || (y+camY)<0 || dtx.quot>=width || dty.quot>=height)
					continue;

				mapTile_t *m = &map[dtx.quot+dty.quot*width];
				if(m->wall)
					continue;

				PlotStar(x,y,starCol[i],dtx.rem,dty.rem,m->floor);
			}
		}
	}
}

void Map::Render(world_t *world,int camX,int camY,byte flags)
{
	int i,j;

	int tileX,tileY;
	int ofsX,ofsY;
	int scrX,scrY;
	mapTile_t *m;
	char lite;
	char lites[9];
	byte shdw;

	camX -= GetDisplayMGL()->GetWidth()/2;
	camY -= GetDisplayMGL()->GetHeight()/2;

	tileX=(camX/TILE_WIDTH)-1;
	tileY=(camY/TILE_HEIGHT)-1;
	ofsX=camX%TILE_WIDTH;
	ofsY=camY%TILE_HEIGHT;

	scrX=-ofsX-TILE_WIDTH;
	for(i=tileX;i<tileX+(GetDisplayMGL()->GetWidth()/TILE_WIDTH+4);i++)
	{
		scrY=-ofsY-TILE_HEIGHT;
		for(j=tileY;j<tileY+(GetDisplayMGL()->GetHeight()/TILE_HEIGHT+6);j++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				m=&map[i+j*width];

				lite=m->templight;

				lites[4]=lite;
				if(i>0 && j>0)
					lites[0]=map[i-1+(j-1)*width].templight;
				else
				{
					if(i>0)
						lites[0]=map[i-1+(j)*width].templight;
					else if(j>0)
						lites[0]=map[i+(j-1)*width].templight;
					else
						lites[0]=lite;
				}
				if(j>0)
					lites[1]=map[i+(j-1)*width].templight;
				else
					lites[1]=lite;
				if(i<width-1 && j>0)
					lites[2]=map[i+1+(j-1)*width].templight;
				else
				{
					if(i<width-1)
						lites[2]=map[i+1+(j)*width].templight;
					else if(j>0)
						lites[2]=map[i+(j-1)*width].templight;
					else
						lites[2]=lite;
				}
				if(i>0)
					lites[3]=map[i-1+j*width].templight;
				else
					lites[3]=lite;
				if(i<width-1)
					lites[5]=map[i+1+(j)*width].templight;
				else
					lites[5]=lite;
				if(i>0 && j<height-1)
					lites[6]=map[i-1+(j+1)*width].templight;
				else
				{
					if(i>0)
						lites[6]=map[i-1+(j)*width].templight;
					else if(j<height-1)
						lites[6]=map[i+(j+1)*width].templight;
					else
						lites[6]=lite;
				}
				if(j<height-1)
					lites[7]=map[i+(j+1)*width].templight;
				else
					lites[7]=lite;
				if(i<width-1 && j<height-1)
					lites[8]=map[i+1+(j+1)*width].templight;
				else
				{
					if(i<width-1)
						lites[8]=map[i+1+(j)*width].templight;
					else if(j<height-1)
						lites[8]=map[i+(j+1)*width].templight;
					else
						lites[8]=lite;
				}

				RenderItem(scrX+camX+(TILE_WIDTH/2),scrY+camY+(TILE_HEIGHT/2)-1,m->item,lite,flags);

				if(m->wall)	// there is a wall on this tile
				{
					if(j<height-1)
					{
						// if the tile below this one is also a wall, don't waste the
						// time of drawing the front of this wall
						if(map[i+(j+1)*width].wall &&
							(!(GetTerrain(world,map[i+(j+1)*width].floor)->flags&TF_TRANS)))
						{
							if(GetTerrain(world,m->floor)->flags&TF_TRANS)
								RoofDraw(scrX+camX,scrY+camY,m->floor,lites,
										DISPLAY_DRAWME|DISPLAY_ROOFTILE|DISPLAY_TRANSTILE);
							else
								RoofDraw(scrX+camX,scrY+camY,m->floor,lites,DISPLAY_DRAWME|DISPLAY_ROOFTILE);
						}
						else
							if(GetTerrain(world,m->floor)->flags&TF_TRANS)
								WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
							else
								WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE);
					}
					// make wall tiles get drawn in sorted order unlike the floor tiles
					else
					{
						if(GetTerrain(world,m->floor)->flags&TF_TRANS)
							WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
								DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
						else
							WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
								DISPLAY_DRAWME|DISPLAY_WALLTILE);
					}
				}
				else
				{
					// Shadow wall macro: used to determine both a wall is there and it's not marked shadowless
#define SHADOW_WALL(WALL) ((WALL) && !(GetTerrain(world, (WALL))->flags&TF_SHADOWLESS))
					if(config.shading==0)
					{
						if(i<width-1 && SHADOW_WALL(map[i+1+j*width].wall))
							shdw=1;
						else
							shdw=0;
					}
					else
					{
						if(j<height-1 && SHADOW_WALL(map[i+(j+1)*width].wall))
						{
							if(i<width-1 && SHADOW_WALL(map[i+1+j*width].wall))
							{
								shdw=6;
							}
							else
							{
								if(i<width-1 && SHADOW_WALL(map[i+1+(j+1)*width].wall))
									shdw=4;
								else
									shdw=7;
							}
						}
						else
						{
							if(i<width-1)
							{
								if(SHADOW_WALL(map[i+1+(j)*width].wall))
								{
									if(j<height-1 && SHADOW_WALL(map[i+1+(j+1)*width].wall))
										shdw=1;
									else
										shdw=2;
								}
								else
								{
									if(j<height-1 && SHADOW_WALL(map[i+1+(j+1)*width].wall))
										shdw=3;
									else
										shdw=0;
								}
							}
							else
								shdw=0;
						}
					}

					if(j<height-1)
					{
						// if there is a wall on the tile below this one, no
						// point in rendering this floor (unless it is transparent
						if((!map[i+(j+1)*width].wall) ||
							(GetTerrain(world,map[i+(j+1)*width].floor)->flags&TF_TRANS))
							RenderFloorTileFancy(scrX,scrY,m->floor,shdw,lites);
					}
					else
					{
						// if there's a wall to the right, draw a shadow on this tile
						RenderFloorTileFancy(scrX,scrY,m->floor,shdw,lites);
					}
				}
			}
			else
			{
				// put black in empty spaces
				DrawFillBox(scrX,scrY,scrX+TILE_WIDTH-1,scrY+TILE_HEIGHT-1,0);
			}
			scrY+=TILE_HEIGHT;
		}
		scrX+=TILE_WIDTH;
	}

	if(this->flags&MAP_STARS)
	{
		RenderStars(camX, camY);
	}
}

void Map::RenderEdit(world_t *world,int camX,int camY,byte flags)
{
	int i,j;

	int tileX,tileY;
	int ofsX,ofsY;
	int scrX,scrY;
	mapTile_t *m;
	char lite,lites[9];
	byte shdw;

	camX -= GetDisplayMGL()->GetWidth()/2;
	camY -= GetDisplayMGL()->GetHeight()/2;

	tileX=(camX/TILE_WIDTH)-1;
	tileY=(camY/TILE_HEIGHT)-1;
	ofsX=camX%TILE_WIDTH;
	ofsY=camY%TILE_HEIGHT;

	scrX=-ofsX-TILE_WIDTH;
	for(i=tileX;i<tileX+(GetDisplayMGL()->GetWidth()/TILE_WIDTH+4);i++)
	{
		scrY=-ofsY-TILE_HEIGHT;
		for(j=tileY;j<tileY+(GetDisplayMGL()->GetHeight()/TILE_HEIGHT+6);j++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				m=&map[i+j*width];

				if((flags&MAP_SHOWSELECT) && !m->select)
				{
					if(((j%2) && (i%2)) || (!(j%2) && !(i%2)))
						DrawFillBox(scrX,scrY,scrX+TILE_WIDTH-1,scrY+TILE_HEIGHT-1,32*7+12);
					else
						DrawFillBox(scrX,scrY,scrX+TILE_WIDTH-1,scrY+TILE_HEIGHT-1,32*7+8);
				}
				else
				{
					if(!(flags&MAP_SHOWLIGHTS))	// we're ignoring lighting
					{
						memset(lites, 0, 9);
						lite=0;
					}
					else
					{
						lite=m->templight;

						lites[4]=lite;
						if(i>0 && j>0)
							lites[0]=map[i-1+(j-1)*width].templight;
						else
						{
							if(i>0)
								lites[0]=map[i-1+(j)*width].templight;
							else if(j>0)
								lites[0]=map[i+(j-1)*width].templight;
							else
								lites[0]=lite;
						}
						if(j>0)
							lites[1]=map[i+(j-1)*width].templight;
						else
							lites[1]=lite;
						if(i<width-1 && j>0)
							lites[2]=map[i+1+(j-1)*width].templight;
						else
						{
							if(i<width-1)
								lites[2]=map[i+1+(j)*width].templight;
							else if(j>0)
								lites[2]=map[i+(j-1)*width].templight;
							else
								lites[2]=lite;
						}
						if(i>0)
							lites[3]=map[i-1+j*width].templight;
						else
							lites[3]=lite;
						if(i<width-1)
							lites[5]=map[i+1+(j)*width].templight;
						else
							lites[5]=lite;
						if(i>0 && j<height-1)
							lites[6]=map[i-1+(j+1)*width].templight;
						else
						{
							if(i>0)
								lites[6]=map[i-1+(j)*width].templight;
							else if(j<height-1)
								lites[6]=map[i+(j+1)*width].templight;
							else
								lites[6]=lite;
						}
						if(j<height-1)
							lites[7]=map[i+(j+1)*width].templight;
						else
							lites[7]=lite;
						if(i<width-1 && j<height-1)
							lites[8]=map[i+1+(j+1)*width].templight;
						else
						{
							if(i<width-1)
								lites[8]=map[i+1+(j)*width].templight;
							else if(j<height-1)
								lites[8]=map[i+(j+1)*width].templight;
							else
								lites[8]=lite;
						}
					}

					RenderItem(scrX+camX+(TILE_WIDTH/2),scrY+camY+(TILE_HEIGHT/2)-1,
						m->item,lite,flags);

					if(m->wall && (flags&MAP_SHOWWALLS))	// there is a wall on this tile
					{
						if(j<height-1)
						{
							// if the tile below this one is also a wall, don't waste the
							// time of drawing the front of this wall
							if(map[i+(j+1)*width].wall &&
								(!(GetTerrain(world,map[i+(j+1)*width].floor)->flags&TF_TRANS)))
							{
								if(GetTerrain(world,m->floor)->flags&TF_TRANS)
									RoofDraw(scrX+camX,scrY+camY,m->floor,lites,
											DISPLAY_DRAWME|DISPLAY_ROOFTILE|DISPLAY_TRANSTILE);

								else
									RoofDraw(scrX+camX,scrY+camY,m->floor,lites,DISPLAY_DRAWME|DISPLAY_ROOFTILE);
							}
							else
								if(GetTerrain(world,m->floor)->flags&TF_TRANS)
									WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
										DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
								else
									WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
										DISPLAY_DRAWME|DISPLAY_WALLTILE);
						}
						// make wall tiles get drawn in sorted order unlike the floor tiles
						else
						{
							if(GetTerrain(world,m->floor)->flags&TF_TRANS)
								WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
							else
								WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE);
						}
					}
					else
					{
						// Macro is defined in Render above. It's kind of gross I know -SpaceManiac
						if(config.shading==0)
						{
							if(i<width-1 && SHADOW_WALL(map[i+1+j*width].wall))
								shdw=1;
							else
								shdw=0;
						}
						else
						{
							if(j<height-1 && SHADOW_WALL(map[i+(j+1)*width].wall))
							{
								if(i<width-1 && SHADOW_WALL(map[i+1+j*width].wall))
								{
									shdw=6;
								}
								else
								{
									if(i<width-1 && SHADOW_WALL(map[i+1+(j+1)*width].wall))
										shdw=4;
									else
										shdw=7;
								}
							}
							else
							{
								if(i<width-1)
								{
									if(SHADOW_WALL(map[i+1+(j)*width].wall))
									{
										if(j<height-1 && SHADOW_WALL(map[i+1+(j+1)*width].wall))
											shdw=1;
										else
											shdw=2;
									}
									else
									{
										if(j<height-1 && SHADOW_WALL(map[i+1+(j+1)*width].wall))
											shdw=3;
										else
											shdw=0;
									}
								}
								else
									shdw=0;
							}
						}
						if(j<height-1 && (flags&MAP_SHOWWALLS))
						{
							// if there is a wall on the tile below this one, no
							// point in rendering this floor (unless it is transparent
							if((!map[i+(j+1)*width].wall) ||
								(GetTerrain(world,map[i+(j+1)*width].floor)->flags&TF_TRANS))
							{
								RenderFloorTileFancy(scrX,scrY,m->floor,shdw,lites);
							}
						}
						else
						{
							RenderFloorTileFancy(scrX,scrY,m->floor,shdw,lites);
						}
					}
				}
			}
			else
			{
				// put black in empty spaces
				DrawFillBox(scrX,scrY,scrX+TILE_WIDTH-1,scrY+TILE_HEIGHT-1,0);
			}
			scrY+=TILE_HEIGHT;
		}
		scrX+=TILE_WIDTH;
	}

	if(this->flags&MAP_STARS)
	{
		RenderStars(camX, camY);
	}
}

void Map::RenderSelect(world_t *world,int camX,int camY,byte flags)
{
	int i,j;

	int tileX,tileY;
	int ofsX,ofsY;
	int scrX,scrY;
	mapTile_t *m;

	if(flags&MAP_SHOWSELECT)
		return;

	camX -= GetDisplayMGL()->GetWidth() / 2;
	camY -= GetDisplayMGL()->GetHeight() / 2;

	tileX=(camX/TILE_WIDTH)-1;
	tileY=(camY/TILE_HEIGHT)-1;
	ofsX=camX%TILE_WIDTH;
	ofsY=camY%TILE_HEIGHT;

	scrX=-ofsX-TILE_WIDTH;
	for(i=tileX;i<tileX+(GetDisplayMGL()->GetWidth()/TILE_WIDTH+4);i++)
	{
		scrY=-ofsY-TILE_HEIGHT;
		for(j=tileY;j<tileY+(GetDisplayMGL()->GetHeight()/TILE_HEIGHT+6);j++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				m=&map[i+j*width];

				if(m->select)
				{
					if(i==0 || !map[i-1+j*width].select)
						GetDisplayMGL()->SelectLineV(scrX,scrY,scrY+TILE_HEIGHT,dottedLineOfs);
					if(i==width-1 || !map[i+1+j*width].select)
						GetDisplayMGL()->SelectLineV(scrX+TILE_WIDTH-1,scrY,scrY+TILE_HEIGHT,dottedLineOfs);
					if(j==0 || !map[i+(j-1)*width].select)
						GetDisplayMGL()->SelectLineH(scrX,scrX+TILE_WIDTH,scrY,dottedLineOfs);
					if(j==height-1 || !map[i+(j+1)*width].select)
						GetDisplayMGL()->SelectLineH(scrX,scrX+TILE_WIDTH,scrY+TILE_HEIGHT-1,dottedLineOfs);
				}
			}
			scrY+=TILE_HEIGHT;
		}
		scrX+=TILE_WIDTH;
	}
}

byte Map::Resize(byte w,byte h)
{
	mapTile_t *newMap;
	int i,j;

	newMap=(mapTile_t *)malloc(w*h*sizeof(mapTile_t));
	if(!newMap)
		return 0;	// not enough memory to do it

	for(i=0;i<w;i++)
		for(j=0;j<h;j++)
		{
			if(i<width && j<height)
				newMap[i+j*w]=map[i+j*width];
			else
			{
				memset(&newMap[i+j*w],0,sizeof(mapTile_t));
				newMap[i+j*w].select=1;
			}
		}

	free(map);
	map=newMap;
	width=w;
	height=h;
	return 1;
}

byte Map::ItemChange(int x,int y,byte item,byte fx)
{
	byte i;

	if(x<0 || y<0 || x>=width || y>=height)
		return ITM_NONE;

	i=map[x+y*width].item;
	map[x+y*width].item=item;

	if(fx && i!=item)
		SmokeTile(x,y);

	return i;
}

void Map::TileChange(int x,int y,int floor,int wall,byte fx)
{
	word preWall,preFloor;

	if(x<0 || y<0 || x>=width || y>=height)
		return;

	preFloor=map[x+y*width].floor;
	preWall=map[x+y*width].wall;
	map[x+y*width].floor=floor;
	map[x+y*width].wall=wall;

	if(fx && (preFloor!=floor || preWall!=wall))
		SmokeTile(x,y);
}

byte Map::ContiguousItemChange(int x,int y,byte item,byte fx)
{
	byte i;

	if(x<0 || y<0 || x>=width || y>=height)
		return 0;

	i=map[x+y*width].item;

	if(i==item)
		return i;

	if(fx)
		SmokeTile(x,y);

	map[x+y*width].item=item;

	if(x>0 && map[x-1+y*width].item==i)
		ContiguousItemChange(x-1,y,item,fx);
	if(x<width-1 && map[x+1+y*width].item==i)
		ContiguousItemChange(x+1,y,item,fx);
	if(y>0 && map[x+(y-1)*width].item==i)
		ContiguousItemChange(x,y-1,item,fx);
	if(y<height-1 && map[x+(y+1)*width].item==i)
		ContiguousItemChange(x,y+1,item,fx);

	return i;
}

byte Map::AllItemChange(int x,int y,byte item,byte fx)
{
	byte i;
	int j;

	if(x<0 || y<0 || x>=width || y>=height)
		return 0;

	i=map[x+y*width].item;

	if(i==item)
		return i;

	for(j=0;j<width*height;j++)
	{
		if(map[j].item==i)
		{
			map[j].item=item;
			if(fx)
				SmokeTile(j%width,j/width);
		}
	}

	return i;
}

void Map::ContiguousTileChange(int x,int y,int floor,int wall,byte fx)
{
	int preFloor,preWall;

	if(x<0 || y<0 || x>=width || y>=height)
		return;

	preFloor=map[x+y*width].floor;
	preWall=map[x+y*width].wall;

	if(preFloor==floor && preWall==wall)
		return;

	if(fx)
		SmokeTile(x,y);

	map[x+y*width].floor=floor;
	map[x+y*width].wall=wall;

	if(x>0 && map[x-1+y*width].wall==preWall &&
		map[x-1+y*width].floor==preFloor)
		ContiguousTileChange(x-1,y,floor,wall,fx);
	if(x<width-1 && map[x+1+y*width].wall==preWall &&
		map[x+1+y*width].floor==preFloor)
		ContiguousTileChange(x+1,y,floor,wall,fx);
	if(y>0 && map[x+(y-1)*width].wall==preWall &&
		map[x+(y-1)*width].floor==preFloor)
		ContiguousTileChange(x,y-1,floor,wall,fx);
	if(y<height-1 && map[x+(y+1)*width].wall==preWall &&
		map[x+(y+1)*width].floor==preFloor)
		ContiguousTileChange(x,y+1,floor,wall,fx);
}

void Map::AllTileChange(int x,int y,int floor,int wall,byte fx)
{
	int preFloor,preWall;
	int i;

	if(x<0 || y<0 || x>=width || y>=height)
		return;

	preFloor=map[x+y*width].floor;
	preWall=map[x+y*width].wall;

	if(preFloor==floor && preWall==wall)
		return;

	for(i=0;i<width*height;i++)
	{
		if(map[i].floor==preFloor && map[i].wall==preWall)
		{
			map[i].floor=floor;
			map[i].wall=wall;
			if(fx)
				SmokeTile(i%width,i/width);
		}
	}
}

void Map::LightRect(int x,int y,int x2,int y2,char brt,byte perm)
{
	int i,j,pos;

	for(j=y;j<=y2;j++)
	{
		pos=x+j*width;
		for(i=x;i<=x2;i++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				if(perm)
					map[pos++].light=brt;
				else
					map[pos++].templight=brt;
			}
			else
				pos++;
		}
	}
}

byte Map::Keychains(void)
{
	int i,j,k,x;
	byte result=0;
	byte flag[]={1,2,4,8};

	for(i=0;i<width*height;i++)
	{
		// first look for actual items
		if(GetItem(map[i].item)->effect==IE_KEYCHAIN)
		{
			result|=flag[GetItem(map[i].item)->effectAmt];
		}
		if(GetItem(map[i].item)->effect==IE_BECOME)
		{
			j=GetItem(map[i].item)->effectAmt;
			k=0;
			while(GetItem(j)->effect==IE_BECOME)
			{
				k++;
				j=GetItem(j)->effectAmt;
				if(k==100)
					break;
			}
			if(GetItem(j)->effect==IE_KEYCHAIN)
				result|=flag[GetItem(j)->effectAmt];
		}
	}
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(special[i].x!=255)
		{
			for(j=0;j<NUM_EFFECTS;j++)
			{
				if(special[i].effect[j].type==EFF_ITEM)
				{
					if(GetItem(special[i].effect[j].value)->effect==IE_KEYCHAIN)
					{
						result|=flag[GetItem(special[i].effect[j].value)->effectAmt];
					}
					if(GetItem(special[i].effect[j].value)->effect==IE_BECOME)
					{
						k=GetItem(special[i].effect[j].value)->effectAmt;
						x=0;
						while(GetItem(k)->effect==IE_BECOME)
						{
							k=GetItem(k)->effectAmt;
							x++;
							if(x==100)
								break;
						}
						if(GetItem(k)->effect==IE_KEYCHAIN)
							result|=flag[GetItem(k)->effectAmt];
					}
				}
				if(special[i].effect[j].type==EFF_SUMMON)
				{
					if(special[i].effect[j].value2!=ITM_RANDOM)
					{
						if(special[i].effect[j].value2>ITM_RANDOM)
							special[i].effect[j].value2=ITM_NONE;
						if(GetItem(special[i].effect[j].value2)->effect==IE_KEYCHAIN)
						{
							result|=flag[GetItem(special[i].effect[j].value2)->effectAmt];
						}
						if(GetItem(special[i].effect[j].value2)->effect==IE_BECOME)
						{
							k=GetItem(special[i].effect[j].value2)->effectAmt;
							x=0;
							while(GetItem(k)->effect==IE_BECOME)
							{
								k=GetItem(k)->effectAmt;
								x++;
								if(x==100)
									break;
							}
							if(GetItem(k)->effect==IE_KEYCHAIN)
								result|=flag[GetItem(k)->effectAmt];
						}
					}
				}
				if(special[i].effect[j].type==EFF_MONSITEM)
				{
					if(special[i].effect[j].value2!=ITM_RANDOM)
					{
						if(special[i].effect[j].value2>ITM_RANDOM)
							special[i].effect[j].value2=ITM_NONE;
						if(GetItem(special[i].effect[j].value2)->effect==IE_KEYCHAIN)
						{
							result|=flag[GetItem(special[i].effect[j].value2)->effectAmt];
						}
						if(GetItem(special[i].effect[j].value2)->effect==IE_BECOME)
						{
							k=GetItem(special[i].effect[j].value2)->effectAmt;
							x=0;
							while(GetItem(k)->effect==IE_BECOME)
							{
								k=GetItem(k)->effectAmt;
								x++;
								if(x==100)
									break;
							}
							if(GetItem(k)->effect==IE_KEYCHAIN)
								result|=flag[GetItem(k)->effectAmt];
						}
					}
				}
			}
		}
	}
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(badguy[i].type)
		{
			if(badguy[i].item && badguy[i].item<ITM_RANDOM)
			{
				if(GetItem(badguy[i].item)->effect==IE_KEYCHAIN)
				{
					result|=flag[GetItem(badguy[i].item)->effectAmt];
				}
				if(GetItem(badguy[i].item)->effect==IE_BECOME)
				{
					k=GetItem(badguy[i].item)->effectAmt;
					x=0;
					while(GetItem(k)->effect==IE_BECOME)
					{
						k=GetItem(k)->effectAmt;
						x++;
						if(x==100)
							break;
					}
					if(GetItem(k)->effect==IE_KEYCHAIN)
						result|=flag[GetItem(k)->effectAmt];
				}
			}
		}
	}
	return result;
}

void Map::FindNearBrain(int myx,int myy)
{
	int i,j,x,y;

	player.brainX=255;
	i=0;
	j=255*255+255*255;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			if(GetItem(map[i].item)->effect==IE_BRAIN)
			{
				if(GetItem(map[i].item)->effectAmt>0 && ((myx-x)*(myx-x)+(myy-y)*(myy-y))<j)
				{
					player.brainX=x;
					player.brainY=y;
					j=(myx-player.brainX)*(myx-player.brainX)+(myy-player.brainY)*(myy-player.brainY);
				}
			}
			i++;
		}
	}

	FindMonsterBrain(myx,myy);
}

void Map::FindNearCandle(int myx,int myy)
{
	int i,j,x,y;

	player.candleX=255;
	i=0;
	j=255*255+255*255;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			if(GetItem(map[i].item)->effect==IE_CANDLE)
			{
				if(GetItem(map[i].item)->effectAmt>0 && ((myx-x)*(myx-x)+(myy-y)*(myy-y))<j)
				{
					player.candleX=x;
					player.candleY=y;
					j=(myx-player.candleX)*(myx-player.candleX)+(myy-player.candleY)*(myy-player.candleY);
				}
			}
			i++;
		}
	}

	FindMonsterCandle(myx,myy);
}

byte Map::CompareRegions(int x,int y,int x2,int y2,int tx,int ty,byte checkMons)
{
	int i,j;
	mapTile_t *m1,*m2;

	for(i=x;i<=x2;i++)
		for(j=y;j<=y2;j++)
		{
			if(i<0 || i>=width || j<0 || j>=height || (i-x+tx)<0 || (i-x+tx)>=width || (j-y+ty)<0 || (j-y+ty)>=height)
				return 0;	// if either rect somehow goes off-map, no match

			m1=&map[i+j*width];
			m2=&map[(i-x+tx)+(j-y+ty)*width];

			if(m1->floor!=m2->floor || m1->wall!=m2->wall || m1->item!=m2->item || m1->light!=m2->light)
				return 0;
		}

	// if you got here, the maps match, so check badguys
	if(checkMons)
		return BadguyRegions(x,y,x2,y2,tx,ty);
	else
		return 1;
}

mapTile_t *Map::GetTile(int x,int y)
{
	static mapTile_t fake={65535,65535,0,0,0,0,0};

	if(x<0 || y<0 || x>=width || y>=height)
		return &fake;
	else
		return &map[x+y*width];
}

static const char lvlFlagName[][16] = {
	"Snowing",
	"Raining",
	"Hub Level",
	"Secret Level",
	"Torch Lit",
	"Lantern Lit",
	"Star Background",
	"Underwater",
	"Underlava",
	"Stealth",
	"Wavy",
	"Oxygen Meter",
};
static_assert(SDL_arraysize(lvlFlagName) == NUM_LVL_FLAGS, "Must give new level flags a name");

const char* MapFlagName(int flagIndex)
{
	if (flagIndex >= 0 && flagIndex < NUM_LVL_FLAGS)
		return lvlFlagName[flagIndex];
	return "???";
}
