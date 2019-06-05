#include "map.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "guy.h"
#include "options.h"
#include "quest.h"
#include "badge.h"

#define NUM_STARS 400

int totalBrains;
static world_t *world;
byte specialClock[MAX_SPECIAL];
byte allSpecialClock;

word starX[NUM_STARS],starY[NUM_STARS];
byte starCol[NUM_STARS];

Map::Map(FILE *f)
{
	fread(&width,1,sizeof(int),f);
	fread(&height,1,sizeof(int),f);

	fread(name,32,sizeof(char),f);
	fread(badguy,MAX_MAPMONS,sizeof(mapBadguy_t),f);
	//memset(special,0,sizeof(special_t)*MAX_SPECIAL);
	fread(special,MAX_SPECIAL,sizeof(special_t),f);
	fread(&song,1,1,f);
	fread(&flags,1,1,f);

	map=(mapTile_t *)calloc(sizeof(mapTile_t)*width*height,1);

	fread(map,width*height,sizeof(mapTile_t),f);
}

Map::Map(int wid,int hei,const char *name)
{
	width=wid;
	height=hei;

	strcpy(this->name,name);

	map=(mapTile_t *)calloc(sizeof(mapTile_t)*width*height,1);
	song=2;
	flags=0;
	memset(badguy,0,sizeof(mapBadguy_t)*MAX_MAPMONS);
	memset(special,0,sizeof(special_t)*MAX_SPECIAL);
}

Map::Map(Map *m)
{
	width=m->width;
	height=m->height;
	song=m->song;
	flags=m->flags;
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

byte Map::Save(FILE *f)
{
	fwrite(&width,1,sizeof(int),f);
	fwrite(&height,1,sizeof(int),f);
	fwrite(name,32,sizeof(char),f);
	fwrite(badguy,MAX_MAPMONS,sizeof(mapBadguy_t),f);
	fwrite(special,MAX_SPECIAL,sizeof(special_t),f);
	fwrite(&song,1,1,f);
	fwrite(&flags,1,1,f);

	fwrite(map,width*height,sizeof(mapTile_t),f);
	return 1;
}

void Map::Init(world_t *wrld)
{
	int i;

	totalBrains=0;

	for(i=0;i<NUM_STARS;i++)
	{
		starX[i]=(word)Random(640);
		starY[i]=(word)Random(480);
		starCol[i]=(byte)Random(32);
	}

	for(i=0;i<width*height;i++)
	{
		map[i].templight=-32;	// make it all black so it'll fade in
		if(map[i].item==ITM_GASHOLE)
			map[i].itemInfo=(byte)Random(20);
		if(map[i].item==ITM_TREE5)	// prevent cat tree from meowing too soon
			map[i].itemInfo=30;
	}

	world=wrld;
	// pop in all the badguys
	AddMapGuys(this);

	// reset all special clocks
	for(i=0;i<MAX_SPECIAL;i++)
	{
		specialClock[i]=0;
		if(special[i].trigger&TRG_TIMED)
		{
			// start it at a different time
			if(special[i].msg[0]=='@')
				specialClock[i]=atoi(&special[i].msg[1]);
		}
	}
	allSpecialClock=0;	// this one just counts up to 30 so you know when a second occurs

	// re-entering a cleansed crypt, light it up
	if(player.levelNum>=10 && player.levelNum<=12 && player.var[VAR_ZOMBIE+player.levelNum-10])
		flags=(flags&(~MAP_TORCHLIT))|MAP_LITDUNGEON;

	if(opt.cheats[CH_LIGHT])
	{
		for(i=0;i<width*height;i++)
			if(map[i].light<0)
				map[i].light=0;	// make it all lit up
	}
}

void Map::Resize(int wid,int hei)
{
	mapTile_t *map2;
	int i,j;

	map2=(mapTile_t *)calloc(sizeof(mapTile_t)*wid*hei,1);

	for(i=0;i<width;i++)
		for(j=0;j<height;j++)
		{
			if(i<wid && j<hei)
				map2[i+j*wid]=map[i+j*width];
		}
	free(map);
	map=map2;
	width=wid;
	height=hei;

	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(badguy[i].x>=wid || badguy[i].y>=hei)
			badguy[i].type=0;
	}
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(special[i].x>=wid || special[i].y>=hei)
			special[i].trigger=0;
	}
	ExitGuys();
	InitGuys(MAX_MAPMONS);
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

