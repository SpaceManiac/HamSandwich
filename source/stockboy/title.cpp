#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "pause.h"
#include "profile.h"
#include "monster.h"
#include "player.h"
#include "particle.h"
#include "stars.h"
#include "giftshop.h"
#include "music.h"
#include "appdata.h"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][32]={
	"STOCKBOY",
	"",
	"Copyright 2002, Hamumu Software",
	"#",
	"Design & Concept",
	"Mike Hommel",
	"%",
	"Programming",
	"Mike Hommel",
	"%",
	"Level Design",
	"Max Beerens",
	"Mike Hommel",
	"Lisa Keck",
	"Ryan Keck",
	"TD Miller",
	"Donna Vaporis",
	"%",
	"Artwork",
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Mike Hommel",
	"Solange Hunt",
	"%",
	"Music",
	"Simon \"Himiona\" Castles",
	"Mike Hommel",
	"%",
	"Testing",
	"Solange Hunt",
	"Lisa Keck",
	"Ryan Keck",
	"TD Miller",
	"Diana Peake",
	"Barry Pennington",
	"Donna Vaporis",
	"%",
	"Muse",
	"Solange Hunt",
	"%",
	"",
	"#",
#ifdef DEXTERITY
	"Developed by Hamumu Software",
	"and",
	"Distributed by",
	"Dexterity Software",
	"http://www.dexterity.com",
	"",
	"Thanks to Todd Chodorow and",
	"Steve Pavlina at Dexterity!",
#else
	"Stop by www.hamumu.com",
	"And see what we're up to!",
	"","","",
#endif
	"","","","","","","","","","","","","","","","",
	"$"
	};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*4-330

#ifdef DEXTERITY
#define VERSION_NO	"Version 1.4D"
#else
#define VERSION_NO	"Version 1.5"
#endif
#define BUTTONX	60
#define BUTTONY 240
#define BUTTONDX 100
#define BUTTONDY 90
#define BUTTONWIDTH 70
#define BUTTONHEIGHT 60

sprite_set_t *planetSpr;
static int numRunsToMakeUp;

static byte oldc=0;

static byte keyAnim=0;
char lvlName[32];
static byte cursor;
byte *backScr;
int numRuns;
byte demoTextCounter;
static byte oldMsBtn;

static byte loadingGame;
static int oldMsx,oldMsy;
static byte combo,boomCombo;
static char bright;
static int guyY,guyDY,guyX,guyDX;
static byte showEndStar;
static byte angle;
static int folderX;

static float stockDone,parallelDone,pestDone,blowDone,clearDone,trainDone,smarchDone,hiddenDone,
			 bounceDone;

typedef struct menu_t
{
	char toptxt[32];
	char btmtxt[32];
	char desc[128];
	byte menuChoice;
} menu_t;

menu_t menu[MENU_CHOICES];

#ifdef BETA
int betaX[5],betaY[5];
int betaDX[5],betaDY[5];
#endif

