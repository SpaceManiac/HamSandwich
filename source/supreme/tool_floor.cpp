#include "tool_floor.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

FloorTool::FloorTool(void)
{
	active=0;
	brush=0;
	plopMode=PLOP_NORMAL;
	for (int i = 0; i < NUM_TILES; ++i)
		tile[i] = i % GetNumTiles();
	lastX=-1;
	lastY=-1;
}

FloorTool::~FloorTool(void)
{
}

void FloorTool::Update(int msx,int msy)
{
	int i;
	MGLDraw* mgl = GetDisplayMGL();

	if(msx<mgl->GetWidth()-260 || msy<mgl->GetHeight()-80 || msx>=mgl->GetWidth() || msy>=mgl->GetHeight())
		return;

	if(mgl->MouseTap())
	{
		for(i=0;i<NUM_TILES;i++)
			if(PointInRect(msx, msy,
				mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)-2, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
				mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1))
			{
				active=i;
				if(mgl->RMouseDown())
				{
					tile[active]=UINT16_MAX;
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
		for(i=0;i<NUM_TILES;i++)
			if(PointInRect(msx, msy,
				mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)-2, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
				mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1))
			{
				pickingTile=i;
				active=i;
				ToolPickInk();
				SetEditMode(EDITMODE_TERRAIN);
				TerrainEdit_InitPicker(EDITMODE_EDIT,EditorGetWorld());
				MakeNormalSound(SND_MENUCLICK);
			}
		if(PointInRect(msx, msy, mgl->GetWidth()-243-15, mgl->GetHeight()-38-15, mgl->GetWidth()-243+16, mgl->GetHeight()-38+16))
		{
			brush--;
			if(brush>13)
				brush=13;
		}
	}
}

void FloorTool::Render(int msx,int msy)
{
	int i;
	int minusBrush,plusBrush;
	MGLDraw* mgl = GetDisplayMGL();

	static const char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle"};

	for(i=0;i<NUM_TILES;i++)
	{
		if(tile[i]<NUMTILES)
			RenderFloorTile(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4), mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4),tile[i],0);
		else
		{
			Print(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+2, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+1,"N",0,1);
			Print(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+9, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+5,"O",0,1);
			Print(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+17, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+10,"N",0,1);
			Print(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+24, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+15,"E",0,1);
		}

		DrawBox(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)-2,mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
				mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1,mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1,
				16);
	}

	// highlight the active one
	i=active;
	DrawBox(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)-1, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)-1,
			mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT,
			16);
	DrawBox(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)-2, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
			mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1,
			31);
	DrawBox(mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)-3, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)-3,
			mgl->GetWidth()-148+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+2, mgl->GetHeight()-60+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+2,
			16);

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

void FloorTool::SetInk(void)
{
	if(EditorGetLastPick()!=-1)
		tile[pickingTile]=EditorGetLastPick();
}

void FloorTool::StartPlop(void)
{
	lastX=-1;
	lastY=-1;
	Plop();
}

void FloorTool::PlopOne(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if (mapTile_t *target = m->TryGetTile(x, y); target && target->select && tile[active]<UINT16_MAX)
	{
		target->floor=tile[active];
		target->wall=0;
	}

	if(plopMode==PLOP_RANDOM)
		active=(byte)Random(NUM_TILES);
	if(plopMode==PLOP_CYCLE)
	{
		active++;
		if(active>=NUM_TILES)
			active=0;
	}
}

void FloorTool::Plop(void)
{
	auto [x, y] = EditorGetTileXY();

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
			active=(byte)Random(NUM_TILES);
		}
		if(plopMode==PLOP_CYCLE2)
		{
			active++;
			if(active>=NUM_TILES)
				active=0;
		}
	}
}

void FloorTool::ShowTarget(void)
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

void FloorTool::SuckUp(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if (mapTile_t *target = m->TryGetTile(x, y))
	{
		tile[active] = target->floor;
	}
}

void FloorTool::StartErase(void)
{
	lastX=-1;
	lastY=-1;
	Erase();
}

void FloorTool::Erase(void)
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
					target->floor=0;
					target->wall=0;
				}
			}

		MakeNormalSound(SND_MENUCLICK);
		lastX=x;
		lastY=y;
	}
}

void FloorTool::BrushUp(void)
{
	brush++;
	if(brush>13)
		brush=0;
}
