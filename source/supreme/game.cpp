#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "rage.h"
#include "special.h"
#include "progress.h"
#include "editor.h"
#include "hiscore.h"
#include "tally.h"
#include "scanner.h"
#include "shop.h"
#include "gallery.h"
#include "goal.h"
#include "config.h"
#include "log.h"
#include "water.h"
#include "palettes.h"
#include "appdata.h"
#include "tile.h"

byte showStats=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
int   visFrms;
float frmRate;
word numRunsToMakeUp;

char lastKey=0;
byte waterFlip=0;
static char lastLevelName[32];
byte shopping,tutorial,verified;
byte doShop;

MGLDraw *gamemgl;
Map *curMap;
byte gameMode=GAMEMODE_PLAY;
byte mapToGoTo;
byte worldNum;
byte mapNum;
world_t curWorld;

byte msgFromOtherModules=0;
byte msgContent;

word windingDown;
byte windingUp;
byte windDownReason;
static byte idleGame=0,pictureNoKey;

void LunaticInit(MGLDraw *mgl)
{
	gamemgl=mgl;
	shopping=0;
	tutorial=0;
	doShop=0;
	verified=0;

	InitCosSin();
	InitDisplay(gamemgl);
	InitSound();
	InitMonsters();
	InitTiles(mgl);
	InitItems();
	InitInterface();
	InitProfile();
	mgl->ClearKeys();
	SeedRNG();
	InitControls();
	msgFromOtherModules=0;
	JamulSoundVolume(profile.sound);
	SetMusicVolume(profile.music);
	InitHiScores();
}

void LunaticExit(void)
{
	ExitHiScores();
	ExitItems();
	ExitSound();
	ExitDisplay();
	ExitTiles();
	ExitMonsters();
	ExitPlayer();
	ExitInterface();
	FreeProfile();
	StopSong();
}

byte VerifyLevel(Map *map)
{
#ifdef _DEBUG
	return 1;
#else
	dword chk,cmp;
	FILE *f;

	chk=ChecksumMap(map);

	f=AssetOpen("worlds/levels.dat","rb");
	if(!f)
		return 0;

	while(fread(&cmp,sizeof(dword),1,f))
	{
		if(cmp==chk)
		{
			fclose(f);
			return 1;
		}
	}
	fclose(f);
	return 0;
#endif
}

byte InitLevel(byte map)
{
	PrintToLog("InitLevel",map);

	JamulSoundPurge();	// each level, that should be good

	if(curWorld.numMaps<=map)
		return 0;	// can't go to illegal map

	// make a copy of the map to be played
	curMap=new Map(curWorld.map[map]);

	verified=VerifyLevel(curMap);

	PrintToLog(curMap->name,0);

	gameStartTime=timeGetTime();
	tickerTime=timeGetTime();
	updFrameCount=0;
	visFrameCount=0;
	numRunsToMakeUp=0;
	frmRate=30.0f;
	visFrms=0;
	if(msgFromOtherModules!=MSG_NEWFEATURE)
		msgFromOtherModules=0;

	InitGuys(config.numGuys+1);
	InitBullets();
	InitPlayer(map,player.worldName);
	InitMessage();
	if(curMap->name[0]!='`')
		NewBigMessage(curMap->name,100);
	InitParticles(config.numParticles);
	lastKey=0;
	curMap->Init(&curWorld);

	windingDown=0;
	windingUp=30;
	ResetInterface();
	InitCheater();

	GetSpecialsFromMap(curMap->special);
	InitSpecialsForPlay();
	
	PlaySong(curMap->song);

	ScoreEvent(SE_INIT,curMap->width*curMap->height);

	strcpy(lastLevelName,curMap->name);

	if(shopping)
	{
		RandomizeLockers();
		SetupShops(curMap);
		InitGallery(curMap);
	}

	LocateKeychains(&curWorld);
	if(curMap->numCandles==0)
		player.levelProg->flags|=LF_CANDLES;

	if(curMap->flags&MAP_SECRET)
		CompleteGoal(70);
	SetupWater();
	RestoreGameplayGfx();
	return 1;
}

