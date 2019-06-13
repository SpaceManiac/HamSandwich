#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "options.h"
#include "quest.h"
#include "survival.h"
#include "endgame.h"
#include "loonyball.h"
#include "bowling.h"
#include "bossbash.h"
#include "help.h"
#include "ch_summon.h"
#include "log.h"
#include "palettes.h"

byte showStats=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
dword garbageTime=0;

int   visFrms;
float frmRate;
word numRunsToMakeUp;

char lastKey=0;

MGLDraw *gamemgl;
Map		*curMap;
byte gameMode=GAMEMODE_PLAY;
byte	mapToGoTo;
byte	worldNum;
byte    mapNum;
byte	curMapFlags;
world_t curWorld;

byte msgFromOtherModules=0;
byte msgContent;

word windingDown;
byte windingUp;
byte windDownReason;
byte loadGame;
static byte idleGame=0;

void LunaticInit(MGLDraw *mgl)
{
	gamemgl=mgl;

	InitCosSin();
	InitDisplay(gamemgl);
	InitSound();
	InitTiles(mgl);
	InitItems();
	InitInterface();
	LoadOptions();
	InitMonsters();

	mgl->ClearKeys();
	SeedRNG();
	InitControls();
	InitPlayer(INIT_GAME,0,0);
	msgFromOtherModules=0;
}

void LunaticExit(void)
{
	ExitItems();
	ExitSound();
	ExitDisplay();
	ExitTiles();
	ExitMonsters();
	ExitPlayer();
	ExitInterface();
}

byte GetCurSong(void)
{
	if(!curMap)
		return 3;
	return curMap->song;
}

byte InitLevel(byte map)
{
	JamulSoundPurge();	// each level, that should be good

	if(opt.cheats[CH_VINTAGE])
		GreyPalette(gamemgl);

	if(loadGame)
	{
		LoadGame(loadGame-1);
		loadGame=0;
		map=player.levelNum;

		if(curWorld.numMaps<=map)
			return 0;	// can't go to illegal map

		curMapFlags=curMap->flags;
		gameStartTime=timeGetTime();
		tickerTime=timeGetTime();
		updFrameCount=0;
		visFrameCount=0;
		numRunsToMakeUp=0;
		frmRate=30.0f;
		visFrms=0;

		InitBullets();
		InitPlayer(INIT_LEVEL,0,map);
		InitMessage();
		InitParticles(512);
		lastKey=0;
		CameraOnPlayer(0);
		SpecialAnytimeCheck(curMap);

		windingDown=0;
		windingUp=20;
		ResetInterface();
		InitCheater();
	}
	else
	{
		if(curWorld.numMaps<=map)
			return 0;	// can't go to illegal map

		// make a copy of the map to be played
		curMap=new Map(curWorld.map[map]);

		curMapFlags=curMap->flags;

		gameStartTime=timeGetTime();
		tickerTime=timeGetTime();
		updFrameCount=0;
		visFrameCount=0;
		numRunsToMakeUp=0;
		frmRate=30.0f;
		visFrms=0;
		if(msgFromOtherModules!=MSG_NEWFEATURE)
			msgFromOtherModules=0;

		InitGuys(MAX_MAPMONS);
		InitBullets();
		InitPlayer(INIT_LEVEL,0,map);
		InitMessage();
		InitParticles(512);
		lastKey=0;
		curMap->Init(&curWorld);
		CameraOnPlayer(1);
		SpecialAnytimeCheck(curMap);

		windingDown=0;
		windingUp=20;
		ResetInterface();
		InitCheater();
		if(player.worldNum==WORLD_SURVIVAL)
			SurvivalBeginMsg();
		if(player.worldNum==WORLD_LOONYBALL)
			InitLoonyball(curMap);
		if(player.worldNum==WORLD_BOWLING)
			InitBowling(curMap);
	}

	if(player.worldNum==WORLD_BOSSBASH)
		MakeNormalSound(SND_SCARYBOSS);

	return 1;
}

void ExitLevel(void)
{
	// exit everything
	if(player.monsType==MONS_PLYRSUMMON)
		GrabSummons();
	ExitGuys();
	ExitBullets();
	ExitParticles();

	delete curMap;
	curMap=NULL;
	PurgeMonsterSprites();
}

