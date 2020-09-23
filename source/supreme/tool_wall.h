#ifndef TOOL_WALL_H
#define TOOL_WALL_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

class WallTool : public Tool
{
	public:
		WallTool(void);
		~WallTool(void);

		void Update(int msx,int msy);
		void Render(int msx,int msy);
		void StartPlop(void);
		void Plop(void);
		void SetInk(void);
		void ShowTarget(void);
		void SuckUp(int x,int y);
		void StartErase(void);
		void Erase(void);
		void BrushUp(void);
	private:
		void PlopOne(int x,int y);
		void PlopFill(int x,int y,int floor,int wall);
		void StopOverflow(void);

		word tile[4][2];
		byte active;	// which of the four is active
		byte target;	// which of the tiles is being replaced?
		byte brush;		// brush size
		byte plopMode;	// how it plops when clicked
		byte lastPlop;	// for "Cycle" plop mode
		int lastX,lastY;	// last place plopped
		int pickingTile;	// which of the four tiles you are picking for
		byte pickWallRoof;	// whether you're picking the wall or roof of that tile
		int tilesCleared;
};

#endif
