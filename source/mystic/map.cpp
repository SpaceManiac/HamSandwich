#include "map.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "guy.h"
#include "water.h"
#include "challenge.h"

int totalBrains;
static world_t *world;
byte brainX,brainY;

Map::Map(FILE *f)
{
	fread(&width,1,sizeof(int),f);
	fread(&height,1,sizeof(int),f);

	fread(name,32,sizeof(char),f);
	fread(badguy,MAX_MAPMONS,sizeof(mapBadguy_t),f);
	fread(special,MAX_SPECIAL,sizeof(special_t),f);
	fread(&song,1,1,f);
	fread(&flags,1,1,f);

	map=(mapTile_t *)calloc(sizeof(mapTile_t)*width*height,1);

	fread(map,width*height,sizeof(mapTile_t),f);
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
	song=2;
	flags=0;
	memset(badguy,0,sizeof(mapBadguy_t)*MAX_MAPMONS);
	memset(special,0,sizeof(special_t)*MAX_SPECIAL);
	// temporary hack so I don't have to slap down ground myself
	for(i=0;i<width*height;i++)
		map[i].floor=(byte)MGL_random(2);

	/*
	// really temporary hack for the maze level
	if(size==2)
	{
		int j,k,l,wid,hei;
		wid=8;
		hei=8;
		for(i=0;i<128/wid;i++)
			for(j=0;j<128/hei;j++)
			{
				for(k=0;k<wid;k++)
					for(l=0;l<hei;l++)
					{
						if(k==0 || l==0 || k==wid-1 || l==hei-1)
						{
							map[i*wid+k+(j*wid+l)*width].floor=199;
							map[i*wid+k+(j*wid+l)*width].wall=22;
						}
						if((k==0 && l==0) || (k==0 && l==hei-1))
						{
							map[i*wid+k+(j*wid+l)*width].floor=51;
						}
						if((k==wid-1 && l==0) || (k==wid-1 && l==hei-1))
						{
							map[i*wid+k+(j*wid+l)*width].floor=49;
						}
					}
			}
	}
	*/
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
	byte s;

	totalBrains=0;
	SetChallengeCrystals(0);

	for(i=0;i<width*height;i++)
	{
		if(map[i].item==ITM_BRAIN)
			totalBrains++;
		if(map[i].item==ITM_KEYCH1 && PlayerKeyChain(0))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_KEYCH2 && PlayerKeyChain(1))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_KEYCH3 && PlayerKeyChain(2))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_KEYCH4 && PlayerKeyChain(3))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_SPELLBOOK && PlayerHasSpell())
			map[i].item=0;

		map[i].templight=-32;	// make it all black so it'll fade in
		if(map[i].item==ITM_SKLDOOR && player.worldNum==2 && player.levelPassed[2][6]==1)
			map[i].item=ITM_SKLDOOR2;

		if(map[i].item==ITM_SPELLBOOK)
		{
			s=SpellBookForThisLevel(player.levelNum,player.worldNum);
			if(player.gotSpell[s+(player.worldNum>1)*10])
				map[i].item=0;
			if(player.worldNum==2 && player.levelNum==16 && player.vaultOpened && !Challenging())
				map[i].item=0;	// take it away if you've opened the vault before
			/*
			if(player.spell[s]==1 && player.worldNum<2)
				map[i].item=0;
			if(player.spell[s]==2 && player.worldNum>1)
				map[i].item=0;
				*/
		}
		if(map[i].item>=ITM_KEYCH1 && map[i].item<=ITM_KEYCH4)
		{
			if(player.keychain[map[i].item-ITM_KEYCH1])
				map[i].item=0;
		}
		if(map[i].item==ITM_FAIRYBELL)
		{
			if(player.haveFairy&(1<<(FairyForThisLevel(player.levelNum+player.worldNum*50)-1)))
				map[i].item=0;
			for(s=0;s<4;s++)
				if(player.chaseFairy[s]==FairyForThisLevel(player.levelNum+player.worldNum*50))
					map[i].item=0;
		}
		if(player.worldNum==2 && player.levelNum==16 && player.vaultOpened && map[i].item==ITM_COIN)
			map[i].item=0;	// if you got the spellbook, take the coins away!
		if(map[i].item>=ITM_LETTERM && map[i].item<=ITM_LETTERC && !Challenging())
			map[i].item=0;	// remove MYSTIC letters when not in challenge mode
		if(map[i].item==ITM_CHLGCRYSTAL)
		{
			if(Challenging())
				SetChallengeCrystals(GetChallengeCrystals()+1);
			else
				map[i].item=0;
		}
	}

	if(player.worldNum==2 && player.levelNum==1)	// castle hub, move the entrances to passed Guardian levels
	{
		for(i=0;i<MAX_SPECIAL;i++)
			if(special[i].effect==SPC_GOTOMAP && (special[i].value==8 || special[i].value==10 ||
				special[i].value==12 || special[i].value==14) && player.levelPassed[2][special[i].value])
			{
				special[i].effect=0;	// disable this special
			}
	}

	if(player.levelNum==19 && player.worldNum==2)
		totalBrains=2;
	if(player.levelNum==12 && player.worldNum==1)
		totalBrains=3;

	world=wrld;
	// pop in all the badguys
	i=BattleMode();
	if(i==0)
		AddMapGuys(this);
	else
		AddBattleGuys(this,i);
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
	if(y<width-1)
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
				if(map[i].item==ITM_HUT || map[i].item==ITM_SHOP)
				{
					if(MGL_random(3)==0)
						BlowWigglySmoke(x+TILE_WIDTH*FIXAMT/2-(30<<FIXSHIFT),
						y+TILE_HEIGHT*FIXAMT/2-(10<<FIXSHIFT),80<<FIXSHIFT,FIXAMT*2);
				}
			}
			if(((player.worldNum==1 && player.levelNum==15) ||
				(player.worldNum==3 && player.levelNum==12)) && map[i].item==ITM_BIGROCKS &&
				world->terrain[map[i].floor].flags&(TF_WATER|TF_LAVA))	// river rapids
			{
				WaterRipple(x/FIXAMT+TILE_WIDTH/2,y/FIXAMT+TILE_HEIGHT/2-20,MGL_random(32*40));
			}
			if(world->terrain[map[i].floor].flags&(TF_LAVA))
				map[i].light=(char)MGL_random(9)-3;
		}
		if(((i+1)/width)!=(i/width))
		{
			x=0;
			y+=TILE_HEIGHT*FIXAMT;
		}
		else
			x+=TILE_WIDTH*FIXAMT;
	}

	if(player.worldNum==1 && player.levelNum==13 && player.templePuzzleDone==0)
	{
		TemplePuzzle(this);
	}
	if(player.worldNum==2 && player.levelNum==17)
	{
		LockedMazePuzzle(this);
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
					if(map[curx+cury*width].wall ||
						map[curx+cury*width].item>=MAX_SHOOTABLE_ITMS)	// there's a wall here, so opaque
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
						map[curx+cury*width].item>=MAX_SHOOTABLE_ITMS)	// there's a wall here, so opaque
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
	MakeSound(SND_ITEMDROP,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,500);
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

