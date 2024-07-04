#ifndef TOOL_FLOOR_H
#define TOOL_FLOOR_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

class FloorTool : public Tool
{
	public:
		FloorTool(void);
		~FloorTool(void);

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

		static const int NUM_TILES = 8;
		static const int ROW_TILES = 4;
		word tile[NUM_TILES];
		byte active;	// which of the tiles is active
		byte brush;		// brush size
		byte plopMode;	// how it plops when clicked
		int lastX,lastY;	// last place plopped
		int pickingTile;	// which of the four tiles you are picking for
};

#endif
