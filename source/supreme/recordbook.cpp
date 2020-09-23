#include "recordbook.h"
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
#include "goal.h"
#include "shop.h"
#include "scanner.h"
#include "lsdir.h"

#define PBTN_HEIGHT	19

static byte *backgd;
static sprite_set_t *plSpr;
static char msBright,msDBright;
static int msx,msy;
static dword totalKills,lvlsPassed;
static float shopPct,playPct,scanPct,gamePct,cardPct;
int recbook,page;

float CalcPlayPercent(void)
{
	int worlds;
	float score;
	worldData_t *tmp;

	worlds=0;

	score=0.0f;
	for (const char* name : filterdir("worlds", ".hsw", 32))
	{
		// rule out the backup worlds, so they don't show up
		if((strcmp(name,"backup_load.hsw")) &&
		   (strcmp(name,"backup_exit.hsw")) &&
		   (strcmp(name,"backup_save.hsw")) &&
		   (strcmp(name,"debug.hsw")))
		{
			tmp=GetWorldProgressNoCreate(name);
			if(tmp)
				score+=tmp->percentage;
			worlds++;
		}
	}

	return (score/(float)worlds);
}

void InitRecordBook(MGLDraw *mgl)
{
	int i;
	int t;

	msBright=0;
	msDBright=1;

	recbook = 0;
	page = 0;
	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	plSpr=new sprite_set_t("graphics/pause.jsp");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
	mgl->LastKeyPressed();
	mgl->MouseTap();

	totalKills=0;
	for(i=2;i<NUM_MONSTERS;i++)	// skip Bouapha
		totalKills+=profile.progress.kills[i];
	lvlsPassed=CountLevelsPassed();
	CalcFinishedWorlds();

	// 150 cals per mile, 1 cal per hammer throw
	// 1 cals per grass chopped
	profile.progress.calsBurned=(profile.progress.footDistance*150/168960)+profile.progress.hammersThrown+
		profile.progress.grassChopped;

	profile.progress.cards = 0;
	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(profile.progress.carded[i]>0)
			profile.progress.cards += profile.progress.carded[i];
	}

	shopPct=ShopPercent();
	playPct=CalcPlayPercent();
	scanPct=ScanPercent();
	cardPct=CardPercent();
	gamePct=playPct*0.5f+scanPct*0.05f+shopPct*0.2f+cardPct*0.15f;	// adds up to 90%
	t=0;
	for(i=0;i<120;i++)
		if(profile.progress.goal[i])
			t++;
	gamePct+=0.1f*((float)t);	// 10% total game progress for gallery goals
}

void ExitRecordBook(void)
{
	recbook = 0;
	free(backgd);
	delete plSpr;
}

byte UpdateRecordBook(int *lastTime,MGLDraw *mgl)
{
	char k;

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	k=mgl->LastKeyPressed();

	if(k==27)
		return 1;

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
	if(mgl->MouseTap())
	{
		if(PointInRect(msx,msy,560,440,560+60,440+PBTN_HEIGHT))
		{
			return 1;
		}
		else if(PointInRect(msx,msy,560,408,560+60,440+PBTN_HEIGHT))
		{
			if(page !=1)
				page = 1;
			else
				page = 0;
		}
	}

	return 0;
}

void RenderBookButton(int x,int y,int wid,const char *txt,MGLDraw *mgl)
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

void RenderStatPercent(int x,int y,const char *txt,float value)
{
	char s[16];

	PrintGlow(x,y,txt,0,2);
	sprintf(s,"%1.1f%%",value);
	PrintGlow(x+110-GetStrLength(s,2),y,s,0,2);
}

void RenderStat(int x,int y,const char *txt,dword value)
{
	char s[16];

	PrintGlow(x,y,txt,0,2);
	sprintf(s,"%lu",value);
	PrintGlow(x+250-GetStrLength(s,2),y,s,0,2);
}

void RenderStatOutOf(int x,int y,const char *txt,dword value,dword outof)
{
	char s[16];

	PrintGlow(x,y,txt,0,2);
	sprintf(s,"%lu/%lu",value,outof);
	PrintGlow(x+250-GetStrLength(s,2),y,s,0,2);
}

void RenderStatTime(int x,int y,const char *txt,dword value)
{
	char s[16];

	PrintGlow(x,y,txt,0,2);
	sprintf(s,"%lu:%02lu",(value/(30*60*60)),(value/(30*60))%60);
	PrintGlow(x+250-GetStrLength(s,2),y,s,0,2);
}

void RenderStatMiles(int x,int y,const char *txt,dword value)
{
	char s[16];

	PrintGlow(x,y,txt,0,2);
	sprintf(s,"%0.02f",(float)value/168960.0f);
	PrintGlow(x+250-GetStrLength(s,2),y,s,0,2);
}

