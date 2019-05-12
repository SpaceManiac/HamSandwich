#ifndef TOOL_SPECIAL_H
#define TOOL_SPECIAL_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

class SpecialTool : public Tool
{
	public:
		SpecialTool(void);
		~SpecialTool(void);

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

		int lastX,lastY;	// last place plopped
};

#endif
