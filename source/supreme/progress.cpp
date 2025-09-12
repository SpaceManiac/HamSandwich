#include "progress.h"
#include "nameentry.h"
#include "player.h"
#include "world.h"
#include "music.h"
#include "shop.h"
#include "appdata.h"
#include "steam.h"
#include "ioext.h"
#include "string_extras.h"
#include "world_io_legacy.h"

static char prfName[64];
static byte firstTime;
profile_t profile;
byte modeShopNum[10];

void ApplyControlSettings()
{
	for (int i = 0; i < 2; ++i)
	{
		byte control[8];
		memcpy(&control[0], &profile.control[i], 6);
		memcpy(&control[6], &profile.progress.moreControl[i], 2);
		SetKeyboardBindings(i, 7, control);
	}

	byte joyControl[4] = {
		profile.joyCtrl[0],
		profile.joyCtrl[1],
		profile.progress.moreJoyCtrl[0],
		profile.progress.moreJoyCtrl[1],
	};
	// Only 3 so the Android version doesn't show a 4th button that does nothing.
	SetJoystickBindings(3, joyControl);
}

void InitProfile(void)
{
	char name[16];
	int i;

	firstTime=0;
	{
		auto f = AppdataOpen("profile.cfg");
		if(!f)
		{
			firstTime=1;	// ask the player to enter their name
			profile.sound=255;
			profile.music=128;
			return;
		}
		SdlRwStream stream(f.get());
		stream.getline(name, std::size(name));
	}
	LoadProfile(name);
	ApplyControlSettings();

	for(i=0;i<7;i++)
		modeShopNum[i]=ShopItemNumber(SHOP_MODE,i);
}

void FreeProfile(void)
{
	int i;

	for(i=0;i<4;i++)
		if(profile.playList[i].numSongs)
			free(profile.playList[i].song);

	if(profile.progress.world)
	{
		for(i=0;i<profile.progress.num_worlds;i++)
		{
			free(profile.progress.world[i].level);
			profile.progress.world[i].level=NULL;
		}
		free(profile.progress.world);
		profile.progress.world=NULL;
	}
}

void SavePlayLists(SDL_RWops *f)
{
	int i;

	for(i=0;i<NUM_PLAYLISTS;i++)
	{
		SDL_RWwrite(f,&profile.playList[i].numSongs,1,sizeof(byte));
		if(profile.playList[i].numSongs>0)
			SDL_RWwrite(f,profile.playList[i].song,SONGNAME_LEN,profile.playList[i].numSongs);
	}
}

void SaveProfile(void)
{
	int i,j;

	auto f = AppdataOpen_Write("profile.cfg");
	SDL_RWprintf(f.get(), "%s\n", profile.name);
	f.reset();

	sprintf(prfName,"profiles/%s.prf",profile.name);
	// also actually save the profile!
	f = AppdataOpen_Write(prfName);
	// begin fwrite(&profile, sizeof(profile_t), 1, f) emulation
	SDL_RWwrite(f, &profile, 68, 1);
	for(i = 0; i < NUM_PLAYLISTS; ++i)
	{
		SDL_RWwrite(f, "\0\0\0\0\0\0\0\0", 8, 1);
	}
	SDL_RWwrite(f, &profile.difficulty, 8, 1);
	// begin progress_t part
	{
		SDL_RWwrite(f, &profile.progress, 112, 1);
		SDL_RWwrite(f, "\0\0\0\0", 4, 1);  // skip worldData_t *world
		SDL_RWwrite(f, &profile.progress.kills, 2152 - 112 - 4, 1);
	}
	// end progress_t part
	SDL_RWwrite(f, &profile.motd, sizeof(profile.motd), 1);
	SDL_assert(SDL_RWtell(f) == 3284);
	// end fwrite emulation

	SavePlayLists(f.get());

	// to enforce the "TEST" world being present
	GetWorldProgress("TEST");
	// so that we can save the word!
	for(i=0;i<profile.progress.num_worlds;i++)
	{
		SDL_RWwrite(f, &profile.progress.world[i],76,1);
		SDL_RWwrite(f, "\0\0\0\0", 4, 1);
		for(j=0;j<profile.progress.world[i].levels;j++)
		{
			SDL_RWwrite(f, &profile.progress.world[i].level[j],sizeof(levelData_t),1);
		}
	}
	f.reset();
	AppdataSync();
	firstTime=0;
}

