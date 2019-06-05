#include "editor.h"
#include "map.h"
#include "water.h"
#include "music.h"

#define PLOPRATE	5

#define EDITMODE_EDIT		0
#define EDITMODE_PICKFLR	1
#define EDITMODE_PICKWALL	2
#define EDITMODE_PICKWALLFLR 3
#define EDITMODE_PICKNEXT	4
#define EDITMODE_PICKITEM   5
#define EDITMODE_PICKENEMY  6
#define EDITMODE_TERRAIN	7
#define EDITMODE_FILE		8
#define EDITMODE_MAPMENU	9
#define EDITMODE_SPECIAL	10
#define EDITMODE_TILELOAD	11
#define EDITMODE_PICKRANDOM 12

static char lastKey=0;
static byte showPwrUps;
static char pwrUpTxt[128];
static char pwrUpTxt2[128];
static char pwrUpTxt3[128];
static byte warning;

static MGLDraw *editmgl;

// error: 'curMap' was declared 'extern' and later 'static'
#define curMap editorMap

static world_t	world;
static Map		*curMap;
static byte		curMapNum;
static int	   mouseX,mouseY;
static int	   tileX,tileY;

static byte  showStats=0;
static dword gameStartTime,visFrameCount,updFrameCount;
static word  numRunsToMakeUp;

static byte plopCounter=0;
static byte msButton=0;
static byte showTags;

static int tileOffset;

byte editing=0;

byte editMode=EDITMODE_EDIT;

static editopt_t editopt={MAP_SHOWLIGHTS|MAP_SHOWWALLS|MAP_SHOWITEMS|MAP_SHOWBADGUYS|MAP_SHOWSPECIALS,
						PLOP_FLOOR,1,1,0,0,1,0,1,1,0,0,0,0};

byte InitEditor(void)
{
	StopSong();
	JamulSoundPurge();
	NewWorld(&world,editmgl);
	curMap=world.map[0];
	curMapNum=0;

	mouseX=320;
	mouseY=240;
	PutCamera(0,0);
	gameStartTime=timeGetTime();
	InitGuys(MAX_MAPMONS);
	lastKey=0;
	InitPlayer(INIT_GAME,0,0);
	tileOffset=0;
	showTags=1;
	editing=1;
	SetupWater();
	warning=255;
	return 1;
}

void ExitEditor(void)
{
	ExitGuys();
	FreeWorld(&world);
	PurgeMonsterSprites();
	editing=0;
}

void ItemPickerClick(void)
{
	int i,x,y;

	editMode=EDITMODE_EDIT;
	x=2;
	y=2;
	for(i=1;i<MAX_ITMS;i++)
	{
		if(mouseX>x-1 && mouseX<x+101 && mouseY>y-1 && mouseY<y+14)
		{
			editopt.curItem=(byte)i;
			return;
		}
		x+=102;
		if(x>640-100)
		{
			x=2;
			y+=14;
		}
	}
}

void EnemyPickerClick(void)
{
	int i,x,y;

	editMode=EDITMODE_EDIT;
	x=2;
	y=2;
	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(mouseX>x-1 && mouseX<x+201 && mouseY>y-1 && mouseY<y+15)
		{
			editopt.curBadguy=(byte)i;
			return;
		}
		x+=202;
		if(x>640-200)
		{
			x=2;
			y+=15;
		}
	}
}

void PickerMouseClick(void)
{
	int n;

	n=(mouseX/TILE_WIDTH)+(mouseY/TILE_HEIGHT)*20;

	if(n>199)	// clicked outside the display
	{
		if(mouseX>560 && mouseY>468)
		{
			tileOffset+=200;
			if(tileOffset>=NUMTILES)
				tileOffset=0;
		}
		else
		{
			if(editMode!=EDITMODE_PICKNEXT)
				editMode=EDITMODE_EDIT;
			else
				editMode=EDITMODE_TERRAIN;	// picking next is a submenu of terrain editing
			return;
		}
	}
	else
	{
		n+=tileOffset;
		if(editMode==EDITMODE_PICKFLR)
			editopt.curFloor=n;
		else if(editMode==EDITMODE_PICKWALL)
			editopt.curWall=n;
		else if(editMode==EDITMODE_PICKWALLFLR)
			editopt.curWallFloor=n;
		else if(editMode==EDITMODE_PICKRANDOM)
			editopt.curRandom=n;
		else if(editMode==EDITMODE_PICKNEXT)
			world.terrain[editopt.curFloor].next=n;

		if(editMode!=EDITMODE_PICKNEXT)
			editMode=EDITMODE_EDIT;
		else
			editMode=EDITMODE_TERRAIN;	// picking next is a submenu of terrain editing
	}
}

void TerrainMouseClick(void)
{
	int n;

	n=(mouseX/TILE_WIDTH)+(mouseY/TILE_HEIGHT)*20;

	if(n>199)	// clicked outside the display
	{
		if(mouseX>4 && mouseX<5+TILE_WIDTH && mouseY>397 && mouseY<398+TILE_HEIGHT)
		{
			editMode=EDITMODE_PICKNEXT;
		}
		if(mouseX>4 && mouseX<16 && mouseY>249 && mouseY<249+12)
			world.terrain[editopt.curFloor].flags^=TF_SOLID;
		if(mouseX>4 && mouseX<16 && mouseY>261 && mouseY<261+12)
			world.terrain[editopt.curFloor].flags^=TF_ICE;
		if(mouseX>4 && mouseX<16 && mouseY>273 && mouseY<273+12)
			world.terrain[editopt.curFloor].flags^=TF_SKYFX;
		if(mouseX>4 && mouseX<16 && mouseY>285 && mouseY<285+12)
			world.terrain[editopt.curFloor].flags^=TF_WATER;
		if(mouseX>4 && mouseX<16 && mouseY>297 && mouseY<297+12)
			world.terrain[editopt.curFloor].flags^=TF_LAVA;
		if(mouseX>4 && mouseX<16 && mouseY>309 && mouseY<309+12)
			world.terrain[editopt.curFloor].flags^=TF_PUSHY;
		if(mouseX>4 && mouseX<16 && mouseY>321 && mouseY<321+12)
			world.terrain[editopt.curFloor].flags^=TF_PUSHON;
		if(mouseX>4 && mouseX<16 && mouseY>333 && mouseY<333+12)
			world.terrain[editopt.curFloor].flags^=TF_ANIM;
		if(mouseX>4 && mouseX<16 && mouseY>345 && mouseY<345+12)
			world.terrain[editopt.curFloor].flags^=TF_STEP;
		if(mouseX>4 && mouseX<16 && mouseY>357 && mouseY<357+12)
			world.terrain[editopt.curFloor].flags^=TF_SKY;
		if(mouseX>4 && mouseX<16 && mouseY>369 && mouseY<370+12)
			world.terrain[editopt.curFloor].flags^=TF_TRANS;
		if(mouseX>249 && mouseX<249+12 && mouseY>249 && mouseY<249+12)
			world.terrain[editopt.curFloor].flags^=TF_NOSHADOW;
		if(mouseX>249 && mouseX<249+12 && mouseY>261 && mouseY<261+12)
			world.terrain[editopt.curFloor].flags^=TF_CLIFF;
		if(mouseX>249 && mouseX<249+12 && mouseY>273 && mouseY<273+12)
			world.terrain[editopt.curFloor].flags^=TF_FIRE;
		if(mouseX>249 && mouseX<249+12 && mouseY>285 && mouseY<285+12)
			world.terrain[editopt.curFloor].flags^=TF_NOBULLET;
		if(mouseX>249 && mouseX<249+12 && mouseY>297 && mouseY<297+12)
			world.terrain[editopt.curFloor].flags^=TF_WATERFX;
		if(mouseX>560 && mouseY>468)
		{
			tileOffset+=200;
			if(tileOffset>=NUMTILES)
				tileOffset=0;
		}
	}
	else
	{
		// pick the current floor
		editopt.curFloor=n+tileOffset;
	}
}