void SetGameIdle(bool b)
{
	idleGame=b;
}

byte GetGameIdle(void)
{
	return idleGame;
}

// this is run whenever the game is swapped away from
void GameIdle(void)
{
	dword start,end;

	start=timeGetTime();
	while(idleGame)
	{
		if(!gamemgl->Process())
			break;
	}
	end=timeGetTime();
	AddGarbageTime(end-start);
	player.boredom=0;
	return;
}

void EnterStatusScreen(void)
{
	player.reload=20;
	player.wpnReload=20;
	gameMode=GAMEMODE_MENU;
}

void EnterPictureDisplay(void)
{
	gameMode=GAMEMODE_PIC;
	GetTaps();	// clear the key tap buffer
}

void EnterChatMode(void)
{
	gameMode=GAMEMODE_CHAT;
	GetTaps();	// clear the key tap buffer
}

void ExitChatMode(void)
{
	player.reload=20;
	player.wpnReload=20;
	gameMode=GAMEMODE_PLAY;
}

void AddGarbageTime(dword t)
{
	garbageTime+=t;
}

byte WindingDown(void)
{
	return (windingDown!=0);
}

void HandleKeyPresses(void);

void UndoWindDown(void)
{
	windingDown=0;
	windingUp=10;
}

byte LunaticRun(int *lastTime)
{
	static byte flip=0;

	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*2)
		*lastTime=TIME_PER_FRAME*2;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(!gamemgl->Process())
		{
			mapToGoTo=255;
			return LEVEL_ABORT;
		}

		if(gameMode==GAMEMODE_PLAY)
		{
			// update everything here
			if(!windingDown)
			{
				if(windingUp)
				{
					curMap->Update(UPDATE_FADEIN,&curWorld);
					EditorUpdateGuys(curMap);
					windingUp--;
				}
				else
				{
					curMap->Update(UPDATE_GAME,&curWorld);
					UpdateGuys(curMap,&curWorld);
					UpdateBullets(curMap,&curWorld);
					SpecialAnytimeCheck(curMap);
					UpdateItems();
					if(player.clockRunning)
						player.timeLimit++;

					if(player.worldNum==WORLD_SURVIVAL)
					{
						if(SurvivalWin(curMap))
						{
							windingDown=30;
							windDownReason=LEVEL_WIN;
						}
					}
					if(player.worldNum==WORLD_LOONYBALL)
					{
						UpdateLoonyball();
					}
					if(player.worldNum==WORLD_BOWLING)
					{
						UpdateBowling();
					}
					if(player.worldNum==WORLD_BOSSBASH)
					{
						UpdateBash();
					}
				}
			}
			else
			{
				curMap->Update(UPDATE_FADE,&curWorld);
				EditorUpdateGuys(curMap);
			}
			UpdateParticles(curMap);
			UpdateMessage();

			if(opt.cheats[CH_DISCO])
			{
				flip=1-flip;

				if(flip)
				{
					CyclePalette(gamemgl, updFrameCount % 256);
				}
			}

			if(curMap->flags&MAP_SNOWING)
				MakeItSnow(curMap);

			if(windingDown)
			{
				windingDown--;
				if(!windingDown)
					return windDownReason;
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
				case 2:
					break;
				case 3:
					if(mapNum)
						mapToGoTo=0;
					else
						mapToGoTo=255;
					lastKey=0;
					return LEVEL_ABORT;
					break;
				case 4:
					mapToGoTo=255;
					lastKey=0;
					return WORLD_QUITGAME;	// dump out altogether
					break;
			}
			worldNum=player.worldNum;
		}
		else if(gameMode==GAMEMODE_PIC)	// gamemode_pic
		{
			if(GetTaps()&(CONTROL_B1|CONTROL_B2))
			{
				gameMode=GAMEMODE_PLAY;
				// restore the palette
				gamemgl->LoadBMP("graphics/title.bmp");
			}
		}
		else if(gameMode==GAMEMODE_CHAT)
		{
			UpdateChat();
		}

		if(msgFromOtherModules==MSG_GOTOMAP)
		{
			mapToGoTo=msgContent;
			if(mapToGoTo!=0)
				MakeNormalSound(SND_ENTERMAP);
			windingDown=40;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
			PlayerClearTempVars();
		}
		else if(msgFromOtherModules==MSG_WINLEVEL)
		{
			DBG("GotWinLevel");
			mapToGoTo=0;
			if(msgContent==1)
				windingDown=1;
			else
				windingDown=40;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
			player.boredom=0;
		}
		else if(msgFromOtherModules==MSG_RESET)
		{
			if(player.worldNum==WORLD_SURVIVAL || player.worldNum==WORLD_SLINGSHOT ||
				((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && (player.cheatsOn&PC_HARDCORE)) ||
				player.worldNum==WORLD_LOONYBALL || player.worldNum==WORLD_BOSSBASH)
				NewBigMessage("You were defeated!",30);
			else if(player.worldNum!=WORLD_BOWLING)
				NewBigMessage("Try Again!",30);
			windingDown=30;
			windDownReason=LEVEL_RESET;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_LOADGAME)
		{
			NewBigMessage("Loading Game",30);
			windingDown=30;
			windDownReason=LEVEL_LOADING;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_WINGAME)
		{
			mapToGoTo=0;
			windingDown=1;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
			player.boredom=0;
		}
		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}
	JamulSoundUpdate();

	return LEVEL_PLAYING;
}

