#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "pause.h"
#include "credits.h"
#include "music.h"
#include "achieve.h"
#include "gallery.h"
#include "leveldef.h"
#include "lsdir.h"
#if __linux__
#include <unistd.h>
#endif

#ifdef DIRECTORS
#define VERSION_NO	"Version 1.2CE"
#else
#define VERSION_NO	"Version 1.0O"
#endif

typedef struct save_t
{
	char name[16];
	byte realNum;
	byte newbie;
	byte level;
	byte madcap;
	dword playtime;
	float percentage;
	byte mod[3];
	char campName[64];
} save_t;

static byte oldc=0;
static byte saveMod[3];

byte gameToLoad;
static word timeToCred;

static byte cursor,submode,subcursor,curChar,numChars;
byte *backScr;
int numRuns;
static int ofsX;
char saveName[16];
static byte whoToDelete,helpTime;

#ifndef DIRECTORS
char menuChoices[MENU_CHOICES][16]={
	"Play",
	"Erase Character",
	"Achievements",
	"Game Options",
	"Exit",
};
#else
char menuChoices[MENU_CHOICES][16]={
	"Play",
	"Erase Character",
	"Achievements",
	"Gallery",
	"Editor",
	"Game Options",
	"Exit",
};
#endif

typedef struct addon_t
{
	char filename[12];
	char dispName[34];
} addon_t;

word addOnCount=0,addOnChoice=0;
addon_t *addOnList=NULL;

save_t save[MAX_CHARS];

char *FetchAddOnName(void)
{
	return addOnList[addOnChoice].filename;
}

void ClearAddOns(void)
{
	if(addOnList!=NULL)
	{
		free(addOnList);
		addOnList=NULL;
		addOnCount=0;
	}
}

void GetAddOn(char *name,int spot)
{
	FILE *f;
	char line[256];
	int pos;

	if(spot>=addOnCount || spot<0)
		return;

	addOnList[spot].dispName[0]='\0';
	addOnList[spot].filename[0]='\0';
	sprintf(line,"addons/%s",name);
	f=fopen(line,"rt");

	if(!f)
	{
		return;
	}
	if(fscanf(f,"%[^\n]\n",line)!=EOF)
	{
		strncpy(addOnList[spot].dispName,line,32);
		addOnList[spot].dispName[32]='\0';
		pos=strcspn(name,".");
		name[pos]='\0';
		pos=strcspn(name,"_");
		if(strlen(&name[pos+1])>10)
		{
			addOnList[spot].dispName[0]='\0';
			addOnList[spot].filename[0]='\0';
		}
		else
			strcpy(addOnList[spot].filename,&name[pos+1]);
	}
	fclose(f);
}

void GetAddOns(void)
{
	int done;

	// count up how many there are to deal with
	ClearAddOns();

	for (const char* name : filterdir("addons", ".txt"))
	{
		if (strncmp(name, "lvl_", 4))
			continue;
		addOnCount++;
	}

	addOnCount++;
	addOnList=(addon_t *)malloc(sizeof(addon_t)*addOnCount);


	strcpy(addOnList[0].dispName,"Original");
	addOnList[0].filename[0]='\0';

	done=1;
	for (char* name : filterdir("addons", ".txt"))
	{
		if (strncmp(name, "lvl_", 4))
			continue;
		GetAddOn(name, done);
		done++;
	}
}

void GetSavesForMenu(void)
{
	FILE *f;
	int i,n,hole,j;
	char txt[64];
	player_t p;

	curChar=0;
	n=0;
	hole=-1;
	for(i=0;i<MAX_CHARS;i++)
	{
		sprintf(txt,"profiles/char%02d.loony",i+1);
		f=fopen(txt,"rb");
		if(!f && hole==-1)
		{
			hole=i;
		}
		else if(f)
		{
			fread(&p,sizeof(player_t),1,f);
			fclose(f);
			save[n].percentage=CalcPercent(&p);
			save[n].newbie=0;
			save[n].realNum=i;
			save[n].playtime=p.playClock;
			save[n].level=p.level;
			save[n].madcap=p.var[VAR_MADCAP];
			save[n].mod[0]=p.var[VAR_MODIFIER+0];
			save[n].mod[1]=p.var[VAR_MODIFIER+1];
			save[n].mod[2]=p.var[VAR_MODIFIER+2];
			strcpy(save[n].name,p.profile);
			save[n].campName[0]='\0';
			for(j=1;j<addOnCount;j++)
				if(!strcmp(p.addonName,addOnList[j].filename))
					strcpy(save[n].campName,addOnList[j].dispName);
			n++;
		}
	}

	if(hole!=-1)
	{
		save[n].percentage=0.0;
		strcpy(save[n].name,"Loony");
		save[n].newbie=1;
		save[n].mod[0]=0;
		save[n].mod[1]=0;
		save[n].mod[2]=0;
		save[n].realNum=(byte)hole;
		save[n].campName[0]='\0';
		n++;
	}
	numChars=n;
}

