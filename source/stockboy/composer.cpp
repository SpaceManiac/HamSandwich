#include "composer.h"
#include "profile.h"
#include "options.h"
#include "clock.h"
#include "game.h"
#include "title.h"
#include "sound.h"
#include "music.h"
#include "editmenu.h"
#include "appdata.h"

#define VOLUME_FACTOR	128

// button types
#define BTN_SML		0	// uses verdana font, surrounded by a box
#define BTN_BIG		1	// uses impact font in white outline, very significant
#define BTN_BOX		2	// a background box for button sets
#define BTN_ARROWUP 3
#define BTN_ARROWDN 4
#define BTN_ARROWLF 5
#define BTN_ARROWRT 6

//  button IDs
#define	BTN_FILE	0
#define BTN_SETUP	1
#define BTN_EXIT	2
#define BTN_SONGNAME 3
#define BTN_AUTHNAME 4

#define BOX_GENERAL	 5	// box containing general options
#define BTN_PLAYMODE 6
#define BTN_SCROLL	 7
#define BTN_RECORD	 8
#define BTN_ZOOM	 9
#define BTN_TEMPO	 10

#define BOX_SONG	 11
#define BTN_NEWSEQ	 12
#define BTN_DELSEQ	 13
#define BTN_CLONESEQ 14
#define BTN_MOVEUP	 15
#define BTN_MOVEDN	 16
#define BTN_SONGZAP  17
#define BTN_SONGCUT	 18
#define BTN_SONGPASTE 19
#define BTN_SONGCOVER 20

#define BOX_SEQ		21
#define BTN_SEQNAME 22
#define BTN_MUTE	23
#define BTN_INSTRUMENT 24
#define BTN_VOLUME	25
#define BTN_SUSTAIN	26
#define BTN_SEQZAP	27
#define BTN_SEQCUT	28
#define BTN_SEQPASTE 29
#define BTN_SEQCOVER 30
#define BTN_PLAY	 31
#define NUM_BUTTONS	 32

typedef struct button_t
{
	byte id;
	byte type;
	int x,y,width,height;
	char txt[32];
} button_t;

// composer modes
#define CM_NORMAL	0
#define CM_TITLE	1	// typing in title
#define CM_AUTHOR	2	// typing in author
#define CM_FILE		3
#define CM_SETUP	4
#define CM_YESNO	5
#define CM_EXIT		6
#define CM_SEQNAME	7

// music play modes
#define MP_SONG		0
#define MP_SEQ		1
#define MP_SONGSEL  2
#define MP_SEQSEL   3

// live record mode
#define REC_OFF		0
#define REC_FREE	1
#define REC_EIGHTH	2
#define REC_FOURTH	3
#define REC_HALF	4

// cursor modes
#define CURSOR_NORMAL	 0
#define CURSOR_SONGPASTE 1
#define CURSOR_SONGCOVER 2
#define CURSOR_SEQPASTE	 3
#define CURSOR_SEQCOVER	 4

#define NOTEHEI		5
#define PLAYHEI		10
#define SONGY		45
#define SONGX		100
#define SONGX2		640

#define SEQX		0
#define SEQY		240
#define SEQX2		640
#define BARHEIGHT	10	// the height of the little bars above and below the sequence
						// for setting playback and selecting, and showing beats, etc
#define SEQSFIT		17
#define BTNY		340

button_t btn[NUM_BUTTONS]={
	{BTN_FILE,BTN_BIG,5,450,55,28,"File"},
	{BTN_SETUP,BTN_BIG,80,450,110,28,"Setup Music"},
	{BTN_EXIT,BTN_BIG,590,450,40,28,"Exit"},
	{BTN_SONGNAME,BTN_BIG,5,1,300,28,"Title: Nosong"},
	{BTN_AUTHNAME,BTN_BIG,320,1,300,28,"Author: Nobody"},

	{BOX_GENERAL, BTN_BOX,3,BTNY,168,95,"General Options"},
	{BTN_PLAYMODE,BTN_SML,7,BTNY+7+17*0,160,15,"Play: Whole Song"},
	{BTN_SCROLL,  BTN_SML,7,BTNY+7+17*1,160,15,"Scroll While Playing"},
	{BTN_RECORD,  BTN_SML,7,BTNY+7+17*2,160,15,"Record: Free"},
	{BTN_ZOOM,    BTN_SML,7,BTNY+7+17*3,160,15,"Zoom: 50%"},
	{BTN_TEMPO,   BTN_SML,7,BTNY+7+17*4,160,15,"Tempo: 450"},

	{BOX_SONG,BTN_BOX,190,BTNY,214,95,"Song Edit"},
	{BTN_NEWSEQ,   BTN_SML,194,BTNY+7+17*0,100,15,"New Seq"},
	{BTN_DELSEQ,   BTN_SML,194,BTNY+7+17*1,100,15,"Delete Seq"},
	{BTN_CLONESEQ, BTN_SML,194,BTNY+7+17*2,100,15,"Clone Seq"},
	{BTN_MOVEUP,   BTN_SML,194,BTNY+7+17*3,100,15,"Move Up"},
	{BTN_MOVEDN,   BTN_SML,194,BTNY+7+17*4,100,15,"Move Down"},
	{BTN_SONGZAP,  BTN_SML,300,BTNY+7+17*0,100,15,"Zap"},
	{BTN_SONGCUT,  BTN_SML,300,BTNY+7+17*1,100,15,"Cut"},
	{BTN_SONGPASTE,BTN_SML,300,BTNY+7+17*2,100,15,"Paste"},
	{BTN_SONGCOVER,BTN_SML,300,BTNY+7+17*3,100,15,"Cover"},

	{BOX_SEQ,BTN_BOX,423,BTNY,214,95,"Seq Edit"},
	{BTN_SEQNAME,   BTN_SML,427,BTNY+7+17*0,100,15,"Name"},
	{BTN_MUTE,      BTN_SML,427,BTNY+7+17*1,100,15,"Mute: Off"},
	{BTN_INSTRUMENT,BTN_SML,427,BTNY+7+17*2,100,15,"Ins: Piano"},
	{BTN_VOLUME,    BTN_SML,427,BTNY+7+17*3,100,15,"Volume: 100%"},
	{BTN_SUSTAIN,   BTN_SML,427,BTNY+7+17*4,100,15,"Sustain: Off"},
	{BTN_SEQZAP,    BTN_SML,533,BTNY+7+17*0,100,15,"Zap"},
	{BTN_SEQCUT,    BTN_SML,533,BTNY+7+17*1,100,15,"Cut"},
	{BTN_SEQPASTE,  BTN_SML,533,BTNY+7+17*2,100,15,"Paste"},
	{BTN_SEQCOVER,  BTN_SML,533,BTNY+7+17*3,100,15,"Cover"},

	{BTN_PLAY,BTN_BIG,210,450,60,28,"Play"},
};

static byte cursor,cursMode,cursModeChgClicked;
static byte oldc;
static dword oldBtn;
static sprite_set_t *compSpr;
static byte oldMouseBtn,oldMouseBtn2;
static byte *backScr;
static int oldmsx,oldmsy;
static int msx,msy;
static int selectMin,selectMax,selectStart;
static int songSelMin,songSelMax,songSelStart;
static byte mode,txtBlink;
static char txtEnter[32];
static char curFname[32];

static int songPos,seqPos;
static byte insUsing;
static byte plopping,plopSeq;
static byte playing,selecting;
static byte lastJump;
static int curSeq,playSeq;

static char *filename;
static char *songname;
static char *author;
static int numSongs,maxFiles;
static int filePos,selectedFile;
static byte askingFor;

static byte playMode,dispMode,recMode,scroll;
static byte lightUp,lightUpTime;

static int  songY;
static byte noteWid,setupSel;
static int screenSize,scrollSize;

static char insName[NUM_INSTRUMENTS][16]={
	"Piano",
	"Drums",
	"Ac.Bass",
	"Marimba",
	"SlapBass",
	"Beep",
	"Choir",
	"Guitar",
	"Funky",
	"VibPhone",
	"Strings",
};

void ScanForSongs(void);
void ClearFileNames(void);
void SetupButtonText(void);

void Song_Zap(void);
void Song_Cut(void);
void Song_Paste(int dst);
void Song_Cover(int dst);
void Seq_Zap(void);
void Seq_Cut(void);
void Seq_Paste(int dst);
void Seq_Cover(int dst);

void InitComposer(void)
{
	int i;

	JamulSoundPurge();	// each level, that should be good
	oldc=255;
	cursor=0;
	oldMouseBtn=1;
	oldMouseBtn2=1;

	LoadOptions();

	compSpr=new sprite_set_t("graphics/giftshop.jsp");

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	GetDisplayMGL()->LoadBMP("graphics/giftshop.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);

	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);

	songPos=0;
	songY=0;
	seqPos=0;
	filePos=0;
	selectedFile=-1;
	insUsing=0;
	playing=0;
	selectMin=-1;
	songSelMin=-1;
	selecting=0;
	lastJump=3;
	txtBlink=0;

	Music_Stop();
	Music_New();

	mode=CM_NORMAL;
	filename=NULL;
	songname=NULL;
	author=NULL;
	numSongs=0;
	maxFiles=0;

	playMode=MP_SONG;
	dispMode=0;
	scroll=1;
	recMode=REC_FREE;
	lightUp=0;
	lightUpTime=0;
	setupSel=0;

	noteWid=5;
	screenSize=(640/noteWid);
	scrollSize=(10-noteWid)*8+16;
	ScanForSongs();
	strcpy(curFname,"newsong.sng");
	curSeq=0;
	playSeq=-1;
	cursModeChgClicked=0;
	cursMode=CURSOR_NORMAL;
	Music_SetPlaySeq(-1);
	SetupButtonText();
	SetVolumes(profile.sound,255);
}

