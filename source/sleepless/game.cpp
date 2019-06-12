#include "game.h"
#include "title.h"
#include "jamulfmv.h"
#include "special.h"
#include "progress.h"
#include "editor.h"
#include "scanner.h"
#include "shop.h"
#include "gallery.h"
#include "goal.h"
#include "config.h"
#include "log.h"
#include "customworld.h"
#include "palettes.h"
#include <stdlib.h>
#if __linux__
#include <unistd.h>
#endif

byte showStats=0,spookIt=0;
dword gameStartTime,visFrameCount,updFrameCount;
dword tickerTime;
int   visFrms;
float frmRate;
word numRunsToMakeUp;

char lastKey=0;
static char lastLevelName[32];
byte shopping,tutorial,verified;
byte doShop;

MGLDraw *gamemgl;
Map		*curMap;
byte gameMode=GAMEMODE_PLAY;
byte	mapToGoTo;
byte	worldNum;
byte    mapNum;
world_t curWorld;

byte msgFromOtherModules=0;
byte msgContent;

word windingDown;
byte windingUp;
byte windDownReason;
static byte idleGame=0,pictureNoKey,loadingUp=1;

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
	FreeProfile();
	StopSong();
}

byte InitLevel(byte map)
{
	PrintToLog("InitLevel",map);

	JamulSoundPurge();	// each level, that should be good

	if(curWorld.numMaps<=map)
		return 0;	// can't go to illegal map

	// make a copy of the map to be played
	curMap=new Map(curWorld.map[map]);

	verified=1;

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
	//InitPlayer(map,player.worldName);
	player.levelNum=map;
	InitMessage();
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

	strcpy(lastLevelName,curMap->name);

	LocateKeychains(&curWorld);
	RestoreGameplayGfx();

	if(LoadState(map,loadingUp)==0)
	{
		if(player.beenThere[map])	// you have been here - the fact that it's not saved means you are cheating!
		{
			NewMessage("YOU CHEAT!",50,1);
#ifndef _DEBUG
			player.brains=0;
			player.candles=0;
			if(player.hammers>1)
				player.hammers=1;
			if(player.hamSpeed<16)
				player.hamSpeed=16;
#endif
		}
		else
		{
			if(map==0)
				PauseGame();
		}
	}
	player.beenThere[map]=1;

	if(!loadingUp)
	{
		EasyStart();	// clear nearby badguys when you switch maps, but no help when you load into a deadly situation
		curMap->flags=curWorld.map[map]->flags;	// start with the default flags for this level
	}
	loadingUp=0;

	GetMonsterType(MONS_BOUAPHA)->hp=PlayerMaxLife();
	GetMonsterType(MONS_EVILCLONE)->hp=PlayerMaxLife();
	goodguy->maxHP=MonsterHP(MONS_BOUAPHA);
	PutCamera(goodguy->x,goodguy->y);

	// colorize according to equipped weapon
	ColorizeBouapha();
	UpdateGoodBone();
	goodguy->seq=ANIM_IDLE;
	goodguy->frm=0;
	goodguy->frmTimer=0;
	goodguy->frmAdvance=128;
	goodguy->action=ACTION_IDLE;
	if(spookIt)
	{
		PutSpook();
		spookIt=0;
	}
	gamemgl->SetMouse(320,240);

	if(player.levelNum==6 && player.ability[ABIL_FISH]==0)
	{
		curMap->map[53+32*curMap->width].item=186;	// place the reel!
	}

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

	delete curMap;
	PurgeMonsterSprites();
}

void SetGameIdle(bool b)
{
	idleGame=b;
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
	if(curMap)
	{
		if(curMap->flags&MAP_UNDERWATER)
		{
			DumbSidePalette(gamemgl);
			return;
		}
	}
	gamemgl->RealizePalette();

}

void EnterPictureDisplay(void)
{
	gameMode=GAMEMODE_PIC;
	pictureNoKey=30;
	GetTaps();	// clear the key tap buffer
	gamemgl->MouseTap();
	gamemgl->RMouseTap();
}

byte GetGameMode(void)
{
	return gameMode;
}

