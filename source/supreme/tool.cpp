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
#include "appdata.h"

// Begin JspEdit crap
#if defined(_WIN32)
#include <windows.h>
#define CAN_LAUNCH_JSPEDIT
bool LaunchTool(char* cmdline)
{
	STARTUPINFOA startupInfo = {};
	PROCESS_INFORMATION processInfo = {};
	startupInfo.cb = sizeof(startupInfo);
	if (CreateProcessA(cmdline, nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &startupInfo, &processInfo))
	{
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		return true;
	}
	else
	{
		fprintf(stderr, "CreateProcess failed: %lu\n", GetLastError());
		return false;
	}
}
#elif !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
#include <unistd.h>
#define CAN_LAUNCH_JSPEDIT
bool LaunchTool(char* cmdline)
{
	int child_pid = fork();
	if (child_pid == 0)
	{
		char* argv[2] = { cmdline, nullptr };

		execv(argv[0], argv);
		fprintf(stderr, "LaunchTool(%s):\n", cmdline);
		perror("execv");
		exit(1);
	}
	else if (child_pid > 0)
	{
		// if execv fails we'll sadly never know
		return true;
	}
	else
	{
		perror("fork");
		return false;
	}
}
#endif

#ifdef CAN_LAUNCH_JSPEDIT
void LaunchJspEdit()
{
	std::string cmdline = EscapeBinDirectory();
	cmdline.append("/jspedit");
#if defined(_WIN32)
	cmdline.append(".exe");
#endif

	if (LaunchTool(cmdline.data()))
	{
		GetDisplayMGL()->SetWindowed(true);
	}
}
bool CheckJspEdit()
{
	std::string cmdline = EscapeBinDirectory();
	cmdline.append("/jspedit");
#if defined(_WIN32)
	cmdline.append(".exe");
#endif

	return bool(owned::fopen(cmdline.c_str(), "rb"));
}
#else
void LaunchJspEdit()
{
}
bool CheckJspEdit()
{
	return false;
}
#endif
// End JspEdit crap

static Tool *tool[NUM_TOOLS];
static Tool *curTool;

static byte whichTool;
static byte doing;

static const char toolName[NUM_TOOLS][16]={
	"Floor","Wall","Item","Badguy","Light","Special","Select","Eraser"
};
static const char menuName[NUM_MENUS][16]={
	"File","World","TEST!","Level","View","Tiles","Items","Sound","JspEdit","Exit"
};
static std::vector<byte> visMenus;

static char worldFilename[64];

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

	visMenus.clear();
	for (int i = 0; i < NUM_MENUS; ++i)
		visMenus.push_back(i);
	if (!CheckJspEdit())
		visMenus.erase(visMenus.begin() + MENU_JSPEDIT);
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