void ExitComposer(void)
{
	ClearFileNames();
	Music_Stop();
	SaveOptions();
	ResetClock(0);
	free(backScr);
	SetVolumes(profile.sound,profile.music);
	delete compSpr;
}

void NewCurSeq(void)
{
	seqPos=0;
	selectMin=-1;
	MakeNormalSound(SND_MENUCLICK);
}

void SetupButtonText(void)
{
	switch(playMode)
	{
		case MP_SONG:
			strcpy(btn[BTN_PLAYMODE].txt,"Play: Whole Song");
			break;
		case MP_SEQ:
			strcpy(btn[BTN_PLAYMODE].txt,"Play: Sequence");
			break;
		case MP_SONGSEL:
			strcpy(btn[BTN_PLAYMODE].txt,"Play: Song Selection");
			break;
		case MP_SEQSEL:
			strcpy(btn[BTN_PLAYMODE].txt,"Play: Seq. Selection");
			break;
	}
	if(scroll)
		strcpy(btn[BTN_SCROLL].txt,"Scroll While Playing");
	else
		strcpy(btn[BTN_SCROLL].txt,"Don't Scroll");
	switch(recMode)
	{
		case REC_OFF:
			strcpy(btn[BTN_RECORD].txt,"Record: Off");
			break;
		case REC_FREE:
			strcpy(btn[BTN_RECORD].txt,"Record: Free");
			break;
		case REC_EIGHTH:
			strcpy(btn[BTN_RECORD].txt,"Record: 1/8 Notes");
			break;
		case REC_FOURTH:
			strcpy(btn[BTN_RECORD].txt,"Record: 1/4 Notes");
			break;
		case REC_HALF:
			strcpy(btn[BTN_RECORD].txt,"Record: 1/2 Notes");
			break;
	}
	switch(noteWid)
	{
		case 3:
			strcpy(btn[BTN_ZOOM].txt,"Zoom: 25%");
			break;
		case 5:
			strcpy(btn[BTN_ZOOM].txt,"Zoom: 50%");
			break;
		case 10:
			strcpy(btn[BTN_ZOOM].txt,"Zoom: 100%");
			break;
	}
	sprintf(btn[BTN_TEMPO].txt,"Tempo: %d",510-Music_GetSong()->tempo);
	sprintf(btn[BTN_SONGNAME].txt,"Title: %s",Music_GetTitle());
	sprintf(btn[BTN_AUTHNAME].txt,"Author: %s",Music_GetAuthor());

	sprintf(btn[BTN_INSTRUMENT].txt,"Ins: %s",insName[Music_GetSong()->seq[curSeq].instrument]);
	if(Music_GetSong()->seq[curSeq].insFlags&SND_SUSTAIN)
		strcpy(btn[BTN_SUSTAIN].txt,"Sustain: On");
	else
		strcpy(btn[BTN_SUSTAIN].txt,"Sustain: Off");
	if(Music_IsMuted(curSeq))
		strcpy(btn[BTN_MUTE].txt,"Mute: On");
	else
		strcpy(btn[BTN_MUTE].txt,"Mute: Off");
	sprintf(btn[BTN_VOLUME].txt,"Volume: %d%%",((VOLUME_FACTOR*10+Music_GetSong()->seq[curSeq].volume)/VOLUME_FACTOR)*10);

	if(playing)
		strcpy(btn[BTN_PLAY].txt,"Stop");
	else
		strcpy(btn[BTN_PLAY].txt,"Play");
}

void ClearFileNames(void)
{
	if(filename)
		free(filename);
	if(songname)
		free(songname);
	if(author)
		free(author);

	author=NULL;
	filename=NULL;
	songname=NULL;
}

