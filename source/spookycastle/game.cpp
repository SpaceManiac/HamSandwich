#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "palettes.h"

byte showStats=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
dword CDMessingTime;	// how long the CD messing with took, take it out of the time budget, because
						// it can bog the game, but it should just freeze the action
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

byte msgFromOtherModules;
byte msgContent;

word windingDown;
byte windingUp;
byte windDownReason;

void LunaticInit(MGLDraw *mgl)
{
	gamemgl=mgl;

	InitCosSin();
	InitDisplay(gamemgl);
	InitSound();
	InitMonsters();
	InitTiles(mgl);
	InitItems();
	InitInterface();
	mgl->LastKeyPressed();
	InitControls();
	InitPlayer(INIT_GAME,0,0);
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

byte InitLevel(byte map)
{
	JamulSoundPurge();	// each level, that should be good

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
	msgFromOtherModules=0;

	InitGuys(128);
	InitBullets();
	InitPlayer(INIT_LEVEL,0,map);
	InitMessage();
	NewBigMessage(curMap->name,100);
	InitParticles(512);
	lastKey=0;
	curMap->Init(&curWorld);

	windingDown=0;
	windingUp=10;
	ResetInterface();
	InitCheater();

	SetGiveUpText((byte)(map!=0));

	return 1;
}

void ExitLevel(void)
{
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

void AddGarbageTime(dword t)
{
	garbageTime+=t;
}

TASK(byte) LunaticRun(int *lastTime)
{
	dword CDtime;
	int b;

	numRunsToMakeUp=0;
	if(*lastTime>(TIME_PER_FRAME*5))
		*lastTime=TIME_PER_FRAME*5;
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

		UpdateInterface(player.life,player.hammerFlags,player.hammers,b,player.score,
					player.weapon,player.ammo,player.hamSpeed);

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
				}
			}
			else
			{
				curMap->Update(UPDATE_FADE,&curWorld);
				EditorUpdateGuys(curMap);
			}
			UpdateParticles(curMap);
			UpdateMessage();

			if(curMap->flags&MAP_SNOWING)
				MakeItSnow(curMap);

			if(windingDown)
			{
				windingDown--;
				if(!windingDown)
					CO_RETURN windDownReason;
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
					if(mapNum)
						mapToGoTo=0;
					else
						mapToGoTo=255;
					lastKey=0;
					CO_RETURN LEVEL_ABORT;
					break;
				case 3:
					mapToGoTo=255;
					lastKey=0;
					CO_RETURN WORLD_QUITGAME;	// dump out altogether
					break;
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
			mapToGoTo=msgContent;
			windingDown=30;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_WINLEVEL)
		{
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
			CDtime=timeGetTime();
			AWAIT VictoryText(gamemgl);
			AWAIT Credits(gamemgl);
			*lastTime=TIME_PER_FRAME;
			//garbageTime+=timeGetTime()-CDtime;
		}
		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}
	CDMessingTime=0;	// that's how long CD messing took
	CDMessingTime+=garbageTime;	// time wasted with such things as playing animations
	garbageTime=0;
	JamulSoundUpdate();

	CO_RETURN LEVEL_PLAYING;
}

TASK(void) LunaticDraw(void)
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
		RenderSpecialXes(gamemgl,curMap,worldNum);
		RenderMessage();
		PlayerRenderInterface(gamemgl);
		if(gameMode==GAMEMODE_MENU)
			RenderPauseMenu();
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
	if(lastKey=='s')
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

	UpdateGuys(curMap,&curWorld);	// this will force the camera into the right position
									// it also makes everybody animate by one frame, but no one will
									// ever notice
	StartClock();
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=TimeLength();
		if(lastTime>TIME_PER_FRAME*5)
			lastTime=TIME_PER_FRAME*5;
		StartClock();
		if(gameMode==GAMEMODE_PLAY)
			HandleKeyPresses();
		exitcode=AWAIT LunaticRun(&lastTime);
		//if(numRunsToMakeUp>0)
			AWAIT LunaticDraw();

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
	}

	ExitLevel();
	CO_RETURN exitcode;
}

TASK(byte) LunaticWorld(byte world,const char *worldName)
{
	byte result;

	InitPlayer(INIT_WORLD,world,0);
	if(!LoadWorld(&curWorld,worldName))
		CO_RETURN WORLD_ABORT;

	worldNum=world;
	InitWorld(&curWorld,worldNum);

	mapNum=0;
	while(1)
	{
		result=AWAIT PlayALevel(mapNum);
		if(result==LEVEL_ABORT)
		{
			PlayerResetScore();
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
			PlayerWinLevel(world,mapNum,curMapFlags&MAP_SECRET);
			mapNum=0;
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

TASK(void) LunaticGame(MGLDraw *mgl,byte load)
{
	char wrldName[32]="castle.scw";
	byte b,worldResult;

	if(!load)	// don't do this if loading a game, it was already done and the player was filled with values
		InitPlayer(INIT_GAME,0,0);

	if(load>0)	// continuing a saved game
		worldResult=WORLD_LOAD;
	else
		worldResult=0;
	while(1)
	{
		b=0;

		worldResult = AWAIT LunaticWorld(b,"castle.scw");

		if(worldResult==WORLD_QUITGAME || worldResult == WORLD_ABORT)
		{
			mgl->LastKeyPressed();	// just to clear key buffer
			break;
		}
	}
	ExitPlayer();
}

TASK(void) TrainingGame(MGLDraw *mgl)
{
	InitPlayer(INIT_GAME,0,0);
	SetCustomName("training.dlw");
	if((AWAIT LunaticWorld(5,"worlds/training.scw"))==WORLD_LOAD)
		AWAIT LunaticGame(mgl,1);
	mgl->LastKeyPressed();	// just to clear key buffer
	ExitPlayer();
}
