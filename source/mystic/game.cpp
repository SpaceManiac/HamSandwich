#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "shop.h"
#include "fairy.h"
#include "water.h"
#include "challenge.h"
#include "options.h"
#include "trivia.h"
#include "palettes.h"
#include "archipelago.h"

byte showStats=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
dword CDMessingTime;	// how long the CD messing with took, take it out of the time budget, because
						// it can bog the game, but it should just freeze the action
dword garbageTime=0;

int   visFrms;
float frmRate;
word numRunsToMakeUp;
static byte waterFlip=0;

char lastKey=0;

MGLDraw *gamemgl;
static Map		*curMap;
byte gameMode=GAMEMODE_PLAY;
byte	mapToGoTo;
byte	worldNum;
byte    mapNum;
byte	curMapFlags;
world_t curWorld;

byte battle;
byte battleFoe;
byte battleIsWon;

byte msgFromOtherModules;
byte msgContent;
static byte newGame;
word windingDown;
byte windingUp;
byte windDownReason;

#ifndef VALUE
char worldName[4][32]={
	"river.dlw",
	"forest.dlw",
	"castle.dlw",
	"under.dlw"
};
#else
char worldName[4][32]={
	"river_val.dlw",
	"forest_val.dlw",
	"castle.dlw",
	"under.dlw"
};
#endif

void LunaticInit(MGLDraw *mgl)
{
	gamemgl=mgl;

	InitOptions();
	InitCosSin();
	InitDisplay(gamemgl);
	InitSound();
	InitMonsters();
	InitTiles(mgl);
	InitItems();
	InitInterface();
	mgl->ClearKeys();
	InitControls();
	InitPlayer(INIT_GAME,0,0);
	InitShop();
	battle=0;
	VolumeSound(opt.soundVol);
	VolumeSong(opt.musicVol);
}

void LunaticExit(void)
{
	ExitShop();
	ExitItems();
	ExitSound();
	ExitDisplay();
	ExitTiles();
	ExitMonsters();
	ExitPlayer();
	ExitInterface();
	ExitOptions();
}

byte BattleMode(void)
{
	if(battle)
		return battleFoe;
	else
		return 0;
}

byte InitLevel(byte map)
{
	int i;

	JamulSoundPurge();	// each level, that should be good

	// make a copy of the map to be played
	curMap=new Map(curWorld.map[map]);

	if(player.worldNum==1 && map==14)	// maze level
		CreateMaze(curMap,16,16,8,4);
	if(player.worldNum==2 && map==17)	// castle maze level
		CreateLockedMaze(curMap,8,8,8,8);

	curMapFlags=curMap->flags;

	gameStartTime=timeGetTime();
	tickerTime=timeGetTime();
	updFrameCount=0;
	visFrameCount=0;
	numRunsToMakeUp=0;
	frmRate=30.0f;
	visFrms=0;
	msgFromOtherModules=0;

	InitGuys(128);
	InitBullets();
	InitPlayer(INIT_LEVEL,0,map);
	InitMessage();
	NewBigMessage(curMap->name,100);
	InitParticles(256);
	lastKey=0;
	curMap->Init(&curWorld);

	windingDown=0;
	windingUp=30;
	if(player.worldNum==3 && player.levelNum==11)
		windingUp=60;

	ResetInterface();
	InitCheater();

	if(map==1)	// Overworld
	{
		SetWaterwalk(0);
		OverworldUpdateGuys(curMap,&curWorld);	// this will force the camera into the right position
												// it also makes everybody animate by one frame, but no one will
												// ever notice
		for(i=0;i<30;i++)
			AddRandomGuy(curMap,&curWorld,player.worldNum+1,0);

		SetGiveUpText(0);
	}
	else	// regular level
	{
		SetWaterwalk(1);
		UpdateGuys(curMap,&curWorld);
		SetGiveUpText(1+battle);

		if(!battle && player.levelPassed[player.worldNum][player.levelNum]==1)
		{
			// you've passed this level before, clean out incriminating prizes
			GetRidOfGoodStuff(curMap);
		}

		if (auto ap = Archipelago())
		{
			for(i=0;i<curMap->width*curMap->height;i++)
			{
				byte item = curMap->map[i].item;
				if (item==ITM_KEYCH1 || item==ITM_KEYCH2 || item==ITM_KEYCH3 || item==ITM_KEYCH4
					|| item==ITM_FAIRYBELL || item==ITM_SPELLBOOK)
				{
					// ^ Each map is presumed to have exactly one of these.
					curMap->map[i].item = ap->MysticItemAtLocation(player.worldNum, player.levelNum) == ITM_NONE ? ITM_NONE : ITM_ARCHIPELAGO;
				}
			}
		}
	}
	battleIsWon=0;

	switch(curMap->song)
	{
		case 1:
			if(player.worldNum<2)
				PlaySong(SONG_CHAP12MAP);
			else
				PlaySong(SONG_CHAP34MAP);
			break;
		case 2:
			if(player.worldNum<2)
				PlaySong(SONG_CHAP12LEVEL);
			else
				PlaySong(SONG_CHAP34LEVEL);
			break;
		case 3:
			if(player.worldNum<2)
				PlaySong(SONG_CHAP12FIGHT);
			else
				PlaySong(SONG_CHAP34FIGHT);
			break;
		case 4:
			PlaySong(SONG_HAPPYSTICK);
			break;
	}
	if(player.levelNum==13 && player.worldNum==1)
		ResetTemplePuzzle(curMap);
	if(player.levelNum==19 && player.worldNum==2)	// whackazoid
		WhackazoidUpdate(1);
	SetupWater();
	InitTrivia();
	return 1;
}

