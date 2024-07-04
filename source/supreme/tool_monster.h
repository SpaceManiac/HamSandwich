#ifndef TOOL_MONSTER_H
#define TOOL_MONSTER_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

class MonsterTool : public Tool
{
	public:
		MonsterTool(void);
		~MonsterTool(void);

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
		void ItemWasDeleted(int item);
	private:
		void PlopOne(int x,int y);

		dword monster[4];
		byte specificItem;
		byte itemMode;
		byte active;	// which of the four is active
		byte brush;		// brush size
		byte plopMode;	// how it plops when clicked
		int lastX,lastY;	// last place plopped
		int pickingMonster;	// which of the four items you are picking for
		byte pickingItem;	// set to 1 if you are waiting for an item to be picked
};

#endif