void LoadPlayLists(SDL_RWops *f)
{
	int i;

	for(i=0;i<NUM_PLAYLISTS;i++)
	{
		SDL_RWread(f,&profile.playList[i].numSongs,1,sizeof(byte));
		if(profile.playList[i].numSongs>0)
		{
			profile.playList[i].song=(char *)malloc(SONGNAME_LEN*profile.playList[i].numSongs);
			SDL_RWread(f,profile.playList[i].song,SONGNAME_LEN,profile.playList[i].numSongs);
			static_assert(SONGNAME_LEN == 128, "save compatibility broken; adjust this assertion if you are sure");
		}
		else
			profile.playList[i].song=NULL;
	}
}

void LoadProfile(const char *name)
{
	int i,j;

	ham_strcpy(profile.name,name);
	sprintf(prfName,"profiles/%s.prf",profile.name);

	// save this profile as the current one.
	auto f = AppdataOpen_Write("profile.cfg");
	SDL_RWprintf(f.get(), "%s\n", profile.name);
	f.reset();
	AppdataSync();

	// now load it
	f = AppdataOpen(prfName);
	if(!f)	// file doesn't exist
	{
		DefaultProfile(name);
		return;
	}
	// begin fread(&profile, sizeof(profile_t), 1, f) emulation
	SDL_RWread(f, &profile, 68, 1);
	SDL_RWseek(f, NUM_PLAYLISTS * 8, RW_SEEK_CUR);
	static_assert(NUM_PLAYLISTS == 4, "save compatibility broken; adjust this assertion if you are sure");
	SDL_RWread(f, &profile.difficulty, 8, 1);
	// begin progress_t part
	{
		SDL_RWread(f, &profile.progress, 112, 1);
		static_assert(offsetof(progress_t, world) == 112, "save compatibility broken; adjust this assertion if you are sure");
		SDL_RWseek(f, 4, RW_SEEK_CUR);  // skip worldData_t *world
		SDL_RWread(f, &profile.progress.kills, 2152 - 112 - 4, 1);
		//static_assert(sizeof(progress_t) == 2152 + sizeof(worldData_t*), "save compatibility broken; adjust this assertion if you are sure");
		static_assert(NUM_PROFILE_MONSTERS == 211, "save compatibility broken; adjust this assertion if you are sure");
	}
	// end progress_t part
	SDL_RWread(f, &profile.motd, sizeof(profile.motd), 1);
	SDL_assert(SDL_RWtell(f) == 3284);
	// end fread emulation
	LoadPlayLists(f.get());

	if(profile.progress.num_worlds==0)
		profile.progress.world=NULL;
	else
	{
		profile.progress.world=(worldData_t *)malloc(sizeof(worldData_t)*profile.progress.num_worlds);
		for(i=0;i<profile.progress.num_worlds;i++)
		{
			SDL_RWread(f, &profile.progress.world[i],76,1);
			SDL_RWseek(f, 4, RW_SEEK_CUR);
			profile.progress.world[i].level=(levelData_t *)malloc(sizeof(levelData_t)*profile.progress.world[i].levels);
			for(j=0;j<profile.progress.world[i].levels;j++)
			{
				SDL_RWread(f, &profile.progress.world[i].level[j],sizeof(levelData_t),1);
			}
		}
	}
	f.reset();

	if (profile.progress.moreJoyCtrl[0] == 0)
	{
		profile.progress.moreControl[0][0]=SDL_SCANCODE_TAB;  // wpnlock
		profile.progress.moreControl[0][1]=0;  // reserved
		profile.progress.moreControl[1][0]=0;  // wpnlock
		profile.progress.moreControl[1][1]=0;  // reserved
		profile.progress.moreJoyCtrl[0]=2;
		profile.progress.moreJoyCtrl[1]=3;
	}

	SteamManager::Get()->ProfileReady();
}

