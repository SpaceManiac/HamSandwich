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
#include "hiscore.h"
#include "nameentry.h"
#include "scanner.h"
#include "netgame.h"

#define PBTN_HEIGHT	19

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

typedef struct profButton_t
{
	int x,y;
	int wid;
	char txt[32];
	byte id;
} profButton_t;

static profButton_t btn[]={
	{20,100,180,"Upload My Scores",BTN_UPLOAD},
	{20,160,180,"Upload My Profile",BTN_PROFILE},
	{20,220,180,"View World Records",BTN_SCOREPAGE},
#ifndef ARCADETOWN
	{20,280,180,"Look For Add-Ons",BTN_ADDONPAGE},
#endif
	{20,444,180,"Exit",BTN_EXIT},
};

#define SCORE_DOMAIN "hamumu.com"

#ifdef ARCADETOWN
#define NUM_NET_BTNS	4
#else
#define NUM_NET_BTNS	5
#endif

static byte mode,webInited;
static int msx,msy;

static byte *backgd;
static sprite_set_t *plSpr;
static char msBright,msDBright;

static byte netChoice,times,nameCheckFor;
static char errTxt[128];
static int socketNum;
static int scoresToUpload,scoresUploaded,scoreLen,numSent,totalUploaded,totalToGo;
static int numRecord[3],numTimeRecord[3],numFail,numIllegal;
static char *uploadList;

static byte pageToDo=0;
static byte mousemode;

void InitNetMenu(MGLDraw *mgl)
{
	int i;

	mousemode=0;
	pageToDo=0;
	mode=NET_NORMAL;
	webInited=0;
	msBright=0;
	msDBright=1;
	uploadList=NULL;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	plSpr=new sprite_set_t("graphics/pause.jsp");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	Init_Netgame();
}

