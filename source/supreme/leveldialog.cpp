#include "winpch.h"
#include "leveldialog.h"
#include "editor.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "map.h"
#include "filedialog.h"

#define LDMODE_NORMAL	0
#define LDMODE_NAME		1	// typing in a name
#define LDMODE_WIDTH	2	// typing in width
#define LDMODE_HEIGHT	3	// typing in height
#define LDMODE_BRAINS	4	// typing in brains
#define LDMODE_CANDLES	5	// typing in candles
#define LDMODE_SONGPICK 6	// file dialog to pick a song
#define LDMODE_ITEMDROP	7	// entering itemdrop %

#define ASK_WIDTH		1	// asking if you should really change width
#define ASK_HEIGHT		2	// asking about height

#define ID_STATIC		1
#define ID_NAME			2
#define ID_WIDTH		3
#define ID_HEIGHT		4
#define ID_BRAINS		5
#define ID_CANDLES		6
#define ID_AUTOBRAIN	7
#define ID_AUTOCANDLE	8
#define ID_EXIT			9
#define ID_SONG			10
#define ID_ITEMDROP		11
#define ID_SLIDE		20
#define ID_FLAG			40
#define ID_PREV			80
#define ID_NEXT			81

#define DLG_X		(100)
#define DLG_Y		(60)
#define DLG_X2		(639-100)
#define DLG_Y2		(380)

static byte asking,yesNo;
static char question[64];
static byte rClick;

static byte mapNum,mode;
static world_t *world;

static word flagNum[]={MAP_SNOWING,MAP_RAIN,MAP_HUB,MAP_SECRET,MAP_TORCHLIT,MAP_WELLLIT,
				MAP_STARS,MAP_UNDERWATER,MAP_LAVA,MAP_STEALTH,MAP_WAVY,MAP_OXYGEN};

static byte *mapZoom;
static byte desiredWidth,desiredHeight;
static sprite_set_t *levelSpr;

void LevelDialogButtons(void);
void RenderZoomMap(void);

static void NameClick(int id)
{
	mode=LDMODE_NAME;
	InitTextDialog("Enter a name for the level:",world->map[mapNum]->name,31);
}

static void SongClick(int id)
{
	if(rClick)
	{
		world->map[mapNum]->song[0]='\0';
		LevelDialogButtons();
	}
	else
	{
		mode=LDMODE_SONGPICK;
		InitFileDialog("music",".ogg",FM_LOAD|FM_EXIT|FM_PLAYSONGS,world->map[mapNum]->song);
	}
}

static void FlagClick(int id)
{
	if(flagNum[id-ID_FLAG]==MAP_HUB && mapNum==0)
		return;	// can't unhub level zero

	world->map[mapNum]->flags^=flagNum[id-ID_FLAG];
	if (flagNum[id-ID_FLAG] == MAP_STARS)
		InitStars();
	LevelDialogButtons();
}

static void PrevClick(int id)
{
	mapNum--;
	if(mapNum>=world->numMaps)
		mapNum=world->numMaps-1;
	EditorSelectMap(mapNum);
	LevelDialogButtons();
	RenderZoomMap();
}

static void NextClick(int id)
{
	mapNum++;
	if(mapNum>=world->numMaps)
		mapNum=0;
	EditorSelectMap(mapNum);
	LevelDialogButtons();
	RenderZoomMap();
}

static void WidthClick(int id)
{
	mode=LDMODE_WIDTH;
	InitTextDialog("Enter level width (20-200):","",3);
}

static void HeightClick(int id)
{
	mode=LDMODE_HEIGHT;
	InitTextDialog("Enter level height (20-200)","",3);
}

static void BrainsClick(int id)
{
	mode=LDMODE_BRAINS;
	InitTextDialog("Enter # of Brains needed to win:","",5);
}

static void CandlesClick(int id)
{
	mode=LDMODE_CANDLES;
	InitTextDialog("Enter # of Candles needed for bonus:","",5);
}

static void ItemDropClick(int id)
{
	mode=LDMODE_ITEMDROP;
	InitTextDialog("Enter chance for enemies to drop items:","",5);
}

static bool CountsDouble(int type)
{
	// Enemies that drop 1 baby on death count 2x, once for them and once for baby.
	// Enemies with infinite babies are impossible to account for, so don't bother.
	return type == MONS_ZOMBONI || type == MONS_COFFIN || type == MONS_DARKCOFFIN || type == MONS_XENOEGG;
}