byte FirstTime(void)
{
	return firstTime;
}

void SetFirstTime()
{
	AppdataDelete("profile.cfg");
	firstTime = true;
}

void ClearProgress(void)
{
	int i;

	profile.progress.totalCandles=0;
	profile.progress.totalBrains=0;
	profile.progress.totalTime=0;
	profile.progress.totalCoins=0;
	profile.progress.coinsSpent=0;
	profile.progress.totalWorlds=5;
	profile.progress.totalPercent=0;
	profile.progress.loonyKeys=0;
	profile.progress.loonyKeysUsed=0;

	profile.progress.totalPercent=0.0f;
	profile.progress.num_worlds=0;
	profile.progress.world=NULL;

	profile.progress.hammersThrown=0;
	profile.progress.damageDone=profile.progress.damageTaken=0;
	profile.progress.shotsFired=0;
	profile.progress.rages=0;
	profile.progress.runOver=0;
	profile.progress.finishedWorlds=0;
	profile.progress.footDistance=profile.progress.raftDistance=profile.progress.cartDistance=profile.progress.driveDistance=0;
	profile.progress.underwaterTime=0;
	profile.progress.grassChopped=0;
	profile.progress.keysFound=profile.progress.doorsOpened=0;
	profile.progress.calories=profile.progress.calsBurned=0;
	profile.progress.bestCombo=0;
	profile.progress.cheats=0;

	profile.progress.wpnLock=0;
	profile.progress.hudChoice = HudChoice::Supreme;
	for(i=0;i<NUM_PROFILE_MONSTERS;i++)
	{
		profile.progress.kills[i]=0;

		if((MonsterFlags(i,i)&(MF_NOHIT)) || (i==MONS_BOUAPHA) || // prescan unhittables, Bouapha, and derivatives
				(i==MONS_PLAYMECHA) || (i==MONS_PLAYSHROOM) ||
				(i==MONS_LUNACHICK) || (i==MONS_PWRBOUAPHA) ||
				(i==MONS_MINISUB))
			profile.progress.scanned[i]=1;
		else
			profile.progress.scanned[i]=0;
	}

	for(i=0;i<20;i++)
		profile.progress.movie[i]=0;
	for(i=0;i<100;i++)
		profile.progress.goal[i]=0;

	memset(profile.progress.expansion, 0, std::size(profile.progress.expansion));	// clear the expansion bytes

	profile.progress.damageDone=0;
	profile.progress.damageTaken=0;

	DefaultShopAvailability();
}

void EmptyPlaylist(playList_t *p)
{
	p->song=NULL;
	p->numSongs=0;
}

void OriginalPlaylist(playList_t *p)
{
	int i;

	p->numSongs=22;
	p->song=(char *)calloc(SONGNAME_LEN*p->numSongs,1);

	for(i=0;i<22;i++)
	{
		strcpy(&p->song[SONGNAME_LEN*i],GetOriginalSongName(i+1));
	}
}