byte FindGuyCallback(int x,int y,int cx,int cy,int value,Map *map)
{
	Guy *g;

	if(map->map[x+y*map->width].item>=MAX_WALKABLE_ITMS)
		return 1;	// can't see through it

	if(map->map[x+y*map->width].wall)
		return 1;

	g=GetGuy(value);
	if(g && g->mapx==x && g->mapy==y)
		return 0;

	return 1;	// failed
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
						map[curx+cury*width].item>=MAX_WALKABLE_ITMS)	// there's a wall here, so opaque
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
						map[curx+cury*width].item>=MAX_WALKABLE_ITMS)	// there's a wall here, so opaque
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

byte Map::FindGuy(int x,int y,byte size,Guy *g)
{
	return TightLOS(x,y,size,g->ID,FindGuyCallback);
}

byte Map::FindGuy2(int x,int y,byte size,Guy *g)
{
	return LOS(x,y,size,g->ID,FindGuyCallback);
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
	if(player.gear&GEAR_LAMP)
		LOS(x,y,brt*2,brt*3,TempTorchCallback);
	else
		LOS(x,y,brt/4,brt,TempTorchCallback);
}

void Map::BrightTorch(int x,int y,char brt,byte size)
{
	LOS(x,y,size,brt,TempBrightCallback);
}

void Map::LightSpecial(int x,int y,char brt,byte size)
{
	LOS(x,y,size,brt,TorchCallback);
}

void Map::Render(world_t *world,int camX,int camY,byte flags)
{
	int i,j,endX,endY;

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
	endX=4;
	endY=6;

	if(player.levelNum==1 && player.worldNum==2)
	{
		endY=10;
		tileX=(camX/TILE_WIDTH)-8;
		scrX=-ofsX-TILE_WIDTH*8;
		endX=14;
	}
	for(i=tileX;i<tileX+(SCRWID/TILE_WIDTH+endX);i++)
	{
		scrY=-ofsY-TILE_HEIGHT;
		for(j=tileY;j<tileY+(SCRHEI/TILE_HEIGHT+endY);j++)
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
					RenderItem(scrX+camX+(TILE_WIDTH/2),scrY+camY+(TILE_HEIGHT/2),
						m->item,lite);

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
								WallDraw(scrX+camX,scrY+camY,m->wall+199,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
							else
								WallDraw(scrX+camX,scrY+camY,m->wall+199,m->floor,lites,
									DISPLAY_DRAWME|DISPLAY_WALLTILE);
					}
					// make wall tiles get drawn in sorted order unlike the floor tiles
					else
					{
						if(world->terrain[m->floor].flags&TF_TRANS)
							WallDraw(scrX+camX,scrY+camY,m->wall+199,m->floor,lites,
								DISPLAY_DRAWME|DISPLAY_WALLTILE|DISPLAY_TRANSTILE);
						else
							WallDraw(scrX+camX,scrY+camY,m->wall+199,m->floor,lites,
								DISPLAY_DRAWME|DISPLAY_WALLTILE);
					}
				}
				else
				{
					if(j<height-1 && map[i+(j+1)*width].wall)
					{
						if(i<width-1 && map[i+1+j*width].wall)
						{
							shdw=6;
						}
						else
						{
							if(i<width-1 && map[i+1+(j+1)*width].wall)
								shdw=4;
							else
								shdw=7;
						}
					}
					else
					{
						if(i<width-1)
						{
							if(map[i+1+(j)*width].wall)
							{
								if(j<height-1 && map[i+1+(j+1)*width].wall)
									shdw=1;
								else
									shdw=2;
							}
							else
							{
								if(j<height-1 && map[i+1+(j+1)*width].wall)
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
							(world->terrain[map[i+(j+1)*width].floor].flags&TF_TRANS) || !(flags&MAP_SHOWWALLS))
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
}

void ZapWall(Map *map,int x,int y,byte newFloor)
{
	byte flr,wall;

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
	byte flr;

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

void SpecialStepCheck(Map *map,int x,int y,Guy *me)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if((map->special[i].trigger&(TRG_STEP|TRG_ENEMYSTEP)) &&
			map->special[i].x==x && map->special[i].y==y)
		{
			if((me->type==MONS_BOUAPHA && (map->special[i].trigger&TRG_STEP)) ||
			   (me->type!=MONS_BOUAPHA && (map->special[i].trigger&TRG_ENEMYSTEP)))
				SpecialTakeEffect(map,&map->special[i],me);
		}
		else if((map->special[i].trigger&TRG_NEAR) && (me->type==MONS_BOUAPHA) &&
			(abs(map->special[i].x-x)+abs(map->special[i].y-y)<=map->special[i].trigValue))
		{
			SpecialTakeEffect(map,&map->special[i],me);
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
			SpecialTakeEffect(map,&map->special[i],NULL);
		}
}

void SpecialAnytimeCheck(Map *map)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
		if((map->special[i].trigger) &&
			(!(map->special[i].trigger&(TRG_SHOOT|TRG_STEP|TRG_ENEMYSTEP|TRG_NEAR|TRG_CHAIN))))
		{
			SpecialTakeEffect(map,&map->special[i],NULL);
		}
	if(player.worldNum==3 && player.levelNum==1 && player.levelsPassed>=8)
	{
		if(player.lunacyKey[3]==0)
		{
			player.lunacyKey[3]=1;	// since lunacy keys aren't used, this is just a handy place to track
									// whether or not this has happened
			ShakeScreen(60);
			MakeNormalSound(SND_BIGRUMBLE);
		}
		// make a bridge over the lava
		map->map[63+12*map->width].floor=35;
		map->map[64+12*map->width].floor=35;
		map->map[63+13*map->width].floor=35;
		map->map[64+13*map->width].floor=35;
		map->map[63+14*map->width].floor=35;
		map->map[64+14*map->width].floor=35;
		// and open the door
		map->map[61+8*map->width].item=80;
	}
}

void RenderSpecialXes(MGLDraw *mgl,Map *map,byte world)
{
	int i;
	int camx,camy;

	GetCamera(&camx,&camy);
	camx-=320;
	camy-=240;
	for(i=0;i<MAX_SPECIAL;i++)
		if(map->special[i].effect==SPC_GOTOMAP && PlayerPassedLevel(world,map->special[i].value))
			DrawRedX(map->special[i].x*TILE_WIDTH-camx+TILE_WIDTH/2,
					map->special[i].y*TILE_HEIGHT-camy+TILE_HEIGHT/2,mgl);
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
						SpecialTakeEffect(map,&map->special[k],NULL);
				}
		}
}

