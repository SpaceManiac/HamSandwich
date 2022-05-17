#include "blowout.h"
#include "map.h"
#include "guy.h"
#include "game.h"
#include "options.h"
#include "score.h"
#include "floor.h"
#include "world.h"
#include "stars.h"
#include "music.h"

static Map *map;
byte bombCount;

// information about the next barrel to drop
byte barrelX,barrelY;
int barrelZ;
int barrelDZ;
byte barrelWait;
byte barrelMaxWait;
byte barrelColor;
static byte diff;

byte blowoutColorTable[]={2,1,4,5,3,6,7};

void InitBlowout(Map *m)
{
	int i,numSpots;
	int x,y;

	map=m;
	bombCount=2;
	barrelWait=1;
	barrelMaxWait=30*5-opt.blowoutDiff*30;

	diff=(opt.blowoutDiff+1)*2;
	if(diff>6)
		diff=6;

	// count how many arrow spaces there are
	numSpots=0;
	for(i=0;i<map->width*map->height;i++)
		if(FloorArrowFacing(map->map[i].floor)!=NOT_AN_ARROW)
			numSpots++;

	numSpots=numSpots/2;	// and fill half of them
	for(i=0;i<numSpots;i++)
	{
		x=Random(map->width);
		y=Random(map->height);

		if(map->map[x+y*map->width].item!=ITM_BARREL && FloorArrowFacing(map->map[x+y*map->width].floor)!=NOT_AN_ARROW)
		{
			map->map[x+y*map->width].item=ITM_BARREL;
			map->map[x+y*map->width].itemInfo=blowoutColorTable[Random(diff)];
		}
		else
		{
			i--;
		}
	}
}

static byte IsOpenSpot(void)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
	{
		if(FloorArrowFacing(map->map[i].floor)!=NOT_AN_ARROW && map->map[i].item!=ITM_BARREL)
			return 1;
	}
	return 0;
}

void DropNewBarrel(void)
{
	int x,y;

	if(!IsOpenSpot())
	{
		barrelZ=FIXAMT*900;
		barrelDZ=0;
		barrelWait=30*5;
		SendMessageToGame(MSG_RESET,0);
		return;
	}

	while(1)
	{
		x=Random(map->width);
		y=Random(map->height);

		if(map->map[x+y*map->width].item!=ITM_BARREL && FloorArrowFacing(map->map[x+y*map->width].floor)!=NOT_AN_ARROW)
			break;
	}
	barrelX=x;
	barrelY=y;
	barrelZ=FIXAMT*480;
	barrelDZ=-FIXAMT*2;
	barrelColor=blowoutColorTable[Random(diff)];
}

