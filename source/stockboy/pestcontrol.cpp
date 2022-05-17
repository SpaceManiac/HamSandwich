#include "pestcontrol.h"
#include "map.h"
#include "guy.h"
#include "game.h"
#include "options.h"
#include "score.h"
#include "floor.h"
#include "world.h"
#include "stars.h"
#include "highscore.h"
#include "music.h"

// genTypes
#define GEN_RANDOM	0	// generate monsters random, 1 per time
#define GEN_ALL		1	// all generators go off at once
#define GEN_SEQUENCE 2	// takes turns going through the generators in order

// weapon choices
#define WPN_BOMB	0
#define WPN_LASER	1
#define WPN_BOTH	2

// monsters
#define MS_BLOBBY	0
#define MS_INCHY	1
#define MS_PIGGY	2
#define MS_BLOBNPIG	3
#define MS_FLUFFY	4

#define NUM_COSTS	6

typedef struct pestLevel_t
{
	char name[16];
	dword startGen;	// the amount of time between generations at level start (in 100ths of 30ths)
	byte genInc;	// how much the rate of gen increases per second (in 100ths of 30ths)
	byte wpn;		// what weapon type the level gives
	byte bombInc;	// how many badguys generate between weapons appearing
	byte mons;		// what monster type is in the level (just for display)
	byte genType;	// how they generate monsters
} pestLevel_t;

static Map *map;

static byte pestLevel=1;

static dword pestClock;
static byte miniClock;

static dword genTime;
static byte monsCount;
static byte cost;
static int lastGen;

static pestLevel_t lvlDef[]={
	{"Traffic",30*2*100,10,WPN_BOMB,2,MS_INCHY,GEN_RANDOM},
	{"Blob Maze",30*4*100,10,WPN_BOMB,4,MS_BLOBBY,GEN_RANDOM},
	{"Pigpen",30*5*100,20,WPN_BOTH,2,MS_PIGGY,GEN_RANDOM},
	{"Spiral",30*3*100,15,WPN_BOMB,3,MS_FLUFFY,GEN_RANDOM},
	{"Surrounded",30*2*100,10,WPN_BOTH,2,MS_INCHY,GEN_SEQUENCE},
	{"Laser Tag",30*4*100,10,WPN_LASER,4,MS_BLOBNPIG,GEN_RANDOM},
	{"King Blobby",30*1*100,5,WPN_BOMB,3,MS_BLOBBY,GEN_RANDOM},
	{"Pork Zone",30*1*100,5,WPN_BOMB,1,MS_PIGGY,GEN_RANDOM},
	{"Hotel Of Doom",30*3*100,10,WPN_BOMB,3,MS_FLUFFY,GEN_SEQUENCE},
};

void InitPestControl(Map *m)
{
	genTime=lvlDef[pestLevel-1].startGen;
	miniClock=0;
	monsCount=254;
	lastGen=-1;

	// difficulty starts the generation faster, by 2 minutes of play per difficulty
	if((dword)((120*opt.pestDiff)*lvlDef[pestLevel-1].genInc)<genTime)
		genTime-=(120*opt.pestDiff)*lvlDef[pestLevel-1].genInc;
	else
		genTime=0;

	pestClock=genTime;
	if(pestClock)
		pestClock--;

	map=m;
}

static byte IsOpenSpot(int x,int y)
{
	int i;

	i=x+y*map->height;
	if(map->map[i].floor==0 && map->map[i].item==0)
		return 1;
	return 0;
}

static void AddBomb(void)
{
	int i;
	int j;

	i=0;
	while((i++)<10000)
	{
		j=Random(map->width*map->height);
		if(map->map[j].item==0 && !FloorIsSolid(map->map[j].floor) && !FloorIsPit(map->map[j].floor) &&
			!FloorIsSummoner(map->map[j].floor))
		{
			switch(lvlDef[pestLevel-1].wpn)
			{
				case WPN_BOMB:
					map->map[j].item=ITM_BOMB;
					break;
				case WPN_LASER:
					map->map[j].item=ITM_LASER;
					break;
				case WPN_BOTH:
					if(Random(3)==0)
						map->map[j].item=ITM_LASER;
					else
						map->map[j].item=ITM_BOMB;
					break;
			}
			map->map[j].itemInfo=0;
			map->map[j].templight=16;
			return;
		}
	}

	// if you got here, the bomb couldn't be placed!
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].item==ITM_ACTIVEBOMB)
			return;	// as long as there are bombs ticking, you're okay
		else
			SendMessageToGame(MSG_RESET,0);
	}
}

