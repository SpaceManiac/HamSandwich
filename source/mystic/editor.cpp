#include "editor.h"

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

#define MS_X (480)
#define MS_Y (480)

static char lastKey=0;

static MGLDraw *editmgl;

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

static byte musicPlaying;

byte editMode=EDITMODE_EDIT;

static editopt_t editopt={MAP_SHOWLIGHTS|MAP_SHOWWALLS|MAP_SHOWITEMS|MAP_SHOWBADGUYS|MAP_SHOWSPECIALS,
						PLOP_FLOOR,1,1,0,0,1,1};

byte InitEditor(void)
{
	NewWorld(&world,editmgl);
	curMap=world.map[0];
	curMapNum=0;

	//mouseX=MS_X;
	//mouseY=MS_Y;
	//MS_moveTo(MS_X,MS_Y);
	PutCamera(0,0);
	gameStartTime=timeGetTime();
	InitGuys(128);
	musicPlaying=0;
	lastKey=0;
	InitPlayer(INIT_LEVEL,0,0);

	return 1;
}

void ExitEditor(void)
{
	ExitGuys();
	FreeWorld(&world);
	PurgeMonsterSprites();
}

void ItemPickerClick(void)
{
	int n;

	n=(mouseX-20)/36+((mouseY-40)/50)*16;

	editMode=EDITMODE_EDIT;

	if(n<1 || n>=MAX_ITMS)
		return;

	editopt.curItem=(byte)n;
}

void EnemyPickerClick(void)
{
	int i,x,y;

	editMode=EDITMODE_EDIT;
	x=2;
	y=2;
#ifdef SHAREWARE
	for(i=1;i<4;i++)
#else
	for(i=1;i<NUM_MONSTERS;i++)
#endif
	{
		// several monster types can't be added, since they are parts of bigger monsters
		if(mouseX>x-1 && mouseX<x+201 && mouseY>y-1 && mouseY<y+15)
		{
			editopt.curBadguy=(byte)i;
			return;
		}
		x+=202;
		if(x>640-200)
		{
			x=2;
			y+=16;
		}
	}
}

void PickerMouseClick(void)
{
	int n;

	n=(mouseX/TILE_WIDTH)+(mouseY/TILE_HEIGHT)*20;

	if(n>199)	// clicked outside the display
	{
		if(editMode!=EDITMODE_PICKNEXT)
			editMode=EDITMODE_EDIT;
		else
			editMode=EDITMODE_TERRAIN;	// picking next is a submenu of terrain editing
		return;
	}
	else
	{
		if(editMode==EDITMODE_PICKFLR)
			editopt.curFloor=(byte)n;
		else if(editMode==EDITMODE_PICKWALL)
			editopt.curWall=(byte)n+1;
		else if(editMode==EDITMODE_PICKWALLFLR)
			editopt.curWallFloor=(byte)n;
		else if(editMode==EDITMODE_PICKNEXT)
			world.terrain[editopt.curFloor].next=(byte)n;

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
			world.terrain[editopt.curFloor].flags^=TF_MUD;
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
			world.terrain[editopt.curFloor].flags^=TF_DESTRUCT;
		if(mouseX>4 && mouseX<16 && mouseY>369 && mouseY<370+12)
			world.terrain[editopt.curFloor].flags^=TF_TRANS;
	}
	else
	{
		// pick the current floor
		editopt.curFloor=(byte)n;
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
	if(type==MONS_BOUAPHA)
	{
		for(i=0;i<MAX_MAPMONS;i++)
			if(curMap->badguy[i].type==MONS_BOUAPHA)
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
			InitSpclDialog(&curMap->special[i],&world,curMapNum);
			return;
		}
	}
	// if you got here, there is no special already on the space
	for(i=0;i<MAX_SPECIAL;i++)
		if(!curMap->special[i].trigger)
		{
			curMap->special[i].x=x;
			curMap->special[i].y=y;
			editMode=EDITMODE_SPECIAL;
			InitSpclDialog(&curMap->special[i],&world,curMapNum);
			return;
		}
}

