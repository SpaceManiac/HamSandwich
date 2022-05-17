#include "clear2.h"
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

static Map *map;

static byte diff;
static byte level;

static byte invColorTable[]={2,1,4,5,3,6,7};

static void PlopBarrels(void);
static byte clearWait;

void InitClear2(Map *m)
{
	level=0;
	clearWait=0;
	map=m;

	diff=3+(opt.clearanceDiff);
	if(diff>6)
		diff=6;

	PlopBarrels();
}

static byte IsOpenSpot(int x,int y)
{
	int i;

	i=x+y*map->height;
	if(map->map[i].floor==0 && map->map[i].item==0)
		return 1;
	return 0;
}

void PlopBarrels(void)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		map->map[i].floor=0;
		if(map->map[i].item!=ITM_PLAYER)
			map->map[i].item=ITM_CRATE;
		map->map[i].itemInfo=invColorTable[Random(diff)];
	}

	for(i=0;i<9;i++)
		player.item[i]=PE_ACID*8;
}

void UpdateClear2(void)
{
	int i,j;
	byte b;
	byte thisEmpty,thatNot;

	b=0;
	for(i=0;i<map->width*map->height;i++)
		if(map->map[i].item==ITM_CRATE)
			b=1;

	if(b==0)	// you won!!
	{
		SendMessageToGame(MSG_RESET,0);
		NewBigMessage("Inventory Cleared!",90);
		return;
	}
	if(clearWait)
	{
		clearWait--;
		return;
	}
	else
	{
		b=0;
		clearWait=3;
		for(i=map->width*(map->height-1)-1;i>=0;i--)
		{
			if(map->map[i].item==ITM_CRATE && map->map[i+map->width].item==ITM_NONE)
			{
				map->map[i].item=ITM_NONE;
				map->map[i+map->width].item=ITM_CRATE;
				map->map[i+map->width].itemInfo=map->map[i].itemInfo;
				b=1;
			}
		}
		if(!b)	// if none dropped, check for empty columns
		{
			for(i=0;i<map->width-1;i++)
			{
				thisEmpty=1;
				for(j=0;j<map->height;j++)
					if(map->map[i+j*map->width].item==ITM_CRATE)
						thisEmpty=0;
					else if(map->map[i+j*map->width].item==ITM_PLAYER)
					{
						if(map->map[i+1+j*map->width].item)
							thisEmpty=0;	// only count the player if he's blocking the shift
					}
				if(thisEmpty)
				{
					thatNot=0;
					for(j=0;j<map->height;j++)
						if(map->map[i+1+j*map->width].item==ITM_CRATE)
							thatNot=1;
				}
				if(thisEmpty && thatNot)
				{
					for(j=0;j<map->height;j++)
					{
						if(map->map[i+1+j*map->width].item==ITM_CRATE)
						{
							map->map[i+j*map->width].item=map->map[i+1+j*map->width].item;
							map->map[i+j*map->width].itemInfo=map->map[i+1+j*map->width].itemInfo;
							map->map[i+1+j*map->width].item=ITM_NONE;
						}
					}
					b=1;
				}
			}
		}
		if(b)
			MakeNormalSound(SND_SURFPUSH);
	}
}

void Clear2Wait(void)
{
	clearWait=30;
}

// -----------------------------------------------------------

// the clearance menu!
static byte *backScr;
static world_t world;
static byte cursor;
static byte txtLevel[6];
static sprite_set_t *blowSpr;
static byte blowSpin;
static byte play;
static int oldmsx,oldmsy,oldMsBtn;
static byte yesno;

void InitClear2Menu(void)
{
	int i;

	yesno=0;
	JamulSoundPurge();
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();

	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	GetArrowTaps();
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	GetDisplayMGL()->LoadBMP("graphics/clearance.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	cursor=0;

	for(i=0;i<6;i++)
		txtLevel[i]=0;

	LoadWorld(&world,"levels/clearance.sbl",GetDisplayMGL());

	blowSpr=new sprite_set_t("graphics/blowout.jsp");
	blowSpin=0;

	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);
	oldMsBtn=255;
	LoadOptions();
	InitStars(profile.starGot[GAME_CLEARANCE][0],0);
}

void ExitClear2Menu(void)
{
	delete blowSpr;
	free(backScr);
	FreeWorld(&world);
	SaveOptions();
}

