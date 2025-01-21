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
#include "loonyArchipelago.h"

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
byte    mapNum;
byte	curMapFlags;
world_t curWorld;

byte msgFromOtherModules=0;
byte msgContent;

word windingDown;
byte windingUp;
byte windDownReason;
byte loadGame;

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
	KillSong();

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

		InitGuys(MAX_MAPMONS * 2);  // Leave room for Farley and summons
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
			UpdateArchipelago();

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
				case PauseMenuResult::Continue:
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					// try to prevent losing your gems if using controller (B) instead of keyboard (Esc)
					player.reload = 20;
					player.wpnReload = 20;
					break;
				case PauseMenuResult::Paused:
					break;
				case PauseMenuResult::GiveUp:
					if(mapNum)
						mapToGoTo=0;
					else
						mapToGoTo=255;
					lastKey=0;
					return LEVEL_ABORT;
					break;
				case PauseMenuResult::Quit:
					mapToGoTo=255;
					lastKey=0;
					return WORLD_QUITGAME;	// dump out altogether
					break;
				case PauseMenuResult::WarpToLooniton:
					mapToGoTo = 0;
					lastKey = 0;
					return LEVEL_ABORT;
					break;
			}

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
				((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX|| player.worldNum==WORLD_RANDOMIZER) && (player.cheatsOn&PC_HARDCORE)) ||
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
//#ifndef NDEBUG
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

void PauseGame()
{
	if (gameMode == GAMEMODE_PLAY)
	{
		InitPauseMenu();
		gameMode = GAMEMODE_MENU;
	}
}

TASK(byte) PlayALevel(byte map)
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
		CO_RETURN LEVEL_ABORT;
	}

	exitcode=LEVEL_PLAYING;
	gameMode=GAMEMODE_PLAY;
	garbageTime=0;

	PrepGuys(curMap);

	SetAreaName(&curWorld);
	if ((player.worldNum == WORLD_NORMAL || player.worldNum == WORLD_REMIX) && (player.cheatsOn & PC_HARDCORE) && player.lastSave == 255)
	{
		BumpSaveGem();
	}

	// If the player walked through walls to leave the map before the portal
	// animation finished playing, put the portal now.
	if (!player.var[VAR_PORTALOPEN])
	{
		int numVampStands = 0;
		for (int i = VAR_VAMPSTAND; i < VAR_VAMPSTAND + 8; ++i)
		{
			if (player.var[i])
			{
				++numVampStands;
			}
		}
		if (numVampStands == 8)
		{
			player.var[VAR_PORTALOPEN]=1;
			PlayerSetVar(VAR_QUESTDONE+QUEST_BUSTS,1);
			SetNoSaving(false);
		}
	}

	//clear items at archipelago checked locations
	if ((player.worldNum == WORLD_NORMAL) && ArchipelagoMode) {
		for (locationData loc : basic_locations)
		{
			if (loc.Map == curMap->name &&
				loc.MapID == mapNum && 
				locsFound.count(loc.ID) > 0) {
				curMap->map[loc.Xcoord + loc.Ycoord * curMap->width].item = ITM_NONE;
			}
		}
	}

	tl=0;
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=tl;
		garbageTime=0;
		StartClock();
		exitcode=LunaticRun(&lastTime);
		LunaticDraw();
		AWAIT gamemgl->Flip();

		if((lastKey==27 || (GetGamepadButtons()&(1<<SDL_CONTROLLER_BUTTON_START))) && gameMode==GAMEMODE_PLAY)
		{
			PauseGame();
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
	CO_RETURN exitcode;
}