void Map::GrowNeighbors(int tag,int x,int y)
{
	if(y>0 && map[x+(y-1)*width].tag==tag && map[x+(y-1)*width].item!=ITM_WALKPLANT)
	{
		MakeSound(SND_PLANTGROW,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,600);
		map[x+(y-1)*width].item=ITM_MAGICPLANT;
		map[x+(y-1)*width].itemInfo=1;
	}
	if(y<height-1 && map[x+(y+1)*width].tag==tag && map[x+(y+1)*width].item!=ITM_WALKPLANT)
	{
		MakeSound(SND_PLANTGROW,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,600);
		map[x+(y+1)*width].item=ITM_MAGICPLANT;
		map[x+(y+1)*width].itemInfo=1;
	}
	if(x>0 && map[x-1+y*width].tag==tag && map[x-1+y*width].item!=ITM_WALKPLANT)
	{
		MakeSound(SND_PLANTGROW,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,600);
		map[x-1+y*width].item=ITM_MAGICPLANT;
		map[x-1+y*width].itemInfo=1;
	}
	if(x<width-1 && map[x+1+y*width].tag==tag && map[x+1+y*width].item!=ITM_WALKPLANT)
	{
		MakeSound(SND_PLANTGROW,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,600);
		map[x+1+y*width].item=ITM_MAGICPLANT;
		map[x+1+y*width].itemInfo=1;
	}
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
				if(world->terrain[map[i].floor].flags&TF_ANIM)
					map[i].floor=world->terrain[map[i].floor].next;
				// animate doors opening
				if(map[i].item>=ITM_DOOR && map[i].item<ITM_DOOR2 && map[i].itemInfo>0)
				{
					map[i].itemInfo++;
					// if done animating, become an open door
					if(map[i].itemInfo/2==3)
					{
						map[i].item+=4;
						map[i].itemInfo=0;
						SpecialItemGetCheck(this,x,y);
					}
				}
				// animate the lake cave rising
				else if(map[i].item==ITM_LAKECAVE)
				{
					if(map[i].itemInfo<9)
						map[i].itemInfo++;
				}
			}
			if(world->terrain[map[i].floor].flags&TF_FIRE)
				Burn((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0);
			if(map[i].item==ITM_TORCH)
				Burn((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,30*FIXAMT);
			if(map[i].item==ITM_SWITCH1ON || map[i].item==ITM_SWITCH1OFF)
			{
				if(map[i].itemInfo>0)
				{
					map[i].itemInfo++;
					if(map[i].itemInfo>7*2)	// then swap it to the other kind
					{
						map[i].item=ITM_SWITCH1ON+ITM_SWITCH1OFF-map[i].item;
						map[i].itemInfo=0;
					}
				}
			}
			if(map[i].item==ITM_SWITCH2 && map[i].itemInfo)
			{
				map[i].itemInfo++;
				if(map[i].itemInfo>3*2)
					map[i].itemInfo=0;
			}
			if(map[i].item==ITM_MAGICPLANT && map[i].itemInfo)
			{
				map[i].itemInfo++;
				if(map[i].itemInfo>4*2)
				{
					map[i].itemInfo=0;
					map[i].item=ITM_WALKPLANT;
					GrowNeighbors(map[i].tag,x,y);
				}
			}
			if(map[i].item==ITM_GASHOLE)
			{
				map[i].itemInfo++;
				if(map[i].itemInfo==20)
				{
					map[i].itemInfo=0;
					GasSpray(x,y);
				}
			}
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
			{
				if(map[i].itemInfo)
				{
					map[i].itemInfo++;
					if(map[i].itemInfo>7)
						map[i].itemInfo=0;
				}
			}
			if(map[i].item==ITM_GASSPARK && map[i].itemInfo>0)
			{
				map[i].itemInfo--;
				Burn((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0);
				if(!WindingDown())
					WeakTorch(x,y);
				SpecialItemGetCheck(this,x,y);
			}
			if(map[i].item==ITM_TREE5)
			{
				if(map[i].itemInfo>0)
					map[i].itemInfo--;
				else
				{
					map[i].itemInfo=(byte)Random(50)+15;
					MakeSound(SND_MEOW,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,1200);
				}
			}
			if(map[i].item==ITM_AUTODOOR)
			{
				if(abs(goodguy->mapx-x)+abs(goodguy->mapy-y)<5)
				{
					if(map[i].itemInfo<6)
					{
						if(map[i].itemInfo==0)
							MakeSound(SND_AUTODOOROPEN,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,1200);
						map[i].itemInfo++;
					}
				}
				else if(map[i].itemInfo>0)
				{
					if(map[i].itemInfo==5)
						MakeSound(SND_AUTODOORSHUT,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,1200);
					map[i].itemInfo--;
				}
			}
			if(map[i].item==ITM_PORTAL)
			{
				map[i].itemInfo++;
				if(map[i].itemInfo>23)
					map[i].itemInfo=16;
			}
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

void Map::GasSpray(int x,int y)
{
	int i;

	if(x>0)
	{
		i=(x-1)+y*width;
		if(GasCanEnter(4,x-1,y,this))
		{
			FireBullet(((x-1)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,4,BLT_SWAMPGAS);
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
				map[i].itemInfo=1;
		}
	}
	if(x<width-1)
	{
		i=(x+1)+y*width;
		if(GasCanEnter(0,x+1,y,this))
		{
			FireBullet(((x+1)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_SWAMPGAS);
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
				map[i].itemInfo=1;
		}
	}
	if(y>0)
	{
		i=x+(y-1)*width;
		if(GasCanEnter(6,x,y-1,this))
		{
			FireBullet((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   ((y-1)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,6,BLT_SWAMPGAS);
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
				map[i].itemInfo=1;
		}
	}
	if(y<height-1)
	{
		i=x+(y+1)*width;
		if(GasCanEnter(2,x,y+1,this))
		{
			FireBullet((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   ((y+1)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,2,BLT_SWAMPGAS);
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
				map[i].itemInfo=1;
		}
	}
}

void Map::GasSpray2(int x,int y)
{
	int i;

	if(x>0)
	{
		i=(x-1)+y*width;
		if(GasCanEnter(4,x-1,y,this))
		{
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
			{
				FireBullet(((x-1)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						   (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,4,BLT_SWAMPGAS2);
				map[i].itemInfo=1;
			}
		}
	}
	if(x<width-1)
	{
		i=(x+1)+y*width;
		if(GasCanEnter(0,x+1,y,this))
		{
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
			{
				FireBullet(((x+1)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						   (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_SWAMPGAS2);
				map[i].itemInfo=1;
			}
		}
	}
	if(y>0)
	{
		i=x+(y-1)*width;
		if(GasCanEnter(6,x,y-1,this))
		{
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
			{
				FireBullet((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						   ((y-1)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,6,BLT_SWAMPGAS2);
				map[i].itemInfo=1;
			}
		}
	}
	if(y<height-1)
	{
		i=x+(y+1)*width;
		if(GasCanEnter(2,x,y+1,this))
		{
			if(map[i].item>=ITM_GASPIPELR && map[i].item<=ITM_GASPIPEUL)
			{
				FireBullet((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						   ((y+1)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,2,BLT_SWAMPGAS2);
				map[i].itemInfo=1;
			}
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
					if(map[curx+cury*width].wall ||
						((ItemFlags(map[curx+cury*width].item)&IF_TALL)	&&
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
					if(map[curx+cury*width].wall ||
						((ItemFlags(map[curx+cury*width].item)&IF_TALL)	&&
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
	if(map->map[x+y*map->width].wall)
		return 1;
	if(world->terrain[map->map[x+y*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))
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

void Map::Swap(int sx,int sy,int blkwidth,int blkheight,int dx,int dy)
{
	mapTile_t *tempMap;
	int i;

	if(sx<0 || sx>=width || sy<0 || sy>=height ||
		(sx+blkwidth)<sx || (sx+blkwidth)>=width ||
		(sy+blkheight)<sy || (sy+blkheight)>=height ||
		dx<0 || dx>=width || dy<0 || dy>=height ||
		(dx+blkwidth)<dx || (dx+blkwidth)>=width ||
		(dy+blkheight)<dy || (dy+blkheight)>=height)
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
		}
		else if(special[i].x>=dx && special[i].y>=dy && special[i].x<dx+blkwidth && special[i].y<dy+blkheight)
		{
			special[i].x=special[i].x+sx-dx;
			special[i].y=special[i].y+sy-dy;
		}
	}

	// and move all guys in the target
	GuySwap(sx,sy,blkwidth,blkheight,dx,dy);
	// don't forget bullets
	BulletSwap(sx,sy,blkwidth,blkheight,dx,dy);
}

void Map::Clone(int sx,int sy,int blkwidth,int blkheight,int dx,int dy)
{
	int i;

	if(sx<0 || sx>=width || sy<0 || sy>=height ||
		(sx+blkwidth)<sx || (sx+blkwidth)>width ||
		(sy+blkheight)<sy || (sy+blkheight)>height ||
		dx<0 || dx>=width || dy<0 || dy>=height ||
		(dx+blkwidth)<dx || (dx+blkwidth)>width ||
		(dy+blkheight)<dy || (dy+blkheight)>height)
		return;	// illegal coordinates

	// copy source to dest
	for(i=0;i<blkheight;i++)
	{
		memcpy(&map[(i+dy)*width+dx],&map[(i+sy)*width+sx],sizeof(mapTile_t)*blkwidth);
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

				if(!(flags&MAP_SHOWLIGHTS))
				{
					lites[0]=lites[1]=lites[2]=lites[3]=lites[4]=lites[5]=lites[6]=lites[7]=lites[8]=0;
				}
				if(flags&MAP_SHOWITEMS)
					RenderItem(scrX+camX+(TILE_WIDTH/2),scrY+camY+(TILE_HEIGHT/2),0,
						m->item,m->itemInfo,lite);

				if(m->wall && (flags&MAP_SHOWWALLS))	// there is a wall on this tile
				{
					if(j<height-1)
					{
						// if the tile below this one is also a wall, don't waste the
						// time of drawing the front of this wall
						if(map[i+(j+1)*width].wall &&
							(!(world->terrain[map[i+(j+1)*width].floor].flags&TF_TRANS)))
						{
							if(world->terrain[m->floor].flags&TF_TRANS)
								RoofDraw(scrX+camX,scrY+camY,m->floor,lites,
										DISPLAY_DRAWME|DISPLAY_ROOFTILE|DISPLAY_TRANSTILE);
							else
								RoofDraw(scrX+camX,scrY+camY,m->floor,lites,DISPLAY_DRAWME|DISPLAY_ROOFTILE);
						}
						else
							if(world->terrain[m->floor].flags&TF_TRANS)
								WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
							else
								WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE);
					}
					// make wall tiles get drawn in sorted order unlike the floor tiles
					else
					{
						if(world->terrain[m->floor].flags&TF_TRANS)
							WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
								DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
						else
							WallDraw(scrX+camX,scrY+camY,m->wall,m->floor,lites,
								DISPLAY_DRAWME|DISPLAY_WALLTILE);
					}
				}
				else
				{
					if(j<height-1 && map[i+(j+1)*width].wall && !(world->terrain[map[i+(j+1)*width].wall].flags&TF_NOSHADOW))
					{
						if(i<width-1 && map[i+1+j*width].wall  && !(world->terrain[map[i+1+(j)*width].wall].flags&TF_NOSHADOW))
						{
							shdw=6;
						}
						else
						{
							if(i<width-1 && map[i+1+(j+1)*width].wall  && !(world->terrain[map[i+1+(j+1)*width].wall].flags&TF_NOSHADOW))
								shdw=4;
							else
								shdw=7;
						}
					}
					else
					{
						if(i<width-1)
						{
							if(map[i+1+(j)*width].wall && !(world->terrain[map[i+1+(j)*width].wall].flags&TF_NOSHADOW))
							{
								if(j<height-1 && map[i+1+(j+1)*width].wall  && !(world->terrain[map[i+1+(j+1)*width].wall].flags&TF_NOSHADOW))
									shdw=1;
								else
									shdw=2;
							}
							else
							{
								if(j<height-1 && map[i+1+(j+1)*width].wall  && !(world->terrain[map[i+1+(j+1)*width].wall].flags&TF_NOSHADOW))
									shdw=3;
								else
									shdw=0;
							}
						}
						else
							shdw=0;
					}

					if(j<height-1)
					{
						// if there is a wall on the tile below this one, no
						// point in rendering this floor (unless it is transparent
						if((!map[i+(j+1)*width].wall) ||
							(world->terrain[map[i+(j+1)*width].floor].flags&TF_TRANS))
							RenderFloorTileFancy(scrX,scrY,m->floor,shdw,lites);
						/*
						// if there is a wall on the tile below this one, no
						// point in rendering this floor (unless it is transparent
						if((!map[i+(j+1)*width].wall) || (!(flags&MAP_SHOWWALLS) ||
							(world->terrain[map[i+(j+1)*width].floor].flags&TF_TRANS)))
						{
							// if there's a wall to the right, draw a shadow on this tile
							if((i<width-1) && (map[i+1+j*width].wall) &&
								!(world->terrain[map[i+1+j*width].wall].flags&TF_NOSHADOW))
								RenderFloorTileShadow(scrX,scrY,m->floor,lite);
							else
								RenderFloorTile(scrX,scrY,m->floor,lite);
						}
						*/
					}
					else
					{
						// if there's a wall to the right, draw a shadow on this tile
						RenderFloorTileFancy(scrX,scrY,m->floor,shdw,lites);
						/*
						// if there's a wall to the right, draw a shadow on this tile
						if((i<width-1) && (map[i+1+j*width].wall) &&
							!(world->terrain[map[i+1+j*width].wall].flags&TF_NOSHADOW))
							RenderFloorTileShadow(scrX,scrY,m->floor,lite);
						else
							RenderFloorTile(scrX,scrY,m->floor,lite);
							*/
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
		int tx,ty;

		for(i=0;i<NUM_STARS;i++)
		{
			tx=(starX[i]+camX)/TILE_WIDTH;
			ty=(starY[i]+camY)/TILE_HEIGHT;

			if(tx<0 || ty<0 || tx>=width || ty>=height)
				continue;

			m=&map[tx+ty*width];
			if(m->wall)
				continue;

			PlotStar(starX[i],starY[i],starCol[i],(starX[i]+camX-tx*TILE_WIDTH),(starY[i]+camY-ty*TILE_HEIGHT),m->floor);
		}
	}
}

void Map::ShowTags(int camX,int camY,int copyX,int copyY,int copyWid,int copyHei)
{
	int i,j;

	int tileX,tileY;
	int ofsX,ofsY;
	int scrX,scrY;
	mapTile_t *m;
	char s[6];

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
				sprintf(s,"%d",m->tag);
				if(m->tag>0)
					Print(i*TILE_WIDTH+2-camX,
						  j*TILE_HEIGHT+6-camY,s,0,1);
			}
			scrY+=TILE_HEIGHT;
		}
		scrX+=TILE_WIDTH;
	}
	if(copyX!=0 && copyY!=0)
	{
		DrawBox((-tileX+copyX)*TILE_WIDTH-ofsX-TILE_WIDTH,
				(-tileY+copyY)*TILE_HEIGHT-ofsY-TILE_HEIGHT,
				(-tileX+copyX+copyWid)*TILE_WIDTH-ofsX-TILE_WIDTH,
				(-tileY+copyY+copyHei)*TILE_HEIGHT-ofsY-TILE_HEIGHT,255);
	}
}

void Map::CopyChunk(int cx,int cy,int cwidth,int cheight,int dx,int dy)
{
	int i,j;

	dx-=cwidth/2;
	dy-=cheight/2;
	for(i=cx;i<cx+cwidth;i++)
	{
		for(j=cy;j<cy+cheight;j++)
		{
			if(i>=0 && i<width && j>=0 && j<height &&
				(dx+i-cx)>=0 && (dx+i-cx)<width && (dy+j-cy)>=0 && (dy+j-cy)<height)
				map[dx+i-cx+(dy+j-cy)*width]=map[i+j*width];
		}
	}
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(badguy[i].x>=cx && badguy[i].y>=cy &&
		   badguy[i].x<cx+cwidth && badguy[i].y<cy+cheight && badguy[i].type!=MONS_NONE)
		{
			for(j=0;j<MAX_MAPMONS;j++)
			{
				if(badguy[j].type==MONS_NONE)
				{
					badguy[j]=badguy[i];
					badguy[j].x+=(-cx+dx);
					badguy[j].y+=(-cy+dy);
					if(badguy[j].x>=width || badguy[j].x<0 ||
						badguy[j].y>=height || badguy[j].y<0)
						badguy[j].type=MONS_NONE;
					break;
				}
			}
		}
	}
	for(i=0;i<MAX_SPECIAL;i++)
	{
		// remove all specials in the destination block
		if(special[i].x>=dx && special[i].y>=dy &&
			special[i].x<dx+cwidth && special[i].y<dy+cheight)
			special[i].trigger=0;

		if(special[i].x>=cx && special[i].y>=cy &&
		   special[i].x<cx+cwidth && special[i].y<cy+cheight)
		{
			for(j=0;j<MAX_SPECIAL;j++)
			{
				if(special[j].trigger==0)
				{
					special[j]=special[i];
					special[j].x+=(-cx+dx);
					special[j].y+=(-cy+dy);
					special[j].trigX+=(-cx+dx);
					special[j].trigY+=(-cy+dy);
					special[j].effectX+=(-cx+dx);
					special[j].effectY+=(-cy+dy);
					if(special[j].x>=width || special[j].x<0 ||
						special[j].y>=height || special[j].y<0)
						special[j].trigger=0;
					break;
				}
			}
		}
	}
	ExitGuys();
	InitGuys(MAX_MAPMONS);
	AddMapGuys(this);
}

void Map::LoadFromProgress(FILE *f)
{
	int i;

	if(map)
		free(map);

	fread(&width,sizeof(int),1,f);
	fread(&height,sizeof(int),1,f);
	fread(&flags,sizeof(byte),1,f);
	map=(mapTile_t *)malloc(width*height*sizeof(mapTile_t));
	fread(map,sizeof(mapTile_t),width*height,f);
	fread(special,sizeof(special_t),MAX_SPECIAL,f);

	if(opt.cheats[CH_LIGHT])
		for(i=0;i<width*height;i++)
			map[i].light=0;
}

void Map::SaveProgress(FILE *f)
{
	fwrite(&width,sizeof(int),1,f);
	fwrite(&height,sizeof(int),1,f);
	fwrite(&flags,sizeof(byte),1,f);
	fwrite(map,sizeof(mapTile_t),width*height,f);
	fwrite(special,sizeof(special_t),MAX_SPECIAL,f);
}

byte Map::FindPowerUps(int x,int y)
{
	int i,xx,yy;
	int result,r;

	xx=0;
	yy=0;
	result=0;

	for(i=0;i<width*height;i++)
	{
		if((map[i].item>=ITM_SUPERGEM && map[i].item<=ITM_FIRERATEUP) ||
			(map[i].item>=ITM_KEY2 && map[i].item<=ITM_WHOTPANTS) ||
			map[i].item==ITM_MYSORB || map[i].item==ITM_SHROOM || map[i].item==ITM_DAISY ||
			map[i].item==ITM_SILVER || map[i].item==ITM_TALISMAN || map[i].item==ITM_TRIPLEFIRE ||
			map[i].item==ITM_BUST || map[i].item==ITM_BAT || map[i].item==ITM_SUPERHEART)
		{
			r=(abs(xx-x)+abs(yy-y));
			if(r>31)
				r=1;
			else
				r=32-r;
			if(r>result)
				result=r;
		}
		xx++;
		if(xx>=width)
		{
			xx=0;
			yy++;
		}
	}
	return result;
}

void ZapWall(Map *map,int x,int y,word newFloor)
{
	word flr,wall;

	if(map->map[x+y*map->width].wall==0)	// no wall to zap
		return;

	// store these for looking for neighbors
	flr=map->map[x+y*map->width].floor;
	wall=map->map[x+y*map->width].wall;

	// turn this square into floor
	map->map[x+y*map->width].floor=newFloor;
	map->map[x+y*map->width].wall=0;

	// recurse to all neighbors that match
	if(x>0 && map->map[x-1+y*map->width].wall==wall &&
		map->map[x-1+y*map->width].floor==flr)
		ZapWall(map,x-1,y,newFloor);
	if(x<map->width-1 && map->map[x+1+y*map->width].wall==wall &&
		map->map[x+1+y*map->width].floor==flr)
		ZapWall(map,x+1,y,newFloor);
	if(y>0 && map->map[x+(y-1)*map->width].wall==wall &&
		map->map[x+(y-1)*map->width].floor==flr)
		ZapWall(map,x,y-1,newFloor);
	if(y<map->height-1 && map->map[x+(y+1)*map->width].wall==wall &&
		map->map[x+(y+1)*map->width].floor==flr)
		ZapWall(map,x,y+1,newFloor);
}

void RaiseWall(Map *map,int x,int y,byte newWall)
{
	word flr;

	if(map->map[x+y*map->width].wall)	// there's a wall already, can't raise one
		return;
	if(newWall==0)	// can't raise a non-wall (wall value of 0 indicates absence of wall)
		return;

	// store these for looking for neighbors
	flr=map->map[x+y*map->width].floor;

	// turn this square into a wall
	map->map[x+y*map->width].wall=newWall;

	// recurse to all neighbors that match
	if(x>0 && map->map[x-1+y*map->width].wall==0 &&
		map->map[x-1+y*map->width].floor==flr)
		RaiseWall(map,x-1,y,newWall);
	if(x<map->width-1 && map->map[x+1+y*map->width].wall==0 &&
		map->map[x+1+y*map->width].floor==flr)
		RaiseWall(map,x+1,y,newWall);
	if(y>0 && map->map[x+(y-1)*map->width].wall==0 &&
		map->map[x+(y-1)*map->width].floor==flr)
		RaiseWall(map,x,y-1,newWall);
	if(y<map->height-1 && map->map[x+(y+1)*map->width].wall==0 &&
		map->map[x+(y+1)*map->width].floor==flr)
		RaiseWall(map,x,y+1,newWall);
}

void ChgTile(Map *map,int x,int y,word newTile)
{
	word flr;

	// store this for looking for neighbors
	flr=map->map[x+y*map->width].floor;

	// change the tile
	map->map[x+y*map->width].floor=newTile;

	// recurse to all neighbors that match
	if(x>0 && map->map[x-1+y*map->width].floor==flr)
		ChgTile(map,x-1,y,newTile);
	if(x<map->width-1 && map->map[x+1+y*map->width].floor==flr)
		ChgTile(map,x+1,y,newTile);
	if(y>0 && map->map[x+(y-1)*map->width].floor==flr)
		ChgTile(map,x,y-1,newTile);
	if(y<map->height-1 && map->map[x+(y+1)*map->width].floor==flr)
		ChgTile(map,x,y+1,newTile);
}

void ZapWallTag(Map *map,byte tag,word newFloor)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			if(map->map[i].wall)
			{
				map->map[i].floor=newFloor;
				map->map[i].wall=0;
			}
		}
	}
}

void RaiseWallTag(Map *map,byte tag,word newWall)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			if(map->map[i].wall==0)
			{

				map->map[i].wall=newWall;
			}
		}
	}
}

void ToggleWallTag(Map *map,byte tag,word newWall)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			if(map->map[i].wall==0)
			{
				map->map[i].wall=newWall;
			}
			else
			{
				map->map[i].wall=0;
			}
		}
	}
}

void ChgTileTag(Map *map,byte tag,word newTile)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			map->map[i].floor=newTile;
		}
	}
}

void SummonGuyTag(Map *map,byte type,byte tag)
{
	int i,x,y;
	Guy *g;

	x=0;
	y=0;
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			g=AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
				0,type);
			if(g)
				g->tag=tag;
		}
		x++;
		if(x==map->width)
		{
			x=0;
			y++;
		}
	}
}

void DropItemTag(Map *map,byte type,byte tag,byte fire)
{
	int i,x,y;

	x=0;
	y=0;
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			if(fire)
			{
				Burn(((i%map->width)*TILE_WIDTH+Random(TILE_WIDTH+1))*FIXAMT,((i/map->width)*TILE_HEIGHT+Random(TILE_HEIGHT+1))*FIXAMT,0);
				Burn(((i%map->width)*TILE_WIDTH+Random(TILE_WIDTH+1))*FIXAMT,((i/map->width)*TILE_HEIGHT+Random(TILE_HEIGHT+1))*FIXAMT,0);
				Burn(((i%map->width)*TILE_WIDTH+Random(TILE_WIDTH+1))*FIXAMT,((i/map->width)*TILE_HEIGHT+Random(TILE_HEIGHT+1))*FIXAMT,0);
				Burn(((i%map->width)*TILE_WIDTH+Random(TILE_WIDTH+1))*FIXAMT,((i/map->width)*TILE_HEIGHT+Random(TILE_HEIGHT+1))*FIXAMT,0);
				Burn(((i%map->width)*TILE_WIDTH+Random(TILE_WIDTH+1))*FIXAMT,((i/map->width)*TILE_HEIGHT+Random(TILE_HEIGHT+1))*FIXAMT,0);
				Burn(((i%map->width)*TILE_WIDTH+Random(TILE_WIDTH+1))*FIXAMT,((i/map->width)*TILE_HEIGHT+Random(TILE_HEIGHT+1))*FIXAMT,0);
			}

			map->map[i].item=type;
		}
		x++;
		if(x==map->width)
		{
			x=0;
			y++;
		}
	}
}

void AutoDoorChange(Map *map,byte tag,byte val)
{
	int i,f;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==tag)
		{
			if(map->map[i].item==ITM_AUTODOOR || map->map[i].item==ITM_AUTODOOR2)
			{
				switch(val)
				{
					case 0:	// shut off
						map->map[i].item=ITM_AUTODOOR2;
						map->map[i].itemInfo=0;
						break;
					case 1: // turn on
						map->map[i].item=ITM_AUTODOOR;
						map->map[i].itemInfo=0;
						break;
					case 2: // toggle
						map->map[i].item=ITM_AUTODOOR2+ITM_AUTODOOR-map->map[i].item;
						map->map[i].itemInfo=0;
						break;
				}
			}
			if(map->map[i].floor>=194 && map->map[i].floor<=197)
			{
				f=map->map[i].floor;
				switch(val)
				{
					case 0:	// shut off
						map->map[i].floor=(f&1)+194;
						break;
					case 1: // turn on
						map->map[i].floor=(f&1)+196;
						break;
					case 2: // toggle
						if(f<196)
							map->map[i].floor=(f&1)+196;
						else
							map->map[i].floor=(f&1)+194;
						break;
				}
			}
		}
	}
}

void SpecialStepCheck(Map *map,int x,int y,Guy *me)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if((map->special[i].trigger&(TRG_STEP|TRG_ENEMYSTEP)) &&
			map->special[i].trigX==x && map->special[i].trigY==y)
		{
			if((me->type==player.monsType && (map->special[i].trigger&TRG_STEP)) ||
			   (me->type!=player.monsType && me->type!=MONS_HELPERBAT && (map->special[i].trigger&TRG_ENEMYSTEP)))
			{
				// if there is a switch here, specials may only be triggered when it is
				// waiting for input
				if(map->map[x+y*map->width].item<ITM_SWITCH1ON ||
					map->map[x+y*map->width].item>ITM_SWITCH2 ||
					map->map[x+y*map->width].itemInfo==0)
				SpecialTakeEffect(i,map,&map->special[i],me);
			}
		}
		else if((map->special[i].trigger&TRG_NEAR) && (me->type==player.monsType) &&
			(abs(map->special[i].trigX-x)+abs(map->special[i].trigY-y)<=map->special[i].trigValue))
		{
			SpecialTakeEffect(i,map,&map->special[i],me);
		}
	}
}