void ExitLevel(void)
{
	PrintToLog("ExitLevel",0);

	// exit everything
	ExitPauseMenu();
	ExitGuys();
	ExitBullets();
	ExitParticles();

	player.vehicle=0;
	player.weapon=0;
	player.hammers=0;

	delete curMap;
	PurgeMonsterSprites();
	gamemgl->RealizePalette();
	if(shopping)
		ExitGallery();
}

void SetGameIdle(bool b)
{
	idleGame = b;
	if (b)
		PauseGame();
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
	player.boredom=0;
	return;
}

void RestoreGameplayGfx(void)
{
	
	if(profile.progress.purchase[modeShopNum[MODE_VIRTUAL]]&SIF_ACTIVE && !shopping)
	{
		RedPalette(gamemgl);
	}
	else if(curMap)
	{
		if(curMap->flags&MAP_WAVY)
			DumbSidePalette(gamemgl);
		else if(curMap->flags&MAP_LAVA)
			LavaPalette(gamemgl);
		else if(curMap->flags&MAP_UNDERWATER)
			WaterPalette(gamemgl);
	}

	if(profile.progress.purchase[modeShopNum[MODE_TEENY]]&SIF_ACTIVE && !shopping)
	{
		gamemgl->ClearScreen();
		GetDisplayMGL()->LoadBMP("graphics/gamepal.bmp");
		gamemgl->Flip();
	}
	if(profile.progress.purchase[modeShopNum[MODE_RASTER]]&SIF_ACTIVE && !shopping)
		gamemgl->ClearScreen();
}

void EnterPictureDisplay(void)
{
	gameMode=GAMEMODE_PIC;
	pictureNoKey=30;
	GetTaps();	// clear the key tap buffer
}

byte GetGameMode(void)
{
	return gameMode;
}

void EnterRage(void)
{
	MakeNormalSound(SND_RAGE);
	gameMode=GAMEMODE_RAGE;
	player.rageClock=30;
	SetRageFace();
}