void SpecialTakeEffect(Map *map,special_t *spcl,Guy *victim)
{
	char s[64];
	// first check to be sure all conditions are met
	if(spcl->trigger&TRG_KILLALL)
	{
		if(MonsterExists(spcl->trigValue))
			return;	// they ain't all dead yet
	}
	if(spcl->trigger&TRG_HAVEBRAINS)
	{
		if(PlayerBrains()<spcl->trigValue)
			return;
	}
	if(spcl->trigger&TRG_KEYCHAINS)
	{
		if((!PlayerKeyChain(0)) ||
			(!PlayerKeyChain(1)) ||
			(!PlayerKeyChain(2)) ||
			(!PlayerKeyChain(3)))
			return;	// must have them all
	}
	if(spcl->trigger&TRG_PASSLEVELS)
	{
		if(PlayerLevelsPassed()<spcl->trigValue)
			return;
	}
	if(!(spcl->trigger&TRG_CHAIN))
		SpecialNeighborCheck(map,spcl);
	switch(spcl->effect)
	{
		case SPC_GOTOMAP:
			//if((player.levelPassed[player.worldNum][spcl->value]==1) &&
			//	player.worldNum==2 && (player.levelNum==8 || player.levelNum==10 || player.levelNum==12 || player.levelNum==14))
			//	return;	// can't enter a guardian level that you've passed
			MakeNormalSound(SND_GOTOMAP);
			SendMessageToGame(MSG_GOTOMAP,spcl->value);
			break;
		case SPC_EXIT:
			if(PlayerBrains()==totalBrains)
			{
				if(BattleMode())
				{
					if(WonTheBattle())
					{
						SendMessageToGame(MSG_WINLEVEL,0);
						MakeNormalSound(SND_WINLEVEL);
					}
					else
						return;
				}
				else
				{
					SendMessageToGame(MSG_WINLEVEL,0);
					MakeNormalSound(SND_WINLEVEL);
				}
			}
			else
				return;	// to avoid triggering a message or shutting it off if repeatable
			break;
		case SPC_ZAPWALL:
			if(map->map[spcl->effectX+spcl->effectY*map->width].wall)
			{
				MakeSound(SND_WALLDOWN,(spcl->effectX*TILE_WIDTH)<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
				ZapWall(map,spcl->effectX,spcl->effectY,spcl->value);
			}
			break;
		case SPC_RAISEWALL:
			if(!map->map[spcl->effectX+spcl->effectY*map->width].wall)
			{
				MakeSound(SND_WALLUP,(spcl->effectX*TILE_WIDTH)<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
				RaiseWall(map,spcl->effectX,spcl->effectY,spcl->value);
			}
			break;
		case SPC_TOGGLEWALL:
			if(map->map[spcl->effectX+spcl->effectY*map->width].wall)
			{
				MakeSound(SND_WALLDOWN,(spcl->effectX*TILE_WIDTH)<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
				// store what the wall looked like for later popping it back up
				spcl->value=map->map[spcl->effectX+spcl->effectY*map->width].wall;
				ZapWall(map,spcl->effectX,spcl->effectY,
					map->map[spcl->effectX+spcl->effectY*map->width].floor);
			}
			else
			{
				MakeSound(SND_WALLUP,(spcl->effectX*TILE_WIDTH)<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
				RaiseWall(map,spcl->effectX,spcl->effectY,spcl->value);
			}
			break;
		case SPC_LIGHT:
			MakeSound(SND_LIGHTSON,(spcl->effectX*TILE_WIDTH)<<FIXSHIFT,
						(spcl->effectY*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1300);
			map->LightSpecial(spcl->effectX,spcl->effectY,spcl->value,abs(spcl->value)/2);
			break;
		case SPC_TELEPORT:
			if(victim)
			{
				if(victim->type==MONS_BOUAPHA)
				{
					if(GetTportClock())
						return;	// can't teleport yet
				}
				MakeSound(SND_TELEPORT,victim->x,victim->y,SND_CUTOFF,1500);
				victim->x=(spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT;
				victim->y=(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT;
				victim->dx=0;
				victim->dy=0;
				if(victim->type==MONS_BOUAPHA)
				{
					PutCamera(victim->x,victim->y);
					UpdateCamera(victim->x,victim->y,victim->facing,map);
					SetTportClock(30);
				}
				map->map[spcl->effectX+spcl->effectY*map->width].templight=34;
				if(spcl->effectX>0)
					map->map[spcl->effectX-1+spcl->effectY*map->width].templight=20;
				if(spcl->effectX<map->width-1)
					map->map[spcl->effectX+1+spcl->effectY*map->width].templight=20;
				if(spcl->effectY>0)
					map->map[spcl->effectX+(spcl->effectY-1)*map->width].templight=20;
				if(spcl->effectY<map->height-1)
					map->map[spcl->effectX+(spcl->effectY+1)*map->width].templight=20;
			}
			break;
		case SPC_SUMMON:
			MakeSound(SND_TELEPORT,(spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,SND_CUTOFF,1500);
			AddGuy((spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					64*FIXAMT,spcl->value);
			break;
		case SPC_PICTURE:
			if(spcl->msg[0]=='~')
				map->SewerTrick(spcl->effectX,spcl->effectY);
			else
			{
				EnterPictureDisplay();
				MakeNormalSound(SND_MESSAGE);
				sprintf(s,"graphics/%s",spcl->msg);
				GetDisplayMGL()->LoadBMP(s);
			}
			break;
	}
	if(spcl->trigger&TRG_MESSAGE)
		NoRepeatNewMessage(spcl->msg,75);
	if(!(spcl->trigger&TRG_REPEATABLE))
		spcl->trigger=0;	// can't trigger anymore
}

byte VaultPuzzleCheck(Map *map)
{

	// X
	if(map->map[2+33*map->width].floor==65)
		return 0;
	if(map->map[4+33*map->width].floor==64)
		return 0;
	if(map->map[6+33*map->width].floor==65)
		return 0;
	if(map->map[2+35*map->width].floor==64)
		return 0;
	if(map->map[4+35*map->width].floor==65)
		return 0;
	if(map->map[6+35*map->width].floor==64)
		return 0;
	if(map->map[2+37*map->width].floor==65)
		return 0;
	if(map->map[4+37*map->width].floor==64)
		return 0;
	if(map->map[6+37*map->width].floor==65)
		return 0;

	// V
	if(map->map[9+33*map->width].floor==65)
		return 0;
	if(map->map[11+33*map->width].floor==64)
		return 0;
	if(map->map[13+33*map->width].floor==65)
		return 0;
	if(map->map[9+35*map->width].floor==65)
		return 0;
	if(map->map[11+35*map->width].floor==64)
		return 0;
	if(map->map[13+35*map->width].floor==65)
		return 0;
	if(map->map[9+37*map->width].floor==64)
		return 0;
	if(map->map[11+37*map->width].floor==65)
		return 0;
	if(map->map[13+37*map->width].floor==64)
		return 0;

	// I (the I is completely lenient about the corners in case you want to make it serif)
	if(map->map[18+33*map->width].floor==65)
		return 0;
	if(map->map[16+35*map->width].floor==64)
		return 0;
	if(map->map[18+35*map->width].floor==65)
		return 0;
	if(map->map[20+35*map->width].floor==64)
		return 0;
	if(map->map[18+37*map->width].floor==65)
		return 0;

	return 1;
}

void OpenVaultDoor(Map *map)
{
	int i;

	// lower the wall
	for(i=29;i<29+7;i++)
	{
		map->map[i+46*map->width].wall=0;
		map->map[i+46*map->width].floor=63;
		map->map[i+46*map->width].wall=0;
	}
	// kill the special too
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].x==32 && map->special[i].y==46)
			map->special[i].trigger=0;
	}
	player.vaultOpened=1;
}

int TotalBrains(void)
{
	return totalBrains;
}

void Cavernize(Map *map)
{
	byte result[]={188,187,185,194,186,189,193,197,184,192,190,198,191,195,196,199};
	byte lavaResult[]={0,18,19,27,20,34,28,30,17,25,33,31,26,29,32,35};
	byte v;
	int i,j;

	for(i=0;i<map->width;i++)
		for(j=0;j<map->height;j++)
		{
			if(map->map[i+j*map->width].wall>0 && map->map[i+j*map->width].floor>183)
			{
				// this tile wants to be cavernized
				v=0;
				if(i==0 || map->map[i-1+j*map->width].wall>0)
					v+=1;
				if(j==0 || map->map[i+(j-1)*map->width].wall>0)
					v+=2;
				if(i==map->width-1 || map->map[i+1+j*map->width].wall>0)
					v+=4;
				if(j==map->height-1 || map->map[i+(j+1)*map->width].wall>0)
					v+=8;
				map->map[i+j*map->width].floor=result[v];
				if(j<map->height-1 && map->map[i+(j+1)*map->width].floor>4
					&& map->map[i+(j+1)*map->width].floor<17 && map->map[i+j*map->width].wall==1)
					map->map[i+j*map->width].wall=2;	// give it a lava tint
			}
			if(map->map[i+j*map->width].floor>4 && map->map[i+j*map->width].floor<17)
			{
				if(MGL_random(6)>0)
					map->map[i+j*map->width].floor=MGL_random(4)+5;
				else
					map->map[i+j*map->width].floor=MGL_random(8)+9;
			}
			if(map->map[i+j*map->width].floor<5 || (map->map[i+j*map->width].floor>16 && map->map[i+j*map->width].floor<36))
			{
				v=0;
				if(i!=0 && map->map[i-1+j*map->width].floor>4 && map->map[i-1+j*map->width].floor<17)
					v+=1;
				if(j!=0 && map->map[i+(j-1)*map->width].floor>4 && map->map[i+(j-1)*map->width].floor<17)
					v+=2;
				if(i!=map->width-1 && map->map[i+1+(j)*map->width].floor>4 && map->map[i+1+(j)*map->width].floor<17)
					v+=4;
				if(j!=map->height-1 && map->map[i+(j+1)*map->width].floor>4 && map->map[i+(j+1)*map->width].floor<17)
					v+=8;
				if(v>0)
				{
					map->map[i+j*map->width].floor=lavaResult[v];
				}
				else
				{
					if(i!=0 && j!=0 && map->map[i-1+(j-1)*map->width].floor>4 && map->map[i-1+(j-1)*map->width].floor<17)
						map->map[i+j*map->width].floor=24;
					if(i!=0 && j!=map->height-1 && map->map[i-1+(j+1)*map->width].floor>4 && map->map[i-1+(j+1)*map->width].floor<17)
						map->map[i+j*map->width].floor=22;
					if(i!=map->width-1 && j!=0 && map->map[i+1+(j-1)*map->width].floor>4 && map->map[i+1+(j-1)*map->width].floor<17)
						map->map[i+j*map->width].floor=23;
					if(i!=map->width-1 && j!=map->height-1 && map->map[i+1+(j+1)*map->width].floor>4 && map->map[i+1+(j+1)*map->width].floor<17)
						map->map[i+j*map->width].floor=21;
				}
			}
		}
}

void GetRidOfGoodStuff(Map *map)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].item==ITM_COIN || map->map[i].item==ITM_BIGCOIN)
			map->map[i].item=0;
	}
}

byte TemplePuzzle(Map *map)
{
	byte rtPiece[25]={91, 92, 93, 94, 95,
		              111,112,113,114,115,
					  131,132,133,134,135,
					  151,152,153,154,155,
					  146,147,148,149,150};
	byte xPos[25]={7,8,9,10,11,
				   7,8,9,10,11,
				   7,8,9,10,11,
				   7,8,9,10,11,
				   7,8,9,10,11};
	byte yPos[25]={114,114,114,114,114,
				   115,115,115,115,115,
				   116,116,116,116,116,
				   117,117,117,117,117,
				   118,118,118,118,118};

	int i;
	byte correct;

	correct=0;
	for(i=0;i<25;i++)
	{
		if(map->map[xPos[i]+yPos[i]*map->width].floor==rtPiece[i])
		{
			correct++;
			GoldUpTile(rtPiece[i],1);
		}
		else
			GoldUpTile(rtPiece[i],-1);
	}

	if(correct==25 && player.templePuzzleDone==0)
	{
		player.templePuzzleDone=1;
		ZapWall(map,9,112,116);
		for(i=0;i<25;i++)
		{
			GoldUpTile(rtPiece[i],2);
			GoldUpTile(rtPiece[i],2);
			map->map[xPos[i]+yPos[i]*map->width].templight=32;
			curWorld.terrain[rtPiece[i]].flags|=TF_SOLID;
		}
	}
	return correct;
}

void ResetTemplePuzzle(Map *map)
{
	byte rtPiece[25]={91, 92, 93, 94, 95,
		              111,112,113,114,115,
					  131,132,133,134,135,
					  151,152,153,154,155,
					  146,147,148,149,150};
	byte xPos[25]={7,8,9,10,11,
				   7,8,9,10,11,
				   7,8,9,10,11,
				   7,8,9,10,11,
				   7,8,9,10,11};
	byte yPos[25]={114,114,114,114,114,
				   115,115,115,115,115,
				   116,116,116,116,116,
				   117,117,117,117,117,
				   118,118,118,118,118};

	int i,j;

	if(player.templePuzzleDone==0)
		return;
	else
	{
		for(i=2;i<17;i++)
			for(j=114;j<127;j++)
				map->map[i+j*map->width].floor=158;	// wipe it away

		for(i=0;i<25;i++)
		{
			GoldUpTile(rtPiece[i],2);
			GoldUpTile(rtPiece[i],2);
			map->map[xPos[i]+yPos[i]*map->width].floor=rtPiece[i];
			curWorld.terrain[rtPiece[i]].flags|=TF_SOLID;
		}
		ZapWall(map,9,112,116);
	}
}

void SwapMazeBlocks(Map *map,byte src,byte dst,byte wid,byte hei,byte mazewid,byte mazehei)
{
	int i,j,srcx,srcy,dstx,dsty;
	mapTile_t m;

	if(src==dst)
		return;

	srcx=(src%mazewid)*wid;
	srcy=(src/mazewid)*hei;
	dstx=(dst%mazewid)*wid;
	dsty=(dst/mazewid)*hei;

	for(i=0;i<wid;i++)
		for(j=0;j<hei;j++)
		{
			// swap the map tiles
			m=map->map[(srcx+i)+(srcy+j)*map->width];
			map->map[(srcx+i)+(srcy+j)*map->width]=map->map[(dstx+i)+(dsty+j)*map->width];
			map->map[(dstx+i)+(dsty+j)*map->width]=m;
		}

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].x>=srcx && map->special[i].y>=srcy &&
		   map->special[i].x<srcx+wid && map->special[i].y<srcy+hei)
		{
			map->special[i].x+=dstx-srcx;
			map->special[i].y+=dsty-srcy;
		}
		else if(map->special[i].x>=dstx && map->special[i].y>=dsty &&
		   map->special[i].x<dstx+wid && map->special[i].y<dsty+hei)
		{
			map->special[i].x+=srcx-dstx;
			map->special[i].y+=srcy-dsty;
		}

		if(map->special[i].effectX>=srcx && map->special[i].effectY>=srcy &&
		   map->special[i].effectX<srcx+wid && map->special[i].effectY<srcy+hei)
		{
			map->special[i].effectX+=dstx-srcx;
			map->special[i].effectY+=dsty-srcy;
		}
		else if(map->special[i].effectX>=dstx && map->special[i].effectY>=dsty &&
		   map->special[i].effectX<dstx+wid && map->special[i].effectY<dsty+hei)
		{
			map->special[i].effectX+=srcx-dstx;
			map->special[i].effectY+=srcy-dsty;
		}
	}
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].x>=srcx && map->badguy[i].y>=srcy &&
		   map->badguy[i].x<srcx+wid && map->badguy[i].y<srcy+hei)
		{
			map->badguy[i].x+=dstx-srcx;
			map->badguy[i].y+=dsty-srcy;
		}
		else if(map->badguy[i].x>=dstx && map->badguy[i].y>=dsty &&
		   map->badguy[i].x<dstx+wid && map->badguy[i].y<dsty+hei)
		{
			map->badguy[i].x+=srcx-dstx;
			map->badguy[i].y+=srcy-dsty;
		}
	}
}

