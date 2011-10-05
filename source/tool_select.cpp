#include "tool_select.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

SelectTool::SelectTool(void)
{
	brush=0;
	lastX=-1;
	lastY=-1;
	plopMode=0;
	clearMode=0;
	doing=0;
}

SelectTool::~SelectTool(void)
{
}

void SelectTool::Update(int msx,int msy)
{
	int i;
	Map *m;

	if(doing==1 || doing==2)
	{
		// copying or swapping
		if(GetDisplayMGL()->MouseTap())
		{
			if(PointInRect(msx,msy,576,460,576+60,460+15))
			{
				// cancel
				doing=0;
			}
		}
		return;
	}

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
		if(PointInRect(msx,msy,382,462,382+30,462+15))
		{
			plopMode++;
			if(plopMode>SELPLOP_OFF)
				plopMode=0;
		}
		if(PointInRect(msx,msy,475,462,475+40,462+15))
		{
			EditorSetSelectMode(1-EditorGetSelectMode());
		}
		if(PointInRect(msx,msy,496,403,496+140,403+15))
		{
			m=EditorGetMap();

			for(i=0;i<m->width*m->height;i++)
				m->map[i].select=clearMode;
			clearMode=1-clearMode;
		}
		if(PointInRect(msx,msy,496,422,496+140,422+15))
		{
			// copy selection
			if(!CalcSource())
			{
				// nothing is selected!
				MakeNormalSound(SND_BOMBBOOM);
			}
			else
				doing=1;
		}
		if(PointInRect(msx,msy,496,441,496+140,441+15))
		{
			// swap selection
			if(!CalcSource())
			{
				// no selection!
				MakeNormalSound(SND_BOMBBOOM);
			}
			else
				doing=2;
		}
		if(PointInRect(msx,msy,576,460,576+60,460+15))
		{
			// invert selection
			m=EditorGetMap();

			for(i=0;i<m->width*m->height;i++)
				m->map[i].select=1-m->map[i].select;

			for(i=0;i<m->width*m->height;i++)
				if(m->map[i].select)
				{
					clearMode=0;
					i=0;
					break;
				}
			if(i==m->width*m->height)
				clearMode=1;
		}

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

void SelectTool::Render(int msx,int msy)
{
	int minusBrush,plusBrush;
	char plopText[][12]={"Toggle","Select","Unselect"};
	char showText[][12]={"Outline","Mask"};
	char clearText[][12]={"Select None","Select All"};

	if(doing==1)
		Print(390,410,"Click where you want to copy to!",0,1);
	if(doing==2)
		Print(390,410,"Click where you want to swap with!",0,1);
	if(doing!=0)
	{
		RenderButtonImage(msx,msy,576,460,60,15,"Cancel");
		return;
	}

	// brush size
	minusBrush=brush;
	plusBrush=brush+1;

	// plop mode
	RenderButtonImage(msx,msy,382,462,30,15,"Plop");
	Print(416,464,plopText[plopMode],0,1);

	// display mode
	RenderButtonImage(msx,msy,475,462,40,15,"Show");
	Print(519,464,showText[EditorGetSelectMode()],0,1);

	// clear all button
	RenderButtonImage(msx,msy,496,403,140,15,clearText[clearMode]);
	// copy button
	RenderButtonImage(msx,msy,496,422,140,15,"Copy Selection");
	// swap button
	RenderButtonImage(msx,msy,496,441,140,15,"Swap Selection");
	// invert button
	RenderButtonImage(msx,msy,576,460,60,15,"Invert");

	if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
		DrawFillBox(397-15,442-15,397+16,442+16,8+32*1);
	DrawBox(397-15,442-15,397+16,442+16,31);
	DrawFillBox(397-(minusBrush),
				442-(minusBrush),
				397+(plusBrush),
				442+(plusBrush),24);
	Print(397-15,442-26,"Brush",0,1);
}

void SelectTool::SetInk(void)
{
}

void SelectTool::StartPlop(void)
{
	int x,y;
	Map *m;

	if(doing==1)
	{
		EditorGetTileXY(&x,&y);
		lastX=x;
		lastY=y;
		doing=0;
		if(!CheckOverlap(0))
			CopySelection();
		else
			MakeNormalSound(SND_TURRETBZZT);
		return;
	}
	if(doing==2)
	{
		EditorGetTileXY(&x,&y);
		lastX=x;
		lastY=y;
		doing=0;
		if(!CheckOverlap(1))
			SwapSelection();
		else
			MakeNormalSound(SND_TURRETBZZT);
		return;
	}

	m=EditorGetMap();
	lastX=-1;
	lastY=-1;

	EditorGetTileXY(&x,&y);
	if(x>=0 && y>=0 && x<m->width && y<m->height)
	{
		if(m->map[x+y*m->width].select)
			toggleType=0;
		else
			toggleType=1;
	}
	else
		toggleType=1;

	Plop();
}

void SelectTool::PlopOne(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height)
	{
		if(plopMode==SELPLOP_ON)
			m->map[x+y*m->width].select=1;
		else if(plopMode==SELPLOP_OFF)
			m->map[x+y*m->width].select=0;
		if(plopMode==SELPLOP_TOGGLE)
		{
			if(toggleType)
				m->map[x+y*m->width].select=1;
			else
				m->map[x+y*m->width].select=0;
		}
	}
}