void EditorMouseClick(void)
{
	int x1,x2,y1,y2,i,j;

	if(editMode==EDITMODE_PICKFLR || editMode==EDITMODE_PICKWALL || editMode==EDITMODE_PICKWALLFLR ||
		editMode==EDITMODE_PICKNEXT)
	{
		PickerMouseClick();
		plopCounter=100;
		return;
	}
	else if(editMode==EDITMODE_PICKITEM)
	{
		ItemPickerClick();
		plopCounter=100;
		return;
	}
	else if(editMode==EDITMODE_PICKENEMY)
	{
		EnemyPickerClick();
		plopCounter=100;
		return;
	}
	else if(editMode==EDITMODE_FILE)
	{
		if(!FileDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitFileDialog();
			plopCounter=100;
		}
		return;
	}
	else if(editMode==EDITMODE_TILELOAD)
	{
		if(!TileDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitTileDialog();
			plopCounter=100;
		}
		return;
	}
	else if(editMode==EDITMODE_MAPMENU)
	{
		if(!MapDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitMapDialog();
			plopCounter=100;
		}
		return;
	}
	else if(editMode==EDITMODE_SPECIAL)
	{
		if(!SpclDialogClick(mouseX,mouseY))
		{
			editMode=EDITMODE_EDIT;
			ExitSpclDialog();
			plopCounter=100;
		}
		return;
	}
	else if(editMode==EDITMODE_TERRAIN)
	{
		TerrainMouseClick();
		return;
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
							curMap->map[i+j*curMap->width].floor=editopt.curFloor;
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
							curMap->map[i+j*curMap->width].floor=editopt.curWallFloor;
							curMap->map[i+j*curMap->width].wall=editopt.curWall;
						}
					}
				break;
			case PLOP_ITEM:
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
		if(musicPlaying)
		{
			musicPlaying=0;
		}
		else
		{
			musicPlaying=1;
		}
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
		curMap->flags^=MAP_MANYITEMS;
	if(mouseX>445 && mouseX<487 && mouseY>453 && mouseY<454+14)
		curMap->flags^=MAP_SECRET;
	if(mouseX>487 && mouseX<529 && mouseY>421 && mouseY<422+14)
		curMap->flags^=MAP_TORCHLIT;


	plopCounter=100;
}

void UpdateMouse(void)
{
//	int msx,msy;
	int cx,cy;

	//MS_getPos(&msx,&msy);
	//MS_moveTo(MS_X,MS_Y);

	//mouseX+=(msx-MS_X);
	//mouseY+=(msy-MS_Y);

	editmgl->GetMouse(&mouseX,&mouseY);
	//mouseX=MS_X;
	//mouseY=MS_Y;

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
			if(mouseY>419)
				EditorMenuClick();
			return;
		}

		// these are the draggable tools
		if((editMode==EDITMODE_EDIT) &&
			(editopt.plopMode==PLOP_FLOOR || editopt.plopMode==PLOP_WALL ||
			editopt.plopMode==PLOP_LIGHT || editopt.plopMode==PLOP_DARK ||
			editopt.plopMode==PLOP_SMOOTH || editopt.plopMode==PLOP_ITEM))
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
	while(*lastTime>0)
	{
		if(!editmgl->Process())
			return QUITGAME;

		// update everything here
		EditorUpdateGuys(curMap);
		curMap->Update(UPDATE_EDIT,&world);
		UpdateMouse();

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}

	JamulSoundUpdate();

	return CONTINUE;
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
	editmgl->Box(446,438,486,438+14,16+15*((curMap->flags&MAP_MANYITEMS)!=0));
	Print(448,440,"Boss",0,1);
	editmgl->Box(446,454,486,454+14,16+15*((curMap->flags&MAP_SECRET)!=0));
	Print(448,456,"Secrt",0,1);
	editmgl->Box(488,422,528,422+14,16+15*((curMap->flags&MAP_TORCHLIT)!=0));
	Print(490,424,"Lit",0,1);

	// now show the current stuff for the plop mode
	switch(editopt.plopMode)
	{
		case PLOP_FLOOR:
			RenderFloorTileUnlit(602,450,editopt.curFloor);
			break;
		case PLOP_WALL:
			RenderFloorTileUnlit(602,450-24,editopt.curWallFloor);
			RenderFloorTileUnlit(602,450,editopt.curWall+199);
			break;
		case PLOP_ITEM:
			InstaRenderItem(626,464,editopt.curItem,0,editmgl);
			break;
		case PLOP_BADGUY:
			InstaRenderMonster(626,464,editopt.curBadguy,0,editmgl);
			break;
	}

	if(tileX>=0 && tileY>=0 && tileX<curMap->width && tileY<curMap->height)
	{
		sprintf(s,"(%03d,%03d)",tileX,tileY);
		Print(580,408,s,0,1);
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
		|| editopt.plopMode==PLOP_TORCH)
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