void SummonMonster(void)
{
	int i;
	int j;
	int x,y;

	if(lvlDef[pestLevel-1].genType==GEN_RANDOM)
	{
		i=0;
		while((i++)<10000)
		{
			x=Random(map->width);
			y=Random(map->height);
			j=x+y*map->width;
			if(FloorIsSummoner(map->map[j].floor) && !map->map[j].item)
			{
				SummonGuy(&map->map[j],x,y);
				monsCount++;
				if(monsCount>=lvlDef[pestLevel-1].bombInc)
				{
					AddBomb();
					monsCount=0;
				}
				return;
			}
		}
	}
	else if(lvlDef[pestLevel-1].genType==GEN_SEQUENCE)
	{
		lastGen++;
		i=0;
		while((i++)<1000)
		{
			if(FloorIsSummoner(map->map[lastGen].floor) && !map->map[lastGen].item)
			{
				x=lastGen%map->width;
				y=lastGen/map->width;
				SummonGuy(&map->map[lastGen],x,y);
				monsCount++;
				if(monsCount>=lvlDef[pestLevel-1].bombInc)
				{
					AddBomb();
					monsCount=0;
				}
				return;
			}
			lastGen++;
			if(lastGen>=map->width*map->height)
				lastGen=0;
		}
	}
}

void UpdatePestControl(void)
{
	pestClock++;
	miniClock++;

	if(pestClock>=genTime/100)
	{
		SummonMonster();
		pestClock=0;
	}
	if(miniClock==30)
	{
		miniClock=0;
		if(genTime>lvlDef[pestLevel-1].genInc)
			genTime-=lvlDef[pestLevel-1].genInc;
		else
			genTime=0;
	}
}

// -----------------------------------------------------------

// the pestcontrol menu!
static byte *backScr;
static world_t world;
static byte cursor;
static byte txtLevel[6];
static sprite_set_t *blowSpr;
static byte blowSpin;
static byte play;
static int oldmsx,oldmsy,oldMsBtn;
static byte yesno;

void InitPestMenu(void)
{
	int i;

	JamulSoundPurge();
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();
	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	GetArrowTaps();
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	GetDisplayMGL()->LoadBMP("graphics\\pestcontrol.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	cursor=0;

	for(i=0;i<6;i++)
		txtLevel[i]=0;

	LoadWorld(&world,"levels\\pestcontrol.sbl",GetDisplayMGL());

	blowSpr=new sprite_set_t("graphics\\blowout.jsp");
	blowSpin=0;

	if(pestLevel==0)
		pestLevel=1;

	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);
	oldMsBtn=255;

	cost=(byte)Random(NUM_COSTS);
	LoadOptions();
	InitStars(profile.starGot[GAME_PESTCONTROL][pestLevel-1],0);
	yesno=0;
}

void ExitPestMenu(void)
{
	delete blowSpr;
	free(backScr);
	FreeWorld(&world);
	SaveOptions();
}

