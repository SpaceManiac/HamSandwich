#include "progress.h"
#include "nameentry.h"
#include "player.h"
#include "world.h"
#include "music.h"
#include "shop.h"
#include "customworld.h"
#include "editor.h"
#include "appdata.h"
#include "ioext.h"
#include "string_extras.h"
#include "archipelago.h"

static byte firstTime;
profile_t profile;
byte modeShopNum[10];

void ApplyControlSettings()
{
	SetKeyboardBindings(0, 6, profile.control[0]);
	SetKeyboardBindings(1, 6, profile.control[1]);
	SetJoystickBindings(2, profile.joyCtrl);
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

	char prfName[128];

	if (auto ap = Archipelago())
	{
		ham_sprintf(prfName, "profiles/archipelago/%s.prf", ap->SaveName());
	}
	else
	{
		ham_sprintf(prfName,"profiles/%s.prf",profile.name);

		// save this profile as the current one.
		auto f = AppdataOpen_Write("profile.cfg");
		SDL_RWprintf(f.get(),"%s\n",profile.name);
		f.reset();
	}

	// also actually save the profile!
	auto f = AppdataOpen_Write(prfName);
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
	SDL_assert(SDL_RWtell(f) == 2260);
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
		}
		else
			profile.playList[i].song=NULL;
	}
}

void LoadProfile(const char *name)
{
	int i,j;

	char prfName[128];
	if (auto ap = Archipelago())
	{
		ham_sprintf(prfName, "profiles/archipelago/%s.prf", ap->SaveName());
	}
	else
	{
		ham_sprintf(prfName,"profiles/%s.prf",name);

		// save this profile as the current one.
		auto f = AppdataOpen_Write("profile.cfg");
		SDL_RWprintf(f.get(),"%s\n",name);
		f.reset();
		AppdataSync();
	}

	// now load it
	auto f = AppdataOpen(prfName);
	if(!f)	// file doesn't exist
	{
		DefaultProfile(name);
		return;
	}
	// begin fread(&profile, sizeof(profile_t), 1, f) emulation
	SDL_RWread(f, &profile, 68, 1);
	for(i = 0; i < NUM_PLAYLISTS; ++i)
	{
		SDL_RWseek(f, 8, RW_SEEK_CUR);
	}
	SDL_RWread(f, &profile.difficulty, 8, 1);
	// begin progress_t part
	{
		SDL_RWread(f, &profile.progress, 112, 1);
		SDL_RWseek(f, 4, RW_SEEK_CUR);  // skip worldData_t *world
		SDL_RWread(f, &profile.progress.kills, 2152 - 112 - 4, 1);
	}
	// end progress_t part
	SDL_assert(SDL_RWtell(f) == 2260);
	// end fread emulation
	LoadPlayLists(f.get());
	InitCustomWorld();

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
	for(i=0;i<NUM_MONSTERS;i++)
	{
		profile.progress.kills[i]=0;

		if((MonsterFlags(i,i)&(MF_NOHIT)) || (i==MONS_BOUAPHA))
			profile.progress.scanned[i]=1;
		else
			profile.progress.scanned[i]=0;
	}

	for(i=0;i<20;i++)
		profile.progress.movie[i]=0;
	for(i=0;i<100;i++)
		profile.progress.goal[i]=0;

	memset(profile.progress.expansion,0,EXPANSION_SIZE);	// clear the expansion bytes

	profile.progress.damageDone=0;
	profile.progress.damageTaken=0;

	DefaultShopAvailability();
}

void EmptyPlaylist(playList_t *p)
{
	p->song=NULL;
	p->numSongs=0;
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

	profile.joyCtrl[0]=0;
	profile.joyCtrl[1]=1;
}