void ExitLevel(void)
{
	ExitTrivia();
	// exit everything
	ExitGuys();
	ExitBullets();
	ExitParticles();

	delete curMap;
	PurgeMonsterSprites();
}

void EnterStatusScreen(void)
{
	gameMode=GAMEMODE_MENU;
}

void EnterPictureDisplay(void)
{
	gameMode=GAMEMODE_PIC;
	GetTaps();	// clear the key tap buffer
}

void EnterSpeechMode(void)
{
	gameMode=GAMEMODE_SPEECH;
	GetTaps();	// clear the key tap buffer
}

void AddGarbageTime(dword t)
{
	garbageTime+=t;
}

byte WonTheBattle(void)
{
	if(!battle)
		return 1;
	if(!MonsterExists(255))
		return 1;

	return 0;
}

byte GetGameMode(void)
{
	return gameMode;
}

TASK(byte) LunaticRun(int *lastTime)
{
	int b;

	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*4)
		*lastTime=TIME_PER_FRAME*4;

	while(*lastTime>=TIME_PER_FRAME)
	{

		if(!gamemgl->Process())
		{
			mapToGoTo=255;
			CO_RETURN LEVEL_ABORT;
		}

		b=TotalBrains();
		if(b!=0)
			b=128-(player.brains*128/b);

		UpdateInterface(player.life,player.hammerFlags,player.hammers,b,player.score,player.weapon,player.ammo,player.hamSpeed);

		player.gameClock++;
		if(gameMode==GAMEMODE_PLAY)
		{
			// update everything here
			if(!windingDown)
			{
				if(windingUp)
				{
					curMap->Update(UPDATE_FADEIN,&curWorld);
					NoMoveUpdateGuys(curMap);
					windingUp--;
				}
				else
				{
					if(newGame)
					{
						newGame=0;
						if(player.nightmare==1)
							InitSpeech(22);
						else if(player.nightmare==2)
							InitSpeech(25);
						else
							InitSpeech(20);
					}

					curMap->Update(UPDATE_GAME,&curWorld);
					if(player.levelNum!=1)
						UpdateGuys(curMap,&curWorld);
					else
					{
						AddRandomGuy(curMap,&curWorld,player.worldNum+1,1);
						OverworldUpdateGuys(curMap,&curWorld);
					}
					ArmageddonUpdate(curMap);
					UpdateBullets(curMap,&curWorld);
					SpecialAnytimeCheck(curMap);
					if(Challenging())
					{
						if(!ChallengeUpdate())
						{
							SendMessageToGame(MSG_RESET,0);
							ChallengeEvent(CE_DIE,0);
						}
					}
				}
			}
			else
			{
				curMap->Update(UPDATE_FADE,&curWorld);
				EditorUpdateGuys(curMap);
			}
			waterFlip=1-waterFlip;
			if(waterFlip)
				UpdateWater();
			UpdateParticles(curMap);
			UpdateMessage();
			UpdateItems();
			UpdateUnPausedMenu();

			if(DoTrivia())
				UpdateTrivia();

			if(player.levelNum==19 && player.worldNum==2)	// whackazoid
				WhackazoidUpdate(0);
			if(player.levelNum==13 && player.worldNum==3)	// smashball
				SmashballCheck(curMap);

			if(curMap->flags&MAP_SNOWING)
				MakeItSnow(curMap);

			if(windingDown)
			{
				windingDown--;
				if(!windingDown)
				{
					if(windDownReason==LEVEL_SHOP)
					{
						gameMode=GAMEMODE_SHOP;
					}
					else if(windDownReason==LEVEL_FAIRY)
					{
						gameMode=GAMEMODE_FAIRY;
					}
					else
						CO_RETURN windDownReason;
				}
			}
			if(battle==1 && (!battleIsWon) && BadguyCount()==0)
			{
				MakeNormalSound(SND_KOOLKAT);
				NewMessage("I'm Kool Kat Bad!",60);
				battleIsWon=1;
			}
		}
		else if(gameMode==GAMEMODE_MENU)
		{
			switch(UpdatePauseMenu(gamemgl))
			{
				case 0:
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					break;
				case 1:
					break;
				case 2:
					StopSong();
					if(mapNum)
						mapToGoTo=1;
					else
						mapToGoTo=255;
					lastKey=0;
					if(!MonsterExists(255))
						battleIsWon=1;
					if(battle==1 && (!battleIsWon))
					{
						NewBigMessage("You Ran Away!",75);
						if(!(player.gear&GEAR_FEATHER))
						{
							MakeNormalSound(SND_HAPPYOUCH);
							player.overworldX=-2000;	// you didn't kill them all, so you lose your place as penalty
							PlayerResetScore();
						}
						else
							MakeNormalSound(SND_CHICKEN);

						mapToGoTo=1;
						windingDown=30;
						windDownReason=LEVEL_ABORT;
						msgFromOtherModules=MSG_NONE;
						battle=0;
						lastKey=0;
						gameMode=GAMEMODE_PLAY;
						break;
					}
					else if(battle==1 && (battleIsWon))
					{
						battle=0;
						CO_RETURN LEVEL_WIN;
					}
					else
						PlayerResetScore();	// if a regular level, reset the score
					battle=0;
					ChallengeEvent(CE_DIE,0);
					CO_RETURN LEVEL_ABORT;
					break;
				case 3:
					mapToGoTo=255;
					lastKey=0;
					ChallengeEvent(CE_QUIT,0);
					CO_RETURN WORLD_QUITGAME;	// dump out altogether
					break;
			}
		}
		else if(gameMode==GAMEMODE_SHOP)
		{
			if(UpdateShop(gamemgl)==1)
			{
				LeaveShop();
				switch(player.worldNum)
				{
					case 0:
					case 1:
						PlaySong(SONG_CHAP12MAP);
						break;
					case 2:
					case 3:
						PlaySong(SONG_CHAP34MAP);
						break;
				}
				gameMode=GAMEMODE_PLAY;
				windingUp=30;
				GetGoodguy()->dx=0;
				GetGoodguy()->dy=0;
			}
		}
		else if(gameMode==GAMEMODE_FAIRY)
		{
			if(UpdateFairyBox(gamemgl)==1)
			{
				LeaveFairyBox();
				switch(player.worldNum)
				{
					case 0:
					case 1:
						PlaySong(SONG_CHAP12MAP);
						break;
					case 2:
					case 3:
						PlaySong(SONG_CHAP34MAP);
						break;
				}
				gameMode=GAMEMODE_PLAY;
				windingUp=5;
				GetGoodguy()->dx=0;
				GetGoodguy()->dy=0;
			}
		}
		else if(gameMode==GAMEMODE_SPEECH)
		{
			if(UpdateSpeech(gamemgl)==1)
			{
				gameMode=GAMEMODE_PLAY;
			}
		}
		else	// gamemode_pic
		{
			if(GetTaps()&(CONTROL_B1|CONTROL_B2))
			{
				gameMode=GAMEMODE_PLAY;
				// restore the palette
				gamemgl->LoadBMP("graphics/title.bmp");
			}
		}

		if(msgFromOtherModules==MSG_GOTOMAP)
		{
			if(msgContent==50)	// That's secret code for "go to the shop"
			{
				EnterShop();
				windingDown=30;
				windDownReason=LEVEL_SHOP;
				msgFromOtherModules=MSG_NONE;
			}
			else if(msgContent==51)	// That's secret code for "go to the fairy box"
			{
				EnterFairyBox();
				windingDown=2;
				windDownReason=LEVEL_FAIRY;
				msgFromOtherModules=MSG_NONE;
			}
			else
			{
				if(!MonsterExists(255))
					battleIsWon=1;
				if(battle==1 && (!battleIsWon))
				{

					NewBigMessage("You Ran Away!",75);
					if(!(player.gear&GEAR_FEATHER))
					{
						MakeNormalSound(SND_HAPPYOUCH);
						player.overworldX=-2000;	// you didn't kill them all, so you lose your place as penalty
						PlayerResetScore();
					}
					else
						MakeNormalSound(SND_CHICKEN);
				}
				mapToGoTo=msgContent;
				windingDown=30;
				windDownReason=LEVEL_ABORT;
				msgFromOtherModules=MSG_NONE;
				battle=0;
			}
		}
		else if(msgFromOtherModules==MSG_WINLEVEL)
		{
			if(!MonsterExists(255))
				battleIsWon=1;
			if(battle==1 && (!battleIsWon))
			{
				NewBigMessage("You Ran Away!",75);
				if(!(player.gear&GEAR_FEATHER))
				{
					MakeNormalSound(SND_HAPPYOUCH);
					player.overworldX=-2000;	// you didn't kill them all, so you lose your place as penalty
					PlayerResetScore();
				}
				else
					MakeNormalSound(SND_CHICKEN);
			}
			battle=0;
			mapToGoTo=0;
			if(msgContent==1)
				windingDown=1;
			else
				windingDown=40;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_RESET)
		{
			NewBigMessage("Try Again!",30);
			windingDown=30;
			windDownReason=LEVEL_RESET;
			msgFromOtherModules=MSG_NONE;
			if(battle==1)
			{
				windDownReason=LEVEL_ABORT;
				mapToGoTo=1;
				battle=0;
				if(!battleIsWon && !(player.gear&GEAR_FEATHER))
					player.overworldX=-2000;	// you didn't kill them all, so you lose your place as penalty
			}
		}
		else if(msgFromOtherModules==MSG_LOADGAME)
		{
			NewBigMessage("Loading Game",30);
			windingDown=30;
			windDownReason=LEVEL_LOADING;
			msgFromOtherModules=MSG_NONE;
			battle=0;
			newGame=0;
		}
		else if(msgFromOtherModules==MSG_WINGAME)
		{
			AWAIT ShowVictoryAnim(0);	// you killed the final boss
			mapToGoTo=0;
			windingDown=1;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
			AWAIT ShowVictoryAnim(4);
		}
		else if(msgFromOtherModules==MSG_BATTLE)
		{
			switch(player.worldNum)	// different chapters have different random levels
			{
				case 0:
					mapToGoTo=MGL_random(4)+6;
					break;
				case 1:
					mapToGoTo=MGL_random(4)+7;
					break;
				case 2:
					mapToGoTo=MGL_random(4)+2;
					break;
				case 3:
					mapToGoTo=MGL_random(4)+2;
					break;
			}
			battle=1;
			battleIsWon=0;
			battleFoe=msgContent;
			windingDown=30;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
		}
		else if (msgFromOtherModules==MSG_SHOWANIM)
		{
			AWAIT ShowVictoryAnim(msgContent);
			msgFromOtherModules=MSG_NONE;
		}

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}
	CDMessingTime=0;	// that's how long CD messing took
	CDMessingTime+=garbageTime;	// time wasted with such things as playing animations
	garbageTime=0;
	JamulSoundUpdate();

	if (auto ap = Archipelago())
		ap->Update();

	CO_RETURN LEVEL_PLAYING;
}

