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

	if(pickingItem)
	{
		specificItem=(byte)EditorGetLastPick();
		itemMode=2;
		pickingItem=0;
	}
	if(msx<380 || msy<400 || msx>639 || msy>479)
		return;

	if(GetDisplayMGL()->MouseTap())
	{
		for(i=0;i<4;i++)
			if(PointInRect(msx,msy,496,412+i*16,496+140,412+i*16+14))
			{
				active=i;
				if(GetDisplayMGL()->RMouseDown())
				{
					monster[i]=MONS_NONE;
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
		if(PointInRect(msx,msy,462,442-15,494,442+16))
		{
			itemMode++;
			if(itemMode==3)
				itemMode=0;
		}
	}

	if(GetDisplayMGL()->RMouseTap())
	{
		for(i=0;i<4;i++)
		{
			if(PointInRect(msx,msy,496,412+i*16,496+140,412+i*16+14))
			{
				pickingMonster=i;
				active=i;
				ToolPickInk();
				MonsterEdit_Init(EDITMODE_EDIT,EditorGetWorld());
				SetEditMode(EDITMODE_PICKENEMY);
				MakeNormalSound(SND_MENUCLICK);
			}
		}
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
		if(PointInRect(msx,msy,462,442-15,494,442+16))
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

	char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle"};


	for(i=0;i<4;i++)
	{
		if(monster[i] != MONS_NONE)
		{
			if(active==i)
				RenderButtonImageLit(msx,msy,496,412+i*16,140,14,MonsterName(monster[i]));
			else
				RenderButtonImage(msx,msy,496,412+i*16,140,14,MonsterName(monster[i]));
		}
		else
		{
			if(active==i)
				RenderButtonImageLit(msx,msy,496,412+i*16,140,14,"NONE");
			else
				RenderButtonImage(msx,msy,496,412+i*16,140,14,"NONE");
		}
	}

	// carrying item
	SetSpriteConstraints(462,442-15,494,442+16);

	if(PointInRect(msx,msy,462,442-15,494,442+16))
		DrawFillBox(462,442-15,494,442+16,8+32*1);

	switch(itemMode)
	{
		case 0:
			Print(464,442-13,"N",0,1);
			Print(464+8,442-8,"O",0,1);
			Print(464+16,442-2,"N",0,1);
			Print(464+23,442+4,"E",0,1);
			break;
		case 1:
			InstaRenderItem((494-462)/2+462,442+10,ITM_RANDOM,0,GetDisplayMGL());
			break;
		case 2:
			InstaRenderItem((494-462)/2+462,442+10,specificItem,0,GetDisplayMGL());
			break;
	}

	DrawBox(462,442-15,494,442+16,31);

	Print(462,442-26,"Item",0,1);
	SetSpriteConstraints(0,0,639,479);

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
	Map *m;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height && m->map[x+y*m->width].select && monster[active] != MONS_NONE)
	{
		int i;

		if((!m->map[x+y*m->width].item || !(GetItem(m->map[x+y*m->width].item)->flags&IF_SOLID))
			&& !m->map[x+y*m->width].wall)
		{
			for(i=0;i<MAX_MAPMONS;i++)
				if((m->badguy[i].type) && (m->badguy[i].x==x) && (m->badguy[i].y==y))
				{
					// get rid of anybody already on this space
					m->badguy[i].type=0;
				}

			// only one bouapha allowed
			if(monster[active]==MONS_BOUAPHA)
			{
				for(i=0;i<MAX_MAPMONS;i++)
					if(m->badguy[i].type==MONS_BOUAPHA)
					{
						m->badguy[i].type=0;
					}
			}

			// now add the new guy
			for(i=0;i<MAX_MAPMONS;i++)
				if(m->badguy[i].type==0)
				{
					m->badguy[i].type = monster[active];
					m->badguy[i].x=x;
					m->badguy[i].y=y;
					switch(itemMode)
					{
						case 0:
							m->badguy[i].item=ITM_NONE;
							break;
						case 1:
							m->badguy[i].item=ITM_RANDOM;
							break;
						case 2:
							m->badguy[i].item=specificItem;
							break;
					}
					if(m->badguy[i].type==MONS_BOUAPHA)
						m->badguy[i].item=ITM_NONE;	// bouapha can't drop items
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

void MonsterTool::ShowTarget(void)
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

void MonsterTool::SuckUp(int x,int y)
{
	Map *m;
	int i;

	m=EditorGetMap();

	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(m->badguy[i].type && m->badguy[i].x==x && m->badguy[i].y==y)
			monster[active]=m->badguy[i].type;
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
	Map *m;
	int x,y;
	int i,j,k,minusBrush,plusBrush;

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
					for(k=0;k<MAX_MAPMONS;k++)
					{
						if(m->badguy[k].type && m->badguy[k].x==x && m->badguy[k].y==y)
							m->badguy[k].type=0;
					}
				}
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