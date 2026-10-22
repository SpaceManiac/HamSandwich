#include "toolmenu.h"
#include "floor.h"
#include "profile.h"

#define NUM_TOOL_ITEMS	17

byte itemList[]={ITM_BARREL,ITM_CRATE,ITM_BALL,ITM_BOMB,
				 ITM_DOOR,ITM_OPENDOOR,ITM_SWITCH,ITM_SWITCH2,
				 ITM_CRANK,ITM_DETONATE,ITM_SHIELD,ITM_ACID,
				 ITM_LASER,ITM_KEY,ITM_ICEBERG,ITM_GOODIE,ITM_PENCIL};

ToolMenu::ToolMenu(MGLDraw *mymgl)
{
	int i;

	x=100;
	y=50;
	mgl=mymgl;
	minmax=1;
	color=0;
	toolType=TOOL_FLOOR;
	curTool=FLR_OPEN;
	facing=0;

	for(i=0;i<TOOL_TYPES;i++)
	{
		lastTool[i]=0;
		lastColor[i]=0;
		lastFacing[i]=0;
	}
}

ToolMenu::~ToolMenu(void)
{
}

void ToolMenu::Render(int msx,int msy)
{
	msx-=x;
	msy-=y;

	if(minmax==0)	// minimized
	{
		editspr->GetSprite(0)->Draw(x,y,mgl);
		// min/max
		if(PointInRect(msx,msy,0,0,18,10))
		{
			editspr->GetSprite(12)->Draw(x,y,mgl);
		}
		// mover
		if(PointInRect(msx,msy,23,-1,39,15))
		{
			editspr->GetSprite(11)->Draw(x,y,mgl);
		}

	}
	else
	{
		// draw the menu
		editspr->GetSprite(2)->Draw(x,y,mgl);
		// draw the highlighted color
		editspr->GetSprite(9)->Draw(x+color*10,y,mgl);

		// min/max
		if(PointInRect(msx,msy,0,0,18,10))
		{
			editspr->GetSprite(10)->Draw(x,y,mgl);
		}
		// mover
		if(PointInRect(msx,msy,23,-1,39,15))
		{
			editspr->GetSprite(11)->Draw(x,y,mgl);
		}

		// prev tool
		if(PtInLeftTri(msx,msy,9,30,21,56))
		{
			editspr->GetSprite(13)->Draw(x,y,mgl);
		}
		// next tool
		if(PtInRightTri(msx,msy,74,30,86,56))
		{
			editspr->GetSprite(14)->Draw(x,y,mgl);
		}
		// facing down
		if(PointInRect(msx,msy,28,61,41,70))
		{
			editspr->GetSprite(15)->Draw(x,y,mgl);
		}
		// facing up
		if(PointInRect(msx,msy,53,61,66,70))
		{
			editspr->GetSprite(16)->Draw(x,y,mgl);
		}

		// current tool
		switch(toolType)
		{
			case TOOL_FLOOR:
				RenderFloor();
				break;
			case TOOL_ITEM:
				RenderItem(x+48,y+43);
				break;
			case TOOL_CRITTER:
				RenderCritter();
				break;
		}
	}
}

byte ToolMenu::PtInLeftTri(int x,int y,int tx,int ty,int tx2,int ty2)
{
	if(x<tx || x>tx2 || y<ty || y>ty2)
		return 0;

	ty+=(tx2-x);
	ty2-=(tx2-x);
	
	if(y>=ty && y<=ty2)
		return 1;

	return 0;
}

byte ToolMenu::PtInRightTri(int x,int y,int tx,int ty,int tx2,int ty2)
{
	if(x<tx || x>tx2 || y<ty || y>ty2)
		return 0;

	ty+=(x-tx);
	ty2-=(x-tx);
	
	if(y>=ty && y<=ty2)
		return 1;

	return 0;
}


// returns whether or not the click occurred in this menu, and hence needs no more processing
byte ToolMenu::Click(int msx,int msy,byte btn)
{
	msx-=x;
	msy-=y;
	
	// min/max
	if(PointInRect(msx,msy,0,0,18,10))
	{
		minmax=1-minmax;
		return 1;
	}
	// mover
	if(PointInRect(msx,msy,23,-1,39,15))
	{
		// set editmode to 'moving tool menu' until button is released
		editMode=EDITMODE_DRAGTOOL;
		relmsx=-msx;
		relmsy=-msy;
		return 1;
	}

	if(minmax==1)	// these buttons require it to be maxed
	{
		// colors
		if(PointInRect(msx,msy,8,16,17+10*7,25))
		{
			// a color was picked
			color=(msx-8)/10;
			return 1;
		}
		// prev tool
		if(PtInLeftTri(msx,msy,9,30,21,56))
		{
			ChgCurTool(-1);
			return 1;
		}
		// next tool
		if(PtInRightTri(msx,msy,74,30,86,56))
		{
			ChgCurTool(1);
			return 1;
		}
		// facing down
		if(PointInRect(msx,msy,28,61,41,70))
		{
			facing--;
			if(facing>3)
				facing=3;
			return 1;
		}
		// facing up
		if(PointInRect(msx,msy,53,61,66,70))
		{
			facing++;
			if(facing>3)
				facing=0;
			return 1;
		}
	}

	if(PointInRect(msx,msy,21,25,74,61))
		return 1;	// has no effect, but absorbs the click

	return 0;
}