TASK(void) LunaticDraw(void)
{
	char s[32];
	dword d;

	// add all the sprites to the list
	if(gameMode!=GAMEMODE_PIC && gameMode!=GAMEMODE_SHOP && gameMode!=GAMEMODE_FAIRY)
	{
		RenderGuys(1);
		RenderBullets();
		RenderParticles();
		RenderItAll(&curWorld,curMap,MAP_SHOWLIGHTS|MAP_SHOWITEMS|MAP_SHOWWALLS);
		RenderSpecialXes(gamemgl,curMap,worldNum);
		RenderMessage();
		PlayerRenderInterface(gamemgl);
		RenderPauseMenu();
		if(gameMode==GAMEMODE_SPEECH)
			RenderSpeech();
	}
	else if(gameMode==GAMEMODE_SHOP)
	{
		//gamemgl->ClearScreen();
		RenderShop();
	}
	else if(gameMode==GAMEMODE_FAIRY)
	{
		//gamemgl->ClearScreen();
		RenderFairyBox();
	}
	else
	{
		// nothing to do for gamemode_pic
	}

	if(Challenging())
		ChallengeRender(gamemgl);

	if(DoTrivia())
		RenderTrivia(gamemgl);

	if(showStats)
	{
		sprintf(s,"QFPS %02.2f",frmRate);
		Print(0,180,s,6,0);
		sprintf(s,"VFPS %02.2f",((float)visFrameCount/(float)((timeGetTime()-gameStartTime)/1000)));
		Print(0,10,s,6,0);
		sprintf(s,"GFPS %02.2f",((float)updFrameCount/(float)((timeGetTime()-gameStartTime)/1000)));
		Print(0,50,s,6,0);
		sprintf(s,"Runs %d",numRunsToMakeUp);
		Print(0,100,s,6,0);
	}
	// update statistics
	d=timeGetTime();
	if(d-tickerTime>999)
	{
		frmRate=(frmRate*3+((float)visFrms/((float)(d-tickerTime)/1000.0f)))/4;
		visFrms=0;
		tickerTime=d;
	}

	AWAIT gamemgl->Flip();
	CDMessingTime+=garbageTime;
	garbageTime=0;

	visFrameCount++;
	visFrms++;
}