void ScanForSongs(void)
{
	int i;
	char tmp[FNAMELEN];
	byte flip;

	numSongs=0;
	maxFiles=32;

	if(filename)
	{
		if(selectedFile!=-1)
			strcpy(tmp,&filename[FNAMELEN*selectedFile]);
		free(filename);
	}
	else
		tmp[0]='\0';
	if(songname)
		free(songname);
	if(author)
		free(author);

	filename=(char *)calloc(FNAMELEN*maxFiles,1);
	songname=(char *)calloc(FNAMELEN*maxFiles,1);
	author=(char *)calloc(FNAMELEN*maxFiles,1);

	for (const auto& file : ListDirectory("music", ".sng", FNAMELEN))
	{
		SDL_utf8strlcpy(&filename[numSongs*FNAMELEN],file.c_str(),FNAMELEN);
		Music_GetSongName(&filename[numSongs*FNAMELEN],&songname[numSongs*FNAMELEN],
						&author[numSongs*FNAMELEN]);
		numSongs++;

		if(numSongs==maxFiles)
		{
			maxFiles+=32;
			filename=(char *)realloc(filename,FNAMELEN*maxFiles);
			songname=(char *)realloc(songname,FNAMELEN*maxFiles);
			author=(char *)realloc(author,FNAMELEN*maxFiles);
			// clear the future space
			memset(&filename[numSongs*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numSongs);
			memset(&songname[numSongs*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numSongs);
			memset(&author[numSongs*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numSongs);
		}
	}

	if(selectedFile>=numSongs)
		selectedFile=-1;
	if(filePos>=numSongs)
		filePos=0;

	if(tmp[0]!='\0')
	{
		for(i=0;i<numSongs-1;i++)
			if(!strcmp(tmp,&filename[i*FNAMELEN]))
				selectedFile=i;
	}

	// sort them
	flip=1;
	while(flip)
	{
		flip=0;
		for(i=0;i<numSongs-1;i++)
		{
			if(strcmp(&filename[i*FNAMELEN],&filename[(i+1)*FNAMELEN])>0)
			{
				flip=1;
				strcpy(tmp,&filename[i*FNAMELEN]);
				strcpy(&filename[i*FNAMELEN],&filename[(i+1)*FNAMELEN]);
				strcpy(&filename[(i+1)*FNAMELEN],tmp);
				strcpy(tmp,&songname[i*FNAMELEN]);
				strcpy(&songname[i*FNAMELEN],&songname[(i+1)*FNAMELEN]);
				strcpy(&songname[(i+1)*FNAMELEN],tmp);
				strcpy(tmp,&author[i*FNAMELEN]);
				strcpy(&author[i*FNAMELEN],&author[(i+1)*FNAMELEN]);
				strcpy(&author[(i+1)*FNAMELEN],tmp);
				if(i==selectedFile)
					selectedFile++;
				else if((i+1)==selectedFile)
					selectedFile--;
			}
		}
	}
}

static void ButtonClick(int num,byte id)
{
	switch(id)
	{
		case BTN_FILE:
			mode=CM_FILE;
			ScanForSongs();
			strcpy(txtEnter,curFname);
			txtBlink=0;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_SETUP:
			mode=CM_SETUP;
			ScanForSongs();
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_EXIT:
			mode=CM_YESNO;
			askingFor=3;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_SONGNAME:
			mode=CM_TITLE;
			strcpy(txtEnter,Music_GetTitle());
			txtBlink=0;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_AUTHNAME:
			mode=CM_AUTHOR;
			strcpy(txtEnter,Music_GetAuthor());
			txtBlink=0;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_PLAYMODE:
			playMode++;
			if(playMode>MP_SEQSEL)
				playMode=0;
			Music_Stop();
			playing=0;
			if(playMode==MP_SONG || playMode==MP_SONGSEL)
			{
				playSeq=-1;
				Music_SetPlaySeq(-1);
			}
			else
			{
				playSeq=curSeq;
				Music_SetPlaySeq(curSeq);
			}
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_SCROLL:
			scroll=1-scroll;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_RECORD:
			recMode++;
			if(recMode>REC_HALF)
				recMode=0;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_ZOOM:
			if(noteWid==3)
				noteWid=5;
			else if(noteWid==5)
				noteWid=10;
			else if(noteWid==10)
				noteWid=3;
			screenSize=640/noteWid;
			scrollSize=screenSize/2;
			if(songPos>SONG_LENGTH-screenSize-1)
				songPos=SONG_LENGTH-screenSize-1;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_TEMPO:
			Music_GetSong()->tempo-=10;
			if(Music_GetSong()->tempo>500 || Music_GetSong()->tempo==0)
				Music_GetSong()->tempo=500;
			if(playing)
			{
				Music_Pause();
				Music_Play();
			}
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_NEWSEQ:
			if(Music_AddSeq())
				MakeNormalSound(SND_MENUSELECT);
			else
				MakeNormalSound(SND_MENUCANCEL);
			break;
		case BTN_DELSEQ:
			mode=CM_YESNO;
			askingFor=5;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_CLONESEQ:
			if(Music_CloneSeq(curSeq))
				MakeNormalSound(SND_MENUSELECT);
			else
				MakeNormalSound(SND_MENUCANCEL);
			break;
		case BTN_MOVEUP:
			if(Music_MoveSeq(curSeq,-1))
			{
				curSeq--;
				MakeNormalSound(SND_MENUSELECT);
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
			break;
		case BTN_MOVEDN:
			if(Music_MoveSeq(curSeq,1))
			{
				curSeq++;
				MakeNormalSound(SND_MENUSELECT);
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
			break;
		case BTN_SONGZAP:
			Song_Zap();
			break;
		case BTN_SONGCUT:
			Song_Cut();
			break;
		case BTN_SONGPASTE:
			if(cursMode==CURSOR_SONGPASTE)
				cursMode=CURSOR_NORMAL;
			else
				cursMode=CURSOR_SONGPASTE;
			cursModeChgClicked=1;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_SONGCOVER:
			if(cursMode==CURSOR_SONGCOVER)
				cursMode=CURSOR_NORMAL;
			else
				cursMode=CURSOR_SONGCOVER;
			cursModeChgClicked=1;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_SEQNAME:
			mode=CM_SEQNAME;
			strcpy(txtEnter,Music_GetSong()->seq[curSeq].name);
			txtBlink=0;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_MUTE:
			Music_Mute(curSeq,1-Music_IsMuted(curSeq));
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_INSTRUMENT:
			Music_GetSong()->seq[curSeq].instrument++;
			if(Music_GetSong()->seq[curSeq].instrument>=NUM_INSTRUMENTS)
				Music_GetSong()->seq[curSeq].instrument=0;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_VOLUME:
			Music_GetSong()->seq[curSeq].volume+=VOLUME_FACTOR;
			if(Music_GetSong()->seq[curSeq].volume>0)
				Music_GetSong()->seq[curSeq].volume=-VOLUME_FACTOR*10;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_SUSTAIN:
			Music_GetSong()->seq[curSeq].insFlags^=SND_SUSTAIN;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_SEQZAP:
			Seq_Zap();
			break;
		case BTN_SEQCUT:
			Seq_Cut();
			break;
		case BTN_SEQPASTE:
			if(cursMode==CURSOR_SEQPASTE)
				cursMode=CURSOR_NORMAL;
			else
				cursMode=CURSOR_SEQPASTE;
			cursModeChgClicked=1;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_SEQCOVER:
			if(cursMode==CURSOR_SEQCOVER)
				cursMode=CURSOR_NORMAL;
			else
				cursMode=CURSOR_SEQCOVER;
			cursModeChgClicked=1;
			MakeNormalSound(SND_MENUSELECT);
			break;
		case BTN_PLAY:
			if(playing)
				Music_Pause();
			else
				Music_Play();
			playing=1-playing;
			SetupButtonText();
			break;
		//case BTN_HELP:
		//	ShellExecute(NULL,"open","help/compose.html","","",SW_SHOWNORMAL);
		//	break;
	}
}

static void ButtonRightClick(int num,byte id)
{
	switch(id)
	{
		case BTN_PLAYMODE:
			playMode--;
			if(playMode>MP_SEQSEL)
				playMode=MP_SEQSEL;
			Music_Stop();
			playing=0;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_SCROLL:
			scroll=1-scroll;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_RECORD:
			recMode--;
			if(recMode>REC_HALF)
				recMode=REC_HALF;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_ZOOM:
			if(noteWid==3)
				noteWid=10;
			else if(noteWid==5)
				noteWid=3;
			else if(noteWid==10)
				noteWid=5;
			screenSize=640/noteWid;
			scrollSize=screenSize/2;
			if(songPos>SONG_LENGTH-screenSize-1)
				songPos=SONG_LENGTH-screenSize-1;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_TEMPO:
			Music_GetSong()->tempo+=10;
			if(Music_GetSong()->tempo>500)
				Music_GetSong()->tempo=10;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			if(playing)
			{
				Music_Pause();
				Music_Play();
			}
			break;
		case BTN_INSTRUMENT:
			Music_GetSong()->seq[curSeq].instrument--;
			if(Music_GetSong()->seq[curSeq].instrument>=NUM_INSTRUMENTS)
				Music_GetSong()->seq[curSeq].instrument=NUM_INSTRUMENTS-1;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
		case BTN_VOLUME:
			Music_GetSong()->seq[curSeq].volume-=VOLUME_FACTOR;
			if(Music_GetSong()->seq[curSeq].volume<-VOLUME_FACTOR*10)
				Music_GetSong()->seq[curSeq].volume=0;
			MakeNormalSound(SND_MENUSELECT);
			SetupButtonText();
			break;
	}
}

static void ButtonCheck(void)
{
	int i;

	for(i=0;i<NUM_BUTTONS;i++)
	{
		if(msx>=btn[i].x && msx<btn[i].x+btn[i].width &&
		   msy>=btn[i].y && msy<btn[i].y+btn[i].height)
		   ButtonClick(i,btn[i].id);
	}
}

static void ButtonRightCheck(void)
{
	int i;

	for(i=0;i<NUM_BUTTONS;i++)
	{
		if(msx>=btn[i].x && msx<btn[i].x+btn[i].width &&
		   msy>=btn[i].y && msy<btn[i].y+btn[i].height)
		   ButtonRightClick(i,btn[i].id);
	}
}

static byte SongLeftClick(byte tap,MGLDraw *mgl)
{
	int i,j;
	int numSeqs;

	numSeqs=Music_GetSong()->numSeqs-songY;
	if(numSeqs>SEQSFIT)
		numSeqs=SEQSFIT;

	if(cursMode==CURSOR_SEQPASTE || cursMode==CURSOR_SEQCOVER)
		return 0;

	if(cursMode==CURSOR_SONGPASTE || cursMode==CURSOR_SONGCOVER)
	{
		if(msx>=SONGX && msy>=SONGY-BARHEIGHT && msy<=SONGY+numSeqs*PLAYHEI+BARHEIGHT)
		{
			i=(msx-SONGX)/noteWid+songPos;
			if(cursMode==CURSOR_SONGPASTE)
				Song_Paste(i);
			else
				Song_Cover(i);
			cursMode=CURSOR_NORMAL;
		}
		return 0;
	}

	if(msy>=SONGY && msy<SONGY+numSeqs*PLAYHEI)
	{
		if(msx<SONGX && tap)
		{
			curSeq=(msy-SONGY)/PLAYHEI+songY;
			songY=curSeq-SEQSFIT/2;

			if(Music_GetSong()->numSeqs-songY<SEQSFIT)
				songY=Music_GetSong()->numSeqs-SEQSFIT;

			if(songY<0)
				songY=0;
			SetupButtonText();
			NewCurSeq();
		}
		else if(msx>=SONGX)
		{
			i=(msy-SONGY)/PLAYHEI+songY;
			j=(msx-SONGX)/noteWid+songPos;
			if(tap)
			{
				if(Music_GetSong()->seq[i].play[j]==SP_PLAY)
				{
					Music_GetSong()->seq[i].play[j]=SP_NONE;
					plopping=1;
					plopSeq=i;
					MakeNormalSound(SND_MENUCANCEL);
					Music_SetLengths();
				}
				else
				{
					Music_GetSong()->seq[i].play[j]=SP_PLAY;
					plopping=2;
					plopSeq=i;
					MakeNormalSound(SND_MENUSELECT);
					Music_SetLengths();
				}
			}
			else
			{
				if(plopping==1)
				{
					Music_GetSong()->seq[plopSeq].play[j]=SP_NONE;
					Music_SetLengths();
				}
				else if(plopping==2)
				{
					Music_GetSong()->seq[plopSeq].play[j]=SP_PLAY;
					Music_SetLengths();
				}
			}
		}
	}
	if(msy>=SONGY+numSeqs*PLAYHEI && msy<=SONGY+numSeqs*PLAYHEI+BARHEIGHT && msx>=SONGX && tap)
	{
		songSelMin=(msx-SONGX)/noteWid+songPos;
		songSelMax=(msx-SONGX)/noteWid+songPos;
		songSelStart=(msx-SONGX)/noteWid+songPos;
		selecting=2;
	}
	if(msy>=SONGY-BARHEIGHT && msy<SONGY && msx>=SONGX && tap)
		Music_Scan(((msx-SONGX)/noteWid+songPos)*SEQ_BLOCK_SIZE,-1);

	return 0;
}

static byte SongRightClick(byte tap,MGLDraw *mgl)
{
	int i,j;
	int numSeqs;

	numSeqs=Music_GetSong()->numSeqs-songY;
	if(numSeqs>SEQSFIT)
		numSeqs=SEQSFIT;

	if(msy>=SONGY && msy<SONGY+numSeqs*PLAYHEI)
	{
		if(tap)
		{
			i=(msy-SONGY)/PLAYHEI+songY;
			j=(msx-SONGX)/noteWid+songPos;
			if(Music_GetSong()->seq[i].play[j]==SP_STOP)
				Music_GetSong()->seq[i].play[j]=SP_NONE;
			else
				Music_GetSong()->seq[i].play[j]=SP_STOP;
			Music_SetLengths();
		}
	}
	if(msy>=SONGY+numSeqs*PLAYHEI && msy<=SONGY+BARHEIGHT+numSeqs*PLAYHEI && tap)
	{
		if(msx>SONGX)
		{
			songSelMin=-1;
			MakeNormalSound(SND_MENUCANCEL);
		}
	}

	return 0;
}

static byte SeqLeftClick(MGLDraw *mgl)
{
	song_t *song;
	int i;

	song=Music_GetSong();

	if(cursMode==CURSOR_SONGPASTE || cursMode==CURSOR_SONGCOVER)
		return 0;

	if(cursMode==CURSOR_SEQPASTE || cursMode==CURSOR_SEQCOVER)
	{
		if(msy>=SEQY-BARHEIGHT && msy<=SEQY+NUM_PITCHES*NOTEHEI+BARHEIGHT)
		{
			i=msx/noteWid+seqPos;
			if(cursMode==CURSOR_SEQPASTE)
				Seq_Paste(i);
			else
				Seq_Cover(i);
			cursMode=CURSOR_NORMAL;
		}
		return 0;
	}

	if(msy>=SEQY && msy<SEQY+NUM_PITCHES*NOTEHEI)
	{
		song->seq[curSeq].note[NUM_PITCHES-1-(msy-SEQY)/NOTEHEI][(msx/noteWid)+seqPos]=1-
			song->seq[curSeq].note[NUM_PITCHES-1-(msy-SEQY)/NOTEHEI][(msx/noteWid)+seqPos];
		if(song->seq[curSeq].note[NUM_PITCHES-1-(msy-SEQY)/NOTEHEI][(msx/noteWid)+seqPos] &&
			!playing)
			Music_PlayNote(NUM_PITCHES-1-(msy-SEQY)/NOTEHEI,song->seq[curSeq].instrument,0,0);
		Music_SetLengths();
	}
	if(msy>=SEQY-BARHEIGHT && msy<SEQY)
		Music_Scan(msx/noteWid+seqPos,curSeq);

	if(msy>=SEQY+NUM_PITCHES*NOTEHEI && msy<=SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI)
	{
		selectMin=msx/noteWid+seqPos;
		selectMax=msx/noteWid+seqPos;
		selectStart=msx/noteWid+seqPos;
		selecting=1;
	}

	return 0;
}

static byte SeqRightClick(MGLDraw *mgl)
{
	if(msy>=SEQY+NUM_PITCHES*NOTEHEI && msy<=SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI)
	{
		selectMin=-1;	// unselect
		MakeNormalSound(SND_MENUCANCEL);
	}
	return 0;
}

static byte ComposerMouseCheck(MGLDraw *mgl)
{
	byte b,b2;
	byte oldcursor;
	int i;
	int scrSize;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();
	b2=mgl->RMouseDown();

	i=Music_GetSong()->numSeqs-songY;
	if(i>SEQSFIT)
		i=SEQSFIT;
	if(msx<=6 && msy>=SONGY-BARHEIGHT && msy<=SONGY+i*PLAYHEI+BARHEIGHT)
	{
		if(msx>=0)
			songPos-=(7-msx);
		else
			songPos-=8;
		if(songPos<0)
			songPos=0;
	}
	if(msx>=640-7 && msy>=SONGY-BARHEIGHT && msy<=SONGY+i*PLAYHEI+BARHEIGHT)
	{
		scrSize=screenSize-(SONGX/noteWid);
		if(msx<640)
			songPos+=msx-(640-8);
		else
			songPos+=8;
		if(songPos>SONG_LENGTH-scrSize-1)
			songPos=SONG_LENGTH-scrSize-1;
	}

	if(msx<=6 && msy>=SEQY-BARHEIGHT && msy<=SEQY+NUM_PITCHES*NOTEHEI+BARHEIGHT)
	{
		if(msx>=0)
			seqPos-=(7-msx);
		else
			seqPos-=8;
		if(seqPos<0)
			seqPos=0;
	}
	if(msx>=640-7 && msy>=SEQY-BARHEIGHT && msy<=SEQY+NUM_PITCHES*NOTEHEI+BARHEIGHT)
	{
		if(msx<640)
			seqPos+=msx-(640-8);
		else
			seqPos+=8;
		if(seqPos>SEQ_LENGTH-screenSize-1)
			seqPos=SEQ_LENGTH-screenSize-1;
	}

	oldcursor=cursor;

	if(cursMode>=CURSOR_SONGPASTE && cursMode<=CURSOR_SEQCOVER && b2)
	{
		MakeNormalSound(SND_MENUCANCEL);
		cursMode=CURSOR_NORMAL;
		return 0;
	}

	if(msx!=oldmsx || msy!=oldmsy)
	{
	}
	if(!b)	// not clicking
	{
		oldMouseBtn=b;
		selecting=0;
		plopping=0;
	}
	else	// mouse is down!
	{
		if(selecting)
		{
			if(selecting==2 && msx>=SONGX)
			{
				i=(msx-SONGX)/noteWid+songPos;
				if(i<=songSelStart)
				{
					songSelMin=i;
					songSelMax=songSelStart;
				}
				else if(i>songSelStart)
				{
					songSelMax=i;
					songSelMin=songSelStart;
				}
			}
			else if(selecting==1)
			{
				i=msx/noteWid+seqPos;
				if(i<=selectStart)
				{
					selectMin=i;
					selectMax=selectStart;
				}
				else if(i>selectStart)
				{
					selectMax=i;
					selectMin=selectStart;
				}
			}
			return 0;
		}
		if(!oldMouseBtn) // only taps allowed here
		{
			SongLeftClick(1,mgl);
			SeqLeftClick(mgl);
			ButtonCheck();
		}
		else
			SongLeftClick(0,mgl);
	}
	if(!b2)	// not clicking right button
	{
		oldMouseBtn2=b2;
	}
	else	// right mouse is down!
	{
		if(!oldMouseBtn2)
		{
			SongRightClick(1,mgl);
			SeqRightClick(mgl);
			ButtonRightCheck();
		}
		else
			SongRightClick(0,mgl);
	}

	if(b && (!oldMouseBtn) && !cursModeChgClicked)	// if you clicked at all, then remove any special cursor modes
		cursMode=CURSOR_NORMAL;

	cursModeChgClicked=0;
	oldMouseBtn=b;
	oldMouseBtn2=b2;
	return 0;
}

static byte TypingMouseCheck(MGLDraw *mgl)
{
	byte b,b2;
	byte oldcursor;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();
	b2=mgl->RMouseDown();

	oldcursor=cursor;

	if(msx!=oldmsx || msy!=oldmsy)
	{
	}
	if(!b)	// not clicking
	{
		oldMouseBtn=b;
		selecting=0;
	}
	else	// mouse is down!
	{
		if(!oldMouseBtn)
		{
			if(msx>=210-30 && msx<=210+30 && msy>270 && msy<290)
			{
				// accept
				if(mode==CM_TITLE)
					strcpy(Music_GetTitle(),txtEnter);
				else if(mode==CM_AUTHOR)
					strcpy(Music_GetAuthor(),txtEnter);
				else
					strcpy(Music_GetSong()->seq[curSeq].name,txtEnter);

				SetupButtonText();
				mode=CM_NORMAL;
				MakeNormalSound(SND_MENUSELECT);
			}
			if(msx>=429-30 && msx<=429+30 && msy>270 && msy<290)
			{
				// cancel
				mode=CM_NORMAL;
				MakeNormalSound(SND_MENUSELECT);
			}
		}
	}
	if(!b2)	// not clicking right button
	{
		oldMouseBtn2=b2;
	}
	else	// right mouse is down!
	{
	}

	oldMouseBtn=b;
	oldMouseBtn2=b2;
	return 0;
}

static byte FileMouseCheck(MGLDraw *mgl)
{
	byte b,b2;
	byte oldcursor;
	int i;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();
	b2=mgl->RMouseDown();

	oldcursor=cursor;

	if(msx!=oldmsx || msy!=oldmsy)
	{
	}
	if(!b)	// not clicking
	{
		oldMouseBtn=b;
		selecting=0;
	}
	else	// mouse is down!
	{
		if(msx>=25 && msx<=320 && msy>=70 && msy<70+20*12)
		{
			i=(msy-70)/20+filePos;
			if(i<numSongs)
			{
				if(i!=selectedFile)
					MakeNormalSound(SND_MENUCLICK);
				selectedFile=i;
				strcpy(txtEnter,&filename[i*FNAMELEN]);
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)+1]='\0';
					txtEnter[strlen(txtEnter)]='<';
				}
			}
		}
		if(!oldMouseBtn)
		{
			if(msx>=30 && msx<=70 && msy>=340 && msy<=360)
			{
				// load
				mode=CM_YESNO;
				askingFor=1;
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)-1]='\0';
				}
				txtBlink=0;
				strcpy(curFname,txtEnter);
				MakeNormalSound(SND_MENUSELECT);
			}
			if(msx>=90 && msx<=130 && msy>=340 && msy<=360)
			{
				// save
				mode=CM_YESNO;
				askingFor=2;
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)-1]='\0';
				}
				txtBlink=0;
				strcpy(curFname,txtEnter);
				MakeNormalSound(SND_MENUSELECT);
			}
			if(msx>=150 && msx<=190 && msy>=340 && msy<=360)
			{
				// new
				mode=CM_YESNO;
				askingFor=4;
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)-1]='\0';
				}
				txtBlink=0;
				strcpy(curFname,txtEnter);
				MakeNormalSound(SND_MENUSELECT);
			}
			if(msx>=586 && msx<=616 && msy>=340 && msy<=360)
			{
				// exit
				mode=CM_NORMAL;
				MakeNormalSound(SND_MENUSELECT);
			}
			if(msx>=322 && msx<=332 && msy>=70 && msy<=190)
			{
				// up scroll
				if(filePos>0)
					filePos-=10;
				if(filePos<0)
					filePos=0;
			}
			if(msx>=322 && msx<=332 && msy>=191 && msy<=309)
			{
				// down scroll
				filePos+=10;
				if(filePos>numSongs-12)
					filePos=numSongs-12;
				if(filePos<0)
					filePos=0;
			}
		}
	}
	if(!b2)	// not clicking right button
	{
		oldMouseBtn2=b2;
	}
	else	// right mouse is down!
	{
	}

	oldMouseBtn=b;
	oldMouseBtn2=b2;
	return 0;
}

