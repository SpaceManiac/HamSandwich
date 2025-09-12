#include "tool_monster.h"
#include "dialogbits.h"
#include "editor.h"
#include "monster.h"
#include "jamulspr.h"
#include "monsteredit.h"
#include "items.h"
#include "itemedit.h"

MonsterTool::MonsterTool(void)
{
	active=0;
	brush=0;
	plopMode=PLOP_NORMAL;
	monster[0]=MONS_BOUAPHA;
	monster[1]=MONS_BONEHEAD;
	monster[2]=MONS_BIGSPDR;
	monster[3]=MONS_ZOMBIE;

	itemMode=1;
	specificItem=ITM_HAMMERUP;
	pickingItem=0;
	lastX=-1;
	lastY=-1;
}

MonsterTool::~MonsterTool(void)
{
}

void MonsterTool::Update(int msx,int msy)
{
	int i;
	MGLDraw* mgl = GetDisplayMGL();

	if(pickingItem)
	{
		specificItem=(byte)EditorGetLastPick();
		itemMode=2;
		pickingItem=0;
	}
	if(msx<mgl->GetWidth()-260 || msy<mgl->GetHeight()-80 || msx>=mgl->GetWidth() || msy>=mgl->GetHeight())
		return;

	if(mgl->MouseTap())
	{
		for(i=0;i<4;i++)
			if(PointInRect(msx, msy, mgl->GetWidth()-144, mgl->GetHeight()-68+i*16, mgl->GetWidth()-144+140, mgl->GetHeight()-68+i*16+14))
			{
				active=i;
				if(mgl->RMouseDown())
				{
					monster[i]=MONS_NONE;
				}
				MakeNormalSound(SND_MENUCLICK);
			}
		if(PointInRect(msx, msy, mgl->GetWidth()-258, mgl->GetHeight()-18, mgl->GetWidth()-258+30, mgl->GetHeight()-18+15))
		{
			plopMode++;
			if(plopMode>=PLOP_MODES)
				plopMode=0;
		}
		if(PointInRect(msx, msy, mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		{
			brush++;
			if(brush>13)
				brush=0;
		}
		if(PointInRect(msx, msy, mgl->GetWidth()-178, mgl->GetHeight()-38-15, mgl->GetWidth()-178+32, mgl->GetHeight()-38+16))
		{
			itemMode++;
			if(itemMode==3)
				itemMode=0;
		}
	}

	if(mgl->RMouseTap())
	{
		for(i=0;i<4;i++)
		{
			if(PointInRect(msx, msy, mgl->GetWidth()-144, mgl->GetHeight()-68+i*16, mgl->GetWidth()-144+140, mgl->GetHeight()-68+i*16+14))
			{
				pickingMonster=i;
				active=i;
				ToolPickInk();
				MonsterEdit_Init(EDITMODE_EDIT,EditorGetWorld());
				SetEditMode(EDITMODE_PICKENEMY);
				MakeNormalSound(SND_MENUCLICK);
			}
		}
		if(PointInRect(msx, msy, mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
		if(PointInRect(msx, msy, mgl->GetWidth()-178, mgl->GetHeight()-38-15, mgl->GetWidth()-178+32, mgl->GetHeight()-38+16))
		{
			ItemEdit_Init(EDITMODE_EDIT,EditorGetWorld(),1);
			SetEditMode(EDITMODE_ITEM);
			MakeNormalSound(SND_MENUCLICK);
			pickingItem=1;
		}
	}
}

void MonsterTool::Render(int msx,int msy)
{
	int i;
	int minusBrush,plusBrush;
	MGLDraw* mgl = GetDisplayMGL();

	char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle"};

	for(i=0;i<4;i++)
	{
		if(monster[i] != MONS_NONE)
		{
			if(active==i)
				RenderButtonImageLit(msx,msy,mgl->GetWidth()-144,mgl->GetHeight()-68+i*16,140,14,MonsterName(monster[i]));
			else
				RenderButtonImage(msx,msy,mgl->GetWidth()-144,mgl->GetHeight()-68+i*16,140,14,MonsterName(monster[i]));
		}
		else
		{
			if(active==i)
				RenderButtonImageLit(msx,msy,mgl->GetWidth()-144,mgl->GetHeight()-68+i*16,140,14,"NONE");
			else
				RenderButtonImage(msx,msy,mgl->GetWidth()-144,mgl->GetHeight()-68+i*16,140,14,"NONE");
		}
	}

	// carrying item
	SetSpriteConstraints(mgl->GetWidth()-178, mgl->GetHeight()-38-15, mgl->GetWidth()-178+32, mgl->GetHeight()-38+16);

	if(PointInRect(msx, msy, mgl->GetWidth()-178, mgl->GetHeight()-38-15, mgl->GetWidth()-178+32, mgl->GetHeight()-38+16))
		DrawFillBox(mgl->GetWidth()-178, mgl->GetHeight()-38-15, mgl->GetWidth()-178+32, mgl->GetHeight()-38+16, 8+32*1);

	switch(itemMode)
	{
		case 0:
			Print(mgl->GetWidth()-178+2,mgl->GetHeight()-38-13,"N",0,1);
			Print(mgl->GetWidth()-178+2+8,mgl->GetHeight()-38-8,"O",0,1);
			Print(mgl->GetWidth()-178+2+16,mgl->GetHeight()-38-2,"N",0,1);
			Print(mgl->GetWidth()-178+2+23,mgl->GetHeight()-38+4,"E",0,1);
			break;
		case 1:
			InstaRenderItem(mgl->GetWidth()-178+32/2,mgl->GetHeight()-38+10,ITM_RANDOM,0,mgl);
			break;
		case 2:
			InstaRenderItem(mgl->GetWidth()-178+32/2,mgl->GetHeight()-38+10,specificItem,0,mgl);
			break;
	}

	DrawBox(mgl->GetWidth()-178, mgl->GetHeight()-38-15, mgl->GetWidth()-178+32, mgl->GetHeight()-38+16, 31);

	Print(mgl->GetWidth()-178, mgl->GetHeight()-38-26, "Item",0,1);
	ClearSpriteConstraints();

	// plop mode
	RenderButtonImage(msx, msy, mgl->GetWidth()-258, mgl->GetHeight()-18, 30, 15, "Plop");
	Print(mgl->GetWidth()-224, mgl->GetHeight()-16, plopText[plopMode], 0, 1);

	// brush size
	minusBrush=brush;
	plusBrush=brush+1;

	if(PointInRect(msx,msy,mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		DrawFillBox(mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16, 8+32*1);
	DrawBox(mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16, 31);
	DrawFillBox(
		mgl->GetWidth()-243-(minusBrush),
		mgl->GetHeight()-38-(minusBrush),
		mgl->GetWidth()-243+(plusBrush),
		mgl->GetHeight()-38+(plusBrush),
		24
	);
	Print(mgl->GetWidth()-243-15, mgl->GetHeight()-38-26, "Brush", 0, 1);
}

void MonsterTool::SetInk(void)
{
	if(EditorGetLastPick()!=-1)
		monster[pickingMonster]=EditorGetLastPick();
}

void MonsterTool::StartPlop(void)
{
	lastX=-1;
	lastY=-1;
	Plop();
}

void MonsterTool::PlopOne(int x,int y)
{
	Map *m = EditorGetMap();

	if (mapTile_t *target = m->TryGetTile(x, y); target && target->select && monster[active] != MONS_NONE)
	{
		if((!target->item || !(GetItem(target->item)->flags&IF_SOLID))
			&& !target->wall)
		{
			for (mapBadguy_t &guy : m->badguy)
				if(guy.type && guy.x==x && guy.y==y)
				{
					// get rid of anybody already on this space
					guy.type=0;
				}

			// only one bouapha allowed
			if(monster[active]==MONS_BOUAPHA)
			{
				for (mapBadguy_t &guy : m->badguy)
					if(guy.type==MONS_BOUAPHA)
					{
						guy.type=0;
					}
			}

			// now add the new guy
			for (mapBadguy_t &guy : m->badguy)
				if(guy.type==0)
				{
					guy.type = monster[active];
					guy.x=x;
					guy.y=y;
					switch(itemMode)
					{
						case 0:
							guy.item=ITM_NONE;
							break;
						case 1:
							guy.item=ITM_RANDOM;
							break;
						case 2:
							guy.item=specificItem;
							break;
					}
					if(guy.type==MONS_BOUAPHA)
						guy.item=ITM_NONE;	// bouapha can't drop items
					break;
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

void MonsterTool::Plop(void)
{
	auto [x, y] = EditorGetTileXY();
	Map *m = EditorGetMap();

	if(x!=lastX || y!=lastY)
	{
		int minusBrush=brush/2;
		int plusBrush=(brush+1)/2;
		for(int j=y-minusBrush;j<=y+plusBrush;j++)
			for(int i=x-minusBrush;i<=x+plusBrush;i++)
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

void MonsterTool::ShowTarget(void)
{
	static byte col=0;
	col=255-col;

	auto [cx, cy] = GetCamera();
	auto [tileX, tileY] = EditorGetTileXY();

	int minusBrush=brush/2;
	int plusBrush=(brush+1)/2;

	int tileX2=tileX+plusBrush;
	int tileY2=tileY+plusBrush;

	tileX-=minusBrush;
	tileY-=minusBrush;

	int x1=tileX*TILE_WIDTH-(cx-GetDisplayMGL()->GetWidth()/2);
	int y1=tileY*TILE_HEIGHT-(cy-GetDisplayMGL()->GetHeight()/2);

	int x2=tileX2*TILE_WIDTH-(cx-GetDisplayMGL()->GetWidth()/2)+TILE_WIDTH-1;
	int y2=tileY2*TILE_HEIGHT-(cy-GetDisplayMGL()->GetHeight()/2)+TILE_HEIGHT-1;

	DrawBox(x1,y1,x2,y1,col);
	DrawBox(x1,y2,x2,y2,col);
	DrawBox(x1,y1,x1,y2,col);
	DrawBox(x2,y1,x2,y2,col);
}

void MonsterTool::SuckUp(int x,int y)
{
	for (const mapBadguy_t &guy : EditorGetMap()->badguy)
	{
		if(guy.type && guy.x==x && guy.y==y)
			monster[active]=guy.type;
	}
}

void MonsterTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void MonsterTool::Erase(void)
{
	auto [x, y] = EditorGetTileXY();
	Map *m = EditorGetMap();

	if(x!=lastX || y!=lastY)
	{
		int minusBrush=brush/2;
		int plusBrush=(brush+1)/2;
		int minX = x - minusBrush, maxX = x + plusBrush;
		int minY = y - minusBrush, maxY = y + plusBrush;
		for (mapBadguy_t &guy : m->badguy)
		{
			if(guy.type && guy.x >= minX && guy.x <= maxX && guy.y >= minY && guy.y <= maxY)
				guy.type=0;
		}

		AddMapGuys(m);
		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void MonsterTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}

void MonsterTool::ItemWasDeleted(int item)
{
	if(item==specificItem)
		specificItem=ITM_HAMMERUP;
	if(specificItem>item)
		specificItem--;
}
