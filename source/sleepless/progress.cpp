#include "progress.h"
#include "nameentry.h"
#include "player.h"
#include "world.h"
#include "music.h"
#include "shop.h"
#include "customworld.h"
#include "editor.h"

static char prfName[64];
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
	FILE *f;
	char name[16];
	int i;

	firstTime=0;
	f=fopen("profile.cfg","rt");
	if(!f)
	{
		firstTime=1;	// ask the player to enter their name
		profile.sound=255;
		profile.music=128;
		return;
	}
	fscanf(f,"%s\n",name);
	fclose(f);
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

void SavePlayLists(FILE *f)
{
	int i;

	for(i=0;i<NUM_PLAYLISTS;i++)
	{
		fwrite(&profile.playList[i].numSongs,1,sizeof(byte),f);
		if(profile.playList[i].numSongs>0)
			fwrite(profile.playList[i].song,SONGNAME_LEN,profile.playList[i].numSongs,f);
	}
}

void SaveProfile(void)
{
	FILE *f;
	int i,j;

	f=fopen("profile.cfg","wt");
	fprintf(f,"%s\n",profile.name);
	fclose(f);

	sprintf(prfName,"profiles/%s.prf",profile.name);
	// also actually save the profile!
	f=fopen(prfName,"wb");
	fwrite(&profile,sizeof(profile_t),1,f);

	SavePlayLists(f);

	// to enforce the "TEST" world being present
	GetWorldProgress("TEST");
	// so that we can save the word!
	for(i=0;i<profile.progress.num_worlds;i++)
	{
		fwrite(&profile.progress.world[i],sizeof(worldData_t),1,f);
		for(j=0;j<profile.progress.world[i].levels;j++)
		{
			fwrite(&profile.progress.world[i].level[j],sizeof(levelData_t),1,f);
		}
	}
	fclose(f);
	firstTime=0;
}

void LoadPlayLists(FILE *f)
{
	int i;

	for(i=0;i<NUM_PLAYLISTS;i++)
	{
		fread(&profile.playList[i].numSongs,1,sizeof(byte),f);
		if(profile.playList[i].numSongs>0)
		{
			profile.playList[i].song=(char *)malloc(SONGNAME_LEN*profile.playList[i].numSongs);
			fread(profile.playList[i].song,SONGNAME_LEN,profile.playList[i].numSongs,f);
		}
		else
			profile.playList[i].song=NULL;
	}
}

void LoadProfile(const char *name)
{
	FILE *f;
	int i,j;

	strcpy(profile.name,name);
	sprintf(prfName,"profiles/%s.prf",profile.name);

	// save this profile as the current one.
	f=fopen("profile.cfg","wt");
	fprintf(f,"%s\n",profile.name);
	fclose(f);

	// now load it
	f=fopen(prfName,"rb");
	if(!f)	// file doesn't exist
	{
		DefaultProfile(name);
		return;
	}
	fread(&profile,sizeof(profile_t),1,f);
	LoadPlayLists(f);
	InitCustomWorld();

	if(profile.progress.num_worlds==0)
		profile.progress.world=NULL;
	else
	{
		profile.progress.world=(worldData_t *)malloc(sizeof(worldData_t)*profile.progress.num_worlds);
		for(i=0;i<profile.progress.num_worlds;i++)
		{
			fread(&profile.progress.world[i],sizeof(worldData_t),1,f);
			profile.progress.world[i].level=(levelData_t *)malloc(sizeof(levelData_t)*profile.progress.world[i].levels);
			for(j=0;j<profile.progress.world[i].levels;j++)
			{
				fread(&profile.progress.world[i].level[j],sizeof(levelData_t),1,f);
			}
		}
	}
	fclose(f);
}

byte FirstTime(void)
{
	return firstTime;
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
	FILE *f;
	char fname[64];
	word w;

	player.journal[50]=0;
	if (editing)
		sprintf(fname,"profiles/_editing_.%03d",player.levelNum);
	else
		sprintf(fname,"profiles/%s.%03d",profile.name,player.levelNum);
	f=fopen(fname,"wb");

	// first write out the player itself
	fwrite(&player,sizeof(player_t),1,f);
	// next the ID of whoever is tagged
	if(TaggedMonster()==NULL)
		w=65535;
	else
		w=TaggedMonster()->ID;

	fwrite(&w,sizeof(word),1,f);
	// then all specials
	SaveSpecials(f);
	// then level flags
	fwrite(&curMap->flags,sizeof(word),1,f);
	// then the map
	fwrite(curMap->map,curMap->width*curMap->height,sizeof(mapTile_t),f);
	// then the monsters
	SaveGuys(f);
	// then the bullets
	SaveBullets(f);

	fclose(f);

}

// set getplayer=2 to restore the level exactly as it was left, except for the player's life/ammo/items found
// (this is used for bouncing back out of the Dumb Side)
// getPlayer=1 is for a load - copies the player data from save
// getPlayer=0 is for changing maps - destroys the player data, replacing it with whatever is current
byte LoadState(byte lvl,byte getPlayer)
{
	FILE *f;
	char fname[64];
	word tagged;
	player_t tmp;

	if (editing)
		sprintf(fname,"profiles/_editing_.%03d",lvl);
	else
		sprintf(fname,"profiles/%s.%03d",profile.name,lvl);
	f=fopen(fname,"rb");
	if(f==NULL)
		return 0;

	// first read the player itself, undoing the damage to worldProg and levelProg
	memcpy(&tmp,&player,sizeof(player_t));
	fread(&player,sizeof(player_t),1,f);
	player.worldProg=tmp.worldProg;
	player.levelProg=tmp.levelProg;
	if(!getPlayer || getPlayer==2)
	{
		player=tmp;
	}

	// next the ID of whoever is tagged
	fread(&tagged,sizeof(word),1,f);
	// then all specials
	LoadSpecials(f,curMap->special);
	// then the flags
	fread(&curMap->flags,sizeof(word),1,f);
	// then the map
	fread(curMap->map,curMap->width*curMap->height,sizeof(mapTile_t),f);
	// then the monsters
	LoadGuys(f);
	if(tagged==65535)
		SetTagged(NULL);
	else
		SetTagged(GetGuy(tagged));
	// then the bullets
	LoadBullets(f);

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
	fclose(f);
	return 1;
}