static byte SetupMouseCheck(MGLDraw *mgl)
{
	byte b,b2;
	byte oldcursor;
	int i;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();
	b2=mgl->RMouseDown();

	oldcursor=cursor;

	if(msx!=oldmsx || msy!=oldmsy)
	{
	}
	if(!b)	// not clicking
	{
		oldMouseBtn=b;
		selecting=0;
	}
	else	// mouse is down!
	{
		if(msx>=25 && msx<=320 && msy>=80 && msy<80+20*12)
		{
			i=(msy-80)/20+filePos;
			if(i<numSongs)
			{
				if(i!=selectedFile)
					MakeNormalSound(SND_MENUSELECT);
				selectedFile=i;
				strcpy(profile.songChoice[setupSel],&filename[selectedFile*FNAMELEN]);
			}
		}
		if(!oldMouseBtn)
		{
			if(msx>=586 && msx<=616 && msy>=340 && msy<=360)
			{
				// exit
				mode=CM_NORMAL;
				MakeNormalSound(SND_MENUSELECT);
				SaveProfile(opt.curProfile);
			}
			if(msx>=322 && msx<=332 && msy>=80 && msy<=200)
			{
				// up scroll
				if(filePos>0)
					filePos-=10;
				if(filePos<0)
					filePos=0;
			}
			if(msx>=322 && msx<=332 && msy>=201 && msy<=319)
			{
				// down scroll
				filePos+=10;
				if(filePos>numSongs-12)
					filePos=numSongs-12;
				if(filePos<0)
					filePos=0;
			}
			for(i=0;i<NUM_SONGS;i++)
			{
				if(msx>=340 && msx<=610 && msy>=80+i*25 && msy<=80+23+i*25)
				{
					setupSel=i;
					MakeNormalSound(SND_MENUCLICK);
				}
			}
		}
	}
	if(!b2)	// not clicking right button
	{
		oldMouseBtn2=b2;
	}
	else	// right mouse is down!
	{
		if(!oldMouseBtn2)
		{
			for(i=0;i<NUM_SONGS;i++)
			{
				if(msx>=340 && msx<=610 && msy>=80+i*25 && msy<=80+23+i*25)
				{
					setupSel=i;
					profile.songChoice[i][0]='\0';
					MakeNormalSound(SND_MENUCANCEL);
				}
			}
		}
	}

	oldMouseBtn=b;
	oldMouseBtn2=b2;
	return 0;
}