byte UpdatePestMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte t;
	static byte fwClock;
	byte btn;
	int msx,msy;
	int i;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;

	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		blowSpin++;
		if(blowSpin>15)
			blowSpin=0;

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

		if(!yesno)
		{
			if(t&CONTROL_DN)
			{
				cursor++;
				if(cursor>5)
					cursor=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(t&CONTROL_UP)
			{
				cursor--;
				if(cursor>5)
					cursor=5;
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
						pestLevel++;
						if(pestLevel>world.numMaps)
							pestLevel=1;
						cost=(byte)Random(NUM_COSTS);
						InitStars(profile.starGot[GAME_PESTCONTROL][pestLevel-1],0);
						MakeNormalSound(SND_MENUSELECT);
						break;
					case 2:	// difficulty;
						opt.pestDiff++;
						if(opt.pestDiff>3)
							opt.pestDiff=0;
						MakeNormalSound(SND_MENUSELECT);
						break;
					case 3:	// clear scores
						yesno=1;
						cursor=0;
						MakeNormalSound(SND_MENUSELECT);
						break;
					case 4: // rules
						break;
					case 5:	// exit
						play=0;
						MakeNormalSound(SND_MENUSELECT);
						return 0;
				}
			}

			if(msx!=oldmsx || msy!=oldmsy)
			{
				for(i=0;i<6;i++)
				{
					if(msx>10 && msx<280 && msy>270-16+i*35 && msy<270+19+i*35)
					{
						if(cursor!=i)
							MakeNormalSound(SND_MENUCLICK);
						cursor=i;
					}
				}
			}
			if(btn && !oldMsBtn)
			{
				for(i=0;i<6;i++)
				{
					if(msx>10 && msx<280 && msy>270-16+i*35 && msy<270+19+i*35)
					{
						switch(cursor)
						{
							case 0:	// begin
								play=1;
								MakeNormalSound(SND_MENUSELECT);
								return 0;
							case 1:	// chg map
								pestLevel++;
								if(pestLevel>world.numMaps)
									pestLevel=1;
								cost=(byte)Random(NUM_COSTS);
								InitStars(profile.starGot[GAME_PESTCONTROL][pestLevel-1],0);
								MakeNormalSound(SND_MENUSELECT);
								break;
							case 2:	// difficulty;
								opt.pestDiff++;
								if(opt.pestDiff>3)
									opt.pestDiff=0;
								MakeNormalSound(SND_MENUSELECT);
								break;
							case 3:	// clear scores
								yesno=1;
								cursor=0;
								MakeNormalSound(SND_MENUSELECT);
								break;
							case 4: // rules
								break;
							case 5:	// exit
								play=0;
								MakeNormalSound(SND_MENUSELECT);
								return 0;

						}
					}
				}
			}
		}
		else	// asking yes/no for clear high scores
		{
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
						yesno=0;
						cursor=3;
						MakeNormalSound(SND_MENUCANCEL);
						break;
					case 1:	// clear them
						if(pestLevel>0)
							ResetHighScore(GAME_PESTCONTROL,pestLevel-1);
						yesno=0;
						cursor=3;
						MakeNormalSound(SND_MENUSELECT);
						break;
				}
			}
			if(c==27)
			{
				yesno=0;
				cursor=3;
				MakeNormalSound(SND_MENUCANCEL);
			}

			if(msx!=oldmsx || msy!=oldmsy)
			{
				for(i=0;i<2;i++)
				{
					if(msx>10 && msx<280 && msy>240+i*40 && msy<240+25+i*40)
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
					if(msx>10 && msx<280 && msy>240+i*40 && msy<240+25+i*40)
					{
						cursor=i;
						switch(cursor)
						{
							case 0:	// don't clear scores
								yesno=0;
								cursor=3;
								MakeNormalSound(SND_MENUCANCEL);
								break;
							case 1:	// clear them
								if(pestLevel>0)
									ResetHighScore(GAME_PESTCONTROL,pestLevel-1);
								yesno=0;
								cursor=3;
								MakeNormalSound(SND_MENUSELECT);
								break;
						}
					}
				}
			}
		}

		oldMsBtn=btn;
		oldmsx=msx;
		oldmsy=msy;

		if(c==27)
		{
			play=0;
			return 0;
		}
		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

