#ifndef TOOL_H
#define TOOL_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"

// the different tools
#define TOOL_FLOOR  0
#define TOOL_WALL	1
#define TOOL_ITEM	2
#define TOOL_BADGUY 3
#define TOOL_LIGHT	4
#define TOOL_SPECIAL 5
#define TOOL_SELECT 6
#define TOOL_ERASER 7
#define NUM_TOOLS	8

#define MENU_FILE	0
#define MENU_WORLD  1
#define MENU_TEST	2
#define MENU_LEVEL	3
#define MENU_DISPLAY 4
#define MENU_TILES	5
#define MENU_ITEMS	6
#define MENU_SOUND  7
#define MENU_EXIT	8
#define NUM_MENUS	9

// ways to plop
#define PLOP_NORMAL		0
#define PLOP_RANDOM		1
#define PLOP_CYCLE		2
#define PLOP_RANDOM2	3
#define PLOP_CYCLE2		4
#define PLOP_CUST1		5
#define PLOP_CUST2		6
#define PLOP_CUST3		7
#define PLOP_MODES		8

// Tool Doings (what is the tool doing right now?)
#define TD_PICKTOOL		0	// selecting from the pop up menu of tools
#define TD_PLOPPING		1	// cursor is held, plopping down tool
#define TD_USING		2	// nothing special going on
#define TD_PICKINK		3	// selecting an 'ink' for the tool
#define TD_PICKMENU		4	// picking a menu from the list
#define TD_ERASING		5	// rightclick is held, erasing

class Tool
{
	public:
		virtual ~Tool(); // warnings otherwise
		virtual void Update(int msx,int msy);
		virtual void Render(int msx,int msy);
		virtual void StartPlop(void);
		virtual void Plop(void);
		virtual void SetInk(void);
		virtual void ShowTarget(void);
		virtual void SuckUp(int x,int y);
		virtual void StartErase(void);
		virtual void Erase(void);
		virtual void BrushUp(void);
};

void ToolInit(void);
void ToolExit(void);

void ToolUpdate(int msx,int msy,byte editMenu,MGLDraw *mgl);
void ToolRender(int msx,int msy,MGLDraw *mgl);
void ToolPickInk(void);
void ToolShowTarget(void);
char *ToolGetFilename(void);
void ToolSetFilename(void);
void ToolSuckUp(int x,int y);
void ToolGetHelp(void);
byte WhichTool(void);
byte ToolDoing(void);
void ToolBrushUp(void);
void PickTool(byte w);
void ToolItemWasDeleted(int i);

#endif