void AddMapBadguy(int x,int y,byte type)
{
	int i,cx,cy;

	if(x<0 || x>curMap->width-1 || y<0 || y>curMap->height-1)
		return;

	if(curMap->map[x+y*curMap->width].item)
		return;	// can't have a badguy sitting on an item

	if(curMap->map[x+y*curMap->width].wall)
		return;	// really not supposed to sit on a wall either

	for(i=0;i<MAX_MAPMONS;i++)
		if((curMap->badguy[i].type) && (curMap->badguy[i].x==x) && (curMap->badguy[i].y==y))
			return;	// somebody is already on this square

	// only one bouapha allowed
	if(type==MONS_LOONY)
	{
		for(i=0;i<MAX_MAPMONS;i++)
			if(curMap->badguy[i].type==MONS_LOONY)
			{
				DeleteGuy((curMap->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					   (curMap->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					   curMap->badguy[i].type);
				curMap->badguy[i].type=0;
			}
	}

	// now add the new guy
	for(i=0;i<MAX_MAPMONS;i++)
		if(curMap->badguy[i].type==0)
		{
			curMap->badguy[i].type=type;
			curMap->badguy[i].x=x;
			curMap->badguy[i].y=y;
			GetCamera(&cx,&cy);
			AddGuy((x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
				   (y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
				   0,
				   type);
			PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
			return;
		}
}

void AddItem(int x,int y,byte type)
{
	int i;

	if(x<0 || x>curMap->width-1 || y<0 || y>curMap->height-1)
		return;

	if(curMap->map[x+y*curMap->width].wall)
		return;	// really not supposed to sit on a wall

	for(i=0;i<MAX_MAPMONS;i++)
		if((curMap->badguy[i].type) && (curMap->badguy[i].x==x) && (curMap->badguy[i].y==y))
			return;	// somebody is already on this square

	curMap->map[x+y*curMap->width].item=type;
}

void Delete(int x,int y)
{
	int i;

	if(x<0 || x>curMap->width-1 || y<0 || y>curMap->height-1)
		return;

	for(i=0;i<MAX_MAPMONS;i++)
		if((curMap->badguy[i].type) && (curMap->badguy[i].x==x) && (curMap->badguy[i].y==y))
		{
			DeleteGuy((x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
				   (y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
				   curMap->badguy[i].type);
			curMap->badguy[i].type=0;
		}
	curMap->map[x+y*curMap->width].item=0;

	for(i=0;i<MAX_SPECIAL;i++)
		if(curMap->special[i].trigger && curMap->special[i].x==x && curMap->special[i].y==y)
		{
			memset(&curMap->special[i],0,sizeof(special_t));
		}
}

void PlopSpecial(int x,int y)
{
	int i;

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(curMap->special[i].trigger && curMap->special[i].x==x && curMap->special[i].y==y)
		{
			editMode=EDITMODE_SPECIAL;
			InitSpclDialog(&curMap->special[i],i,&world,curMapNum);
			return;
		}
	}
	// if you got here, there is no special already on the space
	for(i=0;i<MAX_SPECIAL;i++)
		if(!curMap->special[i].trigger)
		{
			curMap->special[i].x=x;
			curMap->special[i].y=y;
			curMap->special[i].trigX=x;
			curMap->special[i].trigY=y;
			editMode=EDITMODE_SPECIAL;
			InitSpclDialog(&curMap->special[i],i,&world,curMapNum);
			return;
		}
}

void EditorMenuClick(void)
{
	// check for clicking on current plop
	if(editopt.plopMode==PLOP_FLOOR)
	{
		if(mouseX>601 && mouseX<602+TILE_WIDTH && mouseY>449 && mouseY<450+TILE_HEIGHT)
			editMode=EDITMODE_PICKFLR;
	}
	else if(editopt.plopMode==PLOP_WALL)
	{
		if(mouseX>601 && mouseX<602+TILE_WIDTH && mouseY>449 && mouseY<450+TILE_HEIGHT)
			editMode=EDITMODE_PICKWALL;
		if(mouseX>601 && mouseX<602+TILE_WIDTH && mouseY>449-24 && mouseY<450-24+TILE_HEIGHT)
			editMode=EDITMODE_PICKWALLFLR;
	}
	else if(editopt.plopMode==PLOP_ITEM)
	{
		if(mouseX>601 && mouseX<640 && mouseY>420)
			editMode=EDITMODE_PICKITEM;
	}
	else if(editopt.plopMode==PLOP_BADGUY)
	{
		if(mouseX>601 && mouseX<640 && mouseY>420)
			editMode=EDITMODE_PICKENEMY;
	}
	else if(editopt.plopMode==PLOP_RANDOM)
	{
		if(mouseX>601 && mouseX<602+TILE_WIDTH && mouseY>451 && mouseY<450+TILE_HEIGHT)
			editMode=EDITMODE_PICKFLR;
		if(mouseX>601 && mouseX<602+TILE_WIDTH && mouseY>449-27 && mouseY<450-27+TILE_HEIGHT)
			editMode=EDITMODE_PICKRANDOM;
	}

	// plopmode checking
	if(mouseX>0 && mouseX<41 && mouseY>421 && mouseY<422+14)
		editopt.plopMode=PLOP_FLOOR;
	if(mouseX>0 && mouseX<41 && mouseY>437 && mouseY<438+14)
		editopt.plopMode=PLOP_WALL;
	if(mouseX>0 && mouseX<41 && mouseY>453 && mouseY<454+14)
		editopt.plopMode=PLOP_ITEM;
	if(mouseX>41 && mouseX<83 && mouseY>421 && mouseY<422+14)
		editopt.plopMode=PLOP_BADGUY;
	if(mouseX>41 && mouseX<83 && mouseY>437 && mouseY<438+14)
		editopt.plopMode=PLOP_LIGHT;
	if(mouseX>41 && mouseX<83 && mouseY>453 && mouseY<454+14)
		editopt.plopMode=PLOP_DARK;
	if(mouseX>83 && mouseX<125 && mouseY>421 && mouseY<422+14)
		editopt.plopMode=PLOP_SMOOTH;
	if(mouseX>83 && mouseX<125 && mouseY>437 && mouseY<438+14)
		editopt.plopMode=PLOP_TORCH;
	if(mouseX>83 && mouseX<125 && mouseY>453 && mouseY<454+14)
		editopt.plopMode=PLOP_SPECIAL;

	// toggling display flags
	if(mouseX>129 && mouseX<171 && mouseY>421 && mouseY<422+14)
		editopt.displayFlags^=MAP_SHOWLIGHTS;
	if(mouseX>129 && mouseX<171 && mouseY>437 && mouseY<438+14)
		editopt.displayFlags^=MAP_SHOWWALLS;
	if(mouseX>129 && mouseX<171 && mouseY>453 && mouseY<454+14)
		editopt.displayFlags^=MAP_SHOWITEMS;
	if(mouseX>171 && mouseX<213 && mouseY>421 && mouseY<422+14)
		editopt.displayFlags^=MAP_SHOWBADGUYS;
	if(mouseX>171 && mouseX<213 && mouseY>437 && mouseY<438+14)
		editopt.displayFlags^=MAP_SHOWSPECIALS;
	if(mouseX>171 && mouseX<213 && mouseY>453 && mouseY<454+14)
		showTags=1-showTags;

	// song changing/playing
	if(mouseX>215 && mouseX<227 && mouseY>421 && mouseY<422+14)
	{
		curMap->song--;
		if(curMap->song<1)
		{
			curMap->song=99;
			if(curMap->song>99)
				curMap->song=99;
		}
	}
	if(mouseX>289 && mouseX<301 && mouseY>421 && mouseY<422+14)
	{
		curMap->song++;
		if(curMap->song>99)
		{
			curMap->song=1;
		}
	}
	if(mouseX>227 && mouseX<289 && mouseY>421 && mouseY<422+14)
	{
		// playing music
	}

	// file & world menus
	if(mouseX>215 && mouseX<301 && mouseY>437 && mouseY<438+14)
	{
		editMode=EDITMODE_FILE;
		InitFileDialog();
	}
	if(mouseX>215 && mouseX<301 && mouseY>453 && mouseY<454+14)
	{
		editMode=EDITMODE_MAPMENU;
		InitMapDialog(&world,curMapNum);
	}

	// edit terrain
	if(mouseX>303 && mouseX<389 && mouseY>421 && mouseY<422+14)
	{
		editMode=EDITMODE_TERRAIN;
	}
	// load tiles
	if(mouseX>303 && mouseX<389 && mouseY>437 && mouseY<438+14)
	{
		editMode=EDITMODE_TILELOAD;
		InitTileDialog();
	}
	// brush size
	if(mouseX>391 && mouseX<443 && mouseY>421 && mouseY<422+14)
	{
		editopt.brushSize++;
		if(editopt.brushSize>10)
			editopt.brushSize=1;
	}
	if(mouseX>391 && mouseX<443 && mouseY>462 && mouseY<463+14)
	{
		editopt.brushSize--;
		if(editopt.brushSize<1)
			editopt.brushSize=10;
	}

	// map flags
	if(mouseX>445 && mouseX<487 && mouseY>421 && mouseY<422+14)
		curMap->flags^=MAP_SNOWING;
	if(mouseX>445 && mouseX<487 && mouseY>437 && mouseY<438+14)
		curMap->flags^=MAP_LITDUNGEON;
	if(mouseX>445 && mouseX<487 && mouseY>453 && mouseY<454+14)
		curMap->flags^=MAP_SECRET;
	if(mouseX>487 && mouseX<529 && mouseY>421 && mouseY<422+14)
		curMap->flags^=MAP_TORCHLIT;
	if(mouseX>487 && mouseX<529 && mouseY>437 && mouseY<438+14)
		curMap->flags^=MAP_STARS;


	plopCounter=100;
}

word GetFloor(int x,int y,word def)
{
	if(x<0 || y<0 || x>=curMap->width || y>=curMap->height)
		return def;
	return curMap->map[x+y*curMap->width].floor;
}

word SnowCheck(word v)
{
	if((v<=54 && (v%20)>=3 && (v%20)<=14) ||
		(v>=63 && v<=65) || (v>=83 && v<=85) || (v>=103 && v<=105) ||
		(v>=69 && v<=71) || (v>=89 && v<=91) || (v>=109 && v<=111) ||
		(v>=123 && v<=128) || (v>=143 && v<=148) || (v>=163 && v<=168))
		return 1;
	else
		return 0;
}

word DirtCheck(word v)
{
	if((v>=60 && v<120 && ((v%20)<=2  || ((v%20)>=6 && (v%20)<=8))) ||
		(v>=120 && v<180 && ((v%20)<=2)))
		return 1;
	else
		return 0;
}

word ProcessFloorChoice(int x,int y,word f)
{
	word threeby;
	word n[4];

	threeby=0;

	threeby=(x%3)+(y%3)*20;

	switch(f)
	{
		case 0:	// basic rock ground, solid 3x3 pattern
			if(rand()%8==0)
				return threeby+15;
			else
				return threeby;
			break;
		case 3:	// basic snow, 3x3 pattern
			if(rand()%5==0)
				return threeby+63;
			else
				return threeby+3;
			break;
		case 60:	// basic dirt, 3x3 pattern
			if(rand()%5==0)
				return threeby+120;
			else
				return threeby+60;
			break;
		case 75:	// basic ice, 3x3 pattern
			return threeby+75;
			break;
		case 6:	// snow edge piece
			n[0]=SnowCheck(GetFloor(x+1,y,3));
			n[1]=SnowCheck(GetFloor(x,y+1,3));
			n[2]=SnowCheck(GetFloor(x-1,y,3));
			n[3]=SnowCheck(GetFloor(x,y-1,3));
			threeby=21;	// center, all snow
			if(n[0]==0 && n[1]==1 && n[2]==1 && n[3]==1)
				threeby=22;	// right edge of snow
			if(n[0]==1 && n[1]==0 && n[2]==1 && n[3]==1)
				threeby=41;	// bottom edge of snow
			if(n[0]==1 && n[1]==1 && n[2]==0 && n[3]==1)
				threeby=20;	// left edge of snow
			if(n[0]==1 && n[1]==1 && n[2]==1 && n[3]==0)
				threeby=1;	// top edge of snow
			if(n[0]==0 && n[1]==0 && n[2]==1 && n[3]==1)
				threeby=42;	// lower right edge of snow
			if(n[0]==1 && n[1]==0 && n[2]==0 && n[3]==1)
				threeby=40;	// lower left edge of snow
			if(n[0]==1 && n[1]==1 && n[2]==0 && n[3]==0)
				threeby=0;	// upper left edge of snow
			if(n[0]==0 && n[1]==1 && n[2]==1 && n[3]==0)
				threeby=2;	// upper right edge of snow
			return threeby+6+(rand()%3)*3;
			break;
		case 66:	// rock/dirt interface
			n[0]=DirtCheck(GetFloor(x+1,y,3));
			n[1]=DirtCheck(GetFloor(x,y+1,3));
			n[2]=DirtCheck(GetFloor(x-1,y,3));
			n[3]=DirtCheck(GetFloor(x,y-1,3));
			threeby=21;	// center, all dirt
			if(n[0]==0 && n[1]==1 && n[2]==1 && n[3]==1)
				threeby=22;	// right edge of dirt
			if(n[0]==1 && n[1]==0 && n[2]==1 && n[3]==1)
				threeby=41;	// bottom edge of dirt
			if(n[0]==1 && n[1]==1 && n[2]==0 && n[3]==1)
				threeby=20;	// left edge of dirt
			if(n[0]==1 && n[1]==1 && n[2]==1 && n[3]==0)
				threeby=1;	// top edge of dirt
			if(n[0]==0 && n[1]==0 && n[2]==1 && n[3]==1)
				threeby=42;	// lower right edge of dirt
			if(n[0]==1 && n[1]==0 && n[2]==0 && n[3]==1)
				threeby=40;	// lower left edge of dirt
			if(n[0]==1 && n[1]==1 && n[2]==0 && n[3]==0)
				threeby=0;	// upper left edge of dirt
			if(n[0]==0 && n[1]==1 && n[2]==1 && n[3]==0)
				threeby=2;	// upper right edge of dirt
			return threeby+66;
			break;
		case 126:	// snow/ice interface
			n[0]=SnowCheck(GetFloor(x+1,y,3));
			n[1]=SnowCheck(GetFloor(x,y+1,3));
			n[2]=SnowCheck(GetFloor(x-1,y,3));
			n[3]=SnowCheck(GetFloor(x,y-1,3));
			threeby=21;	// center, all snow
			if(n[0]==0 && n[1]==1 && n[2]==1 && n[3]==1)
				threeby=22;	// right edge of snow
			if(n[0]==1 && n[1]==0 && n[2]==1 && n[3]==1)
				threeby=41;	// bottom edge of snow
			if(n[0]==1 && n[1]==1 && n[2]==0 && n[3]==1)
				threeby=20;	// left edge of snow
			if(n[0]==1 && n[1]==1 && n[2]==1 && n[3]==0)
				threeby=1;	// top edge of snow
			if(n[0]==0 && n[1]==0 && n[2]==1 && n[3]==1)
				threeby=42;	// lower right edge of snow
			if(n[0]==1 && n[1]==0 && n[2]==0 && n[3]==1)
				threeby=40;	// lower left edge of snow
			if(n[0]==1 && n[1]==1 && n[2]==0 && n[3]==0)
				threeby=0;	// upper left edge of snow
			if(n[0]==0 && n[1]==1 && n[2]==1 && n[3]==0)
				threeby=2;	// upper right edge of snow
			return threeby+126;
			break;
		case 18:	// cabin floor
			return 18+(x%2)+20*(y%2);
			break;
		case 401:	//top edge snow
			return 401+(x%3);
			break;
		case 461:	//bottom edge snow
			return 461+(x%3);
			break;
		case 406:	//top edge snow with cliff
			return 406+(x%3);
			break;
		case 466:	//top edge snow with cliff
			return 466+(x%3);
			break;
		case 481:	//right edge of snow
			return 481+(y%3)*20;
			break;
		case 482:	//left edge of snow
			return 482+(y%3)*20;
			break;
		case 484:	//right edge of snow+cliff
			return 484+(y%3)*20;
			break;
		case 485:	//left edge of snow+cliff
			return 485+(y%3)*20;
			break;
		case 452:	//left edge of cliff
			return 452+(y%3)*20;
			break;
		case 453:	//right edge of cliff
			return 453+(y%3)*20;
			break;
		case 486:	//left edge of cliff
			return 486+(x%3)+(y%3)*20;
			break;
		case 489:	//ladder
			return 489+(y%3)*20;
			break;
		case 434:	//cliff/cliff left
			return 434+(y%3)*20;
			break;
		case 435:	//cliff/cliff right
			return 435+(y%3)*20;
			break;
		case 534:
			return 534+(rand()%5);
			break;
		case 560:
			return 560+(x%3)+(y%3)*20;
			break;
		case 340:
			return 340+(x%3)+(y%3)*20;
			break;
		case 641: // purple stone
			return 641-(641-625)*(rand()%2)+(x%3)+(y%3)*20;
			break;

	}
	return f;
}

byte EditorMouseClick(void)
{
	int x1,x2,y1,y2,i,j;

	if(editMode==EDITMODE_PICKFLR || editMode==EDITMODE_PICKWALL || editMode==EDITMODE_PICKWALLFLR ||
		editMode==EDITMODE_PICKNEXT || editMode==EDITMODE_PICKRANDOM)
	{
		PickerMouseClick();
		plopCounter=100;
		return 0;
	}
	else if(editMode==EDITMODE_PICKITEM)
	{
		ItemPickerClick();
		plopCounter=100;
		return 0;
	}
	else if(editMode==EDITMODE_PICKENEMY)
	{
		EnemyPickerClick();
		plopCounter=100;
		return 0;
	}
	else if(editMode==EDITMODE_FILE)
	{
		if(!FileDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitFileDialog();
			plopCounter=100;
		}
		return 0;
	}
	else if(editMode==EDITMODE_TILELOAD)
	{
		if(!TileDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitTileDialog();
			plopCounter=100;
		}
		return 0;
	}
	else if(editMode==EDITMODE_MAPMENU)
	{
		if(!MapDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitMapDialog();
			plopCounter=100;
		}
		return 0;
	}
	else if(editMode==EDITMODE_SPECIAL)
	{
		if(!SpclDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitSpclDialog();
			plopCounter=100;
		}
		return 0;
	}
	else if(editMode==EDITMODE_TERRAIN)
	{
		TerrainMouseClick();
		return 0;
	}

	x1=tileX-(editopt.brushSize-1);
	x2=tileX+(editopt.brushSize-1);
	y1=tileY-(editopt.brushSize-1);
	y2=tileY+(editopt.brushSize-1);

	if(mouseY<420)
	{
		switch(editopt.plopMode)
		{
			case PLOP_TORCH:
				curMap->PermaTorch(tileX,tileY,editopt.brushSize*4);
				break;
			case PLOP_SMOOTH:
				for(i=x1;i<=x2;i++)
					for(j=y1;j<=y2;j++)
					{
						if(i>=0 && i<curMap->width && j>=0 && j<curMap->height)
						{
							curMap->SmoothLight(i,j);
						}
					}
				break;
			case PLOP_LIGHT:
				for(i=x1;i<=x2;i++)
					for(j=y1;j<=y2;j++)
					{
						if(i>=0 && i<curMap->width && j>=0 && j<curMap->height)
						{
							if(curMap->map[i+j*curMap->width].light<MAX_LIGHT)
								curMap->map[i+j*curMap->width].light++;
						}
					}
				break;
			case PLOP_DARK:
				for(i=x1;i<=x2;i++)
					for(j=y1;j<=y2;j++)
					{
						if(i>=0 && i<curMap->width && j>=0 && j<curMap->height)
						{
							if(curMap->map[i+j*curMap->width].light>MIN_LIGHT)
								curMap->map[i+j*curMap->width].light--;
						}
					}
				break;
			case PLOP_FLOOR:
				for(i=x1;i<=x2;i++)
					for(j=y1;j<=y2;j++)
					{
						if(i>=0 && i<curMap->width && j>=0 && j<curMap->height)
						{

							curMap->map[i+j*curMap->width].floor=ProcessFloorChoice(i,j,editopt.curFloor);
							curMap->map[i+j*curMap->width].wall=0;
						}
					}
				break;
			case PLOP_WALL:
				for(i=x1;i<=x2;i++)
					for(j=y1;j<=y2;j++)
					{
						if(i>=0 && i<curMap->width && j>=0 && j<curMap->height)
						{
							if(editopt.curWall==514)
							{
								curMap->map[i+j*curMap->width].wall=514+(rand()%5);
								curMap->map[i+j*curMap->width].floor=curMap->map[i+j*curMap->width].wall-20;
							}
							else
							{
								curMap->map[i+j*curMap->width].floor=editopt.curWallFloor;
								curMap->map[i+j*curMap->width].wall=editopt.curWall;
							}
						}
					}
				break;
			case PLOP_RANDOM:
				for(i=x1;i<=x2;i++)
					for(j=y1;j<=y2;j++)
					{
						if(i>=0 && i<curMap->width && j>=0 && j<curMap->height)
						{
							if(editopt.curRandom<editopt.curFloor)
								curMap->map[i+j*curMap->width].floor=(word)Random(editopt.curFloor-editopt.curRandom+1)+editopt.curRandom;
							else
								curMap->map[i+j*curMap->width].floor=(word)Random(editopt.curRandom-editopt.curFloor+1)+editopt.curFloor;
							curMap->map[i+j*curMap->width].wall=0;
						}
					}
				break;
			case PLOP_ITEM:
				switch(editopt.curItem)
				{
					case IT_TREE1:
					case IT_TREE2:
					case IT_TREE3:
						editopt.curItem=IT_TREE1+(rand()%3);
						break;
					case IT_POST1:
					case IT_POST2:
					case IT_POST3:
						editopt.curItem=IT_POST1+(rand()%3);
						break;
					case IT_ROCK:
					case IT_ROCK2:
						editopt.curItem=IT_ROCK+(rand()%2);
						break;
					case IT_ROCK3:
					case IT_ROCK4:
						editopt.curItem=IT_ROCK3+(rand()%2);
						break;
					case IT_DEADPLANT:
					case IT_DEADPLANT2:
					case IT_DEADPLANT3:
						editopt.curItem=IT_DEADPLANT+(rand()%3);
						break;
					case IT_CAVEROCK:
					case IT_CAVEROCK2:
					case IT_CAVEROCK3:
						editopt.curItem=IT_CAVEROCK+(rand()%3);
						break;
					case IT_SMLCAVEROCK:
					case IT_SMLCAVEROCK2:
						editopt.curItem=IT_SMLCAVEROCK+(rand()%2);
						break;
					case IT_MTNROCK:
					case IT_MTNROCK2:
					case IT_MTNROCK3:
						editopt.curItem=IT_MTNROCK+(rand()%3);
						break;
					case IT_PURPLEROCK:
					case IT_PURPLEROCK2:
						editopt.curItem=IT_PURPLEROCK+(rand()%2);
						break;
					case IT_PURPLEROCKSML:
					case IT_PURPLEROCKSML2:
						editopt.curItem=IT_PURPLEROCKSML+(rand()%2);
						break;
					case IT_SHROOM:
					case IT_SHROOM2:
						editopt.curItem=IT_SHROOM+(rand()%2);
						break;
					case IT_SHROOMBIG:
					case IT_SHROOMBIG2:
						editopt.curItem=IT_SHROOMBIG+(rand()%2);
						break;
					case IT_SHROOMPATCH:
					case IT_SHROOMPATCH2:
						editopt.curItem=IT_SHROOMPATCH+(rand()%2);
						break;
					case IT_CRYSTAL:
					case IT_CRYSTAL2:
					case IT_CRYSTAL3:
					case IT_CRYSTAL4:
						editopt.curItem=IT_CRYSTAL+(rand()%4);
						break;
					case IT_VINEWALL:
					case IT_VINEWALL2:
					case IT_VINEWALL3:
						editopt.curItem=IT_VINEWALL+(rand()%3);
						break;
				}
				AddItem(tileX,tileY,editopt.curItem);
				break;
			case PLOP_BADGUY:
				AddMapBadguy(tileX,tileY,editopt.curBadguy);
				break;
			case PLOP_SPECIAL:
				PlopSpecial(tileX,tileY);
				break;
		}
	}
	else
		EditorMenuClick();
	return 1;
}

void UpdateMouse(void)
{
	int cx,cy;

	editmgl->GetMouse(&mouseX,&mouseY);

	if(mouseX<0)
		mouseX=0;
	if(mouseY<0)
		mouseY=0;
	if(mouseX>639)
		mouseX=639;
	if(mouseY>479)
		mouseY=479;

	GetCamera(&cx,&cy);

	tileX=(mouseX+cx-320);
	tileY=(mouseY+cy-240);

	if(tileX<0)
		tileX=tileX/TILE_WIDTH-1;
	else
		tileX=tileX/TILE_WIDTH;
	if(tileY<0)
		tileY=tileY/TILE_HEIGHT-1;
	else
		tileY=tileY/TILE_HEIGHT;

	if(editmgl->MouseDown())
	{
		if(!msButton)
		{
			msButton=1;
			EditorMouseClick();
			return;
		}

		// these are the draggable tools
		if((editMode==EDITMODE_EDIT) &&
			(editopt.plopMode==PLOP_FLOOR || editopt.plopMode==PLOP_WALL ||
			editopt.plopMode==PLOP_LIGHT || editopt.plopMode==PLOP_DARK ||
			editopt.plopMode==PLOP_SMOOTH || editopt.plopMode==PLOP_ITEM || editopt.plopMode==PLOP_RANDOM))
		{
			if(plopCounter>0)
				plopCounter--;
			if(!plopCounter)
			{
				EditorMouseClick();

				// some tools have a limited rate of plopping to avoid making a mess since
				// they can add up
				if(editopt.plopMode==PLOP_LIGHT || editopt.plopMode==PLOP_DARK ||
					editopt.plopMode==PLOP_SMOOTH)
					plopCounter=PLOPRATE;
			}
		}
	}
	else
	{
		// mouse released
		msButton=0;
		plopCounter=0;
	}
}

byte EditorRun(int *lastTime)
{
	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(!editmgl->Process())
			return QUITGAME;

		// update everything here
		EditorUpdateGuys(curMap);
		curMap->Update(UPDATE_EDIT,&world);
		UpdateMouse();
		if(warning)
			warning--;

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}

	JamulSoundUpdate();

	return CONTINUE;
}

int NumMapMons(Map *map)
{
	int i,n;

	n=0;
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
			n++;
	}
	return n;
}

void RenderEditMenu(void)
{
	char s[32];

	editmgl->FillBox(0,420,639,420,16);
	editmgl->FillBox(0,421,639,479,7);

	// the plopMode buttons
	editmgl->Box(1,422,40,422+14,16+15*(editopt.plopMode==PLOP_FLOOR));
	Print(3,424,"Floor",0,1);
	editmgl->Box(1,438,40,438+14,16+15*(editopt.plopMode==PLOP_WALL));
	Print(3,440,"Wall",0,1);
	editmgl->Box(1,454,40,454+14,16+15*(editopt.plopMode==PLOP_ITEM));
	Print(3,456,"Item",0,1);
	editmgl->Box(42,422,82,422+14,16+15*(editopt.plopMode==PLOP_BADGUY));
	Print(44,424,"Mnstr",0,1);
	editmgl->Box(42,438,82,438+14,16+15*(editopt.plopMode==PLOP_LIGHT));
	Print(44,440,"Light",0,1);
	editmgl->Box(42,454,82,454+14,16+15*(editopt.plopMode==PLOP_DARK));
	Print(44,456,"Dark",0,1);
	editmgl->Box(84,422,124,422+14,16+15*(editopt.plopMode==PLOP_SMOOTH));
	Print(86,424,"Smth",0,1);
	editmgl->Box(84,438,124,438+14,16+15*(editopt.plopMode==PLOP_TORCH));
	Print(86,440,"Torch",0,1);
	editmgl->Box(84,454,124,454+14,16+15*(editopt.plopMode==PLOP_SPECIAL));
	Print(86,456,"Spcl",0,1);

	// the display flags
	editmgl->Box(130,422,170,422+14,31*((editopt.displayFlags&MAP_SHOWLIGHTS)>0));
	Print(132,424,"Lghts",0,1);
	editmgl->Box(130,438,170,438+14,31*((editopt.displayFlags&MAP_SHOWWALLS)>0));
	Print(132,440,"Walls",0,1);
	editmgl->Box(130,454,170,454+14,31*((editopt.displayFlags&MAP_SHOWITEMS)>0));
	Print(132,456,"Items",0,1);
	editmgl->Box(172,422,212,422+14,31*((editopt.displayFlags&MAP_SHOWBADGUYS)>0));
	Print(174,424,"Mstrs",0,1);
	editmgl->Box(172,438,212,438+14,31*((editopt.displayFlags&MAP_SHOWSPECIALS)>0));
	Print(174,440,"Spcls",0,1);
	editmgl->Box(172,454,212,454+14,31*(showTags>0));
	Print(174,456,"Tags",0,1);

	// the CD stuff
	editmgl->Box(216,422,226,422+14,16);
	Print(219,424,"-",0,1);
	editmgl->Box(228,422,288,422+14,16);
	sprintf(s,"Song %02d",curMap->song);
	Print(230,424,s,0,1);
	editmgl->Box(290,422,300,422+14,16);
	Print(292,424,"+",0,1);

	// File Menu and World Menu buttons
	editmgl->Box(216,438,300,438+14,16);
	Print(218,440,"File Menu",0,1);
	editmgl->Box(216,454,300,454+14,16);
	Print(218,456,"World Menu",0,1);

	// more buttons
	editmgl->Box(304,422,388,422+14,16);
	Print(306,424,"Edit Terrain",0,1);
	editmgl->Box(304,438,388,438+14,16);
	Print(306,440,"Load Tileset",0,1);
	sprintf(s,"Tag: %d",editopt.curTag);
	Print(306,460,s,0,1);

	// brush size
	editmgl->Box(392,422,442,422+14,16);
	Print(394,424,"Brush+",0,1);
	editmgl->FillBox(392+25-(editopt.brushSize-1),
					 422+14+13-(editopt.brushSize-1),
					 392+25+(editopt.brushSize-1),
					 422+14+13+(editopt.brushSize-1),16);
	editmgl->Box(392,463,442,463+14,16);
	Print(394,465,"Brush-",0,1);

	// map flags
	editmgl->Box(446,422,486,422+14,16+15*((curMap->flags&MAP_SNOWING)!=0));
	Print(448,424,"Snow",0,1);
	editmgl->Box(446,438,486,438+14,16+15*((curMap->flags&MAP_LITDUNGEON)!=0));
	Print(448,440,"Lit1",0,1);
	editmgl->Box(446,454,486,454+14,16+15*((curMap->flags&MAP_SECRET)!=0));
	Print(448,456,"Secrt",0,1);
	editmgl->Box(488,422,528,422+14,16+15*((curMap->flags&MAP_TORCHLIT)!=0));
	Print(490,424,"Trch",0,1);
	editmgl->Box(488,438,528,438+14,16+15*((curMap->flags&MAP_STARS)!=0));
	Print(490,440,"Stars",0,1);

	// now show the current stuff for the plop mode
	switch(editopt.plopMode)
	{
		case PLOP_FLOOR:
			RenderFloorTileUnlit(602,450,editopt.curFloor);
			sprintf(s,"#%03d",editopt.curFloor);
			Print(550,460,s,0,1);
			break;
		case PLOP_WALL:
			RenderFloorTileUnlit(602,450-24,editopt.curWallFloor);
			RenderFloorTileUnlit(602,450,editopt.curWall);
			break;
		case PLOP_ITEM:
			InstaRenderItem(626,464,editopt.curItem,0,editmgl);
			sprintf(s,"#%03d",editopt.curItem);
			Print(550,460,s,0,1);
			break;
		case PLOP_BADGUY:
			InstaRenderMonster(626,464,editopt.curBadguy,0,editmgl);
			sprintf(s,"#%03d",editopt.curBadguy);
			Print(550,460,s,0,1);
			break;
		case PLOP_RANDOM:
			RenderFloorTileUnlit(602,450-26,editopt.curRandom);
			RenderFloorTileUnlit(602,452,editopt.curFloor);
			break;
	}

	if(tileX>=0 && tileY>=0 && tileX<curMap->width && tileY<curMap->height)
	{
		sprintf(s,"(%03d,%03d)",tileX,tileY);
		Print(580,408,s,0,1);
	}
	sprintf(s,"Mons: %d",NumMapMons(curMap));
	Print(2,408,s,0,1);

	if(warning)
	{
		CenterPrint(320,20,"WARNING!  You can totally destroy this game",0,0);
		CenterPrint(320,40,"by using this editor.  If you don't thoroughly",0,0);
		CenterPrint(320,60,"know what you are doing, press ESC now!",0,0);
		CenterPrint(320,100,"If you want to learn, visit the Hamumu Forum.",0,0);
	}
}

void ShowTarget(void)
{
	int x1,x2,y1,y2,cx,cy;
	static byte col=0;

	col=255-col;
	GetCamera(&cx,&cy);

	x1=tileX*TILE_WIDTH-(cx-320);
	y1=tileY*TILE_HEIGHT-(cy-240);

	x2=x1+TILE_WIDTH-1;
	y2=y1+TILE_HEIGHT-1;

	// these are the only tools which allow sizable plops
	if(editopt.plopMode==PLOP_FLOOR || editopt.plopMode==PLOP_WALL ||
		editopt.plopMode==PLOP_LIGHT || editopt.plopMode==PLOP_DARK || editopt.plopMode==PLOP_SMOOTH
		|| editopt.plopMode==PLOP_TORCH || editopt.plopMode==PLOP_RANDOM)
	{
		x1-=TILE_WIDTH*(editopt.brushSize-1);
		y1-=TILE_HEIGHT*(editopt.brushSize-1);
		x2+=TILE_WIDTH*(editopt.brushSize-1);
		y2+=TILE_HEIGHT*(editopt.brushSize-1);
	}

	if(x1<0)
		x1=0;
	if(x1>639)
		x1=639;
	if(x2<0)
		x2=0;
	if(x2>639)
		x2=639;
	if(y1<0)
		y1=0;
	if(y1>479)
		y1=479;
	if(y2<0)
		y2=0;
	if(y2>479)
		y2=479;

	editmgl->Box(x1,y1,x2,y2,col);
}

void RenderCheckbox(int x,int y,int v,const char *txt)
{
	if(v)
		editmgl->FillBox(x,y,x+11,y+11,16);
	else
		editmgl->Box(x,y,x+11,y+11,16);
	Print(x+13,y,txt,0,1);
}

void RenderTerrainInfo(int which)
{
	char s[32];

	RenderCheckbox(5,250,world.terrain[which].flags&TF_SOLID,"Impassable");
	RenderCheckbox(5,262,world.terrain[which].flags&TF_ICE,"Icy");
	RenderCheckbox(5,274,world.terrain[which].flags&TF_SKYFX,"Sky FX");
	RenderCheckbox(5,286,world.terrain[which].flags&TF_WATER,"Water");
	RenderCheckbox(5,298,world.terrain[which].flags&TF_LAVA,"Lava");
	RenderCheckbox(5,310,world.terrain[which].flags&TF_PUSHY,"Pushable");
	RenderCheckbox(5,322,world.terrain[which].flags&TF_PUSHON,"Can be pushed on");
	RenderCheckbox(5,334,world.terrain[which].flags&TF_ANIM,"Animates to next");
	RenderCheckbox(5,346,world.terrain[which].flags&TF_STEP,"Changes when stepped on");
	RenderCheckbox(5,358,world.terrain[which].flags&TF_SKY,"Sky slide");
	RenderCheckbox(5,370,world.terrain[which].flags&TF_TRANS,"Transparent Roof");
	RenderCheckbox(250,250,world.terrain[which].flags&TF_NOSHADOW,"Shadowless Wall");
	RenderCheckbox(250,262,world.terrain[which].flags&TF_CLIFF,"Cliff slide");
	RenderCheckbox(250,274,world.terrain[which].flags&TF_FIRE,"Flaming Terrain");
	RenderCheckbox(250,286,world.terrain[which].flags&TF_NOBULLET,"Bulletproof");
	RenderCheckbox(250,298,world.terrain[which].flags&TF_WATERFX,"Water FX");

	Print(5,386,"Next Terrain:",0,1);
	RenderFloorTileUnlit(5,398,world.terrain[which].next);

	sprintf(s,"Terrain #%03d",which);
	Print(5,468,s,0,1);

	Print(560,468,"Next Page",0,1);
}

void RenderItemPickDisplay(void)
{
	int i,x,y;
	char s[32];

	editmgl->ClearScreen();

	x=2;
	y=2;

	for(i=1;i<MAX_ITMS;i++)
	{
		snprintf(s,20,"%s",ItemName(i));
		if(mouseX>x-1 && mouseX<x+101 && mouseY>y-1 && mouseY<y+14)
			editmgl->Box(x,y,x+100,y+13,31);
		else
			editmgl->Box(x,y,x+100,y+13,16);
		Print(x+2,y+2,s,0,1);
		x+=102;
		if(x>640-100)
		{
			x=2;
			y+=14;
		}
	}
}

void RenderEnemyPickDisplay(void)
{
	int i,x,y;
	char s[32];

	editmgl->ClearScreen();

	x=2;
	y=2;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		snprintf(s,32,"%02d:%s",i,MonsterName(i));
		if(mouseX>x-1 && mouseX<x+201 && mouseY>y-1 && mouseY<y+15)
			editmgl->Box(x,y,x+200,y+14,31);
		else
			editmgl->Box(x,y,x+200,y+14,16);
		Print(x+2,y+2,s,0,1);
		x+=202;
		if(x>640-200)
		{
			x=2;
			y+=15;
		}
	}
}

void RenderPickDisplay(int curPick)
{
	int i,j;
	int selected;

	editmgl->FillBox(0,240,639,479,0);

	selected=-1;
	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
		{
			if(mouseX>i*TILE_WIDTH-1 && mouseX<i*TILE_WIDTH+TILE_WIDTH &&
				mouseY>j*TILE_HEIGHT-1 && mouseY<j*TILE_HEIGHT+TILE_HEIGHT)
			{
				RenderFloorTile(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+tileOffset,16);
				selected=i+j*20+tileOffset;
			}
			else
				RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+tileOffset);

			if(i+j*20+tileOffset==curPick)
				editmgl->Box(i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,
						j*TILE_HEIGHT+TILE_HEIGHT-1,31);
		}

	Print(400,242,"Pick a tile please!!",0,1);

	// describe the highlighted terrain
	if(selected!=-1)
		RenderTerrainInfo(selected);
	else
		Print(560,468,"Next Page",0,1);
}

void RenderTerrainEditor(int curPick)
{
	int i,j;
	int selected;

	editmgl->FillBox(0,240,639,479,0);

	selected=-1;
	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
		{
			if(mouseX>i*TILE_WIDTH-1 && mouseX<i*TILE_WIDTH+TILE_WIDTH &&
				mouseY>j*TILE_HEIGHT-1 && mouseY<j*TILE_HEIGHT+TILE_HEIGHT)
			{
				RenderFloorTile(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+tileOffset,16);
				selected=i+j*20+tileOffset;
			}
			else
				RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+tileOffset);
			if(i+j*20+tileOffset==curPick)
				editmgl->Box(i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,
							j*TILE_HEIGHT+TILE_HEIGHT-1,31);
		}
	RenderTerrainInfo(curPick);
}

