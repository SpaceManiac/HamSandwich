#include "playlist.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "appdata.h"

enum
{
	PM_NORMAL,
	PM_SELPLAY,   // selecting in playlist
	PM_SELFILE,   // selecting in filelist
	PM_SELPLAY2,  // selecting in playlist backwards
	PM_SELFILE2,  // selecting in filelist backwards
	PM_SCROLL,    // moving scroll bar
	PM_SCROLL2,   // moving file scroll bar
};

constexpr int PLBTN_HEIGHT = 19;
constexpr int LISTSPACING = 18;

enum ButtonId : byte
{
	None,
	BTN_LIST1,
	BTN_LIST2,
	BTN_LIST3,
	BTN_LIST4,
	BTN_ADD,
	BTN_ERASE,
	BTN_EXIT,
	BTN_MOVEUP,
	BTN_MOVEDN,
	Playlist,
	AvailableSongs,
};

constexpr int NUMFILESTOGET = 16;

constexpr int SONGS_PER_PAGE = 20;

constexpr int SCROLLBAR_HEIGHT = 361 - 4;

struct playButton_t
{
	int x,y;
	int wid;
	char txt[32];
	ButtonId id;
};

static const playButton_t btn[]={
	{20,20,50,"List#1",BTN_LIST1},
	{75,20,50,"List#2",BTN_LIST2},
	{130,20,50,"List#3",BTN_LIST3},
	{185,20,50,"List#4",BTN_LIST4},

	{280,60, 80,"Add Files",BTN_ADD},
	{280,140,80,"Remove",BTN_ERASE},
	{280,220,80,"Move Up",BTN_MOVEUP},
	{280,300,80,"Move Dn",BTN_MOVEDN},
	{280,380,80,"Exit",BTN_EXIT},
};
constexpr int NUM_PLAY_BTNS = SDL_arraysize(btn);

static int listNum,listPos;
static int msx,msy;

static byte *backgd;
static std::unique_ptr<sprite_set_t> plSpr;
static char msBright,msDBright;

static int numFiles,bufferSize,filePos;
static char *fileList=NULL;
static byte mode;

static int selectMin[2],selectMax[2],scrollOfsY;
static int scrollY[2],scrollHeight[2];

static ButtonId curButton;
static bool mouseMode = true;

void SortSongs(void)
{
	byte flip;
	int i;
	char tmp[SONGNAME_LEN];

	flip=1;

	while(flip)
	{
		flip=0;
		for(i=0;i<numFiles-1;i++)
		{
			if(strcmp(&fileList[i*SONGNAME_LEN],&fileList[(i+1)*SONGNAME_LEN])>0)
			{
				strcpy(tmp,&fileList[i*SONGNAME_LEN]);
				strcpy(&fileList[i*SONGNAME_LEN],&fileList[(i+1)*SONGNAME_LEN]);
				strcpy(&fileList[(i+1)*SONGNAME_LEN],tmp);
				flip=1;
			}
		}
	}
}

void InputSong(const char *fname)
{
	if(fileList==NULL)
	{
		fileList=(char *)malloc(SONGNAME_LEN*NUMFILESTOGET);
		if(!fileList)
			FatalError("Out of memory!");
		numFiles=0;
		bufferSize=NUMFILESTOGET;
	}
	else
	{
		if(numFiles==bufferSize)
		{
			fileList=(char *)realloc(fileList,SONGNAME_LEN*(bufferSize+NUMFILESTOGET));
			if(!fileList)
				FatalError("Out of memory!");
			bufferSize+=NUMFILESTOGET;
		}
	}

	strcpy(&fileList[numFiles*SONGNAME_LEN],fname);
	numFiles++;
}

void ScanSongs(void)
{
	auto files = ListDirectory("music", ".ogg", SONGNAME_LEN);
	for (const auto& str : files)
	{
		InputSong(str.c_str());
	}
	SortSongs();
}