byte LunaticRun(int *lastTime)
{
	byte frmsToRun;

	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*2)
		*lastTime=TIME_PER_FRAME*2;

	frmsToRun=(*lastTime/TIME_PER_FRAME);

	*lastTime-=frmsToRun*TIME_PER_FRAME;

	if(gameMode==GAMEMODE_PLAY && (profile.progress.purchase[modeShopNum[MODE_MANIC]]&SIF_ACTIVE))
		frmsToRun*=2;	// run twice as many frames

	while(frmsToRun--)//(*lastTime>=TIME_PER_FRAME)
	{
		if(!gamemgl->Process())
		{
			mapToGoTo=255;
			return LEVEL_ABORT;
		}

		if(gameMode==GAMEMODE_PLAY)
		{
			if(!editing && !player.cheated)
			{
				profile.progress.totalTime++;
				if((curMap->flags&(MAP_UNDERWATER|MAP_LAVA)) && player.weapon!=WPN_MINISUB)
					profile.progress.underwaterTime++;
			}

			UpdateInterface(curMap);
			UpdateUnpaused();
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
					CheckSpecials(curMap);
					UpdateGuys(curMap,&curWorld);
					UpdateItems();
					UpdateBullets(curMap,&curWorld);
				}
			}
			else
			{
				curMap->Update(UPDATE_FADE,&curWorld);
				EditorUpdateGuys(curMap);
			}
			UpdateParticles(curMap);
			UpdateMessage();

			waterFlip=1-waterFlip;
			if(waterFlip)
				UpdateWater();
			
			if(curMap->flags&MAP_SNOWING)
				MakeItSnow(curMap);
			if(curMap->flags&MAP_RAIN)
			{
				MakeItRain(curMap);
				MakeItRain(curMap);
				MakeItRain(curMap);
			}

			if(windingDown)
			{
				windingDown--;
				if(!windingDown)
				{
					PrintToLog("Wound Down",0);
					return windDownReason;
				}
			}
		}
		else if(gameMode==GAMEMODE_MENU)
		{
			//if (!Mix_PausedMusic())
			//	Mix_PauseMusic();
			switch(UpdatePauseMenu(gamemgl))
			{
				case PAUSE_PAUSED:
					break;
				case PAUSE_CONTINUE:
					//Mix_ResumeMusic();
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					break;
				case PAUSE_GIVEUP:
					SetPlayerStart(-1,-1);
					if(mapNum)
						mapToGoTo=0;
					else
						mapToGoTo=255;
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return LEVEL_ABORT;
					break;
				case PAUSE_WORLDSEL:
					mapToGoTo=255;
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return WORLD_ABORT;	// dump out altogether
					break;
				case PAUSE_RETRY:
					mapToGoTo=player.levelNum;	// repeat this level
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return LEVEL_ABORT;
					break;
				case PAUSE_EXIT:
					mapToGoTo=255;
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return WORLD_QUITGAME;
					break;
				case PAUSE_SHOP:
					mapToGoTo=255;
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					return WORLD_SHOP;
					break;
			}
		}
		else if(gameMode==GAMEMODE_PIC)	// gamemode_pic
		{
			if(pictureNoKey)
				pictureNoKey--;
			else
			{
				if(GetTaps()&(CONTROL_B1|CONTROL_B2))
				{
					gameMode=GAMEMODE_PLAY;
					// restore the palette
					gamemgl->LoadBMP("graphics/title.bmp");
					RestoreGameplayGfx();
				}
			}
		}
		else if(gameMode==GAMEMODE_SCAN)
		{
			if(!UpdateScan(gamemgl))
			{
				gameMode=GAMEMODE_PLAY;
				RestoreGameplayGfx();
			}
		}
		else if(gameMode==GAMEMODE_CARD)
		{
			if(!UpdateCard(gamemgl))
			{
				gameMode=GAMEMODE_PLAY;
				RestoreGameplayGfx();
			}
		}
		else if(gameMode==GAMEMODE_SHOP)
		{
			if(!UpdateShopping(gamemgl))
			{
				gameMode=GAMEMODE_PLAY;
				RestoreGameplayGfx();
			}
		}
		else // gamemode_rage
		{
			UpdateRage(gamemgl);
			if(player.rageClock)
			{
				player.rageClock--;
				if(goodguy)
					goodguy->facing=(goodguy->facing+1)&7;
			}
			else
			{
				gameMode=GAMEMODE_PLAY;
				StartRaging();
			}
		}

		if(msgFromOtherModules==MSG_GOTOMAP)
		{
			GoalTimeDist();
			mapToGoTo=msgContent;
			windingDown=30;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_GOTOMAP2)
		{
			GoalTimeDist();
			mapToGoTo=msgContent;
			windingDown=30;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_WINLEVEL)
		{
			PrintToLog("Level Win!",0);
			GoalTimeDist();
			PrintToLog("GoalTimeDist Done",0);
			mapToGoTo=msgContent;
			windingDown=40;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
			player.boredom=0;
		}
		else if(msgFromOtherModules==MSG_RESET)
		{
			GoalTimeDist();
			NewBigMessage("Try Again!",30);
			windingDown=30;
			windDownReason=LEVEL_RESET;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_SCANMONSTER)
		{
			msgFromOtherModules=MSG_NONE;
			gameMode=GAMEMODE_SCAN;
		}
		else if(msgFromOtherModules==MSG_MONSTERCARD)
		{
			msgFromOtherModules=MSG_NONE;
			gameMode=GAMEMODE_CARD;
		}
		else if(msgFromOtherModules==MSG_SHOPNOW)
		{
			GoalTimeDist();
			msgFromOtherModules=MSG_NONE;
			gameMode=GAMEMODE_SHOP;
		}
		else if(msgFromOtherModules==MSG_WINGAME)
		{
			GoalTimeDist();
			mapToGoTo=0;
			windingDown=1;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
			VictoryText(gamemgl);
			Credits(gamemgl);
			player.boredom=0;
		}
		//*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}

	return LEVEL_PLAYING;
}

