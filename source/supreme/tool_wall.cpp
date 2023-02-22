#include "tool_wall.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

WallTool::WallTool(void)
{
	active=0;
	brush=0;
	plopMode=PLOP_NORMAL;
	tile[0][0]=200;
	tile[1][0]=200;
	tile[2][0]=200;
	tile[3][0]=200;
	tile[0][1]=199;
	tile[1][1]=199;
	tile[2][1]=199;
	tile[3][1]=199;

	lastX=-1;
	lastY=-1;
}

WallTool::~WallTool(void)
{
}

void WallTool::Update(int msx,int msy)
{
	int i;
	MGLDraw* mgl = GetDisplayMGL();

	if(msx<mgl->GetWidth()-260 || msy<mgl->GetHeight()-80 || msx>=mgl->GetWidth() || msy>=mgl->GetHeight())
		return;

	if(mgl->MouseTap())
	{
		for(i=0;i<4;i++)
			if(PointInRect(msx, msy,
				mgl->GetWidth()-144+i*(TILE_WIDTH+4)-2, mgl->GetHeight()-56-2,
				mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH+1, mgl->GetHeight()-56+TILE_HEIGHT*2+1))
			{
				active=i;
				if(mgl->RMouseDown())
				{
					tile[active][0]=800;
					tile[active][1]=800;
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
	}

	if(mgl->RMouseTap())
	{
		for(i=0;i<4;i++)
		{
			if(PointInRect(msx,msy,
				mgl->GetWidth()-144+i*(TILE_WIDTH+4)-2,mgl->GetHeight()-56-2,
				mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH+1,mgl->GetHeight()-56+TILE_HEIGHT+1))
			{
				pickingTile=i;
				active=i;
				pickWallRoof=1;
				if(tile[i][0]==800)
				{
					tile[i][0]=0;
					tile[i][1]=0;
				}
				ToolPickInk();
				SetEditMode(EDITMODE_TERRAIN);
				TerrainEdit_InitPicker(EDITMODE_EDIT,EditorGetWorld());
				MakeNormalSound(SND_MENUCLICK);
			}
			if(PointInRect(msx,msy,
				mgl->GetWidth()-144+i*(TILE_WIDTH+4)-2,mgl->GetHeight()-56+TILE_HEIGHT+2,
				mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH+1,mgl->GetHeight()-56+TILE_HEIGHT*2+1))
			{
				pickingTile=i;
				active=i;
				pickWallRoof=0;
				if(tile[i][0]==800)
				{
					tile[i][0]=0;
					tile[i][1]=0;
				}
				ToolPickInk();
				SetEditMode(EDITMODE_TERRAIN);
				TerrainEdit_InitPicker(EDITMODE_EDIT,EditorGetWorld());
				MakeNormalSound(SND_MENUCLICK);
			}
		}
		if(PointInRect(msx, msy, mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
	}
}

void WallTool::Render(int msx,int msy)
{
	int i;
	int minusBrush,plusBrush;

	MGLDraw* mgl = GetDisplayMGL();

	char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle"};

	for(i=0;i<4;i++)
	{
		if(tile[i][0]<800 && tile[i][1]<800)
		{
			RenderFloorTile(mgl->GetWidth()-144+i*(TILE_WIDTH+4),mgl->GetHeight()-56+TILE_HEIGHT,tile[i][0],0);
			RenderFloorTile(mgl->GetWidth()-144+i*(TILE_WIDTH+4),mgl->GetHeight()-56,tile[i][1],0);
		}
		else
		{
			Print(mgl->GetWidth()-144+i*(TILE_WIDTH+4)+2,mgl->GetHeight()-56+2,"N",0,1);
			Print(mgl->GetWidth()-144+i*(TILE_WIDTH+4)+9,mgl->GetHeight()-56+13,"O",0,1);
			Print(mgl->GetWidth()-144+i*(TILE_WIDTH+4)+17,mgl->GetHeight()-56+25,"N",0,1);
			Print(mgl->GetWidth()-144+i*(TILE_WIDTH+4)+24,mgl->GetHeight()-56+36,"E",0,1);
		}

		DrawBox(
			mgl->GetWidth()-144+i*(TILE_WIDTH+4)-2, mgl->GetHeight()-56-2,
			mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH+1, mgl->GetHeight()-56+TILE_HEIGHT*2+1,
			16
		);
	}

	// highlight the active one
	i=active;
	DrawBox(mgl->GetWidth()-144+i*(TILE_WIDTH+4)-3,mgl->GetHeight()-56-3,
		    mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH+2,mgl->GetHeight()-56+TILE_HEIGHT*2+2,16);
	DrawBox(mgl->GetWidth()-144+i*(TILE_WIDTH+4)-2,mgl->GetHeight()-56-2,
		    mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH+1,mgl->GetHeight()-56+TILE_HEIGHT*2+1,31);
	DrawBox(mgl->GetWidth()-144+i*(TILE_WIDTH+4)-1,mgl->GetHeight()-56-1,
		    mgl->GetWidth()-144+i*(TILE_WIDTH+4)+TILE_WIDTH,mgl->GetHeight()-56+TILE_HEIGHT*2,16);

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

void WallTool::SetInk(void)
{
	if(EditorGetLastPick()!=-1)
		tile[pickingTile][pickWallRoof]=EditorGetLastPick();
}

void WallTool::StartPlop(void)
{
	lastX=-1;
	lastY=-1;
	Plop();
}

void WallTool::PlopOne(int x,int y)
{
	Map *m;
	int i;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height && m->map[x+y*m->width].select && tile[active][0]<800 && tile[active][1]<800)
	{
		m->map[x+y*m->width].floor=tile[active][1];
		m->map[x+y*m->width].wall=tile[active][0];
		m->map[x+y*m->width].item=0;	// no items here!
		for(i=0;i<MAX_MAPMONS;i++)
			if((m->badguy[i].type) && (m->badguy[i].x==x) && (m->badguy[i].y==y))
			{
				// get rid of anybody already on this space
				m->badguy[i].type=0;
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

void WallTool::Plop(void)
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

void WallTool::ShowTarget(void)
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

	x1=tileX*TILE_WIDTH-(cx-GetDisplayMGL()->GetWidth()/2);
	y1=tileY*TILE_HEIGHT-(cy-GetDisplayMGL()->GetHeight()/2);

	x2=tileX2*TILE_WIDTH-(cx-GetDisplayMGL()->GetWidth()/2)+TILE_WIDTH-1;
	y2=tileY2*TILE_HEIGHT-(cy-GetDisplayMGL()->GetHeight()/2)+TILE_HEIGHT-1;

	DrawBox(x1,y1,x2,y1,col);
	DrawBox(x1,y2,x2,y2,col);
	DrawBox(x1,y1,x1,y2,col);
	DrawBox(x2,y1,x2,y2,col);
}

void WallTool::SuckUp(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height)
	{
		if(m->map[x+y*m->width].wall)
		{
			tile[active][1]=m->map[x+y*m->width].floor;
			tile[active][0]=m->map[x+y*m->width].wall;
		}
	}
}

void WallTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void WallTool::Erase(void)
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
					m->map[i+j*m->width].wall=0;
				}
			}

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void WallTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}