static int BrainsForMonster(int type, int item)
{
	int result = 0, brainsPerBrain = BrainsGiven(ITM_BRAIN), brainsForItem = BrainsGiven(item);
	if (type == MONS_ZOMBIE || type == MONS_ZOMBONI || type == MONS_MUTANT)
		result += brainsPerBrain;
	if (type == MONS_SUPERZOMBIE)
		result += 2 * brainsPerBrain;
	if (brainsForItem > 0 && type)
		result += (CountsDouble(type) ? 2 : 1) * brainsForItem;
	return result;
}

static void AutoBrainsClick(int id)
{
	// autocalc brains needed
	int i,j;
	Map *m;

	m=world->map[mapNum];
	m->numBrains=0;

	for(i=0;i<m->width*m->height;i++)
	{
		j=BrainsGiven(m->map[i].item);
		if(j>0)
			m->numBrains+=j;	// negative ones aren't counted
	}
	for(i=0;i<MAX_MAPMONS;i++)
	{
		m->numBrains += BrainsForMonster(m->badguy[i].type, m->badguy[i].item);
	}

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(m->special[i].x!=255)
		{
			for(j=0;j<NUM_EFFECTS;j++)
			{
				if(m->special[i].effect[j].type==EFF_ITEM && BrainsGiven(m->special[i].effect[j].value)>0)
					m->numBrains+=BrainsGiven(m->special[i].effect[j].value);
				if(m->special[i].effect[j].type==EFF_MONSITEM && BrainsGiven(m->special[i].effect[j].value2)>0)
					m->numBrains+=BrainsGiven(m->special[i].effect[j].value2);
				if(m->special[i].effect[j].type==EFF_SUMMON)
				{
					m->numBrains += BrainsForMonster(m->special[i].effect[j].value, m->special[i].effect[j].value2);
				}
			}
		}
	}

	LevelDialogButtons();
}

static void AutoCandlesClick(int id)
{
	// autocalc candles needed
	int i,j;
	Map *m;

	m=world->map[mapNum];
	m->numCandles=0;

	for(i=0;i<m->width*m->height;i++)
	{
		j=CandlesGiven(m->map[i].item);
		if(j>0)
			m->numCandles+=j;	// negative ones aren't counted
	}
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(m->badguy[i].type)
			m->numCandles += (CountsDouble(m->badguy[i].type) ? 2 : 1) * CandlesGiven(m->badguy[i].item);
	}

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(m->special[i].x!=255)
		{
			for(j=0;j<NUM_EFFECTS;j++)
			{
				if(m->special[i].effect[j].type==EFF_ITEM && CandlesGiven(m->special[i].effect[j].value)>0)
					m->numCandles+=CandlesGiven(m->special[i].effect[j].value);
				if(m->special[i].effect[j].type==EFF_MONSITEM && CandlesGiven(m->special[i].effect[j].value2)>0)
					m->numCandles+=CandlesGiven(m->special[i].effect[j].value2);
				if(m->special[i].effect[j].type==EFF_SUMMON && CandlesGiven(m->special[i].effect[j].value2)>0)
					m->numCandles += (CountsDouble(m->special[i].effect[j].value) ? 2 : 1) * CandlesGiven(m->special[i].effect[j].value2);
			}
		}
	}

	LevelDialogButtons();
}