byte UpdateDroppingBarrel(void)
{
	Guy *g;

	barrelDZ-=FIXAMT/2;
	barrelZ+=barrelDZ;

	if(barrelZ<=0)
	{
		if(map->map[barrelX+barrelY*map->width].item==ITM_PLAYER)
		{
			PlayerExplode(map);
			barrelDZ=-barrelDZ;	// pop the barrel back up!
			return 0;
		}
		else if(map->map[barrelX+barrelY*map->width].item==ITM_BOMB ||
		   map->map[barrelX+barrelY*map->width].item==ITM_ACTIVEBOMB)
		{

			if(map->map[barrelX+barrelY*map->width].item==ITM_ACTIVEBOMB)
			{
				g=FindBomb(barrelX,barrelY);
				if(g)
					g->type=MONS_NONE;
			}
			// blow up this barrel and possibly chain
			g=AddGuy((barrelX*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (barrelY*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
			g->color=barrelColor;
			Score_BlowUp(SCORE_OBJECT,barrelColor);
			map->map[barrelX+barrelY*map->width].item=ITM_ACTIVEBOMB;
			return 1;
		}
		else if(map->map[barrelX+barrelY*map->width].item>=ITM_MONS1 &&
			map->map[barrelX+barrelY*map->width].item<=ITM_MONS4)
		{
			g=FindMonster(barrelX,barrelY);
			if(g)
			{
				Score_BlowUp(SCORE_OBJECT,barrelColor);
				barrelColor=g->color;
				g->type=MONS_NONE;
				Score_BlowUp(SCORE_MONSTER,barrelColor);
				g=AddGuy((barrelX*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (barrelY*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
				g->color=barrelColor;
				map->map[barrelX+barrelY*map->width].item=ITM_ACTIVEBOMB;
			}
		}
		else if(map->map[barrelX+barrelY*map->width].item==ITM_BARREL)
		{
			// should never happen
			SendMessageToGame(MSG_RESET,0);
			barrelDZ=-barrelDZ;	// pop the barrel back up!
			return 0;
		}
		else
		{
			// all clear at last!
			map->map[barrelX+barrelY*map->width].item=ITM_BARREL;
			map->map[barrelX+barrelY*map->width].itemInfo=barrelColor;
			return 1;
		}
	}

	if(map->map[barrelX+barrelY*map->width].templight<16)
		map->map[barrelX+barrelY*map->width].templight+=1;

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
		if(map->map[j].item==0 && !FloorIsSolid(map->map[j].floor))
		{
			map->map[j].item=ITM_BOMB;
			map->map[j].itemInfo=0;
			map->map[j].templight=16;
			return;
		}
	}
}

void UpdateBlowout(void)
{
	if(barrelWait)
	{
		barrelWait--;
		if(!barrelWait)
		{
			DropNewBarrel();
		}
	}
	else
	{
		if(UpdateDroppingBarrel())
		{
			barrelWait=barrelMaxWait;
			if(barrelMaxWait>10)
				barrelMaxWait--;
			bombCount++;
			if(bombCount==3)
			{
				bombCount=0;
				AddBomb();
			}
		}
	}
}

void RenderBlowout(void)
{
	/*
	if(!GM_doDraw)
		return;
	*/

	if(!barrelWait)
		RenderItem((barrelX*TILE_WIDTH+TILE_WIDTH/2),
				   (barrelY*TILE_HEIGHT+TILE_HEIGHT/2),
				   barrelZ/FIXAMT,ITM_BARREL,barrelColor,map->map[barrelX+barrelY*map->width].templight);
}

// -----------------------------------------------------------

// the blowout menu!
static byte *backScr;
static byte blowoutLevel=1;
static world_t world;
static byte cursor;
static byte txtLevel[6];
static sprite_set_t *blowSpr;
static byte blowSpin;
static int oldmsx,oldmsy,oldMsBtn;
static byte yesno;

void InitBlowoutMenu(void)
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

	GetDisplayMGL()->LoadBMP("graphics\\blowout.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	cursor=0;

	for(i=0;i<6;i++)
		txtLevel[i]=0;

	LoadWorld(&world,"levels\\blowout.sbl",GetDisplayMGL());

	blowSpr=new sprite_set_t("graphics\\blowout.jsp");
	blowSpin=0;
	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);
	oldMsBtn=255;

	if(blowoutLevel==0)
		blowoutLevel=1;
	LoadOptions();
	yesno=0;
	InitStars(profile.starGot[GAME_BLOWOUT][blowoutLevel-1],0);
}

void ExitBlowoutMenu(void)
{
	free(backScr);
	FreeWorld(&world);
	delete blowSpr;
	SaveOptions();
}

byte UpdateBlowoutMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte t;
	static byte fwClock;
	int msx,msy,i;
	byte btn;

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

		if(yesno==0)
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
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					case 1:	// chg map
						blowoutLevel++;
						if(blowoutLevel>world.numMaps)
							blowoutLevel=1;
						InitStars(profile.starGot[GAME_BLOWOUT][blowoutLevel-1],0);
						MakeNormalSound(SND_MENUSELECT);
						break;
					case 2:	// difficulty;
						opt.blowoutDiff++;
						if(opt.blowoutDiff>3)
							opt.blowoutDiff=0;
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
						blowoutLevel=0;
						MakeNormalSound(SND_MENUSELECT);
						return 0;
				}
			}
			if(c==27)
			{
				blowoutLevel=0;
				return 0;
			}

			if(msx!=oldmsx || msy!=oldmsy)
			{
				for(i=0;i<6;i++)
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
				for(i=0;i<6;i++)
				{
					if(msx>10 && msx<280 && msy>240-16+i*40 && msy<240+22+i*40)
					{
						cursor=i;
						switch(cursor)
						{
							case 0:	// begin
								MakeNormalSound(SND_MENUSELECT);
								return 0;
							case 1:	// chg map
								blowoutLevel++;
								if(blowoutLevel>world.numMaps)
									blowoutLevel=1;
								InitStars(profile.starGot[GAME_BLOWOUT][blowoutLevel-1],0);
								MakeNormalSound(SND_MENUSELECT);
								break;
							case 2:	// difficulty;
								opt.blowoutDiff++;
								if(opt.blowoutDiff>3)
									opt.blowoutDiff=0;
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
								blowoutLevel=0;
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
						if(blowoutLevel>0)
							ResetHighScore(GAME_BLOWOUT,blowoutLevel-1);
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
								if(blowoutLevel>0)
									ResetHighScore(GAME_BLOWOUT,blowoutLevel-1);
								yesno=0;
								cursor=3;
								MakeNormalSound(SND_MENUSELECT);
								break;
						}
					}
				}
			}
		}

		*lastTime-=TIME_PER_FRAME;
		oldmsx=msx;
		oldmsy=msy;
		oldMsBtn=btn;
	}
	return 1;
}

