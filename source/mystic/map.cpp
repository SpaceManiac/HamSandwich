#include "map.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "guy.h"
#include "water.h"
#include "challenge.h"
#include "achieves.h"
#include "spell.h"

int totalBrains;
static world_t *world;
byte brainX,brainY;
byte outXes = 0;
byte lastSpecialShown = 33;
bool vault2Opened;
byte guestProgress,libraryProgress,batsProgress;
byte lightSpeed;

Map::Map(SDL_RWops *f)
{
	SDL_RWread(f,&width,1,sizeof(int));
	SDL_RWread(f,&height,1,sizeof(int));

	SDL_RWread(f,name,32,sizeof(char));
	SDL_RWread(f,badguy,MAX_MAPMONS,sizeof(mapBadguy_t));
	SDL_RWread(f,special,MAX_SPECIAL,sizeof(special_t));
	SDL_RWread(f,&song,1,1);
	SDL_RWread(f,&flags,1,1);

	map=(mapTile_t *)calloc(sizeof(mapTile_t)*width*height,1);

	SDL_RWread(f,map,width*height,sizeof(mapTile_t));
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
	map = nullptr;
}

byte Map::Save(SDL_RWops *f)
{
	SDL_RWwrite(f,&width,1,sizeof(int));
	SDL_RWwrite(f,&height,1,sizeof(int));
	SDL_RWwrite(f,name,32,sizeof(char));
	SDL_RWwrite(f,badguy,MAX_MAPMONS,sizeof(mapBadguy_t));
	SDL_RWwrite(f,special,MAX_SPECIAL,sizeof(special_t));
	SDL_RWwrite(f,&song,1,1);
	SDL_RWwrite(f,&flags,1,1);

	SDL_RWwrite(f,map,width*height,sizeof(mapTile_t));
	return 1;
}

void Map::Init(world_t *wrld)
{
	int i;
	byte s;

	lightSpeed = 0;
	world = wrld;
	outXes = 0;
	lastSpecialShown = 33;
	totalBrains=0;
	SetChallengeCrystals(0);
	vault2Opened = false;
	guestProgress = 0;
	libraryProgress = 0;
	batsProgress = 0;

	for(i=0;i<width*height;i++)
	{
		if(map[i].item==ITM_BRAIN)
			totalBrains++;
		if(map[i].item==ITM_SWORD1 && PlayerHasSwordPiece(0))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_SWORD2 && PlayerHasSwordPiece(1))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_SWORD3 && PlayerHasSwordPiece(2))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_SWORD4 && PlayerHasSwordPiece(3))	// keychains only appear once
			map[i].item=0;
		if(map[i].item==ITM_SPELLBOOK && PlayerHasSpell())
			map[i].item=0;

		map[i].templight=-32;	// make it all black so it'll fade in
		if(map[i].item==ITM_SKLDOOR && player.worldNum==2 && PlayerPassedLevel(2,6))
			map[i].item=ITM_SKLDOOR2;

		if(map[i].item==ITM_SPELLBOOK)
		{
			s=SpellBookForThisLevel(player.levelNum,player.worldNum);
			if(player.gotSpell[s+(player.worldNum>1)*10])
				map[i].item=0;
			if(player.worldNum==2 && player.levelNum==16 && player.vaultOpened && !Challenging())
				map[i].item=0;	// take it away if you've opened the vault before
		}
		if(map[i].item>=ITM_SWORD1 && map[i].item<=ITM_SWORD4)
		{
			if(player.swordPiece[map[i].item-ITM_SWORD1])
				map[i].item=0;
		}
		if(map[i].item==ITM_FAIRYBELL)
		{
			byte fairy = FairyForThisLevel(player.levelNum + player.worldNum * 50);
			if (fairy && ((player.haveFairy & (1 << (fairy - 1))) || (player.chaseFairy & (1 << (fairy - 1)))))
			{
				map[i].item = 0;
			}
		}
		if (map[i].item == ITM_RUNEPOUCH)
			if (GotRunePouchInLevel(player.worldNum, player.levelNum))
				map[i].item = 0;
		if (map[i].item == ITM_SKILLSHARD)
			if (GotSkillShardInLevel(player.worldNum, player.levelNum))
				map[i].item = 0;
		if(player.worldNum==2 && player.levelNum==16 && player.vaultOpened && map[i].item==ITM_COIN)
			map[i].item=0;	// if you got the spellbook, take the coins away!
		if(map[i].item>=ITM_LETTERM && map[i].item<=ITM_LETTERC && !Challenging())
			map[i].item=0;	// remove MYSTIC letters when not in challenge mode
		if (map[i].item ==ITM_SILENTRUNE && GotRuneInLevel(player.worldNum,player.levelNum))
			map[i].item = 0;	// if you got the rune, then remove it
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
				special[i].value==12 || special[i].value==14) && PlayerPassedLevel(2,special[i].value))
			{
				special[i].effect=0;	// disable this special
			}
	}
	if (player.worldNum == 0 && player.levelNum == 20)
		SwampStewSetup(this);
	if (player.worldNum == 1 && player.levelNum == 3)
		MushAidPuzzleReset(this);
	if (player.worldNum == 1 && player.levelNum == 5 && !ClassicMode())
		AbandonedVillagePuzzleReset(this);
	if (player.worldNum == 1 && player.levelNum == 11 && !ClassicMode())
		AmongHedgesPuzzleInit(this);
	if (player.worldNum == 2 && player.levelNum == 24)
		OrderUpSetup(this);

	if(player.levelNum==19 && player.worldNum==2)
		totalBrains=2;
	if(player.levelNum==12 && player.worldNum==1 && !VeryClassicMode())
		totalBrains=3;
	if (player.levelNum == 20 && player.worldNum == 0)
		totalBrains = 1;	// get a brain for cooking, even if you fail
	if (player.levelNum == 19 && player.worldNum == 1)
		HauntedWoodsPuzzleInit(this);
	if (player.levelNum == 7 && player.worldNum == 2 && !ClassicMode())
		DeepEndPuzzleInit();

	// pop in all the badguys
	i=BattleMode();
	if(i==0)
		AddMapGuys(this);
	else
		AddBattleGuys(this,i);

	if (player.levelNum == 19 && player.worldNum == 3)
		HorkBoxInit(this);
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

	if(lightSpeed>0)
		lightSpeed--;
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
			if (lightSpeed > 0 || (player.worldNum == 3 && player.levelNum==11))
			{
				if (map[i].templight > map[i].light)
				{
					map[i].templight -= 2;
					if (map[i].templight < map[i].light)
						map[i].templight = map[i].light;
				}
				else if (map[i].templight < map[i].light)
				{
					map[i].templight+=2;
					if (map[i].templight > map[i].light)
						map[i].templight = map[i].light;
				}
			}
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
						BlowWigglySmoke(x+TILE_WIDTH*FIXAMT/2-(30*FIXAMT),
						y+TILE_HEIGHT*FIXAMT/2-(10*FIXAMT),80*FIXAMT,FIXAMT*2);
				}
			}
			if (player.worldNum == 2 && player.levelNum == 11 && !ClassicMode())
			{
				if (map[i].floor != 35 && (i / width) > 18 && (i / width) < 116)
				{
					if (i > 0 && map[i - 1].floor == 35)	// the leftmost tile of the row
					{
						int j = i;
						while (map[j].floor != 35) j++;
						map[j - 1].light = map[i].light;	// get the light from the far right of this row
					}
					else if (i > 0 && map[i - 1].floor != 35)	// any other tile
					{
						map[i - 1].light = map[i].light;
					}
				}
			}
			if(((player.worldNum==1 && player.levelNum==15) ||
				(player.worldNum==3 && player.levelNum==12)) && (map[i].item==ITM_BIGROCKS || map[i].item==ITM_LAVAROCK) &&
				world->terrain[map[i].floor].flags&(TF_WATER|TF_LAVA))	// river rapids
			{
				WaterRipple(x/FIXAMT+TILE_WIDTH/2-10+Random(20), y / FIXAMT + TILE_HEIGHT / 2 - Random(20), MGL_random(32 * 40));
			}
			if (player.worldNum == 3 && world->terrain[map[i].floor].flags & (TF_LAVA))
			{
				map[i].light = (char)MGL_random(9) - 3;
				if (Random(3000) == 0)
				{
					int xx, yy;
					xx = x / FIXAMT + Random(TILE_WIDTH);
					yy = y / FIXAMT + Random(TILE_HEIGHT);
					WaterRipple(xx,yy, Random(800)+1500);
					BlowWigglySmoke(xx*FIXAMT,yy*FIXAMT, 0, 8);
				}
			}
			else if (player.worldNum == 0 && player.levelNum == 20)
			{
				if (Random(300) == 0)
				{
					int xx, yy;
					xx = x / FIXAMT + Random(TILE_WIDTH);
					yy = y / FIXAMT + Random(TILE_HEIGHT);
					WaterRipple(xx, yy, Random(800) + 500);
				}
			}
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
		TemplePuzzle(this);
	if(player.worldNum==2 && player.levelNum==17)
		LockedMazePuzzle(this);
	if (player.worldNum == 0 && player.levelNum == 20)
		SwampUpdate(this);
	if (player.worldNum == 1 && player.levelNum == 11)
		AmongHedgesPuzzleUpdate(this);
	if (player.worldNum == 2 && player.levelNum == 24)
		OrderUpUpdate(this);

	if(timeToAnim==2)
		timeToAnim=0;
}