void ShowSpecials(void)
{
	int i;
	int sx,sy;

	if(!(editopt.displayFlags&MAP_SHOWSPECIALS))
		return;

	GetCamera(&sx,&sy);
	for(i=0;i<MAX_SPECIAL;i++)
		if(curMap->special[i].trigger)
		{
			Print(curMap->special[i].x*TILE_WIDTH+2-sx+320,
				  curMap->special[i].y*TILE_HEIGHT+6-sy+240,
				  "Spcl",0,1);
		}
}

void EditorDraw(void)
{
	int x,y;

	switch(editMode)
	{
		case EDITMODE_EDIT:
			if(editopt.displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(editopt.displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,curMap,editopt.displayFlags);
			ShowSpecials();
			ShowTarget();
			if(showTags)
			{
				GetCamera(&x,&y);
				curMap->ShowTags(x,y,editopt.copyX,editopt.copyY,editopt.copyWidth,editopt.copyHeight);
			}
			RenderEditMenu();
			break;
		case EDITMODE_TERRAIN:
			RenderTerrainEditor(editopt.curFloor);
			break;
		case EDITMODE_PICKFLR:
			RenderPickDisplay(editopt.curFloor);
			break;
		case EDITMODE_PICKRANDOM:
			RenderPickDisplay(editopt.curRandom);
			break;
		case EDITMODE_PICKWALL:
			RenderPickDisplay(editopt.curWall);
			break;
		case EDITMODE_PICKWALLFLR:
			RenderPickDisplay(editopt.curWallFloor);
			break;
		case EDITMODE_PICKNEXT:
			RenderPickDisplay(world.terrain[editopt.curFloor].next);
			break;
		case EDITMODE_PICKITEM:
			RenderItemPickDisplay();
			break;
		case EDITMODE_PICKENEMY:
			RenderEnemyPickDisplay();
			break;
		case EDITMODE_FILE:
			if(editopt.displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(editopt.displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,curMap,editopt.displayFlags);
			ShowSpecials();
			RenderEditMenu();
			RenderFileDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_TILELOAD:
			if(editopt.displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(editopt.displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,curMap,editopt.displayFlags);
			ShowSpecials();
			RenderEditMenu();
			RenderTileDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_MAPMENU:
			if(editopt.displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(editopt.displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,curMap,editopt.displayFlags);
			ShowSpecials();
			RenderEditMenu();
			RenderMapDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SPECIAL:
			if(editopt.displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(editopt.displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,curMap,editopt.displayFlags);
			ShowSpecials();
			RenderEditMenu();
			RenderSpclDialog(mouseX,mouseY,editmgl);
			break;
	}

	if(showPwrUps)
	{
		Print(2,2,pwrUpTxt,0,1);
		Print(2,15,pwrUpTxt2,0,1);
		Print(2,30,pwrUpTxt3,0,1);
	}

	// draw the mouse cursor
	DrawMouseCursor(mouseX,mouseY);

	editmgl->Flip();
}

void Cavernize(void)
{
	int i,x,y;
	int flr,wal;
	byte walls[8];

	for(x=0;x<curMap->width;x++)
		for(y=0;y<curMap->height;y++)
		{
			flr=curMap->map[x+y*curMap->width].floor;
			wal=curMap->map[x+y*curMap->width].wall;
			if(((flr>=209 && flr<=213) || (flr>=229 && flr<=233) || (flr>=249 && flr<=253)) && wal)
			{
				// wants to be cavernized
				for(i=0;i<8;i++)
					walls[i]=0;
				if(x<curMap->width-1)
				{
					if(curMap->map[x+1+y*curMap->width].wall)
						walls[0]=1;
					if(y<curMap->height-1 && curMap->map[x+1+(y+1)*curMap->width].wall)
						walls[1]=1;
					if(y>0 && curMap->map[x+1+(y-1)*curMap->width].wall)
						walls[7]=1;
				}
				if(x>0)
				{
					if(curMap->map[x-1+y*curMap->width].wall)
						walls[4]=1;
					if(y<curMap->height-1 && curMap->map[x-1+(y+1)*curMap->width].wall)
						walls[3]=1;
					if(y>0 && curMap->map[x-1+(y-1)*curMap->width].wall)
						walls[5]=1;
				}
				if(y<curMap->height-1 && curMap->map[x+(y+1)*curMap->width].wall)
					walls[2]=1;
				if(y>0 && curMap->map[x+(y-1)*curMap->width].wall)
					walls[6]=1;

				flr=230;
				if(walls[6]==0)	// nothing on top, need a rough edge
				{
					if(walls[0] && walls[4])
						flr=210;
					else if(walls[0])
						flr=209;
					else if(walls[4])
						flr=211;
					else	// no neighbors at all
						flr=212;
				}
				else	// something on top
				{
					if(walls[0] && walls[4])
						flr=230;
					else if(walls[0])
						flr=229;
					else if(walls[4])
						flr=231;
					else
						flr=232;
				}
				if(flr==230 || flr==210)
				{
					wal=250;
				}
				else if(flr==229 || flr==209)
					wal=249;
				else if(flr==231 || flr==211)
					wal=251;
				else
					wal=252;

				// random alternates for the normal part
				if(wal==250 && (rand()%4)==0)
					wal=253;
				if(flr==230 && (rand()%4)==0)
					flr=233;
				if(flr==210 && (rand()%4)==0)
					flr=213;

				curMap->map[x+y*curMap->width].floor=flr;
				curMap->map[x+y*curMap->width].wall=wal;
			}
		}
}

byte IsPurpleFloor(word f)
{
	return ((f>=620 && f<=627) || (f>=640 && f<=647) || (f>=660 && f<=667) || (f>=680 && f<=684) || (f>=700 && f<=704));
}

byte IsPurpleHole(word f)
{
	return (f==603 || (f>=720 && f<=724) || (f>=740 && f<=744));
}

byte IsGorgeFloor(word f)
{
	return ((f>=620+48 && f<=627+48) || (f>=640+48 && f<=647+48) || (f>=660+48 && f<=667+48) || (f>=680+48 && f<=684+48) || (f>=700+48 && f<=704+48));
}

byte IsGorgeHole(word f)
{
	return (f==603 || (f>=720+48 && f<=724+48) || (f>=740+48 && f<=744+48));
}

void Gorgify(void)
{
	int i,x,y;
	int flr,wal;
	byte walls[8];

	for(x=0;x<curMap->width;x++)
		for(y=0;y<curMap->height;y++)
		{
			flr=curMap->map[x+y*curMap->width].floor;
			wal=curMap->map[x+y*curMap->width].wall;

			if(IsGorgeHole(flr))
			{
				if(y>0 && IsGorgeHole(curMap->map[x+(y-1)*curMap->width].floor))
				{
					// hole is above this
					if(y>1 && IsGorgeHole(curMap->map[x+(y-2)*curMap->width].floor))
					{
						flr=603;
					}
					else
					{
						if(x>0 && y>1 && IsGorgeHole(curMap->map[x-1+(y-2)*curMap->width].floor))
							flr=740+48;
						else if(x<curMap->width-1 && y>1 && IsGorgeHole(curMap->map[x+1+(y-2)*curMap->width].floor))
							flr=744+48;
						else
							flr=741+48+(x%3);
					}
				}
				else
				{
					if(x>0 && y>0 && IsGorgeHole(curMap->map[x-1+(y-1)*curMap->width].floor))
						flr=720+48;
					else if(x<curMap->width-1 && y>0 && IsGorgeHole(curMap->map[x+1+(y-1)*curMap->width].floor))
						flr=724+48;
					else
						flr=721+48+(x%3);
				}
				curMap->map[x+y*curMap->width].floor=flr;
			}
			else if(IsGorgeFloor(flr))	// is floor
			{
				// walls[]=1 when there is a hole there
				char xtab[]={1,1,0,-1,-1,-1,0,1};
				char ytab[]={0,1,1,1,0,-1,-1,-1};

				for(i=0;i<8;i++)
				{
					if(x+xtab[i]>=0 && x+xtab[i]<curMap->width && y+ytab[i]>=0 && y+ytab[i]<curMap->height)
						walls[i]=IsGorgeHole(curMap->map[x+xtab[i]+(y+ytab[i])*curMap->width].floor);
					else
						walls[i]=0;
				}

				if(walls[6]==1 && walls[0]==1)
					flr=624+48;	// upper right corner
				else if(walls[2]==1 && walls[0]==1)
					flr=704+48;	// lower right corner
				else if(walls[2]==1 && walls[4]==1)
					flr=700+48;	// lower left corner
				else if(walls[4]==1 && walls[6]==1)
					flr=620+48;	// upper left corner
				else if(walls[0]==1)
					flr=644+48+(y%3)*20;	// just the right side
				else if(walls[2]==1)
					flr=701+48+(x%3);		// just the bottom side
				else if(walls[4]==1)
					flr=640+48+(y%3)*20;	// just the left side
				else if(walls[6]==1)
					flr=621+48+(x%3);		// just the top side
				else if(walls[7]==1)
					flr=705+48;	// hole to upper right
				else if(walls[1]==1)
					flr=685+48;	// hole to lower right
				else if(walls[3]==1)
					flr=686+48;	// hole to lower left
				else if(walls[5]==1)
					flr=706+48;	// hole to upper left
				else
					flr=641+48-(641-625)*(rand()%2)+(x%3)+(y%3)*20;	// default is plain old ground

				curMap->map[x+y*curMap->width].floor=flr;
			}
		}
}

void CavernizeBrown(byte adj)
{
	int i,x,y;
	int flr,wal;
	byte walls[8];

	for(x=0;x<curMap->width;x++)
		for(y=0;y<curMap->height;y++)
		{
			flr=curMap->map[x+y*curMap->width].floor;
			wal=curMap->map[x+y*curMap->width].wall;
			if(((flr>=550+adj && flr<=554+adj) || (flr>=570+adj && flr<=574+adj) || (flr>=590+adj && flr<=594+adj)) && wal)
			{
				// wants to be cavernized
				for(i=0;i<8;i++)
					walls[i]=0;
				if(x<curMap->width-1)
				{
					if(curMap->map[x+1+y*curMap->width].wall)
						walls[0]=1;
					if(y<curMap->height-1 && curMap->map[x+1+(y+1)*curMap->width].wall)
						walls[1]=1;
					if(y>0 && curMap->map[x+1+(y-1)*curMap->width].wall)
						walls[7]=1;
				}
				if(x>0)
				{
					if(curMap->map[x-1+y*curMap->width].wall)
						walls[4]=1;
					if(y<curMap->height-1 && curMap->map[x-1+(y+1)*curMap->width].wall)
						walls[3]=1;
					if(y>0 && curMap->map[x-1+(y-1)*curMap->width].wall)
						walls[5]=1;
				}
				if(y<curMap->height-1 && curMap->map[x+(y+1)*curMap->width].wall)
					walls[2]=1;
				if(y>0 && curMap->map[x+(y-1)*curMap->width].wall)
					walls[6]=1;

				flr=571+adj;
				if(walls[6]==0)	// nothing on top, need a rough edge
				{
					if(walls[0] && walls[4])
						flr=551+adj;
					else if(walls[0])
						flr=550+adj;
					else if(walls[4])
						flr=552+adj;
					else	// no neighbors at all
						flr=553+adj;
				}
				else	// something on top
				{
					if(walls[0] && walls[4])
						flr=571+adj;
					else if(walls[0])
						flr=570+adj;
					else if(walls[4])
						flr=572+adj;
					else
						flr=573+adj;
				}
				if(flr==571+adj || flr==551+adj)
				{
					wal=591+adj;
				}
				else if(flr==570+adj || flr==550+adj)
					wal=590+adj;
				else if(flr==572+adj || flr==552+adj)
					wal=592+adj;
				else
					wal=593+adj;

				// random alternates for the normal part
				if(wal==591+adj && (rand()%4)==0)
					wal=594+adj;
				if(flr==571+adj && (rand()%4)==0)
					flr=574+adj;
				if(flr==551+adj && (rand()%4)==0)
					flr=554+adj;

				curMap->map[x+y*curMap->width].floor=flr;
				curMap->map[x+y*curMap->width].wall=wal;
			}
			// and now do holes in the ground, but only for purple floor
			if(adj==60 && IsPurpleHole(flr))
			{
				if(y>0 && IsPurpleHole(curMap->map[x+(y-1)*curMap->width].floor))
				{
					// hole is above this
					if(y>1 && IsPurpleHole(curMap->map[x+(y-2)*curMap->width].floor))
					{
						flr=603;
					}
					else
					{
						if(x>0 && y>1 && IsPurpleHole(curMap->map[x-1+(y-2)*curMap->width].floor))
							flr=740;
						else if(x<curMap->width-1 && y>1 && IsPurpleHole(curMap->map[x+1+(y-2)*curMap->width].floor))
							flr=744;
						else
							flr=741+(x%3);
					}
				}
				else
				{
					if(x>0 && y>0 && IsPurpleHole(curMap->map[x-1+(y-1)*curMap->width].floor))
						flr=720;
					else if(x<curMap->width-1 && y>0 && IsPurpleHole(curMap->map[x+1+(y-1)*curMap->width].floor))
						flr=724;
					else
						flr=721+(x%3);
				}
				curMap->map[x+y*curMap->width].floor=flr;
			}
			else if(adj==60 && IsPurpleFloor(flr))	// is floor
			{
				// walls[]=1 when there is a hole there
				char xtab[]={1,1,0,-1,-1,-1,0,1};
				char ytab[]={0,1,1,1,0,-1,-1,-1};

				for(i=0;i<8;i++)
				{
					if(x+xtab[i]>=0 && x+xtab[i]<curMap->width && y+ytab[i]>=0 && y+ytab[i]<curMap->height)
						walls[i]=IsPurpleHole(curMap->map[x+xtab[i]+(y+ytab[i])*curMap->width].floor);
					else
						walls[i]=0;
				}

				if(walls[6]==1 && walls[0]==1)
					flr=624;	// upper right corner
				else if(walls[2]==1 && walls[0]==1)
					flr=704;	// lower right corner
				else if(walls[2]==1 && walls[4]==1)
					flr=700;	// lower left corner
				else if(walls[4]==1 && walls[6]==1)
					flr=620;	// upper left corner
				else if(walls[0]==1)
					flr=644+(y%3)*20;	// just the right side
				else if(walls[2]==1)
					flr=701+(x%3);		// just the bottom side
				else if(walls[4]==1)
					flr=640+(y%3)*20;	// just the left side
				else if(walls[6]==1)
					flr=621+(x%3);		// just the top side
				else if(walls[7]==1)
					flr=705;	// hole to upper right
				else if(walls[1]==1)
					flr=685;	// hole to lower right
				else if(walls[3]==1)
					flr=686;	// hole to lower left
				else if(walls[5]==1)
					flr=706;	// hole to upper left
				else
					flr=641-(641-625)*(rand()%2)+(x%3)+(y%3)*20;	// default is plain old ground

				curMap->map[x+y*curMap->width].floor=flr;
			}
		}
}

void ScanChests(void)
{
	byte pres[99],chest[50];
	int i,j;
	FILE *f;

	f=fopen("blah.txt","wt");

	for(i=0;i<99;i++)
		pres[i]=0;
	for(i=0;i<50;i++)
		chest[i]=0;

	for(i=0;i<world.numMaps;i++)
	{
		for(j=0;j<world.map[i]->width*world.map[i]->height;j++)
		{
			if(world.map[i]->map[j].item==IT_PRESENT)
			{
				if(pres[world.map[i]->map[j].tag]!=0)
					fprintf(f,"Present #%d in %d and %d!\n",world.map[i]->map[j].tag,pres[world.map[i]->map[j].tag]-1,i);
				pres[world.map[i]->map[j].tag]=i+1;
			}
			if(world.map[i]->map[j].item>=IT_CHEST1 && world.map[i]->map[j].item<=IT_CHEST5)
			{
				if(chest[world.map[i]->map[j].tag]!=0)
					fprintf(f,"Chest #%d in %d and %d!\n",world.map[i]->map[j].tag,chest[world.map[i]->map[j].tag]-1,i);
				chest[world.map[i]->map[j].tag]=i+1;
			}
		}
	}
	for(i=0;i<99;i++)
	{
		if(pres[i]==0)
			fprintf(f,"Present #%d missing!\n",i);
		if(i<50 && chest[i]==0)
			fprintf(f,"Chest #%d missing!\n",i);
	}
	fclose(f);
}

static void HandleKeyPresses(void)
{
	char k;
	int x,y;
	mapTile_t m;

	byte s = LastScanCode();
	k=editmgl->LastKeyPressed();
	if(k)
		lastKey=k;

	switch(s)
	{
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_KP_4:
			GetCamera(&x,&y);
			x-=TILE_WIDTH;
			if(x<0)
				x=0;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_KP_8:
			GetCamera(&x,&y);
			y-=TILE_HEIGHT;
			if(y<0)
				y=0;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_KP_6:
			GetCamera(&x,&y);
			x+=TILE_WIDTH;
			if(x>curMap->width*TILE_WIDTH)
				x=curMap->width*TILE_WIDTH;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_KP_2:
			GetCamera(&x,&y);
			y+=TILE_HEIGHT;
			if(y>curMap->height*TILE_HEIGHT)
				y=curMap->height*TILE_HEIGHT;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
	}

	if(editMode==EDITMODE_EDIT)
	{
		switch(k)
		{
			case 'f':
				editMode=EDITMODE_FILE;
				InitFileDialog();
				break;
			case 'm':
				editMode=EDITMODE_MAPMENU;
				InitMapDialog(&world,curMapNum);
				break;
			case ' ':
				editopt.plopMode++;
				if(editopt.plopMode>=MAXPLOP)
					editopt.plopMode=0;
				break;
			case 'b':
				editopt.brushSize++;
				if(editopt.brushSize>10)
					editopt.brushSize=1;
				break;
			case '-':
				editopt.curFloor--;
				if(editopt.curFloor>NUMTILES-1)
					editopt.curFloor=NUMTILES-1;
				break;
			case '=':
			case '+':
				editopt.curFloor++;
				if(editopt.curFloor>NUMTILES-1)
					editopt.curFloor=0;
				break;
			case 'o':
				editopt.curWallFloor--;
				if(editopt.curWallFloor>NUMTILES-1)
					editopt.curWallFloor=NUMTILES-1;
				break;
			case 'p':
				editopt.curWallFloor++;
				if(editopt.curWallFloor>NUMTILES-1)
					editopt.curWallFloor=0;
				break;
			case '[':
				editopt.curWall--;
				if(editopt.curWall>NUMTILES-1)
					editopt.curWall=NUMTILES-1;
				break;
			case ']':
				editopt.curWall++;
				if(editopt.curWall>NUMTILES-1)
					editopt.curWall=0;
				break;
			case ',':
				editopt.curItem--;
				if(editopt.curItem>=MAX_ITMS)
					editopt.curItem=MAX_ITMS-1;
				break;
			case '.':
				editopt.curItem++;
				if(editopt.curItem>=MAX_ITMS)
					editopt.curItem=1;
				break;
			case 8: // backspace
				Delete(tileX,tileY);
				break;
			case 'A':
				curMap->Resize(curMap->width-1,curMap->height);
				break;
			case 'D':
				curMap->Resize(curMap->width+1,curMap->height);
				break;
			case 'W':
				curMap->Resize(curMap->width,curMap->height-1);
				break;
			case 'X':
				curMap->Resize(curMap->width,curMap->height+1);
				break;
			case 'I':
				for(x=0;x<curMap->width;x++)
				{
					m=curMap->map[x];
					for(y=0;y<curMap->height-1;y++)
					{
						curMap->map[x+y*curMap->width]=curMap->map[x+(y+1)*curMap->width];
					}
					curMap->map[x+y*curMap->width]=m;
				}
				for(x=0;x<MAX_MAPMONS;x++)
				{
					curMap->badguy[x].y--;
					if(curMap->badguy[x].y>curMap->height-1)
						curMap->badguy[x].y=curMap->height-1;
				}
				for(x=0;x<MAX_SPECIAL;x++)
				{
					curMap->special[x].y--;
					if(curMap->special[x].y>curMap->height-1)
						curMap->special[x].y=curMap->height-1;
					curMap->special[x].effectY--;
					if(curMap->special[x].effectY>curMap->height-1)
						curMap->special[x].effectY=curMap->height-1;
				}
				ShiftGuys(0,-1,curMap);
				break;
			case 'J':
				for(y=0;y<curMap->height;y++)
				{
					m=curMap->map[y*curMap->width];
					for(x=0;x<curMap->width-1;x++)
					{
						curMap->map[x+y*curMap->width]=curMap->map[x+1+(y)*curMap->width];
					}
					curMap->map[x+y*curMap->width]=m;
				}
				for(x=0;x<MAX_MAPMONS;x++)
				{
					curMap->badguy[x].x--;
					if(curMap->badguy[x].x>curMap->width-1)
						curMap->badguy[x].x=curMap->width-1;
				}
				for(x=0;x<MAX_SPECIAL;x++)
				{
					curMap->special[x].x--;
					if(curMap->special[x].x>curMap->width-1)
						curMap->special[x].x=curMap->width-1;
					curMap->special[x].effectX--;
					if(curMap->special[x].effectX>curMap->width-1)
						curMap->special[x].effectX=curMap->width-1;
				}
				ShiftGuys(-1,0,curMap);
				break;
			case 'K':
				for(y=0;y<curMap->height;y++)
				{
					m=curMap->map[curMap->width-1+y*curMap->width];
					for(x=curMap->width-1;x>0;x--)
					{
						curMap->map[x+y*curMap->width]=curMap->map[x-1+(y)*curMap->width];
					}
					curMap->map[y*curMap->width]=m;
				}
				for(x=0;x<MAX_MAPMONS;x++)
				{
					curMap->badguy[x].x++;
					if(curMap->badguy[x].x>curMap->width-1)
						curMap->badguy[x].x=0;
				}
				for(x=0;x<MAX_SPECIAL;x++)
				{
					curMap->special[x].x++;
					if(curMap->special[x].x>curMap->width-1)
						curMap->special[x].x=0;
					curMap->special[x].effectX++;
					if(curMap->special[x].effectX>curMap->width-1)
						curMap->special[x].effectX=0;
				}
				ShiftGuys(1,0,curMap);
				break;
			case 'M':
				for(x=0;x<curMap->width;x++)
				{
					m=curMap->map[x+(curMap->height-1)*curMap->width];
					for(y=curMap->height-1;y>0;y--)
					{
						curMap->map[x+y*curMap->width]=curMap->map[x+(y-1)*curMap->width];
					}
					curMap->map[x]=m;
				}
				for(x=0;x<MAX_MAPMONS;x++)
				{
					curMap->badguy[x].y++;
					if(curMap->badguy[x].y>curMap->height-1)
						curMap->badguy[x].y=0;
				}
				for(x=0;x<MAX_SPECIAL;x++)
				{
					curMap->special[x].y++;
					if(curMap->special[x].y>curMap->height-1)
						curMap->special[x].y=0;
					curMap->special[x].effectY++;
					if(curMap->special[x].effectY>curMap->height-1)
						curMap->special[x].effectY=0;
				}
				ShiftGuys(0,1,curMap);
				break;
			case 'T':
				showTags=1-showTags;
				break;
			case 't':
				if(tileX>=0 && tileY>=0 && tileX<curMap->width && tileY<curMap->height)
					curMap->map[tileX+tileY*curMap->width].tag=editopt.curTag;
				break;
			case ';':
				editopt.curTag--;
				break;
			case '\'':
				editopt.curTag++;
				break;
			case 'c':
				// Copy
				editopt.copyX=tileX-(editopt.brushSize-1);
				editopt.copyY=tileY-(editopt.brushSize-1);
				editopt.copyWidth=(editopt.brushSize*2)-1;
				editopt.copyHeight=(editopt.brushSize*2)-1;
				break;
			case 'v':
				// paste
				curMap->CopyChunk(editopt.copyX,editopt.copyY,editopt.copyWidth,editopt.copyHeight,tileX,tileY);
				break;
			case 'r':
				// Random plop
				editopt.plopMode=PLOP_RANDOM;
				break;
			case 'g':
				// get tile
				if(tileX>=0 && tileY>=0 && tileX<curMap->width && tileY<curMap->height)
				{
					editopt.curFloor=curMap->map[tileX+tileY*curMap->width].floor;
					if(curMap->map[tileX+tileY*curMap->width].wall)
					{
						editopt.curWall=curMap->map[tileX+tileY*curMap->width].wall;
						editopt.curWallFloor=curMap->map[tileX+tileY*curMap->width].floor;
					}
					if(curMap->map[tileX+tileY*curMap->width].item)
						editopt.curItem=curMap->map[tileX+tileY*curMap->width].item;
				}
				break;
			case 'C':
				// set all map to the same brightness as this tile
				m=curMap->map[tileX+tileY*curMap->width];
				for(x=0;x<curMap->width*curMap->height;x++)
					curMap->map[x].light=m.light;
				break;
			case 'Q':
				Cavernize();
				CavernizeBrown(0);
				CavernizeBrown(60);
				//Gorgify();
				break;
			case '!':
				ScanChests();
				break;
		}
	}
	else if(editMode==EDITMODE_FILE)
	{
		if(k)
		{
			if(!FileDialogKey(lastKey))
			{
				editMode=EDITMODE_EDIT;
				ExitFileDialog();
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_TILELOAD)
	{
		if(k)
		{
			if(!TileDialogKey(lastKey))
			{
				editMode=EDITMODE_EDIT;
				ExitTileDialog();
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_SPECIAL)
	{
		if(k)
		{
			if(!SpclDialogKey(lastKey))
			{
				editMode=EDITMODE_EDIT;
				ExitSpclDialog();
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_MAPMENU)
	{
		if(k)
		{
			if(!MapDialogKey(lastKey))
			{
				editMode=EDITMODE_EDIT;
				ExitMapDialog();
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_TERRAIN)
	{
		switch(k)
		{
			case 'a':
				world.terrain[editopt.curFloor].flags^=TF_ANIM;
				break;
			case 'n':
				editMode=EDITMODE_PICKNEXT;
				break;
			case 'b':
				world.terrain[editopt.curFloor].flags^=TF_NOBULLET;
				break;
			case 'i':
				world.terrain[editopt.curFloor].flags^=TF_SOLID;
				break;
			case 'w':
				world.terrain[editopt.curFloor].flags^=TF_WATER;
				break;
			case 27:
				editMode=EDITMODE_EDIT;
				lastKey=0;
				break;
		}
	}
	else
	{
		switch(k)
		{
			case 27:
				editMode=EDITMODE_EDIT;
				lastKey=0;
				break;
		}
	}
}

byte LunaticEditor(MGLDraw *mgl)
{
	int lastTime=1;
	byte exitcode=0;

	editmgl=mgl;

	if(!InitEditor())
		return QUITGAME;

	exitcode=CONTINUE;
	while(exitcode==CONTINUE)
	{
		lastTime+=TimeLength();
		StartClock();
		HandleKeyPresses();
		exitcode=EditorRun(&lastTime);
		EditorDraw();

		if(lastKey==27)
			exitcode=QUITGAME;

		if(!editmgl->Process())
			exitcode=QUITGAME;
		EndClock();
	}

	ExitEditor();
	return exitcode;
}

void EditorNewWorld(void)
{
	FreeWorld(&world);
	NewWorld(&world,editmgl);
	curMap=world.map[0];
	ExitGuys();
	InitGuys(MAX_MAPMONS);
	AddMapGuys(curMap);
}

void EditorLoadWorld(const char *fname)
{
	char s[256];

	sprintf(s,"addons/%s",fname);
	FreeWorld(&world);
	if(LoadWorld(&world,s))
	{
		curMap=world.map[0];
		ExitGuys();
		InitGuys(MAX_MAPMONS);
		AddMapGuys(curMap);
	}
	else
	{
		NewWorld(&world,editmgl);
		curMap=world.map[0];
		ExitGuys();
		InitGuys(MAX_MAPMONS);
		AddMapGuys(curMap);
	}
}

void EditorLoadTiles(const char *fname,byte wall)
{
	editmgl->LoadBMP(fname);
	SetTiles(editmgl->GetScreen(),wall);
}

void EditorSaveWorld(const char *fname)
{
	char s[64];

	sprintf(s,"backup/%s_back%d%d%d%d.llw",fname,rand()%10,rand()%10,rand()%10,rand()%10);
	SaveWorld(&world,s);
	sprintf(s,"addons/%s",fname);
	SaveWorld(&world,s);
}

void EditorSelectMap(byte w)
{

	curMap=world.map[w];
	curMapNum=w;

	// repair code for when I add new items
	/*
	int i;
	for(i=0;i<curMap->width*curMap->height;i++)
	{
		if(curMap->map[i].item==IT_LENSK)
			curMap->map[i].item=IT_PRESENT;
		if(curMap->map[i].item==IT_LENSM)
			curMap->map[i].item=IT_TOOLBOX;
	}
	*/

	PutCamera(320<<FIXSHIFT,240<<FIXSHIFT);
	ExitGuys();
	InitGuys(MAX_MAPMONS);
	AddMapGuys(curMap);
}