static void SlideClick(int id)
{
	int i,j;
	int dx,dy,tx,ty;
	Map *m;
	mapTile_t t;

	m=world->map[mapNum];

	dx=dy=0;
	if(id==ID_SLIDE+0)	// up
		dy=-1;
	else if(id==ID_SLIDE+1)	// right
		dx=1;
	else if(id==ID_SLIDE+2)	// down
		dy=1;
	else
		dx=-1;

	// slide the monsters
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(m->badguy[i].type)
		{
			m->badguy[i].x=SlideCoord(m->badguy[i].x,dx,m->width);
			m->badguy[i].y=SlideCoord(m->badguy[i].y,dy,m->height);
		}
	}

	if(dy<0)
	{
		for(j=0;j<m->height-1;j++)
		{
			ty=SlideCoord(j,dy,m->height);
			if(dx<=0)
			{
				for(i=0;i<m->width-1+(dx==0);i++)
				{
					tx=SlideCoord(i,dx,m->width);
					memcpy(&t,&m->map[i+j*m->width],sizeof(mapTile_t));
					memcpy(&m->map[i+j*m->width],&m->map[tx+ty*m->width],sizeof(mapTile_t));
					memcpy(&m->map[tx+ty*m->width],&t,sizeof(mapTile_t));
				}
			}
			else if(dx>0)
			{
				for(i=m->width-1;i>0;i--)
				{
					tx=SlideCoord(i,dx,m->width);
					memcpy(&t,&m->map[i+j*m->width],sizeof(mapTile_t));
					memcpy(&m->map[i+j*m->width],&m->map[tx+ty*m->width],sizeof(mapTile_t));
					memcpy(&m->map[tx+ty*m->width],&t,sizeof(mapTile_t));
				}
			}
		}
	}
	else
	{
		for(j=m->height-1;j>0-(dy==0);j--)
		{
			ty=SlideCoord(j,dy,m->height);
			if(dx<=0)
			{
				for(i=0;i<m->width-1+(dx==0);i++)
				{
					tx=SlideCoord(i,dx,m->width);
					memcpy(&t,&m->map[i+j*m->width],sizeof(mapTile_t));
					memcpy(&m->map[i+j*m->width],&m->map[tx+ty*m->width],sizeof(mapTile_t));
					memcpy(&m->map[tx+ty*m->width],&t,sizeof(mapTile_t));
				}
			}
			else if(dx>0)
			{
				for(i=m->width-1;i>0;i--)
				{
					tx=SlideCoord(i,dx,m->width);
					memcpy(&t,&m->map[i+j*m->width],sizeof(mapTile_t));
					memcpy(&m->map[i+j*m->width],&m->map[tx+ty*m->width],sizeof(mapTile_t));
					memcpy(&m->map[tx+ty*m->width],&t,sizeof(mapTile_t));
				}
			}
		}
	}
	SlideSpecials(m,dx,dy);
	AddMapGuys(world->map[mapNum]);
	RenderZoomMap();
}

void LevelDialogButtons(void)
{
	int i;
	char s[32];

	ClearButtons();

	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+2,DLG_Y+2,2,2,"Level Options",NULL);

	MakeButton(BTN_NORMAL,ID_NAME,0,DLG_X+2,DLG_Y+20,50,15,"Name",NameClick);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+54,DLG_Y+20,50,15,world->map[mapNum]->name,NameClick);

	MakeButton(BTN_NORMAL,ID_SONG,0,DLG_X+2,DLG_Y+37,50,15,"Song",SongClick);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+54,DLG_Y+37,50,15,world->map[mapNum]->song,SongClick);

	for(i=0;i<NUM_LVL_FLAGS;i++)
	{
		MakeButton(BTN_CHECK,ID_FLAG+i,((world->map[mapNum]->flags&flagNum[i])!=0)*CHECK_ON,DLG_X+2,DLG_Y+66+16*i,
			150,15,MapFlagName(i),FlagClick);
	}

	MakeButton(BTN_NORMAL,ID_PREV,0,DLG_X2-104,DLG_Y+2,50,15,"Prev",PrevClick);
	MakeButton(BTN_NORMAL,ID_NEXT,0,DLG_X2-52,DLG_Y+2,50,15,"Next",NextClick);

	MakeButton(BTN_NORMAL,ID_WIDTH,0,DLG_X2-211,DLG_Y2-240,50,15,"Width",WidthClick);
	sprintf(s,"%d",world->map[mapNum]->width);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X2-211+54,DLG_Y2-240,50,15,s,NULL);

	MakeButton(BTN_NORMAL,ID_WIDTH,0,DLG_X2-211+108,DLG_Y2-240,50,15,"Height",HeightClick);
	sprintf(s,"%d",world->map[mapNum]->height);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X2-211+162,DLG_Y2-240,50,15,s,NULL);

	MakeButton(BTN_NORMAL,ID_ITEMDROP,0,DLG_X+2,DLG_Y+262,75,15,"Item Drops",ItemDropClick);
	sprintf(s,"%0.2f%%",(float)world->map[mapNum]->itemDrops/(float)FIXAMT);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+81,DLG_Y+262,40,15,s,NULL);

	MakeButton(BTN_NORMAL,ID_BRAINS,0,DLG_X+2,DLG_Y+280,55,15,"Brains",BrainsClick);
	sprintf(s,"%d",world->map[mapNum]->numBrains);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+59,DLG_Y+280,40,15,s,NULL);
	MakeButton(BTN_NORMAL,ID_AUTOBRAIN,0,DLG_X+110,DLG_Y+280,40,15,"Auto",AutoBrainsClick);
	MakeButton(BTN_NORMAL,ID_BRAINS,0,DLG_X+2,DLG_Y+298,55,15,"Candles",CandlesClick);
	sprintf(s,"%d",world->map[mapNum]->numCandles);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+59,DLG_Y+298,40,15,s,NULL);
	MakeButton(BTN_NORMAL,ID_AUTOCANDLE,0,DLG_X+110,DLG_Y+298,40,15,"Auto",AutoCandlesClick);
}

