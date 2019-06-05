#include "netmenu.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "playlist.h"
#include "nameentry.h"
#include "yesnodialog.h"
#include "recordbook.h"
#include "shop.h"
#include "internet.h"
#include "nameentry.h"
#include "scanner.h"
#include "md5.h"
#include "goal.h"

#define PBTN_HEIGHT	30

#define NET_NORMAL	0
#define NET_UPLOADYES	1
#define NET_UPLOADING	2
#define NET_ERROR		5
#define NET_CANCELUP	6
#define NET_CANCELNAME	7
#define NET_UPLOADDONE	8
#define NET_NAMECHECK	9
#define NET_BADNAME		10
#define NET_GETMOTD		11
#define NET_CANCELMOTD	12
#define NET_PROFILEUP	13
#define NET_CANCELPROF	14

#define BTN_UPLOAD		1
#define BTN_SCOREPAGE	2
#define BTN_PROFILE		3
#define BTN_EXIT		4
#define BTN_ADDONPAGE	5
#define BTN_NAMEMODE	6
#define BTN_PASSMODE	7

typedef struct profButton_t
{
	int x,y;
	int wid;
	char txt[32];
	byte id;
} profButton_t;

static profButton_t btn[]={
	{20,240,300,"Name",BTN_NAMEMODE},
	{20,280,300,"Password",BTN_PASSMODE},
	{20,320,160,"Upload My Goals",BTN_PROFILE},
	{20,420,80,"Exit",BTN_EXIT},
};

#define NUM_NET_BTNS	4

static byte mode,webInited;
static int msx,msy;

static byte *backgd;
static char msBright,msDBright;

static byte netChoice,times,nameCheckFor;
static char errTxt[128],dumbName[64],dumbPass[64];
static int socketNum;

static byte typeMode;
static char *uploadList;

static byte pageToDo=0;
static byte mousemode;

void InitNetMenu(MGLDraw *mgl)
{
	int i;

	typeMode=0;
	mousemode=0;
	pageToDo=0;
	mode=NET_NORMAL;
	webInited=0;
	msBright=0;
	msDBright=1;
	uploadList=NULL;

	mgl->LoadBMP("graphics/pause.bmp");
	backgd=(byte *)malloc(640*480);
	mgl->MouseTap();
	mgl->LastKeyPressed();

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
}

void ExitNetMenu(void)
{
	free(backgd);
	if(webInited)
		Web_Exit();
}

void CreateErrorString(byte err)
{
	switch(err)
	{
		case IE_SITENOTFOUND:
		case IE_NOCONNECT:
			strcpy(errTxt,"Unable to connect to website!");
			break;
		case IE_NOWINSOCK:
			strcpy(errTxt,"Unable to initialize sockets!");
			break;
		case IE_NOSOCKETSFREE:
			strcpy(errTxt,"Unable to find an empty socket!");
			break;
		case IE_BADSOCKET:
			strcpy(errTxt,"Unable to create socket!");
			break;
		case IE_SENDFAIL:
			strcpy(errTxt,"Unable to send request!");
			break;
		case IE_OUTOFMEMORY:
			strcpy(errTxt,"Out of memory!");
			break;
		case IE_RECVFAIL:
			strcpy(errTxt,"Unable to receive data!");
			break;
		case IE_FILENOTFOUND:
			// use the web error string, except for the preceding "HTTP/1.1 "
			sprintf(errTxt,"Transfer failed: %s",&Web_ErrorStringShort()[9]);
			break;
		default:
			strcpy(errTxt,"Unknown error!");
			break;
	}
}

char *ToHex(byte v)
{
	static char result[3];
	char tab[17]={"0123456789abcdef"};

	result[2]='\0';
	result[1]=tab[(v%16)];
	result[0]=tab[(v/16)];
	return result;
}

void NetError(byte err)
{
	mode=NET_ERROR;
	CreateErrorString(err);
	InitYesNoDialog(errTxt,"Okay","Fine");
}

void CatPair(char *buffer,char *var,dword val)
{
	char s[32];

	sprintf(s,"&%s=%ld",var,val);
	strcat(buffer,s);
}

