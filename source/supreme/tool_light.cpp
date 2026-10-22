#include "tool_light.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

LightTool::LightTool(void)
{
	brush=0;
	plopMode=LIGHTPLOP_SOLID;
	lastX=-1;
	lastY=-1;

	bright=16;
	random=0;
	lineOfSight=0;
	dragSlider=0;
}

LightTool::~LightTool(void)
{
}

void LightTool::Update(int msx,int msy)
{
	int i;
	MGLDraw* mgl = GetDisplayMGL();

	if(dragSlider)
	{
		if(dragSlider==1)
		{
			i=(msx-(mgl->GetWidth()-70))-32;
			if(i<-32)
				i=-32;
			if(i>31)
				i=31;
			bright=(char)i;
		}
		else
		{
			i=(msx-(mgl->GetWidth()-70))/4;
			if(i<0)
				i=0;
			if(i>15)
				i=15;
			random=(char)i;
		}
		if(!mgl->MouseDown())
			dragSlider=0;

		return;
	}

	if(msx<mgl->GetWidth()-260 || msy<mgl->GetHeight()-80 || msx>=mgl->GetWidth() || msy>=mgl->GetHeight())
		return;

	if(mgl->MouseTap())
	{
		if(PointInRect(msx, msy, mgl->GetWidth()-258, mgl->GetHeight()-18, mgl->GetWidth()-258+30, mgl->GetHeight()-18+15))
		{
			plopMode++;
			if(plopMode>=LIGHTPLOP_MODES)
				plopMode=LIGHTPLOP_SOLID;
		}
		if(PointInRect(msx, msy, mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		{
			brush++;
			if(brush>13)
				brush=0;
		}
		if(PointInRect(msx,msy, mgl->GetWidth()-120, mgl->GetHeight()-20, mgl->GetWidth()-120+100, mgl->GetHeight()-20+14))
		{
			lineOfSight=1-lineOfSight;
		}
	}

	if(mgl->RMouseTap())
	{
		for(i=0;i<4;i++)
			if(PointInRect(msx,msy,
				mgl->GetWidth()-76+(i%2)*(TILE_WIDTH+4)-2, mgl->GetHeight()-220+(i/2)*(TILE_HEIGHT+4)-2,
				mgl->GetWidth()-76+(i%2)*(TILE_WIDTH+4)+TILE_WIDTH+1, mgl->GetHeight()-220+(i/2)*(TILE_HEIGHT+4)+TILE_HEIGHT+1))
			{
				MakeNormalSound(SND_MENUCLICK);
			}
		if(PointInRect(msx, msy, mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
	}
	if(mgl->MouseDown())
	{
		if(PointInRect(msx,msy,mgl->GetWidth()-70,mgl->GetHeight()-70-4,mgl->GetWidth()-70+63,mgl->GetHeight()-70+5))
		{
			dragSlider=1;
		}
		if(PointInRect(msx,msy,mgl->GetWidth()-70,mgl->GetHeight()-49-4,mgl->GetWidth()-70+63,mgl->GetHeight()-49+5))
		{
			dragSlider=2;
		}
	}
}

void LightTool::Render(int msx,int msy)
{
	int minusBrush,plusBrush;
	char num[20];

	MGLDraw* mgl = GetDisplayMGL();

	char plopText[][12]={"Solid","Fade","Torch","Smooth"};

	// brightness slider
	if(bright>=0)
		sprintf(num,"Bright: %d",bright);
	else
		sprintf(num,"Bright:%d",bright);
	Print(mgl->GetWidth()-140,mgl->GetHeight()-76,num,0,1);
	RenderSliderImage(msx,msy,mgl->GetWidth()-70,mgl->GetHeight()-70,(bright+32));

	// random slider
	sprintf(num,"Random: %d",random);
	Print(mgl->GetWidth()-150,mgl->GetHeight()-55,num,0,1);
	RenderSliderImage(msx,msy,mgl->GetWidth()-70,mgl->GetHeight()-49,(random*4));

	// plop mode
	RenderButtonImage(msx, msy, mgl->GetWidth()-258, mgl->GetHeight()-18, 30, 15, "Plop");
	Print(mgl->GetWidth()-224, mgl->GetHeight()-16, plopText[plopMode], 0, 1);

	// line of sight
	RenderCheckbox(
		PointInRect(msx,msy,mgl->GetWidth()-120, mgl->GetHeight()-20,mgl->GetWidth()-120+100, mgl->GetHeight()-20+14),
		mgl->GetWidth()-120, mgl->GetHeight()-20, lineOfSight*CHECK_ON, "Line Of Sight", mgl
	);

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

void LightTool::SetInk(void)
{
}

void LightTool::StartPlop(void)
{
	lastX=-1;
	lastY=-1;
	Plop();
}

char LightTool::CalcBright(char b)
{
	if(random==0)
		return b;
	else
	{
		b=b-random+Random(random*2+1);
		if(b<-32)
			b=-32;
		if(b>31)
			b=31;
		return b;
	}
}

void LightTool::PlopOne(int centerx,int centery,int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if (mapTile_t *target = m->TryGetTile(x, y); target && target->select)
	{
		switch(plopMode)
		{
			case LIGHTPLOP_SOLID:
				target->light=CalcBright(bright);
				break;
			case LIGHTPLOP_FADE:
				char b;

				b=CalcBright(bright);
				if(target->light<b)
					target->light++;
				else if(target->light>b)
					target->light--;
				break;
			case LIGHTPLOP_SMOOTH:
				m->SmoothLight(x,y);
				break;
			case LIGHTPLOP_TORCH:
				int i;

				i=((centerx-x)*(centerx-x)+(centery-y)*(centery-y))/2;
				i=CalcBright(bright)-i;

				if(i>=0)
				{

					if(target->light<i)
						target->light=i;
					if(target->light>31)
						target->light=31;
				}
				break;
		}
	}
}

void LightTool::Plop(void)
{
	auto [x, y] = EditorGetTileXY();

	if(x!=lastX || y!=lastY)
	{
		int minusBrush=brush/2;
		int plusBrush=(brush+1)/2;
		LOS(x,y,x-minusBrush,y-minusBrush,x+plusBrush,y+plusBrush);

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void LightTool::ShowTarget(void)
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

void LightTool::SuckUp(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if (mapTile_t *target = m->TryGetTile(x, y); target)
	{
		bright = target->light;
	}
}

void LightTool::LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y)
{
	int xdist,ydist;

	xdist=curx-x;
	ydist=cury-y;

	if(xdist<0)
	{
		xdist=-xdist;	// for abs. value needed later
		*p1x=curx+1;
	}
	else if(xdist>0)
		*p1x=curx-1;
	else
		*p1x=curx;

	if(ydist<0)
	{
		ydist=-ydist;	// for abs. value needed later
		*p1y=cury+1;
	}
	else if(ydist>0)
		*p1y=cury-1;
	else
		*p1y=cury;

	if(ydist>xdist)	// need abs. value here
	{
		*p2x=curx;
		*p2y=*p1y;
	}
	else if(ydist<xdist)	// and here
	{
		*p2x=*p1x;
		*p2y=cury;
	}
	else
	{
		*p2x=*p1x;
		*p2y=*p1y;
	}
}

static byte GetOpaque(Map *map, int x, int y)
{
	if (const mapTile_t *tile = map->TryGetTile(x, y))
		return tile->opaque;
	else
		return 2;
}

static void SetOpaque(Map *map, int x, int y, byte opaque)
{
	if (mapTile_t *tile = map->TryGetTile(x, y))
		tile->opaque = opaque;
}

void LightTool::LOS(int x,int y,int rx,int ry,int rx2,int ry2)
{
	int p1x,p1y,p2x,p2y;
	int i,curx,cury;

	Map *map = EditorGetMap();
	int width = map->width;
	int height = map->height;

	int radius=(rx2-rx)/2+2;
	if((ry2-ry)/2+2>radius)
		radius=(ry2-ry)/2+2;

	if(rx<0)
		rx=0;
	if(rx2<0)
		return;
	if(rx>=width)
		return;
	if(rx2>=width)
		rx2=width-1;
	if(ry<0)
		ry=0;
	if(ry2<0)
		return;
	if(ry>=height)
		return;
	if(ry2>=height)
		ry2=height-1;

	// light up the center
	PlopOne(x,y,x,y);

	SetOpaque(map,x,y,0);

	for(i=1;i<radius;i++)	// i is the radius of the square you are working with
	{
		for(cury=y-i;cury<=y+i;cury+=i*2)
			for(curx=x-i;curx<=x+i;curx++)
			{
				// check to be sure point is legal
				if(curx<rx || curx>rx2 || cury<ry || cury>ry2)
					continue;

				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);

				if(lineOfSight)
				{
					if(GetOpaque(map,p1x,p1y)+
						GetOpaque(map,p2x,p2y)>=2)
					{
						SetOpaque(map,curx,cury,1);
					}
					else
					{
						if(map->GetTile(curx,cury)->wall)	// there's a wall here, so opaque
							SetOpaque(map,curx,cury,1);
						else
							SetOpaque(map,curx,cury,0);
						// do what you have to, it's in sight
						PlopOne(x,y,curx,cury);
					}
				}
				else
					PlopOne(x,y,curx,cury);
			}
		for(curx=x-i;curx<=x+i;curx+=i*2)
			for(cury=y-i+1;cury<=y+i-1;cury++)
			{
				// check to be sure point is legal
				if(curx<rx || curx>rx2 || cury<ry || cury>ry2)
					continue;
				LOSPoints(x,y,curx,cury,&p1x,&p1y,&p2x,&p2y);

				if(lineOfSight)
				{
					if(GetOpaque(map,p1x,p1y)+
						GetOpaque(map,p2x,p2y)>=2)
					{
						SetOpaque(map,curx,cury,1);
					}
					else
					{
						if(map->GetTile(curx,cury)->wall)	// there's a wall here, so opaque
							SetOpaque(map,curx,cury,1);
						else
						{
							SetOpaque(map,curx,cury,0);
						}
						// do what you have to, it's in sight
						PlopOne(x,y,curx,cury);
					}
				}
				else
					PlopOne(x,y,curx,cury);
			}
	}
}

void LightTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void LightTool::Erase(void)
{
	auto [x, y] = EditorGetTileXY();
	Map *m = EditorGetMap();

	if(x!=lastX || y!=lastY)
	{
		int minusBrush=brush/2;
		int plusBrush=(brush+1)/2;
		for(int j=y-minusBrush;j<=y+plusBrush;j++)
			for(int i=x-minusBrush;i<=x+plusBrush;i++)
			{
				if (mapTile_t *target = m->TryGetTile(i, j); target && target->select)
				{
					target->light=0;
				}
			}

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void LightTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}
