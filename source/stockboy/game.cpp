#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "options.h"
#include "help.h"
#include "fireworks.h"
#include "training.h"
#include "editor.h"
#include "score.h"
#include "blowout.h"
#include "pestcontrol.h"
#include "tally.h"
#include "stockroom.h"
#include "goodie.h"
#include "empmonth.h"
#include "addon.h"
#include "music.h"
#include "parallel.h"
#include "clear2.h"

byte showStats=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
dword garbageTime=0;
byte canWind;
static byte showTally;

int   visFrms;
float frmRate;
word numRunsToMakeUp;

char lastKey=0;

MGLDraw *gamemgl;
Map		*curMap;
byte	gameMode=GAMEMODE_PLAY;
byte	gameType;
byte	mapToGoTo;
byte	worldNum;
byte    mapNum;
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

	mgl->LastKeyPressed();
	MGL_srand(timeGetTime());
	InitControls();
	InitPlayer(INIT_GAME,0,0);
	msgFromOtherModules=0;
	testingLevel=0;
	Music_Init();
	// TODO mgl->SetColorblind(profile.colorblind);
	mgl->RealizePalette();
}

void SetGameMGL(MGLDraw *mgl)
{
	gamemgl=mgl;
}

void LunaticExit(void)
{
	Music_Exit();
	JamulSoundPurge();
	ExitItems();
	ExitSound();
	ExitDisplay();
	ExitTiles();
	ExitMonsters();
	ExitPlayer();
	ExitInterface();
	FreeHiScoreSprites();
}

byte GameType(void)
{
	return gameType;
}

byte TestingLevel(void)
{
	return testingLevel;
}

TASK(void) TestLevel(byte map)
{
	gameType=GAME_STOCKROOM;
	testingLevel=1;
	player.levelNum=map;
	memcpy(&curWorld,EditorGetWorld(),sizeof(world_t));
	AWAIT PlayALevel(map);
	Music_Stop();
	ResetClock(0);
	testingLevel=0;
}

byte InitLevel(byte map)
{
	int i,sng;

	showTally=0;
	LoadOptions();
	JamulSoundPurge();	// each level, that should be good

	i=0;
	while(i++<300)
	{
		sng=SONG_PLAY+Random(5);
		if(profile.songChoice[sng][0]!='\0' && Music_Load(profile.songChoice[sng]))
		{
			player.songNum=sng;
			Music_Play();
			break;
		}
	}

	if(curWorld.numMaps<=map)
		return 0;	// can't go to illegal map

	// make a copy of the map to be played
	curMap=new Map(curWorld.map[map]);

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
	CameraOnPlayer();

	windingDown=0;
	windingUp=30;
	canWind=0;
	ResetInterface();
	InitCheater();
	Score_Init();

	return 1;
}

void ExitLevel(void)
{
	// exit everything
	ExitGuys();
	ExitBullets();
	ExitParticles();
	ShutoffGoodie();
	delete curMap;
	curMap=NULL;
	PurgeMonsterSprites();
	SaveOptions();
}

void SetGameIdle(byte b)
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
		//WaitMessage();
		if(!gamemgl->Process())
			break;
	}
	end=timeGetTime();
	AddGarbageTime(end-start);
	player.boredom=0;
	ResetClock(0);
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

byte WindingUp(void)
{
	return (windingUp);
}

void StopWindingUp(void)
{
	windingUp=0;
}

void HandleKeyPresses(void);