void ConnectMazeCells(Map *map,byte tgt,byte tgt2,byte dir,byte *data,byte wid,byte hei,byte mazewid,byte mazehei)
{
	int i;

	switch(dir)
	{
		case 0:
			for(i=wid/2-2;i<wid/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+wid-1)+((tgt/mazewid)*hei+i)*map->width].item=0;
				map->map[((tgt2%mazewid)*wid+0)+((tgt2/mazewid)*hei+i)*map->width].item=0;
			}
			break;
		case 1:
			for(i=hei/2-2;i<hei/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+hei-1)*map->width].item=0;
				map->map[((tgt2%mazewid)*wid+i)+((tgt2/mazewid)*hei+0)*map->width].item=0;
			}
			break;
		case 2:
			for(i=wid/2-2;i<wid/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+0)+((tgt/mazewid)*hei+i)*map->width].item=0;
				map->map[((tgt2%mazewid)*wid+wid-1)+((tgt2/mazewid)*hei+i)*map->width].item=0;
			}
			break;
		case 3:
			for(i=hei/2-2;i<hei/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+0)*map->width].item=0;
				map->map[((tgt2%mazewid)*wid+i)+((tgt2/mazewid)*hei+hei-1)*map->width].item=0;
			}
			break;
	}

	for(i=0;i<mazewid*mazehei;i++)
		if(data[i]==data[tgt2])
			data[i]=data[tgt];
}