void ExitNetMenu(void)
{
	free(backgd);
	delete plSpr;
	if(webInited)
		Web_Exit();

	Exit_Netgame();
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

void NetError(byte err)
{
	mode=NET_ERROR;
	CreateErrorString(err);
	InitYesNoDialog(errTxt,"Okay","Fine");
}

char *CreateUploadString(void)
{
	int i;
	char tmp[5];
	static char upString[512];

	strcpy(upString,"/supreme_score/put.php?s0=");

	numSent=0;
	for(i=scoresUploaded;i<scoresToUpload;i++)
	{
		strncat(upString,&uploadList[i*scoreLen],scoreLen);
		numSent++;
		if(numSent==4 || i==scoresToUpload-1)	// only 4 can be done per call
			break;

		sprintf(tmp,"&s%d=",numSent);
		strcat(upString,tmp);
	}
	if(times)
		strcat(upString,"&time=1");
	scoresUploaded+=numSent;

	return upString;
}

void CatPair(char *buffer,const char *var,dword val)
{
	char s[32];

	sprintf(s,"&%s=%lu",var,val);
	strcat(buffer,s);
}

void CreateProfileString(char *buffer)
{
	int i;
	dword cnt;
	float gamePct,playPct,scanPct,shopPct;

	CalcFinishedWorlds();
	profile.progress.calsBurned=(profile.progress.footDistance*150/168960)+profile.progress.hammersThrown+
		profile.progress.grassChopped;

	sprintf(buffer,"/supreme_score/profile.php?nam=%s&can=%lu",profile.name,profile.progress.totalCandles);
	CatPair(buffer,"bra",profile.progress.totalBrains);
	CatPair(buffer,"tim",profile.progress.totalTime);
	CatPair(buffer,"cn1",profile.progress.totalCoins);
	CatPair(buffer,"cn2",profile.progress.coinsSpent);
	CatPair(buffer,"hmr",profile.progress.hammersThrown);
	CatPair(buffer,"dmg",profile.progress.damageDone);
	CatPair(buffer,"och",profile.progress.damageTaken);
	CatPair(buffer,"sht",profile.progress.shotsFired);
	CatPair(buffer,"rag",profile.progress.rages);
	CatPair(buffer,"run",profile.progress.runOver);
	CatPair(buffer,"fds",profile.progress.footDistance);
	CatPair(buffer,"rds",profile.progress.raftDistance);
	CatPair(buffer,"cds",profile.progress.cartDistance);
	CatPair(buffer,"dds",profile.progress.driveDistance);
	CatPair(buffer,"h2o",profile.progress.underwaterTime);
	CatPair(buffer,"grs",profile.progress.grassChopped);
	CatPair(buffer,"key",profile.progress.keysFound);
	CatPair(buffer,"dor",profile.progress.doorsOpened);
	CatPair(buffer,"cal",profile.progress.calories);
	CatPair(buffer,"brn",profile.progress.calsBurned);
	CatPair(buffer,"cmb",profile.progress.bestCombo);
	CatPair(buffer,"die",profile.progress.kills[MONS_BOUAPHA]);

	cnt=0;
	for(i=2;i<NUM_PROFILE_MONSTERS;i++)
		cnt+=profile.progress.kills[i];
	CatPair(buffer,"kil",cnt);

	scanPct=ScanPercent();
	shopPct=ShopPercent();
	playPct=CalcPlayPercent();

	gamePct=playPct*0.5f+scanPct*0.1f+shopPct*0.3f;	// adds up to 90%
	cnt=0;
	for(i=0;i<100;i++)
		if(profile.progress.goal[i])
			cnt++;
	gamePct+=0.1f*((float)cnt);	// 10% total game progress for gallery goals

	CatPair(buffer,"scn",(dword)(ScanPercent()*100.0f));
	CatPair(buffer,"shp",(dword)(ShopPercent()*100.0f));
	CatPair(buffer,"ply",(dword)(CalcPlayPercent()*100.0f));
	CatPair(buffer,"gam",(dword)(gamePct*100.0f));
	CatPair(buffer,"gol",cnt);
	CatPair(buffer,"cht",profile.progress.cheats);

	cnt=strlen(buffer);
}

void BeginUpload(void)
{
	byte err;

	if(uploadList!=NULL)
		free(uploadList);

	if(times)
	{
		uploadList=GetTimesToUpload(&scoresToUpload,&scoreLen);
	}
	else
	{
		totalToGo=CountScoresToUpload();
		totalUploaded=0;
		for(err=0;err<3;err++)
		{
			numRecord[err]=0;
			numTimeRecord[err]=0;
		}
		numFail=0;
		numIllegal=0;
		uploadList=GetScoresToUpload(&scoresToUpload,&scoreLen);
	}

	if(scoresToUpload==0)
	{
		if(times)
			mode=NET_UPLOADDONE;
		else
		{
			if(!CountScoresToUpload())
			{
				InitYesNoDialog("There are no scores that haven't been uploaded already!","Oh!","Duh");
				mode=NET_ERROR;
			}
			else
			{
				times=1;
				BeginUpload();
			}
		}
		return;
	}

	scoresUploaded=0;

	mode=NET_UPLOADING;
	if((err=Web_Init())!=IE_OK)
	{
		NetError(err);
		return;
	}
	webInited=1;

	if((err=Web_RequestData(GetDisplayMGL(),SCORE_DOMAIN,CreateUploadString(),&socketNum))!=IE_OK)
	{
		NetError(err);
		return;
	}
}

void UploadMore()
{
	byte err;

	if((err=Web_RequestData(GetDisplayMGL(),SCORE_DOMAIN,CreateUploadString(),&socketNum))!=IE_OK)
	{
		NetError(err);
		return;
	}
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
	if((err=Web_RequestData(GetDisplayMGL(),SCORE_DOMAIN,req,&socketNum))!=IE_OK)
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

void DealWithSetUploaded(void)
{
	char *result;
	int i;

	result=(char *)Web_GetData(socketNum);

	for(i=0;i<numSent;i++)
	{
		if(result[i]=='F')
		{
			UnUpload(times,&uploadList[(scoresUploaded+i)*scoreLen]);
			numFail++;
		}
		if(result[i]=='N')
			numIllegal++;
		if(times)
		{
			if(result[i]=='1')
				numTimeRecord[0]++;
			if(result[i]=='2')
				numTimeRecord[1]++;
			if(result[i]=='3')
				numTimeRecord[2]++;
		}
		else
		{
			if(result[i]=='1')
				numRecord[0]++;
			if(result[i]=='2')
				numRecord[1]++;
			if(result[i]=='3')
				numRecord[2]++;
		}
		totalUploaded++;
		totalToGo--;
	}
	Web_KillSocket(socketNum);
}

void BeginMOTDGet(void)
{
	byte err;

	mode=NET_GETMOTD;
	if((err=Web_Init())!=IE_OK)
	{
		NetError(err);
		return;
	}
	webInited=1;

	if((err=Web_RequestData(GetDisplayMGL(),SCORE_DOMAIN,"/supreme_score/supremenews.php",&socketNum))!=IE_OK)
	{
		NetError(err);
		return;
	}
}

void EndMOTDGet(void)
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
	if((err=Web_RequestData(GetDisplayMGL(),SCORE_DOMAIN,req,&socketNum))!=IE_OK)
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
		if(mode==NET_NORMAL)
		{
			if(mousemode==2)
				Update_Netgame(1,msx,msy);
			else
				Update_Netgame(0,msx,msy);

		}
		else
			Update_Netgame(0,msx,msy);

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
							case BTN_UPLOAD:
								if(profile.nameVerified)
								{
									times=0;
									BeginUpload();
								}
								else
								{
									nameCheckFor=NET_UPLOADING;
									BeginNameCheck();
								}
								break;
							case BTN_PROFILE:
								if(profile.nameVerified)
								{
									BeginProfileUpload();
								}
								else
								{
									nameCheckFor=NET_PROFILEUP;
									BeginNameCheck();
								}
								break;
							case BTN_SCOREPAGE:
								//mode=NET_GETMOTD;
								//BeginMOTDGet();
								pageToDo=1;
								return 1;
								break;
							case BTN_ADDONPAGE:
								//mode=NET_GETMOTD;
								//BeginMOTDGet();
								pageToDo=2;
								return 1;
								break;
							case BTN_EXIT:
								return 1;
								break;
						}
					}
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
				if(mode==NET_UPLOADYES)
				{
					if(profile.nameVerified)
					{
						times=0;
						BeginUpload();
					}
					else
					{
						BeginNameCheck();
					}
				}
				if(mode==NET_CANCELUP || mode==NET_CANCELNAME || mode==NET_CANCELMOTD || mode==NET_CANCELPROF)
				{
					Web_Exit();
					webInited=0;
					mode=NET_NORMAL;
					SaveHiScores();
				}
				if(mode==NET_BADNAME)
				{
					NameEntry(mgl,0);
					ChangeHighScores(profile.name,GetEnteredName());
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
		case NET_UPLOADING:
			state=Web_GetState(socketNum);
			if(state==SS_FAILED || state==SS_UNUSED)
			{
				NetError(Web_GetError(socketNum));
				break;
			}
			else if(state==SS_DONERECEIVING)
			{
				DealWithSetUploaded();
				if(scoresUploaded<scoresToUpload)
					UploadMore();
				else
				{
					if(times==0)
					{
						times=1;
						Web_Exit();
						webInited=0;
						BeginUpload();
					}
					else
					{
						Web_Exit();
						webInited=0;
						SaveHiScores();
						mode=NET_UPLOADDONE;
					}
				}
			}
			if(k==27)
			{
				mode=NET_CANCELUP;
				InitYesNoDialog("Cancel the Upload?","Yes","No");
			}
			if(mb)
			{
				if(PointInRect(msx,msy,110,370,110+100,370+PBTN_HEIGHT))
				{
					mode=NET_CANCELUP;
					InitYesNoDialog("Cancel the Upload?","Yes","No");
				}
			}
			break;
		case NET_NAMECHECK:
			state=Web_GetState(socketNum);
			if(state==SS_FAILED || state==SS_UNUSED)
			{
				NetError(Web_GetError(socketNum));
				break;
			}
			else if(state==SS_DONERECEIVING)
			{
				k=(char)(Web_GetData(socketNum)[0]);
				EndNameCheck();
				if(k=='Y')
				{
					profile.nameVerified=1;
					SaveProfile();
					times=0;
					if(nameCheckFor==NET_UPLOADING)
						BeginUpload();
					else if(nameCheckFor==NET_PROFILEUP)
						BeginProfileUpload();
				}
				else if(k=='N')
				{
					mode=NET_BADNAME;
					InitYesNoDialog("Your profile name is already in use - would you like to pick "
									"a new one? All your high scores will automatically change.","Yes","No");
				}
				else
				{
					NetError(IE_NOCONNECT);
				}
				break;

			}
			if(k==27)
			{
				mode=NET_CANCELNAME;
				InitYesNoDialog("Cancel name verification?","Yes","No");
			}
			if(mb)
			{
				if(PointInRect(msx,msy,110,370,110+100,370+PBTN_HEIGHT))
				{
					mode=NET_CANCELNAME;
					InitYesNoDialog("Cancel name verification?","Yes","No");
				}
			}
			break;
		case NET_GETMOTD:
			state=Web_GetState(socketNum);
			if(state==SS_FAILED || state==SS_UNUSED)
			{
				NetError(Web_GetError(socketNum));
				break;
			}
			else if(state==SS_DONERECEIVING)
			{
				strcpy(profile.motd,(char *)Web_GetData(socketNum));
				EndMOTDGet();
				SaveProfile();
				mode=NET_NORMAL;
				break;
			}
			if(k==27)
			{
				mode=NET_CANCELMOTD;
				InitYesNoDialog("Cancel news download?","Yes","No");
			}
			if(mb)
			{
				if(PointInRect(msx,msy,110,370,110+100,370+PBTN_HEIGHT))
				{
					mode=NET_CANCELMOTD;
					InitYesNoDialog("Cancel news download?","Yes","No");
				}
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
					InitYesNoDialog("Your profile has been uploaded!","Yay!","Yip!");
				}
				else
				{
					NetError(0);
					InitYesNoDialog("Unable to upload profile.","Oh","Darn");
				}
				break;
			}
			if(k==27)
			{
				mode=NET_CANCELPROF;
				InitYesNoDialog("Cancel profile upload?","Yes","No");
			}
			if(mb)
			{
				if(PointInRect(msx,msy,110,370,110+100,370+PBTN_HEIGHT))
				{
					mode=NET_CANCELPROF;
					InitYesNoDialog("Cancel profile upload?","Yes","No");
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
	if(PointInRect(msx,msy,x,y,x+wid,y+PBTN_HEIGHT))
	{
		mgl->Box(x,y,x+wid,y+PBTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+PBTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+PBTN_HEIGHT,32+16);

	PrintGlow(x+2,y+3,txt,0,2);
}

void PrintUploadStat(const char *name,int val,int x,int y)
{
	char s[16];

	PrintGlow(x,y,name,0,2);

	sprintf(s,"%d",val);

	PrintGlow(x+130-GetStrLength(s,2),y,s,0,2);
}

void RenderUpload(MGLDraw *mgl)
{
	char s[64];
	int size;

	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Uploading Record Scores And Times",0,2);

	PrintUploadStat("#1 Scores:",numRecord[0],110,130);
	PrintUploadStat("#2 Scores:",numRecord[1],110,155);
	PrintUploadStat("#3 Scores:",numRecord[2],110,180);
	PrintUploadStat("#1 Times:",numTimeRecord[0],370,130);
	PrintUploadStat("#2 Times:",numTimeRecord[1],370,155);
	PrintUploadStat("#3 Times:",numTimeRecord[2],370,180);
	PrintUploadStat("Failed:",numFail,110,250);
	PrintUploadStat("Illegal:",numIllegal,370,250);
	PrintUploadStat("Total:",totalUploaded,240,280);

	mgl->Box(110,200,530,240,32*1+16);
	if(totalToGo+totalUploaded>0)
	{
		size=(totalUploaded*(528-112)/(totalToGo+totalUploaded));
		mgl->FillBox(112,202,112+size,238,32*1+8);
		size=(totalUploaded*100/(totalToGo+totalUploaded));
		sprintf(s,"%d%%",size);
		PrintGlow(320-GetStrLength(s,2),208,s,0,2);
	}

	if(mode==NET_UPLOADDONE)
		RenderNetButton(110,370,130,"Okay!",mgl);
	else
		RenderNetButton(110,370,130,"Cancel Upload",mgl);
}

void RenderNameCheck(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Verifying your profile name...",0,2);

	RenderNetButton(110,370,130,"Cancel",mgl);
}

void RenderMOTDGet(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Downloading latest news...",0,2);

	RenderNetButton(110,370,130,"Cancel",mgl);
}

void RenderProfileUp(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Uploading your profile...",0,2);

	RenderNetButton(110,370,130,"Cancel",mgl);
}

void RenderNetMenu(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	Render_Netgame();

	for(i=0;i<NUM_NET_BTNS;i++)
	{
		RenderNetButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl);
	}

	PrintGlow(20,245,"(Exits the game and opens a web browser)",0,2);
#ifndef ARCADETOWN
	PrintGlow(20,305,"(Exits the game and opens a web browser)",0,2);
#endif
	PrintGlow(20,20,"Please be sure you are connected to the internet before",0,2);
	PrintGlow(20,40,"using any of these functions.",0,2);

	//mgl->Box(20,330,620,430,32*1+16);
	//PrintGlow(260,310,"Latest News",0,2);
	//PrintGlowRect(22,332,618,428,20,profile.motd,2);

	if(mode==NET_PROFILEUP || mode==NET_CANCELPROF)
	{
		RenderNameCheck(mgl);
	}

	if(mode==NET_NAMECHECK || mode==NET_CANCELNAME)
	{
		RenderNameCheck(mgl);
	}

	if(mode==NET_GETMOTD || mode==NET_CANCELMOTD)
	{
		RenderMOTDGet(mgl);
	}

	if(mode==NET_UPLOADING || mode==NET_CANCELUP || mode==NET_UPLOADDONE)
	{
		RenderUpload(mgl);
	}
	if(mode==NET_UPLOADYES || mode==NET_ERROR || mode==NET_CANCELUP || mode==NET_CANCELNAME ||
	   mode==NET_BADNAME || mode==NET_CANCELMOTD || mode==NET_CANCELPROF)
		RenderYesNoDialog3(msx,msy,mgl);

	// mouse cursor
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
	SetSpriteConstraints(0,0,639,479);
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