void SetupMenuChoices(void)
{
	char name[][32]={
		"Employee","Training",
		"Stock","Room",
		"Parallel","Universe",
		"Super Summer","Blowout",
		"Inventory","Clearance",
		"Pest","Control",
		"Level","Editor",
		"Add-On","Levels",
		"Employee","Profiles",
		"Gift","Shop",
		"Music","Composer",
		"Quitting","Time",
	};
	char desc[][128]={
		"Easier levels to teach you basic Stockboy skills!",
		"A year of puzzle levels - Can you become Employee Of The Year?",
		"Parallel versions of the Stockroom levels!",
		"Blow up as many Barrels as you can before you run out of space!",
		"Try to rack up a high score by melting crates in big piles!",
		"Blast as many Critters as you can before they get you!",
		"Create your own puzzles!",
		"Play levels made by other fans!",
		"Change your appearance or play as someone else",
		"Buy stuff with your Star Points!",
		"Compose your own background music!",
		"Exit to Windows",
	};

	int i,j,k;
	byte p;

	cursor=0;
	oldMsBtn=1;
	guyY=0;
	guyDY=10;
	guyX=70;
	guyDX=0;
	combo=0;
	boomCombo=0;
	bright=0;
	showEndStar=0;
	angle=0;
	folderX=640+220;

	for(i=0;i<MENU_CHOICES;i++)
		menu[i].menuChoice=255;

	p=0;

	strcpy(menu[p].toptxt,name[0*2]);
	strcpy(menu[p].btmtxt,name[0*2+1]);
	strcpy(menu[p].desc,desc[0]);
	menu[p].menuChoice=MENU_TRAINING;
	p++;

	strcpy(menu[p].toptxt,name[1*2]);
	strcpy(menu[p].btmtxt,name[1*2+1]);
	strcpy(menu[p].desc,desc[1]);
	menu[p].menuChoice=MENU_STOCKROOM;
	p++;

	if(profile.giftShop[GIFT_PARALLEL])
	{
		strcpy(menu[p].toptxt,name[2*2]);
		strcpy(menu[p].btmtxt,name[2*2+1]);
		strcpy(menu[p].desc,desc[2]);
		menu[p].menuChoice=MENU_PARALLEL;
		p++;
	}
	if(profile.giftShop[GIFT_BLOWOUT])
	{
		strcpy(menu[p].toptxt,name[3*2]);
		strcpy(menu[p].btmtxt,name[3*2+1]);
		strcpy(menu[p].desc,desc[3]);
		menu[p].menuChoice=MENU_BLOWOUT;
		p++;
	}
	if(profile.giftShop[GIFT_CLEARANCE])
	{
		strcpy(menu[p].toptxt,name[4*2]);
		strcpy(menu[p].btmtxt,name[4*2+1]);
		strcpy(menu[p].desc,desc[4]);
		menu[p].menuChoice=MENU_CLEARANCE;
		p++;
	}
	if(profile.giftShop[GIFT_PEST])
	{
		strcpy(menu[p].toptxt,name[5*2]);
		strcpy(menu[p].btmtxt,name[5*2+1]);
		strcpy(menu[p].desc,desc[5]);
		menu[p].menuChoice=MENU_PESTCONTROL;
		p++;
	}
	if(profile.giftShop[GIFT_EDITOR])
	{
		strcpy(menu[p].toptxt,name[6*2]);
		strcpy(menu[p].btmtxt,name[6*2+1]);
		strcpy(menu[p].desc,desc[6]);
		menu[p].menuChoice=MENU_EDITOR;
		p++;
	}
	// HamSandwich change: show Add-on Levels if one of the level sets from the
	// 2011 stockaddon.exe is detected. Ordinarily it would require the editor
	// to be unlocked, but old fans may be replaying the game and want quick access.
	bool showAddons = profile.giftShop[GIFT_EDITOR];
	if (AssetOpen_SDL_Owned("levels/cavern.sbl"))
	{
		showAddons = true;
	}
	if (showAddons)
	{
		strcpy(menu[p].toptxt,name[7*2]);
		strcpy(menu[p].btmtxt,name[7*2+1]);
		strcpy(menu[p].desc,desc[7]);
		menu[p].menuChoice=MENU_ADDON;
		p++;
	}
	strcpy(menu[p].toptxt,name[8*2]);
	strcpy(menu[p].btmtxt,name[8*2+1]);
	strcpy(menu[p].desc,desc[8]);
	menu[p].menuChoice=MENU_PROFILE;
	p++;
	strcpy(menu[p].toptxt,name[9*2]);
	strcpy(menu[p].btmtxt,name[9*2+1]);
	strcpy(menu[p].desc,desc[9]);
	menu[p].menuChoice=MENU_GIFTSHOP;
	p++;
	if(profile.giftShop[GIFT_MUSIC])
	{
		strcpy(menu[p].toptxt,name[10*2]);
		strcpy(menu[p].btmtxt,name[10*2+1]);
		strcpy(menu[p].desc,desc[10]);
		menu[p].menuChoice=MENU_COMPOSER;
		p++;
	}
	strcpy(menu[p].toptxt,name[11*2]);
	strcpy(menu[p].btmtxt,name[11*2+1]);
	strcpy(menu[p].desc,desc[11]);
	menu[p].menuChoice=MENU_EXIT;
	p++;

	k=0;
	for(i=0;i<12;i++)
	{
		for(j=0;j<4;j++)
		{
			// k can be 0-4, 1=check, 2=bronze, 3=silver, 4=gold
			k+=profile.starGot[GAME_STOCKROOM][i*4+j];
		}
	}
	stockDone=(100.0f*(float)k)/(4.0f*4.0f*12.0f);

	k=0;
	for(i=0;i<12;i++)
	{
		for(j=0;j<4;j++)
		{
			// k can be 0-4, 1=check, 2=bronze, 3=silver, 4=gold
			k+=profile.starGot[GAME_PARALLEL][i*4+j];
		}
	}
	if(profile.giftShop[GIFT_PARALLEL])
		parallelDone=(100.0f*(float)k)/(4.0f*4.0f*12.0f);
	else
		parallelDone=-1.0f;	// don't show it if they don't have it

	k=0;
	for(i=0;i<9;i++)
	{
		// k can be 0-4, 1=check, 2=bronze, 3=silver, 4=gold
		k+=profile.starGot[GAME_PESTCONTROL][i];
	}
	if(profile.giftShop[GIFT_PEST])
		pestDone=(100.0f*(float)k)/(4.0f*9.0f);
	else
		pestDone=-1.0f;	// don't show it if they don't have it

	k=0;
	for(i=0;i<8;i++)
	{
		// k can be 0-4, 1=check, 2=bronze, 3=silver, 4=gold
		k+=profile.starGot[GAME_BLOWOUT][i];
	}
	if(profile.giftShop[GIFT_BLOWOUT])
		blowDone=(100.0f*(float)k)/(4.0f*8.0f);
	else
		blowDone=-1.0f;	// don't show it if they don't have it

	// k can be 0-4, 1=check, 2=bronze, 3=silver, 4=gold
	k=profile.starGot[GAME_CLEARANCE][0];
	if(profile.giftShop[GIFT_CLEARANCE])
		clearDone=(100.0f*(float)k)/(4.0f);
	else
		clearDone=-1.0f;	// don't show it if they don't have it

	k=0;
	for(i=0;i<10;i++)
	{
		// k can be 0-6, # of levels done (0-5), and then one more to mark that certificate is
		// earned (so ignore that one)
		if(profile.training[i]<6)
			k+=profile.training[i];
		else
			k+=5;
	}
	trainDone=(100.0f*(float)k)/(5.0f*10.0f);

	k=0;
	for(i=0;i<12;i++)
	{
		// k can be 0-1
		k+=profile.zodiac[i];
		if(profile.giftShop[GIFT_PARALLEL])
			k+=profile.zodiac2[i];
	}
	if(profile.giftShop[GIFT_SMARCH])
	{
		k+=profile.zodiac[12];
		if(profile.giftShop[GIFT_PARALLEL])
			k+=profile.zodiac2[12];
	}
	i=12;
	if(profile.giftShop[GIFT_SMARCH])
		i++;
	if(profile.giftShop[GIFT_PARALLEL])
		i*=2;

	hiddenDone=(100.0f*(float)k)/((float)i);
	if(hiddenDone<0.001f)
		hiddenDone=-1.0f;	// if you have done no hidden goals, let's not point them out!

	if(profile.giftShop[GIFT_SMARCH])
	{
		k=0;
		for(i=0;i<4;i++)
		{
			// k can be 0-4, 1=check, 2=bronze, 3=silver, 4=gold
			k+=profile.starGot[GAME_STOCKROOM][12*4+i];
			if(profile.giftShop[GIFT_PARALLEL])
				k+=profile.starGot[GAME_PARALLEL][12*4+i];
		}
		i=4*4;
		if(profile.giftShop[GIFT_PARALLEL])
			i+=4*4;

		smarchDone=(100.0f*(float)k)/((float)i);
	}
	else
		smarchDone=-1.0f;

	bounceDone=(100.0f*(float)profile.bestBounce)/9.0f;
	if(profile.bestBounce==0)
		bounceDone=-1.0f;

#ifdef BETA
	for(i=0;i<5;i++)
	{
		betaX[i]=Random(640);
		betaY[i]=Random(480);
		betaDX[i]=0;
		betaDY[i]=0;
	}
#endif
}