void ToolMenu::DraggedByMouse(int msx,int msy)
{
	x=msx+relmsx;
	y=msy+relmsy;
}

void ToolMenu::ChgCurTool(char chg)
{
	switch(toolType)
	{
		case TOOL_FLOOR:
			curTool+=chg;
			if(curTool>=FLR_MAX)
			{
				if(chg<0)
					curTool=FLR_MAX-1;
				else
					curTool=0;
			}
			break;
		case TOOL_ITEM:
			curTool+=chg;
			if(curTool>=NUM_TOOL_ITEMS)
			{
				if(chg>0)
					curTool=0;
				else
					curTool=NUM_TOOL_ITEMS-1;
			}
			break;
		case TOOL_CRITTER:
			curTool+=chg;
			if(curTool>4)
			{
				if(chg>0)
					curTool=0;
				else
					curTool=4;
			}
			break;
	}
}

void ToolMenu::RenderFloor(void)
{
	RenderFloorTile(x+48-16,y+43-10,GetTileNumber(curTool,color,facing),0);
}

void ToolMenu::RenderItem(int x,int y)
{
	if(itemList[curTool]==ITM_GOODIE)
	{
		InstaRenderItem(x,y,itemList[curTool],facing*8,0,mgl);
	}
	else
		InstaRenderItem(x,y,itemList[curTool],color,0,mgl);
}

void ToolMenu::RenderCritter(void)
{
	switch(curTool)
	{
		case 0:
			InstaRenderMonster(x+44,y+50,MONS_LOONY,1,profile.color,profile.bright,mgl);
			break;
		case 1:
			InstaRenderMonster(x+44,y+50,MONS_BLOBBY,facing,color,0,mgl);
			break;
		case 2:
			InstaRenderMonster(x+44,y+50,MONS_BUNNY,facing,color,0,mgl);
			break;
		case 3:
			InstaRenderMonster(x+44,y+50,MONS_INCHY,facing,color,0,mgl);
			break;
		case 4:
			InstaRenderMonster(x+44,y+50,MONS_PIGGY,facing,color,0,mgl);
			break;
	}
}

void ToolMenu::Plop(mapTile_t *m,Map *map)
{
	int i;

	DeleteMonsters();

	switch(toolType)
	{
		case TOOL_FLOOR:
			if(m->item!=ITM_DOOR && m->item!=ITM_OPENDOOR)
				m->floor=GetTileNumber(curTool,color,facing);
			break;
		case TOOL_ITEM:
			if(itemList[curTool]==ITM_GOODIE)
			{
				for(i=0;i<map->width*map->height;i++)
					if(map->map[i].item==ITM_GOODIE)
						map->map[i].item=ITM_NONE;
			}
			m->item=itemList[curTool];
			m->itemInfo=color+facing*8;
			if(m->item==ITM_OPENDOOR)
			{
				m->itemInfo=color+8*6;
				m->floor=GetTileNumber(FLR_DOOR,color,facing);
			}
			else if(m->item==ITM_DOOR)
			{
				m->itemInfo=color;
				m->floor=GetTileNumber(FLR_DOOR,color,facing);
			}
			break;
		case TOOL_CRITTER:
			if(curTool==0)
			{
				map->ClearStarts();
				m->item=ITM_START;
			}
			else
			{
				m->item=ITM_MONS1+curTool-1;
				m->itemInfo=color+facing*8;
			}
			break;
	}

	AddMapGuys(map);
}

void ToolMenu::SwitchTool(byte newTool)
{
	lastTool[toolType]=curTool;
	lastColor[toolType]=color;
	lastFacing[toolType]=facing;

	toolType=newTool;
	curTool=lastTool[toolType];
	color=lastColor[toolType];
	facing=lastFacing[toolType];
}

void ToolMenu::NextTool(void)
{
	byte b;

	b=toolType+1;
	if(b>=TOOL_TYPES)
		b=0;

	SwitchTool(b);
}

byte ToolMenu::GetItemTool(void)
{
	return itemList[curTool];
}

void ToolMenu::Suck(mapTile_t *m)
{
	int i,j,k;

	switch(toolType)
	{
		case TOOL_FLOOR:
			for(i=0;i<FLR_MAX;i++)
				for(j=0;j<8;j++)
					for(k=0;k<4;k++)
						if(m->floor==GetTileNumber(i,j,k))
						{
							curTool=i;
							color=j;
							facing=k;
						}
			break;
		case TOOL_ITEM:
			if(m->item!=ITM_NONE)
			{
				for(i=0;i<NUM_TOOL_ITEMS;i++)
				{
					if(m->item==itemList[i])
					{
						curTool=i;
						facing=m->itemInfo/8;
						color=m->itemInfo%8;
					}
				}
			}
			break;
		case TOOL_CRITTER:
			if(m->item==ITM_START || m->item==ITM_PLAYER)
				curTool=0;
			else if(m->item>=ITM_MONS1 && m->item<=ITM_MONS4)
			{
				curTool=m->item-ITM_MONS1+1;
				facing=m->itemInfo/8;
				color=m->itemInfo%8;
			}
			break;
	}
}