void ConnectMazeCells2(Map *map,byte tgt,byte tgt2,byte dir,byte *data,byte wid,byte hei,byte mazewid,byte mazehei,byte lock)
{
	int i;

	switch(dir)
	{
		case 0:
			for(i=wid/2-2;i<wid/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+wid-1)+((tgt/mazewid)*hei+i)*map->width].wall=0;
				map->map[((tgt%mazewid)*wid+wid-1)+((tgt/mazewid)*hei+i)*map->width].floor=(byte)MGL_random(3);
				map->map[((tgt2%mazewid)*wid+0)+((tgt2/mazewid)*hei+i)*map->width].wall=0;
				map->map[((tgt2%mazewid)*wid+0)+((tgt2/mazewid)*hei+i)*map->width].floor=(byte)MGL_random(3);
				if(lock)
					map->map[((tgt2%mazewid)*wid+0)+((tgt2/mazewid)*hei+i)*map->width].item=ITM_DOOR2+lock;
			}
			break;
		case 1:
			for(i=hei/2-2;i<hei/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+hei-1)*map->width].wall=0;
				map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+hei-1)*map->width].floor=(byte)MGL_random(3);
				map->map[((tgt2%mazewid)*wid+i)+((tgt2/mazewid)*hei+0)*map->width].wall=0;
				map->map[((tgt2%mazewid)*wid+i)+((tgt2/mazewid)*hei+0)*map->width].floor=(byte)MGL_random(3);
				if(lock)
					map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+hei-1)*map->width].item=ITM_DOOR1+lock;
			}
			break;
		case 2:
			for(i=wid/2-2;i<wid/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+0)+((tgt/mazewid)*hei+i)*map->width].wall=0;
				map->map[((tgt%mazewid)*wid+0)+((tgt/mazewid)*hei+i)*map->width].floor=(byte)MGL_random(3);
				map->map[((tgt2%mazewid)*wid+wid-1)+((tgt2/mazewid)*hei+i)*map->width].wall=0;
				map->map[((tgt2%mazewid)*wid+wid-1)+((tgt2/mazewid)*hei+i)*map->width].floor=(byte)MGL_random(3);
				if(lock)
					map->map[((tgt%mazewid)*wid+0)+((tgt/mazewid)*hei+i)*map->width].item=ITM_DOOR2+lock;
			}
			break;
		case 3:
			for(i=hei/2-2;i<hei/2+2;i++)
			{
				map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+0)*map->width].wall=0;
				map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+0)*map->width].floor=(byte)MGL_random(3);
				map->map[((tgt2%mazewid)*wid+i)+((tgt2/mazewid)*hei+hei-1)*map->width].wall=0;
				map->map[((tgt2%mazewid)*wid+i)+((tgt2/mazewid)*hei+hei-1)*map->width].floor=(byte)MGL_random(3);
				if(lock)
					map->map[((tgt%mazewid)*wid+i)+((tgt/mazewid)*hei+0)*map->width].item=ITM_DOOR1+lock;
			}
			break;
	}

	if(lock==0)
	{
		for(i=0;i<mazewid*mazehei;i++)
			if(data[i]==data[tgt2])
				data[i]=data[tgt];
	}
}