static void CalcScrollBar(byte b)
{
	if(b==0)
	{
		if(profile.playList[listNum].numSongs==0)
		{
			scrollY[0]=0;
			scrollHeight[0]=SCROLLBAR_HEIGHT;
		}
		else
		{
			scrollHeight[0]=SCROLLBAR_HEIGHT*SONGS_PER_PAGE/profile.playList[listNum].numSongs;
			if(scrollHeight[0]<10)
				scrollHeight[0]=10;
			if(scrollHeight[0]>SCROLLBAR_HEIGHT)
				scrollHeight[0]=SCROLLBAR_HEIGHT;
			scrollY[0]=SCROLLBAR_HEIGHT*listPos/profile.playList[listNum].numSongs;
			if(scrollY[0]+scrollHeight[0]>SCROLLBAR_HEIGHT)
				scrollY[0]=SCROLLBAR_HEIGHT-scrollHeight[0]+1;
		}
	}
	else
	{
		if(numFiles==0)
		{
			scrollY[1]=0;
			scrollHeight[1]=SCROLLBAR_HEIGHT;
		}
		else
		{
			scrollHeight[1]=SCROLLBAR_HEIGHT*SONGS_PER_PAGE/numFiles;
			if(scrollHeight[1]<10)
				scrollHeight[1]=10;
			if(scrollHeight[1]>SCROLLBAR_HEIGHT)
				scrollHeight[1]=SCROLLBAR_HEIGHT;
			scrollY[1]=SCROLLBAR_HEIGHT*filePos/numFiles;
			if(scrollY[1]+scrollHeight[1]>SCROLLBAR_HEIGHT)
				scrollY[1]=SCROLLBAR_HEIGHT-scrollHeight[1]+1;
		}
	}
}

static void ReverseCalcScrollBar(byte b)
{
	if(b==0)
	{
		if(scrollY[0]<0)
			scrollY[0]=0;
		if(scrollY[0]+scrollHeight[0]+1>SCROLLBAR_HEIGHT)
			scrollY[0]=SCROLLBAR_HEIGHT-scrollHeight[0]-1;

		if(scrollY[0]>0)
			listPos=scrollY[0]*profile.playList[listNum].numSongs/SCROLLBAR_HEIGHT+1;
		else
			listPos=0;
	}
	else
	{
		if(scrollY[1]<0)
			scrollY[1]=0;
		if(scrollY[1]+scrollHeight[1]+1>SCROLLBAR_HEIGHT)
			scrollY[1]=SCROLLBAR_HEIGHT-scrollHeight[1]-1;

		if(scrollY[1]>0)
			filePos=scrollY[1]*numFiles/SCROLLBAR_HEIGHT+1;
		else
			filePos=0;
	}
}

void InitPlayListMenu(MGLDraw *mgl)
{
	int i;

	listNum=0;
	listPos=0;
	msBright=0;
	msDBright=1;

	selectMin[0]=-1;
	selectMax[0]=-1;
	selectMin[1]=-1;
	selectMax[1]=-1;

	fileList=NULL;
	numFiles=0;
	bufferSize=0;
	filePos=0;
	mode=PM_NORMAL;

	plSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	ScanSongs();
	CalcScrollBar(0);
	CalcScrollBar(1);

	curButton = ButtonId::None;
	GetTaps(); GetArrowTaps();
}

void ExitPlayListMenu(void)
{
	free(backgd);
	plSpr.reset();
}

