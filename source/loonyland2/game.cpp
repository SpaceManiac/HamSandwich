#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "options.h"
#include "quest.h"
#include "help.h"
#include "spellpick.h"
#include "shop.h"
#include "newskill.h"
#include "water.h"
#include "leveldef.h"
#include "arena.h"
#include "alchemy.h"
#include "music.h"
#include "credits.h"
#include "achieve.h"
#include "ironman.h"
#include "title.h"
#include "services.h"
#include "gallery.h"

byte showStats=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
dword garbageTime=0;
static word beatUp;

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
	beatUp=0;

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
	msgFromOtherModules=0;
	InitWater();
	ResetLevelDefs();
	picToDrop=0;
}

void LunaticExit(void)
{
	SaveOptions();
	ExitChatStuff();
	ExitWater();
	ExitItems();
	ExitSound();
	ExitDisplay();
	ExitTiles();
	ExitMonsters();
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

	if(loadGame)
	{
		InitBullets();
		InitPlayer(INIT_LEVEL,0,map);
		InitMessage();
		InitParticles(512);
		LoadGame();
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
		lastKey=0;
		CameraOnPlayer(0);
		SpecialAnytimeCheck(curMap);

		windingDown=0;
		windingUp=20;
		ResetInterface();
		InitCheater();
		if(ModifierOn(MOD_IRONMAN))
			DeleteCharacter();
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
		PlantHerbs(curMap,&curWorld);
		CameraOnPlayer(1);
		SpecialAnytimeCheck(curMap);

		windingDown=0;
		windingUp=20;
		ResetInterface();
		InitCheater();

		if(player.var[VAR_TIP+0]==0)
		{
			gameMode=GAMEMODE_CHAT;
			GetTaps();	// clear the key tap buffer
			BeginChatting(255);
			if(!player.var[VAR_MADCAP])
				TipEvent(TIP_BEGIN);
			else
				player.var[VAR_TIP+0]=1;
		}
		SetupArena(curMap);
		if(ModifierOn(MOD_IRONMAN))
			DeleteCharacter();
		else
			SaveGame();
	}
	ApplyControlSettingsMem(&player.var[VAR_CONTROLS]);
	JamulSoundVolume(player.var[VAR_SOUND]*2);
	SetMusicVolume(player.var[VAR_MUSIC]*2);
	Song(levelDef[player.levelNum].song);
	SetupWater();

	if(player.levelNum==LVL_TOYFACTORY && player.var[VAR_QUESTDONE+QUEST_FACTORY2] && !player.var[VAR_ONIONCHAT])
	{
		gameMode=GAMEMODE_CHAT;
		GetTaps();	// clear the key tap buffer
		BeginChatting(24);	// send in the onion to make an offer
	}

	if(player.levelNum==LVL_ONIONCAMP)
	{
		if(player.var[VAR_SNUGGLY]==0)
		{
			levelDef[player.levelNum].flags|=LF_TOWN;
			levelDef[player.levelNum].badguyCount=0;
		}
		else
		{
			levelDef[player.levelNum].flags&=(~LF_TOWN);
			levelDef[player.levelNum].badguyCount=20;
		}
	}
	if(player.levelNum==LVL_SNUGGLYHQ)
	{
		if(player.var[VAR_SNUGGLY]==1)
		{
			levelDef[player.levelNum].flags|=LF_TOWN;
			levelDef[player.levelNum].badguyCount=0;
		}
		else
		{
			levelDef[player.levelNum].flags&=(~LF_TOWN);
			levelDef[player.levelNum].badguyCount=20;
		}
	}
	if(levelDef[player.levelNum].flags&LF_TOWN)
		player.lastTown=player.levelNum;
	if(player.levelNum==LVL_LAKE)
		monsType[MONS_MIMIC].flags&=(~MF_INVINCIBLE);
	else
		monsType[MONS_MIMIC].flags|=MF_INVINCIBLE;

#ifdef DIRECTORS
	if(picToDrop)
	{
		if(opt.galPix[picToDrop-1]==0)
		{
			bullet_t *b;

			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
			if(b)
			{
				b->anim=IT_PICTURE;
				b->target=picToDrop;
			}
		}
	}
	picToDrop=0;
#endif
	return 1;
}