void RenderRecordBook(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	if(page==0)
		RenderBookButton(560,408,60,"Next",mgl);
	else
		RenderBookButton(560,408,60,"Prev",mgl);
	RenderBookButton(560,440,60,"Exit",mgl);

	if (page == 0)
	{
		PrintGlow(20,20,"-- COMBAT --",0,2);
		RenderStat(20,40+18*0,"Hammers Thrown:",profile.progress.hammersThrown);
		RenderStat(20,40+18*1,"Damage Dealt:",profile.progress.damageDone);
		RenderStat(20,40+18*2,"Damage Taken:",profile.progress.damageTaken);
		RenderStat(20,40+18*3,"Monsters Slain:",totalKills);
		RenderStat(20,40+18*4,"Defeats:",profile.progress.kills[MONS_BOUAPHA]);
		RenderStat(20,40+18*5,"Weapons Fired:",profile.progress.shotsFired);
		RenderStat(20,40+18*6,"Rages:",profile.progress.rages);
		RenderStat(20,40+18*7,"Badguys Run Over:",profile.progress.runOver);
		RenderStatTime(20,40+18*8,"Time Spent Frozen:",profile.progress.frozen);
		RenderStatTime(20,40+18*9,"Time Spent Ignited:",profile.progress.ignited);
		RenderStatTime(20,40+18*10,"Time Spent Poisoned:",profile.progress.poisoned);

		PrintGlow(20,260,"-- ITEMS --",0,2);
		RenderStatOutOf(20,260+18*1,"Coins:",(profile.progress.totalCoins-profile.progress.coinsSpent),profile.progress.totalCoins);
		RenderStat(20,260+18*2,"Brains:",profile.progress.totalBrains);
		RenderStat(20,260+18*3,"Candles:",profile.progress.totalCandles);
		RenderStat(20,260+18*4,"Monster Cards:",profile.progress.cards);
		RenderStatOutOf(20,260+18*5,"Cheat Points:",(profile.progress.totalCheatPoints-profile.progress.cheatPointsSpent),profile.progress.totalCheatPoints);
		RenderStatOutOf(20,260+18*6,"Keys Of Lunacy:",(profile.progress.loonyKeys-profile.progress.loonyKeysUsed),profile.progress.loonyKeys);

		PrintGlow(350,20,"-- GAMEPLAY --",0,2);
		RenderStatTime(350,40+18*0,"Play Time:",profile.progress.totalTime);
		RenderStat(350,40+18*1,"Levels Passed:",lvlsPassed);
		RenderStat(350,40+18*2,"100% Worlds:",profile.progress.finishedWorlds);
		RenderStatMiles(350,40+18*3,"Miles Walked:",profile.progress.footDistance);
		RenderStatMiles(350,40+18*4,"Miles Rafted:",profile.progress.raftDistance);
		RenderStatMiles(350,40+18*5,"Miles Carted:",profile.progress.cartDistance);
		RenderStatMiles(350,40+18*6,"Miles Driven:",profile.progress.driveDistance);
		RenderStat(350,40+18*7,"Grass Chopped:",profile.progress.grassChopped);
		RenderStat(350,40+18*8,"Hay Burned:",profile.progress.hayBurned);
		RenderStat(350,40+18*9,"Keys Collected:",profile.progress.keysFound);
		RenderStat(350,40+18*10,"Doors Opened:",profile.progress.doorsOpened);
		RenderStat(350,40+18*11,"Calories Eaten:",profile.progress.calories);
		RenderStat(350,40+18*12,"Calories Burned:",profile.progress.calsBurned);
		RenderStatTime(350,40+18*13,"Breath Held:",profile.progress.underwaterTime);
		RenderStat(350,40+18*14,"Cheats Used:",profile.progress.cheats);
		
	RenderStatPercent(80,415,"SHOP:",shopPct);
	RenderStatPercent(240,415,"CARD:",cardPct);
	RenderStatPercent(400,415,"SCAN:",scanPct);
	RenderStatPercent(160,439,"PLAY:",playPct);
	RenderStatPercent(320,439,"GAME:",gamePct);
	}
	mgl->FillBox(18,400,640-19,400,32*1+10);
	
	if(page == 1)
	{
		msx2=20;
		msy2=48;
		PrintGlow(20,msy2-24,"Gallery Goals Completed",0,2);
		for(i=0;i<120;i++)
		{
			int c;
			if(i<30)
				c=7;
			else if(i<60)
				c=1;
			else if(i<90)
				c=5;
			else
				c=6;
			
			if(PointInRect(msx,msy,msx2,msy2,msx2+28,msy2+28))
			{
				DrawBox(msx2,msy2,msx2+28,msy2+28,32*c+16);
				PrintGoalInfo(20,308,i);
			}
			else
				DrawBox(msx2,msy2,msx2+28,msy2+28,32*c+8);
			if(profile.progress.goal[i])
				DrawFillBox(msx2+1,msy2+1,msx2+27,msy2+27,32*c+20);

			msx2+=32;
			if(msx2>640-24-120)
			{
				msx2=20;
				msy2+=32;
			}
		}
	}

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

void RecordBook(MGLDraw *mgl)
{
	int lastTime=1;

	InitRecordBook(mgl);

	while(recbook != 1)
	{
		lastTime+=TimeLength();
		StartClock();
		recbook=UpdateRecordBook(&lastTime,mgl);
		RenderRecordBook(mgl);

		mgl->Flip();

		if(!mgl->Process())
			recbook=1;
		EndClock();
	}
	ExitRecordBook();
}