void RenderTerrainInfo(byte which)
{
	char s[32];

	RenderCheckbox(5,250,world.terrain[which].flags&TF_SOLID,"Impassable");
	RenderCheckbox(5,262,world.terrain[which].flags&TF_ICE,"Icy");
	RenderCheckbox(5,274,world.terrain[which].flags&TF_MUD,"Muddy");
	RenderCheckbox(5,286,world.terrain[which].flags&TF_WATER,"Water");
	RenderCheckbox(5,298,world.terrain[which].flags&TF_LAVA,"Lava");
	RenderCheckbox(5,310,world.terrain[which].flags&TF_PUSHY,"Pushable");
	RenderCheckbox(5,322,world.terrain[which].flags&TF_PUSHON,"Can be pushed on");
	RenderCheckbox(5,334,world.terrain[which].flags&TF_ANIM,"Animates to next");
	RenderCheckbox(5,346,world.terrain[which].flags&TF_STEP,"Changes when stepped on");
	RenderCheckbox(5,358,world.terrain[which].flags&TF_DESTRUCT,"Changes when damaged");
	RenderCheckbox(5,370,world.terrain[which].flags&TF_TRANS,"Transparent Roof");

	Print(5,386,"Next Terrain:",0,1);
	RenderFloorTileUnlit(5,398,world.terrain[which].next);

	sprintf(s,"Terrain #%03d",which);
	Print(5,468,s,0,1);
}

void RenderItemPickDisplay(void)
{
	int i,j;
	char s[32];
	byte on;

	editmgl->ClearScreen();

	on=0;
	for(i=0;i<16;i++)
		for(j=0;j<8;j++)
		{
			if(i+j*16<MAX_ITMS)
			{
				editmgl->Box(20+i*36,40+j*50,20+(i+1)*36,40+(j+1)*50,16);
				if(mouseX>20+i*36 && mouseX<20+(i+1)*36 &&
					mouseY>40+j*50 && mouseY<40+(j+1)*50)
				{
					InstaRenderItem(20+i*36+18,40+j*50+25,i+j*16,16,editmgl);
					on=i+j*16;
				}
				else
					InstaRenderItem(20+i*36+18,40+j*50+25,i+j*16,0,editmgl);
			}
		}

	if(on)
	{
		sprintf(s,"Item #%03d",on);
		Print(5,468,s,0,1);
	}
}

void RenderEnemyPickDisplay(void)
{
	int i,x,y;
	char s[32];

	editmgl->ClearScreen();

	x=2;
	y=2;

#ifdef SHAREWARE
	for(i=1;i<4;i++)	// limit the number of monsters selectable in shareware editor
#else
	for(i=1;i<NUM_MONSTERS;i++)
#endif
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
			y+=16;
		}
	}
}

void RenderPickDisplay(void)
{
	int i,j;
	byte selected;
	char s[32];

	editmgl->FillBox(0,240,639,479,0);

	selected=255;
	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
		{
			if((editMode==EDITMODE_PICKFLR) || (editMode==EDITMODE_PICKWALLFLR) ||
				(editMode==EDITMODE_PICKNEXT))
			{
				if(mouseX>i*TILE_WIDTH-1 && mouseX<i*TILE_WIDTH+TILE_WIDTH &&
					mouseY>j*TILE_HEIGHT-1 && mouseY<j*TILE_HEIGHT+TILE_HEIGHT)
				{
					RenderFloorTile(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20,16);
					selected=i+j*20;
				}
				else
					RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20);

				if(i+j*20==editopt.curFloor && (editMode==EDITMODE_PICKFLR || editMode==EDITMODE_PICKNEXT))
					editmgl->Box(i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,
							j*TILE_HEIGHT+TILE_HEIGHT-1,31);
				if(i+j*20==editopt.curWallFloor && editMode==EDITMODE_PICKWALLFLR)
					editmgl->Box(i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,
							j*TILE_HEIGHT+TILE_HEIGHT-1,31);
			}
			else if(editMode==EDITMODE_PICKWALL)
			{
				if(mouseX>i*TILE_WIDTH-1 && mouseX<i*TILE_WIDTH+TILE_WIDTH &&
					mouseY>j*TILE_HEIGHT-1 && mouseY<j*TILE_HEIGHT+TILE_HEIGHT)
				{
					selected=i+j*20;
					RenderFloorTile(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+200,16);
				}
				else
					RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+200);

				if(i+j*20==editopt.curWall-1)
					editmgl->Box(i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,
							j*TILE_HEIGHT+TILE_HEIGHT-1,31);
			}
		}

	if(editMode==EDITMODE_PICKFLR)
		Print(400,242,"Click on floor tile to use",0,1);
	else if(editMode==EDITMODE_PICKWALL)
		Print(400,242,"Click on wall tile to use",0,1);
	else if(editMode==EDITMODE_PICKWALLFLR)
		Print(400,242,"Click on roof tile to use",0,1);
	else if(editMode==EDITMODE_PICKNEXT)
		Print(400,242,"Click on 'next' tile",0,1);

	if((editMode!=EDITMODE_PICKWALL) && (selected!=255))	// walls have no special features
	{
		// describe the highlighted terrain
		RenderTerrainInfo(selected);
	}
	else if(editMode==EDITMODE_PICKWALL)
	{
		if(selected!=255)
		{
			sprintf(s,"Wall #%03d",selected+1);
			Print(5,468,s,0,1);
		}
	}

}

