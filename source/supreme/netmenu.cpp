#include "netmenu.h"
#include <memory>
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
#include "openurl.h"
#include "steam.h"

constexpr int PBTN_HEIGHT = 19;

enum Mode : byte
{
	NET_NORMAL,
	NET_UPLOADYES,
	NET_UPLOADING,
	NET_ERROR,
	NET_CANCELUP,
	NET_CANCELNAME,
	NET_UPLOADDONE,
	NET_NAMECHECK,
	NET_BADNAME,
	NET_GETMOTD,
	NET_CANCELMOTD,
	NET_PROFILEUP,
	NET_CANCELPROF,
};

enum class ButtonId
{
	None,
	Exit,
	HamumuWebsite,
	HamSandwich,
	LoonyChat,
	SteamWorkshop,
	Yes,
	No,
	// ...
	Upload,
	Profile,
	ScorePage,
	AddonPage,
};

struct NetMenuButton
{
	int x,y;
	int wid;
	char txt[64];
	ButtonId id;
	bool steamOnly = false;
};

static const NetMenuButton btn[]={
	{20,100,320,"More games by Hamumu", ButtonId::HamumuWebsite},
	{20,160,320,"Get this game's source code", ButtonId::HamSandwich},
	{20,220,320,"Join the LoonyChat on Discord", ButtonId::LoonyChat},
	{20,280,320,"Browse add-ons in the Steam Workshop", ButtonId::SteamWorkshop, true},
	{20,444,180,"Exit", ButtonId::Exit},
};

#define SCORE_DOMAIN "hamumu.com"

constexpr int NUM_NET_BTNS = SDL_arraysize(btn);

static Mode mode;
static bool webInited;
static int msx,msy;

static byte *backgd;
static std::unique_ptr<sprite_set_t> plSpr;
static char msBright,msDBright;

static byte times,nameCheckFor;
static char errTxt[128];
static int socketNum;
static int scoresToUpload,scoresUploaded,scoreLen,numSent,totalUploaded,totalToGo;
static int numRecord[3],numTimeRecord[3],numFail,numIllegal;
static char *uploadList;

static ButtonId curButton = ButtonId::None;

enum class MouseMode
{
	Idle,
	Buttons,
	NetGame,
};
static MouseMode mousemode;

