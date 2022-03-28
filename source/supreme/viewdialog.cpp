#include "viewdialog.h"
#include "editor.h"
#include "dialogbits.h"
#include "textdialog.h"

#define ID_STATIC		1
#define ID_WALL			2
#define ID_LIGHT		3
#define ID_MONSTER		4
#define ID_SPECIAL		5
#define ID_PICKUP		6
#define ID_OTHERITEM	7
#define ID_TEMPTORCH	8
#define ID_EXIT			9
#define ID_SONG			10


#define DLG_X		(0)
#define DLG_Y		(400)
#define DLG_X2		(308)
#define DLG_Y2		(480)

void ViewDialogButtons(void);
static byte songIsPlaying=0;

static void ViewItemClick(int id)
{
	byte flagNum[]={MAP_SHOWWALLS,MAP_SHOWLIGHTS,MAP_SHOWBADGUYS,
			MAP_SHOWSPECIALS,MAP_SHOWPICKUPS,MAP_SHOWOTHERITEMS,MAP_TEMPTORCH};

	ToggleDisplayFlag(flagNum[id-ID_WALL]);
	ViewDialogButtons();
}

static void ExitClick(int id)
{
	ViewMenuOff();
}

void ReturnToSong(void)
{
	if(!songIsPlaying)
		StopSong();
	else
		PlaySongForce(EditorGetMap()->song);
}

static void SongClick(int id)
{
	if(songIsPlaying)
		StopSong();
	else
		PlaySongForce(EditorGetMap()->song);
	songIsPlaying=1-songIsPlaying;
}

void ViewDialogButtons(void)
{
	ClearButtons();

	MakeButton(BTN_STATIC,ID_STATIC,0,DLG_X+2,DLG_Y+2,2,2,"View Options",NULL);

	MakeButton(BTN_CHECK,ID_WALL,((GetDisplayFlags()&MAP_SHOWWALLS)!=0)*CHECK_ON,DLG_X+2,DLG_Y+20,100,15,"Walls",ViewItemClick);
	MakeButton(BTN_CHECK,ID_LIGHT,((GetDisplayFlags()&MAP_SHOWLIGHTS)!=0)*CHECK_ON,DLG_X+2,DLG_Y+37,100,15,"Lights",ViewItemClick);
	MakeButton(BTN_CHECK,ID_MONSTER,((GetDisplayFlags()&MAP_SHOWBADGUYS)!=0)*CHECK_ON,DLG_X+2,DLG_Y+54,100,15,"Monsters",ViewItemClick);
	MakeButton(BTN_CHECK,ID_SPECIAL,((GetDisplayFlags()&MAP_SHOWSPECIALS)!=0)*CHECK_ON,DLG_X+104,DLG_Y+20,100,15,"Specials",ViewItemClick);
	MakeButton(BTN_CHECK,ID_PICKUP,((GetDisplayFlags()&MAP_SHOWPICKUPS)!=0)*CHECK_ON,DLG_X+104,DLG_Y+37,100,15,"Pick-Up Items",ViewItemClick);
	MakeButton(BTN_CHECK,ID_OTHERITEM,((GetDisplayFlags()&MAP_SHOWOTHERITEMS)!=0)*CHECK_ON,DLG_X+104,DLG_Y+54,100,15,"Other Items",ViewItemClick);
	MakeButton(BTN_CHECK,ID_TEMPTORCH,((GetDisplayFlags()&MAP_TEMPTORCH)!=0)*CHECK_ON,DLG_X+204,DLG_Y+20,100,15,"Glow Cursor",ViewItemClick);
	MakeButton(BTN_NORMAL,ID_SONG,0,DLG_X2-52,DLG_Y2-35,50,15,"Song",SongClick);
	MakeButton(BTN_NORMAL,ID_EXIT,0,DLG_X2-52,DLG_Y2-18,50,15,"Exit",ExitClick);
}

void InitViewDialog(void)
{
	ViewDialogButtons();
}

void ExitViewDialog(void)
{
	ClearButtons();
}

void RenderViewDialog(int msx,int msy,MGLDraw *mgl)
{
	char s[32];
	int x,y;
	Map *m;
	int t;

	// box for the whole dialog
	mgl->FillBox(DLG_X,DLG_Y,DLG_X2,DLG_Y2,6);
	mgl->Box(DLG_X,DLG_Y,DLG_X2,DLG_Y2,31);

	RenderButtons(msx,msy,mgl);
	EditorGetTileXY(&x,&y);
	if(x<0 || x>=EditorGetMap()->width)
		x=0;
	if(y<0 || y>=EditorGetMap()->height)
		y=0;

	m=EditorGetMap();

	if(x>=0 && y>=0 && x<m->width && y<m->height)
	{
		t=m->map[x+y*m->width].floor;
	}
	else
		t=999;

	sprintf(s,"T#%03d (%03d,%03d)",t,x,y);

	Print(DLG_X+190,DLG_Y+2,s,0,1);
	PrintSpecialComment(DLG_X+3,DLG_Y2-13,x,y);
}

byte ViewDialogClick(int msx,int msy)
{
	if(!PointInRect(msx,msy,DLG_X,DLG_Y,DLG_X2,DLG_Y2))
		return 1;

	if(GetDisplayMGL()->MouseTap())
		CheckButtons(msx,msy);

	return 0;
}