void SendMessageToGame(byte msg,int content)
{
	msgFromOtherModules=msg;
	msgContent=content;
}

void HandleKeyPresses(void)
{
	char k;

	k=gamemgl->LastKeyPressed();
	if(k)
	{
		lastKey=k;
		if((lastKey>='a' && lastKey<='z') || (lastKey>='A' && lastKey<='Z'))
			CheatKey(lastKey);
	}
#ifndef NDEBUG
	// can't show stats unless in debug mode
	if(lastKey=='S')
	{
		showStats=1-showStats;
		lastKey=0;
	}
#endif
	if(lastKey=='g')
	{
		k=GetGamma();
		k++;
		if(k>3)
			k=0;
		GammaCorrect(gamemgl, k);
		SetGamma(k);
		lastKey=0;
	}
	if(lastKey>='0' && lastKey<='9')
	{
		k=lastKey-'0';
		if(k==0)
			k=10;
		k--;
		if(player.spell[k])
			player.curSpell=k;
	}
	if(lastKey==27 && gameMode==GAMEMODE_PLAY && !windingUp && !windingDown && !newGame)
	{
		InitPauseMenu();
		gameMode=GAMEMODE_MENU;
	}
}

TASK(byte) PlayALevel(byte map)
{
	int lastTime=1;
	byte exitcode=0;

	if(!InitLevel(map))
	{
		CO_RETURN LEVEL_ABORT;
		mapToGoTo=255;
	}

	exitcode=LEVEL_PLAYING;
	gameMode=GAMEMODE_PLAY;
	CDMessingTime=0;
	garbageTime=0;

	StartClock();
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=TimeLength()-CDMessingTime;
		StartClock();
		if(gameMode==GAMEMODE_PLAY)
			HandleKeyPresses();
		exitcode=AWAIT LunaticRun(&lastTime);
		//if(numRunsToMakeUp>0)
			AWAIT LunaticDraw();

		if(!gamemgl->Process())
		{
			exitcode=LEVEL_ABORT;
			mapToGoTo=255;
		}
		EndClock();
	}

	if(Challenging())
		AWAIT ChallengeTally(gamemgl);

	ExitLevel();
	CO_RETURN exitcode;
}