void ExitLevel(void)
{
	// exit everything
	ExitGuys();
	ExitBullets();
	ExitParticles();
	ApplyControlSettings();

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

void EnterShop(byte shop)
{
	player.reload=20;
	gameMode=GAMEMODE_SHOP;
	InitShop(shop);
}

void EnterStatusScreen(void)
{
	player.reload=20;
	gameMode=GAMEMODE_MENU;
}

void EnterSpellPick(void)
{
	player.reload=5;
	gameMode=GAMEMODE_SPELL;
	InitSpellPick();
}

void EnterNewSkill(byte sk)
{
	player.reload=5;
	gameMode=GAMEMODE_NEWSKILL;
	InitNewSkill(sk);
}

void EnterAchieve(byte ach,byte mod1,byte mod2)
{
	player.reload=5;
	gameMode=GAMEMODE_ACHIEVE;
	InitAchieve(ach,mod1,mod2);
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
	gameMode=GAMEMODE_PLAY;
}

void BeatLoonyUp(void)
{
	beatUp=30*5;
	MakeNormalSound(SND_BEATENUP);
	player.destx=25;
	player.desty=2;
	PlayerSetVar(VAR_PRISONBREAK,250);
	SendMessageToGame(MSG_GOTOMAP,LVL_ONIONCAMP);
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

	if(ModifierOn(MOD_RUSH))
		*lastTime=(*lastTime)*2;

	while(*lastTime>=TIME_PER_FRAME)
	{
		player.playClock++;
		if(!gamemgl->Process())
		{
			mapToGoTo=255;
			return LEVEL_ABORT;
		}

		if(UpdateHelp(gamemgl))
		{
		}
		else if(gameMode==GAMEMODE_PLAY)
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
					RandomRespawnMap(curMap);
					curMap->Update(UPDATE_GAME,&curWorld);
					UpdateGuys(curMap,&curWorld);
					UpdateBullets(curMap,&curWorld);
					SpecialAnytimeCheck(curMap);
					UpdateItems();
					UpdateArena();
					AddCaravans();
					if(player.clockRunning)
						player.timeLimit++;
				}
			}
			else
			{
				if(beatUp)
				{
					beatUp--;
					windingDown++;
				}
				curMap->Update(UPDATE_FADE,&curWorld);
				EditorUpdateGuys(curMap);
			}
			AchieveCheck();
			UpdateParticles(curMap);
			UpdateMessage();
			UpdateShopInventories();

			if(curMap->flags&MAP_SNOWING)
				MakeItSnow(curMap);

			if(windingDown)
			{
				windingDown--;
				if(!windingDown)
				{
					return windDownReason;
				}
			}
		}
		else if(gameMode==GAMEMODE_MENU)
		{
			switch(UpdatePauseMenu(gamemgl))
			{
				case PAUSE_RESUME:
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					break;
				case PAUSE_QUIT:
					mapToGoTo=255;
					lastKey=0;
					return WORLD_QUITGAME;	// dump out altogether
					break;
			}
			worldNum=player.worldNum;
		}
		else if(gameMode==GAMEMODE_SPELL)
		{
			if(UpdateSpellPick(gamemgl)==0)
			{
				lastKey=0;
				gameMode=GAMEMODE_PLAY;
			}
		}
		else if(gameMode==GAMEMODE_NEWSKILL)
		{
			if(UpdateNewSkill(gamemgl)==0)
			{
				lastKey=0;
				gameMode=GAMEMODE_PLAY;
			}
		}
		else if(gameMode==GAMEMODE_ACHIEVE)
		{
			if(UpdateAchieve(gamemgl)==0)
			{
				lastKey=0;
				gameMode=GAMEMODE_PLAY;
			}
		}
		else if(gameMode==GAMEMODE_SHOP)
		{
			if(UpdateShop(gamemgl)==0)
			{
				lastKey=0;
				gameMode=GAMEMODE_PLAY;
			}
		}
		else if(gameMode==GAMEMODE_PIC)	// gamemode_pic
		{
			if(GetTaps()&(CONTROL_B1|CONTROL_B2))
			{
				gameMode=GAMEMODE_PLAY;
				// restore the palette
				gamemgl->LoadBMP(TITLEBMP);
			}
		}
		else if(gameMode==GAMEMODE_CHAT)
		{
			if(windingUp)
			{
				curMap->Update(UPDATE_FADEIN,&curWorld);
				EditorUpdateGuys(curMap);
				windingUp--;
			}
			else
			{
				curMap->Update(UPDATE_FADEIN,&curWorld);
				EditorUpdateGuys(curMap);
				UpdateChat(0);
			}
		}

		if(msgFromOtherModules==MSG_GOTOMAP)
		{
			mapToGoTo=msgContent;
			if(mapToGoTo==0)
				TipEvent(TIP_GO2);
			else if(mapToGoTo==LVL_PEAK)
				TipEvent(TIP_ENDING);
			else if(mapToGoTo!=LVL_MICKEYCABIN && mapToGoTo!=LVL_WOODYCABIN && mapToGoTo!=LVL_AXESRUS && mapToGoTo!=LVL_GURUCABIN)
				TipEvent(TIP_GO);

			if(!(levelDef[player.levelNum].flags&LF_ARENA))
				MakeNormalSound(SND_ENTERMAP);
			windingDown=40;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
			PlayerClearTempVars();
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
			player.boredom=0;
		}
		else if(msgFromOtherModules==MSG_RESET)
		{
			TipEvent(TIP_DIE);
			NewBigMessage("You were defeated!",30,2);
			windingDown=30;
			windDownReason=LEVEL_RESET;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_LOADGAME)
		{
			NewBigMessage("Loading Game",30,0);
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
		RenderInterface(gamemgl);
		RenderMessage();
		if(gameMode==GAMEMODE_MENU)
			RenderPauseMenu(gamemgl);
		if(gameMode==GAMEMODE_SPELL)
			RenderSpellPick(gamemgl);
		if(gameMode==GAMEMODE_SHOP)
			RenderShop(gamemgl);
		if(gameMode==GAMEMODE_CHAT)
			RenderChat(gamemgl);
		if(gameMode==GAMEMODE_NEWSKILL)
			RenderNewSkill(gamemgl);
		if(gameMode==GAMEMODE_ACHIEVE)
			RenderAchieve(gamemgl);
	}
	else
	{
		// nothing to do
	}

	RenderHelp(gamemgl);

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

	exitcode=LEVEL_PLAYING;
	gameMode=GAMEMODE_PLAY;
	garbageTime=0;

	if(!InitLevel(map))
	{
		mapToGoTo=255;
		return LEVEL_ABORT;
	}

	PrepGuys(curMap);

	tl=0;
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=tl;
		garbageTime=0;
		StartClock();
		exitcode=LunaticRun(&lastTime);
		LunaticDraw();

		if(lastKey==27 && gameMode==GAMEMODE_PLAY && !windingDown && !windingUp)
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
	char wName[64];

	if(player.addonName[0]=='\0')
	{
		ResetLevelDefs();
		strcpy(wName,"winter.llw");
	}
	else
	{
		sprintf(wName,"addons/%s.llw",player.addonName);
		LoadLevelDefs(player.addonName);
	}

	if(!loadGame)
	{
		if(!LoadWorld(&curWorld,wName))
			return WORLD_ABORT;
	}

 	worldNum=world;

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
			// return to last town
			if(ModifierOn(MOD_IRONMAN))
			{
				IronmanScreen(gamemgl);
				Credits(gamemgl,0);
				break;
			}
			else
			{
				mapNum=player.lastTown;
				PlayerBackToLastTown(&mapNum);
				player.hearts=player.maxHearts;
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
			// won the game!  Go back to Woody's cabin
			// movie goes here
			player.var[VAR_COMMENTARY]=0;
#ifdef DIRECTORS
			if(opt.modifier[29]==0)
			{
				opt.modifier[29]=1;	// secret modifier slot used to house whether you can hear commentary or not
				player.var[VAR_COMMENTARY]=1;
			}
#endif
			Song(SONG_WINTER);
			FLI_play("graphics/ending.flc",0,10,gamemgl);
			if(ModifierOn(MOD_IRONMAN) && player.var[VAR_MADCAP])	// just won Madcap mode!
			{
				IronmanScreen(gamemgl);
				Credits(gamemgl,1);
				break;
			}
			else
			{
				Credits(gamemgl,1);
				JamulSoundPurge();
				MadcapPlayer();
				mapNum=5;
			}
		}
	}
	FreeWorld(&curWorld);
	return WORLD_ABORT;
}

void LunaticGame(MGLDraw *mgl,byte load,byte mode)
{
	byte worldResult;

	InitPlayer(INIT_GAME,mode,5);	// start on level 5 - woody's cabin
	MadcapArena();
	while(1)
	{
		loadGame=load;
		worldResult=LunaticWorld(0,"winter.llw");
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