void AddSongs(void)
{
	int i,addSongs;

	addSongs=selectMax[1]-selectMin[1]+1;

	if(!profile.playList[listNum].song)
	{
		profile.playList[listNum].song=(char *)calloc(SONGNAME_LEN*addSongs,1);
		memcpy(profile.playList[listNum].song,
			&fileList[SONGNAME_LEN*selectMin[1]],SONGNAME_LEN*addSongs);

		profile.playList[listNum].numSongs+=addSongs;
		CalcScrollBar(0);
		return;
	}
	else
	{
		profile.playList[listNum].song=(char *)realloc(profile.playList[listNum].song,SONGNAME_LEN*(profile.playList[listNum].numSongs+addSongs));
	}
	if(!profile.playList[listNum].song)
		FatalError("Out of memory!");

	if(selectMin[0]==-1 || selectMax[0]==profile.playList[listNum].numSongs-1)
	{
		// put the new songs at the very end
		memcpy(&profile.playList[listNum].song[profile.playList[listNum].numSongs*SONGNAME_LEN],
			&fileList[SONGNAME_LEN*selectMin[1]],SONGNAME_LEN*addSongs);
	}
	else
	{
		// move all songs that will go after the new songs aside
		for(i=profile.playList[listNum].numSongs+addSongs-1;i>selectMax[0]+addSongs;i--)
		{
			memcpy(&profile.playList[listNum].song[i*SONGNAME_LEN],
				&profile.playList[listNum].song[(i-addSongs)*SONGNAME_LEN],SONGNAME_LEN);
		}
		// now copy in the new songs
		memcpy(&profile.playList[listNum].song[(selectMax[0]+1)*SONGNAME_LEN],
			&fileList[SONGNAME_LEN*selectMin[1]],SONGNAME_LEN*addSongs);
	}
	profile.playList[listNum].numSongs+=addSongs;
	CalcScrollBar(0);
}

void RemoveSongs(void)
{
	int i,killSongs;

	if(selectMin[0]==-1)
		return;

	killSongs=selectMax[0]-selectMin[0]+1;

	if(killSongs==profile.playList[listNum].numSongs)
	{
		free(profile.playList[listNum].song);
		profile.playList[listNum].song=NULL;
		profile.playList[listNum].numSongs=0;
		selectMin[0]=-1;
		selectMax[0]=-1;
		CalcScrollBar(0);
		return;
	}
	// slide back the songs past the gap-to-be
	for(i=selectMin[0];i<profile.playList[listNum].numSongs-killSongs+1;i++)
	{
		memcpy(&profile.playList[listNum].song[i*SONGNAME_LEN],
			&profile.playList[listNum].song[(i+killSongs)*SONGNAME_LEN],SONGNAME_LEN);
	}
	profile.playList[listNum].song=(char *)realloc(profile.playList[listNum].song,SONGNAME_LEN*(profile.playList[listNum].numSongs-killSongs));
	profile.playList[listNum].numSongs-=killSongs;
	if (mouseMode)
	{
		selectMin[0]=-1;
		selectMax[0]=-1;
	}
	else if (selectMin[0] > profile.playList[listNum].numSongs - killSongs)
	{
		selectMin[0] -= killSongs;
		selectMax[0] -= killSongs;
		if (selectMin[0] < 0 || selectMax[0] < 0)
		{
			selectMin[0] = -1;
			selectMax[0] = -1;
		}
	}
	CalcScrollBar(0);
}

void MoveSongs(char dir)
{
	char tmp[SONGNAME_LEN];
	int i;

	if(selectMin[0]==-1 || selectMin[0]+dir<0 || selectMax[0]+dir>=profile.playList[listNum].numSongs)
	{
		MakeNormalSound(SND_BOMBBOOM);
		return;
	}

	if(dir==-1)	// going up
	{
		memcpy(tmp,&profile.playList[listNum].song[SONGNAME_LEN*(selectMin[0]-1)],SONGNAME_LEN);
		for(i=selectMin[0]-1;i<selectMax[0];i++)
			memcpy(&profile.playList[listNum].song[SONGNAME_LEN*i],
				&profile.playList[listNum].song[SONGNAME_LEN*(i+1)],SONGNAME_LEN);
		memcpy(&profile.playList[listNum].song[SONGNAME_LEN*(selectMax[0])],tmp,SONGNAME_LEN);
	}
	else	// going down
	{
		memcpy(tmp,&profile.playList[listNum].song[SONGNAME_LEN*(selectMax[0]+1)],SONGNAME_LEN);
		for(i=selectMax[0]+1;i>selectMin[0];i--)
			memcpy(&profile.playList[listNum].song[SONGNAME_LEN*i],
				&profile.playList[listNum].song[SONGNAME_LEN*(i-1)],SONGNAME_LEN);
		memcpy(&profile.playList[listNum].song[SONGNAME_LEN*(selectMin[0])],tmp,SONGNAME_LEN);
	}
	selectMin[0]+=dir;
	selectMax[0]+=dir;
	MakeNormalSound(SND_MENUSELECT);
}