byte LunaticRun(int *lastTime)
{
	static byte flip=0;

	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;

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
					if(windingUp!=5)
					{
						curMap->Update(UPDATE_FADEIN,&curWorld);
						EditorUpdateGuys(curMap);
					}
					if(windingUp>5 || canWind)
						windingUp--;
					if(windingUp<6 && ((GetControls()|GetArrows()) || gamemgl->MouseDown()))
						canWind=1;
				}
				else
				{
					curMap->Update(UPDATE_GAME,&curWorld);
					UpdateGuys(curMap,&curWorld);
					UpdateBullets(curMap,&curWorld);
					UpdateItems();
					Score_Update();
					if(GoalIsMet(&curMap->goal,curMap))
					{
						PlayerSoundEffect(SE_WIN);
						windingDown=30;
						windDownReason=LEVEL_WIN;
						NewBigMessage("You did it!",30);
						if(gameType!=GAME_TRAINING)
						{
							showTally=1;
							if(gameType==GAME_STOCKROOM || gameType==GAME_ADDON)
								SetTallyMsg("Victory!!",1);
							else if(gameType==GAME_PARALLEL)
								SetTallyMsg("!!yrotciV",1);
							else
								SetTallyMsg("Game Over",0);
						}
					}
					if(GoalIsMet(&curMap->antigoal,curMap))
					{
						windingDown=30;
						windDownReason=LEVEL_RESET;
						NewBigMessage("You blew it bigtime!",30);
					}
					if(gameType==GAME_BLOWOUT)
						UpdateBlowout();
					if(gameType==GAME_CLEARANCE)
						UpdateClear2();
					if(gameType==GAME_PESTCONTROL)
						UpdatePestControl();
					if(gameType==GAME_STOCKROOM || gameType==GAME_PARALLEL || gameType==GAME_ADDON)
						UpdateGoodie();
				}
			}
			else
			{
				curMap->Update(UPDATE_FADE,&curWorld);
				EditorUpdateGuys(curMap);
			}

			UpdateInterface(0);
			UpdateParticles(curMap);
			UpdateMessage();

			if(windingDown)
			{
				if(windingDown==30 && showTally)
					SetupTally(curMap);
				if(gameType==GAME_BLOWOUT)
					UpdateBlowout();
				if(Score_Update() && windingDown<5)
					windingDown++;
				windingDown--;
				if(windingDown==1 && showTally && UpdateTally())
					windingDown++;
				if(!windingDown)
				{
					if(testingLevel)
					{
						mapToGoTo=255;
						return LEVEL_ABORT;
					}
					return windDownReason;
				}
			}
		}
		else if(gameMode==GAMEMODE_MENU)
		{
			switch(UpdatePauseMenu(gamemgl))
			{
				case PAUSE_RETURN:
					ExitPauseMenu();
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					break;
				case PAUSE_RETRY:
					ExitPauseMenu();
					mapToGoTo=mapNum;
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return LEVEL_RESET;
					break;
				case PAUSE_EXITGAME:
					ExitPauseMenu();
					mapToGoTo=255;
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return WORLD_QUITGAME;	// dump out altogether
					break;
				case PAUSE_PAUSING:
					// do nothing, it will keep pausing
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

		if(msgFromOtherModules==MSG_GOTOMAP)
		{
			mapToGoTo=msgContent;
			windingDown=40;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_WINLEVEL)
		{
			PlayerSoundEffect(SE_WIN);
			mapToGoTo=0;
			if(msgContent==1)
				windingDown=1;
			else
				windingDown=40;
			windDownReason=LEVEL_WIN;
			if(gameType!=GAME_TRAINING)
			{
				showTally=1;
				if(gameType==GAME_STOCKROOM || gameType==GAME_ADDON)
					SetTallyMsg("Victory!!",1);
				else if(gameType==GAME_PARALLEL)
					SetTallyMsg("!!yrotciV",1);
				else
					SetTallyMsg("Game Over",0);
			}
			msgFromOtherModules=MSG_NONE;
			player.boredom=0;
		}
		else if(msgFromOtherModules==MSG_RESET)
		{
			if(windingDown==0)
			{
				if(gameType!=GAME_CLEARANCE)
				{
					if(curMap->timer==0 || player.clock<curMap->timer)
						NewBigMessage("You got blown up!",30);
					else
						NewBigMessage("Time ran out!",30);
				}
				windingDown=30;
				windDownReason=LEVEL_RESET;
				msgFromOtherModules=MSG_NONE;
				if(gameType==GAME_BLOWOUT ||
					gameType==GAME_PESTCONTROL)
				{
					showTally=1;
					SetTallyMsg("Game Over",0);
				}
				if(gameType==GAME_CLEARANCE)
				{
					showTally=1;
					SetTallyMsg("Inventory Cleared!",1);
				}

				if(testingLevel)
					showTally=0;
			}
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
		Music_Update();
	}
	JamulSoundUpdate();
	Music_Update();

	return LEVEL_PLAYING;
}

TASK(void) LunaticDraw(void)
{
	char s[32];
	dword d;

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	// add all the sprites to the list
	if(gameMode!=GAMEMODE_PIC)
	{
		RenderGuys(1);
		RenderBullets();
		RenderParticles();
		if(gameType==GAME_BLOWOUT)
			RenderBlowout();
		RenderItAll(&curWorld,curMap,MAP_SHOWLIGHTS|MAP_SHOWITEMS|MAP_SHOWWALLS);
		RenderFireworks(gamemgl);
		if(gameType==GAME_STOCKROOM || gameType==GAME_PARALLEL || gameType==GAME_ADDON)
			RenderGoodie(gamemgl);
		RenderMessage();
		RenderInterface(gamemgl);
		Score_Render();
		if(gameMode==GAMEMODE_MENU)
		{
			RenderPauseMenu(gamemgl);
			if(gameType==GAME_TRAINING || gameType==GAME_STOCKROOM || gameType==GAME_PARALLEL ||
				gameType==GAME_ADDON)
				PrintGoal(curMap->goal,curMap->antigoal,curMap->timer);
		}
		if(windingUp)
		{
			if(gameType==GAME_STOCKROOM && !testingLevel)
			{
				CenterPrintOutline(320,70,NameLevel(),0,2);
			}
			else if(gameType==GAME_PARALLEL)
			{
				CenterPrintOutline(320,70,RvsNameLevel(),0,2);
			}
			if(gameType!=GAME_TRAINING)
				CenterPrintOutline(320,150,curWorld.desc,0,2);

			if(gameType!=GAME_PARALLEL)
				CenterPrintOutline(320,100,curMap->name,0,2);
			else
				RvsCenterPrintOutline(320,100,curMap->name,0,2);
			if(gameType==GAME_TRAINING)
				RenderHelpText(player.levelNum+player.worldNum*5);
			if(gameType==GAME_TRAINING || gameType==GAME_STOCKROOM || gameType==GAME_PARALLEL ||
				gameType==GAME_ADDON)
				PrintGoal(curMap->goal,curMap->antigoal,curMap->timer);
		}
		else if(windingDown==2 && showTally)
		{
			RenderTally(gamemgl);
		}
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

	AWAIT gamemgl->Flip();

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

TASK(byte) PlayALevel(byte map)
{
	int lastTime=1;
	byte exitcode=0;
	dword tl;

	if(!InitLevel(map))
	{
		mapToGoTo=255;
		CO_RETURN LEVEL_ABORT;
	}

	switch(gameType)
	{
		case GAME_BLOWOUT:
			InitBlowout(curMap);
			break;
		case GAME_CLEARANCE:
			InitClear2(curMap);
			break;
		case GAME_PESTCONTROL:
			InitPestControl(curMap);
			break;
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
		AWAIT LunaticDraw();

		if(lastKey==27 && gameMode==GAMEMODE_PLAY && !windingUp && !windingDown)
		{
			if(!testingLevel)
			{
				InitPauseMenu();
				gameMode=GAMEMODE_MENU;
				gamemgl->RealizePalette();
			}
			else
			{
				exitcode=LEVEL_ABORT;
				mapToGoTo=255;
			}
		}
		else if(lastKey==27)
			lastKey=0;

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
	ExitTally();
	ExitLevel();
	CO_RETURN exitcode;
}

TASK(byte) LunaticWorld(byte world,const char *worldName)
{
	byte result;

	if(!LoadWorld(&curWorld,worldName,gamemgl))
		CO_RETURN WORLD_ABORT;

	worldNum=world;
	InitWorld(&curWorld,worldNum);

	if(gameType==GAME_BLOWOUT || gameType==GAME_PESTCONTROL)
		player.levelNum=world;

	mapNum=player.levelNum;
	player.worldNum=worldNum;

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
			// just go again!
			switch(gameType)
			{
				case GAME_BLOWOUT:
				case GAME_CLEARANCE:
				case GAME_PESTCONTROL:
					FreeWorld(&curWorld);
					CO_RETURN WORLD_MENUAGAIN;
					break;
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
			if(gameType==GAME_TRAINING)
				mapNum++;
			if(gameType==GAME_STOCKROOM)
			{
				// earn employee of the month
				if(!profile.empMonth[player.worldNum] &&
				   profile.starGot[GAME_STOCKROOM][player.worldNum*4]==4 &&
				   profile.starGot[GAME_STOCKROOM][player.worldNum*4+1]==4 &&
				   profile.starGot[GAME_STOCKROOM][player.worldNum*4+2]==4 &&
				   profile.starGot[GAME_STOCKROOM][player.worldNum*4+3]==4)
					AWAIT EmpMonth(player.worldNum,gamemgl);
				FreeWorld(&curWorld);
				CO_RETURN WORLD_WIN;
			}
			if(gameType==GAME_PARALLEL)
			{
				// earn employee of the month
				if(!profile.empMonth2[player.worldNum] &&
				   profile.starGot[GAME_PARALLEL][player.worldNum*4]==4 &&
				   profile.starGot[GAME_PARALLEL][player.worldNum*4+1]==4 &&
				   profile.starGot[GAME_PARALLEL][player.worldNum*4+2]==4 &&
				   profile.starGot[GAME_PARALLEL][player.worldNum*4+3]==4)
					AWAIT EmpMonth(player.worldNum,gamemgl);
				FreeWorld(&curWorld);
				CO_RETURN WORLD_WIN;
			}
			if(gameType==GAME_TRAINING && profile.training[player.worldNum]<player.levelNum+1)
			{
				profile.training[player.worldNum]=player.levelNum+1;
				SaveProfile(opt.curProfile);
			}
			if(mapNum>=curWorld.numMaps)
			{
				FreeWorld(&curWorld);
				CO_RETURN WORLD_WIN;
			}
			if(gameType==GAME_ADDON)
			{
				FreeWorld(&curWorld);
				CO_RETURN WORLD_WIN;
			}
		}
	}
	FreeWorld(&curWorld);
	CO_RETURN WORLD_ABORT;
}

TASK(void) LunaticGame(MGLDraw *mgl,byte load,byte mode)
{
	byte worldResult;

	InitPlayer(INIT_GAME,mode,0);

	while(1)
	{
		loadGame=load;

		gameType=mode;
		switch(mode)
		{
			case GAME_ADDON:
				worldResult=AWAIT AddOnMenu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
			case GAME_TRAINING:
				worldResult=AWAIT TrainingMenu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
			case GAME_BLOWOUT:
				worldResult=AWAIT BlowoutMenu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
			case GAME_CLEARANCE:
				worldResult=AWAIT Clear2Menu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
			case GAME_PESTCONTROL:
				worldResult=AWAIT PestControlMenu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
			case GAME_STOCKROOM:
				worldResult=AWAIT StockroomMenu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
			case GAME_PARALLEL:
				worldResult=AWAIT ParallelMenu(mgl);
				if(worldResult!=WORLD_ABORT)
					worldResult=WORLD_PLAYING;
				break;
		}
		if(worldResult==WORLD_QUITGAME || worldResult==WORLD_WIN)
		{
			mgl->LastKeyPressed();	// just to clear key buffer
			break;
		}
		if(worldResult==WORLD_ABORT)
			break;
	}
	ExitPlayer();
	// TODO GetDisplayMGL()->SetReversePal(0);
	GetDisplayMGL()->RealizePalette();
}
