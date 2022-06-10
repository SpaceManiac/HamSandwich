#include "parallel.h"
#include "map.h"
#include "guy.h"
#include "game.h"
#include "options.h"
#include "score.h"
#include "floor.h"
#include "world.h"
#include "stars.h"
#include "highscore.h"
#include "giftshop.h"
#include "music.h"
#include "palettes.h"

// the parallel universe menu!
static byte *backScr;
static byte cursor;
static byte txtLevel[6];
static sprite_set_t *blowSpr;
static byte play;
static int oldmsx,oldmsy,oldMsBtn;
static byte pickMode;
static byte month=0,week=0;
static byte moPic;
static byte moUpClock;
static byte smarch=0;

static char monthName[16][13]={
		"yraunaJ",
		"yraurbeF",
		"hcraM",
		"lirpA",
		"yaM",
		"enuJ",
		"yluJ",
		"tsuguA",
		"rebmetpeS",
		"rebotcO",
		"rebmevoN",
		"rebmeceD",
		"hcramS",
	};

static void UpdateBackgd(void);

void InitParallelMenu(void)
{
	int i;

	if(month==12)
	{
		month=2;
		smarch=1;
	}
	if(month==255)
	{
		month=0;
		week=0;
	}

	ReversePalette(GetDisplayMGL());
	JamulSoundPurge();
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();
	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	GetArrowTaps();
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");


	moPic=200;
	moUpClock=14;
	UpdateBackgd();

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	cursor=0;

	for(i=0;i<6;i++)
		txtLevel[i]=0;

	blowSpr=new sprite_set_t("graphics/blowout.jsp");

	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);
	oldMsBtn=255;

	LoadOptions();
	pickMode=0;
	if(month!=2 || !smarch)
		InitStars(profile.starGot[GAME_PARALLEL][month*4+week],0);
	else
		InitStars(profile.starGot[GAME_PARALLEL][12*4+week],0);
	moUpClock=0;
}

void ExitParallelMenu(void)
{
	delete blowSpr;
	free(backScr);
	SaveOptions();
}

static void UpdateBackgd(void)
{
	char s[32];
	int i;
	byte mo;

	if(month==255)
		return;

	mo=month;

	if(month==2 && smarch)
		mo=12;

	if(moPic!=mo)
	{
		moUpClock++;
	}
	else
		moUpClock=0;
	if(moPic!=mo && moUpClock>=15)
	{
		moPic=mo;
		moUpClock=0;
		sprintf(s,"graphics/stockroom%02d.bmp",moPic);
		GetDisplayMGL()->LoadBMP(s, nullptr);

		for(i=0;i<480;i++)
			memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	}
}

static void ToggleSmarch(void)
{
	smarch=1-smarch;
	if(smarch)
		strcpy(monthName[2],"hcramS");
	else
		strcpy(monthName[2],"hcraM");
}