void DefaultProfile(const char *name)
{
	strcpy(profile.name,name);

	profile.sound=255;
	profile.music=128;
	profile.musicMode=MUSIC_OFFICIAL|MUSIC_REPEAT;
	profile.difficulty=0;
	profile.playAs=PLAY_BOUAPHA;

	profile.brainRadar=0;
	profile.candleRadar=0;
	profile.moveNShoot=0;
	profile.nameVerified=0;
	strcpy(profile.lastWorld, "hollow.shw");
	EmptyPlaylist(&profile.playList[0]);
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
	worldData_t *w;
	int i;

	w=GetWorldProgress(fname);

	for(i=0;i<w->levels;i++)
	{
		if(w->level[i].levelNum==levelNum)
		{
			return &w->level[i];
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

	strcpy(me->filename,fname);
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

worldData_t *GetWorldProgressNoCreate(char *fname)
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
	int i;
	byte total=0;

	for(i=0;i<world->levels;i++)
		if((world->level[i].flags&LF_PASSED) && !(curWorld.map[world->level[i].levelNum]->flags&(MAP_SECRET|MAP_HUB)))
			total++;

	return total;
}

byte LevelIsPassed(worldData_t *world,byte level)
{
	int i;

	if(world==NULL)
		return 0;

	for(i=0;i<world->levels;i++)
		if(world->level[i].levelNum==level && (world->level[i].flags&LF_PASSED))
			return 1;

	return 0;
}

void StoreWorldResults(worldData_t *me,world_t *world)
{
	float newpct;
	int i,maxPts,pts;


	newpct=0.0f;
	pts=0;
	maxPts=0;
	for(i=0;i<world->numMaps;i++)
		if(!(world->map[i]->flags&MAP_HUB))
			maxPts+=1000;

	for(i=0;i<me->levels;i++)
	{
		if(me->level[i].flags&LF_PASSED)
		{
			pts+=800;
			if(me->level[i].flags&LF_CANDLES)
				pts+=200;
		}
	}

	newpct=(float)pts*100.0f/(float)maxPts;

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
}

void EraseWorldProgress(char *fname)
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

void SaveState(void)
{
	char fname[128];
	word w;

	player.journal[50]=0;
	if (auto ap = Archipelago())
	{
		ham_sprintf(fname, "profiles/archipelago/%s.%03d", ap->SaveName(), player.levelNum);
	}
	else if (editing)
	{
		ham_sprintf(fname,"profiles/_editing_.%03d",player.levelNum);
	}
	else
	{
		ham_sprintf(fname,"profiles/%s.%03d",profile.name,player.levelNum);
	}
	auto f = AppdataOpen_Write(fname);

	// first write out the player itself
	SDL_RWwrite(f, &player, offsetof(player_t, worldProg), 1);
	SDL_RWwrite(f, "\0\0\0\0\0\0\0\0", 8, 1);
	SDL_RWwrite(f, &player.shield, sizeof(player_t) - offsetof(player_t, shield), 1);

	// next the ID of whoever is tagged
	if(TaggedMonster()==NULL)
		w=65535;
	else
		w=TaggedMonster()->ID;

	SDL_RWwrite(f,&w,sizeof(word),1);
	// then all specials
	SaveSpecials(f.get());
	// then level flags
	SDL_RWwrite(f,&curMap->flags,sizeof(word),1);
	// then the map
	SDL_RWwrite(f,curMap->map,curMap->width*curMap->height,sizeof(mapTile_t));
	// then the monsters
	SaveGuys(f.get());
	// then the bullets
	SaveBullets(f.get());

	f.reset();
	AppdataSync();
}

// set getplayer=2 to restore the level exactly as it was left, except for the player's life/ammo/items found
// (this is used for bouncing back out of the Dumb Side)
// getPlayer=1 is for a load - copies the player data from save
// getPlayer=0 is for changing maps - destroys the player data, replacing it with whatever is current
// getPlayer=3 is for Archipelago load - load the player then bail w/o loading the map contents
byte LoadState(byte lvl,byte getPlayer)
{
	char fname[128];
	word tagged;

	if (auto ap = Archipelago())
	{
		ham_sprintf(fname, "profiles/archipelago/%s.%03d", ap->SaveName(), lvl);
	}
	else if (editing)
	{
		ham_sprintf(fname,"profiles/_editing_.%03d",lvl);
	}
	else
	{
		ham_sprintf(fname,"profiles/%s.%03d",profile.name,lvl);
	}
	auto f = AppdataOpen(fname);
	if(f==NULL)
		return 0;

	// first read the player itself, undoing the damage to worldProg and levelProg
	if (getPlayer == 1)
	{
		SDL_RWread(f, &player, offsetof(player_t, worldProg), 1);
		SDL_RWseek(f, 8, RW_SEEK_CUR);
		SDL_RWread(f, &player.shield, sizeof(player_t) - offsetof(player_t, shield), 1);
	}
	else // !getPlayer || getPlayer==2
	{
		SDL_RWseek(f, 368, RW_SEEK_CUR);
	}
	static_assert(sizeof(player_t) - offsetof(player_t, shield) + offsetof(player_t, worldProg) + 8 == 368, "save compatibility broken; adjust this assertion if you are sure");

	if (getPlayer == 3)
	{
		return 1;
	}

	// next the ID of whoever is tagged
	SDL_RWread(f,&tagged,sizeof(word),1);
	// then all specials
	LoadSpecials(f.get(),curMap->special);
	// then the flags
	SDL_RWread(f,&curMap->flags,sizeof(word),1);
	// then the map
	SDL_RWread(f,curMap->map,curMap->width*curMap->height,sizeof(mapTile_t));
	// then the monsters
	LoadGuys(f.get());
	if(tagged==65535)
		SetTagged(NULL);
	else
		SetTagged(GetGuy(tagged));
	// then the bullets
	LoadBullets(f.get());

	if(getPlayer==2)
	{
		// goodguy better be there, but just in case, I guess
		if(goodguy==NULL)
		{
			AddGuy(0,0,0,MONS_BOUAPHA,1);
			PutPlayerAtStart(goodguy);
		}
		goodguy->hp=player.life;
	}
	else
	{
		if(goodguy==NULL)
		{
			AddGuy(0,0,0,MONS_BOUAPHA,1);
			PutPlayerAtStart(goodguy);
		}
		if(!getPlayer)
		{
			goodguy->hp=player.life;
			PutPlayerAtStart(goodguy);
		}
	}
	PutCamera(goodguy->x,goodguy->y);
	for(int i=0;i<curMap->width*curMap->height;i++)
	{
		curMap->map[i].templight=-32;
	}
	return 1;
}