void RenderBlowoutMenu(MGLDraw *mgl)
{
	char txt[6][32]={
		"Let The Savings Begin!",
		"Change Location",
		"Difficulty:",
		"Show/Reset High Scores",
		"Rules",
		"Exit",
	};
	char yesnotxt[2][32]={
		"Nevermind!",
		"Really Reset High Scores",
	};
	char lvlName[][32]={
		"Normalia",
		"Chessboard",
		"Quadraphonic",
		"Tiny Franchise",
		"Islands",
		"Customers",
		"Pushy Customers",
		"Scarytown!",
	};

	int i,x;

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	if(blowoutLevel>0)
	{
		mgl->Box(314,234,635,475,0);
		if(!yesno && cursor==4)
		{
			mgl->FillBox(314,234,636,476,0);
			CenterPrint(475,276+00,"Barrels fall from the sky onto",0,0);
			CenterPrint(475,276+25,"Arrow Tiles.  Every third Barrel,",0,0);
			CenterPrint(475,276+50,"a Bomb appears somewhere.  Blow",0,0);
			CenterPrint(475,276+75,"up as many Barrels as you can.",0,0);
			CenterPrint(475,276+100,"Once all Arrow Tiles are filled,",0,0);
			CenterPrint(475,276+125,"the game is over!",0,0);
		}
		else if(yesno || cursor==3)
		{
			world.map[blowoutLevel-1]->RenderTiny(&world,315,235);
			RenderStartStar(475*FIXAMT,355*FIXAMT,mgl);
			RenderHighScore(GAME_BLOWOUT,blowoutLevel-1,2,415,270,mgl);
			RenderHighScore(GAME_BLOWOUT,blowoutLevel-1,1,415,270+60,mgl);
			RenderHighScore(GAME_BLOWOUT,blowoutLevel-1,0,415,270+60*2,mgl);
			CenterPrintOutline(475,450,lvlName[blowoutLevel-1],0,0);

		}
		else
		{
			world.map[blowoutLevel-1]->RenderTiny(&world,315,235);
			CenterPrintOutline(475,450,lvlName[blowoutLevel-1],0,0);
			RenderStartStar(475*FIXAMT,355*FIXAMT,mgl);
		}
	}

	if(!yesno)
	{
		for(i=0;i<6;i++)
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
			if(i<=opt.blowoutDiff)
				blowSpr->GetSprite(blowSpin/2)->Draw(120+i*35,328,mgl);
			else
				blowSpr->GetSprite(8)->Draw(120+i*35,328,mgl);
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

byte BlowoutMenu(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;

	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitBlowoutMenu();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateBlowoutMenu(&lastTime,mgl);
		RenderBlowoutMenu(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			ExitBlowoutMenu();
			return WORLD_ABORT;
		}
		EndClock();
	}
	then=timeGetTime();
	ResetClock(hangon);
	ExitBlowoutMenu();

	if(blowoutLevel>0)
		return LunaticWorld(blowoutLevel-1,"levels\\blowout.sbl");
	else
		return WORLD_ABORT;
}