byte UpdateParallelMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte t;
	static byte fwClock;
	byte btn;
	int msx,msy;
	int i,j;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;

	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		UpdateBackgd();

		for(t=0;t<6;t++)
		{
			if(cursor==t && txtLevel[t]<8)
				txtLevel[t]++;
			if(cursor!=t && txtLevel[t]>0)
				txtLevel[t]--;
		}

		mgl->GetMouse(&msx,&msy);
		btn=mgl->MouseDown();

		c=mgl->LastKeyPressed();
		t=GetTaps()|GetArrowTaps();

		switch(pickMode)
		{
			case 0:	// picking a week
				if(month==255)
					cursor=0;
				else
					cursor=8;

				if(c==27)
				{
					play=0;
					return 0;
				}

				if(month==255)
				{
					if(t&CONTROL_UP)
					{
						month=8;
						week=3;
					}
					if(t&CONTROL_DN)
					{
						month=0;
						week=0;
					}
					if(t&CONTROL_B1)
					{
						play=0;
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					}
				}
				else
				{
					if(t&CONTROL_RT)
					{
						if((month&3)<3)
							month++;
						else
							month-=3;
					}
					if(t&CONTROL_LF)
					{
						if((month&3)>0)
							month--;
						else
							month+=3;
					}
					if(t&CONTROL_UP)
					{
						week--;
						if(week>3)
						{
							if(month==2 && profile.giftShop[GIFT_SMARCH])
							{
								ToggleSmarch();
								week=0;
							}
							else
							{
								week=3;
								month-=4;
								if(month>11)
								{
									month=255;
								}
							}
						}
					}
					if(t&CONTROL_DN)
					{
						week++;
						if(week>3)
						{
							week=0;
							month+=4;
							if(month>11)
							{
								month=255;
							}
						}
					}
					if(month!=255)
					{
						if(t&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
						{
							if(month!=2 || !smarch)
								InitStars(profile.starGot[GAME_PARALLEL][month*4+week],0);
							else
								InitStars(profile.starGot[GAME_PARALLEL][12*4+week],0);
						}
						if(t&CONTROL_B1)
						{
							pickMode=1;
							cursor=0;
						}
					}
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=0;i<4;i++)
					{
						for(j=0;j<3;j++)
						{
							if(msx>i*160 && msx<i*160+160 && msy>j*88+24 && msy<j*88+88)
							{
								month=i+j*4;
								// we know which month we're in
								if(msy<j*88+24+16)
									week=0;
								else if(msy<j*88+24+16*2)
									week=1;
								else if(msy<j*88+24+16*3)
									week=2;
								else
									week=3;
								if(month!=2 || !smarch)
									InitStars(profile.starGot[GAME_PARALLEL][month*4+week],0);
								else
									InitStars(profile.starGot[GAME_PARALLEL][12*4+week],0);
							}
						}
					}
					if(msx>10 && msx<160 && msy>310-16 && msy<310+20)
					{
						if(month!=255)
							MakeNormalSound(SND_MENUCLICK);
						month=255;
					}

				}
				if(btn && !oldMsBtn)
				{
					if(msx>2*160 && msx<2*160+160 && msy<24)
					{
						if(profile.giftShop[GIFT_SMARCH])
							ToggleSmarch();
					}
					for(i=0;i<4;i++)
					{
						for(j=0;j<3;j++)
						{
							if(msx>i*160 && msx<i*160+160 && msy>j*88+24 && msy<j*88+88)
							{
								month=i+j*4;
								// we know which month we're in
								if(msy<j*88+24+16)
									week=0;
								else if(msy<j*88+24+16*2)
									week=1;
								else if(msy<j*88+24+16*3)
									week=2;
								else
									week=3;
								if(month!=2 || !smarch)
									InitStars(profile.starGot[GAME_PARALLEL][month*4+week],0);
								else
									InitStars(profile.starGot[GAME_PARALLEL][12*4+week],0);
								pickMode=1;
								cursor=0;
							}
						}
					}
					if(msx>10 && msx<160 && msy>310-16 && msy<310+19)
					{
						play=0;
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					}
				}
				break;
			case 1: // picking what to do with your week
				if(c==27)
				{
					pickMode=0;
					MakeNormalSound(SND_MENUCANCEL);
				}
				if(t&CONTROL_DN)
				{
					cursor++;
					if(cursor>3)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(t&CONTROL_UP)
				{
					cursor--;
					if(cursor>3)
						cursor=3;
					MakeNormalSound(SND_MENUCLICK);
				}

				if(t&(CONTROL_B1|CONTROL_B2))
				{
					switch(cursor)
					{
						case 0:	// begin
							play=1;
							MakeNormalSound(SND_MENUSELECT);
							return 0;
						case 1:	// chg map
							pickMode=0;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 2:	// clear scores
							pickMode=2;
							cursor=0;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 3:	// exit
							play=0;
							MakeNormalSound(SND_MENUSELECT);
							return 0;
					}
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=0;i<4;i++)
					{
						if(msx>10 && msx<160 && msy>310-16+i*35 && msy<310+19+i*35)
						{
							if(cursor!=i)
								MakeNormalSound(SND_MENUCLICK);
							cursor=i;
						}
					}
				}
				if(btn && !oldMsBtn)
				{
					for(i=0;i<4;i++)
					{
						if(msx>10 && msx<160 && msy>310-16+i*35 && msy<310+19+i*35)
						{
							switch(cursor)
							{
								case 0:	// begin
									play=1;
									MakeNormalSound(SND_MENUSELECT);
									return 0;
								case 1:	// chg map
									pickMode=0;
									MakeNormalSound(SND_MENUSELECT);
									break;
								case 2:	// clear scores
									pickMode=2;
									cursor=0;
									MakeNormalSound(SND_MENUSELECT);
									break;
								case 3:	// exit
									play=0;
									MakeNormalSound(SND_MENUSELECT);
									return 0;

							}
						}
					}
					if(msx>2*160 && msx<2*160+160 && msy<24)
					{
						if(profile.giftShop[GIFT_SMARCH])
							ToggleSmarch();
					}
					for(i=0;i<4;i++)
					{
						for(j=0;j<3;j++)
						{
							if(msx>i*160 && msx<i*160+160 && msy>j*88+24 && msy<j*88+88)
							{
								byte oldMonth,oldWeek;

								oldMonth=month;
								oldWeek=week;

								month=i+j*4;
								// we know which month we're in
								if(msy<j*88+24+16)
									week=0;
								else if(msy<j*88+24+16*2)
									week=1;
								else if(msy<j*88+24+16*3)
									week=2;
								else
									week=3;
								if(month!=2 || !smarch)
									InitStars(profile.starGot[GAME_PARALLEL][month*4+week],0);
								else
									InitStars(profile.starGot[GAME_PARALLEL][12*4+week],0);
								pickMode=1;

								if(oldMonth==month && oldWeek==week)
								{
									play=1;	// re-clicking the same level starts play
									MakeNormalSound(SND_MENUSELECT);
									return 0;
								}
							}
						}
					}
				}
				break;
			case 2:	// really want to change high scores?
				if(t&CONTROL_DN)
				{
					cursor++;
					if(cursor>1)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(t&CONTROL_UP)
				{
					cursor--;
					if(cursor>1)
						cursor=1;
					MakeNormalSound(SND_MENUCLICK);
				}

				if(t&(CONTROL_B1|CONTROL_B2))
				{
					switch(cursor)
					{
						case 0:	// don't clear scores
							pickMode=1;
							cursor=2;
							MakeNormalSound(SND_MENUCANCEL);
							break;
						case 1:	// clear them
							if(month==2 && smarch)
								ResetHighScore(GAME_PARALLEL,12*4+week);
							else
								ResetHighScore(GAME_PARALLEL,month*4+week);
							pickMode=1;
							cursor=2;
							MakeNormalSound(SND_MENUSELECT);
							break;
					}
				}
				if(c==27)
				{
					pickMode=1;
					cursor=3;
					MakeNormalSound(SND_MENUCANCEL);
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=0;i<2;i++)
					{
						if(msx>10 && msx<200 && msy>310+i*40 && msy<310+25+i*40)
						{
							if(cursor!=i)
								MakeNormalSound(SND_MENUCLICK);
							cursor=i;
						}
					}
				}
				if(btn && !oldMsBtn)
				{
					for(i=0;i<2;i++)
					{
						if(msx>10 && msx<200 && msy>310+i*40 && msy<310+25+i*40)
						{
							cursor=i;
							switch(cursor)
							{
								case 0:	// don't clear scores
									pickMode=1;
									cursor=2;
									MakeNormalSound(SND_MENUCANCEL);
									break;
								case 1:	// clear them
									if(month==2 && smarch)
										ResetHighScore(GAME_PARALLEL,12*4+week);
									else
										ResetHighScore(GAME_PARALLEL,month*4+week);
									pickMode=1;
									cursor=2;
									MakeNormalSound(SND_MENUSELECT);
									break;
							}
						}
					}
				}
				break;
		}

		oldMsBtn=btn;
		oldmsx=msx;
		oldmsy=msy;

		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