void InitNetMenu(MGLDraw *mgl)
{
	mousemode = MouseMode::Idle;
	mode = NET_NORMAL;
	webInited = false;
	msBright = 0;
	msDBright = 1;
	uploadList = NULL;
	curButton = ButtonId::None;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	for(int i=0; i<480; i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	plSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");

	GetTaps(); GetArrowTaps();

	Init_Netgame();
}

void ExitNetMenu(void)
{
	free(backgd);
	plSpr.reset();
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
	char tmp[16];
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

	sprintf(s,"&%s=%u",var,val);
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

	sprintf(buffer,"/supreme_score/profile.php?nam=%s&can=%u",profile.name,profile.progress.totalCandles);
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
	webInited = true;

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
	webInited = true;

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
	webInited = false;
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
	webInited = true;

	if((err=Web_RequestData(GetDisplayMGL(),SCORE_DOMAIN,"/supreme_score/supremenews.php",&socketNum))!=IE_OK)
	{
		NetError(err);
		return;
	}
}

void EndMOTDGet(void)
{
	Web_Exit();
	webInited = false;
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
	webInited = true;

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
	webInited = false;
}

TASK(byte) UpdateNetMenu(int *lastTime,MGLDraw *mgl)
{
	byte b;
	byte state;

	// Mouse
	int oldMsx = msx, oldMsy = msy;
	mgl->GetMouse(&msx, &msy);
	bool mb = mgl->MouseTap();
	if (mb || msx != oldMsx || msy != oldMsy)
	{
		curButton = ButtonId::None;
		for(int i=0; i<NUM_NET_BTNS; i++)
		{
			if ((!btn[i].steamOnly || SteamManager::Get()->IsSteamEdition()) && PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PBTN_HEIGHT))
			{
				curButton = btn[i].id;
			}
		}
	}

	if(mousemode == MouseMode::Idle)	// not locked into either buttons or game right now
	{
		if(mb)
		{
			if (curButton != ButtonId::None)
				mousemode = MouseMode::Buttons;	// in buttons-only mode

			if(mousemode == MouseMode::Idle && mode==NET_NORMAL)
				mousemode = MouseMode::NetGame;	// game-only mode
			else
				mousemode = MouseMode::Buttons;
		}
	}
	else
	{
		if(!mgl->MouseDown())
			mousemode = MouseMode::Idle;
		else
			mb = false;
	}

	// Keyboard
	char k = mgl->LastKeyPressed();

	// Controller
	byte tap = GetTaps() | GetArrowTaps();

	if (tap & CONTROL_DN)
	{
		switch (curButton)
		{
			case ButtonId::None:
				curButton = ButtonId::HamumuWebsite;
				break;
			case ButtonId::HamumuWebsite:
				curButton = ButtonId::HamSandwich;
				break;
			case ButtonId::HamSandwich:
				curButton = ButtonId::LoonyChat;
				break;
			case ButtonId::LoonyChat:
				curButton = SteamManager::Get()->IsSteamEdition() ? ButtonId::SteamWorkshop : ButtonId::Exit;
				break;
			case ButtonId::SteamWorkshop:
				curButton = ButtonId::Exit;
				break;
			default: break;
		}
	}
	if (tap & CONTROL_UP)
	{
		switch (curButton)
		{
			case ButtonId::HamSandwich:
				curButton = ButtonId::HamumuWebsite;
				break;
			case ButtonId::LoonyChat:
				curButton = ButtonId::HamSandwich;
				break;
			case ButtonId::SteamWorkshop:
				curButton = ButtonId::LoonyChat;
				break;
			case ButtonId::Exit:
				curButton = SteamManager::Get()->IsSteamEdition() ? ButtonId::SteamWorkshop : ButtonId::LoonyChat;
				break;
			case ButtonId::None:
				curButton = ButtonId::Exit;
				break;
			default: break;
		}
	}

	// Polling
	if(webInited)
		Web_Poll(socketNum);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;
	while(*lastTime>=TIME_PER_FRAME)
	{
		Update_Netgame(mode == NET_NORMAL && mousemode==MouseMode::NetGame, msx, msy);

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
			if(mb || (tap & CONTROL_B1))
			{
				switch (curButton)
				{
					case ButtonId::HamumuWebsite:
						MakeNormalSound(SND_MENUSELECT);
						if (SteamManager::Get()->IsSteamEdition())
							SteamManager::Get()->OpenURLOverlay("https://store.steampowered.com/developer/Hamumu");
						else
							SDL_OpenURL("https://hamumu.com");
						break;
					case ButtonId::HamSandwich:
						MakeNormalSound(SND_MENUSELECT);
						SteamManager::Get()->OpenURLOverlay("https://github.com/SpaceManiac/HamSandwich");
						break;
					case ButtonId::LoonyChat:
						MakeNormalSound(SND_MENUSELECT);
						SDL_OpenURL("https://discord.gg/BJpDSCK9s4");
						break;
					case ButtonId::SteamWorkshop:
						MakeNormalSound(SND_MENUSELECT);
						SteamManager::Get()->OpenURLOverlay("https://steamcommunity.com/app/2547330/workshop/");
						break;
					case ButtonId::Upload:
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
					case ButtonId::Profile:
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
					case ButtonId::ScorePage:
						SDL_OpenURL("http://hamumu.com/scores.php");
						CO_RETURN 1;
						break;
					case ButtonId::AddonPage:
						SDL_OpenURL("http://hamumu.com/addon.php");
						CO_RETURN 1;
						break;
					case ButtonId::Exit:
						CO_RETURN 1;
						break;
					default: break;
				}
			}
			if(k==27)
				CO_RETURN 1;	// exit
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
					webInited = false;
					mode=NET_NORMAL;
					SaveHiScores();
				}
				if(mode==NET_BADNAME)
				{
					AWAIT NameEntry(mgl,0);
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
						webInited = false;
						BeginUpload();
					}
					else
					{
						Web_Exit();
						webInited = false;
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

	CO_RETURN 0;
}

void RenderNetButton(int x, int y, int wid, const char *txt, MGLDraw *mgl, ButtonId buttonId)
{
	if (curButton == buttonId)
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
		RenderNetButton(110,370,130,"Okay!",mgl,ButtonId::Yes);
	else
		RenderNetButton(110,370,130,"Cancel Upload",mgl,ButtonId::No);
}

void RenderNameCheck(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Verifying your profile name...",0,2);

	RenderNetButton(110,370,130,"Cancel",mgl,ButtonId::No);
}

void RenderMOTDGet(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Downloading latest news...",0,2);

	RenderNetButton(110,370,130,"Cancel",mgl,ButtonId::No);
}

void RenderProfileUp(MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(100,100,540,400,32*1+4);
	mgl->Box(100,100,540,400,32*1+16);

	PrintGlow(102,102,"Uploading your profile...",0,2);

	RenderNetButton(110,370,130,"Cancel",mgl,ButtonId::No);
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
		if (!btn[i].steamOnly || SteamManager::Get()->IsSteamEdition())
		{
			RenderNetButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl, btn[i].id);
		}
	}

	//PrintGlow(20,245,"(Exits the game and opens a web browser)",0,2);
	//PrintGlow(20,305,"(Exits the game and opens a web browser)",0,2);
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
	ClearSpriteConstraints();
}

//----------------

TASK(void) NetMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitNetMenu(mgl);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=AWAIT UpdateNetMenu(&lastTime,mgl);
		RenderNetMenu(mgl);

		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	SaveProfile();
	ExitNetMenu();
}
