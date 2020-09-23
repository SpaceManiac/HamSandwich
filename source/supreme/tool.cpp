#include "winpch.h"
#include "tool.h"
#include "tool_floor.h"
#include "tool_wall.h"
#include "tool_item.h"
#include "tool_monster.h"
#include "tool_light.h"
#include "tool_eraser.h"
#include "tool_special.h"
#include "tool_select.h"
#include "dialogbits.h"
#include "terrainedit.h"
#include "soundedit.h"
#include "monsteredit.h"
#include "itemedit.h"
#include "edithelp.h"
#include "yesnodialog.h"
#include "mapdialog.h"
#include "leveldialog.h"
#include "viewdialog.h"

static Tool *tool[NUM_TOOLS],*curTool;

static byte whichTool;
static byte doing;

static char toolName[NUM_TOOLS][16]={"Floor","Wall","Item","Badguy","Light","Special","Select",
		"Eraser"};
static char menuName[NUM_MENUS][16]={"File","World","TEST!","Level","View","Tiles","Items","Sound",
		"Exit"};

char worldFilename[64];

void ToolInit(void)
{
	int i;

	worldFilename[0]='\0';
	doing=TD_USING;
	whichTool=TOOL_FLOOR;

	for(i=0;i<NUM_TOOLS;i++)
		tool[i]=NULL;

	tool[TOOL_FLOOR]=new FloorTool();
	tool[TOOL_WALL]=new WallTool();
	tool[TOOL_ITEM]=new ItemTool();
	tool[TOOL_BADGUY]=new MonsterTool();
	tool[TOOL_LIGHT]=new LightTool();
	tool[TOOL_SELECT]=new SelectTool();
	tool[TOOL_ERASER]=new EraserTool();
	tool[TOOL_SPECIAL]=new SpecialTool();
	curTool=tool[0];
}

void ToolExit(void)
{
	int i;

	for(i=0;i<NUM_TOOLS;i++)
		if(tool[i])
		{
			delete tool[i];
			tool[i]=NULL;
		}
}

byte ToolDoing(void)
{
	return doing;
}

void ToolUpdate(int msx,int msy,byte editMenu,MGLDraw *mgl)
{
	switch(doing)
	{
		case TD_USING:
			if(editMenu)
			{
				if(PointInRect(msx,msy,380,386,435,400))	// tool name
				{
					if(mgl->MouseTap())
						doing=TD_PICKTOOL;
				}
				else if(PointInRect(msx,msy,584,386,639,400))	// menu list
				{
					if(mgl->MouseTap())
						doing=TD_PICKMENU;
				}
				else if(!PointInRect(msx,msy,380,400,639,479))
				{
					// not anywhere in the menu
					if(mgl->MouseTap())
					{
						doing=TD_PLOPPING;
						curTool->StartPlop();
					}
					if(mgl->RMouseTap())
					{
						doing=TD_ERASING;
						curTool->StartErase();
					}
				}
			}
			else
			{
				// not anywhere in the menu, no matter what, since the menu is missing!
				if(mgl->MouseTap())
				{
					doing=TD_PLOPPING;
					curTool->StartPlop();
				}
				if(mgl->RMouseTap())
				{
					doing=TD_ERASING;
					curTool->StartErase();
				}
			}
			curTool->Update(msx,msy);
			break;
		case TD_PICKTOOL:
			if(mgl->MouseDown())
			{
				// do nothing, continue picking the tool
			}
			else
			{
				// you released the mouse, so select the tool if on it
				if(PointInRect(msx,msy,380,400-16*NUM_TOOLS,435,399))
				{
					whichTool=(msy-(400-16*NUM_TOOLS))/16;
					curTool=tool[whichTool];
				}
				doing=TD_USING;
			}
			break;
		case TD_PICKMENU:
			if(mgl->MouseDown())
			{
				// do nothing, continue picking the menu
			}
			else
			{
				// you released the mouse, so select the menu if on it
				if(PointInRect(msx,msy,584,400-16*NUM_MENUS,639,399))
				{
					switch((msy-(400-16*NUM_MENUS))/16)
					{
						case MENU_FILE:
							InitFileDialog("worlds/*.hsw",FM_SAVE|FM_LOAD|FM_NEW|FM_EXIT|FM_ASKLOAD,worldFilename);
							SetEditMode(EDITMODE_FILE);
							break;
						case MENU_WORLD:
							SetEditMode(EDITMODE_MAPMENU);
							InitMapDialog(EditorGetWorld(),EditorGetMapNum());
							break;
						case MENU_TEST:
							int cx,cy;
							GetCamera(&cx,&cy);
							TestLevel(EditorGetWorld(),EditorGetMapNum());
							StopSong();
							SetPlayerStart(-1,-1);
							AddMapGuys(EditorGetMap());
							PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
							break;
						case MENU_LEVEL:
							SetEditMode(EDITMODE_LEVELMENU);
							InitLevelDialog(EditorGetWorld(),EditorGetMapNum());
							break;
						case MENU_TILES:
							TerrainEdit_Init(EditorGetWorld());
							SetEditMode(EDITMODE_TERRAIN);
							break;
						case MENU_ITEMS:
							ItemEdit_Init(EDITMODE_EDIT,EditorGetWorld(),0);
							SetEditMode(EDITMODE_ITEM);
							break;
						case MENU_SOUND:
							SoundEdit_Init(EditorGetWorld());
							SetEditMode(EDITMODE_SOUND);
							break;
						case MENU_DISPLAY:
							ViewMenuOn();
							break;
						case MENU_EXIT:
							InitYesNoDialog("Exit the editor?","Yes","No");
							SetEditMode(EDITMODE_EXIT);
							break;
					}
				}
				doing=TD_USING;
			}
			break;
		case TD_PICKINK:
			curTool->SetInk();
			doing=TD_USING;
			break;
		case TD_PLOPPING:
			if(mgl->MouseDown())
			{
				curTool->Plop();
			}
			else
				doing=TD_USING;
			break;
		case TD_ERASING:
			if(mgl->RMouseDown())
			{
				curTool->Erase();
			}
			else
				doing=TD_USING;
			break;
	}
}