byte ZoomTileColor(int x,int y)
{
	mapTile_t *m;
	int i;

	m=&world->map[mapNum]->map[x+y*world->map[mapNum]->width];

	for(i=0;i<MAX_MAPMONS;i++)
		if(world->map[mapNum]->badguy[i].type &&
			world->map[mapNum]->badguy[i].x==x &&
			world->map[mapNum]->badguy[i].y==y)
		{
			return (4*32)+20;	// red=badguy
		}

	if(GetItem(m->item)->flags&IF_SOLID)
	{
		if(GetItem(m->item)->flags&IF_BULLETPROOF)
			return 31;	// white=completely solid object
		else
			return 20;	// grey=solid object that bullets can fly over
	}
	if(m->wall)
		return 31;	// white=wall
	else
	{
		if(world->terrain[m->floor].flags&TF_SOLID)
			return (20);	// grey=solid floor tile
		if(world->terrain[m->floor].flags&TF_WATER)
			return (32*3)+8;	// blue=water
		if(world->terrain[m->floor].flags&TF_LAVA)
			return (32*4)+8;	// dark red=lava
		if(world->terrain[m->floor].flags&TF_ICE)
			return 8;	// really dark grey=ice
		if(world->terrain[m->floor].flags&TF_MINECART)
			return (32*1)+5;	// darker green=mine path
		if(world->terrain[m->floor].flags&TF_BUNNY)
			return (32*1)+12;	// brighter green=bunny path

		return (32*1)+8;	// dark green=floor
	}
}

void RenderZoomMap(void)
{
	int i,j;

	memset(mapZoom,0,MAX_MAPSIZE*MAX_MAPSIZE);
	for(i=0;i<world->map[mapNum]->width;i++)
	{
		for(j=0;j<world->map[mapNum]->height;j++)
		{
			mapZoom[i+j*MAX_MAPSIZE]=ZoomTileColor(i,j);
		}
	}
}

void InitLevelDialog(world_t *wrld,byte currentMap)
{
	mode=LDMODE_NORMAL;
	world=wrld;
	mapNum=currentMap;

	asking=0;

	levelSpr=new sprite_set_t("graphics/pause.jsp");
	LevelDialogButtons();
	mapZoom=new byte[MAX_MAPSIZE*MAX_MAPSIZE];
	RenderZoomMap();
}

void ExitLevelDialog(void)
{
	ClearButtons();
	delete[] mapZoom;
	delete levelSpr;
}

void RenderLevelDialogZoom(MGLDraw *mgl)
{
	mgl->ResizeBuffer(world->map[mapNum]->width, world->map[mapNum]->height);
	for (int y = 0; y < mgl->GetHeight(); ++y)
		memcpy(mgl->GetScreen() + mgl->GetWidth() * y, &mapZoom[y * MAX_MAPSIZE], mgl->GetWidth());
}

