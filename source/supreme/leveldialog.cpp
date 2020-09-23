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
#define LDMODE_TIMER	8	// typing in seconds

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
#define ID_TIMER		12
#define ID_SLIDE		20
#define ID_FLAG			40
#define ID_PREV			80
#define ID_NEXT			81

#define DLG_X		(16)
#define DLG_Y		(16)
#define DLG_X2		(640-16)
#define DLG_Y2		(480-16)
#define MAP_X		(DLG_X2-311)
#define MAP_Y		(DLG_Y2-311)

static byte asking,yesNo;
static char question[64];
static byte rClick;

static byte mapNum,mode;
static world_t *world;

static word flagNum[]={MAP_SNOWING,MAP_RAIN,MAP_HUB,MAP_SECRET,MAP_TORCHLIT,MAP_WELLLIT,
				MAP_STARS,MAP_UNDERWATER,MAP_LAVA,MAP_STEALTH,MAP_WAVY,MAP_OXYGEN,MAP_TIMER,MAP_DYWTR,MAP_DYLVA,MAP_SHOWV0};
static char flagName[][32]={
	"Snowing",
	"Raining",
	"Hub Level",
	"Secret Level",
	"Torch Lit",
	"Lantern Lit",
	"Star Background",
	"Underwater",
	"Underlava",
	"Stealth",
	"Dumb Side",
	"Oxygen Meter",
	"Timed Level",
	"Dynamic Water",
	"Dynamic Lava",
	"Var 0 Bar",
};

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
		InitFileDialog("music/*.ogg",FM_LOAD|FM_EXIT|FM_PLAYSONGS,world->map[mapNum]->song);
	}
}

static void FlagClick(int id)
{
	if(flagNum[id-ID_FLAG]==MAP_HUB && mapNum==0)
		return;	// can't unhub level zero

	world->map[mapNum]->flags^=flagNum[id-ID_FLAG];
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
	InitTextDialog("Enter level width (4-254):","",3);
}