byte UpdateClear2Menu(int *lastTime,MGLDraw *mgl)
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
				if(cursor>4)
					cursor=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(t&CONTROL_UP)
			{
				cursor--;
				if(cursor>4)
					cursor=4;
				MakeNormalSound(SND_MENUCLICK);
			}

			if(t&(CONTROL_B1|CONTROL_B2))
			{
				switch(cursor)
				{
					case 0:	// begin
						MakeNormalSound(SND_MENUSELECT);
						play=1;
						return 0;
					case 1:	// difficulty;
						opt.clearanceDiff++;
						if(opt.clearanceDiff>3)
							opt.clearanceDiff=0;
						MakeNormalSound(SND_MENUSELECT);
						break;
					case 2:	// clear scores
						yesno=1;
						cursor=0;
						MakeNormalSound(SND_MENUSELECT);
						break;
					case 3:	// rules
						break;
					case 4: // exit
						MakeNormalSound(SND_MENUSELECT);
						play=0;
						return 0;
				}
			}

			if(msx!=oldmsx || msy!=oldmsy)
			{
				for(i=0;i<5;i++)
				{
					if(msx>10 && msx<280 && msy>240-16+i*40 && msy<240+22+i*40)
					{
						if(cursor!=i)
							MakeNormalSound(SND_MENUCLICK);
						cursor=i;
					}
				}
			}
			if(btn && !oldMsBtn)
			{
				for(i=0;i<5;i++)
				{
					if(msx>10 && msx<280 && msy>240-16+i*40 && msy<240+22+i*40)
					{
						cursor=i;
						switch(cursor)
						{
							case 0:	// begin
								MakeNormalSound(SND_MENUSELECT);
								play=1;
								return 0;
							case 1:	// difficulty;
								opt.clearanceDiff++;
								if(opt.clearanceDiff>3)
									opt.clearanceDiff=0;
								MakeNormalSound(SND_MENUSELECT);
								break;
							case 2:	// clear scores
								yesno=1;
								cursor=0;
								MakeNormalSound(SND_MENUSELECT);
								break;
							case 3:	// rules
								break;
							case 4: // exit
								play=0;
								MakeNormalSound(SND_MENUSELECT);
								return 0;
						}
					}
				}
			}
		}
		else
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
						break;
					case 1:	// clear them
						ResetHighScore(GAME_CLEARANCE,0);
						yesno=0;
						cursor=2;
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
								ResetHighScore(GAME_CLEARANCE,0);
								yesno=0;
								cursor=2;
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

void RenderClear2Menu(MGLDraw *mgl)
{
	char txt[6][32]={
		"Let The Savings Begin!",
		"Difficulty:",
		"Show/Reset High Scores",
		"Rules",
		"Exit",
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

	if(cursor==3)
	{
		mgl->FillBox(314,234,636,476,0);
		CenterPrint(475,256+00,"You have an unlimited supply of Acid.",0,0);
		CenterPrint(475,256+25,"This Acid melts all Crates of the same",0,0);
		CenterPrint(475,256+50,"color that are touching. The Crates",0,0);
		CenterPrint(475,256+75,"move down and left when there's",0,0);
		CenterPrint(475,256+100,"empty space. The more Crates you zap",0,0);
		CenterPrint(475,256+125,"at once, the more points you get. Clear",0,0);
		CenterPrint(475,256+150,"the entire board, getting as many",0,0);
		CenterPrint(475,256+175,"points as possible!",0,0);
	}
	else if(yesno || cursor==2)
	{
		mgl->FillBox(314,234,636,476,0);
		RenderStartStar(474*FIXAMT,354*FIXAMT,mgl);
		RenderHighScore(GAME_CLEARANCE,0,2,415,270,mgl);
		RenderHighScore(GAME_CLEARANCE,0,1,415,270+60,mgl);
		RenderHighScore(GAME_CLEARANCE,0,0,415,270+60*2,mgl);
	}
	else
	{
		RenderStartStar(474*FIXAMT,354*FIXAMT,mgl);
	}

	if(!yesno)
	{
		for(i=0;i<5;i++)
		{
			PrintUnGlow(10,240+i*40,txt[i],0);
			for(x=0;x<8;x++)
			{
				if(txtLevel[i]>x)
				{
					if(x==7)
					{
						Print(10+x-1,240-x+i*40-1,txt[i],-32,0);
						Print(10+x+1,240-x+i*40+1,txt[i],-32,0);
						Print(10+x-1,240-x+i*40+1,txt[i],-32,0);
						Print(10+x+1,240-x+i*40-1,txt[i],-32,0);
					}

					Print(10+x,240-x+i*40,txt[i],-32+x*6,0);
				}
			}
		}

		for(i=0;i<4;i++)
			if(i<=opt.clearanceDiff)
				blowSpr->GetSprite(blowSpin/2)->Draw(120+i*35,288,mgl);
			else
				blowSpr->GetSprite(8)->Draw(120+i*35,288,mgl);
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

byte Clear2Menu(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;

	EndClock();
	hangon=TimeLength();

	level=0;
	now=timeGetTime();
	InitClear2Menu();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateClear2Menu(&lastTime,mgl);
		RenderClear2Menu(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			ExitClear2Menu();
			return WORLD_ABORT;
		}
		EndClock();
	}
	then=timeGetTime();
	ResetClock(hangon);
	ExitClear2Menu();

	if(play)
		return LunaticWorld(0,"levels/clearance.sbl");
	else
		return WORLD_ABORT;
}