static void YesNoYes(void)
{
	switch(askingFor)
	{
		case 1:	// load
			Music_Stop();
			Music_Load(curFname);
			curSeq=0;
			songY=0;
			playing=0;
			SetupButtonText();
			mode=CM_NORMAL;
			break;
		case 2:	// save
			Music_Save(curFname);
			mode=CM_NORMAL;
			break;
		case 3:	// exit
			mode=CM_EXIT;
			break;
		case 4:	// new
			Music_New();
			curSeq=0;
			SetupButtonText();
			mode=CM_NORMAL;
			break;
		case 5:
			Music_DelSeq(curSeq);
			while(curSeq>=Music_GetSong()->numSeqs)
				curSeq--;
			SetupButtonText();
			mode=CM_NORMAL;
			break;
	}
}

static byte YesNoMouseCheck(MGLDraw *mgl)
{
	byte b,b2;
	byte oldcursor;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();
	b2=mgl->RMouseDown();

	oldcursor=cursor;

	if(msx!=oldmsx || msy!=oldmsy)
	{
	}
	if(!b)	// not clicking
	{
		oldMouseBtn=b;
		selecting=0;
	}
	else	// mouse is down!
	{
		if(!oldMouseBtn)
		{
			if(msx>=210-30 && msx<=210+30 && msy>270 && msy<290)
			{
				// accept
				YesNoYes();
				MakeNormalSound(SND_MENUSELECT);
			}
			if(msx>=429-30 && msx<=429+30 && msy>270 && msy<290)
			{
				// cancel
				mode=CM_NORMAL;
				MakeNormalSound(SND_MENUCANCEL);
			}
		}
	}
	if(!b2)	// not clicking right button
	{
		oldMouseBtn2=b2;
	}
	else	// right mouse is down!
	{
	}

	oldMouseBtn=b;
	oldMouseBtn2=b2;
	return 0;
}