void CreateProfileString(char *buffer)
{
	int i,pos;
	char longName[64*3];
	char *hex;
	char md5pass[17],md5passText[33],goalText[27],fishText[10];

	strcpy(buffer,"/supreme_score/hollowgoal.php?n=");

	pos=0;
	for(i=0;i<(int)strlen(dumbName);i++)
	{
		if((dumbName[i]>='0' && dumbName[i]<='9') || (dumbName[i]>='A' && dumbName[i]<='Z') ||
			(dumbName[i]>='a' && dumbName[i]<='z'))
		{
			longName[pos++]=dumbName[i];
		}
		else
		{
			longName[pos++]='%';
			hex=ToHex((byte)dumbName[i]);
			longName[pos++]=hex[0];
			longName[pos++]=hex[1];
		}
	}
	longName[pos]='\0';

	strcat(buffer,longName);
	strcat(buffer,"&p=");

	md5_buffer(dumbPass,strlen(dumbPass),md5pass);

	pos=0;
	for(i=0;i<16;i++)
	{
		hex=ToHex((byte)md5pass[i]);
		md5passText[pos++]=hex[0];
		md5passText[pos++]=hex[1];
	}
	md5passText[pos]='\0';
	strcat(buffer,md5passText);
	strcat(buffer,"&g=");

	for(i=0;i<NUM_GOALS;i++)
		goalText[i]='0'+(profile.progress.goal[i]>0);
	goalText[NUM_GOALS]='\0';
	strcat(buffer,goalText);

	sprintf(fishText,"%d%03d",profile.progress.goal[98],profile.progress.goal[99]);
	strcat(buffer,"&f=");
	strcat(buffer,fishText);

	strcat(buffer,"&c=");
	// we are going to do an md5 hash on name+POPPY+md5'd password+PERSIMMON+goals completed+biggest fish
	sprintf(longName,"%sPOPPY%sPERSIMMON%s%s",dumbName,md5passText,goalText,fishText);
	md5_buffer(longName,strlen(longName),md5pass);

	pos=0;
	for(i=0;i<16;i++)
	{
		hex=ToHex((byte)md5pass[i]);
		md5passText[pos++]=hex[0];
		md5passText[pos++]=hex[1];
	}
	md5passText[pos]='\0';
	strcat(buffer,md5passText);
}

void BeginNameCheck(void)
{
	byte err;
	char req[256];

	mode=NET_NAMECHECK;
	if((err=Web_Init())!=IE_OK)
	{
		NetError(err);
		return;
	}
	webInited=1;

	sprintf(req,"/supreme_score/namecheck.php?name=%s",profile.name);
	if((err=Web_RequestData(GetDisplayMGL(),"hamumu.com",req,&socketNum))!=IE_OK)
	{
		NetError(err);
		return;
	}
}

void EndNameCheck(void)
{
	Web_Exit();
	webInited=0;
}

void BeginProfileUpload(void)
{
	byte err;
	char *req;

	mode=NET_PROFILEUP;
	if((err=Web_Init())!=IE_OK)
	{
		NetError(err);
		return;
	}
	webInited=1;

	req=(char *)malloc(1024);
	CreateProfileString(req);
	if((err=Web_RequestData(GetDisplayMGL(),"hamumu.com",req,&socketNum))!=IE_OK)
	{
		NetError(err);
		free(req);
		return;
	}
	free(req);
}

void EndProfileUp(void)
{
	Web_Exit();
	webInited=0;
}