void InitMainMenu(MGLDraw *mgl)
{
	int i;

	timeToCred=0;
	GetAddOns();
	GetSavesForMenu();
	helpTime=0;
	saveName[0]='\0';
	ofsX=0;
	submode=0;
	subcursor=0;
	cursor=0;

	mgl->LoadBMP(TITLEBMP);
	mgl->LastKeyPressed();
	GetTaps();
	oldc=255;
	ApplyControlSettings();
	JamulSoundVolume(opt.sound*2);
	SetMusicVolume(opt.music*2);

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	curChar=0;
	for(i=0;i<numChars;i++)
		if(save[i].realNum==opt.lastProfile)
		{
			curChar=i;
			whoToDelete=i;
		}

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],mgl->GetScreen()+mgl->GetWidth()*i,640);
}

char *ModifierList(byte n)
{
	static char s[64];
	int i;

	i=0;
	s[0]='\0';
	if(save[n].mod[0])
	{
		i++;
		strcat(s,ModifierName(save[n].mod[0]));
	}
	if(save[n].mod[1])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,ModifierName(save[n].mod[1]));
	}
	if(save[n].mod[2])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,ModifierName(save[n].mod[2]));
	}
	if(save[n].madcap)
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,"MADCAP!");
	}
	if(i==0)
		strcat(s,"No Modifiers");

	return s;
}

void ShowCharacter(int x,int y,byte n,byte on,MGLDraw *mgl)
{
	char s[32];
	int yy;

	x-=125;

	if(save[n].newbie)
	{
		if(on)
			PauseBox(x-4,y-4,x+250+4,y+100+4,5*32+16);
		PauseBox(x,y,x+250,y+100,234-5);
		CenterPrintGlow(x+125,y+8,"New Character",0,0);
	}
	else
	{
		if(on)
			PauseBox(x-4,y-4,x+250+4,y+100+4,5*32+16);
		PauseBox(x,y,x+250,y+100,234);
		CenterPrintGlow(x+125,y+2,save[n].name,0,0);
		CenterPrintGlow(x+125,y+28,ModifierList(n),0,1);
		yy=y+53;
		if(save[n].campName[0]!='\0')
		{
			CenterPrintGlow(x+125,y+42,save[n].campName,0,1);
		}

		sprintf(s,"Level %d",save[n].level);
		CenterPrintGlow(x+125,yy,s,0,0);
		sprintf(s,"%d:%02d",save[n].playtime/(60*60*30),(save[n].playtime/(60*30))%60);
		PrintGlow(x+8,yy+22,s,0,0);
		if(save[n].madcap)
			sprintf(s,"*%03.0f%%",save[n].percentage);
		else
			sprintf(s,"%03.0f%%",save[n].percentage);
		RightPrintGlow(x+250-8,yy+22,s,0,0);
	}
}

void CharaList(MGLDraw *mgl)
{
	int i;
	byte n;
	int x,y;

	for(i=5;i>0;i--)
	{
		n=curChar+i;

		if(n<numChars)
		{
			x=320+ofsX+i*100;
			y=200+((320-x)*(320-x))/800;
			ShowCharacter(x,y,n,0,mgl);
		}
		n=curChar-i;
		if(n<numChars)
		{
			x=320+ofsX-i*100;
			y=200+((320-x)*(320-x))/800;
			ShowCharacter(x,y,n,0,mgl);
		}
	}
	ShowCharacter(320+ofsX,200+abs(ofsX)/4,curChar,1,mgl);
}