TASK(byte) LunaticWorld(const char *worldName)
{
	byte result;

	if(!LoadWorld(&curWorld,worldName))
		CO_RETURN WORLD_ABORT;

	InitWorld(&curWorld,player.worldNum);
	if(player.worldNum==WORLD_SURVIVAL)
		InitSurvival();

	if(player.worldNum==WORLD_BOSSBASH)
	{
		player.levelNum=AWAIT BossMenu(gamemgl);
		if(player.levelNum==0)
		{
			SetSongRestart(0);
			FreeWorld(&curWorld);
			CO_RETURN WORLD_ABORT;
		}
		InitPlayer(INIT_GAME,WORLD_BOSSBASH,player.levelNum);
		SetSongRestart(1);
	}

	mapNum=player.levelNum;
	while(1)
	{
		result=AWAIT PlayALevel(mapNum);
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
				AWAIT EndGameTally(gamemgl);
				FreeWorld(&curWorld);
				CO_RETURN WORLD_QUITGAME;	// you LOSE! (or win in the case of bowling)
			}
			if(player.cheatsOn&PC_HARDCORE)
			{
				if(msgContent!=255)
					DeleteSave(msgContent+1);
				AWAIT EndGameTally(gamemgl);
				FreeWorld(&curWorld);
				CO_RETURN WORLD_QUITGAME;	// you LOSE!!
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
				InitPlayer(INIT_GAME,player.worldNum,0);
			}
		}
		else if(result==LEVEL_LOADING)
		{
			FreeWorld(&curWorld);
			CO_RETURN WORLD_LOAD;
		}
		else if(result==WORLD_QUITGAME)
		{
			FreeWorld(&curWorld);
			CO_RETURN WORLD_QUITGAME;
		}
		else if(result==LEVEL_WIN)
		{
			if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX || player.worldNum==WORLD_RANDOMIZER))
			{
				// won the game!  Go back to Luniton.
				AWAIT ShowVictoryAnim(0);
				AWAIT VictoryText(gamemgl);
				AWAIT Credits(gamemgl,0);
				JamulSoundPurge();
				KillSong();
				AWAIT EndGameTally(gamemgl);
				mapNum=0;
			}
			if(player.worldNum==WORLD_LOONYBALL)
			{
				// go on to the next
				mapNum++;
				if(mapNum==5)
				{
					player.levelNum=5;
					AWAIT EndGameTally(gamemgl);
					FreeWorld(&curWorld);
					CO_RETURN WORLD_QUITGAME;	// you win!
				}
			}
			if(player.worldNum==WORLD_BOSSBASH)
			{
				DBG("ReadyToTally");
				AWAIT EndGameTally(gamemgl);
				FreeWorld(&curWorld);
				CO_RETURN WORLD_QUITGAME;	// you win!
			}
		}
		if(player.worldNum==WORLD_SURVIVAL && SurvivalOver())
		{
			AWAIT EndGameTally(gamemgl);
			FreeWorld(&curWorld);
			CO_RETURN WORLD_QUITGAME;
		}
	}
	FreeWorld(&curWorld);
	CO_RETURN WORLD_ABORT;
}

TASK(void) LunaticGame(MGLDraw *mgl,byte load,byte mode)
{
	byte worldResult;
	char buff[128];

	InitPlayer(INIT_GAME,mode,0);

	while(1)
	{
		loadGame=load;
		SetNoSaving(false);
		switch(player.worldNum)
		{
			case WORLD_NORMAL:
				if (ArchipelagoMode)
				{
					worldResult = AWAIT LunaticWorld(("Archipelago/" + ArchipelagoSeed + "_" + std::to_string(ArchipelagoSlotNum) + "/ap.llw").c_str());
					break;
				}
				else
				{
					if (!loadGame)
						AWAIT Help(gamemgl);
					worldResult = AWAIT LunaticWorld("loony.llw");
					break;
				}
			case WORLD_REMIX:
				if(!loadGame)
					AWAIT Help(gamemgl);
				worldResult=AWAIT LunaticWorld("remix.llw");
				break;
			case WORLD_RANDOMIZER:
				if(!loadGame)
					AWAIT Help(gamemgl);
				LoadRandoItems();
				sprintf(buff, "randomizer/%s rando.llw", GetSeed().c_str());
				worldResult=AWAIT LunaticWorld(buff);
				break;
			case WORLD_SURVIVAL:
				AWAIT Help(gamemgl);
				worldResult=AWAIT LunaticWorld("survive.llw");
				break;
			case WORLD_SLINGSHOT:
				AWAIT Help(gamemgl);
				worldResult=AWAIT LunaticWorld("sling.llw");
				break;
			case WORLD_LOONYBALL:
				AWAIT Help(gamemgl);
				worldResult=AWAIT LunaticWorld("ball.llw");
				break;
			case WORLD_BOWLING:
				AWAIT Help(gamemgl);
				worldResult=AWAIT LunaticWorld("bowl.llw");
				break;
			case WORLD_BOSSBASH:
				AWAIT Help(gamemgl);
				worldResult=AWAIT LunaticWorld("boss.llw");
				break;
			default:
				worldResult = WORLD_ABORT;
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
