#ifndef TOOL_ERASER_H
#define TOOL_ERASER_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

#define EF_ITEM		1
#define EF_BADGUY	2
#define EF_LIGHT	4
#define EF_WALL		8
#define EF_SPECIAL	16

class EraserTool : public Tool
{
	public:
		EraserTool(void);
		~EraserTool(void);

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

		byte eraseFlag;
		byte brush;		// brush size
		int lastX,lastY;	// last place plopped
};

#endif