void CreateMaze(Map *map,byte wid,byte hei,byte mazewid,byte mazehei)
{
	int i;
	byte data[8*8];
	byte tgt,tgt2,ones,dir;

	// first we just jumble up which block is where
	for(i=0;i<mazewid*mazehei;i++)
	{
		tgt=(byte)MGL_random(mazewid*mazehei);	// which one to swap with
		SwapMazeBlocks(map,i,tgt,wid,hei,mazewid,mazehei);
		data[i]=i;
	}

	ones=1;

	while(ones<mazewid*mazehei)
	{
		tgt=(byte)MGL_random(mazewid*mazehei);	// pick one
		while(data[tgt]!=0)
			tgt=(byte)MGL_random(mazewid*mazehei);	// pick one
		dir=(byte)MGL_random(4);
		switch(dir)	// and pick a neighbor
		{
			case 0:
				if((tgt%mazewid)<mazewid-1 && data[tgt+1]!=0)
					tgt2=tgt+1;
				else
					tgt2=255;
				break;
			case 1:
				if((tgt/mazewid)<mazehei-1 && data[tgt+mazewid]!=0)
					tgt2=tgt+mazewid;
				else
					tgt2=255;
				break;
			case 2:
				if((tgt%mazewid)>0 && data[tgt-1]!=0)
					tgt2=tgt-1;
				else
					tgt2=255;
				break;
			case 3:
				if((tgt/mazewid)>0 && data[tgt-mazewid]!=0)
					tgt2=tgt-mazewid;
				else
					tgt2=255;
				break;
		}
		if(tgt2!=255)	// found a usable neighbor
			ConnectMazeCells(map,tgt,tgt2,dir,data,wid,hei,mazewid,mazehei);

		// now calculate how many are connected
		ones=0;
		for(i=0;i<mazewid*mazehei;i++)
			if(data[i]==data[0])
				ones++;
	}
}