void RenderToolList(int msx,int msy,MGLDraw *mgl)
{
	int i;

	mgl->FillBox(380,400-16*NUM_TOOLS,435,400,6);
	mgl->Box(380,400-16*NUM_TOOLS,435,400,31);
	for(i=0;i<NUM_TOOLS;i++)
	{
		if(PointInRect(msx,msy,380,400-16*NUM_TOOLS+i*16,435,400-16*NUM_TOOLS+i*16+15))
			mgl->FillBox(381,401-16*NUM_TOOLS+i*16,434,400-16*NUM_TOOLS+i*16+15,8+32*1);
		CenterPrint((435-380)/2+380,400-16*NUM_TOOLS+2+i*16,toolName[i],0,1);
	}
}

void RenderMenuList(int msx,int msy,MGLDraw *mgl)
{
	int i;

	mgl->FillBox(584,400-16*NUM_MENUS,639,400,6);
	mgl->Box(584,400-16*NUM_MENUS,639,400,31);
	for(i=0;i<NUM_MENUS;i++)
	{
		if(PointInRect(msx,msy,584,400-16*NUM_MENUS+i*16,639,400-16*NUM_MENUS+i*16+15))
			mgl->FillBox(585,401-16*NUM_MENUS+i*16,638,400-16*NUM_MENUS+i*16+15,8+32*1);
		CenterPrint((639-584)/2+584,400-16*NUM_MENUS+2+i*16,menuName[i],0,1);
	}

}

void ToolRender(int msx,int msy,MGLDraw *mgl)
{
	// the base portion of the tool menu
	mgl->FillBox(380,400,639,479,6);
	mgl->Box(380,400,639,479,31);

	if(doing!=TD_PICKTOOL)
	{
		// make the tab for the tool name
		if(PointInRect(msx,msy,380,386,435,400))
			mgl->FillBox(380,386,435,400,8+32*1);
		else
			mgl->FillBox(380,386,435,400,6);
		mgl->Box(380,386,435,400,31);
		CenterPrint((435-380)/2+380,389,toolName[whichTool],0,1);
	}
	else
	{
		RenderToolList(msx,msy,mgl);
	}
	if(doing==TD_PICKMENU)
		RenderMenuList(msx,msy,mgl);
	else
	{
		if(PointInRect(msx,msy,584,386,639,400))
			mgl->FillBox(584,386,639,400,8+32*1);
		else
			mgl->FillBox(584,386,639,400,6);
		mgl->Box(584,386,639,400,31);
		CenterPrint((639-584)/2+584,389,"Menus",0,1);
	}

	curTool->Render(msx,msy);
}

void ToolPickInk(void)
{
	doing=TD_PICKINK;
}

void ToolShowTarget(void)
{
	curTool->ShowTarget();
}

void ToolSetFilename(void)
{
	strcpy(worldFilename,GetFilename(""));
}

char *ToolGetFilename(void)
{
	return worldFilename;
}

void ToolSuckUp(int x,int y)
{
	curTool->SuckUp(x,y);
}

void ToolBrushUp(void)
{
	curTool->BrushUp();
}

void ToolGetHelp(void)
{
	SetEditMode(EDITMODE_HELP);
	InitEditHelp(HELP_FLOORTOOL+whichTool);
}

byte WhichTool(void)
{
	return whichTool;
}

void PickTool(byte w)
{
	whichTool=w;
	curTool=tool[w];
	doing=TD_USING;
}

void ToolItemWasDeleted(int i)
{
	((MonsterTool *)tool[TOOL_BADGUY])->ItemWasDeleted(i);
	((ItemTool *)tool[TOOL_ITEM])->ItemWasDeleted(i);
}

//------------------------------------
Tool::~Tool()
{
}
void Tool::Update(int msx,int msy)
{
}
void Tool::Render(int msx,int msy)
{
}
void Tool::StartPlop(void)
{
}
void Tool::Plop(void)
{
}
void Tool::StartErase(void)
{
}
void Tool::Erase(void)
{
}
void Tool::SetInk(void)
{
}
void Tool::ShowTarget(void)
{
}

void Tool::SuckUp(int x,int y)
{
}

void Tool::BrushUp(void)
{
}