void LunaticDraw(void)
{
	char s[32];
	dword d;

	// add all the sprites to the list
	if(gameMode!=GAMEMODE_PIC)
	{
		RenderGuys(1);
		RenderBullets();
		RenderParticles();
		RenderItAll(&curWorld,curMap,MAP_SHOWLIGHTS|MAP_SHOWITEMS|MAP_SHOWWALLS);
		RenderMessage();
		RenderInterface(gamemgl);
		if(gameMode==GAMEMODE_MENU)
			RenderPauseMenu(gamemgl);
		if(gameMode==GAMEMODE_CHAT)
			RenderChat(gamemgl);
	}
	else
	{
		// nothing to do
	}

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

	gamemgl->Flip();

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
//#ifdef _DEBUG
	// can't show stats unless in debug mode
	/*
	if(lastKey=='s')
	{
		EndGameTally(gamemgl);
		showStats=1-showStats;
		lastKey=0;
	}
	*/
//#endif
}

byte PlayALevel(byte map)
{
	int lastTime=1;
	byte exitcode=0;
	dword tl;

	if(player.worldNum==WORLD_SURVIVAL)
	{
		map=(byte)Random(7);	// play on a random map!
		if(GetSurvLevel()==23)
			map=6;	// frankenj can only be on the empty map
	}
	if(!InitLevel(map))
	{
		mapToGoTo=255;
		return LEVEL_ABORT;
	}

	exitcode=LEVEL_PLAYING;
	gameMode=GAMEMODE_PLAY;
	garbageTime=0;

	PrepGuys(curMap);

	tl=0;
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=tl;
		garbageTime=0;
		StartClock();
		exitcode=LunaticRun(&lastTime);
		LunaticDraw();

		if(lastKey==27 && gameMode==GAMEMODE_PLAY)
		{
			InitPauseMenu();
			gameMode=GAMEMODE_MENU;
		}

		if(!gamemgl->Process())
		{
			exitcode=LEVEL_ABORT;
			mapToGoTo=255;
		}
		EndClock();
		tl=TimeLength()-garbageTime;
		if(gameMode==GAMEMODE_PLAY)
			HandleKeyPresses();
	}

	ExitLevel();
	return exitcode;
}

