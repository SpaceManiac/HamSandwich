#ifndef TOOL_SELECT_H
#define TOOL_SELECT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

#define SELPLOP_TOGGLE	0
#define SELPLOP_ON		1
#define SELPLOP_OFF		2

class SelectTool : public Tool
{
	public:
		SelectTool(void);
		~SelectTool(void);

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
		byte CalcSource(void);
		void RenderCopyTarget(byte color);
		byte CheckOverlap(byte swap);
		void CopySelection(void);
		void SwapSelection(void);
		int sourceX,sourceY;
		byte doing;
		byte plopMode;
		byte toggleType;
		byte clearMode;
		byte brush;		// brush size
		int lastX,lastY;	// last place plopped
};

#endif