void Song_Zap(void)
{
	int i,j;
	song_t *song;

	if(songSelMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	song=Music_GetSong();
	for(i=songSelMin;i<=songSelMax;i++)
	{
		for(j=0;j<song->numSeqs;j++)
			song->seq[j].play[i]=0;
	}
	Music_SetLengths();
	MakeNormalSound(SND_BOMBBOOM2);
}

void Song_Cut(void)
{
	if(songSelMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	Music_Shift(-1,songSelMax+1,-(songSelMax-songSelMin+1));
	Music_SetLengths();
	MakeNormalSound(SND_BOMBBOOM);
}

void Song_Paste(int dst)
{
	int wid,i,j;
	song_t *song;

	if(songSelMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	song=Music_GetSong();

	wid=(songSelMax-songSelMin+1);
	if(songSelMin>dst && songSelMax+wid>=SONG_LENGTH)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	if(songSelMin<dst && songSelMax>dst)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	// make room for the new plays
	Music_Shift(-1,dst,wid);
	if(songSelMin>dst)
	{
		songSelMin+=wid;
		songSelMax+=wid;
	}
	for(j=0;j<song->numSeqs;j++)
		for(i=0;i<wid;i++)
		{
			song->seq[j].play[dst+i]=song->seq[j].play[songSelMin+i];
		}
	Music_SetLengths();
	MakeNormalSound(SND_BOMBDROP);
}

void Song_Cover(int dst)
{
	int wid,i,j;
	song_t *song;

	if(songSelMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	song=Music_GetSong();

	wid=(songSelMax-songSelMin+1);
	if(songSelMin>dst && songSelMax+wid>=SONG_LENGTH)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	if(songSelMin<dst && songSelMax>dst)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	for(j=0;j<song->numSeqs;j++)
		for(i=0;i<wid;i++)
		{
			song->seq[j].play[dst+i]=song->seq[j].play[songSelMin+i];
		}
	Music_SetLengths();
	MakeNormalSound(SND_STARGO);
}

void Seq_Zap(void)
{
	int i,j;
	song_t *song;

	if(selectMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	song=Music_GetSong();
	for(i=selectMin;i<=selectMax;i++)
	{
		for(j=0;j<NUM_PITCHES;j++)
			song->seq[curSeq].note[j][i]=0;
	}
	Music_SetLengths();
	MakeNormalSound(SND_BOMBBOOM2);
}

void Seq_Cut(void)
{
	if(selectMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	Music_Shift(curSeq,selectMax+1,-(selectMax-selectMin+1));
	Music_SetLengths();
	MakeNormalSound(SND_BOMBBOOM);
}

void Seq_Paste(int dst)
{
	int wid,i,j;
	song_t *song;

	if(selectMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	song=Music_GetSong();
	wid=(selectMax-selectMin+1);
	if(selectMin>dst && selectMax+wid>=SEQ_LENGTH)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	if(selectMin<dst && selectMax>dst)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	// make room for the new notes
	Music_Shift(curSeq,dst,wid);
	if(selectMin>dst)
	{
		selectMin+=wid;
		selectMax+=wid;
	}
	for(j=0;j<NUM_PITCHES;j++)
		for(i=0;i<wid;i++)
		{
			song->seq[curSeq].note[j][dst+i]=song->seq[curSeq].note[j][selectMin+i];
		}
	Music_SetLengths();
	MakeNormalSound(SND_BOMBDROP);
}

void Seq_Cover(int dst)
{
	int wid,i,j;
	song_t *song;

	if(selectMin==-1)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	song=Music_GetSong();
	wid=(selectMax-selectMin+1);
	if(selectMin>dst && selectMax+wid>=SEQ_LENGTH)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}
	if(selectMin<dst && selectMax>dst)
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;
	}

	for(j=0;j<NUM_PITCHES;j++)
		for(i=0;i<wid;i++)
		{
			song->seq[curSeq].note[j][dst+i]=song->seq[curSeq].note[j][selectMin+i];
		}
	Music_SetLengths();
	MakeNormalSound(SND_STARGO);
}

void RecordNote(byte note)
{
	int j;

	if(playing && recMode>REC_OFF)
	{
		j=Music_GetPos(curSeq);
		switch(recMode)
		{
			case REC_FREE:
				// don't quantize
				break;
			case REC_EIGHTH:
				j=(j/2)*2;
				break;
			case REC_FOURTH:
				j=((j+2)/4)*4;
				break;
			case REC_HALF:
				j=((j+4)/8)*8;
				break;
		}
		Music_GetSong()->seq[curSeq].note[note][j]=1;
		if(Music_GetPos(curSeq)>j)	// don't play if it'll cause a double note
			Music_PlayNote(note,Music_GetSong()->seq[curSeq].instrument,Music_GetSong()->seq[curSeq].volume,((Music_GetSong()->seq[curSeq].insFlags&SND_SUSTAIN)>0));
	}
	else	// if not recording, you can play all you want
		Music_PlayNote(note,Music_GetSong()->seq[curSeq].instrument,Music_GetSong()->seq[curSeq].volume,((Music_GetSong()->seq[curSeq].insFlags&SND_SUSTAIN)>0));

	lightUp=NUM_PITCHES-note-1;
	lightUpTime=8;
}

byte UpdateComposer(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;
	char pianoKey[NUM_PITCHES]={
		'q','w','e','r','t','y','u','i',
		'1','2','3','4','5','6','7'};
	int i;
	int scrSize;
	int pos;

	scrSize=screenSize-(SONGX/noteWid);

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;
	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		if(lightUpTime)
			lightUpTime--;

		if(playing)
		{
			pos=Music_GetPos(playSeq);
			if(playSeq==-1)
				pos/=SEQ_BLOCK_SIZE;

			if(playMode==MP_SONGSEL)
			{
				if(songSelMin==-1)
					Music_SetLoopSpot(0,0);
				else
					Music_SetLoopSpot(songSelMin*SEQ_BLOCK_SIZE,(songSelMax+1)*SEQ_BLOCK_SIZE);
			}
			else if(playMode==MP_SEQSEL)
			{
				if(selectMin==-1)
					Music_SetLoopSpot(0,0);
				else
					Music_SetLoopSpot(selectMin,(selectMax+1));
			}

			if(scroll)
			{
				if(playSeq==-1)
				{
					songPos=pos-scrSize/2;
					if(songPos<0)
						songPos=0;
				}
				else
				{
					seqPos=pos-scrSize/2;
					if(seqPos<0)
						seqPos=0;
				}
			}
		}
		Music_Update();

		c=mgl->LastKeyPressed();
		c2=GetArrows()|GetControls();

		switch(mode)
		{
			case CM_NORMAL:
				if(ComposerMouseCheck(mgl))
					return 1;

				if(c2&CONTROL_LF)
				{
					if(c2&CONTROL_B1)
					{
						seqPos-=scrollSize;
						if(seqPos<0)
							seqPos=0;
					}
					else if(seqPos>1)
						seqPos-=2;
					else
						seqPos=0;
				}
				if(c2&CONTROL_RT)
				{
					if(c2&CONTROL_B1)
					{
						seqPos+=scrollSize;

						if(seqPos>SEQ_LENGTH-screenSize-1)
							seqPos=SEQ_LENGTH-screenSize-1;
					}
					else if(seqPos<SEQ_LENGTH-screenSize-2)
						seqPos+=2;
					else
						seqPos=SEQ_LENGTH-screenSize-1;
				}

				if((c2&CONTROL_UP) && !(oldc&CONTROL_UP))
				{
					if(curSeq>0)
						curSeq--;
					else
						curSeq=Music_GetSong()->numSeqs-1;
					NewCurSeq();
				}
				if((c2&CONTROL_DN) && !(oldc&CONTROL_DN))
				{
					if(curSeq<Music_GetSong()->numSeqs-1)
						curSeq++;
					else
						curSeq=0;

					NewCurSeq();
				}

				if(c==' ')
				{
					if(playing)
						Music_Pause();
					else
						Music_Play();
					playing=1-playing;
					SetupButtonText();
				}

				if(c==27)
				{
					mode=CM_YESNO;
					askingFor=3;
					return 0;
				}
				if(c=='Z')	// ZAP selection
					Song_Zap();
				if(c=='X')	// CUT selection
					Song_Cut();
				if(c=='V')	// PASTE selection
				{
					if(cursMode==CURSOR_SONGPASTE)
						cursMode=CURSOR_NORMAL;
					else
						cursMode=CURSOR_SONGPASTE;
				}
				if(c=='C')	// COVER with selection
				{
					if(cursMode==CURSOR_SONGCOVER)
						cursMode=CURSOR_NORMAL;
					else
						cursMode=CURSOR_SONGCOVER;
				}
				if(c=='z')	// ZAP selection
					Seq_Zap();
				if(c=='x')	// CUT selection
					Seq_Cut();
				if(c=='v')	// PASTE selection
				{
					if(cursMode==CURSOR_SEQPASTE)
						cursMode=CURSOR_NORMAL;
					else
						cursMode=CURSOR_SEQPASTE;
				}
				if(c=='c')	// COVER with selection
				{
					if(cursMode==CURSOR_SEQCOVER)
						cursMode=CURSOR_NORMAL;
					else
						cursMode=CURSOR_SEQCOVER;
				}
				for(i=0;i<NUM_PITCHES;i++)
				{
					if(c==pianoKey[i])
					{
						RecordNote(i);
					}
				}
				break;
			case CM_SETUP:
				if(SetupMouseCheck(mgl))
					return 1;

				if(c==27)
				{
					mode=CM_NORMAL;
					SaveProfile(opt.curProfile);
				}
				break;
			case CM_TITLE:
			case CM_AUTHOR:
			case CM_SEQNAME:
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)-1]='\0';
				}
				txtBlink=1-txtBlink;

				TypingMouseCheck(mgl);

				if(c==27)
				{
					mode=CM_NORMAL;
				}
				else if(c==13)
				{
					if(mode==CM_TITLE)
						strcpy(Music_GetTitle(),txtEnter);
					else if(mode==CM_AUTHOR)
						strcpy(Music_GetAuthor(),txtEnter);
					else
						strcpy(Music_GetSong()->seq[curSeq].name,txtEnter);
					SetupButtonText();
					mode=CM_NORMAL;
				}
				else if(c==8)
				{
					if(strlen(txtEnter)>0)
						txtEnter[strlen(txtEnter)-1]='\0';
				}
				else
				{
					// character to be typed
					if(strlen(txtEnter)<30)
					{
						txtEnter[strlen(txtEnter)+1]='\0';
						txtEnter[strlen(txtEnter)]=c;
					}
				}
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)+1]='\0';
					txtEnter[strlen(txtEnter)]='<';
				}
				break;
			case CM_FILE:
				FileMouseCheck(mgl);
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)-1]='\0';
				}
				txtBlink=1-txtBlink;

				if(c==27)
				{
					mode=CM_NORMAL;
				}
				else if(c==13)
				{
					mode=CM_NORMAL;
				}
				else if(c==8)
				{
					if(strlen(txtEnter)>0)
						txtEnter[strlen(txtEnter)-1]='\0';
				}
				else
				{
					// character to be typed
					if(strlen(txtEnter)<30)
					{
						txtEnter[strlen(txtEnter)+1]='\0';
						txtEnter[strlen(txtEnter)]=c;
					}
				}
				if(txtBlink)
				{
					txtEnter[strlen(txtEnter)+1]='\0';
					txtEnter[strlen(txtEnter)]='<';
				}
				break;
			case CM_YESNO:
				YesNoMouseCheck(mgl);

				if(c==27)
				{
					mode=CM_NORMAL;
				}
				else if(c=='n' || c=='N')
				{
					mode=CM_NORMAL;
				}
				else if(c=='y' || c=='Y')
				{
					YesNoYes();
				}
				break;
		}

		oldmsx=msx;
		oldmsy=msy;
		oldc=c2;

		*lastTime-=TIME_PER_FRAME;
	}
	JamulSoundUpdate();


	return 0;
}