byte choseColor;
void LunaticDraw(void)
{
	char s[128];
	dword d;

	// add all the sprites to the list
	if(gameMode!=GAMEMODE_PIC && gameMode!=GAMEMODE_SCAN && gameMode!=GAMEMODE_CARD)
	{
		RenderGuys(1);
		RenderBullets();
		RenderParticles();
		RenderItAll(&curWorld,curMap,MAP_SHOWLIGHTS|MAP_SHOWPICKUPS|MAP_SHOWOTHERITEMS|MAP_SHOWWALLS);
		RenderSpecialXes(curMap);
		RenderMessage();
		PlayerRenderInterface(gamemgl);
		if(gameMode==GAMEMODE_MENU)
			RenderPauseMenu();
		else
			RenderUnpaused();
		if(gameMode==GAMEMODE_RAGE)
			ShowRage(gamemgl);
		else if(gameMode==GAMEMODE_SHOP)
		{
			RenderShopping(gamemgl);
		}
	}
	else if(gameMode==GAMEMODE_PIC)
	{
		// nothing to do
	}
	else if(gameMode==GAMEMODE_SCAN)
	{
		RenderScan(gamemgl);
	}
	else if(gameMode==GAMEMODE_CARD)
	{
		RenderCard(gamemgl);
	}
	if(showStats)
	{
		sprintf(s,"Debug Menu - F3 to close");
		PrintGlow(5,30,s,8,2);

		sprintf(s,"VFPS %02.2f",((float)visFrameCount/(float)((timeGetTime()-gameStartTime)/1000)));
		PrintGlow(5,50,s,8,2);
		sprintf(s,"GFPS %02.2f",((float)updFrameCount/(float)((timeGetTime()-gameStartTime)/1000)));
		PrintGlow(120,50,s,8,2);

		sprintf(s,"QFPS %02.2f",frmRate);
		PrintGlow(5,70,s,8,2);
		sprintf(s,"Runs %d",numRunsToMakeUp);
		PrintGlow(120,70,s,8,2);

		sprintf(s,"Mons %d (%d)",CountMonsters(MONS_ANYBODY),CountMonsters(MONS_NOBODY));
		PrintGlow(5,90,s,8,2);
		sprintf(s,"Bul %d",config.numBullets - CountBullets(BLT_NONE)); // a little hackish but whatever
		PrintGlow(120,90,s,8,2);
		
		sprintf(s,"X: %i, Y: %i",goodguy->mapx,goodguy->mapy); // a little hackish but whatever
		PrintGlow(120,150,s,8,2);

		int n = 0;
		for(int i=0; i<MAX_SPECIAL; i++)
			if(curMap->special[i].x!=255)
				++n;

		sprintf(s,"Fx %d", CountParticles());
		PrintGlow(5,110,s,8,2);
		sprintf(s,"Spcl %03d", n);
		PrintGlow(120,110,s,8,2);

		n = 0;
		for(int x=0; x<curMap->width; ++x)
			for(int y=0; y<curMap->height; ++y)
				if(curMap->GetTile(x, y)->item)
					++n;

		mapTile_t* t = curMap->GetTile(goodguy->mapx, goodguy->mapy);
		sprintf(s,"Light %d/%d", t->templight, t->light);
		PrintGlow(5,130,s,8,2);
		sprintf(s,"Items %d", n);
		PrintGlow(120,130,s,8,2);

		int KEY_MAX = 0;
		const byte* key = SDL_GetKeyboardState(&KEY_MAX);
		char* end = s + sprintf(s,"Keys: ");
		for (int i = 0; i < KEY_MAX; ++i)
			if (key[i])
				end += sprintf(end, "%s ", ScanCodeText(i));
		PrintGlow(5,150,s,8,2);

		end = s + sprintf(s,"Mouse: ");
		for (int i = 0; i < 10; ++i)
			if (gamemgl->mouse_b & (1<<i))
				end += sprintf(end, "%d ", i);
		PrintGlow(5,170,s,8,2);
	}
	// update statistics
	d=timeGetTime();
	if(d-tickerTime>999)
	{
		frmRate=(frmRate*3+((float)visFrms/((float)(d-tickerTime)/1000.0f)))/4;
		visFrms=0;
		tickerTime=d;
	}
	if(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE)
	{
		if (player.clock%10==0)
		{
			if(!choseColor)
			{
				PickDiscoColor();
				choseColor = true;
			}
		}
		else if (choseColor)
			choseColor=false;
	}
	if(profile.progress.purchase[modeShopNum[MODE_TEENY]]&SIF_ACTIVE && !shopping)
	{
		if(curMap->flags&(MAP_UNDERWATER|MAP_LAVA|MAP_WAVY))
			gamemgl->TeensyWaterFlip(updFrameCount/2);
		else
			gamemgl->TeensyFlip();
	}
	else if(profile.progress.purchase[modeShopNum[MODE_RASTER]]&SIF_ACTIVE && !shopping)
	{
		if(curMap->flags&(MAP_UNDERWATER|MAP_LAVA|MAP_WAVY))
			gamemgl->RasterWaterFlip(updFrameCount/2);
		else
			gamemgl->RasterFlip();
	}
	else
	{
		if(curMap->flags&(MAP_UNDERWATER|MAP_LAVA|MAP_WAVY) && !shopping)
			gamemgl->WaterFlip(updFrameCount/2);
		else
			gamemgl->Flip();
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

	// stats for everyone, takes more effort though
	if(LastScanCode() == SDL_SCANCODE_F3)
	{
		showStats=1-showStats;
		lastKey=0;
	}
}

void PauseGame(void)
{
	if (gameMode!=GAMEMODE_PLAY)
		return;
	InitPauseMenu();
	gameMode=GAMEMODE_MENU;
}

byte PlayALevel(byte map)
{
	int lastTime=1;
	byte exitcode=0;
	static byte wasPaused;

	if(!InitLevel(map))
	{
		mapToGoTo=255;
		return LEVEL_ABORT;
	}

	exitcode=LEVEL_PLAYING;
	gameMode=GAMEMODE_PLAY;

	UpdateGuys(curMap,&curWorld);	// this will force the camera into the right position
									// it also makes everybody animate by one frame, but no one will
									// ever notice
	while(exitcode==LEVEL_PLAYING)
	{
		lastTime+=TimeLength();
		StartClock();
		if(gameMode==GAMEMODE_PLAY)
			HandleKeyPresses();
		exitcode=LunaticRun(&lastTime);
		if(exitcode==LEVEL_PLAYING)
			LunaticDraw();

		if(gameMode==GAMEMODE_PLAY && wasPaused)
		{
			if(!(GetJoyButtons()&4))
				wasPaused=0;
		}
		if((lastKey==27 || (GetJoyButtons()&4)) && !wasPaused && gameMode==GAMEMODE_PLAY)
		{
			wasPaused=1;
			PauseGame();
		}

		if(!gamemgl->Process())
		{
			exitcode=LEVEL_ABORT;
			mapToGoTo=255;
		}
		EndClock();
	}

	if(exitcode==LEVEL_WIN)
	{
		PlayerWinLevel(0);
		PrintToLog("Tally",0);
		Tally(gamemgl,lastLevelName,0);
	}
	ExitLevel();
	return exitcode;
}

byte PlayWorld(MGLDraw *mgl,const char *fname)
{
	char fullName[64];
	byte result;

	if(!strcmp(fname,"tutorial.hsw"))
		tutorial=1;
	else
		tutorial=0;

	sprintf(fullName,"worlds/%s",fname);

	InitPlayer(GetWorldProgress(fname)->levelOn,fname);
	if(!LoadWorld(&curWorld,fullName))
		return 1;

	StopSong();
	InitWorld(&curWorld);

	mapNum=player.levelNum;
	SetPlayerStart(-1,-1);
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
			// don't do anything, play the same level
		}
		else if(result==LEVEL_WIN)
		{
			mapNum=mapToGoTo;
		}
		else if(result==WORLD_QUITGAME || result==WORLD_ABORT || result==WORLD_SHOP)
			break;
	}
	FreeWorld(&curWorld);
	if(result==WORLD_SHOP)
		doShop=1;
	if(result==WORLD_QUITGAME || result==WORLD_SHOP)
		return 0;
	else
		return 1;
}

void TestLevel(world_t *world,byte level)
{
	byte result;

	editing=2;

	ExitGuys();
	ClearTestProgress();

	memcpy(&curWorld,world,sizeof(world_t));
	InitWorld(&curWorld);

	GetWorldProgress("TEST")->levelOn=level;
	InitPlayer(GetWorldProgress("TEST")->levelOn,"TEST");

	mapNum=player.levelNum;
	SetPlayerStart(-1,-1);
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
			// don't do anything, play the same level
		}
		else if(result==LEVEL_WIN)
		{
			mapNum=mapToGoTo;
		}
		else if(result==WORLD_QUITGAME || result==WORLD_ABORT)
			break;
	}
	memset(&curWorld,0,sizeof(world_t));
	InitGuys(256);
	GetSpecialsFromMap(EditorGetMap()->special);
	editing=1;
}