void Map::FillRect(int x, int y, int x2, int y2, byte flr, byte wall)
{
	for(int i=x;i<=x2;i++)
		for (int j = y; j <= y2; j++)
		{
			if (i >= 0 && i < width && j >= 0 && j < height)
			{
				map[i + j * width].floor = flr;
				map[i + j * width].wall = wall;
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
	MakeSound(SND_ITEMDROP,(x*TILE_WIDTH)*FIXAMT,(y*TILE_HEIGHT)*FIXAMT,SND_CUTOFF,500);
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

	camX-=HALFWID;
	camY-=HALFHEI;

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
				if (flags & MAP_SHOWITEMS)
					RenderItem(scrX + camX + (TILE_WIDTH / 2), scrY + camY + (TILE_HEIGHT / 2),
						m->item, lite);
				else if (m->item)
					RenderItem(scrX + camX + (TILE_WIDTH / 2), scrY + camY + (TILE_HEIGHT / 2),
						ITM_BIGCOIN, lite);

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
}

void ChangeFloor(Map* map, int x, int y, byte newFloor)
{
	byte flr, wall;

	// store these for looking for neighbors
	flr = map->map[x + y * map->width].floor;
	wall = map->map[x + y * map->width].wall;

	// turn this square into the new floor. Ignore the wall, keep it the same
	map->map[x + y * map->width].floor = newFloor;

	// recurse to all neighbors that match
	if (x > 0 && map->map[x - 1 + y * map->width].wall == wall &&
		map->map[x - 1 + y * map->width].floor == flr)
		ChangeFloor(map, x - 1, y, newFloor);
	if (x < map->width - 1 && map->map[x + 1 + y * map->width].wall == wall &&
		map->map[x + 1 + y * map->width].floor == flr)
		ChangeFloor(map, x + 1, y, newFloor);
	if (y > 0 && map->map[x + (y - 1) * map->width].wall == wall &&
		map->map[x + (y - 1) * map->width].floor == flr)
		ChangeFloor(map, x, y - 1, newFloor);
	if (y < map->height - 1 && map->map[x + (y + 1) * map->width].wall == wall &&
		map->map[x + (y + 1) * map->width].floor == flr)
		ChangeFloor(map, x, y + 1, newFloor);
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
	if (me->type == MONS_GHOSTSPITTER)
		return;	// ghost spitters can't step on triggers

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if((map->special[i].trigger&(TRG_STEP|TRG_ENEMYSTEP)) &&
			map->special[i].x==x && map->special[i].y==y)
		{
			if((me->type==MONS_BOUAPHA && (map->special[i].trigger&TRG_STEP)) ||
			   (me->type!=MONS_BOUAPHA && me->type!=MONS_FAIRY2 && !(monsType[me->type].flags&MF_GOODGUY) && (map->special[i].trigger & TRG_ENEMYSTEP)))
				SpecialTakeEffect(map,&map->special[i],me);
		}
		else if((map->special[i].trigger&TRG_NEAR) && (me->type==MONS_BOUAPHA) &&
			(abs(map->special[i].x-x)+abs(map->special[i].y-y)<=map->special[i].trigValue))
		{
			SpecialTakeEffect(map,&map->special[i],me);
		}
	}
}

void SpecialShootCheck(Map *map,int x,int y,bool justThisTile)
{
	int i;

	if (justThisTile)
	{
		for (i = 0; i < MAX_SPECIAL; i++)
			if ((map->special[i].trigger & TRG_SHOOT) &&
				map->special[i].x == x && map->special[i].y == y)
			{
				SpecialTakeEffect(map, &map->special[i], NULL);
				if (player.worldNum == 3 && player.levelNum == 10)
					BatsPuzzle(map, map->special[i].x, map->special[i].y);
			}
		if (player.worldNum == 1 && player.levelNum == 11)
			AmongHedgesShootCheck(map, x, y);
	}
	else
	{
		for (i = 0; i < MAX_SPECIAL; i++)
			if ((map->special[i].trigger & TRG_SHOOT) &&
				map->special[i].x >= x - 1 && map->special[i].y >= y - 1 &&
				map->special[i].x <= x + 1 && map->special[i].y <= y + 1)
			{
				SpecialTakeEffect(map, &map->special[i], NULL);
				if (player.worldNum == 3 && player.levelNum == 10)
					BatsPuzzle(map, map->special[i].x, map->special[i].y);
			}
		if (player.worldNum == 1 && player.levelNum == 11)
			AmongHedgesShootCheck(map, x, y);
	}
}

void SpecialKillCheck(Map* map,byte type)
{
	int i;

	for (i = 0; i < MAX_SPECIAL; i++)
		if ((map->special[i].trigger & TRG_KILLONE) &&
			map->special[i].trigValue==type)
		{
			if (type == MONS_HUGEBAT2 && batsProgress < 3)
				continue;	// killing huge bat 2 only counts if you have done all 3 mutagens
			SpecialTakeEffect(map, &map->special[i], NULL);
		}

	if (player.worldNum == 2 && player.levelNum == 7)
		DeepEndPuzzleKill(map, type);
}

void SpecialOuchCheck(Map* map, byte type)
{
	int i;

	for (i = 0; i < MAX_SPECIAL; i++)
		if ((map->special[i].trigger & TRG_GETHURT) &&
			map->special[i].trigValue == type)
		{
			SpecialTakeEffect(map, &map->special[i], NULL);
		}
}

void SpecialAnytimeCheck(Map *map)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
		if((map->special[i].trigger) &&
			(!(map->special[i].trigger&(TRG_SHOOT|TRG_STEP|TRG_ENEMYSTEP|TRG_NEAR|TRG_CHAIN|TRG_KILLONE|TRG_GETHURT))))
		{
			SpecialTakeEffect(map,&map->special[i],NULL);
		}
	if(player.worldNum==3 && player.levelNum==1 && (player.levelsPassed>=4+VeryClassicMode()?0:4))
	{
		if(player.bobbyDoorOpen==0)
		{
			player.bobbyDoorOpen=1;
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
	camx-=HALFWID;
	camy-=HALFHEI;
	Guy* g = GetGoodguy();

	bool outOfRange = true;
	byte closest = 255;
	int squareDist = 999 * 999;
	for (i = 0; i < MAX_SPECIAL; i++)
	{
		int cx = map->special[i].x * TILE_WIDTH - camx + TILE_WIDTH / 2;
		int cy = map->special[i].y * TILE_HEIGHT - camy + TILE_HEIGHT / 2;
		if(map->special[i].effect == SPC_GOTOMAP)
		{
			if(ClassicMode() && PlayerPassedLevel(world, map->special[i].value))
				DrawRedX(cx,cy, mgl);

			if(!ClassicMode() && map->special[i].value<MAX_MAPS && g)
			{
				int myDist = (abs(g->x / (TILE_WIDTH * FIXAMT) - map->special[i].x)* abs(g->x / (TILE_WIDTH * FIXAMT) - map->special[i].x)) +
					(abs(g->y / (TILE_HEIGHT * FIXAMT) - map->special[i].y)* abs(g->y / (TILE_HEIGHT * FIXAMT) - map->special[i].y));

				if (abs(g->x / (TILE_WIDTH * FIXAMT) - map->special[i].x) <= 6 &&
					abs(g->y / (TILE_HEIGHT * FIXAMT) - map->special[i].y) <= 6)	// when player is close, add details
				{
					outOfRange = false;
					if (closest == 255 || myDist < squareDist)
					{
						closest = (byte)i;
						squareDist = myDist;
					}
				}
			}
		}
	}
	if (outOfRange)
	{
		if (outXes > 0)
			outXes--;
		else
			lastSpecialShown = 33;	// after they full recede, stop showing it
	}
	else if(closest!=255)
	{
		if (lastSpecialShown != closest)
		{
			lastSpecialShown = closest;
			outXes = 0;
		}
		else if (outXes < 40)
		{
			outXes += 2;
			if (outXes > 40) outXes = 40;
		}
	}
	if (!ClassicMode() && lastSpecialShown < 33)
	{
		byte mNum = map->special[lastSpecialShown].value;
		int cx = map->special[lastSpecialShown].x * TILE_WIDTH - camx + TILE_WIDTH / 2;
		int cy = map->special[lastSpecialShown].y * TILE_HEIGHT - camy + TILE_HEIGHT / 2;
		Map* m = curWorld.map[mNum];
		if (PlayerPassedLevel(world, mNum))
		{
			byte angList[] = { 96, 32, 0, 128 };
			byte ang=0;

			if (m->contentFlags & LP_GOTFAIRY)
			{
				for(int xx=cx-1;xx<=cx+1;xx++)
					for (int yy = cy - 1; yy <= cy + 1; yy++)
					{
						if(xx!=cx && yy!=cy)
							GetItemSprite(173)->DrawBright(xx - 12 + Cosine(angList[ang]) * outXes / FIXAMT, yy + 16 + Sine(angList[ang]) * outXes / FIXAMT, mgl, -31);
					}
				if (GotFairyBellInLevel(world, mNum))
					GetItemSprite(173)->DrawBright(cx - 12 + Cosine(angList[ang]) * outXes / FIXAMT, cy + 16 + Sine(angList[ang]) * outXes / FIXAMT, mgl, 0);

				ang++;
			}
			if (m->contentFlags & LP_GOTRUNE)
			{
				for (int xx = cx - 1; xx <= cx + 1; xx++)
					for (int yy = cy - 1; yy <= cy + 1; yy++)
					{
						char b = -31;
						if (xx!=cx && yy!=cy)
							GetItemSprite(281)->DrawBright(xx + Cosine(angList[ang]) * outXes / FIXAMT, yy + 8 + Sine(angList[ang]) * outXes / FIXAMT, mgl, -31);
					}
				if (GotRuneInLevel(world, mNum))
					GetItemSprite(281)->DrawBright(cx + Cosine(angList[ang]) * outXes / FIXAMT, cy + 8 + Sine(angList[ang]) * outXes / FIXAMT, mgl, 0);
				ang++;
			}
			if (m->contentFlags & LP_GOTSPELL)
			{
				for (int xx = cx - 1; xx <= cx + 1; xx++)
					for (int yy = cy - 1; yy <= cy + 1; yy++)
					{
						if (xx!=cx && yy!=cy)
							GetItemSprite(43)->DrawBright(xx + Cosine(angList[ang]) * outXes / FIXAMT, yy + 12 + Sine(angList[ang]) * outXes / FIXAMT, mgl, -31);
					}
				if (GotSpellInLevel(world, mNum))
					GetItemSprite(43)->DrawBright(cx + Cosine(angList[ang]) * outXes / FIXAMT, cy + 12 + Sine(angList[ang]) * outXes / FIXAMT, mgl, 0);
				ang++;
			}
			if (m->contentFlags & LP_GOTSWORD)
			{
				for (int xx = cx - 1; xx <= cx + 1; xx++)
					for (int yy = cy - 1; yy <= cy + 1; yy++)
					{
						if (xx != cx && yy != cy)
							GetItemSprite(131)->DrawBright(xx + Cosine(angList[ang]) * outXes / FIXAMT, yy + 8 + Sine(angList[ang]) * outXes / FIXAMT, mgl, -31);
					}
				if (GotSwordInLevel(world, mNum))
					GetItemSprite(131)->DrawBright(cx + Cosine(angList[ang]) * outXes / FIXAMT, cy + 8 + Sine(angList[ang]) * outXes / FIXAMT, mgl, 0);
				ang++;
			}
			if (m->contentFlags & LP_GOTRUNEPOUCH)
			{
				for (int xx = cx - 1; xx <= cx + 1; xx++)
					for (int yy = cy - 1; yy <= cy + 1; yy++)
					{
						if (xx != cx && yy != cy)
							GetItemSprite(4)->DrawBright(xx + Cosine(angList[ang]) * outXes / FIXAMT, yy + 3 + Sine(angList[ang]) * outXes / FIXAMT, mgl, -31);
					}
				if (GotRunePouchInLevel(world, mNum))
					GetItemSprite(4)->DrawBright(cx + Cosine(angList[ang]) * outXes / FIXAMT, cy + 3 + Sine(angList[ang]) * outXes / FIXAMT, mgl, 0);
				ang++;
			}
			if (m->contentFlags & LP_GOTSKILLSHARD)
			{
				for (int xx = cx - 1; xx <= cx + 1; xx++)
					for (int yy = cy - 1; yy <= cy + 1; yy++)
					{
						if (xx != cx && yy != cy)
							GetItemSprite(71)->DrawBright(xx + Cosine(angList[ang]) * outXes / FIXAMT, yy + 8 + Sine(angList[ang]) * outXes / FIXAMT, mgl, -31);
					}
				if (GotSkillShardInLevel(world, mNum))
					GetItemSprite(71)->DrawColored(cx + Cosine(angList[ang]) * outXes / FIXAMT, cy + 8 + Sine(angList[ang]) * outXes / FIXAMT, mgl, 6, 0);
				ang++;
			}
		}
		CenterPrintCompressed(cx + 1, cy - outXes + 1, m->name, outXes * 100 / 40, -31, 1);
		CenterPrintCompressed(cx, cy - outXes, m->name, outXes * 100 / 40, 0, 1);
	}
	if (!ClassicMode())
	{
		for (i = 0; i < MAX_SPECIAL; i++)
		{
			int cx = map->special[i].x * TILE_WIDTH - camx + TILE_WIDTH / 2;
			int cy = map->special[i].y * TILE_HEIGHT - camy + TILE_HEIGHT / 2;
			if (map->special[i].effect == SPC_GOTOMAP)
			{
				if (map->special[i].value < MAX_MAPS && PlayerPassedLevel(world, map->special[i].value))
					DrawRedX(cx, cy, mgl);
			}
		}
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
		if(!PlayerHasSword())
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
			if (VeryClassicMode() && spcl->value != 50 && player.levelPassed[player.worldNum][spcl->value])
				return;
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
					else if (VeryClassicMode())
					{
						// This will hit a !WonTheBattle() branch in game.cpp
						// and will count as running away instead.
						SendMessageToGame(MSG_WINLEVEL,0);
					}
					else
					{
						return;
					}
				}
				else
				{
					SendMessageToGame(MSG_WINLEVEL,0);
					MakeNormalSound(SND_WINLEVEL);
					bool ok = true;
					for (int i = 0; i < 10; i++)
						if ((i != SPL_SUMMON && i!=SPL_HEAL && i!=SPL_ARMOR && player.usedSpells[i]) ||
							(i==SPL_SUMMON && player.usedSpells[i]!=1))	// you have to have cast summon, and summon=2 if you killed something with something else (stepping on spider, reflected shots, etc)
						{
							ok = false;
							break;
						}
					if (ok && player.usedFireballs == 0 && player.summonKills >= 20)	// the only spells you used were summon, heal, and armor, and you used no fireballs, and summons got 20 kills
						EarnAchieve(Achievement::SUMMON);
					ok = true;
					for (int i = 0; i < 10; i++)
						if (i != SPL_ARMOR && player.usedSpells[i])
						{
							ok = false;
							break;
						}
					if (ok && player.usedFireballs==0 && player.totalKills >= 20)	// you used no fireballs and no spell other than armor
						EarnAchieve(Achievement::STONESKIN);
				}
			}
			else
				return;	// to avoid triggering a message or shutting it off if repeatable
			break;
		case SPC_ZAPWALL:
			if(map->map[spcl->effectX+spcl->effectY*map->width].wall)
			{
				MakeSound(SND_WALLDOWN,(spcl->effectX*TILE_WIDTH)*FIXAMT,
						(spcl->effectY*TILE_HEIGHT)*FIXAMT,SND_CUTOFF,1500);
				ZapWall(map,spcl->effectX,spcl->effectY,spcl->value);
			}
			break;
		case SPC_CHGFLOOR:
			MakeSound(SND_WALLDOWN, (spcl->effectX * TILE_WIDTH) *FIXAMT,
				(spcl->effectY * TILE_HEIGHT) *FIXAMT, SND_CUTOFF, 1500);
			ChangeFloor(map, spcl->effectX, spcl->effectY, spcl->value);
			break;
		case SPC_CHGITEM:
			MakeSound(SND_WALLDOWN, (spcl->effectX * TILE_WIDTH) *FIXAMT,
				(spcl->effectY * TILE_HEIGHT) *FIXAMT, SND_CUTOFF, 1500);
			if ((spcl->value == ITM_RUNEPOUCH && GotRunePouchInLevel(player.worldNum, player.levelNum)) ||
				(spcl->value == ITM_SILENTRUNE && GotRuneInLevel(player.worldNum, player.levelNum)) ||
				(spcl->value == ITM_SPELLBOOK && GotSpellInLevel(player.worldNum, player.levelNum)) ||
				(spcl->value == ITM_FAIRYBELL && GotFairyBellInLevel(player.worldNum, player.levelNum)) ||
				(spcl->value == ITM_SKILLSHARD && GotSkillShardInLevel(player.worldNum, player.levelNum)))
				//don't do it if you already got the special item
			{

			}
			else
				map->map[spcl->effectX + spcl->effectY * map->width].item = spcl->value;
			break;
		case SPC_RAISEWALL:
			if(!map->map[spcl->effectX+spcl->effectY*map->width].wall)
			{
				MakeSound(SND_WALLUP,(spcl->effectX*TILE_WIDTH)*FIXAMT,
						(spcl->effectY*TILE_HEIGHT)*FIXAMT,SND_CUTOFF,1500);
				RaiseWall(map,spcl->effectX,spcl->effectY,spcl->value);
			}
			break;
		case SPC_TOGGLEWALL:
			if(map->map[spcl->effectX+spcl->effectY*map->width].wall)
			{
				MakeSound(SND_WALLDOWN,(spcl->effectX*TILE_WIDTH)*FIXAMT,
						(spcl->effectY*TILE_HEIGHT)*FIXAMT,SND_CUTOFF,1500);
				// store what the wall looked like for later popping it back up
				spcl->value=map->map[spcl->effectX+spcl->effectY*map->width].wall;
				ZapWall(map,spcl->effectX,spcl->effectY,
					map->map[spcl->effectX+spcl->effectY*map->width].floor);
			}
			else
			{
				MakeSound(SND_WALLUP,(spcl->effectX*TILE_WIDTH)*FIXAMT,
						(spcl->effectY*TILE_HEIGHT)*FIXAMT,SND_CUTOFF,1500);
				RaiseWall(map,spcl->effectX,spcl->effectY,spcl->value);
			}
			break;
		case SPC_LIGHT:
			MakeSound(SND_LIGHTSON,(spcl->effectX*TILE_WIDTH)*FIXAMT,
						(spcl->effectY*TILE_HEIGHT)*FIXAMT,SND_CUTOFF,1300);
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
				if (spcl->value == 0)	// classic teleport
				{
					MakeSound(SND_TELEPORT, victim->x, victim->y, SND_CUTOFF, 1500);
				}
				else // staircase teleport
				{
					if (victim->type == MONS_BOUAPHA)
					{
						for (int i = 0; i < map->width * map->height; i++)
							map->map[i].templight = -31;	// go black briefly
						lightSpeed = 10;
						MakeNormalSound(SND_STAIRS);
					}
				}
				victim->x = (spcl->effectX * TILE_WIDTH + (TILE_WIDTH / 2)) *FIXAMT;
				victim->y = (spcl->effectY * TILE_HEIGHT + (TILE_HEIGHT / 2)) *FIXAMT;
				victim->mapx = victim->x / (TILE_WIDTH * FIXAMT);
				victim->mapy = victim->y / (TILE_HEIGHT * FIXAMT);
				victim->dx=0;
				victim->dy=0;
				if (victim->type == MONS_BOUAPHA)
				{
					PutCamera(victim->x, victim->y);
					UpdateCamera(victim->x, victim->y, victim->facing, map);
					SetTportClock(30);
				}
				else
				{
					victim->birthState = 2;	// restart them tiny in case they overlap something
					if (MonsterFlags(victim->type) & MF_WALLWALK)
						victim->birthState = 0;	// wall walking enemies can spawn anywhere
				}

				if (spcl->value == 0)	// classic teleport
				{
					map->map[spcl->effectX + spcl->effectY * map->width].templight = 34;
					if (spcl->effectX > 0)
						map->map[spcl->effectX - 1 + spcl->effectY * map->width].templight = 20;
					if (spcl->effectX < map->width - 1)
						map->map[spcl->effectX + 1 + spcl->effectY * map->width].templight = 20;
					if (spcl->effectY > 0)
						map->map[spcl->effectX + (spcl->effectY - 1) * map->width].templight = 20;
					if (spcl->effectY < map->height - 1)
						map->map[spcl->effectX + (spcl->effectY + 1) * map->width].templight = 20;
				}
			}
			break;
		case SPC_SUMMON:
			MakeSound(SND_TELEPORT,(spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))*FIXAMT,
					(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))*FIXAMT,SND_CUTOFF,1500);
			AddGuy((spcl->effectX*TILE_WIDTH+(TILE_WIDTH/2))*FIXAMT,
					(spcl->effectY*TILE_HEIGHT+(TILE_HEIGHT/2))*FIXAMT,
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
		case SPC_KILLSPCL:
			for (int i = 0; i < MAX_SPECIAL; i++)
			{
				if (map->special[i].x == spcl->effectX && map->special[i].y == spcl->effectY)
				{
					map->special[i].trigger = 0;	// can no longer be triggered!
				}
			}
			break;
		case SPC_CHGMONS:
			ChangeAllGuysOfType(spcl->effectX, spcl->value);
			break;
	}
	if (spcl->trigger & TRG_MESSAGE)
	{
		if (spcl->msg[0] == '%')
		{
			byte n = (byte)atoi(&spcl->msg[1]);
			InitSpeech(n);
			EnterSpeechMode();
			GetGoodguy()->dx = 0;
			GetGoodguy()->dy = 0;	// stop it from retriggering
			GetGoodguy()->x = ((GetGoodguy()->x / (TILE_WIDTH * FIXAMT)) * TILE_WIDTH+TILE_WIDTH/2) * FIXAMT;
			GetGoodguy()->y = ((GetGoodguy()->y / (TILE_HEIGHT * FIXAMT)) * TILE_HEIGHT+TILE_HEIGHT/2) * FIXAMT;
		}
		else
			NoRepeatNewMessage(spcl->msg, 75);
	}
	if(!(spcl->trigger&TRG_REPEATABLE))
		spcl->trigger=0;	// can't trigger anymore
}

