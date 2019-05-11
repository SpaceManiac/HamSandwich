#include "tool_eraser.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

EraserTool::EraserTool(void)
{
	brush=0;
	lastX=-1;
	lastY=-1;
	eraseFlag=EF_ITEM|EF_BADGUY|EF_SPECIAL;
}

EraserTool::~EraserTool(void)
{
}

void EraserTool::Update(int msx,int msy)
{
	if(msx<380 || msy<400 || msx>639 || msy>479)
		return;

	if(GetDisplayMGL()->MouseTap())
	{
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		{
			brush++;
			if(brush>13)
				brush=0;
		}
		if(PointInRect(msx,msy,520,402,520+100,402+14))
			eraseFlag^=EF_ITEM;
		if(PointInRect(msx,msy,520,417,520+100,417+14))
			eraseFlag^=EF_BADGUY;
		if(PointInRect(msx,msy,520,432,520+100,432+14))
			eraseFlag^=EF_SPECIAL;
		if(PointInRect(msx,msy,520,447,520+100,447+14))
			eraseFlag^=EF_WALL;
		if(PointInRect(msx,msy,520,462,520+100,462+14))
			eraseFlag^=EF_LIGHT;
	}

	if(GetDisplayMGL()->RMouseTap())
	{
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
	}
}

void EraserTool::Render(int msx,int msy)
{
	int minusBrush,plusBrush;


	if(PointInRect(msx,msy,520,402,520+100,402+14))
		RenderCheckbox(1,520,402,CHECK_ON*((eraseFlag&EF_ITEM)!=0),"Items",GetDisplayMGL());
	else
		RenderCheckbox(0,520,402,CHECK_ON*((eraseFlag&EF_ITEM)!=0),"Items",GetDisplayMGL());

	if(PointInRect(msx,msy,520,417,520+100,417+14))
		RenderCheckbox(1,520,417,CHECK_ON*((eraseFlag&EF_BADGUY)!=0),"Badguys",GetDisplayMGL());
	else
		RenderCheckbox(0,520,417,CHECK_ON*((eraseFlag&EF_BADGUY)!=0),"Badguys",GetDisplayMGL());

	if(PointInRect(msx,msy,520,432,520+100,432+14))
		RenderCheckbox(1,520,432,CHECK_ON*((eraseFlag&EF_SPECIAL)!=0),"Specials",GetDisplayMGL());
	else
		RenderCheckbox(0,520,432,CHECK_ON*((eraseFlag&EF_SPECIAL)!=0),"Specials",GetDisplayMGL());

	if(PointInRect(msx,msy,520,447,520+100,447+14))
		RenderCheckbox(1,520,447,CHECK_ON*((eraseFlag&EF_WALL)!=0),"Walls",GetDisplayMGL());
	else
		RenderCheckbox(0,520,447,CHECK_ON*((eraseFlag&EF_WALL)!=0),"Walls",GetDisplayMGL());

	if(PointInRect(msx,msy,520,462,520+100,462+14))
		RenderCheckbox(1,520,462,CHECK_ON*((eraseFlag&EF_LIGHT)!=0),"Light",GetDisplayMGL());
	else
		RenderCheckbox(0,520,462,CHECK_ON*((eraseFlag&EF_LIGHT)!=0),"Light",GetDisplayMGL());

	// brush size
	minusBrush=brush;
	plusBrush=brush+1;

	if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		DrawFillBox(397-15,442-15,397+16,442+16,8+32*1);
	DrawBox(397-15,442-15,397+16,442+16,31);
	DrawFillBox(397-(minusBrush),
				442-(minusBrush),
				397+(plusBrush),
				442+(plusBrush),24);
	Print(397-15,442-26,"Brush",0,1);
}

void EraserTool::SetInk(void)
{
}

void EraserTool::StartPlop(void)
{
	lastX=-1;
	lastY=-1;
	Plop();
}

void EraserTool::PlopOne(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height && m->map[x+y*m->width].select)
	{
		if(eraseFlag&EF_LIGHT)
			m->map[x+y*m->width].light=0;
		if(eraseFlag&EF_WALL)
			m->map[x+y*m->width].wall=0;
		if(eraseFlag&EF_ITEM)
			m->map[x+y*m->width].item=0;
		if(eraseFlag&EF_BADGUY)
		{
			int i;

			for(i=0;i<MAX_MAPMONS;i++)
				if((m->badguy[i].type) && (m->badguy[i].x==x) && (m->badguy[i].y==y))
					m->badguy[i].type=0;
		}
		if(eraseFlag&EF_SPECIAL)
		{
			int i;

			for(i=0;i<MAX_SPECIAL;i++)
			{
				if(m->special[i].x==x && m->special[i].y==y)
				{
					m->special[i].x=255;
				}
			}
		}
	}
}

void EraserTool::Plop(void)
{
	Map *m;
	int x,y;
	int i,j,minusBrush,plusBrush;

	EditorGetTileXY(&x,&y);
	m=EditorGetMap();

	if(x!=lastX || y!=lastY)
	{
		minusBrush=brush/2;
		plusBrush=(brush+1)/2;
		for(j=y-minusBrush;j<=y+plusBrush;j++)
			for(i=x-minusBrush;i<=x+plusBrush;i++)
				PlopOne(i,j);

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
		AddMapGuys(m);
	}
}

void EraserTool::ShowTarget(void)
{
	int x1,x2,y1,y2,cx,cy;
	static byte col=0;
	int tileX,tileY;
	int tileX2,tileY2,minusBrush,plusBrush;

	col=255-col;
	GetCamera(&cx,&cy);

	EditorGetTileXY(&tileX,&tileY);

	minusBrush=brush/2;
	plusBrush=(brush+1)/2;

	tileX2=tileX+plusBrush;
	tileY2=tileY+plusBrush;

	tileX-=minusBrush;
	tileY-=minusBrush;

	x1=tileX*TILE_WIDTH-(cx-320);
	y1=tileY*TILE_HEIGHT-(cy-240);

	x2=tileX2*TILE_WIDTH-(cx-320)+TILE_WIDTH-1;
	y2=tileY2*TILE_HEIGHT-(cy-240)+TILE_HEIGHT-1;

	DrawBox(x1,y1,x2,y1,col);
	DrawBox(x1,y2,x2,y2,col);
	DrawBox(x1,y1,x1,y2,col);
	DrawBox(x2,y1,x2,y2,col);
}

void EraserTool::SuckUp(int x,int y)
{
}

void EraserTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}

void EraserTool::StartErase(void)
{
	StartPlop();
}

void EraserTool::Erase(void)
{
	Plop();
}