byte HandleTitleKeys(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();

	if(k=='e')
		return 1;	// go to the editor
	if(k==27)
		return 2;	// exit
	else
		return 0;	// play the game
}

void PrintPercentage(int y,const char *name,float pct)
{
	char s[48];
	static int yy;

	if(y==0)
		yy=0;

	if(pct<0)
		return;

	if(y!=0)
	{
		yy++;

		sprintf(s,"%s:",name);
		PrintUnGlow(folderX-172,227+yy*20,s,1);
		sprintf(s,"%0.1f%%",pct);
		RightPrintUnGlow(folderX-5,227+yy*20,s,1);
	}
	else
	{
		sprintf(s,"%s:",name);
		PrintUnGlow(folderX-172,227+yy*20,s,0);
		sprintf(s,"%0.1f%%",pct);
		RightPrintUnGlow(folderX-5,227+yy*20,s,0);
		DrawFillBox(folderX-172,227+yy*20+24,folderX-5,227+yy*20+25,0);
		yy++;
	}
}

void RenderProfSheet(MGLDraw *mgl)
{
	planetSpr->GetSprite(13)->Draw(folderX,345,mgl);
	PrintUnGlowSideways(folderX-200,322,322-78,profile.name,0);

	PrintPercentage(0,"Total Done",((float)profile.stars)*100.0f/999.0f);
	PrintPercentage(1,"Training",trainDone);
	PrintPercentage(1,"Stockroom",stockDone);
	PrintPercentage(1,"Parallel",parallelDone);
	PrintPercentage(1,"Pest Control",pestDone);
	PrintPercentage(1,"Summer Blowout",blowDone);
	PrintPercentage(1,"Clearance",clearDone);
	PrintPercentage(1,"Smarch",smarchDone);
	PrintPercentage(1,"Hidden Goals",hiddenDone);
	PrintPercentage(1,"Boom Bounce",bounceDone);

	// bouncing guy
	InstaRenderMonsterShadow(500+guyX,455,MONS_LOONY,1,mgl);
	InstaRenderMonster(500+guyX,455-guyY,MONS_LOONY,1,profile.color,profile.bright+bright,mgl);
}