void DefaultControls(void)
{
	profile.control[0][0]=SDL_SCANCODE_UP;
	profile.control[0][1]=SDL_SCANCODE_DOWN;
	profile.control[0][2]=SDL_SCANCODE_LEFT;
	profile.control[0][3]=SDL_SCANCODE_RIGHT;
	profile.control[0][4]=SDL_SCANCODE_LCTRL;
	profile.control[0][5]=SDL_SCANCODE_LSHIFT;
	profile.control[1][0]=0;
	profile.control[1][1]=0;
	profile.control[1][2]=0;
	profile.control[1][3]=0;
	profile.control[1][4]=SDL_SCANCODE_Z;
	profile.control[1][5]=SDL_SCANCODE_X;

	profile.progress.moreControl[0][0]=SDL_SCANCODE_TAB;  // wpnlock
	profile.progress.moreControl[0][1]=0;  // reserved
	profile.progress.moreControl[1][0]=0;  // wpnlock
	profile.progress.moreControl[1][1]=0;  // reserved

	profile.joyCtrl[0]=0;
	profile.joyCtrl[1]=1;
	profile.progress.moreJoyCtrl[0]=2;
	profile.progress.moreJoyCtrl[1]=3;
}

void DefaultProfile(const char *name)
{
	ham_strcpy(profile.name,name);

	profile.sound=255;
	profile.music=128;
	profile.musicMode=MUSIC_OFFICIAL|MUSIC_REPEAT;
	profile.difficulty=0;
	profile.playAs=PLAY_BOUAPHA;

	profile.brainRadar=0;
	profile.candleRadar=0;
	profile.moveNShoot=0;
	profile.nameVerified=0;
	strcpy(profile.motd,"Thanks for buying Supreme With Cheese!  Click the button to download the latest news update.");
	profile.lastWorld[0]='\0';
	OriginalPlaylist(&profile.playList[0]);
	EmptyPlaylist(&profile.playList[1]);
	EmptyPlaylist(&profile.playList[2]);
	EmptyPlaylist(&profile.playList[3]);
	DefaultControls();
	ClearProgress();
	ApplyControlSettings();
	SaveProfile();
	JamulSoundVolume(profile.sound);
	SetMusicVolume(profile.music);
}

void DefaultLevelProgress(levelData_t *me,byte levelNum)
{
	me->flags=0;
	me->levelNum=levelNum;
	me->percentage=0;
	me->recordBaseScore=0;
	me->recordCombo=0;
	me->recordDestroy=0;
}

levelData_t *GetLevelProgress(const char *fname,byte levelNum)
{
	worldData_t *w = GetWorldProgress(fname);

	for (levelData_t &level : w->Levels())
	{
		if (level.levelNum==levelNum)
		{
			return &level;
		}
	}

	// if you got here, the progress for this level is not stored
	w->levels++;
	if(w->level==NULL)
		w->level=(levelData_t *)malloc(sizeof(levelData_t));
	else
		w->level=(levelData_t *)realloc(w->level,sizeof(levelData_t)*w->levels);

	if(w->level==NULL)
		FatalError("Out of memory!!");

	DefaultLevelProgress(&w->level[w->levels-1],levelNum);

	return &w->level[w->levels-1];
}

void DefaultWorldProgress(worldData_t *me,const char *fname)
{
	int i;

	if(me->level)
		free(me->level);

	ham_strcpy(me->filename,fname);
	me->keychains=0;
	me->percentage=0;
	me->levelOn=0;
	me->level=NULL;
	me->levels=0;

	for(i=0;i<8;i++)
		me->var[i]=0;
}

void ClearTestProgress(void)
{
	DefaultWorldProgress(GetWorldProgress("TEST"),"TEST");
}

worldData_t *GetWorldProgress(const char *fname)
{
	int i;

	for(i=0;i<profile.progress.num_worlds;i++)
	{
		if(!strcmp(profile.progress.world[i].filename,fname))
		{
			return &profile.progress.world[i];
		}
	}

	// if you got here, the world is not in the list
	profile.progress.num_worlds++;
	if(profile.progress.world==NULL)
		profile.progress.world=(worldData_t *)malloc(sizeof(worldData_t));
	else
		profile.progress.world=(worldData_t *)realloc(profile.progress.world,sizeof(worldData_t)*profile.progress.num_worlds);

	if(profile.progress.world==NULL)
		FatalError("Out of memory!!");

	profile.progress.world[profile.progress.num_worlds-1].level=NULL;
	DefaultWorldProgress(&profile.progress.world[profile.progress.num_worlds-1],fname);

	return &profile.progress.world[profile.progress.num_worlds-1];
}