void RenderLevelDialog(int msx,int msy,MGLDraw *mgl)
{
	int i;

	// box for the whole dialog
	mgl->FillBox(DLG_X,DLG_Y,DLG_X2,DLG_Y2,8);
	mgl->Box(DLG_X,DLG_Y,DLG_X2,DLG_Y2,31);

	if(PointInRect(msx,msy,DLG_X2-211,DLG_Y2-220,DLG_X2-10,DLG_Y2-211))
		levelSpr->GetSprite(4)->DrawBright(DLG_X2-219,DLG_Y2-220,mgl,8);
	else
		levelSpr->GetSprite(4)->Draw(DLG_X2-219,DLG_Y2-220,mgl);

	if(PointInRect(msx,msy,DLG_X2-9,DLG_Y2-210,DLG_X2-2,DLG_Y2-9))
		levelSpr->GetSprite(5)->DrawBright(DLG_X2-219,DLG_Y2-220,mgl,8);
	else
		levelSpr->GetSprite(5)->Draw(DLG_X2-219,DLG_Y2-220,mgl);

	if(PointInRect(msx,msy,DLG_X2-211,DLG_Y2-9,DLG_X2-10,DLG_Y2-2))
		levelSpr->GetSprite(6)->DrawBright(DLG_X2-219,DLG_Y2-220,mgl,8);
	else
		levelSpr->GetSprite(6)->Draw(DLG_X2-219,DLG_Y2-220,mgl);

	if(PointInRect(msx,msy,DLG_X2-220,DLG_Y2-210,DLG_X2-212,DLG_Y2-9))
		levelSpr->GetSprite(7)->DrawBright(DLG_X2-219,DLG_Y2-220,mgl,8);
	else
		levelSpr->GetSprite(7)->Draw(DLG_X2-219,DLG_Y2-220,mgl);

	for(i=0;i<MAX_MAPSIZE;i++)
	{
		memcpy((byte *)(mgl->GetScreen()+mgl->GetWidth()*(DLG_Y2+i-210)+DLG_X2-211),
			&mapZoom[i*MAX_MAPSIZE],MAX_MAPSIZE);
	}
	switch(mode)
	{
		case LDMODE_SONGPICK:
			RenderButtons(-1,-1,mgl);
			RenderFileDialog(msx,msy,mgl);
			break;
		case LDMODE_NORMAL:
			RenderButtons(msx,msy,mgl);
			if(asking)
			{
				mgl->FillBox(320-140,240-10,320+140,240+60,8);
				mgl->Box(320-140,240-10,320+140,240+60,31);
				CenterPrint(320,244,question,0,1);
				RenderButtonImage(msx,msy,320-50-15,240+40,30,15,"Yes");
				RenderButtonImage(msx,msy,320+50-15,240+40,30,15,"No");
			}
			break;
		case LDMODE_NAME:
		case LDMODE_WIDTH:
		case LDMODE_HEIGHT:
		case LDMODE_BRAINS:
		case LDMODE_CANDLES:
		case LDMODE_ITEMDROP:
			RenderButtons(-1,-1,mgl);
			RenderTextDialog(msx,msy,mgl);
			break;
	}
}

void LevelDialogYes(void)
{
	int i;
	Map *m;
	switch(asking)
	{
		case ASK_WIDTH:
		case ASK_HEIGHT:
			asking=0;
			m=world->map[mapNum];
			// resize the map as requested
			if(!m->Resize(desiredWidth,desiredHeight))
				return;	// couldn't resize it
			for(i=0;i<MAX_MAPMONS;i++)
			{
				if(m->badguy[i].type && (m->badguy[i].x>=m->width || m->badguy[i].y>=m->height))
					m->badguy[i].type=0;	// remove excess badguys
			}
			for(i=0;i<MAX_SPECIAL;i++)
			{
				if(m->special[i].x!=255 && (m->special[i].x>=m->width || m->special[i].y>=m->height))
				{
					DeleteSpecial(i);
					i--;	// have to back up a notch in case the one that moved down was also
							// off the edge
				}
			}
			AddMapGuys(m);
			RenderZoomMap();
			LevelDialogButtons();
			break;
	}
}