void MainMenuDisplay(MGLDraw *mgl)
{
	int i;
	int x,y;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	RenderProfSheet(mgl);

	// menu options
	x=BUTTONX;
	y=BUTTONY;

	for(i=0;i<MENU_CHOICES;i++)
	{
		if(menu[i].menuChoice<255)
		{
			if(cursor!=i)
			{
				planetSpr->GetSprite(1+menu[i].menuChoice)->DrawGlow(x,y,mgl,-15);
			}
			else
			{
				planetSpr->GetSprite(1+menu[i].menuChoice)->Draw(x+(Cosine(angle)*6)/FIXAMT,y+(Sine(angle)*6)/FIXAMT,mgl);
				CenterPrintOutline(x,y-40,menu[i].toptxt,0,0);
				CenterPrintOutline(x,y+30,menu[i].btmtxt,0,0);
			}

			x+=BUTTONDX;
			if(((i+1)%4)==0)
			{
				x=BUTTONX;
				y+=BUTTONDY;
			}
		}
	}

	// version #:
	RightPrintOutline(638,3,VERSION_NO,1,0);
	// Copyright:
	RightPrint(639,178,"Copyright 2004, Hamumu Software",-32,1);
	RightPrint(638,177,"Copyright 2004, Hamumu Software",0,1);

	RightPrintOutline(635,455,menu[cursor].desc,0,0);

#ifdef BETA
	CenterPrintOutline(betaX[1],betaY[1],"BETA VERSION!",0,0);
	CenterPrintOutline(betaX[2],betaY[2],"* DO NOT DISTRIBUTE *",0,0);
	CenterPrintOutline(betaX[3],betaY[3],"BETA VERSION!",0,0);
	CenterPrintOutline(betaX[4],betaY[4],"* DO NOT DISTRIBUTE *",0,0);
	for(i=0;i<5;i++)
	{
		betaX[i]+=betaDX[i];
		betaDX[i]+=(-1+Random(3));
		if(betaX[i]<0)
		{
			betaX[i]=0;
			betaDX[i]=-betaDX[i];
		}
		if(betaX[i]>639)
		{
			betaX[i]=639;
			betaDX[i]=-betaDX[i];
		}
		betaY[i]+=betaDY[i];
		betaDY[i]+=(-1+Random(3));
		if(betaY[i]<0)
		{
			betaY[i]=0;
			betaDY[i]=-betaDY[i];
		}
		if(betaY[i]>479)
		{
			betaY[i]=479;
			betaDY[i]=-betaDY[i];
		}
		Clamp(&betaDX[i],5);
		Clamp(&betaDY[i],5);
	}
#endif

	planetSpr->GetSprite(0)->Draw(oldMsx,oldMsy,mgl);

	if(combo>2)
	{
		char s[32];

		sprintf(s,"Bounce Combo x%d!",combo);
		RightPrintOutline(635,200,s,0,0);
	}

	if(showEndStar)
		RenderEndStar(mgl);

	RenderStars(mgl);
	RenderParticles();
	RenderItAll(NULL,NULL,0);
}