void EraseCharDisplay(MGLDraw *mgl)
{
	char s[32];
	int x,y;

	x=-70;
	y=180;
	PauseBox(x+180,y+80,x+600,y+240,234);

	sprintf(s,"Really erase '%s'?",save[curChar].name);
	CenterPrintGlow(x+390,y+100,s,0,0);

	if(subcursor==0)
		PauseBox(x+250,y+200,x+380,y+235,32*5+16);
	else
		PauseBox(x+250,y+200,x+380,y+235,240);
	CenterPrintDark(x+315,y+204,"Yes",0);
	if(subcursor==1)
		PauseBox(x+400,y+200,x+530,y+235,32*5+16);
	else
		PauseBox(x+400,y+200,x+530,y+235,240);
	CenterPrintDark(x+465,y+204,"No",0);
}

void NewCharButton(int x,int y,byte on,const char *s,MGLDraw *mgl)
{
	if(on)
		PauseBox(x,y,x+100,y+35,32*5+16);
	else
		PauseBox(x,y,x+100,y+35,234+6);
	CenterPrintDark(x+50,y+5,s,0);
}

void NewCharDisplay(MGLDraw *mgl)
{
	static byte flip=0;

	flip=1-flip;

	PauseBox(120,50,520,400,234);
	CenterPrintGlow(320,58,"New Game",0,0);

	NewCharButton(125,100,(subcursor==0),"Name",mgl);

	NewCharButton(125,140,(subcursor==1),"Mod1",mgl);
	PrintDark(250,145,ModifierName(save[curChar].mod[0]),0);
	NewCharButton(125,180,(subcursor==2),"Mod2",mgl);
	PrintDark(250,185,ModifierName(save[curChar].mod[1]),0);
	NewCharButton(125,220,(subcursor==3),"Mod3",mgl);
	PrintDark(250,225,ModifierName(save[curChar].mod[2]),0);

	switch(subcursor)
	{
		case 0:
			PrintRectBlack2(125,265,515,350,"Type in a name for this profile.  Up and Down will select other options.",14,1);
			break;
		case 1:
		case 2:
		case 3:
			PrintRectBlack2(125,265,515,320,"Press Left or Right to flip through Modifiers.  You may apply up to 3 Modifiers to your game.  You'll unlock more as you play.",14,1);
			PrintRectBlack2(125,320,515,355,ModifierDesc(save[curChar].mod[subcursor-1]),14,1);
			break;
		case 4:
#ifdef DIRECTORS
			PrintRectBlack2(125,265,515,350,"Press Left or Right to choose which campaign to play.  \"Original\" is the normal game.  Any other campaign is at your own risk!  Press Fire to start playing!!  Or ESC to cancel.",14,1);
#else
			PrintRectBlack2(125,265,515,350,"Press Fire to start playing!!  Or ESC to cancel.",14,1);
#endif
			break;
	}

	PrintDark(250,105,save[curChar].name,0);
	if(flip && subcursor==0)
		PrintDark(250+GetStrLength(save[curChar].name,0),105,"_",0);

	NewCharButton(125,355,(subcursor==4),"Go!",mgl);
#ifdef DIRECTORS
	PrintDark(230,360,"Campaign to play:",1);
	PrintDark(230,376,addOnList[addOnChoice].dispName,1);
#endif
}

void MainMenuDisplay(MGLDraw *mgl)
{
	int i;
	char s[96];

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// version #:
	RightPrint(638,3,VERSION_NO,1,1);
	RightPrint(637,2,VERSION_NO,0,1);
	// Copyright:
	strcpy(s,"Copyright 2007, Hamumu Software");
	Print(2-1,467-1,s,0,1);
	Print(2+1,467+1,s,0,1);
	Print(2+1,467-1,s,0,1);
	Print(2-1,467+1,s,0,1);
	PrintDark(2,467,s,1);

	CenterPrint(320,4,LevelError(),0,1);
	if(submode==0 && helpTime>90)
	{
		strcpy(s,"Left & Right to change profile, Up & Down to select options, Fire or Enter to go!");
		CenterPrint(320-1,170-1,s,0,1);
		CenterPrint(320+1,170+1,s,0,1);
		CenterPrint(320+1,170-1,s,0,1);
		CenterPrint(320-1,170+1,s,0,1);
		CenterPrintDark(320,170,s,1);
	}

	CharaList(mgl);

	for(i=0;i<MENU_CHOICES;i++)
	{
#ifdef DIRECTORS
		if(cursor==i)
		{
			CenterPrintDark(320+1,310+1+i*24,menuChoices[i],0);
			CenterPrintDark(320-1,310-1+i*24,menuChoices[i],0);
			CenterPrint(320,310+i*24,menuChoices[i],0,0);
		}
		else
		{
			CenterPrintDark(320,310+i*24,menuChoices[i],0);
		}
#else
		if(cursor==i)
		{
			CenterPrintDark(320+1,320+1+i*33,menuChoices[i],0);
			CenterPrintDark(320-1,320-1+i*33,menuChoices[i],0);
			CenterPrint(320,320+i*33,menuChoices[i],0,0);
		}
		else
		{
			CenterPrintDark(320,320+i*33,menuChoices[i],0);
		}
#endif
	}

	if(submode==1)	// erase char
		EraseCharDisplay(mgl);
	if(submode==2)	// new char
		NewCharDisplay(mgl);
}