byte UpdateNetMenu(int *lastTime,MGLDraw *mgl)
{
	int i;
	byte b,mb;
	char k;
	byte state;
	char *entry;

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	k=mgl->LastKeyPressed();

	if(webInited)
		Web_Poll(socketNum);

	if(mousemode==0)	// not locked into either buttons or game right now
	{
		mb=mgl->MouseTap();
		if(mb)
		{
			for(i=0;i<NUM_NET_BTNS;i++)
			{
				if(PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PBTN_HEIGHT))
					mousemode=1;	// in buttons-only mode
			}
			if(mousemode==0 && mode==NET_NORMAL)
				mousemode=2;	// game-only mode
			else
				mousemode=1;
		}
	}
	else
	{
		mb=mgl->MouseDown();
		if(!mb)
			mousemode=0;
		else
			mb=0;
	}

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

	switch(mode)
	{
		case NET_NORMAL:
			if(mb)
			{
				for(i=0;i<NUM_NET_BTNS;i++)
				{
					if(PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PBTN_HEIGHT))
					{
						MakeNormalSound(SND_MENUSELECT);
						switch(btn[i].id)
						{
							case BTN_NAMEMODE:
								typeMode=0;
								break;
							case BTN_PASSMODE:
								typeMode=1;
								break;
							case BTN_PROFILE:
								BeginProfileUpload();
								break;
							case BTN_EXIT:
								return 1;
								break;
						}
					}
				}
			}
			if(typeMode==0)
				entry=dumbName;
			else
				entry=dumbPass;

			if(k==13)	// enter
			{
				MakeNormalSound(SND_MENUSELECT);
				if(typeMode==0)
					typeMode=1;
				else
				{
					BeginProfileUpload();
				}
			}
			else if(k==9)	// tab
			{
				MakeNormalSound(SND_MENUSELECT);
				typeMode=1-typeMode;
			}
			else if(k>=32 && k<=126)	// valid characters
			{
				if(strlen(entry)<62)
				{
					entry[strlen(entry)+1]='\0';
					if(ShiftState())
					{
						if(k>='a' && k<='z')
							k+='A'-'a';
					}
					entry[strlen(entry)]=k;
					MakeNormalSound(SND_MENUCLICK);
				}
			}
			else if(k==8)
			{
				if(strlen(entry)>0)
				{
					entry[strlen(entry)-1]='\0';
					MakeNormalSound(SND_MENUCLICK);
				}
			}
			if(k==27)
				return 1;	// exit
			break;
		case NET_UPLOADYES:
		case NET_CANCELUP:
		case NET_CANCELNAME:
		case NET_CANCELMOTD:
		case NET_CANCELPROF:
		case NET_BADNAME:
			b=0;
			if(mb)
				b=YesNoDialogClick(msx,msy);
			if(k)
				b=YesNoDialogKey(k);

			if(b==YNM_YES)
			{
				MakeNormalSound(SND_MENUSELECT);
				if(mode==NET_CANCELUP || mode==NET_CANCELNAME || mode==NET_CANCELMOTD || mode==NET_CANCELPROF)
				{
					Web_Exit();
					webInited=0;
					mode=NET_NORMAL;
				}
				if(mode==NET_BADNAME)
				{
					NameEntry(mgl,0);
					strcpy(profile.name,GetEnteredName());
					SaveProfile();
					BeginNameCheck();
				}
			}
			if(b==YNM_NO)
			{
				MakeNormalSound(SND_MENUSELECT);
				if(mode==NET_CANCELUP)
					mode=NET_UPLOADING;
				else if(mode==NET_CANCELNAME)
					mode=NET_NAMECHECK;
				else if(mode==NET_CANCELMOTD)
					mode=NET_GETMOTD;
				else if(mode==NET_CANCELPROF)
					mode=NET_PROFILEUP;
				else
					mode=NET_NORMAL;
			}
			break;
		case NET_ERROR:
			b=0;
			if(mb)
				b=YesNoDialogClick(msx,msy);
			if(k)
				b=YesNoDialogKey(k);

			if(b==YNM_NO || b==YNM_YES)
			{
				MakeNormalSound(SND_MENUSELECT);
				mode=NET_NORMAL;
			}
			break;
		case NET_PROFILEUP:
			state=Web_GetState(socketNum);
			if(state==SS_FAILED || state==SS_UNUSED)
			{
				NetError(Web_GetError(socketNum));
				EndProfileUp();
				break;
			}
			else if(state==SS_DONERECEIVING)
			{
				k=(char)(Web_GetData(socketNum)[0]);
				EndProfileUp();
				if(k=='Y')
				{
					NetError(0);
					InitYesNoDialog("Your goals have been uploaded!","Yay!","Yip!");
				}
				else if(k=='N')
				{
					NetError(0);
					InitYesNoDialog("Invalid account or password.","Oh","Darn");
				}
				else if(k=='G')
				{
					NetError(0);
					InitYesNoDialog("You don't own this game!","Oh","Darn");
				}
				else if(k=='V')
				{
					NetError(0);
					InitYesNoDialog("Game version too low - update!","Oh","Darn");
				}
				else
				{
					NetError(0);
					InitYesNoDialog("Unable to upload goals.","Oh","Darn");
				}
				break;
			}
			if(k==27)
			{
				mode=NET_CANCELPROF;
				InitYesNoDialog("Cancel goal upload?","Yes","No");
			}
			if(mb)
			{
				if(PointInRect(msx,msy,110,370,110+100,370+PBTN_HEIGHT))
				{
					mode=NET_CANCELPROF;
					InitYesNoDialog("Cancel goal upload?","Yes","No");
				}
			}
			break;
		case NET_UPLOADDONE:
			if(k==27 || k==13)
			{
				mode=NET_NORMAL;
				MakeNormalSound(SND_MENUSELECT);
			}
			if(mb)
			{
				if(PointInRect(msx,msy,110,370,110+100,370+PBTN_HEIGHT))
				{
					MakeNormalSound(SND_MENUSELECT);
					mode=NET_NORMAL;
				}
			}
			break;
	}

	return 0;
}