byte LunaticRun(int *lastTime)
{
	byte frmsToRun;

	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	frmsToRun=(*lastTime/TIME_PER_FRAME);

	*lastTime-=frmsToRun*TIME_PER_FRAME;

	while(frmsToRun--)//(*lastTime>=TIME_PER_FRAME)
	{
		if(!gamemgl->Process())
		{
			mapToGoTo=255;
			return LEVEL_ABORT;
		}

		if(gameMode==GAMEMODE_PLAY)
		{
			profile.progress.totalTime=player.clock;

			if((rand()%20)==0)
				RespawnGuys(curMap);	// 1 in 20 chance of calling this, then each dead guy has 1 in 100 chance of respawning
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
					UpdateGuys(curMap,&curWorld);
					UpdateItems();
					UpdateBullets(curMap,&curWorld);
					CheckSpecials(curMap);
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
			switch(UpdatePauseMenu(gamemgl))
			{
				case PAUSE_PAUSED:
					break;
				case PAUSE_CONTINUE:
					lastKey=0;
					gameMode=GAMEMODE_PLAY;
					RestoreGameplayGfx();
					gamemgl->SetMouse(320,240);
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
					RestoreGameplayGfx();
					gameMode=GAMEMODE_PLAY;
					return LEVEL_ABORT;
					break;
				case PAUSE_EXIT:
					mapToGoTo=255;
					lastKey=0;
					RestoreGameplayGfx();
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
				byte t;

				t=GetTaps();
				if(gamemgl->MouseTap())
					t|=CONTROL_B1;
				if(gamemgl->RMouseTap())
					t|=CONTROL_B2;

				if(t&(CONTROL_B1|CONTROL_B2))
				{
					gameMode=GAMEMODE_PLAY;
					// restore the palette
					gamemgl->LoadBMP("graphics/title.bmp");
					RestoreGameplayGfx();
				}
			}
		}

		if(msgFromOtherModules==MSG_GOTOMAP)
		{
			if(goodguy->hp==0)
				goodguy->hp=1;
			spookIt=1;
			GetSpook();
			GoalTimeDist();
			goodguy->dx=goodguy->dy=0;
			SaveState();
			mapToGoTo=msgContent;
			windingDown=30;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
			player.mapFrom=player.levelNum;
		}
		else if(msgFromOtherModules==MSG_DUMBDIE)
		{
			spookIt=1;
			GetSpook();
			GoalTimeDist();
			goodguy->dx=goodguy->dy=0;
			SaveState();
			mapToGoTo=msgContent;
			windingDown=30;
			windDownReason=LEVEL_ABORT;
			msgFromOtherModules=MSG_NONE;
			MakeNormalSound(SND_TURNEVIL);
		}
		else if(msgFromOtherModules==MSG_WINLEVEL)
		{
			PlaySong("spooky-pumpkin.ogg");
			VictoryText(gamemgl);
			Credits(gamemgl);
			CompleteGoal(9);
			if(profile.progress.totalTime<30*60*240)
				CompleteGoal(12);
			RemoveGuy(GetGuyOfType(MONS_RADISH));
			PrintToLog("Level Win!",0);
			goodguy->dx=goodguy->dy=0;
			if(goodguy->hp==0)
				goodguy->hp=1;
			player.journal[63]=1;
			SaveState();
			GoalTimeDist();
			PrintToLog("GoalTimeDist Done",0);
			mapToGoTo=msgContent;
			windingDown=40;
			windDownReason=LEVEL_WIN;
			msgFromOtherModules=MSG_NONE;
			player.boredom=0;

			// Throw down a dummy data file to mark the editor achieved
			// It's cheatable but that's kind of ok
			if (!IsCustomWorld())
			{
				FILE* f = fopen("profiles/editor.dat", "wt");
				char text[256];
				sprintf(text,"Editor unlocked by %s, what a cool guy!\n", profile.name);
				fwrite(text, sizeof(char), strlen(text), f);
				fclose(f);
			}
		}
		else if(msgFromOtherModules==MSG_RESET)
		{
			GoalTimeDist();
			goodguy->dx=goodguy->dy=0;
			goodguy->hp=goodguy->maxHP;
			player.life=goodguy->maxHP;
			player.ammo=0;
			SaveState();
			NewBigMessage("Try Again!",30);
			windingDown=20;
			windDownReason=LEVEL_RESET;
			msgFromOtherModules=MSG_NONE;
		}
		else if(msgFromOtherModules==MSG_SCANMONSTER)
		{
			msgFromOtherModules=MSG_NONE;
			gameMode=GAMEMODE_SCAN;
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

void LunaticDraw(void)
{
	char s[128];
	dword d;

	// add all the sprites to the list
	if(gameMode!=GAMEMODE_PIC && gameMode!=GAMEMODE_SCAN)
	{
		RenderGuys(1);
		RenderBullets();
		RenderParticles();
		RenderItAll(&curWorld,curMap,MAP_SHOWLIGHTS|MAP_SHOWPICKUPS|MAP_SHOWOTHERITEMS|MAP_SHOWWALLS);
		RenderMessage();
		PlayerRenderInterface(gamemgl);
		if(gameMode==GAMEMODE_MENU)
			RenderPauseMenu();
		else
			RenderUnpaused();
	}
	else if(gameMode==GAMEMODE_PIC)
	{
		// nothing to do
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

		sprintf(s,"Mons %d",CountMonsters(MONS_ANYBODY));
		PrintGlow(5,90,s,8,2);
		sprintf(s,"Bul %d",config.numBullets - CountBullets(0)); // a little hackish but whatever
		PrintGlow(120,90,s,8,2);

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

	if(profile.progress.purchase[modeShopNum[MODE_TEENY]]&SIF_ACTIVE)
	{
		if(curMap->flags&(MAP_UNDERWATER|MAP_LAVA))
			gamemgl->TeensyWaterFlip(updFrameCount/2);
		else
			gamemgl->TeensyFlip();
	}
	else if(profile.progress.purchase[modeShopNum[MODE_RASTER]]&SIF_ACTIVE)
	{
		if(curMap->flags&(MAP_UNDERWATER|MAP_LAVA))
			gamemgl->RasterWaterFlip(updFrameCount/2);
		else
			gamemgl->RasterFlip();
	}
	else
	{
		if(curMap->flags&(MAP_UNDERWATER|MAP_LAVA))
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
	static byte mouse3=1;

	k=gamemgl->LastKeyPressed();
	if(k)
	{
		lastKey=k;
		if((lastKey>='a' && lastKey<='z') || (lastKey>='A' && lastKey<='Z'))
			CheatKey(lastKey);
		if(lastKey=='1' && player.weaponLvl[0]>0)
		{
			player.weapon=1;
			ColorizeBouapha();
		}
		if(lastKey=='2' && player.weaponLvl[1]>0)
		{
			player.weapon=2;
			ColorizeBouapha();
		}
		if(lastKey=='3' && player.weaponLvl[2]>0)
		{
			player.weapon=3;
			ColorizeBouapha();
		}
		if(lastKey=='4' && player.weaponLvl[3]>0)
		{
			player.weapon=4;
			ColorizeBouapha();
		}
		if(lastKey=='5' && player.weaponLvl[4]>0)
		{
			player.weapon=5;
			ColorizeBouapha();
		}
		if(lastKey=='6' && player.weaponLvl[5]>0)
		{
			player.weapon=6;
			ColorizeBouapha();
		}
	}
	if(gamemgl->MouseDown3())
	{
		if(mouse3==0)
		{
			ChangeWeapon(1);
			ColorizeBouapha();
		}
		mouse3=1;
	}
	else
		mouse3=0;

	// stats for everyone, takes more effort though
	if(LastScanCode() == SDL_SCANCODE_F3)
	{
		showStats=1-showStats;
		lastKey=0;
	}
}

void PauseGame(void)
{
	if (gameMode!=GAMEMODE_PLAY || curMap == NULL)
		return;
	InitPauseMenu();
	gameMode=GAMEMODE_MENU;
}

byte PlayALevel(byte map)
{
	int lastTime=1;
	byte exitcode=0;
	static byte wasPaused;

	gameMode=0;

	if(!InitLevel(map))
	{
		mapToGoTo=255;
		return LEVEL_ABORT;
	}

	exitcode=LEVEL_PLAYING;
	if(gameMode!=GAMEMODE_MENU)
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
		if((lastKey==27 || (GetJoyButtons()&4)) && !wasPaused && gameMode==GAMEMODE_PLAY && windingDown==0 && windingUp==0)
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
	}
	ExitLevel();
	return exitcode;
}

byte PlayWorld(MGLDraw *mgl,const char *fname)
{
	char fullName[64];
	byte result;

	tutorial=0;
	loadingUp=1;

	sprintf(fullName,"worlds/%s",fname);

	InitPlayer(GetWorldProgress(fname)->levelOn,fname);
	if(!LoadWorld(&curWorld,fullName))
		return 1;

	StopSong();
	InitWorld(&curWorld);
	InitCustomWorld();
	if (!IsCustomWorld() && !VerifyHollowShw())
	{
		// maybe be mean later
		return 1;
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
			// don't do anything, play the same level
		}
		else if(result==LEVEL_WIN)
		{
			mapNum=mapToGoTo;
		}
		else if(result==WORLD_QUITGAME || result==WORLD_ABORT)
			break;
	}
	FreeWorld(&curWorld);

	if(result==WORLD_QUITGAME || result==WORLD_SHOP)
		return 0;
	else
		return 1;
}

void TestLevel(world_t *world,byte level)
{
	byte result;

	// TODO: try to make testing make sense
	MakeNormalSound(SND_BOUAPHAOUCH);
	return;

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

	char s[64];
	// blast all the editing profile data
	for(int i=0;i<64;i++) // used to be 19, upped to 64 to tolerate custom worlds
	{
		// delete all maps
		sprintf(s,"profiles/_editing_.%03d",i);
		unlink(s);
	}
}