TASK(byte) PlayOverworld(void)
{
	int lastTime=1;
	byte exitcode=0;

	if(!InitLevel(1))
	{
		CO_RETURN LEVEL_ABORT;
		mapToGoTo=255;
	}

	exitcode=LEVEL_PLAYING;
	gameMode=GAMEMODE_PLAY;
	CDMessingTime=0;
	garbageTime=0;

	StartClock();
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=TimeLength()-CDMessingTime;
		StartClock();
		if(gameMode==GAMEMODE_PLAY && !windingUp && !windingDown)
			HandleKeyPresses();
		exitcode=AWAIT LunaticRun(&lastTime);
		//if(numRunsToMakeUp>0)
			AWAIT LunaticDraw();

		if(!gamemgl->Process())
		{
			exitcode=LEVEL_ABORT;
			mapToGoTo=255;
		}
		EndClock();
	}

	ExitLevel();
	CO_RETURN exitcode;
}

TASK(byte) ChallengePlay(byte world,byte lvl)
{
	byte result;

	if(!LoadWorld(&curWorld,worldName[world]))
		CO_RETURN WORLD_ABORT;

	InitPlayer(INIT_WORLD,world,0);

	worldNum=world;
	InitWorld(&curWorld,worldNum);
	ResetPauseMenu();

	mapNum=lvl;
	battle=0;
	result=AWAIT PlayALevel(mapNum);

	FreeWorld(&curWorld);
	CO_RETURN 0;
}