static void HeightClick(int id)
{
	mode=LDMODE_HEIGHT;
	InitTextDialog("Enter level height (4-254)","",3);
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

static void TimerClick(int id)
{
	mode=LDMODE_TIMER;
	InitTextDialog("Enter # of seconds on timer:","",3);
}

static void ItemDropClick(int id)
{
	mode=LDMODE_ITEMDROP;
	InitTextDialog("Enter chance for enemies to drop items:","",5);
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
		if(m->badguy[i].type==MONS_ZOMBIE || m->badguy[i].type==MONS_ZOMBONI || m->badguy[i].type==MONS_MUTANT || m->badguy[i].type==MONS_BOMBIE || m->badguy[i].type==MONS_ZOMBIELORD || m->badguy[i].type==MONS_BOMBIELORD || m->badguy[i].type==MONS_FROZOMBIE|| m->badguy[i].type==MONS_FLYINGZOMBIE)
			m->numBrains++;
		if(m->badguy[i].type==MONS_SUPERZOMBIE || m->badguy[i].type==MONS_SUMUZOMBIE || m->badguy[i].type==MONS_SUPRBOMBIE)
			m->numBrains+=2;
		if(m->badguy[i].type && BrainsGiven(m->badguy[i].item)>0)
			m->numBrains+=BrainsGiven(m->badguy[i].item);
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
					if(BrainsGiven(m->special[i].effect[j].value2)>0)
						m->numBrains+=BrainsGiven(m->special[i].effect[j].value2);
					if(m->special[i].effect[j].value==MONS_ZOMBIE || m->special[i].effect[j].value==MONS_ZOMBONI ||
						m->special[i].effect[j].value==MONS_MUTANT)
						m->numBrains++;
					if(m->special[i].effect[j].value==MONS_SUPERZOMBIE)
						m->numBrains+=2;
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
			m->numCandles+=CandlesGiven(m->badguy[i].item);
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
					m->numCandles+=CandlesGiven(m->special[i].effect[j].value2);
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
			150,15,flagName[i],FlagClick);
	}

	MakeButton(BTN_NORMAL,ID_PREV,0,DLG_X2-104,DLG_Y+2,50,15,"Prev",PrevClick);
	MakeButton(BTN_NORMAL,ID_NEXT,0,DLG_X2-52,DLG_Y+2,50,15,"Next",NextClick);

	MakeButton(BTN_NORMAL,ID_WIDTH,0,DLG_X2-310,DLG_Y2-342,50,15,"Width",WidthClick);
	sprintf(s,"%d",world->map[mapNum]->width);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X2-310+54,DLG_Y2-342,50,15,s,NULL);

	MakeButton(BTN_NORMAL,ID_WIDTH,0,DLG_X2-310+108,DLG_Y2-342,50,15,"Height",HeightClick);
	sprintf(s,"%d",world->map[mapNum]->height);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X2-310+162,DLG_Y2-342,50,15,s,NULL);

	MakeButton(BTN_NORMAL,ID_ITEMDROP,0,DLG_X+2,DLG_Y+262+72,75,15,"Item Drops",ItemDropClick);
	sprintf(s,"%0.2f%%",(float)world->map[mapNum]->itemDrops/(float)FIXAMT);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+81,DLG_Y+262+72,40,15,s,NULL);

	MakeButton(BTN_NORMAL,ID_BRAINS,0,DLG_X+2,DLG_Y+280+72,55,15,"Brains",BrainsClick);
	sprintf(s,"%d",world->map[mapNum]->numBrains);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+59,DLG_Y+280+72,40,15,s,NULL);
	MakeButton(BTN_NORMAL,ID_AUTOBRAIN,0,DLG_X+110,DLG_Y+280+72,40,15,"Auto",AutoBrainsClick);
	MakeButton(BTN_NORMAL,ID_CANDLES,0,DLG_X+2,DLG_Y+298+72,55,15,"Candles",CandlesClick);
	sprintf(s,"%d",world->map[mapNum]->numCandles);
	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+59,DLG_Y+298+72,40,15,s,NULL);
	MakeButton(BTN_NORMAL,ID_AUTOCANDLE,0,DLG_X+110,DLG_Y+298+72,40,15,"Auto",AutoCandlesClick);
	//MakeButton(BTN_NORMAL,ID_BRAINS,0,DLG_X+2,DLG_Y+316+72,55,15,"Timer",TimerClick);
	//sprintf(s,"%d",world->map[mapNum]->numSeconds);
	//MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+59,DLG_Y+316+72,40,15,s,NULL);
}

