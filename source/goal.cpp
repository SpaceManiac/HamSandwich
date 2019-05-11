#include "goal.h"
#include "progress.h"
#include "editor.h"
#include "monster.h"
#include "player.h"
#include "bullet.h"
#include "shop.h"
#include "theater.h"
#include "gallery.h"

char goalDesc[][48]={
	// 0
	"Yriry Hc!","ernpuvat yriry 20",	// "Level Up!","reaching level 20",
	"Synzr Ba!","yriryvat Gur Synzroevatre gb 10",	// "Flame On!","leveling The Flamebringer to 10",
	"Ohyyrgcebbs","yriryvat Gur Yvtugernire gb 10",	// "Bulletproof","leveling The Lightreaver to 10",
	"Cynarg, Fznfurq","yriryvat Gur Cynargfznfure gb 10", // "Planet, Smashed","leveling The Planetsmasher to 10",
	"Fcnexyvat!","yriryvat Gur Fcnexguebjre gb 10",	// "Sparkling!","leveling The Sparkthrower to 10",
	"JUNG QVQ LBH FNL?","yriryvat Gur Rnefcyvggre gb 10",	// "WHAT DID YOU SAY?","leveling The Earsplitter to 10",
	"Orfg Ohqql","yriryvat Gur Obarpehfure gb 10",	// "Best Buddy","leveling The Bonecrusher to 10",
	"Oenval","pbyyrpgvat 100 Oenvaf",	// "Brainy","collecting 100 Brains",
	"Rzretrapl Fhccyvrf","pbyyrpgvat 100 Pnaqyrf",	// "Emergency Supplies","collecting 100 Candles",
	"Fyrrcyrff Ureb","jvaavat gur tnzr",	// "Sleepless Hero","winning the game",
	//10
	"Unccl Gevpx","jvcvat bhg gur Unccl Fgvpx Gerrf",	// "Happy Trick","wiping out the Happy Stick Trees",
	"Fhcre Fubccre","ohlvat bhg Ze. Xyrva'f fubc",	// "Super Shopper","buying out Mr. Klein's shop",
	"Fcrrql Ubyybj","svavfuvat va haqre 4 ubhef",	// "Speedy Hollow","finishing in under 4 hours",
	"Cresrpgvbavfz","trggvat 100% pbzcyrgr",	// "Perfectionism","getting 100% complete",
	"Natel Puvpxra","fcryyvat UVPX FBAT",	// "Angry Chicken","spelling HICK SONG",
	"Ebpx Yrtraq","ebpxvat 30 abgrf va n ebj", // "Rock Legend","rocking 30 notes in a row",
	"Frira Lrnef","fznfuvat nyy bs lbhe ersyrpgvbaf", // "Seven Years","smashing all of your reflections",
	"Gnxvat Abgrf","pbyyrpgvat nyy Wbheany Cntrf", // "Taking Notes","collecting all Journal Pages",
	"Tbnyqvttre","qvttvat hc n tbny", // "Goaldigger","digging up a goal",
	"Nhgbtencurq Pbcl","nfxvat sbe vg", // "Autographed Copy","asking for it",
	// 20
	"Tbar Svfuva'","pngpuvat bar bs rirel jvyq svfu", // "Gone Fishin'","catching one of every wild fish",
	"Navzny Erfphr","pngpuvat gur hajnagrq tbyqsvfu", // "Animal Rescue","catching the unwanted goldfish",
	"Vg Jnf Guvf Ovt!","pngpuvat n 150yo svfu", // "It Was This Big!","catching a 150lb fish",
	"Svfuznfgre","n svfuvat fxvyy bs 100",	// "Fishmaster","a fishing skill of 100",
	"V'q Engure Or...","pngpuvat 200 svfu", 	// "I'd Rather Be...","catching 200 fish",
};

static char tmptxt[48];

