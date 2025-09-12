#include "tool_special.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"
#include "specialedit.h"
#include "special.h"

SpecialTool::SpecialTool(void)
{
	lastX=-1;
	lastY=-1;
}

SpecialTool::~SpecialTool(void)
{
}

void SpecialTool::Update(int msx,int msy)
{
	if(msx<380 || msy<400 || msx>639 || msy>479)
		return;

	if(GetDisplayMGL()->MouseTap())
	{
	}

	if(GetDisplayMGL()->RMouseTap())
	{
	}
}

void SpecialTool::Render(int msx,int msy)
{

}

void SpecialTool::SetInk(void)
{
}

void SpecialTool::StartPlop(void)
{
	EditorGetTileXY(&lastX, &lastY);
	Map *m = EditorGetMap();

	if(lastX<0 || lastY<0 || lastX>=m->width || lastY>=m->height)
		return;

	int s=GetSpecial((byte)lastX,(byte)lastY);
	if(s==-1)
	{
		s=NewSpecial((byte)lastX,(byte)lastY);
		if(s==-1)
		{
			MakeNormalSound(SND_BOMBBOOM);
			return;
		}
	}

	SpecialEdit_Init(s);
	SetEditMode(EDITMODE_SPECIAL);
}

void SpecialTool::PlopOne(int x,int y)
{
}

void SpecialTool::Plop(void)
{
}

void SpecialTool::ShowTarget(void)
{
	static byte col=0;
	col=255-col;

	auto [cx, cy] = GetCamera();

	auto [tileX, tileY] = EditorGetTileXY();

	int x1=tileX*TILE_WIDTH-(cx-GetDisplayMGL()->GetWidth()/2);
	int y1=tileY*TILE_HEIGHT-(cy-GetDisplayMGL()->GetHeight()/2);

	int x2=x1+TILE_WIDTH-1;
	int y2=y1+TILE_HEIGHT-1;

	DrawBox(x1,y1,x2,y2,col);
}

void SpecialTool::SuckUp(int x,int y)
{
}

void SpecialTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void SpecialTool::Erase(void)
{
	auto [x, y] = EditorGetTileXY();
	Map *m = EditorGetMap();

	if(x!=lastX || y!=lastY)
	{
		if (mapTile_t *target = m->TryGetTile(x, y); target && target->select)
		{
			DeleteSpecial(GetSpecial((byte)x,(byte)y));
		}

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void SpecialTool::BrushUp(void)
{
}