byte UpdatePlayListMenu(int *lastTime,MGLDraw *mgl)
{
	int i;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;
	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	int oldMsx = msx, oldMsy = msy;
	mgl->GetMouse(&msx,&msy);
	bool mb = mgl->MouseTap();
	if (mb || msx != oldMsx || msy != oldMsy)
	{
		mouseMode = true;
		curButton = ButtonId::None;
		for(i=0;i<NUM_PLAY_BTNS;i++)
		{
			if(PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PLBTN_HEIGHT))
			{
				curButton = btn[i].id;
			}
		}
	}

	byte taps = GetTaps() | GetArrowTaps();
	if (taps)
	{
		mode = PM_NORMAL;
		mouseMode = false;
	}

	switch(mode)
	{
		case PM_NORMAL:
			if (taps & CONTROL_UP)
			{
				switch (curButton)
				{
					case ButtonId::None:
					case ButtonId::BTN_LIST1:
					case ButtonId::BTN_LIST2:
					case ButtonId::BTN_LIST3:
					case ButtonId::BTN_LIST4:
						curButton = ButtonId::BTN_EXIT;
						break;
					case ButtonId::BTN_ADD:
						curButton = ButtonId(ButtonId::BTN_LIST1 + listNum);
						break;
					case ButtonId::BTN_ERASE:
						curButton = ButtonId::BTN_ADD;
						break;
					case ButtonId::BTN_MOVEUP:
						curButton = ButtonId::BTN_ERASE;
						break;
					case ButtonId::BTN_MOVEDN:
						curButton = ButtonId::BTN_MOVEUP;
						break;
					case ButtonId::BTN_EXIT:
						curButton = ButtonId::BTN_MOVEDN;
						break;
					case ButtonId::Playlist:
						if (selectMin[0] > 0)
						{
							selectMax[0] = selectMin[0] = selectMin[0] - 1;
						}
						if (listPos > selectMin[0])
						{
							listPos = selectMin[0];
						}
						CalcScrollBar(0);
						break;
					case ButtonId::AvailableSongs:
						if (selectMin[1] > 0)
						{
							selectMax[1] = selectMin[1] = selectMin[1] - 1;
						}
						if (filePos > selectMin[1])
						{
							filePos = selectMin[1];
						}
						CalcScrollBar(1);
						break;
				}
			}
			if (taps & CONTROL_DN)
			{
				switch (curButton)
				{
					case ButtonId::None:
					case ButtonId::BTN_LIST1:
					case ButtonId::BTN_LIST2:
					case ButtonId::BTN_LIST3:
					case ButtonId::BTN_LIST4:
						curButton = ButtonId::BTN_ADD;
						break;
					case ButtonId::BTN_ADD:
						curButton = ButtonId::BTN_ERASE;
						break;
					case ButtonId::BTN_ERASE:
						curButton = ButtonId::BTN_MOVEUP;
						break;
					case ButtonId::BTN_MOVEUP:
						curButton = ButtonId::BTN_MOVEDN;
						break;
					case ButtonId::BTN_MOVEDN:
						curButton = ButtonId::BTN_EXIT;
						break;
					case ButtonId::BTN_EXIT:
						curButton = ButtonId(ButtonId::BTN_LIST1 + listNum);
						break;
					case ButtonId::Playlist:
						if (selectMin[0] < profile.playList[listNum].numSongs)
						{
							selectMax[0] = selectMin[0] = selectMin[0] + 1;
						}
						if (listPos < selectMin[0] - SONGS_PER_PAGE + 1)
						{
							listPos = selectMin[0] - SONGS_PER_PAGE + 1;
						}
						CalcScrollBar(0);
						break;
					case ButtonId::AvailableSongs:
						if (selectMin[1] < numFiles)
						{
							selectMax[1] = selectMin[1] = selectMin[1] + 1;
						}
						if (filePos < selectMin[1] - SONGS_PER_PAGE + 1)
						{
							filePos = selectMin[1] - SONGS_PER_PAGE + 1;
						}
						CalcScrollBar(1);
						break;
				}
			}
			if (taps & CONTROL_LF)
			{
				switch (curButton)
				{
					case ButtonId::None:
					case ButtonId::BTN_ADD:
					case ButtonId::BTN_ERASE:
					case ButtonId::BTN_MOVEUP:
					case ButtonId::BTN_MOVEDN:
					case ButtonId::BTN_EXIT:
						curButton = ButtonId::Playlist;
						if (profile.playList[listNum].numSongs && (selectMax[0] == -1 || selectMin[0] == -1))
						{
							selectMax[0] = selectMin[0] = 0;
						}
						break;
					case ButtonId::BTN_LIST1:
						curButton = ButtonId::BTN_LIST4;
						break;
					case ButtonId::BTN_LIST2:
						curButton = ButtonId::BTN_LIST1;
						break;
					case ButtonId::BTN_LIST3:
						curButton = ButtonId::BTN_LIST2;
						break;
					case ButtonId::BTN_LIST4:
						curButton = ButtonId::BTN_LIST3;
						break;
					case ButtonId::AvailableSongs:
						curButton = ButtonId::BTN_ADD;
						break;
					case ButtonId::Playlist:
						break;
				}
			}
			if (taps & CONTROL_RT)
			{
				switch (curButton)
				{
					case ButtonId::None:
					case ButtonId::BTN_ADD:
					case ButtonId::BTN_ERASE:
					case ButtonId::BTN_MOVEUP:
					case ButtonId::BTN_MOVEDN:
					case ButtonId::BTN_EXIT:
						curButton = ButtonId::AvailableSongs;
						if (numFiles && (selectMax[1] == -1 || selectMin[1] == -1))
						{
							selectMax[1] = selectMin[1] = 0;
						}
						break;
					case ButtonId::BTN_LIST1:
						curButton = ButtonId::BTN_LIST2;
						break;
					case ButtonId::BTN_LIST2:
						curButton = ButtonId::BTN_LIST3;
						break;
					case ButtonId::BTN_LIST3:
						curButton = ButtonId::BTN_LIST4;
						break;
					case ButtonId::BTN_LIST4:
						curButton = ButtonId::BTN_LIST1;
						break;
					case ButtonId::Playlist:
						curButton = ButtonId::BTN_ERASE;
						break;
					case ButtonId::AvailableSongs:
						break;
				}
			}
			if (mb || (taps & CONTROL_B1))
			{
				switch (curButton)
				{
					case ButtonId::None:
						break;
					case BTN_LIST1:
					case BTN_LIST2:
					case BTN_LIST3:
					case BTN_LIST4:
						listNum=curButton-BTN_LIST1;
						listPos=0;
						selectMin[0]=-1;
						selectMax[0]=-1;
						CalcScrollBar(0);
						MakeNormalSound(SND_MENUSELECT);
						break;
					case BTN_ADD:
					case ButtonId::AvailableSongs:
						if(selectMin[1]==-1 || selectMax[1]==-1)
							MakeNormalSound(SND_BOMBBOOM);
						else
						{
							MakeNormalSound(SND_MENUSELECT);
							AddSongs();
						}
						break;
					case BTN_ERASE:
					case ButtonId::Playlist:
						if(selectMin[0]==-1 || selectMax[0]==-1)
							MakeNormalSound(SND_BOMBBOOM);
						else
						{
							MakeNormalSound(SND_MENUSELECT);
							RemoveSongs();
						}
						break;
					case BTN_MOVEUP:
						MoveSongs(-1);
						break;
					case BTN_MOVEDN:
						MoveSongs(1);
						break;
					case BTN_EXIT:
						MakeNormalSound(SND_MENUSELECT);
						return 1;
				}
			}
			if (taps & CONTROL_B2)
			{
				switch (curButton)
				{
					case ButtonId::Playlist:
						if (selectMin[0] != -1)
							PlaySongForce(&profile.playList[listNum].song[selectMin[0]*SONGNAME_LEN]);
						break;
					case ButtonId::AvailableSongs:
						if (selectMin[1] != -1)
							PlaySongForce(&fileList[selectMin[1]*SONGNAME_LEN]);
						break;
					default: break;
				}
			}

			if(mgl->RMouseTap())
			{
				if(PointInRect(msx,msy,18,48,18+232,48+360))
				{
					i=((msy-48)/LISTSPACING)+listPos;
					if(i<profile.playList[listNum].numSongs)
						PlaySongForce(&profile.playList[listNum].song[i*SONGNAME_LEN]);
				}
				if(PointInRect(msx,msy,370,48,370+232,48+360))
				{
					i=((msy-48)/LISTSPACING)+filePos;
					if(i<numFiles)
						PlaySongForce(&fileList[i*SONGNAME_LEN]);
				}
			}
			if(mb)
			{
				if(PointInRect(msx,msy,18,48,18+232,48+360))
				{
					selectMin[0]=((msy-48)/LISTSPACING)+listPos;
					selectMax[0]=selectMin[0];
					if(selectMin[0]>=profile.playList[listNum].numSongs)
					{
						selectMin[0]=-1;
						selectMax[0]=-1;
						MakeNormalSound(SND_BOMBBOOM);
					}
					else
					{
						MakeNormalSound(SND_MENUCLICK);
						mode=PM_SELPLAY;
					}
				}
				if(PointInRect(msx,msy,370,48,370+232,48+360))
				{
					selectMin[1]=((msy-48)/LISTSPACING)+filePos;
					selectMax[1]=selectMin[1];
					if(selectMin[1]>=numFiles)
					{
						selectMin[1]=-1;
						selectMax[1]=-1;
						MakeNormalSound(SND_BOMBBOOM);
					}
					else
					{
						mode=PM_SELFILE;
						MakeNormalSound(SND_MENUCLICK);
					}
				}

				if(PointInRect(msx,msy,18+234,48,18+249,48+361))
				{
					MakeNormalSound(SND_MENUCLICK);
					if(msy<scrollY[0]+48+2)
					{
						listPos-=SONGS_PER_PAGE-2;
						if(listPos<0)
							listPos=0;
						CalcScrollBar(0);
					}
					else if(msy>48+2+scrollY[0]+scrollHeight[0])
					{
						listPos+=SONGS_PER_PAGE-2;
						if(listPos>profile.playList[listNum].numSongs-SONGS_PER_PAGE)
							listPos=profile.playList[listNum].numSongs-SONGS_PER_PAGE;
						CalcScrollBar(0);
					}
					else
					{
						mode=PM_SCROLL;
						scrollOfsY=msy-scrollY[0];
					}
				}

				if(PointInRect(msx,msy,370+234,48,370+249,48+361))
				{
					MakeNormalSound(SND_MENUCLICK);
					if(msy<scrollY[1]+48+2)
					{
						filePos-=SONGS_PER_PAGE-2;
						if(filePos<0)
							filePos=0;
						CalcScrollBar(1);
					}
					else if(msy>48+2+scrollY[1]+scrollHeight[1])
					{
						filePos+=SONGS_PER_PAGE-2;
						if(filePos>numFiles-SONGS_PER_PAGE)
							filePos=numFiles-SONGS_PER_PAGE;
						CalcScrollBar(1);
					}
					else
					{
						mode=PM_SCROLL2;
						scrollOfsY=msy-scrollY[1];
					}
				}
			}
			break;
		case PM_SCROLL:
			scrollY[0]=msy-scrollOfsY;
			ReverseCalcScrollBar(0);
			if(!mgl->MouseDown())
			{
				mode=PM_NORMAL;
			}
			break;
		case PM_SCROLL2:
			scrollY[1]=msy-scrollOfsY;
			ReverseCalcScrollBar(1);
			if(!mgl->MouseDown())
			{
				mode=PM_NORMAL;
			}
			break;
		case PM_SELPLAY:
			if(!mgl->MouseDown())
			{
				mode=PM_NORMAL;
			}
			else
			{
				if(msy<=48)
				{
					if(listPos>0)
						listPos--;
					i=listPos;
					CalcScrollBar(0);
				}
				else if(msy>=48+361)
				{
					if(listPos<profile.playList[listNum].numSongs-SONGS_PER_PAGE)
						listPos++;
					i=listPos+SONGS_PER_PAGE-1;
					if(i>=profile.playList[listNum].numSongs)
						i=profile.playList[listNum].numSongs-1;
					CalcScrollBar(0);
				}
				else
					i=((msy-48)/LISTSPACING)+listPos;
				if(i>=0 && i<profile.playList[listNum].numSongs)
				{
					if(i<selectMin[0])
					{
						selectMax[0]=selectMin[0];
						selectMin[0]=i;
						mode=PM_SELPLAY2;
					}
					else
						selectMax[0]=i;
				}
			}
			break;
		case PM_SELPLAY2:
			if(!mgl->MouseDown())
			{
				mode=PM_NORMAL;
			}
			else
			{
				if(msy<=48)
				{
					if(listPos>0)
						listPos--;
					i=listPos;
					CalcScrollBar(0);
				}
				else if(msy>=48+361)
				{
					if(listPos<profile.playList[listNum].numSongs-SONGS_PER_PAGE)
						listPos++;
					i=listPos+SONGS_PER_PAGE-1;
					if(i>=profile.playList[listNum].numSongs)
						i=profile.playList[listNum].numSongs-1;
					CalcScrollBar(0);
				}
				else
					i=((msy-48)/LISTSPACING)+listPos;
				if(i>=0 && i<profile.playList[listNum].numSongs)
				{
					if(i>selectMax[0])
					{
						selectMin[0]=selectMax[0];
						selectMax[0]=i;
						mode=PM_SELPLAY;
					}
					else
						selectMin[0]=i;
				}
			}
			break;
		case PM_SELFILE:
			if(!mgl->MouseDown())
			{
				mode=PM_NORMAL;
			}
			else
			{
				if(msy<=48)
				{
					if(filePos>0)
						filePos--;
					i=filePos;
					CalcScrollBar(1);
				}
				else if(msy>=48+361)
				{
					if(filePos<numFiles-SONGS_PER_PAGE)
						filePos++;
					i=filePos+SONGS_PER_PAGE-1;
					if(i>=numFiles)
						i=numFiles-1;
					CalcScrollBar(1);
				}
				else
					i=((msy-48)/LISTSPACING)+filePos;
				if(i>=0 && i<numFiles)
				{
					if(i<selectMin[1])
					{
						selectMax[1]=selectMin[1];
						selectMin[1]=i;
						mode=PM_SELFILE2;
					}
					else
						selectMax[1]=i;
				}
			}
			break;
		case PM_SELFILE2:
			if(!mgl->MouseDown())
			{
				mode=PM_NORMAL;
			}
			else
			{
				if(msy<=48)
				{
					if(filePos>0)
						filePos--;
					i=filePos;
					CalcScrollBar(1);
				}
				else if(msy>=48+361)
				{
					if(filePos<numFiles-SONGS_PER_PAGE)
						filePos++;
					i=filePos+SONGS_PER_PAGE-1;
					if(i>=numFiles)
						i=numFiles-1;
					CalcScrollBar(1);
				}
				else
					i=((msy-48)/LISTSPACING)+filePos;
				if(i>=0 && i<numFiles)
				{
					if(i>selectMax[1])
					{
						selectMin[1]=selectMax[1];
						selectMax[1]=i;
						mode=PM_SELFILE;
					}
					else
						selectMin[1]=i;
				}
			}
			break;
	}


	return 0;
}