void RenderMusicStaff(MGLDraw *mgl)
{
	//char notename[]={"CBAGFEDCBAGFEDC"};
	int i,j,x;
	char s[64];
	word len;
	word pos;
	song_t *song;

	song=Music_GetSong();

	x=SEQX;
	// light up the centerline, middle C
	mgl->FillBox(SEQX,SEQY+((NUM_PITCHES/2)*NOTEHEI),SEQX2-1,SEQY+NOTEHEI-1+((NUM_PITCHES/2)*NOTEHEI),90);

	len=Music_GetLength(curSeq)*SEQ_BLOCK_SIZE;
	pos=Music_GetPos(curSeq);

	// draw in the upper empty bar for setting playpos and endpoint
	mgl->FillBox(SEQX,SEQY-BARHEIGHT,SEQX2-1,SEQY-BARHEIGHT,0);
	// and the lower bar
	mgl->FillBox(SEQX,SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI,SEQX2,SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI,0);

	/*
	s[1]='\0';
	for(i=0;i<NUM_PITCHES;i++)
	{
		s[0]=notename[i];
		Print(1,SEQY+NOTEHEI*i,s,-32,1);
	}
	*/
	// now fill in the sequence
	for(i=seqPos;i<seqPos+screenSize;i++)
	{
		// if this is selected, put a selectblock at the bottom
		if(selectMin!=-1 && i>=selectMin && i<=selectMax)
			mgl->FillBox(x+1,SEQY+NUM_PITCHES*NOTEHEI,x+noteWid-1,SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI,32*3+20);

		// display beats as needed
		if((i%16)==0)
		{
			if(dispMode==0)	// show beats
				sprintf(s,"%d",i/16);
			else
			{
				sprintf(s,"%d:%02d.%d",(i/30)/60,(i/30)%60,((i*10)/30)%10);
			}
			CenterPrint(x+noteWid/2,SEQY+NUM_PITCHES*NOTEHEI,s,-32,1);
			// fill in this column lighter so it shows
			mgl->FillBox(x+1,SEQY,x+noteWid-1,SEQY+NUM_PITCHES*NOTEHEI,92);
		}
		// twice as often, put in slightly less light columns
		else if((i%8)==0)
		{
			mgl->FillBox(x+1,SEQY,x+noteWid-1,SEQY+NUM_PITCHES*NOTEHEI,90);
		}

		// if this is the song endpoint, mark it
		if(i==len)
		{
			CenterPrint(x+noteWid/2,SEQY-BARHEIGHT,"END",-32,1);
			mgl->FillBox(x+1,SEQY,x+noteWid-1,SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI,0);
		}
		// if this is the current play point, show that
		if(i==pos)
		{
			mgl->FillBox(x+noteWid/2-1,SEQY-BARHEIGHT,x+noteWid/2+1,SEQY+BARHEIGHT+NUM_PITCHES*NOTEHEI,32*4+15);
		}

		// now go and fill in the notes
		for(j=0;j<NUM_PITCHES;j++)
		{
			// put a box around if not in the skinniest zoom
			if(noteWid>3)
				mgl->Box(x,SEQY+j*NOTEHEI,x+noteWid,SEQY+NOTEHEI+j*NOTEHEI,0);
			// if there's a note here, fill it in!
			if(song->seq[curSeq].note[j][i])
			{
				mgl->FillBox(x+1,SEQY+1+(NUM_PITCHES-1-j)*NOTEHEI,x+noteWid-1,SEQY+NOTEHEI-1+(NUM_PITCHES-1-j)*NOTEHEI,1*32+15);
			}
		}

		x+=noteWid;
	}

	// if a note is being played, light up the row it's in
	if(lightUpTime)
	{
		DrawBox(0,SEQY+lightUp*NOTEHEI,SEQX2-1,SEQY+NOTEHEI+lightUp*NOTEHEI,32*1+31-16+lightUpTime*2);
	}

	// current position of mouse
	if(msy>=SEQY && msy<SEQY+NUM_PITCHES*NOTEHEI)
	{
		x=msx/noteWid;
		mgl->Box(x*noteWid,SEQY,x*noteWid+noteWid,SEQY+NUM_PITCHES*NOTEHEI,31);
	}

	// selection box
	if(selectMin!=-1)
	{
		mgl->Box((selectMin-seqPos)*noteWid-1,SEQY-1,(selectMax-seqPos)*noteWid+noteWid+1,SEQY+NUM_PITCHES*NOTEHEI,32*3+20);
		mgl->Box((selectMin-seqPos)*noteWid,SEQY,(selectMax-seqPos)*noteWid+noteWid,SEQY-1+NUM_PITCHES*NOTEHEI,32*3+20);
	}
}

void RenderSong(MGLDraw *mgl)
{
	int i,j,len,y,scrSize;
	song_t *song;
	char s[10];

	song=Music_GetSong();
	scrSize=screenSize-(SONGX/noteWid);
	y=SONGY-BARHEIGHT;

	// draw the top gap (where you click to set playback)
	mgl->FillBox(SONGX,y,SONGX2,y,0);
	mgl->FillBox(SONGX,y+BARHEIGHT,SONGX2,y+BARHEIGHT,0);

	// draw the current playpos
	i=Music_GetPos(-1)/SEQ_BLOCK_SIZE;
	if(i-songPos>=0 && i-songPos<=scrSize)
		mgl->FillBox(SONGX+(i-songPos)*noteWid+noteWid/2,y,SONGX+noteWid/2+1+(i-songPos)*noteWid,y+BARHEIGHT,32*1+16);

	y+=BARHEIGHT;
	// now cycle through the sequences, rendering their playlists
	for(i=songY;i<song->numSeqs;i++)
	{
		// sequence name
		if(i==curSeq)
		{
			mgl->FillBox(0,y,SONGX-8,y+PLAYHEI-1,88+6);
		}
		PrintLimited(2,y,SONGX-10,song->seq[i].name,-32,1);
		if(Music_IsMuted(i))
			mgl->FillBox(0,y+PLAYHEI/2,SONGX-10,y+PLAYHEI/2,0);

		len=0;
		for(j=songPos-1;j>0;j--)
		{
			if(song->seq[i].play[j]==SP_STOP)
				break;
			if(song->seq[i].play[j]==SP_PLAY)
			{
				len=song->seq[i].length-(songPos-j);
				break;
			}
			if(j<songPos-song->seq[i].length)
				break;
		}
		// now the playblocks
		for(j=songPos;j<songPos+scrSize;j++)
		{
			// every four is brightened to help counting
			if((j%4)==0)
			{
				mgl->FillBox(SONGX+(j-songPos)*noteWid,y,SONGX+noteWid+(j-songPos)*noteWid,y+PLAYHEI-1,92);
			}

			// if there is a play block here, show it, and track how many more blocks long it is
			if(song->seq[i].play[j]==SP_PLAY)
			{
				len=song->seq[i].length-1;
				mgl->FillBox(SONGX+(j-songPos)*noteWid,y,SONGX+noteWid+(j-songPos)*noteWid,y+PLAYHEI-1,32*4+20);
			}
			// stop block similar, but it has no length
			else if(song->seq[i].play[j]==SP_STOP)
			{
				len=0;
				mgl->FillBox(SONGX+(j-songPos)*noteWid,y,SONGX+noteWid+(j-songPos)*noteWid,y+PLAYHEI-1,3);
			}
			else
			{
				// if neither block is present, then continue showing the current play block if
				// it is not done yet (using skinny blocks to differentiate from the starting block)
				if(len>0)
				{
					len--;
					mgl->FillBox(SONGX+(j-songPos)*noteWid,y+PLAYHEI/2-1,SONGX+noteWid+(j-songPos)*noteWid,y+PLAYHEI/2+1,32*4+20);
				}
			}

			// if not in the skinniest zoom mode, then you want to make outlines around the blocks
			if(noteWid>3)
			{
				mgl->Box(SONGX+(j-songPos)*noteWid,y,SONGX+noteWid+(j-songPos)*noteWid,y+PLAYHEI,0);
			}
			// if the play pointer is here, draw it in
			if((Music_GetPos(-1)/SEQ_BLOCK_SIZE)==j)
				mgl->FillBox(SONGX+(j-songPos)*noteWid+noteWid/2,y,SONGX+noteWid/2+1+(j-songPos)*noteWid,y+PLAYHEI,32*1+16);
		}
		y+=PLAYHEI;
		if(y>SEQY-BARHEIGHT*3)
			break;	// no more sequences shown!
	}

	mgl->FillBox(SONGX,y,SONGX2,y,0);
	mgl->FillBox(SONGX,y+BARHEIGHT,SONGX2,y+BARHEIGHT,0);

	// selection box
	if(songSelMin!=-1)
	{
		i=(songSelMin-songPos)*noteWid+SONGX;
		j=(songSelMax-songPos)*noteWid+noteWid+SONGX;

		if(j>=SONGX && i<640)	// don't render anything if it's all to the left or right
		{
			if(i<SONGX)
			{
				mgl->Box(SONGX-1,SONGY-1,j+1,SONGY,32*3+20);
				mgl->Box(SONGX-1,y,j+1,y+1,32*3+20);
				mgl->Box(j,SONGY-1,j+1,y+1,32*3+20);
			}
			else
			{
				mgl->Box(i,SONGY,j,y,32*3+20);
				mgl->Box(i-1,SONGY-1,j+1,y+1,32*3+20);
			}
		}
	}

	if(msx>=SONGX && msy>=SONGY-BARHEIGHT && msy<=y+BARHEIGHT)
		i=(msx-SONGX)/noteWid+songPos;
	else
		i=-1;

	// fill in the numbers indicating the song time
	for(j=songPos;j<songPos+scrSize;j++)
	{
		// if it's selected, light it up
		if(j>=songSelMin && j<=songSelMax && songSelMin>-1)
			mgl->FillBox(SONGX+(j-songPos)*noteWid+1,y+1,SONGX+noteWid+(j-songPos)*noteWid-1,y+BARHEIGHT-1,32*3+20);
		if(((j%4)==0 && noteWid==10) || ((j%8)==0 && noteWid==5) || ((j%16)==0 && noteWid==3))
		{
			sprintf(s,"%d",j);
			CenterPrint(SONGX+(j-songPos)*noteWid+noteWid/2,y,s,-32,1);
		}
		// and put in the play pointer if it's here
		if((Music_GetPos(-1)/SEQ_BLOCK_SIZE)==j)
			mgl->FillBox(SONGX+(j-songPos)*noteWid+noteWid/2,y,SONGX+noteWid/2+1+(j-songPos)*noteWid,y+BARHEIGHT,32*1+16);
		if(j==i)
			mgl->Box(SONGX+(j-songPos)*noteWid,SONGY,SONGX+(j-songPos)*noteWid+noteWid,y,31);
	}
}

void RenderTypingWindow(MGLDraw *mgl)
{
	// box background
	mgl->FillBox(100,150,539,300,88+4);
	mgl->Box(100,150,539,300,0);
	mgl->Box(100-1,150-1,539+1,300+1,0);

	if(mode==CM_TITLE)
		CenterPrintUnGlow(320,155,"Enter Song Title",0);
	else if(mode==CM_AUTHOR)
		CenterPrintUnGlow(320,155,"Enter Song Author",0);
	else
		CenterPrintUnGlow(320,155,"Enter Sequence Name",0);

	mgl->FillBox(120,180,519,200,0);
	PrintLimited(124,180,519,txtEnter,0,0);

	CenterPrintOutline(210,270,"Accept",0,0);
	CenterPrintOutline(429,270,"Cancel",0,0);
}