void SelectTool::Plop(void)
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

		for(i=0;i<m->width*m->height;i++)
			if(m->map[i].select)
			{
				clearMode=0;
				i=0;
				break;
			}
		if(i==m->width*m->height)
			clearMode=1;
	}
}

void SelectTool::ShowTarget(void)
{
	int x1,x2,y1,y2,cx,cy;
	static byte col=0;
	int tileX,tileY;
	int tileX2,tileY2,minusBrush,plusBrush;

	col=255-col;

	if(doing==0)
	{
		// render a normal brush
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
	else
	{
		// render the big copy/swap brush
		RenderCopyTarget(col);
	}
}

void SelectTool::SuckUp(int x,int y)
{
}

byte SelectTool::CalcSource(void)
{
	int i;
	Map *m;

	m=EditorGetMap();

	sourceX=0;
	sourceY=0;
	for(i=0;i<m->width*m->height;i++)
	{
		if(m->map[i].select)
			return 1;
		sourceX++;
		if(sourceX==m->width)
		{
			sourceX=0;
			sourceY++;
		}
	}

	return 0;
}

void SelectTool::RenderCopyTarget(byte color)
{
	int i,j;

	int tileX,tileY;
	int tileModX,tileModY;
	int ofsX,ofsY;
	int scrX,scrY;
	int camX,camY;
	mapTile_t *m;
	Map *map;

	map=EditorGetMap();
	GetCamera(&camX,&camY);
	camX-=320;
	camY-=240;

	EditorGetTileXY(&tileModX,&tileModY);
	tileX=(camX/TILE_WIDTH)-1;
	tileY=(camY/TILE_HEIGHT)-1;
	ofsX=camX%TILE_WIDTH;
	ofsY=camY%TILE_HEIGHT;

	scrX=-ofsX-TILE_WIDTH;

	tileModX=sourceX-tileModX;
	tileModY=sourceY-tileModY;

	tileX+=tileModX;
	tileY+=tileModY;
	for(i=tileX;i<tileX+(640/TILE_WIDTH+4);i++)
	{
		scrY=-ofsY-TILE_HEIGHT;
		for(j=tileY;j<tileY+(480/TILE_HEIGHT+6);j++)
		{
			if(i>=0 && i<map->width && j>=0 && j<map->height)
			{
				m=&map->map[i+j*map->width];

				if(m->select)
				{
					if(i==0 || !map->map[i-1+j*map->width].select)
						DrawFillBox(scrX,scrY,scrX,scrY+TILE_HEIGHT-1,color);
					if(i==map->width-1 || !map->map[i+1+j*map->width].select)
						DrawFillBox(scrX+TILE_WIDTH-1,scrY,scrX+TILE_WIDTH-1,scrY+TILE_HEIGHT-1,color);
					if(j==0 || !map->map[i+(j-1)*map->width].select)
						DrawFillBox(scrX,scrY,scrX+TILE_WIDTH-1,scrY,color);
					if(j==map->height-1 || !map->map[i+(j+1)*map->width].select)
						DrawFillBox(scrX,scrY+TILE_HEIGHT-1,scrX+TILE_WIDTH-1,scrY+TILE_HEIGHT-1,color);
				}
			}
			scrY+=TILE_HEIGHT;
		}
		scrX+=TILE_WIDTH;
	}
}

byte SelectTool::CheckOverlap(byte swap)
{
	int i,j;
	Map *m;
	int ofsX,ofsY;

	m=EditorGetMap();
	ofsX=lastX-sourceX;
	ofsY=lastY-sourceY;

	for(i=0;i<m->width;i++)
	{
		for(j=0;j<m->height;j++)
		{
			if(m->map[i+j*m->width].select)
			{
				// this tile is in the selection, so we need to see if:
				// A> if swapping, is its destination still on the map at all?
				// B> is its destination also selected?  (It's not allowed to be)
				if(i+ofsX<0 || i+ofsX>=m->width || j+ofsY<0 || j+ofsY>=m->height)
				{
					if(swap)	// not valid
						return 1;
					else		// is valid, and can't fail the next test, so...
						continue;	// continue on!
				}

				if(m->map[i+ofsX+(j+ofsY)*m->width].select)
					return 1;	// overlaps!
			}
		}
	}
	return 0;	// no overlap encountered!
}

void SelectTool::CopySelection(void)
{
	int i,j,k,z,z2;
	Map *m;
	mapTile_t *src,*dst;
	int ofsX,ofsY;

	m=EditorGetMap();
	ofsX=lastX-sourceX;
	ofsY=lastY-sourceY;

	for(i=0;i<m->width;i++)
	{
		for(j=0;j<m->height;j++)
		{
			if(m->map[i+j*m->width].select==1)
			{
				if(i+ofsX<0 || i+ofsX>=m->width || j+ofsY<0 || j+ofsY>=m->height)
				{
					// can't copy to there!
				}
				else
				{
					src=&m->map[i+j*m->width];
					dst=&m->map[i+ofsX+(j+ofsY)*m->width];

					*dst=*src;
					dst->select=2;
					src->select=0;

					for(k=0;k<MAX_MAPMONS;k++)
					{
						if(m->badguy[k].type && m->badguy[k].x==i+ofsX && m->badguy[k].y==j+ofsY)
							m->badguy[k].type=MONS_NONE;	// remove any monsters previously in the destination area
					}
					for(k=0;k<MAX_MAPMONS;k++)
					{
						if(m->badguy[k].type && m->badguy[k].x==i && m->badguy[k].y==j &&
							m->badguy[k].type!=MONS_BOUAPHA)
						{
							for(z=0;z<MAX_MAPMONS;z++)
							{
								if(m->badguy[z].type==0)
								{
									m->badguy[z]=m->badguy[k];
									m->badguy[z].x=i+ofsX;
									m->badguy[z].y=j+ofsY;
									break;
								}
							}
							break;
						}
					}

					// also copy specials!
					for(k=0;k<MAX_SPECIAL;k++)
					{
						if(m->special[k].x==i && m->special[k].y==j)
						{
							z=NewSpecial(i+ofsX,j+ofsY);
							if(z!=-1)
							{
								m->special[z]=m->special[k];
								m->special[z].x=i+ofsX;
								m->special[z].y=j+ofsY;
								AdjustSpecialCoords(&m->special[z],ofsX,ofsY);
								AdjustSpecialEffectCoords(&m->special[z],ofsX,ofsY);

								for(z2=0;z2<MAX_SPECIAL;z2++)
								{
									if(z2!=z && m->special[z2].x==m->special[z].x && m->special[z2].y==m->special[z].y)
										m->special[z2].x=255;	// delete the special being covered
								}
							}
						}
					}
				}
			}
		}
	}
	AddMapGuys(m);

	// now that you've done that, swap the selection box itself over
	for(i=0;i<m->width;i++)
	{
		for(j=0;j<m->height;j++)
		{
			if(m->map[i+j*m->width].select==2)
			{
				m->map[i+j*m->width].select=1;
			}
			else if(m->map[i+j*m->width].select==1)
			{
				m->map[i+j*m->width].select=0;
			}
		}
	}
}

void SelectTool::SwapSelection(void)
{
	int i,j,k;
	Map *m;
	mapTile_t *src,*dst,tmp;
	int ofsX,ofsY;

	m=EditorGetMap();
	ofsX=lastX-sourceX;
	ofsY=lastY-sourceY;

	for(i=0;i<m->width;i++)
	{
		for(j=0;j<m->height;j++)
		{
			if(m->map[i+j*m->width].select==1)
			{
				if(i+ofsX<0 || i+ofsX>=m->width || j+ofsY<0 || j+ofsY>=m->height)
				{
					// can't copy to there!
				}
				else
				{
					src=&m->map[i+j*m->width];
					dst=&m->map[i+ofsX+(j+ofsY)*m->width];

					tmp=*src;
					*src=*dst;
					*dst=tmp;
					dst->select=2;
					src->select=0;	// select the new spot, not the old

					for(k=0;k<MAX_MAPMONS;k++)
					{
						if(m->badguy[k].type && m->badguy[k].x==i && m->badguy[k].y==j)
						{
							m->badguy[k].x=i+ofsX;
							m->badguy[k].y=j+ofsY;
						}
						else if(m->badguy[k].type && m->badguy[k].x==i+ofsX && m->badguy[k].y==j+ofsY)
						{
							m->badguy[k].x=i;
							m->badguy[k].y=j;
						}
					}

					// also swap specials!
					for(k=0;k<MAX_SPECIAL;k++)
					{
						if(m->special[k].x==i && m->special[k].y==j)
						{
							m->special[k].x=i+ofsX;
							m->special[k].y=j+ofsY;
							AdjustSpecialCoords(&m->special[k],ofsX,ofsY);
							AdjustSpecialEffectCoords(&m->special[k],ofsX,ofsY);
						}
						else if(m->special[k].x==i+ofsX && m->special[k].y==j+ofsY)
						{
							m->special[k].x=i;
							m->special[k].y=j;
							AdjustSpecialCoords(&m->special[k],-ofsX,-ofsY);
							AdjustSpecialEffectCoords(&m->special[k],-ofsX,-ofsY);
						}
					}
				}
			}
		}
	}
	AddMapGuys(m);

	// now that you've done that, swap the selection box itself over
	for(i=0;i<m->width;i++)
	{
		for(j=0;j<m->height;j++)
		{
			if(m->map[i+j*m->width].select==2)
			{
				m->map[i+j*m->width].select=1;
			}
			else if(m->map[i+j*m->width].select==1)
			{
				m->map[i+j*m->width].select=0;
			}
		}
	}
}

void SelectTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void SelectTool::Erase(void)
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
					m->map[i+j*m->width].select=0;
				}
			}

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void SelectTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}