TASK(void) ToolUpdate(int msx,int msy,byte editMenu,MGLDraw *mgl)
{
	switch(doing)
	{
		case TD_USING:
			if(editMenu)
			{
				if(PointInRect(msx, msy, mgl->GetWidth()-260, mgl->GetHeight()-80-14, mgl->GetWidth()-260+55, mgl->GetHeight()-80))  // tool name
				{
					if(mgl->MouseTap())
						doing=TD_PICKTOOL;
				}
				else if(PointInRect(msx, msy, mgl->GetWidth()-55-1, mgl->GetHeight()-80-14, mgl->GetWidth()-1, mgl->GetHeight()-80))  // menu list
				{
					if(mgl->MouseTap())
						doing=TD_PICKMENU;
				}
				else if(!PointInRect(msx, msy, mgl->GetWidth()-260, mgl->GetHeight()-80, mgl->GetWidth()-1, mgl->GetHeight()-1))
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
				if(PointInRect(msx,msy,mgl->GetWidth()-260,mgl->GetHeight()-80-16*NUM_TOOLS,mgl->GetWidth()-260+55,mgl->GetHeight()-80-1))
				{
					whichTool=(msy-(mgl->GetHeight()-80-16*NUM_TOOLS))/16;
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
				if(PointInRect(msx,msy,mgl->GetWidth()-55-1,mgl->GetHeight()-80-16*visMenus.size(),mgl->GetWidth()-1,mgl->GetHeight()-80-1))
				{
					switch(visMenus[(msy-(mgl->GetHeight()-80-16*visMenus.size()))/16])
					{
						case MENU_FILE:
							InitFileDialog("worlds",".dlw",FM_SAVE|FM_LOAD|FM_NEW|FM_EXIT|FM_ASKLOAD|FM_SAVEPACK,worldFilename);
							SetEditMode(EDITMODE_FILE);
							break;
						case MENU_WORLD:
							SetEditMode(EDITMODE_MAPMENU);
							InitMapDialog(EditorGetWorld(),EditorGetMapNum());
							break;
						case MENU_TEST:
						{
							auto [cx, cy] = GetCamera();
							AWAIT TestLevel(EditorGetWorld(),EditorGetMapNum());
							StopSong();
							SetPlayerStart(-1,-1);
							AddMapGuys(EditorGetMap());
							PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
							break;
						}
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
						case MENU_JSPEDIT:
							LaunchJspEdit();
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
	mgl->FillBox(mgl->GetWidth()-260, mgl->GetHeight()-80-16*NUM_TOOLS, mgl->GetWidth()-260+55, mgl->GetHeight()-80, 6);
	mgl->Box(mgl->GetWidth()-260, mgl->GetHeight()-80-16*NUM_TOOLS, mgl->GetWidth()-260+55, mgl->GetHeight()-80, 31);
	for (int i=0; i<NUM_TOOLS; i++)
	{
		if (PointInRect(msx, msy, mgl->GetWidth()-260, mgl->GetHeight()-80-16*NUM_TOOLS+i*16, mgl->GetWidth()-260+55, mgl->GetHeight()-80-16*NUM_TOOLS+i*16+15))
			mgl->FillBox(mgl->GetWidth()-260+1, mgl->GetHeight()-80+1-16*NUM_TOOLS+i*16, mgl->GetWidth()-260+55-1, mgl->GetHeight()-80-16*NUM_TOOLS+i*16+15, 8+32*1);
		CenterPrint(mgl->GetWidth()-260+55/2, mgl->GetHeight()-80-16*NUM_TOOLS+2+i*16, toolName[i], 0, 1);
	}
}

void RenderMenuList(int msx,int msy,MGLDraw *mgl)
{
	mgl->FillBox(mgl->GetWidth()-55-1, mgl->GetHeight()-80-16*visMenus.size(), mgl->GetWidth()-1, mgl->GetHeight()-80, 6);
	mgl->Box(mgl->GetWidth()-55-1, mgl->GetHeight()-80-16*visMenus.size(), mgl->GetWidth()-1, mgl->GetHeight()-80, 31);
	for (int i=0; i<int(visMenus.size()); i++)
	{
		if (PointInRect(msx, msy, mgl->GetWidth()-55-1, mgl->GetHeight()-80-16*visMenus.size()+i*16, mgl->GetWidth()-1, mgl->GetHeight()-80-16*visMenus.size()+i*16+15))
			mgl->FillBox(mgl->GetWidth()-55, mgl->GetHeight()-80+1-16*visMenus.size()+i*16, mgl->GetWidth()-2, mgl->GetHeight()-80-16*visMenus.size()+i*16+15, 8+32*1);
		CenterPrint(mgl->GetWidth()-1-55+55/2, mgl->GetHeight()-80-16*visMenus.size()+2+i*16, menuName[visMenus[i]], 0, 1);
	}
}

void ToolRender(int msx,int msy,MGLDraw *mgl)
{
	// the base portion of the tool menu
	mgl->FillBox(mgl->GetWidth()-260, mgl->GetHeight()-80, mgl->GetWidth()-1, mgl->GetHeight()-1, 6);
	mgl->Box(mgl->GetWidth()-260, mgl->GetHeight()-80, mgl->GetWidth()-1, mgl->GetHeight()-1, 31);

	if(doing==TD_PICKTOOL)
	{
		RenderToolList(msx, msy, mgl);
	}
	else
	{
		// make the tab for the tool name
		if(PointInRect(msx, msy, mgl->GetWidth()-260, mgl->GetHeight()-80-14, mgl->GetWidth()-260+55, mgl->GetHeight()-80))
			mgl->FillBox(mgl->GetWidth()-260, mgl->GetHeight()-80-14, mgl->GetWidth()-260+55, mgl->GetHeight()-80, 8+32*1);
		else
			mgl->FillBox(mgl->GetWidth()-260, mgl->GetHeight()-80-14, mgl->GetWidth()-260+55, mgl->GetHeight()-80, 6);
		mgl->Box(mgl->GetWidth()-260, mgl->GetHeight()-80-14, mgl->GetWidth()-260+55, mgl->GetHeight()-80, 31);
		CenterPrint(mgl->GetWidth()-260+55/2, mgl->GetHeight()-91, toolName[whichTool], 0, 1);
	}

	if(doing==TD_PICKMENU)
	{
		RenderMenuList(msx,msy,mgl);
	}
	else
	{
		if(PointInRect(msx, msy, mgl->GetWidth()-1-55, mgl->GetHeight()-80-14, mgl->GetWidth()-1, mgl->GetHeight()-80))
			mgl->FillBox(mgl->GetWidth()-1-55, mgl->GetHeight()-80-14, mgl->GetWidth()-1, mgl->GetHeight()-80, 8+32*1);
		else
			mgl->FillBox(mgl->GetWidth()-1-55, mgl->GetHeight()-80-14, mgl->GetWidth()-1, mgl->GetHeight()-80, 6);
		mgl->Box(mgl->GetWidth()-1-55, mgl->GetHeight()-80-14, mgl->GetWidth()-1, mgl->GetHeight()-80, 31);
		CenterPrint(mgl->GetWidth()-55-1+55/2, mgl->GetHeight()-91, "Menus", 0, 1);
	}

	curTool->Render(msx, msy);
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
