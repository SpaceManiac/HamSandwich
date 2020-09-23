#include "tool_floor.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

FloorTool::FloorTool(void)
{
	active=0;
	tilesCleared=0;
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

	if(msx<380 || msy<400 || msx>639 || msy>479)
		return;

	if(GetDisplayMGL()->MouseTap())
	{
		for(i=0;i<NUM_TILES;i++)
			if(PointInRect(msx,msy,
				492+(i%ROW_TILES)*(TILE_WIDTH+4)-2,420+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
				492+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1))
			{
				active=i;
				if(GetDisplayMGL()->RMouseDown())
				{
					tile[active]=800;
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
		for(i=0;i<NUM_TILES;i++)
			if(PointInRect(msx,msy,
				492+(i%ROW_TILES)*(TILE_WIDTH+4)-2,420+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
				492+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1))
			{
				pickingTile=i;
				active=i;
				ToolPickInk();
				SetEditMode(EDITMODE_TERRAIN);
				TerrainEdit_InitPicker(EDITMODE_EDIT,EditorGetWorld());
				MakeNormalSound(SND_MENUCLICK);
			}
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
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

	char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle","FillTouch","FillAll","CycleAll"};

	for(i=0;i<NUM_TILES;i++)
	{
		if(tile[i]<800)
			RenderFloorTile(492+(i%ROW_TILES)*(TILE_WIDTH+4),420+(i/ROW_TILES)*(TILE_HEIGHT+4),tile[i],0);
		else
		{
			Print(492+(i%ROW_TILES)*(TILE_WIDTH+4)+2,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+1,"N",0,1);
			Print(492+(i%ROW_TILES)*(TILE_WIDTH+4)+9,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+5,"O",0,1);
			Print(492+(i%ROW_TILES)*(TILE_WIDTH+4)+17,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+10,"N",0,1);
			Print(492+(i%ROW_TILES)*(TILE_WIDTH+4)+24,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+15,"E",0,1);
		}

		DrawBox(492+(i%ROW_TILES)*(TILE_WIDTH+4)-2,420+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
				492+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1,
				16);
	}

	// highlight the active one
	i=active;
	DrawBox(492+(i%ROW_TILES)*(TILE_WIDTH+4)-1,420+(i/ROW_TILES)*(TILE_HEIGHT+4)-1,
			492+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT,
			16);
	DrawBox(492+(i%ROW_TILES)*(TILE_WIDTH+4)-2,420+(i/ROW_TILES)*(TILE_HEIGHT+4)-2,
			492+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+1,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+1,
			31);
	DrawBox(492+(i%ROW_TILES)*(TILE_WIDTH+4)-3,420+(i/ROW_TILES)*(TILE_HEIGHT+4)-3,
			492+(i%ROW_TILES)*(TILE_WIDTH+4)+TILE_WIDTH+2,420+(i/ROW_TILES)*(TILE_HEIGHT+4)+TILE_HEIGHT+2,
			16);

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

	if(x>=0 && y>=0 && x<m->width && y<m->height && m->map[x+y*m->width].select && tile[active]<800)
	{
		m->map[x+y*m->width].floor=tile[active];
		m->map[x+y*m->width].wall=0;
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

void FloorTool::PlopFill(int x,int y,int floor,int wall)
{
	Map *m;
	int preFloor,preWall;
	m=EditorGetMap();
	
	if(x<0 || y<0 || x>=m->width || y>=m->height || tile[active]>=800 || tilesCleared>=10000)
	{
		MakeNormalSound(SND_BOMBBOOM);
		return;
	}

	tilesCleared++;
	preFloor=m->map[x+y*m->width].floor;
	preWall=m->map[x+y*m->width].wall;

	if(preFloor==floor && preWall==wall)
		return;

	m->map[x+y*m->width].floor=floor;
	m->map[x+y*m->width].wall=wall;

	if(x>0 && m->map[x-1+y*m->width].wall==preWall &&
		m->map[x-1+y*m->width].floor==preFloor)
		PlopFill(x-1,y,floor,wall);
	if(x<m->width-1 && m->map[x+1+y*m->width].wall==preWall &&
		m->map[x+1+y*m->width].floor==preFloor)
		PlopFill(x+1,y,floor,wall);
	if(y>0 && m->map[x+(y-1)*m->width].wall==preWall &&
		m->map[x+(y-1)*m->width].floor==preFloor)
		PlopFill(x,y-1,floor,wall);
	if(y<m->height-1 && m->map[x+(y+1)*m->width].wall==preWall &&
		m->map[x+(y+1)*m->width].floor==preFloor)
		PlopFill(x,y+1,floor,wall);
}

void FloorTool::Plop(void)
{
	Map *m;
	int x,y;
	int i,j,minusBrush,plusBrush;

	EditorGetTileXY(&x,&y);
	m=EditorGetMap();
	tilesCleared = 0;

	if(x!=lastX || y!=lastY)
	{
		if(plopMode==PLOP_CUST1)
		{
			PlopFill(x,y,tile[active],0);
		}
		else
		{
		minusBrush=brush/2;
		plusBrush=(brush+1)/2;
		for(j=y-minusBrush;j<=y+plusBrush;j++)
			for(i=x-minusBrush;i<=x+plusBrush;i++)
				PlopOne(i,j);
		}

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
		if(plopMode==PLOP_CUST3)
		{
			active=0;
			if(tile[active]<799)
				tile[active]++;
			else
				tile[active]=0;
		}
	}
}

void FloorTool::ShowTarget(void)
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

void FloorTool::SuckUp(int x,int y)
{
	Map *m;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height)
	{
		tile[active]=m->map[x+y*m->width].floor;
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
					m->map[i+j*m->width].floor=0;
					m->map[i+j*m->width].wall=0;
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