void RenderPestMenu(MGLDraw *mgl)
{
	char txt[6][32]={
		"Start Exterminating",
		"Change Location",
		"Difficulty:",
		"Show/Reset High Scores",
		"Rules",
		"Exit",
	};
	char mons[5][20]={
		"Blobbies",
		"Inchies",
		"Piggies",
		"Blobbies & Piggies",
		"Fluffies",
	};
	char wpnName[4][16]={
		"Bombs",
		"Lasers",
		"Bombs & Lasers",
		"",
	};

	char costTxt[][32]={
		"What ya got?",
		"$$$$$$$$$",
		"$1.50",
		"$7.8 Trillion",
		"Very Pricey",
		"Your firstborn",
	};
	char yesnotxt[2][32]={
		"Nevermind!",
		"Really Reset High Scores",
	};

	int i,x;

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	if(pestLevel>0)
	{
		mgl->Box(304,208,625,448,0);
		if(cursor==4)
		{
			mgl->FillBox(305,209,624,447,0);
			CenterPrint(465,276+00,"Critters appear continuously, and",0,0);
			CenterPrint(465,276+25,"gadgets appear at random. Survive",0,0);
			CenterPrint(465,276+50,"as long as you can and rack up",0,0);
			CenterPrint(465,276+75,"points by blowing up the Critters!",0,0);
		}
		else if(yesno || cursor==3)
		{
			world.map[pestLevel-1]->RenderTiny(&world,305,209);
			RenderStartStar(465*FIXAMT,329*FIXAMT,mgl);
			RenderHighScore(GAME_PESTCONTROL,pestLevel-1,2,405,244,mgl);
			RenderHighScore(GAME_PESTCONTROL,pestLevel-1,1,405,244+60,mgl);
			RenderHighScore(GAME_PESTCONTROL,pestLevel-1,0,405,244+60*2,mgl);
			CenterPrintOutline(465,420,lvlDef[pestLevel-1].name,0,0);
		}
		else
		{
			world.map[pestLevel-1]->RenderTiny(&world,305,209);
			CenterPrintOutline(465,420,lvlDef[pestLevel-1].name,0,0);
			RenderStartStar(465*FIXAMT,329*FIXAMT,mgl);
		}
	}

	PrintUnGlow(112,121,mons[lvlDef[pestLevel-1].mons],0);
	PrintUnGlow(96,150,lvlDef[pestLevel-1].name,0);
	PrintUnGlow(116,178,wpnName[lvlDef[pestLevel-1].wpn],0);
	PrintUnGlow(135,205,costTxt[cost],0);

	if(!yesno)
	{
		for(i=0;i<6;i++)
		{
			PrintUnGlow(10,270+i*35,txt[i],0);
			for(x=0;x<8;x++)
			{
				if(txtLevel[i]>x)
				{
					if(x==7)
					{
						Print(10+x-1,270-x+i*35-1,txt[i],-32,0);
						Print(10+x+1,270-x+i*35+1,txt[i],-32,0);
						Print(10+x-1,270-x+i*35+1,txt[i],-32,0);
						Print(10+x+1,270-x+i*35-1,txt[i],-32,0);
					}

					Print(10+x,270-x+i*35,txt[i],-32+x*6,0);
				}
			}
		}

		for(i=0;i<4;i++)
			if(i<=opt.pestDiff)
				blowSpr->GetSprite(blowSpin/2)->Draw(120+i*35,348,mgl);
			else
				blowSpr->GetSprite(8)->Draw(120+i*35,348,mgl);
	}
	else
	{
		for(i=0;i<2;i++)
		{
			PrintUnGlow(10,240+i*40,yesnotxt[i],0);
			for(x=0;x<8;x++)
			{
				if(txtLevel[i]>x)
				{
					if(x==7)
					{
						Print(10+x-1,240-x+i*40-1,yesnotxt[i],-32,0);
						Print(10+x+1,240-x+i*40+1,yesnotxt[i],-32,0);
						Print(10+x-1,240-x+i*40+1,yesnotxt[i],-32,0);
						Print(10+x+1,240-x+i*40-1,yesnotxt[i],-32,0);
					}

					Print(10+x,240-x+i*40,yesnotxt[i],-32+x*6,0);
				}
			}
		}
	}

	blowSpr->GetSprite(9)->Draw(oldmsx,oldmsy,mgl);
}

byte PestControlMenu(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;

	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitPestMenu();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdatePestMenu(&lastTime,mgl);
		RenderPestMenu(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			ExitPestMenu();
			return WORLD_ABORT;
		}
		EndClock();
	}
	then=timeGetTime();
	ResetClock(hangon);
	ExitPestMenu();

	if(play)
		return LunaticWorld(pestLevel-1,"levels\\pestcontrol.sbl");
	else
		return WORLD_ABORT;
}