void PlaceMazeKey(byte seg,byte itm,byte *data,byte wid,byte hei,byte mazewid,byte mazehei,Map *map)
{
	int n,x,y,i,j;

	while(1)
	{
		n=MGL_random(mazewid*mazehei);
		if(data[n]==seg)
		{
			x=(n%mazewid)*wid;
			y=(n/mazewid)*hei;

			while(1)
			{
				i=x+MGL_random(wid);
				j=y+MGL_random(hei);

				if(map->map[i+j*map->width].item==ITM_NONE && map->map[i+j*map->width].floor<3)
				{
					if(itm==0)
					{
						for(n=0;n<MAX_MAPMONS;n++)
							if(map->badguy[n].type==MONS_BOUAPHA)
							{
								map->badguy[n].x=i;
								map->badguy[n].y=j;	// place player instead
							}
					}
					else
						map->map[i+j*map->width].item=itm;
					return;
				}
			}
		}
	}
}

void CreateLockedMaze(Map *map,byte wid,byte hei,byte mazewid,byte mazehei)
{
	int i,j,segpos,workseg;
	byte data[16*16],data2[16*16];
	byte tgt,tgt2,segments,dir;

	// first we just jumble up which block is where
	for(i=0;i<mazewid*mazehei;i++)
	{
		tgt=(byte)MGL_random(mazewid*mazehei);	// which one to swap with
		SwapMazeBlocks(map,i,tgt,wid,hei,mazewid,mazehei);
		data[i]=i;
	}

	segments=100;

	workseg=0;
	while(segments>3)
	{
		tgt=(byte)MGL_random(mazewid*mazehei);	// pick one
		while(data[tgt]!=workseg)
			tgt=(byte)MGL_random(mazewid*mazehei);	// pick one
		workseg++;
		if(workseg==3)
			workseg=0;
		dir=(byte)MGL_random(4);
		switch(dir)	// and pick a neighbor
		{
			case 0:
				if((tgt%mazewid)<mazewid-1 && data[tgt+1]>2)
					tgt2=tgt+1;
				else
					tgt2=255;
				break;
			case 1:
				if((tgt/mazewid)<mazehei-1 && data[tgt+mazewid]>2)
					tgt2=tgt+mazewid;
				else
					tgt2=255;
				break;
			case 2:
				if((tgt%mazewid)>0 && data[tgt-1]>2)
					tgt2=tgt-1;
				else
					tgt2=255;
				break;
			case 3:
				if((tgt/mazewid)>0 && data[tgt-mazewid]>2)
					tgt2=tgt-mazewid;
				else
					tgt2=255;
				break;
		}
		if(tgt2!=255)	// found a usable neighbor
			ConnectMazeCells2(map,tgt,tgt2,dir,data,wid,hei,mazewid,mazehei,0);

		// now calculate how many are connected
		segments=0;
		segpos=0;
		for(i=0;i<mazewid*mazehei;i++)
			data2[i]=255;
		for(i=0;i<mazewid*mazehei;i++)
		{
			for(j=0;j<mazewid*mazehei;j++)
				if(data[i]==data2[j])
					break;
			if(j>=mazewid*mazehei)	// the number wasn't found
			{
				data2[segpos++]=data[i];	// so add it to the list
			}
		}
		segments=segpos;
	}
	// now the maze has 3 segments
	while(1)
	{
		tgt=(byte)MGL_random(mazewid*mazehei);
		while(data[tgt]!=data2[0])
			tgt=(byte)MGL_random(mazewid*mazehei);	// loop until you find one of the cells from the 1st segment
		dir=(byte)MGL_random(4);
		switch(dir)	// and pick a neighbor
		{
			case 0:
				if((tgt%mazewid)<mazewid-1 && data[tgt+1]==data2[1])
					tgt2=tgt+1;
				else
					tgt2=255;
				break;
			case 1:
				if((tgt/mazewid)<mazehei-1 && data[tgt+mazewid]==data2[1])
					tgt2=tgt+mazewid;
				else
					tgt2=255;
				break;
			case 2:
				if((tgt%mazewid)>0 && data[tgt-1]==data2[1])
					tgt2=tgt-1;
				else
					tgt2=255;
				break;
			case 3:
				if((tgt/mazewid)>0 && data[tgt-mazewid]==data2[1])
					tgt2=tgt-mazewid;
				else
					tgt2=255;
				break;
		}
		if(tgt2!=255)	// found a usable neighbor, that is in segment 2
		{
			ConnectMazeCells2(map,tgt,tgt2,dir,data,wid,hei,mazewid,mazehei,1);
			break;
		}
	}

	while(1)
	{
		tgt=(byte)MGL_random(mazewid*mazehei);
		while(data[tgt]!=data2[2])
			tgt=(byte)MGL_random(mazewid*mazehei);	// loop until you find one of the cells from the 3rd segment
		dir=(byte)MGL_random(4);
		switch(dir)	// and pick a neighbor
		{
			case 0:
				if((tgt%mazewid)<mazewid-1 && (data[tgt+1]==data2[1] || data[tgt+1]==data2[0]))
					tgt2=tgt+1;
				else
					tgt2=255;
				break;
			case 1:
				if((tgt/mazewid)<mazehei-1 && (data[tgt+mazewid]==data2[1] || data[tgt+mazewid]==data2[0]))
					tgt2=tgt+mazewid;
				else
					tgt2=255;
				break;
			case 2:
				if((tgt%mazewid)>0 && (data[tgt-1]==data2[1] || data[tgt-1]==data2[0]))
					tgt2=tgt-1;
				else
					tgt2=255;
				break;
			case 3:
				if((tgt/mazewid)>0 && (data[tgt-mazewid]==data2[1] || data[tgt-mazewid]==data2[0]))
					tgt2=tgt-mazewid;
				else
					tgt2=255;
				break;
		}
		if(tgt2!=255)	// found a usable neighbor, that is in segment 1 or 2
		{
			ConnectMazeCells2(map,tgt,tgt2,dir,data,wid,hei,mazewid,mazehei,2);
			break;
		}
	}

	if(data[tgt2]==data2[0])	// the green key belongs in segment 1, like the red key
	{
		PlaceMazeKey(data2[0],ITM_KEYG,data,wid,hei,mazewid,mazehei,map);
	}
	else	// green key goes in segment 2
	{
		PlaceMazeKey(data2[1],ITM_KEYG,data,wid,hei,mazewid,mazehei,map);
	}

	// and finally, red key in segment 1
	PlaceMazeKey(data2[0],ITM_KEYR,data,wid,hei,mazewid,mazehei,map);
	// finally finally place player in segment 1
	PlaceMazeKey(data2[0],0,data,wid,hei,mazewid,mazehei,map);
}