TASK(byte) LunaticWorld(byte world)
{
	byte result;

	if(!LoadWorld(&curWorld,worldName[world]))
		CO_RETURN WORLD_ABORT;

	InitPlayer(INIT_WORLD,world,0);

	worldNum=world;
	InitWorld(&curWorld,worldNum);
	ResetPauseMenu();

	mapNum=1;
	battle=0;
	while(1)
	{
		if(mapNum!=1)
			result=AWAIT PlayALevel(mapNum);
		else
			result=AWAIT PlayOverworld();

		if(player.worldNum==3 && mapNum==18)
		{
			player.worldNum=0;
			player.nightmare=2;
			FreeWorld(&curWorld);
			if(!LoadWorld(&curWorld,worldName[player.worldNum]))
				CO_RETURN WORLD_ABORT;
			ResetPlayerLevels();
			InitWorld(&curWorld,worldNum);
			InitPlayer(INIT_WORLD,player.worldNum,1);

			worldNum=player.worldNum;
			world=worldNum;
			ResetPauseMenu();
			player.overworldX=-2000;
			mapNum=1;
			battle=0;
			newGame=1;
			result=LEVEL_PLAYING;
		}

		if(result==LEVEL_ABORT)
		{
			if(mapToGoTo<255)
				mapNum=mapToGoTo;
			else
				break;
		}
		else if(result==LEVEL_RESET)
		{
			PlayerResetScore();
			// don't do anything, play the same level
		}
		else if(result==LEVEL_WIN)
		{
			 mapNum=player.levelNum;
			PlayerWinLevel(world,mapNum,curMapFlags&MAP_SECRET);
			if((player.worldNum==0 && mapNum==14) ||
				(player.worldNum==1 && mapNum==12) ||
				(player.worldNum==2 && mapNum==15))
			{
#ifdef DEMO
				ShowVictoryAnim(5);
				return WORLD_NAG;
#else
				player.worldNum++;
				InitPlayer(INIT_WORLD,player.worldNum,1);
				FreeWorld(&curWorld);
				if(!LoadWorld(&curWorld,worldName[player.worldNum]))
					CO_RETURN WORLD_ABORT;

				worldNum=player.worldNum;
				world=worldNum;
				InitWorld(&curWorld,worldNum);
				ResetPauseMenu();
				player.overworldX=-2000;
				mapNum=1;
				battle=0;
				AWAIT ShowVictoryAnim(player.worldNum);
#endif
			}
			else if(player.worldNum==3 && mapNum==11)
			{
				AWAIT ShowVictoryAnim(4);
				//return WORLD_QUITGAME;
				if(player.nightmare)
				{
					player.worldNum=3;
					player.levelNum=18;
					mapNum=18;
					worldNum=player.worldNum;
					world=worldNum;
					ResetPauseMenu();
					player.overworldX=-2000;
				}
				else
				{
					player.worldNum=0;
					player.nightmare=1;
					FreeWorld(&curWorld);
					if(!LoadWorld(&curWorld,worldName[player.worldNum]))
						CO_RETURN WORLD_ABORT;
					ResetPlayerLevels();
					InitWorld(&curWorld,worldNum);
					InitPlayer(INIT_WORLD,player.worldNum,1);

					worldNum=player.worldNum;
					world=worldNum;
					ResetPauseMenu();
					player.overworldX=-2000;
					mapNum=1;
					battle=0;
					newGame=1;
				}
			}
			else
				mapNum=1;
		}
		else if(result==LEVEL_LOADING)
		{
			FreeWorld(&curWorld);
			PlayerResetScore();
			CO_RETURN WORLD_LOAD;
		}
		else if(result==WORLD_QUITGAME)
		{
			FreeWorld(&curWorld);
			PlayerResetScore();
			CO_RETURN WORLD_QUITGAME;
		}
	}
	FreeWorld(&curWorld);
	CO_RETURN WORLD_ABORT;
}

