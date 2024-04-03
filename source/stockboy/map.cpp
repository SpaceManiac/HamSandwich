#include "map.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "guy.h"
#include "options.h"
#include "floor.h"
#include "goodie.h"

#define SUMMON_TIME	30*5

static world_t *world;
static int summonClock;
static byte goodieX,goodieY,goodieType;

Map::Map(FILE *f)
{
	width=MAP_SIZE;
	height=MAP_SIZE;

	fread(name,sizeof(char),32,f);
	fread(&goal,sizeof(goal_t),1,f);
	fread(&antigoal,sizeof(goal_t),1,f);
	fread(&timer,sizeof(word),1,f);

	fread(&genTime,sizeof(word),1,f);
	fread(map,sizeof(mapTile_t),width*height,f);
}

Map::Map(int wid,int hei,const char *name)
{
	width=wid;
	height=hei;

	strcpy(this->name,name);
	goal.type=GOAL_LIVE;
	antigoal.type=GOAL_NONE;
	timer=0;
	genTime=SUMMON_TIME;

	memset(map,0,width*height*sizeof(mapTile_t));
}

Map::Map(Map *m)
{
	width=m->width;
	height=m->height;
	strcpy(this->name,m->name);
	this->goal=m->goal;
	this->antigoal=m->antigoal;
	this->timer=m->timer;
	this->genTime=m->genTime;

	memcpy(map,m->map,sizeof(mapTile_t)*width*height);
}

Map::~Map(void)
{
}

byte Map::Save(FILE *f)
{
	fwrite(name,sizeof(char),32,f);
	fwrite(&goal,sizeof(goal_t),1,f);
	fwrite(&antigoal,sizeof(goal_t),1,f);
	fwrite(&timer,sizeof(word),1,f);
	fwrite(&genTime,sizeof(word),1,f);
	fwrite(map,sizeof(mapTile_t),width*height,f);
	return 1;
}

void Map::Init(world_t *wrld)
{
	int i;

	goodieX=255;
	for(i=0;i<width*height;i++)
	{
		map[i].light=0;
		map[i].templight=-32;	// make it all black so it'll fade in
		if(map[i].item==ITM_GOODIE)
		{
			goodieX=i%width;
			goodieY=i/width;
			goodieType=map[i].itemInfo;
			map[i].item=ITM_NONE;
		}
	}

	summonClock=genTime;
	world=wrld;
	AddMapGuys(this);
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
	int i,x,y;
	static byte timeToReset=0;
	static byte timeToAnim=0;

	timeToReset++;
	if(timeToReset<2)
		return;

	timeToAnim++;

	timeToReset=0;

	if(mode==UPDATE_GAME)
	{
		summonClock--;
		if(summonClock==0)
			summonClock=genTime;
	}
	x=0;
	y=0;
	for(i=0;i<width*height;i++)
	{
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
			if(map[i].templight>map[i].light)
				map[i].templight--;
			else if(map[i].templight<map[i].light)
				map[i].templight++;

			// animate teleporters
			if(FloorIsTeleporter(map[i].floor))
			{
				map[i].floor+=20;
				if(map[i].floor>=90+20*4)
					map[i].floor-=80;
			}

			// animate doors
			if(map[i].item==ITM_DOOR)
			{
				if(map[i].itemInfo/8>0)
				{
					map[i].itemInfo-=8;
				}
			}
			if(map[i].item==ITM_OPENDOOR)
			{
				if(map[i].itemInfo/8<6)
				{
					map[i].itemInfo+=8;
				}
				else
				{
					map[i].item=ITM_NONE;
				}
			}
			// countdown switches so they can be reused
			if(map[i].item==ITM_SWITCH || map[i].item==ITM_SWITCH2)
			{
				if(map[i].itemInfo>7)
					map[i].itemInfo-=8;
			}
			// and cranks
			if(map[i].item==ITM_CRANK)
			{
				if(map[i].itemInfo>7)
					map[i].itemInfo-=8;
			}

			if(GameType()!=GAME_PESTCONTROL && FloorIsSummoner(map[i].floor))
			{
				if(summonClock==genTime)
					SummonGuy(&map[i],x,y);
			}

			if(FloorIsExit(map[i].floor))
			{
				map[i].floor+=20;
				if(map[i].floor>98+60)
					map[i].floor=98;
			}
			// countdown
			PressurePlateCheck(x,y);
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

		x++;
		if(x==width)
		{
			x=0;
			y++;
		}
	}

	if(timeToAnim==2)
		timeToAnim=0;
}