byte ZoomTileColor(int x,int y)
{
	mapTile_t *m;
	int i;

	m=&world->map[mapNum]->map[x+y*world->map[mapNum]->width];

	for(i=0;i<MAX_MAPMONS;i++)
		if (world->map[mapNum]->badguy[i].type &&
			world->map[mapNum]->badguy[i].x==x &&
			world->map[mapNum]->badguy[i].y==y && 
			(world->map[mapNum]->badguy[i].type==MONS_BOUAPHA||world->map[mapNum]->badguy[i].type==MONS_BUNNY||world->map[mapNum]->badguy[i].type==MONS_FOLLOWBUNNY||world->map[mapNum]->badguy[i].type==MONS_FRIENDLY2||world->map[mapNum]->badguy[i].type==MONS_GOODROBOT||
			world->map[mapNum]->badguy[i].type==MONS_GOODBONE||world->map[mapNum]->badguy[i].type==MONS_GOODROBOT2||world->map[mapNum]->badguy[i].type==MONS_WIZARD||world->map[mapNum]->badguy[i].type==MONS_PUNKBUNNY||world->map[mapNum]->badguy[i].type==MONS_MINECART||
			world->map[mapNum]->badguy[i].type==MONS_RAFT||world->map[mapNum]->badguy[i].type==MONS_LOG||world->map[mapNum]->badguy[i].type==MONS_YERFDOG||
			world->map[mapNum]->badguy[i].type==MONS_GOLEM))
		{
			return (1*32)+16;	// bright green=goodguy
		}
		else if(world->map[mapNum]->badguy[i].type &&
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
			return (8);	// grey=solid floor tile
		if(world->terrain[m->floor].flags&TF_WATER)
			return (32*3)+8;	// blue=water
		if(world->terrain[m->floor].flags&TF_LAVA)
			return (32*4)+8;	// dark red=lava
		if(world->terrain[m->floor].flags&TF_ICE)
			return (32*7)+10;	// aqua=ice
		if(world->terrain[m->floor].flags&TF_RUBBER)
			return (32*7)+10;	// pink = bouncy
		if(world->terrain[m->floor].flags&TF_MINECART)
			return (32*2)+5;	// dark brown=mine path
		if(world->terrain[m->floor].flags&TF_BUNNY)
			return (32*6)+10;	// light brown=bunny path
		if(world->terrain[m->floor].flags&TF_SPACE)
			return (32*0)+24;	// light grey=space
		if(world->terrain[m->floor].flags&TF_PROPULSE)
			return (32*3)+16;	// light blue=propulsion
		if(world->terrain[m->floor].flags&TF_QUICKS)
			return (32*5)+16;	// dark yellow=quicksand

		return (32*1)+6;	// green=floor
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
	delete mapZoom;
	delete levelSpr;
}

void RenderLevelDialog(int msx,int msy,MGLDraw *mgl)
{
	int i;

	// box for the whole dialog
	mgl->FillBox(DLG_X,DLG_Y,DLG_X2,DLG_Y2,69);
	mgl->Box(DLG_X,DLG_Y,DLG_X2,DLG_Y2,31);

	for(i=0;i<MAX_MAPSIZE;i++)
	{
		memcpy((byte *)(mgl->GetScreen()+mgl->GetWidth()*(DLG_Y2+i-310)+DLG_X2-310),
			&mapZoom[i*MAX_MAPSIZE],MAX_MAPSIZE);
	}
	//310,310
	levelSpr->GetSprite(4)->Draw(MAP_X,MAP_Y-8,mgl);
	levelSpr->GetSprite(5)->Draw(MAP_X-8+257+8,MAP_Y,mgl);
	levelSpr->GetSprite(6)->Draw(MAP_X,MAP_Y+257,mgl);
	levelSpr->GetSprite(7)->Draw(MAP_X-8,MAP_Y,mgl);
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
				mgl->FillBox(320-140,240-10,320+140,240+60,69);
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
		case LDMODE_TIMER:
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
		case LDMODE_TIMER:
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
					if(desiredWidth<4)
						desiredWidth=4;
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
					if(desiredHeight<4)
						desiredHeight=4;
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

				if(PointInRect(msx,msy,MAP_X,MAP_Y-8,MAP_X+257,MAP_Y))
				{
					// slide up
					SlideClick(ID_SLIDE+0);
					MakeNormalSound(SND_MENUCLICK);
				}
				else if(PointInRect(msx,msy,MAP_X+257,MAP_Y,MAP_X+257+8,MAP_Y+257))
				{
					// slide right
					SlideClick(ID_SLIDE+1);
					MakeNormalSound(SND_MENUCLICK);
				}
				else if(PointInRect(msx,msy,MAP_X,MAP_Y+257,MAP_X+257,MAP_Y+257+8))
				{
					// slide down
					SlideClick(ID_SLIDE+2);
					MakeNormalSound(SND_MENUCLICK);
				}
				else if(PointInRect(msx,msy,MAP_X-8,MAP_Y,MAP_X,MAP_Y+257))
				{
					// slide left
					SlideClick(ID_SLIDE+3);
					MakeNormalSound(SND_MENUCLICK);
				}
				else if(PointInRect(msx,msy,MAP_X,MAP_Y,MAP_X+255,MAP_Y+255))
				{
					// jump on the map
					int x,y;

					x=(msx-(MAP_X))*TILE_WIDTH;
					y=(msy-(MAP_Y))*TILE_HEIGHT;
					if(x<0)
						x=0;
					if(y<0)
						y=0;
					if(x>EditorGetMap()->width*TILE_WIDTH)
						x=EditorGetMap()->width*TILE_WIDTH;
					if(y>EditorGetMap()->height*TILE_HEIGHT)
						y=EditorGetMap()->height*TILE_HEIGHT;
					PutCamera(x*FIXAMT,y*FIXAMT);
					MakeNormalSound(SND_TELEPORT);
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
		case LDMODE_TIMER:
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
					if(desiredWidth<4)
						desiredWidth=4;
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
					if(desiredHeight<4)
						desiredHeight=4;
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
