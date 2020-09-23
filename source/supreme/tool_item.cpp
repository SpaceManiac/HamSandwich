#include "tool_item.h"
#include "dialogbits.h"
#include "editor.h"
#include "items.h"
#include "jamulspr.h"
#include "itemedit.h"

ItemTool::ItemTool(void)
{
	active=0;
	brush=0;
	plopMode=PLOP_NORMAL;
	item[0]=ITM_HAMMERUP;
	item[1]=ITM_PANTS;
	item[2]=ITM_BRAIN;
	item[3]=ITM_CANDLE;

	lastX=-1;
	lastY=-1;
}

ItemTool::~ItemTool(void)
{
}

void ItemTool::Update(int msx,int msy)
{
	int i;

	if(msx<380 || msy<400 || msx>639 || msy>479)
		return;

	if(GetDisplayMGL()->MouseTap())
	{
		for(i=0;i<4;i++)
			if(PointInRect(msx,msy,
			   496+i*(TILE_WIDTH+4)-2,424-2,
			   496+i*(TILE_WIDTH+4)+TILE_WIDTH+1,424+TILE_HEIGHT*2+1))
			{
				active=i;
				if(GetDisplayMGL()->RMouseDown())
				{
					item[i]=256;
				}

				MakeNormalSound(SND_MENUCLICK);
			}
		if(PointInRect(msx,msy,382,462,382+30,462+15))
		{
			plopMode++;
			if(plopMode>=PLOP_MODES)
				plopMode=0;
		}
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		{
			brush++;
			if(brush>13)
				brush=0;
		}
	}

	if(GetDisplayMGL()->RMouseTap())
	{
		for(i=0;i<4;i++)
		{
			if(PointInRect(msx,msy,
				           496+i*(TILE_WIDTH+4)-2,424-2,
						   496+i*(TILE_WIDTH+4)+TILE_WIDTH+1,424+TILE_HEIGHT*2+1))
			{
				pickingItem=i;
				active=i;
				ToolPickInk();
				ItemEdit_Init(EDITMODE_EDIT,EditorGetWorld(),1);
				SetEditMode(EDITMODE_ITEM);
				MakeNormalSound(SND_MENUCLICK);
			}
		}
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
	}
}

void ItemTool::Render(int msx,int msy)
{
	int i;
	int minusBrush,plusBrush;

	char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle","FillTouch","FillAll","CycleAll"};
	if (plopMode==PLOP_CUST1)
	plopMode=0;

	for(i=0;i<4;i++)
	{
		SetSpriteConstraints(496+i*(TILE_WIDTH+4)-1,424-1,
			496+i*(TILE_WIDTH+4)+TILE_WIDTH,424+TILE_HEIGHT*2);

		if(item[i]<256)
			InstaRenderItem(496+i*(TILE_WIDTH+4)+2+TILE_WIDTH/2,424+TILE_HEIGHT+6,(byte)item[i],0,GetDisplayMGL());
		else
		{
			Print(496+i*(TILE_WIDTH+4)+2,424+2,"N",0,1);
			Print(496+i*(TILE_WIDTH+4)+9,424+13,"O",0,1);
			Print(496+i*(TILE_WIDTH+4)+17,424+25,"N",0,1);
			Print(496+i*(TILE_WIDTH+4)+24,424+36,"E",0,1);
		}

		DrawBox(496+i*(TILE_WIDTH+4)-2,424-2,
				496+i*(TILE_WIDTH+4)+TILE_WIDTH+1,424+TILE_HEIGHT*2+1,16);
	}
	SetSpriteConstraints(0,0,639,479);

	// highlight the active one
	i=active;
	DrawBox(496+i*(TILE_WIDTH+4)-3,424-3,
			496+i*(TILE_WIDTH+4)+TILE_WIDTH+2,424+TILE_HEIGHT*2+2,16);
	DrawBox(496+i*(TILE_WIDTH+4)-2,424-2,
			496+i*(TILE_WIDTH+4)+TILE_WIDTH+1,424+TILE_HEIGHT*2+1,31);
	DrawBox(496+i*(TILE_WIDTH+4)-1,424-1,
			496+i*(TILE_WIDTH+4)+TILE_WIDTH,424+TILE_HEIGHT*2,16);

	// plop mode
	RenderButtonImage(msx,msy,382,462,30,15,"Plop");
	Print(416,464,plopText[plopMode],0,1);

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

void ItemTool::SetInk(void)
{
	if(EditorGetLastPick()!=-1)
		item[pickingItem]=EditorGetLastPick();
}

void ItemTool::StartPlop(void)
{
	lastX=-1;
	lastY=-1;
	Plop();
}

void ItemTool::PlopOne(int x,int y)
{
	Map *m;
	int i;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height && m->map[x+y*m->width].select && item[active]<256)//		m->map[x+y*m->width].wall==0)
	{
		m->map[x+y*m->width].item=(byte)item[active];
		if(GetItem(item[active])->flags&IF_SOLID)
		{
			for(i=0;i<MAX_MAPMONS;i++)
				if((m->badguy[i].type) && (m->badguy[i].x==x) && (m->badguy[i].y==y))
				{
					// delete a guy if he's here - this item is solid
					m->badguy[i].type=0;
				}
		}
	}

	if(plopMode==PLOP_RANDOM)
		active=(byte)Random(4);
	if(plopMode==PLOP_CYCLE)
	{
		active++;
		if(active>3)
			active=0;
	}
}

void ItemTool::Plop(void)
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
		if(plopMode==PLOP_RANDOM || plopMode==PLOP_RANDOM2)
		{
			active=(byte)Random(4);
		}
		if(plopMode==PLOP_CYCLE2)
		{
			active++;
			if(active>3)
				active=0;
		}
		AddMapGuys(m);
	}
}

void ItemTool::ShowTarget(void)
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

void ItemTool::SuckUp(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height)
	{
		if(m->map[x+y*m->width].item)
			item[active]=m->map[x+y*m->width].item;
	}
}

void ItemTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void ItemTool::Erase(void)
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
			{
				if(i>=0 && j>=0 && i<m->width && j<m->height && m->map[i+j*m->width].select)
				{
					m->map[i+j*m->width].item=0;
				}
			}

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void ItemTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}

void ItemTool::ItemWasDeleted(int i)
{
	int n;

	for(n=0;n<4;n++)
	{
		if(item[n]==i)
			item[n]=ITM_HAMMERUP;
		if(item[n]>i && item[n]!=256)
			item[n]--;
	}
}