void RenderNetButton(int x,int y,int wid,const char *txt,MGLDraw *mgl)
{
	PrintUnGlow(x+2,y+2-5,txt,2);

	if(PointInRect(msx,msy,x,y,x+wid,y+PBTN_HEIGHT))
	{
		PrintUnGlow(x+1,y+1-5,txt,2);
		PrintUnGlow(x+3,y+3-5,txt,2);
		PrintUnGlow(x+1,y+3-5,txt,2);
		PrintUnGlow(x+3,y+1-5,txt,2);
		PrintGlow(x+2,y+2-5,txt,0,2);
	}
}

void RenderProfileUp(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,31);
	mgl->Box(100,100,540,400,32*2+20);

	PrintUnGlow(102,102,"Uploading your goals...",2);

	RenderNetButton(110,370,130,"Cancel",mgl);
}

void PrintThing(int x,int y,const char *txt,byte pass,byte flash)
{
	int i;
	int len=GetStrLength("*",1),pos;

	if(pass)
	{
		for(i=0;i<(int)strlen(txt);i++)
			PrintUnGlow(x+i*len,y,"*",1);
		pos=i*len;
	}
	else
	{
		PrintUnGlow(x,y,txt,1);
		pos=GetStrLength(txt,1);
	}
	if(flash)
		PrintUnGlow(x+pos,y,"_",1);
}

void RenderNetMenu(MGLDraw *mgl)
{
	int i;
	static byte flash=0;
	byte f;

	flash=1-flash;
	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_NET_BTNS;i++)
	{
		RenderNetButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl);
	}

	mgl->FillBox(110,240,310,270,32*2+20);
	mgl->FillBox(110,280,310,310,32*2+20);
	if(typeMode==0)
	{
		mgl->FillBox(110,240,310,270,31);
		mgl->Box(110,240,310,270,0);
	}
	else
	{
		mgl->FillBox(110,280,310,310,31);
		mgl->Box(110,280,310,310,0);

	}

	if(typeMode==0)
		f=flash;
	else
		f=0;
	PrintThing(112,250,dumbName,0,f);
	if(typeMode==1)
		f=flash;
	else
		f=0;
	PrintThing(112,290,dumbPass,1,f);

	PrintUnGlowRect(10,10,310,470,30,"Please make sure you are connected to the internet before uploading!  Enter your "
					"Dumb Account name and password below.  If you don't have a Dumb Account, visit hamumu.com to get one!",2);

	if(mode==NET_PROFILEUP || mode==NET_CANCELPROF)
	{
		RenderProfileUp(mgl);
	}

	if(mode==NET_UPLOADYES || mode==NET_ERROR || mode==NET_CANCELUP || mode==NET_CANCELNAME ||
	   mode==NET_BADNAME || mode==NET_CANCELMOTD || mode==NET_CANCELPROF)
		RenderYesNoDialog2(msx,msy,mgl);

	// mouse cursor
	DrawMouseCursor(msx,msy);
}

//----------------

void NetMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitNetMenu(mgl);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateNetMenu(&lastTime,mgl);
		RenderNetMenu(mgl);

		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	SaveProfile();
	ExitNetMenu();
}

byte DoWebPage(void)
{
	return pageToDo;
}