byte Map::ToggleDoors(byte color)
{
	int i;

	// first check to be sure the doors CAN be toggled -
	// if any of them is blocked in any way, then this will not be allowed to occur
	for(i=0;i<width*height;i++)
	{
		if((map[i].floor==82+color || map[i].floor==82+20+color ||
		    map[i].floor==82+40+color || map[i].floor==82+60+color) &&
			map[i].item!=0 && map[i].item!=ITM_OPENDOOR && map[i].item!=ITM_DOOR)
			return 0;
	}

	for(i=0;i<width*height;i++)
	{
		if(map[i].item==ITM_DOOR && (map[i].itemInfo%8)==color)
		{
			map[i].item=ITM_OPENDOOR;
		}
		else if((map[i].floor==82+color || map[i].floor==82+20+color ||
			    map[i].floor==82+40+color || map[i].floor==82+60+color))
				// don't let it close if something is on it
		{
			if(map[i].item==0)
			{
				map[i].item=ITM_DOOR;
				map[i].itemInfo=color+6*8;
			}
			else
			{
				map[i].item=ITM_DOOR;
				// leave info as it was, so it rises from where it was lowering
			}
		}
	}
	return 1;
}

byte Map::TurnArrows(void)
{
	int i;

	for(i=0;i<width*height;i++)
	{
		if(FloorArrowFacing(map[i].floor)!=NOT_AN_ARROW)
		{
			FloorTurnArrow(&map[i].floor);
			ShowArrow(i%width,i/width,FloorArrowFacing(map[i].floor));
		}
	}
	return 1;
}

void Map::PressurePlateCheck(int x,int y)
{
	mapTile_t *m;
	byte b;

	m=&map[x+y*width];

	b=PressurePlateState(m->floor);
	if(b==NOT_PRESSURE_PLATE)
		return;

	if((m->item && b==PRESSURE_UP) || (!m->item && b==PRESSURE_DOWN))
	{
		if(ToggleDoors((m->floor%20)-12))
		{
			m->floor+=20;
			if(m->floor>=80)
				m->floor-=80;
		}
	}
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
					if(((ItemFlags(map[curx+cury*width].item)&IF_TALL)	&&
						 (!(ItemFlags(map[curx+cury*width].item)&IF_LIGHTOK))))
						// there's a wall or tall item here, so opaque
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
					if(((ItemFlags(map[curx+cury*width].item)&IF_TALL)	&&
						 (!(ItemFlags(map[curx+cury*width].item)&IF_LIGHTOK))))
						// there's a wall or tall item here, so opaque
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
	if(map->map[x+y*map->width].item)
		return 1;

	map->map[x+y*map->width].item=(byte)value;
	return 0;	// all done, you placed the item
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

	if(map->map[x+y*map->width].light<b)
		map->map[x+y*map->width].light=b;
	if(map->map[x+y*map->width].light>MAX_LIGHT)
		map->map[x+y*map->width].light=MAX_LIGHT;
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

	if(map->map[x+y*map->width].templight<desiredLight-1)
		map->map[x+y*map->width].templight+=2;
	else if(map->map[x+y*map->width].templight==desiredLight-1)
		map->map[x+y*map->width].templight=desiredLight;

	return 1;
}

byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y))*3;
	b=value-b;

	if(b<0)
		return 1;
	b+=map->map[x+y*map->width].light;
	if(b>MAX_LIGHT)
		b=MAX_LIGHT;

	if(map->map[x+y*map->width].templight<b-1)
		map->map[x+y*map->width].templight+=2;
	else if(map->map[x+y*map->width].templight==b-1)
		map->map[x+y*map->width].templight=b;

	return 1;
}

byte TempNormalLightCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;
	char brt;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y));

	brt=0;

	if(b>6*6)
	{
		brt=-((b-36)/20);
		if(brt<-8)
			return 1;
	}

	b=map->map[x+y*map->width].light;
	if(b>brt)
		return 1;	// already bright enough here
	else b=brt;

	if(map->map[x+y*map->width].templight<b-1)
		map->map[x+y*map->width].templight+=2;
	else if(map->map[x+y*map->width].templight==b-1)
		map->map[x+y*map->width].templight=b;

	return 1;
}

byte TempDarkCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	int b;

	b=((cx-x)*(cx-x)+(cy-y)*(cy-y));
	b=value+b;

	b+=map->map[x+y*map->width].light;
	if(b<MIN_LIGHT)
		b=MIN_LIGHT;

	if(map->map[x+y*map->width].templight>b+1)
		map->map[x+y*map->width].templight-=2;
	else if(map->map[x+y*map->width].templight==b+1)
		map->map[x+y*map->width].templight=b;

	return 1;
}

byte MapCheckCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	return (x!=(value%1024) || y!=(value/1024));
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

void Map::WeakTorch(int x,int y)
{
	LOS(x,y,10,8,TempNormalLightCallback);
}

void Map::BrightTorch(int x,int y,char brt,byte size)
{
	LOS(x,y,size,brt,TempBrightCallback);
}

void Map::DarkTorch(int x,int y,char brt,byte size)
{
	LOS(x,y,size,brt,TempDarkCallback);
}

void Map::LightSpecial(int x,int y,char brt,byte size)
{
	LOS(x,y,size,brt,TorchCallback);
}

byte Map::CheckLOS(int x,int y,int radius,int x2,int y2)
{
	return LOS(x,y,radius,x2+y2*1024,MapCheckCallback);
}