char *ConvertText(char *src)
{
	int i;
	byte b;

	tmptxt[strlen(src)]='\0';
	for(i=0;i<(int)strlen(src);i++)
	{
		if(src[i]>='a' && src[i]<='z')
		{
			b=(byte)src[i];
			b+=13;
			if(b>(byte)'z')
				b-=26;
			tmptxt[i]=(char)b;
		}
		else if(src[i]>='A' && src[i]<='Z')
		{
			b=(byte)src[i];
			b+=13;
			if(b>(byte)'Z')
				b-=26;
			tmptxt[i]=(char)b;
		}
		else
			tmptxt[i]=src[i];
	}
	return tmptxt;
}

void PrintGoalInfo(int x,int y,byte goal)
{
	char s[128];

	sprintf(s,"#%d: ",goal+1);

	if(profile.progress.goal[goal])
	{
		strcat(s,ConvertText(goalDesc[goal*2]));
	}
	else
		strcat(s,"???????");

	PrintUnGlow(x,y,s,1);
	if(profile.progress.goal[goal])
	{
		sprintf(s,"Earned for %s.",ConvertText(goalDesc[goal*2+1]));
		PrintUnGlowRect(x,y+16,316,y+16+40,12,s,1);
	}
}

void CompleteGoal(byte goal)
{
#ifndef DEMO
	if(profile.progress.goal[goal])
		return;

	profile.progress.goal[goal]=1;
	ShowGoalEarned(goal);
#endif
}

void GoalKilledSomebody(Guy *g,byte type,byte frozen)
{


}

void GoalTallyPage(int score,byte perfect,byte destruct,int time)
{
}

void GoalTimeDist(void)
{
}

void GoalPurchase(void)
{
}

void GoalWinLevel(void)
{
}

void GoalFire(void)
{
}

// -------------------------------------------------
static byte noKeyTime;
static msBtn;

void InitShowGoal(MGLDraw *mgl,byte num)
{
	char txt[64];
	int i,pg;

	noKeyTime=60;

	MakeNormalSound(SND_ALLCANDLE);
	pg=0;
	for(i=0;i<NUM_GOALS;i++)
		if(profile.progress.goal[i])
			pg++;
	pg--;
	sprintf(txt,"graphics\\comic%d.bmp",pg);
	GetDisplayMGL()->LoadBMP(txt);

	sprintf(txt,"\"%s\"",ConvertText(goalDesc[num*2]));
	Print(638-GetStrLength(txt,0),0,txt,0,0);
	sprintf(txt,"Comic Page earned for %s.",ConvertText(goalDesc[num*2+1]));
	PrintUnGlow(2,450,txt,2);

	GetTaps();
	GetArrowTaps();
	mgl->MouseTap();
	mgl->RMouseTap();
	mgl->LastKeyPressed();
}

void ExitShowGoal(void)
{
	GetDisplayMGL()->ClearScreen();
	GetDisplayMGL()->Flip();

	RestoreGameplayGfx();
}

byte UpdateShowGoal(int *lastTime,MGLDraw *mgl)
{
	byte t;
	char k;

	k=mgl->LastKeyPressed();
	t=GetTaps()|GetArrowTaps();
	if(mgl->MouseTap())
		t|=CONTROL_B1;
	if(mgl->RMouseTap())
		t|=CONTROL_B2;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
		if(noKeyTime)
			noKeyTime--;
	}

	if(!noKeyTime)
	{
		if(k==27)
			return 1;

		if(t&(CONTROL_B1|CONTROL_B2))
			return 1;
	}

	return 0;
}

void RenderShowGoal(MGLDraw *mgl)
{
}

void ShowGoalEarned(byte num)
{
	byte done=0;
	int lastTime=1;

	InitShowGoal(GetDisplayMGL(),num);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateShowGoal(&lastTime,GetDisplayMGL());
		RenderShowGoal(GetDisplayMGL());

		GetDisplayMGL()->Flip();

		if(!GetDisplayMGL()->Process())
			done=1;
		EndClock();
	}

	ExitShowGoal();
}