byte LunaticWorld(byte world,const char *worldName)
{
	byte result;

	if(!LoadWorld(&curWorld,worldName))
		return WORLD_ABORT;

	worldNum=world;
	InitWorld(&curWorld,worldNum);
	if(player.worldNum==WORLD_SURVIVAL)
		InitSurvival();

	if(player.worldNum==WORLD_BOSSBASH)
	{
		player.levelNum=BossMenu(gamemgl);
		if(player.levelNum==0)
		{
			SetSongRestart(0);
			FreeWorld(&curWorld);
			return WORLD_ABORT;
		}
		InitPlayer(INIT_GAME,WORLD_BOSSBASH,player.levelNum);
		SetSongRestart(1);
	}

	mapNum=player.levelNum;
	while(1)
	{
		result=PlayALevel(mapNum);
		if(result==LEVEL_ABORT)
		{
			if(mapToGoTo<255)
				mapNum=mapToGoTo;
			else
				break;
		}
		else if(result==LEVEL_RESET)
		{
			if(player.worldNum==WORLD_SURVIVAL || player.worldNum==WORLD_SLINGSHOT ||
				player.worldNum==WORLD_BOWLING || player.worldNum==WORLD_LOONYBALL ||
				player.worldNum==WORLD_BOSSBASH)
			{
				EndGameTally(gamemgl);
				FreeWorld(&curWorld);
				return WORLD_QUITGAME;	// you LOSE! (or win in the case of bowling)
			}
			if(player.cheatsOn&PC_HARDCORE)
			{
				if(msgContent!=255)
					DeleteSave(msgContent+1);
				EndGameTally(gamemgl);
				FreeWorld(&curWorld);
				return WORLD_QUITGAME;	// you LOSE!!
			}
			// reload saved game
			if(msgContent!=255)
			{
				mapNum=255;
				loadGame=msgContent+1;
			}
			else
			{
				mapNum=0;
				InitPlayer(INIT_GAME,0,0);
			}
		}
		else if(result==LEVEL_LOADING)
		{
			FreeWorld(&curWorld);
			return WORLD_LOAD;
		}
		else if(result==WORLD_QUITGAME)
		{
			FreeWorld(&curWorld);
			return WORLD_QUITGAME;
		}
		else if(result==LEVEL_WIN)
		{
			if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX))
			{
				// won the game!  Go back to Luniton.
				ShowVictoryAnim(0);
				VictoryText(gamemgl);
				Credits(gamemgl,0);
				JamulSoundPurge();
				EndGameTally(gamemgl);
				mapNum=0;
			}
			if(player.worldNum==WORLD_LOONYBALL)
			{
				// go on to the next
				mapNum++;
				if(mapNum==5)
				{
					player.levelNum=5;
					EndGameTally(gamemgl);
					FreeWorld(&curWorld);
					return WORLD_QUITGAME;	// you win!
				}
			}
			if(player.worldNum==WORLD_BOSSBASH)
			{
				DBG("ReadyToTally");
				EndGameTally(gamemgl);
				FreeWorld(&curWorld);
				return WORLD_QUITGAME;	// you win!
			}
		}
		if(player.worldNum==WORLD_SURVIVAL && SurvivalOver())
		{
			EndGameTally(gamemgl);
			FreeWorld(&curWorld);
			return WORLD_QUITGAME;
		}
	}
	FreeWorld(&curWorld);
	return WORLD_ABORT;
}

void LunaticGame(MGLDraw *mgl,byte load,byte mode)
{
	byte worldResult;

	InitPlayer(INIT_GAME,mode,0);

	while(1)
	{
		loadGame=load;
		SetNoSaving(0);
		switch(player.worldNum)
		{
			case WORLD_NORMAL:
				if(!loadGame)
					Help(gamemgl);
				worldResult=LunaticWorld(0,"loony.llw");
				break;
			case WORLD_REMIX:
				if(!loadGame)
					Help(gamemgl);
				worldResult=LunaticWorld(WORLD_REMIX,"remix.llw");
				break;
			case WORLD_SURVIVAL:
				Help(gamemgl);
				worldResult=LunaticWorld(0,"survive.llw");
				break;
			case WORLD_SLINGSHOT:
				Help(gamemgl);
				worldResult=LunaticWorld(0,"sling.llw");
				break;
			case WORLD_LOONYBALL:
				Help(gamemgl);
				worldResult=LunaticWorld(0,"ball.llw");
				break;
			case WORLD_BOWLING:
				Help(gamemgl);
				worldResult=LunaticWorld(0,"bowl.llw");
				break;
			case WORLD_BOSSBASH:
				Help(gamemgl);
				worldResult=LunaticWorld(0,"boss.llw");
				break;
		}
		if(worldResult==WORLD_QUITGAME)
		{
			mgl->LastKeyPressed();	// just to clear key buffer
			break;
		}
		if(worldResult==WORLD_ABORT)
			break;
	}
	ExitPlayer();
}