byte MainMenuUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	int i,msx,msy;
	static byte reptCounter=0;
	byte btn;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;

	btn=mgl->MouseDown();

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(folderX>650)
			folderX-=8;
		else if(folderX>640)
			folderX-=4;

		angle+=8;
		Music_Update();

		UpdateParticles(NULL);

		UpdateStars();
		if(StarsLeft())
			showEndStar=30;
		else if(showEndStar)
			showEndStar--;

		Music_Update();
		// now real updating
		c=GetControls()|GetArrows();

		guyY+=guyDY;
		guyX+=guyDX;
		if(guyX<0)
			guyX=0;
		if(guyX>140)
			guyX=140;
		guyDY--;
		if(guyY<0)
		{
			bright=0;
			combo=0;
			boomCombo=0;
			guyY=0;
			guyDX=0;
			if(Random(15)==0)
			{
				guyDY=2+Random(10);
				guyDX=-4+Random(9);
			}
		}
		if(bright>0)
			bright--;

		if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
		{
			cursor-=4;
			if(cursor>MENU_CHOICES)
			{
				cursor+=12;
				while(menu[cursor].menuChoice==255)
					cursor-=4;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
		{
			cursor+=4;
			if(cursor>=MENU_CHOICES || menu[cursor].menuChoice==255)
			{
				cursor-=12;
				while(cursor>MENU_CHOICES)
					cursor+=4;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_LF) && !(oldc&CONTROL_LF))
		{
			cursor--;
			if(cursor>MENU_CHOICES)
			{
				cursor=3;
			}
			else if((cursor%4)==3)
			{
				cursor+=4;
				while(menu[cursor].menuChoice==255)
					cursor--;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_RT) && !(oldc&CONTROL_RT))
		{
			cursor++;
			if((cursor%4)==0)
				cursor-=4;
			if(menu[cursor].menuChoice==255)
			{
				cursor=(cursor/4)*4;
			}
			MakeNormalSound(SND_MENUCLICK);
		}

		if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
		{
			MakeNormalSound(SND_MENUSELECT);
			return menu[cursor].menuChoice+1;
		}

		mgl->GetMouse(&msx,&msy);
		for(i=0;i<MENU_CHOICES;i++)
		{
			if(menu[i].menuChoice<255 &&
				msx>BUTTONX+(i%4)*BUTTONDX-BUTTONWIDTH/2 &&
				msx<BUTTONX+(i%4)*BUTTONDX+BUTTONWIDTH/2 &&
			    msy>BUTTONY+(i/4)*BUTTONDY-BUTTONHEIGHT/2 &&
				msy<BUTTONY+(i/4)*BUTTONDY+BUTTONHEIGHT/2)
			{
				if(cursor!=i && (oldMsx!=msx || oldMsy!=msy))
				{
					cursor=i;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(btn && !oldMsBtn)
				{
					MakeNormalSound(SND_MENUSELECT);
					return menu[cursor].menuChoice+1;
				}
			}
		}
		if(oldMsx!=msx || oldMsy!=msy)
			numRuns=0;

		oldMsx=msx;
		oldMsy=msy;

		if(msx>guyX+500-18 && msx<guyX+500+18 && msy>460-guyY-25 && msy<460-guyY+10)
		{
			if(btn && !oldMsBtn)
			{
				guyDY=12;
				guyDX=-4+Random(6);
				combo++;
				bright+=16;
				if(combo>99)
					combo=99;
				if(bright>31)
					bright=31;
				PlayerSoundEffect(SE_PUSH);
				if(guyY>300)
				{
					boomCombo++;
					if(boomCombo>profile.bestBounce && boomCombo<=9)
					{
						profile.bestBounce=boomCombo;
						profile.stars++;
						profile.starsLeft++;
						bounceDone=(100.0f*(float)profile.bestBounce)/9.0f;
						LaunchStar((guyX+500)*FIXAMT,(460-guyY)*FIXAMT);
					}
					Explosion((guyX+500)*FIXAMT,(460-guyY)*FIXAMT,0,(byte)Random(8));
					MakeNormalSound(SND_BOMBBOOM+Random(3));

				}
			}
		}

		if(c)	// hitting any key prevents credits
			numRuns=0;

		oldc=c;
		oldMsBtn=btn;

		c=mgl->LastKeyPressed();
		if(c==27)
			return MENU_EXIT+1;
		if(c || btn)
			numRuns=0;

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 0;
}

TASK(byte) MainMenu(byte pageUp,MGLDraw *mgl)
{
	byte b=0;
	int i;
	int lastTime=1;

	Music_Load(profile.songChoice[SONG_TITLE]);
	Music_Play();

	SetupMenuChoices();

	InitStars(4,profile.starsLeft);
	LoadOptions();
	PurgeMonsterSprites();
	InitParticles(50);
	planetSpr=new sprite_set_t("graphics/title.jsp");
	mgl->LoadBMP("graphics/title.bmp");
	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],mgl->GetScreen()+mgl->GetWidth()*i,640);

	cursor=0;
	loadingGame=0;
	GetTaps();
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();

		b=MainMenuUpdate(&lastTime,mgl);
		MainMenuDisplay(mgl);
		Music_Update();
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			PurgeMonsterSprites();
			free(backScr);
			delete planetSpr;
			ExitParticles();
			CO_RETURN 255;
		}
		EndClock();
		if(numRuns>=30*20)
		{
			GetTaps();
			oldc=255;
			AWAIT Credits(mgl,1);
			mgl->LoadBMP("graphics/title.bmp");
			mgl->LastKeyPressed();
			numRuns=0;
		}
	}
	SaveProfile(opt.curProfile);
	SaveOptions();
	PurgeMonsterSprites();
	free(backScr);
	delete planetSpr;
	ExitParticles();
	CO_RETURN b;
}