void SpecialShootCheck(Map *map,int x,int y)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
		if((map->special[i].trigger&TRG_SHOOT) &&
			map->special[i].x>=x-1 && map->special[i].y>=y-1 &&
			map->special[i].x<=x+1 && map->special[i].y<=y+1)
		{
			SpecialTakeEffect(i,map,&map->special[i],NULL);
		}
}

void SpecialPushCheck(Map *map,int x,int y)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
		if((map->special[i].trigger&TRG_ITEMHERE) &&
			map->special[i].trigX==x && map->special[i].trigY==y &&
			map->map[x+y*map->width].item==map->special[i].trigValue)
		{
			SpecialTakeEffect(i,map,&map->special[i],NULL);
		}
}

void SpecialAnytimeCheck(Map *map)
{
	int i;

	allSpecialClock++;
	if(allSpecialClock==30)
	{
		allSpecialClock=0;
		for(i=0;i<MAX_SPECIAL;i++)
		{
			if((!(map->special[i].trigger&TRG_DELAYCHAIN)) || (specialClock[i]!=0))
				specialClock[i]++;
		}
	}

	for(i=0;i<MAX_SPECIAL;i++)
		if((map->special[i].trigger) &&
			(!(map->special[i].trigger&(TRG_SHOOT|TRG_ITEMHERE|TRG_STEP|TRG_ENEMYSTEP|TRG_NEAR|TRG_CHAIN|TRG_GETITEM))))
		{
			SpecialTakeEffect(i,map,&map->special[i],NULL);
		}
}