void RenderParallelMenu(MGLDraw *mgl)
{
	char txt[4][32]={
		"Work This keeW",
		"Pick Another keeW",
		"Reset High Scores",
		"Exit",
	};
	char yesnotxt[2][32]={
		"Nevermind!",
		"Really Reset High Scores",
	};

	char exitTxt[32]="Exit To Menu";

	int i,j,x;
	byte sg;
	char s[48];

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			if(month==i+j*4)
			{
				mgl->FillBox(160*i+1,1+88*j,160*i+158,22+88*j,32*6+16);
				CenterPrintGlow(160*i+80,2+88*j,monthName[i+j*4],0,0);
				mgl->FillBox(160*i+1,24+16*week+88*j,160*i+158,24+14+16*week+88*j,32*6+8);
				sprintf(s,"%d keeW",week+1);
				CenterPrintGlow(160*i+80,23+3+16*week+88*j,s,0,1);
			}
			else
				CenterPrintUnGlow(160*i+80,2+88*j,monthName[i+j*4],0);

			for(x=0;x<4;x++)
			{
				sg=profile.starGot[GAME_PARALLEL][(i+j*4)*4+x];
				if((i+j*4)==2 && smarch)
					sg=profile.starGot[GAME_PARALLEL][12*4+x];
				switch(sg)
				{
					case 0:	// no star at all
						break;
					case 1:	// no star, but passed the level
						RenderStar(0,34,(160*i+130)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						RenderStar(0,34,(160*i+30)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						break;
					case 2:	// bronze
						RenderStar(2,33,(160*i+130)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						RenderStar(2,33,(160*i+30)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						break;
					case 3:	// silver
						RenderStar(0,33,(160*i+130)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						RenderStar(0,33,(160*i+30)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						break;
					case 4:	// gold
						RenderStar(5,33,(160*i+130)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						RenderStar(5,33,(160*i+30)*FIXAMT,(88*j+24+6+16*x)*FIXAMT,mgl);
						break;
				}
			}
			sg=profile.zodiac2[i+j*4];
			if((i+j*4)==2 && smarch)
				sg=profile.zodiac2[12];
			if(sg)
				RenderStar(5,4,(160*i+25)*FIXAMT,(88*j+14)*FIXAMT,mgl);
			sg=profile.empMonth2[i+j*4];
			if((i+j*4)==2 && smarch)
				sg=profile.empMonth2[12];
			if(sg)
				RenderStar(5,30,(160*i+130)*FIXAMT,(88*j+12)*FIXAMT,mgl);
		}
	}
	if(profile.empYear2)
		RenderStar(5,31,320*FIXAMT,140*FIXAMT,mgl);

	if(month!=255)
	{
		sprintf(s,"%d keeW, %s",week+1,monthName[month]);
		RightPrintUnGlow(635,270,s,0);

		RenderStartStar(450*FIXAMT,370*FIXAMT,mgl);
		if(month!=2 || !smarch)
		{
			RenderHighScore(GAME_PARALLEL,month*4+week,2,500,315,mgl);
			RenderHighScore(GAME_PARALLEL,month*4+week,1,500,315+50,mgl);
			RenderHighScore(GAME_PARALLEL,month*4+week,0,500,315+50*2,mgl);
		}
		else
		{
			RenderHighScore(GAME_PARALLEL,12*4+week,2,500,315,mgl);
			RenderHighScore(GAME_PARALLEL,12*4+week,1,500,315+50,mgl);
			RenderHighScore(GAME_PARALLEL,12*4+week,0,500,315+50*2,mgl);
		}
	}

	switch(pickMode)
	{
		case 0:	// pick a week
			PrintUnGlow(10,310,exitTxt,0);
			for(x=0;x<8;x++)
			{
				if(txtLevel[0]>x)
				{
					if(x==7)
					{
						Print(10+x-1,310-x-1,exitTxt,-32,0);
						Print(10+x+1,310-x+1,exitTxt,-32,0);
						Print(10+x-1,310-x+1,exitTxt,-32,0);
						Print(10+x+1,310-x-1,exitTxt,-32,0);
					}
					Print(10+x,310-x,exitTxt,-32+x*6,0);
				}
			}
			PrintUnGlow(5,450,"Select a keew to work in",0);
			break;
		case 1:	// choices per week
			for(i=0;i<4;i++)
			{
				PrintUnGlow(10,310+i*35,txt[i],0);
				for(x=0;x<8;x++)
				{
					if(txtLevel[i]>x)
					{
						if(x==7)
						{
							Print(10+x-1,310-x+i*35-1,txt[i],-32,0);
							Print(10+x+1,310-x+i*35+1,txt[i],-32,0);
							Print(10+x-1,310-x+i*35+1,txt[i],-32,0);
							Print(10+x+1,310-x+i*35-1,txt[i],-32,0);
						}

						Print(10+x,310-x+i*35,txt[i],-32+x*6,0);
					}
				}
			}
			break;
		case 2: // clear high scores
			for(i=0;i<2;i++)
			{
				PrintUnGlow(10,310+i*40,yesnotxt[i],0);
				for(x=0;x<8;x++)
				{
					if(txtLevel[i]>x)
					{
						if(x==7)
						{
							Print(10+x-1,310-x+i*40-1,yesnotxt[i],-32,0);
							Print(10+x+1,310-x+i*40+1,yesnotxt[i],-32,0);
							Print(10+x-1,310-x+i*40+1,yesnotxt[i],-32,0);
							Print(10+x+1,310-x+i*40-1,yesnotxt[i],-32,0);
						}

						Print(10+x,310-x+i*40,yesnotxt[i],-32+x*6,0);
					}
				}
			}
			break;
	}

	blowSpr->GetSprite(9)->Draw(oldmsx,oldmsy,mgl);
}

TASK(byte) ParallelMenu(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;
	char s[32];
	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitParallelMenu();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateParallelMenu(&lastTime,mgl);
		RenderParallelMenu(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			ExitParallelMenu();
			CO_RETURN WORLD_ABORT;
		}
		EndClock();
	}
	then=timeGetTime();
	ResetClock(hangon);
	ExitParallelMenu();

	if(play)
	{
		if(month==2 && smarch)
			month=12;
		sprintf(s,"levels/pu%02d.sbl",month);
		player.levelNum=week;
		CO_RETURN AWAIT LunaticWorld(month,s);
	}
	else
		CO_RETURN WORLD_ABORT;
}

char *RvsNameLevel(void)
{
	static char nameStr[32];

	sprintf(nameStr,"%d keeW - %s",player.levelNum+1,monthName[player.worldNum]);
	return nameStr;
}