//-------------------------------------------------------------------------

void CreditsRender(int y)
{
	int i,ypos,xpos;
	char *s;

	i=0;

	ypos=0;
	xpos=320;
	while(credits[i][0]!='$')
	{
		s=credits[i];
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrintUnGlow(xpos,ypos-y,&s[1],0);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(xpos-200,ypos-y+8,xpos+200,ypos-y+11,0);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(xpos-70,ypos-y+8,xpos+70,ypos-y+9,0);
			}
			else
				CenterPrintUnGlow(xpos,ypos-y,s,0);
		}
		ypos+=25;
		i++;
		if(ypos-y>=480)
			return;
	}
}

TASK(void) Credits(MGLDraw *mgl,byte init)
{
	int y=-470;
	int lastTime;
	int wid;
	byte* pos;
	int i;
	byte c;

	dword hangon;

	EndClock();
	hangon=TimeLength();

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	lastTime=1;

	while(1)
	{
		lastTime+=TimeLength();
		StartClock();

		wid=mgl->GetWidth();
		pos=mgl->GetScreen()+0*wid;
		for(i=0;i<480;i++)
		{
			if(i>31)
			{
				if(i>=479-31)
					c=64+31-(i-(479-31));
				else
					c=64+31;
			}
			else
				c=i+64;
			memset(pos,c,640);
			pos+=wid;
		}

		Music_Update();
		CreditsRender(y);

		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME)
		{
			JamulSoundUpdate();
			Music_Update();
			y+=1;
			lastTime-=TIME_PER_FRAME;
		}

		AWAIT mgl->Flip();
		EndClock();

		if(!mgl->Process())
		{
			ResetClock(hangon);
			CO_RETURN;
		}
		if(GetControls()||GetArrows()||mgl->MouseDown()||mgl->RMouseDown()||mgl->LastKeyPressed())
		{
			ResetClock(hangon);
			CO_RETURN;
		}

		if(y==END_OF_CREDITS)
		{
			ResetClock(hangon);
			CO_RETURN;
		}
	}
	ResetClock(hangon);
}

