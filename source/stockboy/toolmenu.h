#ifndef TOOLMENU_H
#define TOOLMENU_H

#include "editor.h"

// tool types
#define TOOL_FLOOR	0
#define TOOL_ITEM	1
#define TOOL_CRITTER 2

#define TOOL_TYPES	3

typedef class ToolMenu
{
	public:
		ToolMenu(MGLDraw *mymgl);
		~ToolMenu(void);

		void Render(int msx,int msy);
		void RenderFloor(void);
		void RenderItem(int x,int y);
		void RenderCritter(void);

		byte Click(int msx,int msy,byte btn);
		void DraggedByMouse(int msx,int msy);
		void Plop(mapTile_t *m,Map *map);

		byte PtInLeftTri(int x,int y,int tx,int ty,int tx2,int ty2);
		byte PtInRightTri(int x,int y,int tx,int ty,int tx2,int ty2);

		void ChgCurTool(char chg);
		void SwitchTool(byte newTool);
		byte GetItemTool(void);
		void NextTool(void);

		void Suck(mapTile_t *m);	// make the highlighted thing the current tool

		byte color;
		byte toolType;
		byte curTool;
		byte facing;
		byte minmax;
		int x,y;
		int relmsx,relmsy;
		MGLDraw *mgl;

		// remember for when switching
		byte lastTool[TOOL_TYPES];
		byte lastColor[TOOL_TYPES];
		byte lastFacing[TOOL_TYPES];
} ToolMenu;

#endif