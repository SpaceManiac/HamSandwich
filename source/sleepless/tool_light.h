#ifndef TOOL_LIGHT_H
#define TOOL_LIGHT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"
#include "tool.h"

#define LIGHTPLOP_SOLID	0
#define LIGHTPLOP_FADE	1
#define LIGHTPLOP_TORCH 2
#define LIGHTPLOP_SMOOTH 3
#define LIGHTPLOP_MODES 4

class LightTool : public Tool
{
	public:
		LightTool(void);
		~LightTool(void);

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
		char CalcBright(char b);
		void PlopOne(int centerx,int centery,int x,int y);
		void LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y);
		void LOS(int x,int y,int rx,int ry,int rx2,int ry2);
		byte GetOpaque(int x,int y,mapTile_t *map,int width,int height);
		void SetOpaque(int x,int y,byte opaque,mapTile_t *map,int width,int height);

		char bright;	// brightness
		byte random;	// randomness
		byte lineOfSight;	// toggle line of sight mode

		byte brush;		// brush size
		byte plopMode;	// how it plops when clicked
		byte dragSlider;
		int lastX,lastY;	// last place plopped
};

#endif