TASK(byte) SpeedSplash(MGLDraw *mgl,const char *fname)
{
	int i,j,clock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;
	byte c,oldc;


	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();
	oldc=GetControls()|GetArrows();

	mgl->LoadBMP(fname, desiredpal);

	mode=0;
	clock=0;
	done=0;
	while(!done)
	{
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN 0;
		c=mgl->LastKeyPressed();

		if(c==27)
			CO_RETURN 0;
		else if(c)
			mode=2;

		c=GetControls()|GetArrows();
		if((c&(CONTROL_B1|CONTROL_B2)) && (!(oldc&(CONTROL_B1|CONTROL_B2))))
			mode=2;
		if(mgl->MouseDown() || mgl->RMouseDown())
			mode=2;

		oldc=c;

		clock++;
		switch(mode)
		{
			case 0:	// fading in
				for(j=0;j<16;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].r<desiredpal[i].r)
							curpal[i].r++;
						if(curpal[i].g<desiredpal[i].g)
							curpal[i].g++;
						if(curpal[i].b<desiredpal[i].b)
							curpal[i].b++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock>16)
				{
					mode=1;
					clock=0;
				}
				break;
			case 1:
				// sit around
				break;
			case 2:	// fading out
				clock=0;
				for(j=0;j<16;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].r>0)
							curpal[i].r--;
						else
							clock++;
						if(curpal[i].g>0)
							curpal[i].g--;
						else
							clock++;
						if(curpal[i].b>0)
							curpal[i].b--;
						else
							clock++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock==256*3*16)
					done=1;
				break;
		}
	}
	mgl->ClearScreen();
	AWAIT mgl->Flip();
	CO_RETURN 1;
}

TASK(void) HelpScreens(MGLDraw *mgl)
{
	int i;
	char name[32];

	for(i=0;i<5;i++)
	{
		sprintf(name,"docs/help%d.bmp",i+1);
		if(!AWAIT SpeedSplash(mgl,name))
			CO_RETURN;
	}
}

TASK(void) DemoSplashScreens(MGLDraw *mgl)
{
	if(!AWAIT SpeedSplash(mgl,"docs/demosplash.bmp"))
		CO_RETURN;
	if(!AWAIT SpeedSplash(mgl,"docs/demosplash2.bmp"))
		CO_RETURN;
}

TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound)
{
	int i,j,clock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;
	dword now,then;

	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();

	mgl->LoadBMP(fname, desiredpal);

	mode=0;
	clock=0;
	done=0;
	then=timeGetTime();
	while(!done)
	{
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if(mgl->LastKeyPressed())
			mode=2;
		if(mgl->MouseDown() || mgl->RMouseDown())
			mode=2;

		JamulSoundUpdate();

		now=timeGetTime();
		while(now>then && now-then>=TIME_PER_FRAME)
		{
			then+=TIME_PER_FRAME;
			clock++;
			switch(mode)
			{
				case 0:	// fading in
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r<desiredpal[i].r)
								curpal[i].r++;
							if(curpal[i].g<desiredpal[i].g)
								curpal[i].g++;
							if(curpal[i].b<desiredpal[i].b)
								curpal[i].b++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==32)
					{
						if(sound==2)
							MakeNormalSound(SND_HAMUMU);
					}
					if(clock>64)
					{
						mode=1;
						clock=0;
					}
					break;
				case 1:
					if(clock>delay)
					{
						mode=2;
						clock=0;
					}
					break;
				case 2:	// fading out
					clock=0;
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r>0)
								curpal[i].r--;
							else
								clock++;
							if(curpal[i].g>0)
								curpal[i].g--;
							else
								clock++;
							if(curpal[i].b>0)
								curpal[i].b--;
							else
								clock++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==256*3*8)
						done=1;
					break;
			}
		}
	}
	mgl->ClearScreen();
	AWAIT mgl->Flip();
}