void RenderTerrainEditor(void)
{
	int i,j;
	byte selected;

	editmgl->FillBox(0,240,639,479,0);

	selected=255;
	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
		{
			if(mouseX>i*TILE_WIDTH-1 && mouseX<i*TILE_WIDTH+TILE_WIDTH &&
				mouseY>j*TILE_HEIGHT-1 && mouseY<j*TILE_HEIGHT+TILE_HEIGHT)
			{
				RenderFloorTile(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20,16);
				selected=i+j*20;
			}
			else
				RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20);
			if(i+j*20==editopt.curFloor)
				editmgl->Box(i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,
							j*TILE_HEIGHT+TILE_HEIGHT-1,31);
		}
	RenderTerrainInfo(editopt.curFloor);
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
	//editmgl->ClearScreen();
	switch(editMode)
	{
		case EDITMODE_EDIT:
			if(editopt.displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(editopt.displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,curMap,editopt.displayFlags);
			ShowSpecials();
			ShowTarget();
			RenderEditMenu();
			break;
		case EDITMODE_TERRAIN:
			RenderTerrainEditor();
			break;
		case EDITMODE_PICKFLR:
		case EDITMODE_PICKWALL:
		case EDITMODE_PICKWALLFLR:
		case EDITMODE_PICKNEXT:
			RenderPickDisplay();
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

	// draw the mouse cursor
	DrawMouseCursor(mouseX,mouseY);

	editmgl->Flip();
}

static void HandleKeyPresses(void)
{
	char k;
	int x,y;

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
			case 'c':
				Cavernize(curMap);
				break;
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
				if(editopt.curFloor>199)
					editopt.curFloor=199;
				break;
			case '=':
			case '+':
				editopt.curFloor++;
				if(editopt.curFloor>199)
					editopt.curFloor=0;
				break;
			case 'o':
				editopt.curWallFloor--;
				if(editopt.curWallFloor>199)
					editopt.curWallFloor=199;
				break;
			case 'p':
				editopt.curWallFloor++;
				if(editopt.curWallFloor>199)
					editopt.curWallFloor=0;
				break;
			case '[':
				editopt.curWall--;
				if(editopt.curWall>199)
					editopt.curWall=199;
				break;
			case ']':
				editopt.curWall++;
				if(editopt.curWall>199)
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
			case ';':
				editopt.curBadguy--;
				if(editopt.curBadguy<1)
					editopt.curBadguy=NUM_MONSTERS-1;
				break;
			case '\'':
				editopt.curBadguy++;
				if(editopt.curBadguy>=NUM_MONSTERS)
					editopt.curBadguy=1;
				break;
			case 8:
				Delete(tileX,tileY);
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
		if(numRunsToMakeUp>0)
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
	InitGuys(128);
	AddMapGuys(curMap);
}

void EditorLoadWorld(char *fname)
{
	FreeWorld(&world);
	if(LoadWorld(&world,fname))
	{
		curMap=world.map[0];
		ExitGuys();
		InitGuys(128);
		AddMapGuys(curMap);
	}
	else
	{
		NewWorld(&world,editmgl);
		curMap=world.map[0];
		ExitGuys();
		InitGuys(128);
		AddMapGuys(curMap);
	}
}

void EditorLoadTiles(char *fname)
{
	editmgl->LoadBMP(fname);
	SetTiles(editmgl->GetScreen());
}

void EditorSaveWorld(char *fname)
{
	SaveWorld(&world,fname);
}

void EditorSelectMap(byte w)
{
	curMap=world.map[w];
	curMapNum=w;
	PutCamera(320<<FIXSHIFT,240<<FIXSHIFT);
	ExitGuys();
	InitGuys(128);
	AddMapGuys(curMap);
}