TASK(byte) LunaticGame(MGLDraw *mgl,byte load)
{
	byte b,worldResult;

	if(!load)	// don't do this if loading a game, it was already done and the player was filled with values
	{
		InitPlayer(INIT_GAME,0,0);
		AWAIT ShowVictoryAnim(0);
	}

	newGame=0;
	if(load>0)	// continuing a saved game
		worldResult=WORLD_LOAD;
	else
	{
		worldResult=0;
		newGame=1;
	}
	while(1)
	{
		b=player.worldNum;

		worldResult=AWAIT LunaticWorld(b);

		if(worldResult==WORLD_QUITGAME)
		{
			mgl->LastKeyPressed();	// just to clear key buffer
			break;
		}
		if(worldResult==WORLD_NAG)
		{
			ExitPlayer();
			CO_RETURN 1;
		}
	}
	ExitPlayer();
	CO_RETURN 0;
}

/*void TrainingGame(MGLDraw *mgl)
{
	InitPlayer(INIT_GAME,0,0);
	SetCustomName("training.dlw");
	if(AWAIT LunaticWorld(5)==WORLD_LOAD)
		LunaticGame(mgl,1);
	mgl->LastKeyPressed();	// just to clear key buffer
	ExitPlayer();
}*/

Map *CurrentMap(void)
{
	return curMap;
}