byte LevelDialogKey(char key)
{
	switch(mode)
	{
		case LDMODE_NORMAL:
			if(asking)
			{
				if(key==27 || key=='n' || key=='N')
				{
					yesNo=0;
					asking=0;
				}
				if(key=='y' || key=='Y')
				{
					LevelDialogYes();
				}
				return 1;
			}
			else
			{
				if(key==27)	// esc
					return 0;
			}
			break;
		case LDMODE_SONGPICK:
			FileDialogKey(key);
			if(FileDialogCommand()==FM_LOAD)
			{
				strcpy(world->map[mapNum]->song,GetFilename(""));
				ExitFileDialog();
				mode=LDMODE_NORMAL;
				LevelDialogButtons();
			}
			else if(FileDialogCommand()==FM_EXIT)
			{
				ExitFileDialog();
				mode=LDMODE_NORMAL;
				LevelDialogButtons();
			}
			break;
		case LDMODE_NAME:
		case LDMODE_WIDTH:
		case LDMODE_HEIGHT:
		case LDMODE_BRAINS:
		case LDMODE_CANDLES:
		case LDMODE_ITEMDROP:
			TextDialogKey(key);
			if(TextDialogCommand()==TM_OKAY)
			{
				if(mode==LDMODE_NAME)
				{
					strcpy(world->map[mapNum]->name,GetText());
					for(int i=0;i<(int)strlen(world->map[mapNum]->name);i++)
						if(world->map[mapNum]->name[i]=='|')
							world->map[mapNum]->name[i]='/';	// remove any vertical pipes

				}
				else if(mode==LDMODE_WIDTH)
				{
					//resize map
					desiredWidth=(byte)atoi(GetText());
					desiredHeight=world->map[mapNum]->height;
					if(desiredWidth<20)
						desiredWidth=20;
					if(desiredWidth>MAX_MAPSIZE)
						desiredWidth=MAX_MAPSIZE;
					asking=ASK_WIDTH;
					sprintf(question,"Width %d will erase stuff! OK?",desiredWidth);
					if(desiredWidth>=world->map[mapNum]->width)
						LevelDialogYes();	// instant yes, if you're making it bigger
				}
				else if(mode==LDMODE_HEIGHT)
				{
					//resize map
					desiredWidth=world->map[mapNum]->width;
					desiredHeight=(byte)atoi(GetText());
					if(desiredHeight<20)
						desiredHeight=20;
					if(desiredHeight>MAX_MAPSIZE)
						desiredHeight=MAX_MAPSIZE;
					asking=ASK_HEIGHT;
					sprintf(question,"Height %d will erase stuff! OK?",desiredHeight);
					if(desiredHeight>=world->map[mapNum]->height)
						LevelDialogYes();	// instant yes, if you're making it bigger
				}
				else if(mode==LDMODE_BRAINS)
				{
					world->map[mapNum]->numBrains=(word)atoi(GetText());
				}
				else if(mode==LDMODE_CANDLES)
				{
					world->map[mapNum]->numCandles=(word)atoi(GetText());
				}
				else if(mode==LDMODE_ITEMDROP)
				{
					char s[8];

					strcpy(s,GetText());
					if(strchr(s,'%'))
					{
						*strchr(s,'%')='\0';
					}
					world->map[mapNum]->itemDrops=(word)(atof(s)*FIXAMT);
					if(world->map[mapNum]->itemDrops>100*FIXAMT)
						world->map[mapNum]->itemDrops=100*FIXAMT;
				}
				mode=LDMODE_NORMAL;
				LevelDialogButtons();
			}
			else if(TextDialogCommand()==TM_CANCEL)
			{
				mode=LDMODE_NORMAL;
			}
			break;
	}
	return 1;
}

byte LevelDialogRClick(int msx,int msy)
{
	switch(mode)
	{
		case LDMODE_NORMAL:
			if(asking)
			{
			}
			else
			{
				rClick=1;
				CheckButton(msx,msy,ID_SONG);
			}
			break;
	}
	return 1;
}