void RenderPlayListButton(int x,int y,int wid,const char *txt,MGLDraw *mgl, ButtonId buttonId)
{
	if(curButton == buttonId)
	{
		mgl->Box(x,y,x+wid,y+PLBTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+PLBTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+PLBTN_HEIGHT,32+16);

	PrintGlow(x+2,y+3,txt,0,2);
}

char *ShortName(char *name)
{
	static char result[128];

	strcpy(result,name);

	if(!strcasecmp(".ogg",&name[strlen(name)-4]))
	{
		result[strlen(name)-4]='\0';
	}
	return result;
}

void RenderSongList(char *list,int pos,int num,int x,int y,int selectMin,int selectMax,byte scroll,MGLDraw *mgl)
{
	int i;

	mgl->FillBox(x,y,x+232,y+361,32*1+4);
	mgl->Box(x,y,x+232,y+361,32*1+16);

	mgl->Box(x,y+361,x+232,y+380,32*1+16);

	for(i=pos;i<pos+SONGS_PER_PAGE;i++)
	{
		if(i<num)
		{
			if (mouseMode)
			{
				if(i>=selectMin && i<=selectMax)
					mgl->FillBox(x+1,y+1+(i-pos)*18,x+231,y+(i-pos)*18+18,32*1+8);
				if(PointInRect(msx,msy,x,y+(i-pos)*18,x+232,y+(i-pos)*18+18))
					mgl->Box(x+1,y+1+(i-pos)*18,x+231,y+(i-pos)*18+18,32*1+12);
			}
			else if(i>=selectMin && i<=selectMax)
			{
				if (curButton == (scroll ? ButtonId::AvailableSongs : ButtonId::Playlist))
					mgl->FillBox(x+1,y+1+(i-pos)*18,x+231,y+(i-pos)*18+18,32*1+8);
				else
					mgl->Box(x+1,y+1+(i-pos)*18,x+231,y+(i-pos)*18+18,32*1+12);
			}
			PrintGlowLimited(x+2,y+2+(i-pos)*18,x+232,ShortName(&list[i*SONGNAME_LEN]),0,2);
		}
	}

	mgl->FillBox(x+234,y,x+249,y+361,32*1+4);
	mgl->Box(x+234,y,x+249,y+361,32*1+16);

	if(mode!=PM_SCROLL+scroll)
		mgl->FillBox(x+236,y+2+scrollY[scroll],x+247,y+2+scrollY[scroll]+scrollHeight[scroll],32*1+8);
	else
		mgl->FillBox(x+236,y+2+scrollY[scroll],x+247,y+2+scrollY[scroll]+scrollHeight[scroll],32*1+16);
}

void RenderPlayListMenu(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;
	char s[32];

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	PrintGlow(520,20,"Playlist Editor",0,2);

	sprintf(s,"Playlist #%d",listNum+1);
	PrintGlow(20,410,s,0,2);
	PrintGlow(372,410,"Available Songs",0,2);

	if (mouseMode)
	{
		PrintGlow(20,432,"Right click a song to hear it. Click or drag to select, and use the buttons to",0,2);
		PrintGlow(20,450,"modify the playlist. Press ESC to exit.",0,2);
	}
	else
	{
		PrintGlow(20,432,"Press button 1 to add or remove a song. Press button 2 to hear it.",0,2);
		PrintGlow(20,450,"Use bumpers to reorder songs.",0,2);
	}

	for(i=0;i<NUM_PLAY_BTNS;i++)
		RenderPlayListButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl,btn[i].id);

	// box in the selected playlist
	mgl->Box(18+55*listNum,18,18+54+55*listNum,18+PLBTN_HEIGHT+4,32*1+20);

	// play list
	RenderSongList(profile.playList[listNum].song,listPos,profile.playList[listNum].numSongs,
		18,48,selectMin[0],selectMax[0],0,mgl);

	// file list
	RenderSongList(fileList,filePos,numFiles,370,48,selectMin[1],selectMax[1],1,mgl);

	if (mouseMode)
	{
		SetSpriteConstraints(13,13,627,467);
		msx2=msx;
		msy2=msy;
		if(msx2<13)
			msx2=13;
		if(msy2<13)
			msy2=13;
		if(msx2>622)
			msx2=622;
		if(msy2>462)
			msy2=462;
		plSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
		ClearSpriteConstraints();
	}
}

//----------------

TASK(void) PlayListMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitPlayListMenu(mgl);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdatePlayListMenu(&lastTime,mgl);
		RenderPlayListMenu(mgl);
		AWAIT mgl->Flip();

		if(mgl->LastKeyPressed()==27)
			done=1;

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitPlayListMenu();
}