void DeleteCharacter(void)
{
	char s[64];

	sprintf(s,"profiles/char%02d.loony",save[whoToDelete].realNum+1);
	unlink(s);	// delete that file
	GetSavesForMenu();
	curChar=0;
}

byte MainMenuUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c,k;
	static byte reptCounter=0;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(submode==0)
			helpTime++;

		if(ofsX<0)
			ofsX+=10;
		if(ofsX>0)
			ofsX-=10;

		timeToCred++;
		// now real updating
		c=GetControls()|GetArrows();

		k=mgl->LastKeyPressed();

		if(c!=0 || k!=0)
			timeToCred=0;

		if(submode==0)	// normal menu
		{
			if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
			{
				cursor--;
				if(cursor>=MENU_CHOICES)
					cursor=MENU_CHOICES-1;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
			{
				cursor++;
				if(cursor>=MENU_CHOICES)
					cursor=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_LF) && ofsX==0 && curChar>0) //!(oldc&CONTROL_LF))
			{
				curChar--;
				if(curChar>=numChars)
					curChar=numChars-1;
				whoToDelete=curChar;
				ofsX=-100;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_RT) && ofsX==0 && curChar<numChars-1) //!(oldc&CONTROL_RT))
			{
				curChar++;
				ofsX=100;
				if(curChar>=numChars)
					curChar=0;
				whoToDelete=curChar;
				MakeNormalSound(SND_MENUCLICK);
			}

			if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
			{
				switch(cursor)
				{
					case MM_PLAY:	// play
						if(save[curChar].newbie)
						{
							MakeNormalSound(SND_MENUSELECT);
							gameToLoad=save[curChar].realNum;
							opt.lastProfile=gameToLoad;
							submode=2;
							subcursor=0;
						}
						else
						{
							MakeNormalSound(SND_MENUSELECT);
							gameToLoad=save[curChar].realNum;
							opt.lastProfile=gameToLoad;
							return MENU_PLAY;
						}
						break;
					case MM_ERASE:	// erase guy
						if(save[curChar].newbie==0)
						{
							MakeNormalSound(SND_MENUSELECT);
							submode=1;
							subcursor=1;
						}
						else
							MakeNormalSound(SND_MENUCANCEL);
						break;
					case MM_ACHIEVE:
						AchieveMenu(mgl,backScr);
						break;
#ifdef DIRECTORS
					case MM_GALLERY:
						if(HaveGallery())
							Gallery(mgl);
						else
							MakeNormalSound(SND_MENUCANCEL);
						break;
					case MM_EDITOR:
						return MENU_EDITOR;
						break;
#endif
					case MM_OPTIONS:	// options
						OptionsMenu(mgl,backScr);
						break;
					case MM_QUIT:	// exit
						MakeNormalSound(SND_MENUSELECT);
						return MENU_EXIT;
						break;
				}
			}
			if(k==27)
				return MENU_EXIT;
#ifdef _DEBUG
			if(k=='e' || k=='E')
				return MENU_EDITOR;