static void RenderFileWindow(MGLDraw *mgl)
{
	int i;

	// box background
	mgl->FillBox(20,50,619,370,88-4);
	mgl->Box(20,50,619,370,0);
	mgl->Box(20-1,50-1,619+1,370+1,0);

	RightPrintGlow(616,55,"File Menu",0,0);
	mgl->FillBox(25,70,320,309,0);

	mgl->FillBox(25,312,320,332,0);
	PrintLimited(27,312,320,txtEnter,0,0);


	// scroll bar
	DrawFillBox(322,70,332,309,0);
	if(numSongs<=12)	// all songs fit on one page
		DrawFillBox(323,71,331,308,88+7);
	else	// just showing a subset of songs
	{
		if(filePos==0)	// if you're at the top, have the top half of the bar highlighted
			DrawFillBox(323,71,331,190,88+7);
		else if(filePos+12>=numSongs)	// at the bottom, highlight the bottom half
			DrawFillBox(323,190,331,308,88+7);
		else	// you're somewhere between, so fill the middle of the bar
			DrawFillBox(323,130,331,249,88+7);
	}

	PrintOutline(30,340,"Load",0,0);
	PrintOutline(90,340,"Save",0,0);
	PrintOutline(150,340,"New",0,0);
	RightPrintOutline(616,340,"Exit",0,0);

	for(i=filePos;i<filePos+12;i++)
		if(i<numSongs)
		{
			if(selectedFile==i)
				mgl->FillBox(25,70+(i-filePos)*20,320,70+19+(i-filePos)*20,80);
			PrintGlow(27,70+(i-filePos)*20,&filename[i*FNAMELEN],0,0);
		}

	if(selectedFile!=-1)
	{
		PrintGlow(350,95,"Song Title:",0,0);
		PrintGlow(350,115,&songname[selectedFile*FNAMELEN],0,0);
		PrintGlow(350,135,"Author:",0,0);
		PrintGlow(350,155,&author[selectedFile*FNAMELEN],0,0);
	}
}

static void RenderSetupWindow(MGLDraw *mgl)
{
	int i;
	char songName[32][NUM_SONGS]={
		"Title:",
		"Menu:",
		"Play1:",
		"Play2:",
		"Play3:",
		"Play4:",
		"Play5:"
	};

	// box background
	mgl->FillBox(20,50,619,370,88-4);
	mgl->Box(20,50,619,370,0);
	mgl->Box(20-1,50-1,619+1,370+1,0);

	RightPrintGlow(616,55,"Music Setup",0,0);

	mgl->FillBox(25,80,320,319,0);

	// scroll bar
	DrawFillBox(322,80,332,319,0);
	if(numSongs<=12)	// all songs fit on one page
		DrawFillBox(323,81,331,318,88+7);
	else	// just showing a subset of songs
	{
		if(filePos==0)	// if you're at the top, have the top half of the bar highlighted
			DrawFillBox(323,81,331,200,88+7);
		else if(filePos+12>=numSongs)	// at the bottom, highlight the bottom half
			DrawFillBox(323,200,331,318,88+7);
		else	// you're somewhere between, so fill the middle of the bar
			DrawFillBox(323,140,331,259,88+7);
	}

	for(i=0;i<NUM_SONGS;i++)
	{
		PrintOutline(340,80+i*25,songName[i],0,0);
		DrawFillBox(400,80+i*25,610,80+23+i*25,0);
		if(setupSel==i)
			DrawBox(400,80+i*25,610,80+23+i*25,31);
		if(profile.songChoice[i][0]=='\0')
			Print(402,82+i*25,"-- NO SONG --",0,0);
		else
			Print(402,82+i*25,profile.songChoice[i],0,0);
	}

	PrintGlow(340,260,"Click any to select, then",0,0);
	PrintGlow(340,280,"click any song to apply it.",0,0);
	PrintGlow(340,300,"Right-click to clear.",0,0);

	RightPrintOutline(616,340,"Exit",0,0);

	for(i=filePos;i<filePos+12;i++)
		if(i<numSongs)
		{
			if(selectedFile==i)
				mgl->FillBox(25,80+(i-filePos)*20,320,80+19+(i-filePos)*20,80);
			PrintGlow(27,80+(i-filePos)*20,&filename[i*FNAMELEN],0,0);
		}

	if(selectedFile!=-1)
	{
		PrintGlow(25,340,"Title:",0,0);
		PrintGlow(100,340,&songname[selectedFile*FNAMELEN],0,0);
		PrintGlow(250,340,"Author:",0,0);
		PrintGlow(330,340,&author[selectedFile*FNAMELEN],0,0);
	}
}

static void RenderYesNo(MGLDraw *mgl)
{
	char s[64];

	// box background
	mgl->FillBox(100,150,539,300,88+4);
	mgl->Box(100,150,539,300,0);
	mgl->Box(100-1,150-1,539+1,300+1,0);

	switch(askingFor)
	{
		case 1:	// load
			sprintf(s,"Load %s?",curFname);
			CenterPrintUnGlow(320,155,s,0);
			break;
		case 2:	// save
			sprintf(s,"Save %s?",curFname);
			CenterPrintUnGlow(320,155,s,0);
			break;
		case 3:	// exit
			CenterPrintUnGlow(320,155,"Really exit the composer?",0);
			break;
		case 4:	// new
			CenterPrintUnGlow(320,155,"Erase your song and start over?",0);
			break;
		case 5:	// delete a sequence
			sprintf(s,"Delete sequence '%s'?",Music_GetSong()->seq[curSeq].name);
			CenterPrintUnGlow(320,155,s,0);
			break;
	}

	CenterPrintOutline(210,270,"Yes",0,0);
	CenterPrintOutline(429,270,"No",0,0);
}

void RenderButton(button_t *b,MGLDraw *mgl)
{
	int x2,y2;

	x2=b->x+b->width-1;
	y2=b->y+b->height-1;
	switch(b->type)
	{
		case BTN_SML:
			mgl->Box(b->x,b->y,x2,y2,0);
			if(msx>=b->x && msx<=x2 && msy>=b->y && msy<=y2)
				mgl->FillBox(b->x+1,b->y+1,x2-1,y2-1,88+6);
			else
				mgl->FillBox(b->x+1,b->y+1,x2-1,y2-1,88);
			PrintLimited(b->x+2,b->y+2,x2-1,b->txt,-32,1);
			break;
		case BTN_BIG:
			mgl->FillBox(b->x,b->y,x2,y2,0);
			if(msx>=b->x && msx<=x2 && msy>=b->y && msy<=y2)
				mgl->FillBox(b->x+1,b->y+1,x2-1,y2-1,88+6);
			else
				mgl->FillBox(b->x+2,b->y+2,x2-2,y2-2,88+4);
			CenterPrintUnGlow((x2-b->x)/2+b->x,b->y+5,b->txt,0);
			break;
		case BTN_BOX:
			mgl->Box(b->x,b->y,x2,y2,0);
			mgl->Box(b->x+2,b->y-9,b->x+3+GetStrLength(b->txt,1)+2,b->y,0);
			mgl->FillBox(b->x+1,b->y+1,x2-1,y2-1,88-4);
			mgl->FillBox(b->x+3,b->y-8,b->x+3+GetStrLength(b->txt,1)+1,b->y,88-4);
			Print(b->x+5,b->y-7,b->txt,0,1);
			break;
	}
}

void RenderButtons(MGLDraw *mgl)
{
	int i;

	for(i=0;i<NUM_BUTTONS;i++)
	{
		RenderButton(&btn[i],mgl);
	}
}

void RenderComposer(MGLDraw *mgl)
{
	mgl->FillBox(0,0,639,479,88);

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	RenderSong(mgl);
	RenderMusicStaff(mgl);

	RenderButtons(mgl);

	switch(mode)
	{
		case CM_NORMAL:
			break;
		case CM_AUTHOR:
		case CM_SEQNAME:
		case CM_TITLE:
			RenderTypingWindow(mgl);
			break;
		case CM_FILE:
			RenderFileWindow(mgl);
			break;
		case CM_SETUP:
			RenderSetupWindow(mgl);
			break;
		case CM_YESNO:
			RenderYesNo(mgl);
			break;
	}

	if(cursMode==CURSOR_SONGPASTE)
	{
		PrintOutline(oldmsx,oldmsy+8,"Song",0,1);
		PrintOutline(oldmsx,oldmsy+20,"Paste",0,1);
	}
	if(cursMode==CURSOR_SONGCOVER)
	{
		PrintOutline(oldmsx,oldmsy+8,"Song",0,1);
		PrintOutline(oldmsx,oldmsy+20,"Cover",0,1);
	}
	if(cursMode==CURSOR_SEQPASTE)
	{
		PrintOutline(oldmsx,oldmsy+8,"Seq",0,1);
		PrintOutline(oldmsx,oldmsy+20,"Paste",0,1);
	}
	if(cursMode==CURSOR_SEQCOVER)
	{
		PrintOutline(oldmsx,oldmsy+8,"Seq",0,1);
		PrintOutline(oldmsx,oldmsy+20,"Cover",0,1);
	}
	compSpr->GetSprite(0)->Draw(oldmsx,oldmsy,mgl);
}

void Composer(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitComposer();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateComposer(&lastTime,mgl);
		RenderComposer(mgl);
		mgl->Flip();
		EndClock();

		if(mode==CM_EXIT)
			done=1;
		if(!mgl->Process())
			done=1;

	}

	ExitComposer();
}
