#include "editor.h"
#include "map.h"
#include "editmenu.h"
#include "toolmenu.h"
#include "yesnomenu.h"
#include "music.h"

#define PLOPRATE	5

byte testingLevel;

static char lastKey=0;

static MGLDraw *editmgl;

static world_t	world;
#define curMap editMap
static Map		*curMap;
static byte		curMapNum;
static int	   mouseX,mouseY;
static int	   tileX,tileY;

static byte  showStats=0;
static dword gameStartTime,visFrameCount,updFrameCount;
static word  numRunsToMakeUp;

static byte msButton=0,msButton2=0;
static byte showTags;

static char cursorBright;
static char cursorDBright;

static int tileOffset;
static byte ms2HeldOnMenu,msHeldOnMenu;

static byte menusHidden=0;

byte editing=0;

byte editMode=EDITMODE_EDIT;

static editopt_t editopt={MAP_SHOWLIGHTS|MAP_SHOWWALLS|MAP_SHOWITEMS|MAP_SHOWBADGUYS|MAP_SHOWSPECIALS,
						PLOP_FLOOR,1,1,0,0,1,0,1,1,0,0,0,0};

sprite_set_t *editspr;
ToolMenu *toolMenu;
EditMenu *editMenu;
YesNoMenu *yesNoMenu;

byte InitEditor(void)
{
	JamulSoundPurge();
	NewWorld(&world,editmgl);
	curMap=world.map[0];
	curMapNum=0;

	mouseX=320;
	mouseY=240;
	editmgl->SetMouse(320,240);
	gameStartTime=timeGetTime();
	InitGuys(MAX_MAPMONS);
	lastKey=0;
	InitPlayer(INIT_GAME,0,0);
	tileOffset=0;
	showTags=1;
	editing=1;

	editMode=EDITMODE_EDIT;

	msHeldOnMenu=0;
	ms2HeldOnMenu=0;

	cursorBright=0;
	cursorDBright=1;
	editspr=new sprite_set_t("graphics/editmenu.jsp");
	toolMenu=new ToolMenu(editmgl);
	yesNoMenu=new YesNoMenu(editmgl);
	editMenu=new EditMenu(curMap,yesNoMenu,toolMenu,editmgl);

	Music_Stop();
	menusHidden=0;

	return 1;
}

void ExitEditor(void)
{
	ExitGuys();
	FreeWorld(&world);
	PurgeMonsterSprites();
	editing=0;
	delete editspr;
	delete toolMenu;
	delete editMenu;
	delete yesNoMenu;
}

world_t *EditorGetWorld(void)
{
	return &world;
}

Map *EditorGetMap(void)
{
	return curMap;
}

byte EditorGetMapNum(void)
{
	return curMapNum;
}

void Delete(int x,int y)
{
	if(x<0 || x>curMap->width-1 || y<0 || y>curMap->height-1)
		return;

	MakeNormalSound(SND_MENUCANCEL);
	curMap->map[x+y*curMap->width].item=0;

	if(editMenu->DelMode()==0)
		curMap->map[x+y*curMap->width].floor=0;

	DeleteMonsters();
	AddMapGuys(curMap);
}

byte EditorMouseClick(void)
{
	toolMenu->Plop(&curMap->map[tileX+tileY*curMap->width],curMap);

	MakeNormalSound(SND_MENUCLICK);

	return 1;
}

void UpdateMouse(void)
{
	int msx,msy;
	int cx,cy;
	static int oldTileX,oldTileY;

	editmgl->GetMouse(&msx,&msy);

	mouseX=msx;
	mouseY=msy;

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
			if(yesNoMenu->Click(mouseX,mouseY,1))
			{
				MakeNormalSound(SND_MENUCLICK);
				editMenu->GetAnswer();
				msHeldOnMenu=1;
				return;	// nothing more to do
			}
			if(!menusHidden && toolMenu->Click(mouseX,mouseY,1))
			{
				MakeNormalSound(SND_MENUCLICK);
				msHeldOnMenu=1;
				return;	// nothing more to do
			}
			if(!menusHidden && editMenu->Click(mouseX,mouseY,1))
			{
				MakeNormalSound(SND_MENUCLICK);
				msHeldOnMenu=1;
				return;	// nothing more to do
			}

			msHeldOnMenu=0;
			// if you got here, no menus are being clicked
			EditorMouseClick();
			editMode=EDITMODE_DRAGPLOP;
		}
		else	// button is held down
		{
			if(editMode==EDITMODE_DRAGTOOL)
			{
				toolMenu->DraggedByMouse(mouseX,mouseY);
				return;
			}
			if(editMode==EDITMODE_DRAGMENU)
			{
				editMenu->DraggedByMouse(mouseX,mouseY);
				return;
			}
			if(msHeldOnMenu)
			{
				editMenu->MouseHold(mouseX,mouseY,1);
				return;
			}
			else if(editMode==EDITMODE_DRAGPLOP && (tileX!=oldTileX || tileY!=oldTileY))
			{
				EditorMouseClick();
			}

		}
	}
	else
	{
		// mouse released
		msButton=0;

		// any 'drag' modes are exited
		if(editMode>=EDITMODE_DRAGTOOL && editMode<=EDITMODE_DRAGPLOP)
			editMode=EDITMODE_EDIT;
	}

	if(editmgl->RMouseDown())
	{
		if(!msButton2)
		{
			msButton2=1;
			if(yesNoMenu->Click(mouseX,mouseY,2))
			{
				MakeNormalSound(SND_MENUCLICK);
				editMenu->GetAnswer();
				ms2HeldOnMenu=1;
				return;	// nothing more to do
			}
			if(!menusHidden && toolMenu->Click(mouseX,mouseY,2))
			{
				MakeNormalSound(SND_MENUCLICK);
				ms2HeldOnMenu=1;
				return;	// nothing more to do
			}
			if(!menusHidden && editMenu->Click(mouseX,mouseY,2))
			{
				MakeNormalSound(SND_MENUCLICK);
				ms2HeldOnMenu=1;
				return;	// nothing more to do
			}
			Delete(tileX,tileY);
			ms2HeldOnMenu=0;
		}
		else
		{
			if(ms2HeldOnMenu)
			{
				editMenu->MouseHold(mouseX,mouseY,2);
				return;
			}
			else if(tileX!=oldTileX || tileY!=oldTileY)
				Delete(tileX,tileY);
		}
	}
	else
		msButton2=0;

	oldTileX=tileX;
	oldTileY=tileY;
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
		cursorBright+=cursorDBright;
		if(cursorBright>10)
			cursorDBright=-1;
		if(cursorBright<-4)
			cursorDBright=1;

		EditorUpdateGuys(curMap);
		UpdateItems();
		curMap->Update(UPDATE_EDIT,&world);
		UpdateMouse();

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;

		Music_Update();
	}

	JamulSoundUpdate();

	return CONTINUE;
}