#endif
		}
		else if(submode==1)	// sure you want to delete?
		{
			if((c&CONTROL_LF) && !(oldc&CONTROL_LF))
			{
				subcursor=1-subcursor;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_RT) && !(oldc&CONTROL_RT))
			{
				subcursor=1-subcursor;
				MakeNormalSound(SND_MENUCLICK);
			}

			if((c&(CONTROL_B1)) && !(oldc&(CONTROL_B1)))
			{
				switch(subcursor)
				{
					case 0:	// yep, slice him up
						MakeNormalSound(SND_MENUSELECT);
						DeleteCharacter();
						whoToDelete=0;
						subcursor=0;
						submode=0;
						cursor=0;
						break;
					case 1:	// nevermind
						MakeNormalSound(SND_MENUSELECT);
						subcursor=0;
						submode=0;
						cursor=0;
						break;
				}
			}
			if((c&(CONTROL_B2)) && !(oldc&(CONTROL_B2)))
			{
				// nevermind
				MakeNormalSound(SND_MENUCANCEL);
				subcursor=0;
				submode=0;
				cursor=0;
			}
			if(k==27)
			{
				MakeNormalSound(SND_MENUCANCEL);
				subcursor=0;
				submode=0;
				cursor=0;
			}
		}
		else if(submode==2)	// make a new char
		{
			if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
			{
				subcursor--;
				if(subcursor>4)
					subcursor=4;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
			{
				subcursor++;
				if(subcursor>4)
					subcursor=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_LF) && !(oldc&CONTROL_LF))
			{
				if(subcursor>=1 && subcursor<=3)
				{
					AdjustModifier(&save[curChar].mod[subcursor-1],subcursor-1,-1,save[curChar].mod);
					MakeNormalSound(SND_MENUCLICK);
				}
#ifdef DIRECTORS
				if(subcursor==4)
				{
					addOnChoice--;
					while(addOnChoice<0 || addOnChoice>=addOnCount || (addOnList[addOnChoice].filename[0]=='\0' && addOnChoice!=0))
						addOnChoice--;
				}
#endif
			}
			if((c&CONTROL_RT) && !(oldc&CONTROL_RT))
			{
				if(subcursor>=1 && subcursor<=3)
				{
					AdjustModifier(&save[curChar].mod[subcursor-1],subcursor-1,1,save[curChar].mod);
					MakeNormalSound(SND_MENUCLICK);
				}
#ifdef DIRECTORS
				if(subcursor==4)
				{
					addOnChoice++;
					while(addOnChoice<0 || addOnChoice>=addOnCount || (addOnList[addOnChoice].filename[0]=='\0' && addOnChoice!=0))
						addOnChoice++;
				}
#endif
			}
			if((c&(CONTROL_B1)) && !(oldc&(CONTROL_B1)))
			{
				switch(subcursor)
				{
					case 0:	// don't do anything, just enter that name
						break;
					case 1:	// modifiers
					case 2:
					case 3:
						break;
					case 4:	// go!
						MakeNormalSound(SND_MENUSELECT);
						gameToLoad=save[curChar].realNum;
						strcpy(saveName,save[curChar].name);
						saveMod[0]=save[curChar].mod[0];
						saveMod[1]=save[curChar].mod[1];
						saveMod[2]=save[curChar].mod[2];
						return MENU_NEWCHAR;
						break;
				}
			}
			if(k==27)
			{
				MakeNormalSound(SND_MENUCANCEL);
				subcursor=0;
				submode=0;
				cursor=0;
			}
			if(k==8 && subcursor==0)
			{
				// backspace
				if(strlen(save[curChar].name)>0)
				{
					save[curChar].name[strlen(save[curChar].name)-1]='\0';
					MakeNormalSound(SND_MENUCLICK);
				}
			}
			if(k==13 && subcursor==0)
			{
				MakeNormalSound(SND_MENUCLICK);
				subcursor=1;
			}
			if((k>='a' && k<='z') || (k>='A' && k<='Z') || (k>='0' && k<='9') && subcursor==0)
			{
				if(strlen(save[curChar].name)<10)
				{
					MakeNormalSound(SND_MENUCLICK);
					save[curChar].name[strlen(save[curChar].name)+1]='\0';
					save[curChar].name[strlen(save[curChar].name)]=k;
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
			}
		}

		oldc=c;

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 0;
}

byte MainMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	InitMainMenu(mgl);

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();

		b=MainMenuUpdate(&lastTime,mgl);
		MainMenuDisplay(mgl);

		mgl->Flip();
		if(!mgl->Process())
		{
			free(backScr);
			return 255;
		}
		EndClock();
		if(timeToCred>30*20)
		{
			timeToCred=0;
			Credits(mgl,0);
		}
	}
	free(backScr);

	return b;
}

char *GetSavedName(void)
{
	return saveName;
}

byte GetSavedMod(byte n)
{
	return saveMod[n];
}