worldData_t *GetWorldProgressNoCreate(const char *fname)
{
	int i;

	for(i=0;i<profile.progress.num_worlds;i++)
	{
		if(!strcmp(profile.progress.world[i].filename,fname))
		{
			return &profile.progress.world[i];
		}
	}

	// if you got here, the world is not in the list
	return NULL;
}

byte LevelsPassed(worldData_t *world)
{
	byte total=0;

	for (const levelData_t &level : world->Levels())
		if((level.flags&LF_PASSED) && !(curWorld.map[level.levelNum]->flags&(MAP_SECRET|MAP_HUB)))
			total++;

	return total;
}

byte LevelIsPassed(worldData_t *world,byte level)
{
	if(world==NULL)
		return 0;

	for (const levelData_t &levelData : world->Levels())
		if (levelData.levelNum==level && (levelData.flags&LF_PASSED))
			return 1;

	return 0;
}

void StoreWorldResults(worldData_t *me,world_t *world)
{
	int pts=0;
	int maxPts=0;
	for (const Map *map : world->Maps())
		if(!(map->flags&MAP_HUB))
			maxPts+=1000;

	for (const levelData_t &level : me->Levels())
	{
		if(level.flags&LF_PASSED)
		{
			pts+=800;
			if(level.flags&LF_CANDLES)
				pts+=200;
		}
	}

	float newpct=(float)pts*100.0f/(float)maxPts;

	if(newpct>me->percentage)
		me->percentage=newpct;
}

dword CountLevelsPassed(void)
{
	dword result;
	int i,j;

	result=0;
	for(i=0;i<profile.progress.num_worlds;i++)
	{
		if(strcmp(profile.progress.world[i].filename,"TEST"))
		{
			for(j=0;j<profile.progress.world[i].levels;j++)
			{
				if(profile.progress.world[i].level[j].flags&LF_PASSED)
					result++;
			}
		}
	}
	return result;
}

void CalcFinishedWorlds(void)
{
	dword result;
	int i;

	result=0;
	for(i=0;i<profile.progress.num_worlds;i++)
	{
		if(strcmp(profile.progress.world[i].filename,"TEST") &&
			strcmp(profile.progress.world[i].filename,"tutorial.dlw"))
		{
			if(profile.progress.world[i].percentage==100.0f)
				result++;
		}
	}
	profile.progress.finishedWorlds=result;
}

void EraseWorldProgress(const char *fname)
{
	int i,me;

	me=-1;
	for(i=0;i<profile.progress.num_worlds;i++)
		if(!strcmp(profile.progress.world[i].filename,fname))
		{
			me=i;
			break;
		}
	if(me==-1)
		return;	// world isn't stored anyway

	free(profile.progress.world[me].level);	// free the levels

	for(i=me+1;i<profile.progress.num_worlds;i++)	// and shove down all later worlds to cover this one
	{
		memcpy(&profile.progress.world[i-1],&profile.progress.world[i],sizeof(worldData_t));
	}
	profile.progress.num_worlds--;
	profile.progress.world=(worldData_t *)realloc(profile.progress.world,sizeof(worldData_t)*profile.progress.num_worlds);
	SaveProfile();
}

static const char diffName[][16] = {
	"Normal",
	"Hard",
	"Lunatic",
};
static_assert(std::size(diffName) == MAX_DIFFICULTY, "Must give new difficulties a name");

const char* GetDifficultyName(int difficulty)
{
	if (difficulty >= 0 && difficulty < MAX_DIFFICULTY)
		return diffName[difficulty];
	return "???";
}