byte LevelDialogClick(int msx,int msy)
{
	switch(mode)
	{
		case LDMODE_NORMAL:
			if(asking)
			{
				if(PointInRect(msx,msy,320-50-15,240+40,320-50+15,240+40+15))
				{
					LevelDialogYes();
				}
				else if(PointInRect(msx,msy,320+50-15,240+40,320+50+15,240+40+15))
				{
					asking=0;
					yesNo=0;
				}
			}
			else
			{
				rClick=0;
				CheckButtons(msx,msy);

				if(PointInRect(msx,msy,DLG_X2-211,DLG_Y2-220,DLG_X2-10,DLG_Y2-211))
				{
					// slide up
					SlideClick(ID_SLIDE+0);
				}
				else if(PointInRect(msx,msy,DLG_X2-9,DLG_Y2-210,DLG_X2-2,DLG_Y2-9))
				{
					// slide right
					SlideClick(ID_SLIDE+1);
				}
				else if(PointInRect(msx,msy,DLG_X2-211,DLG_Y2-9,DLG_X2-10,DLG_Y2-2))
				{
					// slide down
					SlideClick(ID_SLIDE+2);
				}
				else if(PointInRect(msx,msy,DLG_X2-220,DLG_Y2-210,DLG_X2-212,DLG_Y2-9))
				{
					// slide left
					SlideClick(ID_SLIDE+3);
				}
				else if(PointInRect(msx,msy,DLG_X2-211,DLG_Y2-210,DLG_X2-211+200,DLG_Y2-210+200))
				{
					// jump on the map
					int x,y;

					x=(msx-(DLG_X2-211))*TILE_WIDTH;
					y=(msy-(DLG_Y2-210))*TILE_HEIGHT;
					if(x<0)
						x=0;
					if(y<0)
						y=0;
					if(x>EditorGetMap()->width*TILE_WIDTH)
						x=EditorGetMap()->width*TILE_WIDTH;
					if(y>EditorGetMap()->height*TILE_HEIGHT)
						y=EditorGetMap()->height*TILE_HEIGHT;
					PutCamera(x*FIXAMT,y*FIXAMT);
				}
			}
			break;
		case LDMODE_SONGPICK:
			FileDialogClick(msx,msy);
			if(FileDialogCommand()==FM_LOAD)
			{
				strcpy(world->map[mapNum]->song,GetFilename(""));
				ExitFileDialog();
				mode=LDMODE_NORMAL;
				LevelDialogButtons();
			}
			else if(FileDialogCommand()==FM_EXIT)
			{
				ExitFileDialog();
				mode=LDMODE_NORMAL;
				LevelDialogButtons();
			}
			break;
		case LDMODE_NAME:
		case LDMODE_WIDTH:
		case LDMODE_HEIGHT:
		case LDMODE_BRAINS:
		case LDMODE_CANDLES:
		case LDMODE_ITEMDROP:
			TextDialogClick(msx,msy);
			if(TextDialogCommand()==TM_OKAY)
			{
				if(mode==LDMODE_NAME)
					strcpy(world->map[mapNum]->name,GetText());
				else if(mode==LDMODE_WIDTH)
				{
					//resize map
					desiredWidth=(byte)atoi(GetText());
					desiredHeight=world->map[mapNum]->height;
					if(desiredWidth<20)
						desiredWidth=20;
					if(desiredWidth>MAX_MAPSIZE)
						desiredWidth=MAX_MAPSIZE;
					asking=ASK_WIDTH;
					sprintf(question,"Width %d will erase stuff! OK?",desiredWidth);
					if(desiredWidth>=world->map[mapNum]->width)
						LevelDialogYes();	// instant yes, if you're making it bigger
				}
				else if(mode==LDMODE_HEIGHT)
				{
					//resize map
					desiredWidth=world->map[mapNum]->width;
					desiredHeight=(byte)atoi(GetText());
					if(desiredHeight<20)
						desiredHeight=20;
					if(desiredHeight>MAX_MAPSIZE)
						desiredHeight=MAX_MAPSIZE;
					asking=ASK_HEIGHT;
					sprintf(question,"Height %d will erase stuff! OK?",desiredHeight);
					if(desiredHeight>=world->map[mapNum]->height)
						LevelDialogYes();	// instant yes, if you're making it bigger
				}
				else if(mode==LDMODE_BRAINS)
				{
					world->map[mapNum]->numBrains=(word)atoi(GetText());
				}
				else if(mode==LDMODE_CANDLES)
				{
					world->map[mapNum]->numCandles=(word)atoi(GetText());
				}
				else if(mode==LDMODE_ITEMDROP)
				{
					char s[8];

					strcpy(s,GetText());
					if(strchr(s,'%'))
					{
						*strchr(s,'%')='\0';
					}
					world->map[mapNum]->itemDrops=(word)(atof(s)*FIXAMT);
					if(world->map[mapNum]->itemDrops>100*FIXAMT)
						world->map[mapNum]->itemDrops=100*FIXAMT;
				}
				mode=LDMODE_NORMAL;
				LevelDialogButtons();
			}
			else if(TextDialogCommand()==TM_CANCEL)
			{
				mode=LDMODE_NORMAL;
			}
			break;
	}
	return 1;
}
