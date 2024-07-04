#ifndef TOOL_H
#define TOOL_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"

// the different tools
enum : byte
{
	TOOL_FLOOR,
	TOOL_WALL,
	TOOL_ITEM,
	TOOL_BADGUY,
	TOOL_LIGHT,
	TOOL_SPECIAL,
	TOOL_SELECT,
	TOOL_ERASER,

	NUM_TOOLS
};

enum : byte
{
	MENU_FILE,
	MENU_WORLD,
	MENU_TEST,
	MENU_LEVEL,
	MENU_DISPLAY,
	MENU_TILES,
	MENU_ITEMS,
	MENU_SOUND,
	MENU_JSPEDIT,
	MENU_EXIT,

	NUM_MENUS
};

// ways to plop
enum : byte
{
	PLOP_NORMAL,
	PLOP_RANDOM,
	PLOP_CYCLE,
	PLOP_RANDOM2,
	PLOP_CYCLE2,

	PLOP_MODES
};

// Tool Doings (what is the tool doing right now)
enum : byte
{
	TD_PICKTOOL,  // selecting from the pop up menu of tools
	TD_PLOPPING,  // cursor is held, plopping down tool
	TD_USING,     // nothing special going on
	TD_PICKINK,   // selecting an 'ink' for the tool
	TD_PICKMENU,  // picking a menu from the list
	TD_ERASING,   // rightclick is held, erasing
};

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

TASK(void) ToolUpdate(int msx,int msy,byte editMenu,MGLDraw *mgl);
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