void Map::FindNearBrain(int myx,int myy)
{
	int i,j,x,y;

	brainX=255;
	i=0;
	j=255*255+255*255;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			if(map[i].item==ITM_BRAIN)
			{
				if(((myx-x)*(myx-x)+(myy-y)*(myy-y))<j)
				{
					brainX=x;
					brainY=y;
					j=(myx-brainX)*(myx-brainX)+(myy-brainY)*(myy-brainY);
				}
			}
			i++;
		}
	}
}

void LockedMazePuzzle(Map *map)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
		if(map->map[i].floor==65)	// must only be 64's
			return;

	// if you got here, the 'puzzle' is solved
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].trigger==(TRG_STEP|TRG_MESSAGE|TRG_REPEATABLE) && map->special[i].effect==SPC_NONE)
		{
			map->special[i].trigger=TRG_STEP|TRG_MESSAGE;
			map->special[i].effect=SPC_ZAPWALL;
			strcpy(map->special[i].msg,"Ka-chunk!");
			NewMessage("Something clanked...",60);
		}
	}
}

void Map::SewerTrick(int x,int y)
{
	// add flushing noise or flooding noise depending
	MakeSound(SND_FLUSH,x*TILE_WIDTH*FIXAMT,y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,50);
	if(map[x+y*width].wall==61)	// this is currently wet, so dry it
	{
		ZapWall(this,x,y,87);		// 62 wall, 87 roof = dry
		RaiseWall(this,x,y,62);

		RaiseWall(this,x,y+1,1);
		ZapWall(this,x,y+1,85);		// 85 = begin draining
	}
	else
	{
		ZapWall(this,x,y,86);		// 61 wall, 86 roof = wet
		RaiseWall(this,x,y,61);

		RaiseWall(this,x,y+1,1);
		ZapWall(this,x,y+1,135);	// 135 = begin filling
	}
}

byte SmashballCheck(Map *map)
{
	int i,j;

	if(!map->map[23+50*map->width].wall)
		return 0;	// already did it

	for(i=1;i<=22;i++)
		for(j=46;j<=53;j++)
			if(map->map[i+j*map->width].wall)
				return 0;	// sorry dude

	ZapWall(map,23,49,0);
	MakeNormalSound(SND_PURCHASE);
	NewMessage("Cleared the board!",60);
	return 1;
}

void ToggleLightThing(Map *map,int x,int y)
{
	if(x<71)
		x+=8;
	if(x>77)
		x-=8;
	if(y<82)
		y+=8;
	if(y>88)
		y-=8;

	map->map[x+y*map->width].floor=(72+73)-map->map[x+y*map->width].floor;
	map->map[x+y*map->width].templight=16;
}

void LightsOutPuzzle(Map *map,int x,int y)
{
	int i,j;
	byte on;

	ToggleLightThing(map,x,y);
	ToggleLightThing(map,x-2,y);
	ToggleLightThing(map,x+2,y);
	ToggleLightThing(map,x,y-2);
	ToggleLightThing(map,x,y+2);

	on=0;
	for(i=71;i<=77;i+=2)
		for(j=82;j<=88;j+=2)
			if(map->map[i+j*map->width].floor==72)
				on++;

	// if you got here, all lights are on
	if(on==16 && map->map[73+79*map->width].wall)
	{
		ZapWall(map,73,79,0);
		MakeNormalSound(SND_PURCHASE);
		NewMessage("Nice work!",60);
	}

	if(on==0 && map->map[61+71*map->width].wall)
	{
		ZapWall(map,61,71,0);
		MakeNormalSound(SND_PURCHASE);
		NewMessage("Hmm...",60);
	}
}

void OctoBossDeath(Map *map)
{
	int i,j;

	for(j=32;j<=50;j++)
		for(i=27;i<=38;i++)
		{
			map->map[i+j*map->width].floor=map->map[i+51*map->width].floor;
		}
	NewMessage("Weird, the bridge came back!",60);
}

byte MySpecialType(byte type,Map *map,int mapx,int mapy)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].x==mapx && map->special[i].y==mapy)
		{
			if((map->special[i].trigger&(TRG_STEP|TRG_NEAR)) && type==MONS_BOUAPHA)
				return 1;	// it's cool if you're the kid and it's a player step
			if((map->special[i].trigger&(TRG_ENEMYSTEP)) && (type!=MONS_BOUAPHA && type!=MONS_GOLEM))
				return 1;	// it's cool if you're an enemy and it's an enemy step

			if(map->special[i].trigger==0)
				return 1;	// dead ones are always cool

			return 0;	// if neither case, it's not cool
		}
	}
	return 1;
}
