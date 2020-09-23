#include "tool_wall.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"

WallTool::WallTool(void)
{
	active=0;
	target=0;
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
					tile[active][0]=800;
					tile[active][1]=800;
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
						   496+i*(TILE_WIDTH+4)+TILE_WIDTH+1,424+TILE_HEIGHT+1))
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
				           496+i*(TILE_WIDTH+4)-2,424+TILE_HEIGHT+2,
						   496+i*(TILE_WIDTH+4)+TILE_WIDTH+1,424+TILE_HEIGHT*2+1))
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
		if(PointInRect(msx,msy,397-15,442-15,397+16,442+16))
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

	char plopText[][12]={"Normal","Random","Cycle","BigRandom","BigCycle","FillTouch","FillAll","CycleAll"};

	for(i=0;i<4;i++)
	{
		if(tile[i][0]<800 && tile[i][1]<800)
		{
			RenderFloorTile(496+i*(TILE_WIDTH+4),424+TILE_HEIGHT,tile[i][0],0);
			RenderFloorTile(496+i*(TILE_WIDTH+4),424,tile[i][1],0);
		}
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

	if(plopMode==PLOP_CUST1 && m->map[x+y*m->width].floor != tile[active][1])
	{
		PlopFill(x,y,tile[active][1],tile[active][0]);
	}
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

void WallTool::PlopFill(int x,int y,int floor,int wall)
{
	Map *m;
	int preFloor,preWall;
	m=EditorGetMap();

	if(x<0 || y<0 || x>=m->width || y>=m->height || tile[active][1]>=800)
		return;

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

	x1=tileX*TILE_WIDTH-(cx-320);
	y1=tileY*TILE_HEIGHT-(cy-240);

	x2=tileX2*TILE_WIDTH-(cx-320)+TILE_WIDTH-1;
	y2=tileY2*TILE_HEIGHT-(cy-240)+TILE_HEIGHT-1;

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
