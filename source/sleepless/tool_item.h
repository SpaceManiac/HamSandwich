#ifndef TOOL_ITEM_H
#define TOOL_ITEM_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

class ItemTool : public Tool
{
	public:
		ItemTool(void);
		~ItemTool(void);

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
		void ItemWasDeleted(int i);
	private:
		void PlopOne(int x,int y);

		word item[4];
		byte active;	// which of the four is active
		byte brush;		// brush size
		byte plopMode;	// how it plops when clicked
		int lastX,lastY;	// last place plopped
		int pickingItem;	// which of the four items you are picking for
};

#endif