void SpecialNeighborCheck(Map *map,special_t *spcl)
{
	int i,j,k;

	for(i=spcl->x-1;i<=spcl->x+1;i++)
		for(j=spcl->y-1;j<=spcl->y+1;j++)
		{
			if(i!=spcl->x || j!=spcl->x)
				for(k=0;k<MAX_SPECIAL;k++)
				{
					if(map->special[k].x==i && map->special[k].y==j && (map->special[k].trigger&TRG_CHAIN))
						SpecialTakeEffect(i,map,&map->special[k],NULL);
					if(map->special[k].x==i && map->special[k].y==j && (map->special[k].trigger&TRG_DELAYCHAIN))
						specialClock[k]=1;
				}
		}
}

void SpecialItemGetCheck(Map *map,int x,int y)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
		if((map->special[i].trigger&TRG_GETITEM) && map->special[i].trigX==x &&
			map->special[i].trigY==y)
		{
			SpecialTakeEffect(i,map,&map->special[i],NULL);
		}
}

void SpecialTakeEffect(byte num,Map *map,special_t *spcl,Guy *victim)
{
	char s[64];
	char *tok;
	byte fail;
	int sx,sy,sx2,sy2;
	Guy *g;

	// first check to be sure all conditions are met
	if(spcl->trigger&TRG_KILLALL)
	{
		if(spcl->trigValue!=0)
		{
			// dealing with a specific type
			if(MonsterExists(spcl->trigValue))
				return;	// they ain't all dead yet
		}
		else
		{
			// dealing with tagged monsters
			if(TaggedMonsterExists(spcl->trigValue2))
				return;	// they ain't all dead yet
		}
	}
	if(spcl->trigger&TRG_VARON)
	{
		if(player.var[spcl->trigValue]!=spcl->trigValue2)
			return;
	}
	if(spcl->trigger&TRG_VAROFF)
	{
		if(player.var[spcl->trigValue]==spcl->trigValue2)
			return;
	}
	if(spcl->trigger&TRG_TIMED)
	{
		if(specialClock[num]!=spcl->trigValue)
			return;
		specialClock[num]=0;	// repeat
	}
	if(spcl->trigger&TRG_RANDOM)
	{
		if(allSpecialClock!=0)	// only can happen once a second
			return;
		if((int)Random(100)>=spcl->trigValue)
				return;
	}
	if(spcl->trigger&TRG_DELAYCHAIN)
	{
		if(specialClock[num]!=spcl->trigValue+1)
			return;
		specialClock[num]=0;	// repeat
	}

	if(!(spcl->trigger&(TRG_CHAIN|TRG_DELAYCHAIN)))
		SpecialNeighborCheck(map,spcl);

	sx=map->map[spcl->trigX+spcl->trigY*map->width].item;
	if(sx==ITM_SWITCH1ON || sx==ITM_SWITCH1OFF || sx==ITM_SWITCH2)
	{
		// start the switch moving!
		if(map->map[spcl->trigX+spcl->trigY*map->width].itemInfo==0)
		{
			if(sx==ITM_SWITCH2)
				MakeNormalSound(SND_SWITCH2);
			else
				MakeNormalSound(SND_SWITCH);
			map->map[spcl->trigX+spcl->trigY*map->width].itemInfo=1;
		}
	}
	switch(spcl->effect)
	{
		case SPC_GOTOMAP:
			if(spcl->value==41 && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) &&
				player.var[VAR_QUESTDONE+QUEST_HILL]==1)
				spcl->value=44;	// go to empty rooftop instead
			SendMessageToGame(MSG_GOTOMAP,spcl->value);
			player.destx=spcl->effectX;
			player.desty=spcl->effectY;
			BadgeCheck(BE_GOTOMAP,spcl->value,map);
			break;
		case SPC_SETVAR:
			PlayerSetVar(spcl->value,spcl->effectTag);
			break;
		case SPC_ZAPWALL:
			if(spcl->effectTag)
				ZapWallTag(map,spcl->effectTag,spcl->value);
			else
				if(map->map[spcl->effectX+spcl->effectY*map->width].wall)
				{
					ZapWall(map,spcl->effectX,spcl->effectY,spcl->value);
				}
			break;
		case SPC_RAISEWALL:
			if(spcl->effectTag)
				RaiseWallTag(map,spcl->effectTag,spcl->value);
			else
				if(!map->map[spcl->effectX+spcl->effectY*map->width].wall)
				{
					RaiseWall(map,spcl->effectX,spcl->effectY,spcl->value);
				}
			break;
		case SPC_TOGGLEWALL:
			if(spcl->effectTag)
				ToggleWallTag(map,spcl->effectTag,spcl->value);
			else
			{
				if(map->map[spcl->effectX+spcl->effectY*map->width].wall)
				{
					// store what the wall looked like for later popping it back up
					spcl->value=map->map[spcl->effectX+spcl->effectY*map->width].wall;
					ZapWall(map,spcl->effectX,spcl->effectY,
						map->map[spcl->effectX+spcl->effectY*map->width].floor);
				}
				else
				{
					RaiseWall(map,spcl->effectX,spcl->effectY,spcl->value);
				}
			}
			break;
		case SPC_LIGHT:
			map->LightSpecial(spcl->effectX,spcl->effectY,spcl->value,abs(spcl->value)/2);
			break;
		case SPC_TELEPORT:
			if(victim)
			{
				if(victim->type==player.monsType)
				{
					if(GetTportClock())
						return;	// can't teleport yet
				}
				victim->x=(spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT;
				victim->y=(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT;
				victim->dx=0;
				victim->dy=0;
				if(victim->type==player.monsType)
				{
					PutCamera(victim->x,victim->y);
					UpdateCamera(victim->x,victim->y,victim->facing,map);
					SetTportClock(30);
				}
			}
			break;
		case SPC_SUMMON:
			if(spcl->effectTag==0)
			{
				g=AddGuy((spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
						64*FIXAMT,spcl->value);
				if(g)
					g->tag=map->map[spcl->effectX+spcl->effectY*map->width].tag;
			}
			else
			{
				SummonGuyTag(map,spcl->value,spcl->effectTag);
			}
			break;
		case SPC_PICTURE:
			ShowImageOrFlic(spcl->msg);
			break;
		case SPC_PLAYSOUND:
			if(spcl->effectX==0 && spcl->effectY==0)
				MakeNormalSound(spcl->value);
			else
				MakeSound(spcl->value,(spcl->effectX*TILE_WIDTH)<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,10000);
			break;
		case SPC_DROPITEM:
			if(spcl->effectTag==0)
				map->map[spcl->effectX+spcl->effectY*map->width].item=spcl->value;
			else
				DropItemTag(map,spcl->value,spcl->effectTag,(spcl->msg[0]=='@'));
			break;
		case SPC_TEMPLIGHT:
			map->BrightTorch(spcl->effectX,spcl->effectY,spcl->value,spcl->value/2);
			break;
		case SPC_SWAPMAP:
			// must parse coords from message text
			strcpy(s,spcl->msg);
			fail=0;
			tok=strtok(s,", ");
			if(tok)
			{
				sx=atoi(tok);
				tok=strtok(NULL,"- ");
				if(tok)
				{
					sy=atoi(tok);
					tok=strtok(NULL,", ");
					if(tok)
					{
						sx2=atoi(tok);
						tok=strtok(NULL," ");
						if(tok)
							sy2=atoi(tok);
						else fail=1;
					}
					else fail=1;
				}
				else fail=1;
			}
			else fail=1;

			if(!fail)
			{
				map->Swap(sx,sy,(sx2-sx+1),(sy2-sy+1),spcl->effectX,spcl->effectY);
			}
			break;
		case SPC_COPYMAP:
			// must parse coords from message text
			strcpy(s,spcl->msg);
			fail=0;
			tok=strtok(s,", ");
			if(tok)
			{
				sx=atoi(tok);
				tok=strtok(NULL,"- ");
				if(tok)
				{
					sy=atoi(tok);
					tok=strtok(NULL,", ");
					if(tok)
					{
						sx2=atoi(tok);
						tok=strtok(NULL," ");
						if(tok)
							sy2=atoi(tok);
						else fail=1;
					}
					else fail=1;
				}
				else fail=1;
			}
			else fail=1;

			if(!fail)
			{
				map->Clone(sx,sy,(sx2-sx+1),(sy2-sy+1),spcl->effectX,spcl->effectY);
			}
			break;
		case SPC_CHGTILE:
			if(spcl->effectTag)
				ChgTileTag(map,spcl->effectTag,spcl->value);
			else
				ChgTile(map,spcl->effectX,spcl->effectY,spcl->value);
			break;
		case SPC_CHAT:
			BeginChatting(spcl->value);
			break;
		case SPC_LIGHTBALL:
			if(spcl->value==5)
			{
				FireBullet((spcl->effectX*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						   (spcl->effectY*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
						   spcl->effectTag,BLT_FOUNTAIN);
			}
			else if(spcl->value==6)
			{
				FireBullet((spcl->effectX*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						   (spcl->effectY*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
						   0,BLT_PORTAL);
			}
			else
			{
				LightBallChange(spcl->effectTag,spcl->value);
				AutoDoorChange(map,spcl->effectTag,spcl->value);
			}
			break;
	}
	if(spcl->trigger&TRG_MESSAGE)
		NoRepeatNewMessage(spcl->msg,75);
	if(!(spcl->trigger&TRG_REPEATABLE))
		spcl->trigger=0;	// can't trigger anymore
}

int TotalBrains(void)
{
	return totalBrains;
}

void CheckHiPuzzle(Map *map)
{
	byte layout[]={
		1,0,1,0,1,
		1,1,1,0,1,
		1,0,1,0,1,
	};
	int i,j,match;

	match=0;
	for(i=0;i<5;i++)
		for(j=0;j<3;j++)
		{
			if((layout[i+j*5]==1 && map->map[2+i+(18+j)*map->width].floor==207) ||
				(layout[i+j*5]==0 && map->map[2+i+(18+j)*map->width].floor==208))
				match++;
		}
	if(match==15)
		PlayerSetVar(VAR_HIPUZZLE,1);
}

byte CheckForPipes(Map *map)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].item==ITM_GASPIPELR || map->map[i].item==ITM_GASPIPEUD ||
		   map->map[i].item==ITM_GASPIPEDR || map->map[i].item==ITM_GASPIPEDL ||
		   map->map[i].item==ITM_GASPIPEUR || map->map[i].item==ITM_GASPIPEUL)
		   return 1;
	}
	return 0;
}