void Map::ClearStarts(void)
{
	int i;

	for(i=0;i<width*height;i++)
		if(map[i].item==ITM_START || map[i].item==ITM_PLAYER)
			map[i].item=ITM_NONE;
}

void Map::Render(world_t *world,int camX,int camY,byte flags)
{
	int i,j;

	int tileX,tileY;
	int ofsX,ofsY;
	int scrX,scrY;
	mapTile_t *m;
	char lite;

	camX-=320;
	camY-=240;

	tileX=(camX/TILE_WIDTH)-1;
	tileY=(camY/TILE_HEIGHT)-1;
	ofsX=camX%TILE_WIDTH;
	ofsY=camY%TILE_HEIGHT;

	scrX=-ofsX-TILE_WIDTH;
	for(i=tileX;i<tileX+(SCRWID/TILE_WIDTH+4);i++)
	{
		scrY=-ofsY-TILE_HEIGHT;
		for(j=tileY;j<tileY+(SCRHEI/TILE_HEIGHT+6);j++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				m=&map[i+j*width];

				if(!(flags&MAP_SHOWLIGHTS))	// we're ignoring lighting
					lite=0;
				else
					lite=m->templight;

				if(flags&MAP_SHOWITEMS)
					RenderItem(scrX+camX+(TILE_WIDTH/2),scrY+camY+(TILE_HEIGHT/2),0,
						m->item,m->itemInfo,lite);

				RenderFloorTile(scrX,scrY,m->floor,lite);
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
}

void Map::RenderTiny(world_t *world,int x,int y)
{
	int i,j;

	int scrX,scrY;
	mapTile_t *m;

	scrX=x;
	for(i=0;i<20;i++)
	{
		scrY=y;
		for(j=0;j<20;j++)
		{
			if(i>=0 && i<width && j>=0 && j<height)
			{
				m=&map[i+j*width];

				RenderFloorTileSml(scrX,scrY,m->floor);
			}
			else
			{
				// put black in empty spaces
				DrawFillBox(scrX,scrY,scrX+TILE_WIDTH/2-1,scrY+TILE_HEIGHT/2-1,0);
			}
			scrY+=TILE_HEIGHT/2;
		}
		scrX+=TILE_WIDTH/2;
	}
}

byte Map::SelectTeleporter(int srcx,int srcy,int *dstx,int *dsty)
{
	int i,tries;
	byte c;

	// get color
	c=(map[srcx+srcy*width].floor-90)%20;

	tries=1000;

	i=Random(width*height);
	while(tries-->0)
	{
		if(FloorIsTeleporter(map[i].floor) && ((map[i].floor-90)%20)==c && map[i].item==ITM_NONE &&
			i!=srcx+srcy*width)
		{
			// found a target
			*dstx=i%width;
			*dsty=i/width;
			return 1;
		}
		i++;
		if(i>=width*height)
			i=0;
	}

	// if you got here, there are no destinations
	return 0;
}

byte Map::GoodieCheck(int x,int y,byte type)
{
	if(x==goodieX && y==goodieY && type==goodieType/8)
	{
		goodieX=255;
		InitGoodie(x,y);
		return 1;
	}
	return 0;
}

void Map::Parallelize(void)
{
	int i,j,k;
	mapTile_t m,*m2;

	DeleteMonsters();
	for(i=0;i<width/2;i++)
		for(j=0;j<height;j++)
		{
			m=map[i+j*width];
			map[i+j*width]=map[width-1-i+j*width];
			map[width-1-i+j*width]=m;

			m2=&map[i+j*width];
			for(k=0;k<2;k++)
			{
				if(m2->floor==2)	// right arrow
					m2->floor=42;
				else if(m2->floor==42)	// left arrow
					m2->floor=2;
				else if(m2->floor>=160 && m2->floor<=175)
					m2->floor+=40;
				else if(m2->floor>=200 && m2->floor<=215)
					m2->floor-=40;
				else if(m2->floor>=240 && m2->floor<=255)
					m2->floor+=40;
				else if(m2->floor>=280 && m2->floor<=295)
					m2->floor-=40;
				if(m2->item>=ITM_MONS1 && m2->item<=ITM_MONS4)
				{
					if((m2->itemInfo/8)==0)
						m2->itemInfo+=8*2;
					else if((m2->itemInfo/8)==2)
						m2->itemInfo-=8*2;
				}
				else if(m2->item==ITM_SWITCH)
					m2->item=ITM_SWITCH2;
				else if(m2->item==ITM_SWITCH2)
					m2->item=ITM_SWITCH;

				m2=&map[width-1-i+j*width];
			}
		}
	AddMapGuys(this);
}

void Map::Shift(void)
{
	int i,j;

	DeleteMonsters();
	for(i=0;i<width;i++)
		for(j=height-1;j>=0;j--)
		{
			if(j>0)
				map[i+j*width]=map[i+(j-1)*width];
		}
	AddMapGuys(this);
}