byte VaultPuzzleCheck(Map *map)
{
	if (map->map[29 + 46 * map->width].floor == 63)
		return 0;	// don't let it repeat on the same visit to avoid messing up the switch
	// X V I
	byte code[] = {
		1,0,1, 1,0,1, 2,1,2,
		0,1,0, 1,0,1, 0,1,0,
		1,0,1, 0,1,0, 2,1,2,
	};
	byte xTab[] = { 2,4,6,9,11,13,16,18,20 };
	byte yTab[] = { 33,35,37 };
	byte code2mustBe = 0;
	for (int j = 0; j < 3; j++)
		for (int i = 0; i < 9; i++)
		{
			if (code[i + j * 9] == 1 && map->map[xTab[i] + yTab[j] * map->width].floor != 64)	// must be on
				return 0;
			if (code[i + j * 9] == 0 && map->map[xTab[i] + yTab[j] * map->width].floor != 65)	// must be off
				return 0;
			if (code[i + j * 9] == 2)	// all code=2's must be the same, but either way
			{
				if (code2mustBe == 0)
					code2mustBe = map->map[xTab[i] + yTab[j] * map->width].floor;
				else if (map->map[xTab[i] + yTab[j] * map->width].floor != code2mustBe)
					return 0;
			}
		}

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

byte VaultPuzzle2Check(Map* map)
{
	if (ClassicMode())
		return 0;

	if (vault2Opened)
		return 0;	// don't repeat on the same visit, so we can't mess up the switch position
	// L I V
	byte code[] = {
		1,0,0, 2,1,2, 1,0,1,
		1,0,0, 0,1,0, 1,0,1,
		1,1,1, 2,1,2, 0,1,0
	};
	byte xTab[] = { 2,4,6,9,11,13,16,18,20 };
	byte yTab[] = { 33,35,37 };
	byte code2mustBe = 0;
	for(int j=0;j<3;j++)
		for (int i = 0; i < 9; i++)
		{
			if (code[i + j * 9] == 1 && map->map[xTab[i] + yTab[j] * map->width].floor != 64)	// must be on
				return 0;
			if (code[i + j * 9] == 0 && map->map[xTab[i] + yTab[j] * map->width].floor != 65)	// must be off
				return 0;
			if (code[i + j * 9] == 2)	// all code=2's must be the same, but either way
			{
				if (code2mustBe == 0)
					code2mustBe = map->map[xTab[i] + yTab[j] * map->width].floor;
				else if (map->map[xTab[i] + yTab[j] * map->width].floor != code2mustBe)
					return 0;
			}
		}
	return 1;
}

void OpenVaultDoor2(Map* map)
{
	if (ClassicMode())
		return;

	// open the secret book passage
	map->map[25 + 58 * map->width].wall = 0;
	map->map[25 + 58 * map->width].floor = 0;
	map->map[25 + 59 * map->width].wall = 0;
	map->map[25 + 59 * map->width].floor = 0;
	map->map[24 + 59 * map->width].wall = 24;
	map->map[24 + 59 * map->width].floor = 154;
	for(int i=19;i<=27;i++)
		for (int j = 47; j <= 57; j++)
		{
			map->map[i+j * map->width].light = -6;	// lighten it up a bit
		}
	vault2Opened = true;
}

int TotalBrains(void)
{
	return totalBrains;
}

bool WallIsCavern(byte wall)
{
	return ((wall > 0 && wall < 3) || wall == 8 || wall == 10 || wall == 16);
}

void Treeify(Map* map)
{
	int i, j;
	for (i = 0; i < map->width; i++)
		for (j = 0; j < map->height; j++)
		{
			mapTile_t* t = map->GetTile(i, j);
			if (t->item == ITM_TREE || t->item == ITM_TREE2)
			{
				t->item = Random(2) ? ITM_TREE : ITM_TREE2;
			}
			else if (t->item == ITM_BUSH || t->item == ITM_BUSH2)
				t->item = Random(2) ? ITM_BUSH : ITM_BUSH2;
			else if (t->item == ITM_DEADTREE || t->item == ITM_DEADTREE2)
				t->item = Random(2) ? ITM_DEADTREE : ITM_DEADTREE2;
		}
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
			if(WallIsCavern(map->map[i+j*map->width].wall) && map->map[i+j*map->width].floor>183)
			{
				// this tile wants to be cavernized
				v=0;
				if(i==0 || WallIsCavern(map->map[i-1+j*map->width].wall))
					v+=1;
				if(j==0 || WallIsCavern(map->map[i+(j-1)*map->width].wall))
					v+=2;
				if(i==map->width-1 || WallIsCavern(map->map[i+1+j*map->width].wall))
					v+=4;
				if(j==map->height-1 || WallIsCavern(map->map[i+(j+1)*map->width].wall))
					v+=8;
				map->map[i+j*map->width].floor=result[v];
				if(j<map->height-1 && map->map[i+(j+1)*map->width].floor>4
					&& map->map[i+(j+1)*map->width].floor<17 && map->map[i+j*map->width].wall==1)
					map->map[i+j*map->width].wall=2;	// give it a lava tint
			}
			if (map->map[i + j * map->width].floor <= 4)	// basic floor
			{
				byte b = Random(2);
				if (Random(7) == 0)
					b = Random(3) + 2;
				map->map[i + j * map->width].floor = b;
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

void Waterize(Map* map)
{
	byte result[] = { 188,187,185,194,186,189,193,197,184,192,190,198,191,195,196,199 };
	byte lavaResult[] = { 0,18,19,27,20,34,28,30,17,25,33,31,26,29,32,35 };
	byte v;
	int i, j;

	for (i = 0; i < map->width; i++)
		for (j = 0; j < map->height; j++)
		{
			if (map->map[i + j * map->width].wall > 0 && map->map[i + j * map->width].floor > 183)
			{
				// this tile wants to be cavernized
				v = 0;
				if (i == 0 || map->map[i - 1 + j * map->width].wall > 0)
					v += 1;
				if (j == 0 || map->map[i + (j - 1) * map->width].wall > 0)
					v += 2;
				if (i == map->width - 1 || map->map[i + 1 + j * map->width].wall > 0)
					v += 4;
				if (j == map->height - 1 || map->map[i + (j + 1) * map->width].wall > 0)
					v += 8;
				map->map[i + j * map->width].floor = result[v];
				if (j < map->height - 1 && map->map[i + (j + 1) * map->width].floor>4
					&& map->map[i + (j + 1) * map->width].floor < 17 && map->map[i + j * map->width].wall == 1)
					map->map[i + j * map->width].wall = 2;	// give it a lava tint
			}
			if (map->map[i + j * map->width].floor <= 4)	// basic floor
			{
				byte b = Random(2);
				if (Random(7) == 0)
					b = Random(3) + 2;
				map->map[i + j * map->width].floor = b;
			}
			if (map->map[i + j * map->width].floor > 4 && map->map[i + j * map->width].floor < 17)
			{
				if (MGL_random(6) > 0)
					map->map[i + j * map->width].floor = MGL_random(4) + 5;
				else
					map->map[i + j * map->width].floor = MGL_random(8) + 9;
			}
			if (map->map[i + j * map->width].floor < 5 || (map->map[i + j * map->width].floor > 16 && map->map[i + j * map->width].floor < 36))
			{
				v = 0;
				if (i != 0 && map->map[i - 1 + j * map->width].floor > 4 && map->map[i - 1 + j * map->width].floor < 17)
					v += 1;
				if (j != 0 && map->map[i + (j - 1) * map->width].floor > 4 && map->map[i + (j - 1) * map->width].floor < 17)
					v += 2;
				if (i != map->width - 1 && map->map[i + 1 + (j)*map->width].floor > 4 && map->map[i + 1 + (j)*map->width].floor < 17)
					v += 4;
				if (j != map->height - 1 && map->map[i + (j + 1) * map->width].floor > 4 && map->map[i + (j + 1) * map->width].floor < 17)
					v += 8;
				if (v > 0)
				{
					map->map[i + j * map->width].floor = lavaResult[v];
				}
				else
				{
					if (i != 0 && j != 0 && map->map[i - 1 + (j - 1) * map->width].floor > 4 && map->map[i - 1 + (j - 1) * map->width].floor < 17)
						map->map[i + j * map->width].floor = 24;
					if (i != 0 && j != map->height - 1 && map->map[i - 1 + (j + 1) * map->width].floor > 4 && map->map[i - 1 + (j + 1) * map->width].floor < 17)
						map->map[i + j * map->width].floor = 22;
					if (i != map->width - 1 && j != 0 && map->map[i + 1 + (j - 1) * map->width].floor > 4 && map->map[i + 1 + (j - 1) * map->width].floor < 17)
						map->map[i + j * map->width].floor = 23;
					if (i != map->width - 1 && j != map->height - 1 && map->map[i + 1 + (j + 1) * map->width].floor > 4 && map->map[i + 1 + (j + 1) * map->width].floor < 17)
						map->map[i + j * map->width].floor = 21;
				}
			}
		}
}

void GetRidOfGoodStuff(Map *map)
{
	int i;

	if (player.worldNum == 3 && player.levelNum == 13)
		return;	// smashball keeps its coins, they're bonus markers

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

void LockedMazeSpikes(Map* map,int x,int y)
{
	mapTile_t* t = map->GetTile(x, y);
	if (t && t->floor == 67)
	{
		t->floor = 68;
		MakeNormalSound(SND_MENUCLICK);
		int i;
		int tnum=0;
		for (i = 0; i < map->width * map->height; i++)
		{
			if (map->map[i].floor == 67)
				return;	// not all spikes yet
			if (map->map[i].floor == 63)	// this is where the shard goes
				tnum = i;
		}
		if (!GotSkillShardInLevel(player.worldNum, player.levelNum))
		{
			MakeNormalSound(SND_INFERNAL);
			map->map[tnum].item = ITM_SKILLSHARD;
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

byte mushAidStep = 0;
byte mushAidCode[4];

void MushAidPuzzleReset(Map *map)
{
	if (ClassicMode())
		return;

	mushAidStep = 0;
	for (int i = 0; i < 4; i++)
		mushAidCode[i] = 0;
	map->GetTile(35, 8)->floor = 120;
	map->GetTile(33, 9)->floor = 127;
	map->GetTile(37, 9)->floor = 124;
	map->GetTile(35, 10)->floor = 126;
	map->GetTile(33, 11)->floor = 122;
	map->GetTile(37, 11)->floor = 125;
	map->GetTile(35, 12)->floor = 121;
}

void MushAidPuzzleUpdate(Map *map)
{
	if (ClassicMode())
		return;

	if (GotRuneInLevel(player.worldNum, player.levelNum))
		return;

	byte corrCode[4] = { 120,121,124,127 };
	Guy* me = GetGoodguy();
	mapTile_t* m = map->GetTile(me->mapx, me->mapy);
	if (m->floor >= 120 && m->floor <= 127)	// one of the rune tiles, not lit
	{
		byte preFloor = m->floor;
		if (m->floor >= 126) m->floor -= 20;	// light up
		else m->floor += 20;	// also light up
		if (me->mapx >= 33 && me->mapy >= 8 && me->mapx <= 37 && me->mapy <= 12 && mushAidStep<4)	// it's on the circle
		{
			mushAidCode[mushAidStep] = preFloor;
			mushAidStep++;
			MakeNormalSound(SND_CHLGCRYSTAL);
			if(mushAidStep==4)
			{
				bool fail = false;
				for (int i = 0; i < 4; i++)
					if (mushAidCode[i] != corrCode[i])
					{
						fail = true;
						break;
					}
				if (fail)
				{
					MushAidPuzzleReset(map);
					AddGuy(40 * TILE_WIDTH*FIXAMT, 10 * TILE_HEIGHT*FIXAMT, 0, MONS_MUSH);
				}
				else
				{
					MakeNormalSound(SND_INFERNAL);
					if (!GotRuneInLevel(player.worldNum, player.levelNum))
					{
						map->GetTile(35, 10)->item = ITM_SILENTRUNE;
						map->TempTorch(35, 10, 16);
					}
				}
			}
		}
	}
}

byte abandonPos = 0;

void AbandonedVillagePuzzleReset(Map* map)
{
	if (ClassicMode())
		return;

	abandonPos = 0;
	for (int i = 0; i < 6; i++)
		map->GetTile(57 + i, 6)->floor = 97;	// wipe the 'screen' of runes
}

void AbandonedVillagePuzzle(Map *map)
{
	/*
	 57,6 is where the row of runes is
	 55,8 is the upper left of the keyboard
	 59,4 is the wall to zap
	 22,30 is the book (should change the texture so it's obvious)
	*/
	if (ClassicMode())
		return;

	byte code[] = { 127-20,128-20,129-20,122+20,123+20,130-20 };
	if (GotRuneInLevel(player.worldNum, player.levelNum))
		return;

	Guy* me = GetGoodguy();
	if (abandonPos<6 && me->mapx >= 55 && me->mapy >= 8 && me->mapx <= 61 && me->mapy <= 12)
	{
		mapTile_t* m = map->GetTile(me->mapx, me->mapy);
		if (m->floor >= 120 && m->floor <= 130)	// a rune tile
		{
			byte lit = m->floor;
			if (m->floor <= 125)
				lit = m->floor + 20;
			else
				lit = m->floor - 20;
			map->GetTile(57 + abandonPos, 6)->floor = lit;
			MakeSound(SND_CHLGCRYSTAL, me->x, me->y, SND_CUTOFF, 100);
			abandonPos++;
			if (abandonPos == 6)	// filled the whole word
			{
				byte ok = 0;
				for (abandonPos = 0; abandonPos < 6; abandonPos++)
					if (map->GetTile(57 + abandonPos, 6)->floor == code[abandonPos])
						ok++;
				if (ok == 6)	// spelled it right!
				{
					ZapWall(map, 59, 5, 96);	// knock the wall down
					MakeNormalSound(SND_INFERNAL);
					map->GetTile(59,2)->item = ITM_SILENTRUNE;
					map->TempTorch(59, 2, 16);
					abandonPos = 6;
				}
				else
				{
					MakeNormalSound(SND_UNAVAILABLE);
					AbandonedVillagePuzzleReset(map);
				}
			}
		}
	}
}

void Map::ScanForContent(void)
{
	contentFlags = LP_PASSED;	// this level can be passed... weirdly enough
	for (int i = 0; i < 32; i++)
	{
		if (special[i].effect == SPC_CHGITEM)
		{
			if (special[i].value == ITM_FAIRYBELL)
				contentFlags |= LP_GOTFAIRY;
			if (special[i].value == ITM_SPELLBOOK)
				contentFlags |= LP_GOTSPELL;
			if (special[i].value == ITM_SILENTRUNE)
				contentFlags |= LP_GOTRUNE;
			if (special[i].value >=ITM_SWORD1 && special[i].value<=ITM_SWORD4)
				contentFlags |= LP_GOTSWORD;
			if (special[i].value == ITM_RUNEPOUCH)
				contentFlags |= LP_GOTRUNEPOUCH;
			if (special[i].value == ITM_SKILLSHARD)
				contentFlags |= LP_GOTSKILLSHARD;
		}
	}
	for (int i = 0; i < width * height; i++)
	{
		if (map[i].item  == ITM_SPELLBOOK)
			contentFlags |= LP_GOTSPELL;
		if (map[i].item == ITM_SILENTRUNE)
			contentFlags |= LP_GOTRUNE;
		if (map[i].item == ITM_FAIRYBELL)
			contentFlags |= LP_GOTFAIRY;
		if (map[i].item == ITM_RUNEPOUCH)
			contentFlags |= LP_GOTRUNEPOUCH;
		if (map[i].item == ITM_SKILLSHARD)
			contentFlags |= LP_GOTSKILLSHARD;
		if (map[i].item >= ITM_SWORD1 && map[i].item <= ITM_SWORD4)
			contentFlags |= LP_GOTSWORD;
	}
}

void GuestChamberTeleport(int toX, int toY,Map *map)
{
	if (ClassicMode())
		return;

	Guy* g = GetGoodguy();

	map->TempTorch(g->x / (FIXAMT * TILE_WIDTH), g->y / (FIXAMT * TILE_HEIGHT), 20);
	g->x = (toX * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
	g->y = (toY * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
	g->dx = 0;
	g->dy = 0;
	g->z = 20 * FIXAMT;
	g->dz = -3*FIXAMT;
	map->TempTorch(g->x / (FIXAMT * TILE_WIDTH), g->y / (FIXAMT * TILE_HEIGHT), 20);
}

void GuestChamberPuzzleStep(Map* map, int mapx, int mapy)
{
	if (ClassicMode())
		return;

	char code[] = "BONEHEAD";
	Guy* g = GetGoodguy();
	if (!g) return;
	if (map->map[mapx + mapy * map->width].floor != 121 || map->map[mapx+(mapy-1)*map->width].wall<81 || map->map[mapx+(mapy-1)*map->width].wall>(81+24))
		return;	// we only count the letter teleporters here

		// 1,35-12,49
		// 121=teleporter, 120=lit up teleporter
		// 67 = not yet spikes, 68=spikes
		// 3,33 = destination after each letter

		if (guestProgress >= 8)
		{
			// if you're done, teleport to the end
			MakeNormalSound(SND_TELEPORT);
			GuestChamberTeleport(5, 55, map);
			return;
		}

		byte c = map->map[mapx + (mapy - 1) * map->width].wall - 81;
		if (c == code[guestProgress] - 'A')
		{
			guestProgress++;
			MakeNormalSound(SND_LIGHTSON);
			if (guestProgress < 8)
				GuestChamberTeleport(3, 33, map);
			else
				GuestChamberTeleport(5, 55, map);
		}
		else // uh oh, you are wrong
		{
			guestProgress = 0;
			MakeNormalSound(SND_UNAVAILABLE);
			GuestChamberTeleport(3, 33, map);
			for (int i = 0; i < 5; i++)	// add 5 spikes
			{
				byte tries = 255;
				while (tries > 0)
				{
					int x, y;
					x = 1 + Random(13 - 1);
					y = 35 + Random(50 - 35);
					if (map->map[x + y * map->width].floor == 67)
					{
						tries = 0;
						map->map[x + y * map->width].floor = 68;	// SPIKES!
						if (Random(2))
						{
							Guy* m = AddGuy((x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 0, Random(2) ? MONS_PEEPBOMB : MONS_PEEPBOMB2);
							if (m) m->mind1 = (byte)Random(256);
						}
					}
				}
			}

		}
}

void LibraryPuzzle(Map* map)
{
	if (ClassicMode())
		return;

	byte coords[] = {
		63,69,
		66,58,
		70,67,
		76,58,
		79,54,
	};

	if (libraryProgress >= 5)
		return;

	byte status[5];
	for (int i = 0; i < 5; i++)
		status[i] = (map->map[coords[i * 2] + coords[i * 2 + 1] * map->width].floor == 116);

	byte advance = true;
	for (int i = 0; i < 5; i++)
	{
		if ((!status[i] && i <= libraryProgress) || (status[i] && i > libraryProgress))
		{
			advance = false;
			break;
		}
	}
	if (advance)
	{
		libraryProgress++;
		if (libraryProgress == 5 && !GotRuneInLevel(player.worldNum, player.levelNum))
		{
			map->map[73 + 57 * map->width].item = ITM_SILENTRUNE;
			MakeNormalSound(SND_INFERNAL);
		}
	}
	else
		libraryProgress = 0;
}

void MinesPuzzle(Map* map,int x,int y)
{
	if (ClassicMode())
		return;

	byte pos[5][10] = {
		{ // floor 1
			77,63,
			76,62,
			76,60,
			76,64,
			77,61,
		},
		{ // floor 2
			9,7,
			9,5,
			8,6,
			9,9,
			8,8,
		},
		{ // floor 3
			120,56,
			121,55,
			118,56,
			117,55,
			119,55,
		},
		{ // floor 4
			124,122,
			122,122,
			123,123,
			120,122,
			121,123,
		},
		{ // floor 5
			61,83,
			60,82,
			63,83,
			62,82,
			64,82,
		},
	};

	byte myFloor = 6;

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			if (pos[i][j * 2] == x && pos[i][j * 2 + 1] == y)
				myFloor = i;

	byte correct = 0;
	for (int i = 0; i < 5; i++)
	{
		if (myFloor == i)	// we only change stuff on your current floor
		{
			for (int num = 0; num < 5; num++)
			{
				if (map->map[pos[i][num * 2] + pos[i][num * 2 + 1] * map->width].floor >= 104 &&
					(pos[i][num * 2] != x || pos[i][num * 2 + 1] != y))	// tile is lit, and it's not the one you stepped on, unlight it
					map->map[pos[i][num * 2] + pos[i][num * 2 + 1] * map->width].floor -= 24;

				if (map->map[pos[i][num * 2] + pos[i][num * 2 + 1] * map->width].floor < 104 &&
					(pos[i][num * 2] == x && pos[i][num * 2 + 1] == y))	// tile is not lit, and it IS the one you stepped on, so keep it lit. No turning off
					map->map[pos[i][num * 2] + pos[i][num * 2 + 1] * map->width].floor += 24;
			}
		}
		if (map->map[pos[i][i * 2] + pos[i][i * 2 + 1] * map->width].floor >= 104)	// the correct one for this floor is lit, and we know at most one is lit
			correct++;
	}
	if(correct==5 && map->map[56 + 96 * map->width].wall != 0 && !GotRuneInLevel(player.worldNum, player.levelNum))
	{
		for (int i = 94; i <= 96; i++)
		{
			map->map[56 + i * map->width].wall = 0;
			map->map[56 + i * map->width].floor = 0;
			MakeNormalSound(SND_WALLDOWN);
			NewMessage("Something thunked!", 60);

		}
	}
}

void BatsPuzzle(Map* map,int x,int y)
{
	if (ClassicMode())
		return;

	if (x == 12 && y == 62)
	{
		ChangeAllGuysOfType(MONS_BAT, MONS_FIREBAT);
		ChangeMind2OfType(MONS_BIGBAT, 1);
		ChangeMind2OfType(MONS_BIGBAT2, 1);
		ChangeMind2OfType(MONS_HUGEBAT, 1);
		ChangeMind2OfType(MONS_HUGEBAT2, 1);
		BlowUpGuy((x * TILE_WIDTH + TILE_WIDTH / 2-16),
			(y * TILE_HEIGHT + TILE_HEIGHT / 2-16),
			(x * TILE_WIDTH + TILE_WIDTH / 2+16),
			(y * TILE_HEIGHT + TILE_HEIGHT / 2+16), 0, 4);

		for (int i = 0; i < 100; i++)
		{
			int px, py;
			px = (x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
			py = (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
			px = px - (HALFWID + Random(SCRWID))*FIXAMT;
			py = py - (HALFHEI + Random(SCRHEI)) * FIXAMT;

			BlowWigglySmoke(px, py, 0, 8);
		}
		batsProgress++;
	}
	if (x == 2 && y == 2)
	{
		ChangeAllGuysOfType(MONS_BIGBAT, MONS_BIGBAT2);
		BlowUpGuy((x * TILE_WIDTH + TILE_WIDTH / 2 - 16),
			(y * TILE_HEIGHT + TILE_HEIGHT / 2 - 16),
			(x * TILE_WIDTH + TILE_WIDTH / 2 + 16),
			(y * TILE_HEIGHT + TILE_HEIGHT / 2 + 16), 0, 4);

		for (int i = 0; i < 100; i++)
		{
			int px, py;
			px = (x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
			py = (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
			px = px - (HALFWID + Random(SCRWID)) * FIXAMT;
			py = py - (HALFHEI + Random(SCRHEI)) * FIXAMT;

			BlowWigglySmoke(px, py, 0, 8);
		}
		batsProgress++;
	}
	if (x == 36 && y == 8)
	{
		ChangeAllGuysOfType(MONS_HUGEBAT,MONS_HUGEBAT2);
		BlowUpGuy((x * TILE_WIDTH + TILE_WIDTH / 2 - 16),
			(y * TILE_HEIGHT + TILE_HEIGHT / 2 - 16),
			(x * TILE_WIDTH + TILE_WIDTH / 2 + 16),
			(y * TILE_HEIGHT + TILE_HEIGHT / 2 + 16), 0, 4);

		for (int i = 0; i < 100; i++)
		{
			int px, py;
			px = (x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
			py = (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
			px = px - (HALFWID + Random(SCRWID)) * FIXAMT;
			py = py - (HALFHEI + Random(SCRHEI)) * FIXAMT;

			BlowWigglySmoke(px, py, 0, 8);
		}
		batsProgress++;
	}
}

//--------------------------------------- SWAMP PUZZLE TECHNOLOGY IS REALLY BIG
byte swampRecipe[6];
byte potContents[3];
byte potOrder[3],potPos;
byte swampTicker;
byte boilTemp, simmerTemp;
byte swampTemp;
word swampCookTime;
word swampBoilTime, swampSimmerTime,swampPreheatTime;
bool swampHaveHitBoil;
byte swampScore;

void SwampSpawn(Map* map, byte type,int minx, int miny, int maxx, int maxy)
{
	int tries = 200;
	while (tries-- > 0)
	{
		int x = Random(maxx - minx+1) + minx;
		int y = Random(maxy - miny + 1) + miny;
		mapTile_t* t = map->GetTile(x, y);
		if (type == MONS_OCTOPUS && (t->floor < 20 || t->floor>23))
			continue;	// on water only
		else if (type!=MONS_OCTOPUS && t->floor >= 20 && t->floor < 74)	// not on water or water-edge tiles
			continue;
		if (world->terrain[t->floor].flags & TF_NOENEMY)
			continue;
		if (t->item || t->wall)
			continue;

		AddGuy((x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 0, type);
		return;
	}
}

void SwampItem(Map* map, byte type, int minx, int miny, int maxx, int maxy)
{
	int tries = 200;
	while (tries-- > 0)
	{
		int x = Random(maxx - minx + 1) + minx;
		int y = Random(maxy - miny + 1) + miny;
		mapTile_t* t = map->GetTile(x, y);
		if (t->floor >= 20 && t->floor < 74)	// not on water or water-edge tiles
			continue;
		if (world->terrain[t->floor].flags & TF_NOENEMY)
			continue;
		if (t->item || t->wall)
			continue;

		t->item = type;
		return;
	}
}

void SwampStewSetup(Map* map)
{
	// goodies and enemies can't spawn in (53,28-73,70)
	// they also can't spawn on water-edge pieces 24-74, or of course on water 20-23
	// also not on no-enemy tiles
	// first key spawns at 61,64 when you chat with her
	// cauldron is 65,62
	// button is 68,61
	// left zone is (13,42-53,71)
	// middle zone is
	// right zone is

	byte coords[] =
	{
		13,42,53,71,	// the rectangle that contains zone1 (upper left)
		37,73,95,103,	// zone2 (bottom middle)
		72,27,112,72,	// zone3 (top right)
	};

	byte zone[3] = { 0,1,2 };
	byte ingredientOrder[3] = { 0,1,2 };
	byte flips = 10;

	potContents[0] = potContents[1] = potContents[2] = 0;
	potOrder[0] = potOrder[1] = potOrder[2] = 255;
	potPos = 255;	// nothing put in pot yet
	swampTicker = 0;
	// temps are: red, yellow, green, purple, blue, cyan
	boilTemp = Random(3) + 3;	// one of the top 3 temps
	simmerTemp = Random(3) + 1; // one of the 3 temps just above red
	if (boilTemp == simmerTemp) simmerTemp--;
	
	while ((flips--)>0)
	{
		byte first = Random(3);
		byte second = (first + 1) % 3;
		byte z = zone[first];
		zone[first] = zone[second];
		zone[second] = z;
		first = Random(3);
		second = (first + 1) % 3;
		z = ingredientOrder[first];
		ingredientOrder[first] = ingredientOrder[second];
		ingredientOrder[second] = z;
	}

	for (int i = 0; i < 3; i++)
	{
		swampRecipe[i*2]=Random(7) + 5;	// amount you need, 5-11
		swampRecipe[i*2+1]=ingredientOrder[i];	// which ingredient it is (0=grimbleweed, 1=octon juice, 2=toadstool)
	}

	byte keyForZone[4];
	for (int i = 0; i < 4; i++) keyForZone[i] = 255;

	keyForZone[3] = Random(3);			// first zone is just random (the key is in 'zone 3' which is the cabin)
	keyForZone[keyForZone[3]] = keyForZone[3];	// this code looks insane, but we are setting the key that's in that first zone to anything other than itself
	while (keyForZone[keyForZone[3]] == keyForZone[3])
		keyForZone[keyForZone[3]] = Random(3);
	keyForZone[keyForZone[keyForZone[3]]] = 0;	// and then in the zone that that 2nd key leads to, we need to put the last key
	if (keyForZone[3] == 0 || keyForZone[keyForZone[3]] == 0)
	{
		keyForZone[keyForZone[keyForZone[3]]] = 1;
		if (keyForZone[3] == 1 || keyForZone[keyForZone[3]] == 1)
			keyForZone[keyForZone[keyForZone[3]]] = 2;
	}
	// and in whatever that last zone is, the key is 255, meaning don't add a key

	// the first key is in the cabin
	map->GetTile(61, 64)->item = ITM_KEYR + keyForZone[3];

	std::vector<byte> goodies = {
		ITM_HAMMERUP,
		ITM_HAMMERUP,
		ITM_HAMMERUP,
		ITM_HAMMERUP,
		ITM_HAMMERUP,
		ITM_PANTS,
		ITM_PANTS,
		ITM_PANTS,
		ITM_PANTS,
		ITM_REFLECT,
		ITM_REVERSE,
	};

	// now zone[0-2] are randomized betwen 0,1,2 for what goes in them
	for (int i = 0; i < 3; i++)	// fill each zone with what it wants
	{
		std::vector<byte> itemSpots;
		byte spots = 0;
		for (int j = coords[i * 4]; j <= coords[i * 4 + 2]; j++)
			for (int k = coords[i * 4 + 1]; k <= coords[i * 4 + 3]; k++)
			{
				if (map->GetTile(j, k)->floor == 96)
				{
					itemSpots.push_back((byte)j);
					itemSpots.push_back((byte)k);
					spots++;
				}
			}
		byte keySpot = Random(spots);
		for (int j = 0; j < spots; j++)
		{
			if (j == keySpot && keyForZone[i]!=255)
				map->GetTile(itemSpots[j * 2], itemSpots[j * 2 + 1])->item = ITM_KEYR + keyForZone[i];
			else
			{
				byte n = (byte)Random(goodies.size());
				map->GetTile(itemSpots[j * 2], itemSpots[j * 2 + 1])->item = goodies[n];
				goodies[n] = Random(2)?ITM_HEALTHPOT:ITM_MANAPOT;	// as items get taken, their slot goes to a potion
			}
		}

		if (zone[i] == 0)	// grimbleweed, guarded by spiders
		{
			for (int j = 0; j < 10; j++)
				SwampSpawn(map, MONS_BIGSPDR, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
			for (int j = 0; j < 30; j++)
				SwampSpawn(map, MONS_SPIDER, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
			for (int j = 0; j < 12; j++)
				SwampItem(map, ITM_GRIMBLEWEED, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
		}
		else if (zone[i] == 1)	// octons, and some bushes for spice
		{
			for (int j = 0; j < 12; j++)
				SwampSpawn(map, MONS_OCTOPUS, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
			for (int j = 0; j < 20; j++)
				SwampSpawn(map, MONS_SWAMPWEED, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
		}
		else if (zone[i] == 2)	// toadstools and the obvious guardians
		{
			for (int j = 0; j < 12; j++)
				SwampItem(map, ITM_TOADSTOOL, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
			for (int j = 0; j < 10; j++)
				SwampSpawn(map, MONS_SHROOM, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
			for (int j = 0; j < 5; j++)
				SwampSpawn(map, MONS_SHRMLORD, coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
		}
	}

	if (PlayerPassedLevel(player.worldNum, player.levelNum))
	{
		map->GetTile(61, 54)->floor = 86;
		strcpy(map->special[1].msg, "Skip to cooking with button!");
	}
}

bool SwampPotIsFull(void)
{
	return potPos == 2;
}

void SwampUpdate(Map* map)
{
	if (!GetGoodguy())
		return;
	int tx, ty, dx, dy;
	if (swampTicker > 0)
		swampTicker--;


	tx = GetGoodguy()->x / (TILE_WIDTH * FIXAMT);
	ty = GetGoodguy()->y / (TILE_HEIGHT * FIXAMT);
	if (PlayerPassedLevel(player.worldNum, player.levelNum))
	{
		if (tx == 61 && ty == 54 && (potPos < 3 || potPos==255))
		{
			potPos = 3;
			player.puzzleVar[2] = 1;
			map->GetTile(61, 54)->floor = 87;
			MakeNormalSound(SND_CHEATWIN);	// snicker
			potOrder[0] = swampRecipe[1];
			potContents[swampRecipe[1]] = swampRecipe[0];
			potOrder[1] = swampRecipe[3];
			potContents[swampRecipe[3]] = swampRecipe[2];
			potOrder[2] = swampRecipe[5];
			potContents[swampRecipe[5]] = swampRecipe[4];	// put all the right stuff in the pot
			map->GetTile(65, 56)->item = 0;	// open the door free of charge
			SwampEnableCooking();
		}
	}
	byte putting = 255;
	if (tx == 64 && ty == 62 && player.puzzleVar[0] > 0)	// left side of cauldron, grimbleweed
	{
		putting = 0;
		dx = FIXAMT * 2;
		dy = 0;
	}
	if (tx == 65 && ty == 63 && player.puzzleVar[1] > 0)	// bottom of cauldron, octon juice
	{
		putting = 1;
		dx = 0;
		dy = -FIXAMT * 2;
	}
	if (tx == 66 && ty == 62 && player.puzzleVar[2] > 0)	// right side of cauldron, toadstool
	{
		putting = 2;
		dx = -FIXAMT * 3 / 2;
		dy = 0;
	}

	if (putting != 255 && (potPos < 3 || potPos==255) && swampTicker == 0)	// we are putting in an ingredient!
	{
		swampTicker = 10;	// 3 ingredients per second
		if (potPos == 255)	// this is your first ingredient
		{
			potPos = 0;
		}
		else if (potOrder[potPos] != putting)	// you are putting in a different ingredient than the previous one
		{
			for (int i = 0; i < potPos; i++)
				if (potOrder[i] == putting)	// you have already put some of this in before, so you can't put it in again
					putting = 255;
			if (putting != 255)
			{
				potPos++;
				potOrder[potPos] = putting;
			}
		}
		if (putting != 255)
		{
			potOrder[potPos] = putting;
			player.puzzleVar[putting]--;
			potContents[putting]++;
			MakeNormalSound(SND_PURCHASE);
			AddParticle((tx * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (ty * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 20 * FIXAMT, dx, dy, FIXAMT * 4, 30, PART_INGREDIENT, putting);

		}
	}

	// cooking is in action
	if (potPos == 3) // not yet cooking, but done with ingredients
	{
		if (tx == 68 && ty == 61)	// button to cook
		{
			swampTemp = 0;
			swampCookTime = 0;
			potPos = 4;
		}
	}
	else if (potPos == 4)	// already cooking
	{
		BlowWigglySmoke(65*TILE_WIDTH * FIXAMT + Random(TILE_WIDTH * FIXAMT),
			62*TILE_HEIGHT*FIXAMT + Random(TILE_HEIGHT * FIXAMT), (32+MGL_random(32)) *FIXAMT, MGL_randoml(FIXAMT * 4));

		byte c[] = { 4,5,1,6,3,7 };	// colors for the flames
		byte color = swampTemp / 40;
		if (color > 5) color = 5;
		AddParticle((65 * TILE_WIDTH + Random(TILE_WIDTH)) * FIXAMT, (62 * TILE_HEIGHT + TILE_HEIGHT) * FIXAMT, 0, 0, 0, 0, 10, PART_COLORFIRE, c[color]);

		if (Random(10) == 0)
			map->TempTorch(64 + Random(3), 61 + Random(3), 5 + Random(12));
		if (tx == 68 && ty == 61)	// button to cook
		{
			if (swampTemp < 6 * 40-3)
				swampTemp += 3;
			if (!swampHaveHitBoil)
			{
				swampPreheatTime++;	// note that preheat time is only legal if it is spent raising temp. You can't hover it up and down and then do a quick boil at the end
				if (swampTemp >= boilTemp * 40)
				{
					swampHaveHitBoil = true;
					swampBoilTime++;
				}
			}
		}
		else
		{
			if(swampTemp>1)
				swampTemp -= 2;
		}

		if(swampHaveHitBoil) // once you have hit boil, you want to get as close to 3s of boil time as possible, and the rest of your time should be spent simmering, as much as possible
		{
			if (swampTemp >= boilTemp * 40 && swampTemp < (boilTemp + 1) * 40)
				swampBoilTime++;
			else if (swampTemp >= simmerTemp * 40 && swampTemp < (simmerTemp + 1) * 40)
				swampSimmerTime++;
		}
		swampCookTime++;
		if (swampCookTime == 10 * 30)	// 10s total cook!
		{
			potPos = 5;	// done cooking!
			PlayerGetItem(ITM_BRAIN, 65, 62);
			// report results
			int s;
			s = 130;	// better than perfect! Because there's some failure that's unavoidable as you shift between temps
			s -= abs(swampBoilTime - 3 * 30); // minus the amount you over- or under-boiled
			s -= abs(10 * 30 - swampPreheatTime - swampBoilTime - swampSimmerTime);	// minus the amount of time you spent not preheating, boiling, or simmering
			if (swampBoilTime == 0 || swampSimmerTime == 0)
				s = 0;	// you can't win if you never boiled or simmered
			if (s < 0) s = 0;
			swampScore = s;
			if (swampScore >= 100)
				EarnAchieve(Achievement::CHEF);
			ChallengeEvent(CE_COOK, swampScore);

			if (potOrder[0] != swampRecipe[1] || potOrder[1] != swampRecipe[3])	// you did things in the wrong order (you only have to check 2, obviously!)
			{
				swampScore = 0;	// total fail
				InitSpeech(43);
			}
			else
			{
				// need to search the recipe to find out how much of each thing
				byte needed[3];
				for (int i = 0; i < 3; i++)
				{
					needed[swampRecipe[i * 2+1]] = swampRecipe[i * 2];
				}
				if (potContents[0] != needed[0] || potContents[1] != needed[1] || potContents[2] != needed[2])
				{
					swampScore = 0;	// total fail also
					InitSpeech(44);
				}
				else // you got in the ingredients right, and in the right order
				{
					if (swampScore < 50)	// if you failed, then we need to know if it's under or over cooked
					{
						if (swampBoilTime < 30 * 3)
							InitSpeech(41);	// undercooked
						else
							InitSpeech(42);	// overcooked

					}
					else if (swampScore < 75)
						InitSpeech(47); // below 75% is fine, but she notes it is bad
					else
						InitSpeech(45); // above 75% is perfect, because cooking is hard
				}
			}
		}
		//SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%d > TT:%d B:%d S:%d F:%d\n", (byte)swampHaveHitBoil, swampCookTime, swampBoilTime, swampSimmerTime, 10 * 30 - swampBoilTime - swampSimmerTime);
	}
}

void GetSwampTemps(byte* boil, byte* simmer)
{
	(*boil) = boilTemp;
	(*simmer) = simmerTemp;
}

void SwampEnableCooking(void)
{
	potPos = 3;	// mark the putting-in phase over
	swampTemp = 0;
	swampCookTime = 0;
	swampBoilTime = 0;	// you have cooked at the right temperature for 0 seconds
	swampPreheatTime = 0;
	swampSimmerTime = 0;
	swampHaveHitBoil = false;
	player.puzzleVar[2] = 1;	// the display only shows if puzzleVar is non-zero, so let's make sure one is
}

char swampTempNames[6][16] = {
	"Red",
	"Yellow",
	"Green",
	"Purple",
	"Blue",
	"Cyan",
};

void RenderSwampRecipe(void)
{
	char s[48];
	RenderSkillBox(140 - 2, 200 - 2, SCRWID - 140 + 2, 460 + 2, 16, 0);
	RenderSkillBox(140, 200, SCRWID - 140, 460, 16, 27);
	CenterPrint(HALFWID, 200 + 5, "Swamp Stew", -31, 2);
	CenterPrint(HALFWID,200+25,"For The Mystic Soul", -31, 2);
	for (int i = 0; i < 3; i++)
	{
		if (swampRecipe[i * 2 + 1] == 0)
			sprintf(s, "%d Grimbleweed", swampRecipe[i * 2]);
		else if (swampRecipe[i * 2 + 1] == 1)
			sprintf(s, "%d Octon Juice", swampRecipe[i * 2]);
		else if (swampRecipe[i * 2 + 1] == 2)
			sprintf(s, "%d Toadstools", swampRecipe[i * 2]);
		CenterPrint(HALFWID, 200 + 60 + i * 40, s, -31, 2);
	}
	sprintf(s, "Boil over %s flame for", swampTempNames[boilTemp]);
	CenterPrint(HALFWID, 200 + 50 + 3 * 40 + 20, s, -31, 2);
	CenterPrint(HALFWID, 200 + 50 + 3 * 40 + 40, "3 seconds, then simmer", -31, 2);
	sprintf(s, "over %s flame until done", swampTempNames[simmerTemp]);
	CenterPrint(HALFWID, 200 + 50 + 3 * 40 + 60, s, -31, 2);
}

const char* GetSwampTempName(byte temp)
{
	if (temp >= 0 && temp < 6)
		return swampTempNames[temp];
	else
		return "???";
}

void GetSwampStats(byte *state,byte* temp, word* time,byte *score)
{
	if (potPos < 3 || potPos == 255)
		(*state) = 0;	// putting in ingredients
	else if (potPos == 3)
		(*state) = 1;	// not yet cooking
	else if (potPos == 4)
		(*state) = 2;	// cooking
	else if (potPos == 5)
		(*state) = 3;	// done, score is now valid!
	(*temp) = swampTemp;
	(*time) = swampCookTime;
	(*score) = swampScore;
}

void SwampDestroyCauldron(byte how)
{
	CurrentMap()->GetTile(65, 62)->item = 0;	// remove the cauldron
	if (how != 3)	// mushie one does not make a water floor
	{
		CurrentMap()->GetTile(64, 62)->floor = 20;	// replace the + shape under the cauldron with swamp water
		CurrentMap()->GetTile(65, 62)->floor = 20;
		CurrentMap()->GetTile(66, 62)->floor = 20;
		CurrentMap()->GetTile(65, 61)->floor = 20;
		CurrentMap()->GetTile(65, 63)->floor = 20;
		int tx, ty;
		tx = GetGoodguy()->x / (TILE_WIDTH * FIXAMT);
		ty = GetGoodguy()->y / (TILE_HEIGHT * FIXAMT);
		if ((tx >= 64 && tx <= 66 && ty == 62) ||
			(tx == 65 && ty >= 61 && ty <= 63))	// you are in what is now water
		{
			if (tx == 64 || tx == 66)
				ty = 61;
			else if (ty == 61)
				tx = 66;
			else
			{
				tx = 66;
				ty = 63;
			}
			GetGoodguy()->x = (tx * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
			GetGoodguy()->y = (ty * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;	// move you to a non-water tile
		}
	}
	BlowUpGuy((65 * TILE_WIDTH + TILE_WIDTH / 2-1),
		(62 * TILE_HEIGHT + TILE_HEIGHT / 2-1),
		(65 * TILE_WIDTH + TILE_WIDTH / 2+1),
		(62 * TILE_HEIGHT + TILE_HEIGHT / 2+1), 0, 1);	// make an explosion on the cauldron
	if (how == 0)	// tentacle
	{
		AddGuy((65 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT,
			(62 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT,
			0, MONS_OCTOTENT);
	}
	else if (how == 1) // magmazoids
	{
		AddGuy((64 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT,
			(62 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT,
			0, MONS_MAGMAZOID);
		AddGuy((66 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT,
			(62 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT,
			0, MONS_MAGMAZOID);
	}
	else if (how == 2)	// explosions
	{
		for (int i = 0; i < 4; i++)
		{
			int x, y;
			x = (63 * TILE_WIDTH + Random(5 * TILE_WIDTH)) * FIXAMT;
			y = (60 * TILE_HEIGHT + Random(5 * TILE_HEIGHT)) * FIXAMT;
			FireBullet(x, y, 0, BLT_SHOCKWAVE);
		}
		BlowUpGuy((65 * TILE_WIDTH + TILE_WIDTH / 2 - TILE_WIDTH*4),
			(62 * TILE_HEIGHT + TILE_HEIGHT / 2 - TILE_HEIGHT*4),
			(65 * TILE_WIDTH + TILE_WIDTH / 2 + TILE_WIDTH*4),
			(62 * TILE_HEIGHT + TILE_HEIGHT / 2 + TILE_HEIGHT*4), 0, 10);
		MakeNormalSound(SND_INFERNAL);
	}
	else if (how == 3) // mushies
	{
		for (int i = 0; i < 4; i++)
		{
			int x, y;
			x = (63 * TILE_WIDTH + Random(5 * TILE_WIDTH)) * FIXAMT;
			y = (60 * TILE_HEIGHT + Random(5 * TILE_HEIGHT)) * FIXAMT;
			AddGuy(x, y, 0, MONS_SHRMLORD);
		}
	}
}
//---------------------------- END OF SWAMP PUZZLE
byte hedgePuzzleState=0;
byte hedgePuzzleTimer;
byte hedgeCoords[] = {
		7,31,
		10,36,
		9,41,
		15,39,
		22,39,
		25,32,
		22,28,
};
void AmongHedgesPuzzleInit(Map* map)
{
	hedgePuzzleState = 0;
}

void AmongHedgesPuzzleUpdate(Map *map)
{
	int tx, ty;
	if (ClassicMode())
		return;

	if (!GetGoodguy())
		return;
	if (GotSkillShardInLevel(player.worldNum, player.levelNum))
		return;
	if (hedgePuzzleState == 8)
		return;

	if (hedgePuzzleState>0)	// timer ticking, shoot them all before it's gone
	{
		hedgePuzzleTimer--;
		if (hedgePuzzleTimer == 0)
		{
			for (int i = 0; i < 7; i++)
			{
				map->GetTile(hedgeCoords[i * 2], hedgeCoords[i * 2 + 1])->wall = 0;
				map->GetTile(hedgeCoords[i * 2], hedgeCoords[i * 2 + 1])->floor = 20;
			}
			map->GetTile(16, 33)->floor = 86;
			hedgePuzzleState = 0;
			MakeNormalSound(SND_UNAVAILABLE);
		}
		else if (((hedgePuzzleTimer % 15) == 0) || (hedgePuzzleTimer<30 && (hedgePuzzleTimer%5)==0))
			MakeNormalSound(SND_MENUCLICK);	// tick

		return;
	}
	if (hedgePuzzleState == 0)
	{
		tx = GetGoodguy()->x / (TILE_WIDTH * FIXAMT);
		ty = GetGoodguy()->y / (TILE_HEIGHT * FIXAMT);
		if (tx == 16 && ty == 33)
		{
			hedgePuzzleState = 1;
			hedgePuzzleTimer = 30 * 4;
			MakeNormalSound(SND_FLUSH);
			for (int i = 0; i < 7; i++)
			{
				ExplodeParticles2(PART_WATER, hedgeCoords[i * 2] * TILE_WIDTH + TILE_WIDTH / 2, hedgeCoords[i * 2 + 1] * TILE_HEIGHT + TILE_HEIGHT / 2, 0, 20, 10);
				map->GetTile(hedgeCoords[i * 2], hedgeCoords[i * 2 + 1])->wall = 31;
				map->GetTile(hedgeCoords[i * 2], hedgeCoords[i * 2 + 1])->floor = 100;
			}
		}
	}
}

void AmongHedgesShootCheck(Map* map, int x, int y)
{
	if (ClassicMode())
		return;

	if (hedgePuzzleState == 8)
		return;

	for (int i = 0; i < 7; i++)
	{
		if (x >= hedgeCoords[i * 2] - 1 && x <= hedgeCoords[i * 2] + 1 &&
			y >= hedgeCoords[i * 2 + 1] - 1 && y <= hedgeCoords[i * 2+1] + 1 &&
			map->GetTile(hedgeCoords[i*2], hedgeCoords[i*2+1])->wall == 31)
		{
			x = hedgeCoords[i * 2];
			y = hedgeCoords[i * 2 + 1];
			MakeNormalSound(SND_PURCHASE);
			hedgePuzzleState++;
			map->GetTile(x, y)->wall = 29;
			map->GetTile(x, y)->floor = 101;
			break;
		}
	}
	if (hedgePuzzleState == 8)
	{
		map->GetTile(16, 26)->item = ITM_SKILLSHARD;
		MakeNormalSound(SND_INFERNAL);
		FloaterParticles((16 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (26 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 1, 32, -1, 16);
		FloaterParticles((16 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (26 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 1, 10, 1, 16);
		map->BrightTorch(16, 26, 20, 10);
	}
}

void LostInWoodsPuzzleStep(Map* map, int x, int y)
{
	if (ClassicMode())
		return;

	int shardX,shardY;
	if (GotSkillShardInLevel(player.worldNum, player.levelNum))
		return;

	shardX = shardY = 0;
	for (int i = 0; i < map->width * map->height; i++)
	{
		if (map->map[i].floor == 96)	// the spot where the skill shard goes
		{
			shardX = i % map->width;
			shardY = i / map->width;
		}
	}

	mapTile_t* t = map->GetTile(x, y);
	if (t->floor >= 120 && t->floor <= 123 && (x<shardX-2 || x>shardX+2 || y<shardY-2 || y>shardY+2))	// triggering the ones around the shard itself don't count!
	{
		byte f = t->floor;
		for (int i = 0; i < map->width * map->height; i++)
		{
			if (map->map[i].floor == f)
			{
				map->map[i].floor = f + 20;	// light up all matching symbols
			}
		}

		MakeNormalSound(SND_INFERNAL);
		if (map->map[(shardX - 2)+shardY*map->width].floor >= 140 &&
			map->map[(shardX+2)+shardY*map->width].floor >= 140 &&
			map->map[shardX + (shardY-2) * map->width].floor >= 140 &&
			map->map[shardX + (shardY+2) * map->width].floor >= 140)	// all 4 around the spot are lit up
		{
			map->map[shardX+shardY*map->width].item = ITM_SKILLSHARD;
			MakeNormalSound(SND_PURCHASE);
			FloaterParticles((shardX * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (shardY * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 1, 32, -1, 16);
			FloaterParticles((shardX * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (shardY * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 1, 10, 1, 16);
		}
	}
}

std::vector<byte> hauntedInfo;	// sets of 4 bytes: X,Y,direction it sends ghosts,and which 'track' it is
byte hauntedWoodsGhosts;

void HauntedWoodsPuzzleInit(Map* map)
{
	if (ClassicMode())
		return;

	int nextTrack = 0;
	for (int i = 0; i < map->width; i++)
	{
		for (int j = 0; j < map->height; j++)
		{
			mapTile_t* t = map->GetTile(i,j);
			if (t->floor >= 120 && t->floor <= 123)	// ghost director tile
			{
				hauntedInfo.push_back((byte)i);
				hauntedInfo.push_back((byte)j);
				hauntedInfo.push_back(t->floor-120);	// 0,1,2,3 = up,down,left,right
				byte foundTrack = false;
				for (int k = 0; k < (int)hauntedInfo.size()-3; k += 4)
				{
					if (i == hauntedInfo[k] || j == hauntedInfo[k + 1])	// if we match the X or Y of an existing tile, then we are on its track
					{
						foundTrack = true;
						hauntedInfo.push_back(hauntedInfo[k + 3]);	// give it the same ID as the existing one
						break;
					}
				}
				if (!foundTrack)
					hauntedInfo.push_back(nextTrack++);

				t->floor = Random(3);	// replace it with grass
			}
		}
	}
	hauntedWoodsGhosts = 3;

	int usedTrack[3];
	for (int i = 0; i < 3; i++)
		usedTrack[i] = 999;

	for (int i = 0; i < 3; i++)	// add 3 ghosts of christmas past
	{
		bool bad = true;
		while (bad)
		{
			bad = false;
			int n = Random((int)(hauntedInfo.size() / 4))*4;
			for (int j = 0; j < i; j++)	// make sure we are on a unique track
			{
				if (hauntedInfo[n+3] == usedTrack[j])
				{
					bad = true;
					break;
				}
			}
			if (!bad)
			{
				AddGuy((hauntedInfo[n] * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (hauntedInfo[n+1] * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 0, MONS_GHOST);
				usedTrack[i] = hauntedInfo[n + 3];
				break;
			}
		}
	}
}

void HauntedPuzzleGhostControl(Map* map,Guy *me)
{
	if (ClassicMode())
		return;

	int tx, ty;
	tx = me->x / (TILE_WIDTH * FIXAMT);
	ty = me->y / (TILE_HEIGHT * FIXAMT);

	for(int i=0;i<(int)hauntedInfo.size();i+=4)
	{
		if (tx == hauntedInfo[i] && ty == hauntedInfo[i + 1])
		{
			me->mind = hauntedInfo[i + 2];	// tell it to go that direction
		}
	}
}

bool HauntedWoodsCastSpell(Map *map)
{
	if (ClassicMode())
		return true;

	if (player.worldNum != 1 || player.levelNum != 19 || hauntedWoodsGhosts == 0 || !GetGoodguy())
		return true;	// you are free to cast a spell
	RevealEctoplasm(GetGoodguy()->x, GetGoodguy()->y);
	return false;
}

void HauntedWoodsCatchGhost(Guy* me)
{
	if (ClassicMode())
		return;

	hauntedWoodsGhosts--;
	ChallengeEvent(CE_KILL, MONS_GHOST);
	me->type = 0;
	FloaterParticles(me->x, me->y, 7, 16, 1, 10);
	FloaterParticles(me->x, me->y, 7, 32, 1, 10);
	FloaterParticles(me->x, me->y, 7, 48, 1, 10);
	MakeNormalSound(SND_GOLEMDIE);
	CurrentMap()->BrightTorch((me->x / (TILE_WIDTH * FIXAMT)), (me->y / (TILE_HEIGHT * FIXAMT)), 31, 10);
	if (hauntedWoodsGhosts == 0)
	{
		for (int i = 0; i < CurrentMap()->width * CurrentMap()->height; i++)
			CurrentMap()->map[i].light = 0;	// light the world up!
		Guy *g=FindNearestOfType(0, 0, MONS_FRIENDLY);
		if (g) g->mind2 = 1;	// make it so we don't get her initial speech if you skipped talking to her
		InitSpeech(64);
	}
}

byte HauntedWoodsGhosts(void)
{
	return hauntedWoodsGhosts;
}

byte deepEndSeq;

void DeepEndPuzzleInit(void)
{
	deepEndSeq = 0;
}

void DeepEndPuzzleKill(Map* map, byte type)
{
	if (ClassicMode())
		return;

	//14,2,13
	//52,41-50,41
	if (GotSkillShardInLevel(player.worldNum, player.levelNum))
		return;
	if (type != MONS_EYEGUY && type != MONS_BONEHEAD && type != MONS_PEEPER)
		return;	// minions dying or whatever is ignored
	bool good = false;

	if ((deepEndSeq == 0 && type == MONS_PEEPER) ||
		(deepEndSeq == 1 && type == MONS_BONEHEAD) ||
		(deepEndSeq == 2 && type == MONS_EYEGUY))
		good = true;
	if (good)
	{
		map->GetTile(52 - deepEndSeq, 41)->floor = 23;
		map->GetTile(52 - deepEndSeq, 42)->floor = 34;
		deepEndSeq++;
	}
	else
	{
		deepEndSeq = 0;
		for (int i = 0; i < 3; i++)
		{
			map->GetTile(52 - i, 41)->floor = 35;
			map->GetTile(52 - i, 42)->floor = 35;
		}
		if (GetGoodguy())	// if you are out on the gangplank, you need to be sent back or you'll be stuck
		{
			int tx, ty;
			tx = GetGoodguy()->x / (TILE_WIDTH * FIXAMT);
			ty = GetGoodguy()->y / (TILE_HEIGHT * FIXAMT);
			if (tx >= 49 && tx <= 53 && ty == 41)
			{
				GetGoodguy()->x = (53 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
				GetGoodguy()->y = (41 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
			}
		}
	}
}

byte horkBoxBones = 0;
word lastHorkIndex;
void HorkBoxInit(Map* map)
{
	if (ClassicMode())
		return;

	SetupHorkboxes();
	horkBoxBones = 0;
	lastHorkIndex = 0;
	player.puzzleVar[0] = HORK_NONE;
	player.puzzleVar[2] = 0;	// how many boxes you've beaten. They get tougher as you go
	totalBrains = 8;	// one per horkbox, which drop them on kill, and then you have to get one more from Madam Kromch
}

void HorkBoxBones(Map* map, int x, int y)
{
	// rib 40
	// skull 41
	// leg 129
	// round 130
	// foot 131
	// teeth 132
	if (ClassicMode())
		return;

	byte seq[] = { 129,40,132,130,41,131,255 };
	if (horkBoxBones < 6 && !GotSkillShardInLevel(player.worldNum,player.levelNum))
	{
		mapTile_t* t = map->GetTile(x, y);
		if (t && t->floor == seq[horkBoxBones] && (x+y*map->width)!=lastHorkIndex)
		{
			horkBoxBones++;
			lastHorkIndex = x + y * map->width;
			if (horkBoxBones == 6)
			{
				NewMessage("Something thunked...", 60);
				ZapWall(map, 68, 71, 0);
				MakeNormalSound(SND_INFERNAL);
			}
		}
		else if ((x+y*map->width)!=lastHorkIndex && t && (t->floor == 40 || t->floor == 41 || (t->floor >= 129 && t->floor <= 132)))
			horkBoxBones = 0;	// step on a wrong bone and you reset
	}
}

byte orderUpTimer;
byte orderUpTemp, orderUpState, orderUpStews;
byte orderUpDeliveries,orderUpHot;

void OrderUpSetup(Map* map)
{
	if (ClassicMode())
		return;

	totalBrains = 1;
	// 14,99 - 46,126 = grimbleweed and toadstools
	int toads = Random(10) + 23;
	int grimbles = Random(10) + 23;
	while (toads > 0 || grimbles>0)
	{
		int x, y;
		x = Random(46 - 14+1) + 14;
		y = Random(126 - 99+1) + 99;

		mapTile_t* t = map->GetTile(x, y);
		if (t && t->item == 0 && t->wall == 0)
		{
			if (toads > 0)
			{
				t->item = ITM_TOADSTOOL;
				toads--;
			}
			else if (grimbles > 0)
			{
				t->item = ITM_GRIMBLEWEED;
				grimbles--;
			}
		}
	}
	// the octons are already placed in the sewer beyond

	orderUpTimer = 0;
	orderUpState = 0;	// haven't talked to witch yet
	orderUpStews = 0;
	orderUpTemp = 0;
	orderUpHot = 0;
	orderUpDeliveries = 0;
	player.puzzleVar[0] = 0;
	player.puzzleVar[1] = 0;
	player.puzzleVar[2] = 0;
	if (PlayerPassedLevel(player.worldNum, player.levelNum))
	{
		map->GetTile(1, 114)->wall = 0;
		map->GetTile(1, 114)->floor = 0;
	}
}

void OrderUpUpdate(Map* map)
{
	// water alternates flushing:
	// 14,66-15,74,
	// 18,58-19,74, 20,60-21,62, and a water gate at 18,57-19,57 (wall 61,flr 86 when wet, wall 62, flr 87 when dry)
	//  22,63-28,65, 22,66-24,74
	//  (130=begin filling, 85=begin draining)

	if (!GetGoodguy() || ClassicMode())
		return;
	if ((orderUpState == 0 || orderUpState == 1) && player.puzzleVar[0]<99 && GetGoodguy()->mapx == 1 && GetGoodguy()->mapy == 115)
	{
		player.puzzleVar[0] = 99;
		player.puzzleVar[1] = 99;
		player.puzzleVar[2] = 99;
		orderUpState = 1;
		MakeNormalSound(SND_INFERNAL);
	}
	// if the water takes you, let's get you to safety instead of double-instakilling you
	if (GetGoodguy()->IsInTileRect(14,66,15,74))
		SetLastSafeXY(13*TILE_WIDTH*FIXAMT,70*TILE_HEIGHT*FIXAMT);
	if (GetGoodguy()->IsInTileRect(18,58,21,74))
		SetLastSafeXY(17 * TILE_WIDTH * FIXAMT, 69 * TILE_HEIGHT * FIXAMT);
	if (GetGoodguy()->IsInTileRect(22,63,28,74))
		SetLastSafeXY(23 * TILE_WIDTH * FIXAMT, 62 * TILE_HEIGHT * FIXAMT);
	if (orderUpTimer == 0)	// empty them all
	{
		MakeSound(SND_FLUSH, (18 * TILE_WIDTH * FIXAMT), (70 * TILE_HEIGHT * FIXAMT), SND_CUTOFF, 100);
		map->FillRect(14, 66, 15, 74, 85, 0);

		map->FillRect(18, 58, 19, 74, 85, 0);
		map->FillRect(20, 60, 21, 62, 85, 0);
		map->FillRect(18, 57, 19, 57, 87, 62);

		map->FillRect(22, 63, 24, 74, 85, 0);
		map->FillRect(25, 71, 25, 74, 85, 0);
		map->FillRect(26, 63, 28, 74, 85, 0);
	}
	else if (orderUpTimer == 30 * 1+15)
	{
		MakeSound(SND_FLUSH, (15 * TILE_WIDTH * FIXAMT), (70 * TILE_HEIGHT * FIXAMT), SND_CUTOFF, 100);
		map->FillRect(14, 66, 15, 74, 130, 0);
	}
	else if (orderUpTimer == 30 * 3)
	{
		MakeSound(SND_FLUSH, (18 * TILE_WIDTH * FIXAMT), (60 * TILE_HEIGHT * FIXAMT), SND_CUTOFF, 100);
		map->FillRect(18, 58, 19, 74, 130, 0);
		map->FillRect(20, 60, 21, 62, 130, 0);
		map->FillRect(18, 57, 19, 57, 86, 61);
	}
	else if (orderUpTimer == 30 * 4+15)
	{
		MakeSound(SND_FLUSH, (22 * TILE_WIDTH * FIXAMT), (66 * TILE_HEIGHT * FIXAMT), SND_CUTOFF, 100);
		map->FillRect(22, 63, 24, 74, 130, 0);
		map->FillRect(25, 71, 25, 74, 130, 0);
		map->FillRect(26, 63, 28, 74, 130, 0);
	}
	else if (orderUpTimer == 30 * 6)
		orderUpTimer = 255;

	orderUpTimer++;
	if (orderUpState == 2)	// delivering
	{
		if((orderUpTimer%10)==0 && orderUpTemp>0)
			orderUpTemp--;

		// delivery 1: 22,52-24,52 - bowl ends up at 21,49
		if (GetGoodguy()->IsInTileRect(22, 52, 24, 52) && (orderUpDeliveries & 1) == 0)
		{
			orderUpDeliveries |= 1;
			orderUpStews++;
			map->GetTile(21, 49)->item = ITM_STEW;
			if (orderUpTemp > 80)
			{
				NewMessage("Mmm, piping hot!", 60);
				orderUpHot++;
			}
			else
				NewMessage("Bluh, lukewarm mush.", 60);
		}
		// delivery 2: 30,75-31,75 - bowl ends up at 33,72
		if (GetGoodguy()->IsInTileRect(30,75,31,75) && (orderUpDeliveries & 2) == 0)
		{
			orderUpDeliveries |= 2;
			orderUpStews++;
			map->GetTile(33,72)->item = ITM_STEW;
			if (orderUpTemp > 80)
			{
				NewMessage("Yikes, I'm burning my mouth!", 60);
				orderUpHot++;
			}
			else
				NewMessage("Ugh, it's not hot.", 60);
		}
		// delivery 3: 36,68-37,68 - bowl ends up at 32,65
		if (GetGoodguy()->IsInTileRect(36, 68, 37, 68) && (orderUpDeliveries & 4) == 0)
		{
			orderUpDeliveries |= 4;
			orderUpStews++;
			map->GetTile(32,65)->item = ITM_STEW;
			if (orderUpTemp > 80)
			{
				NewMessage("Hoo, warm and delicious!", 60);
				orderUpHot++;
			}
			else
				NewMessage("Gag! I guess it's food.", 60);
		}
		// delivery 4: 57,66-58,66 - bowl ends up at 55,63
		if (GetGoodguy()->IsInTileRect(57,66,58,66) && (orderUpDeliveries & 8) == 0)
		{
			orderUpDeliveries |= 8;
			orderUpStews++;
			map->GetTile(55,63)->item = ITM_STEW;
			if (orderUpTemp > 80)
			{
				NewMessage("Mmm, it's warming my belly.", 60);
				orderUpHot++;
			}
			else
				NewMessage("This is cold and greasy.", 60);
		}
		if (orderUpStews == 4 && orderUpState==2)	// you got them all!
		{
			map->GetTile(5, 109)->item = ITM_BRAIN;
			orderUpState = 3;
			if (orderUpHot == 4)
				EarnAchieve(Achievement::DELIVERY);
			if (orderUpTemp > 0)
			{
				InitSpeech(85);
				if (GotRunePouchInLevel(player.worldNum, player.levelNum))
					map->GetTile(4, 111)->item = ITM_DIAMOND;
				else
					map->GetTile(4, 111)->item = ITM_RUNEPOUCH;
			}
			else
				InitSpeech(86);
		}
	}
}

void GetOrderUpStats(byte* state, byte* stews, byte* temp)
{
	(*state) = orderUpState;
	(*stews) = orderUpStews;
	(*temp) = orderUpTemp;
}

void OrderUpSetState(byte s)
{
	orderUpState = s;
	if (s == 2)	// delivering stew
	{
		player.puzzleVar[0] = 0;
		player.puzzleVar[1] = 0;
		player.puzzleVar[2] = 0;
		orderUpTemp = 240;
		CurrentMap()->GetTile(7, 111)->item = ITM_KEYR;
	}
}