void ShowTarget(void)
{
	int x1;

	for(x1=0;x1<curMap->width*curMap->height;x1++)
	{
		if(curMap->map[x1].light>0)
			curMap->map[x1].light--;
	}
	if(tileX>=0 && tileX<curMap->width && tileY>=0 && tileY<curMap->height)
		if(curMap->map[tileX+tileY*curMap->width].light<16)
			curMap->map[tileX+tileY*curMap->width].light+=2;
}

void EditorDraw(void)
{
	if(editMenu->ShowGuys())
		RenderGuys(1);
	editopt.displayFlags=MAP_SHOWLIGHTS|MAP_SHOWWALLS|MAP_SHOWBADGUYS|MAP_SHOWSPECIALS;
	if(editMenu->ShowItems())
		editopt.displayFlags|=MAP_SHOWITEMS;

	RenderItAll(&world,curMap,editopt.displayFlags);
	ShowTarget();

	if(!menusHidden)
	{
		editMenu->Render(mouseX,mouseY);
		toolMenu->Render(mouseX,mouseY);
	}
	yesNoMenu->Render(mouseX,mouseY);

	// draw the mouse cursor
	editspr->GetSprite(19)->DrawBright(mouseX,mouseY,editmgl,cursorBright);

	editmgl->Flip();
}

static void HandleKeyPresses(void)
{
	char k;
	byte c;

	k=editmgl->LastKeyPressed();
	if(k)
		lastKey=k;

	c=GetTaps();

	if(editMode==EDITMODE_EDIT || editMode==EDITMODE_YESNO)
	{
		if(yesNoMenu->KeyPress(k))
		{
			editMenu->GetAnswer();
		}
		else if(menusHidden || !editMenu->KeyPress(k))
		{
			if(!editMenu->AlwaysKeyPress(k,tileX,tileY))
			{
				switch(k)
				{
					case 8: // backspace
						Delete(tileX,tileY);
						break;
					case 9:	// tab
						menusHidden=1-menusHidden;
						break;
					case 'Y':
						curMap->Parallelize();
						break;
					//case 'U':
					//	curMap->Shift();
					//	break;
					case '-':
						toolMenu->ChgCurTool(-1);
						break;
					case '+':
					case '=':
						toolMenu->ChgCurTool(1);
						break;
				}
			}
		}
	}

	if(c&CONTROL_LF)
	{
		toolMenu->color--;
		if(toolMenu->color>7)
			toolMenu->color=7;

	}
	if(c&CONTROL_RT)
	{
		toolMenu->color++;
		if(toolMenu->color>7)
			toolMenu->color=0;
	}
	if(c&CONTROL_UP)
	{
		toolMenu->facing--;
		if(toolMenu->facing>3)
			toolMenu->facing=3;
	}
	if(c&CONTROL_DN)
	{
		toolMenu->facing++;
		if(toolMenu->facing>3)
			toolMenu->facing=0;
	}
}

byte MenusHidden(void)
{
	return menusHidden;
}

void HideMenus(void)
{
	menusHidden=1-menusHidden;
}

byte LunaticEditor(MGLDraw *mgl)
{
	int lastTime=1;
	byte exitcode=0;

	editmgl=mgl;
	GetTaps();

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

		if(editMode==EDITMODE_EXIT)
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

void EditorLoadWorld(char *fname)
{
	FreeWorld(&world);
	if(LoadWorld(&world,fname,editmgl))
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

void EditorMergeWorld(char *fname)
{
	int i;
	world_t tmp;

	if(LoadWorld(&tmp,fname,editmgl))
	{
		for(i=0;i<tmp.numMaps;i++)
		{
			if(world.numMaps<MAX_MAPS)
			{
				world.map[world.numMaps]=new Map(tmp.map[i]);
				world.numMaps++;
			}
		}
		FreeWorld(&tmp);
	}
}

void EditorLoadTiles(char *fname,byte wall)
{
	editmgl->LoadBMP(fname);
	SetTiles(editmgl->GetScreen(),wall);
}

void EditorSaveWorld(char *fname)
{
	int i;

	for(i=0;i<curMap->width*curMap->height;i++)
		curMap->map[i].light=0;
	SaveWorld(&world,fname);
}

void EditorSelectMap(byte w)
{
	curMap=world.map[w];
	curMapNum=w;
	ExitGuys();
	InitGuys(MAX_MAPMONS);
	AddMapGuys(curMap);
}
