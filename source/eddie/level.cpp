#include "level.h"
#include "random.h"
#include "backgd.h"
#include "guy.h"
#include "particle.h"
#include "tile.h"
#include "player.h"
#include "mission.h"
#include "intro.h"
#include "game.h"
#include "options.h"
#include "sound.h"

worldDef_t world[NUM_WORLDS]={
	{"Earth",
		{
			{"graphics\\earth1.bmp",
			0,1,0,15,0,FIXAMT*4,0,16,0,20,GUY_SHROOM,NULL,{0,0,0,0},{0,0}},
			{"graphics\\earth1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,NULL,{120,0,0,0},{0,0}},
			{"graphics\\earth1.bmp",
			0,1,0,15,0,FIXAMT*6,0,20,0,20,GUY_SHROOM,NULL,{30,0,0,0},{0,0}},
			{"graphics\\earth1.bmp",
			0,1,0,10,20,FIXAMT*4,FIXAMT*4,12,12,20,GUY_SHROOM,NULL,{30,0,0,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,0,0,0,0,0,0,0,0,GUY_SHROOM,InitHordeLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Rigel IV",
		{
			{"graphics\\rigel1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,NULL,{60,0,0,0},{0,0}},
			{"graphics\\rigel1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,NULL,{60,120,0,0},{0,0}},
			{"graphics\\rigel1.bmp",
			0,1,0,15,0,FIXAMT*8,0,20,0,20,GUY_SHROOM,NULL,{30,120,0,0},{0,0}},
			{"graphics\\rigel1.bmp",
			0,1,0,10,20,FIXAMT*6,FIXAMT*4,12,12,20,GUY_SHROOM,NULL,{1,60,0,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,1800,0,0,0,0,0,0,0,GUY_SHROOM,InitAsteroidLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Aguaworld",
		{
			{"graphics\\agua1.bmp",
			1,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,NULL,{60,30,0,0},{0,0}},
			{"graphics\\agua1.bmp",
			1,1,0,15,0,FIXAMT*8,0,16,0,20,GUY_SHROOM,NULL,{10,30,120,0},{0,0}},
			{"graphics\\agua1.bmp",
			1,1,0,15,0,FIXAMT*8,0,20,0,20,GUY_SHROOM,NULL,{1,5,20,120},{0,0}},
			{"graphics\\agua1.bmp",
			1,1,0,10,20,FIXAMT*8,FIXAMT*6,12,12,20,GUY_SHROOM,NULL,{1,10,20,60},{0,0}},
			{"graphics\\space.bmp",
			1,0,1800,0,0,0,0,0,0,0,GUY_SHROOM,InitTerritoryLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Prometheus IX",
		{
			{"graphics\\prom1.bmp",
			0,1,0,15,0,FIXAMT*4,0,16,0,20,GUY_SHROOM,InitPrometheusLevel,{30,0,0,0},{0,0}},
			{"graphics\\prom1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,InitPrometheusLevel,{30,120,0,0},{0,0}},
			{"graphics\\prom1.bmp",
			0,1,0,15,0,FIXAMT*8,0,20,0,20,GUY_SHROOM,InitPrometheusLevel,{30,120,0,0},{0,0}},
			{"graphics\\prom1.bmp",
			0,1,0,10,20,FIXAMT*8,FIXAMT*6,12,12,20,GUY_SHROOM,InitPrometheusLevel,{30,120,0,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,0,0,0,0,0,0,0,0,GUY_SHROOM,InitHordeLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Tartarus II",
		{
			{"graphics\\tart1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,InitTartarusLevel,{30,0,0,0},{0,0}},
			{"graphics\\tart1.bmp",
			0,1,0,15,0,FIXAMT*8,0,16,0,20,GUY_SHROOM,InitTartarusLevel,{30,80,0,0},{0,0}},
			{"graphics\\tart1.bmp",
			0,1,0,15,0,FIXAMT*8,0,20,0,20,GUY_SHROOM,InitTartarusLevel,{30,30,0,0},{0,0}},
			{"graphics\\tart1.bmp",
			0,1,0,10,20,FIXAMT*8,FIXAMT*8,12,12,20,GUY_SHROOM,InitTartarusLevel,{30,30,0,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,1800,0,0,0,0,0,0,0,GUY_SHROOM,InitAsteroidLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Asteroid Belt",
		{
			{"graphics\\astro1.bmp",
			0,0,0,0,0,0,0,0,0,4,GUY_BIGROID,InitOtherAsteroidLevel,{0,0,0,0},{0,0}},
			{"graphics\\astro2.bmp",
			0,0,0,0,0,0,0,0,0,10,GUY_BIGROID,InitOtherAsteroidLevel,{0,0,0,0},{0,0}},
			{"graphics\\astro3.bmp",
			0,0,0,0,0,0,0,0,0,20,GUY_BIGROID,InitOtherAsteroidLevel,{0,0,0,0},{0,0}},
			{"graphics\\astro1.bmp",
			0,0,0,0,0,0,0,0,0,30,GUY_BIGROID,InitOtherAsteroidLevel,{0,0,0,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,1800,0,0,0,0,0,0,0,GUY_SHROOM,InitTerritoryLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Varakkis",
		{
			{"graphics\\varak1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,NULL,{160,60,0,0},{0,0}},
			{"graphics\\varak1.bmp",
			0,1,0,15,0,FIXAMT*8,0,16,0,20,GUY_SHROOM,NULL,{120,60,60,0},{0,0}},
			{"graphics\\varak1.bmp",
			0,1,0,15,0,FIXAMT*10,0,20,0,20,GUY_SHROOM,NULL,{30,60,30,0},{0,0}},
			{"graphics\\varak1.bmp",
			0,1,0,10,20,FIXAMT*8,FIXAMT*8,12,12,20,GUY_SHROOM,NULL,{30,60,30,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,0,0,0,0,0,0,0,0,GUY_SHROOM,InitHordeLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Jurassika",
		{
			{"graphics\\juras1.bmp",
			0,1,0,15,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,InitJurassicLevel,{30,10,0,0},{0,0}},
			{"graphics\\juras1.bmp",
			0,1,0,15,0,FIXAMT*8,0,16,0,20,GUY_SHROOM,InitJurassicLevel,{120,10,20,0},{0,0}},
			{"graphics\\juras1.bmp",
			0,1,0,15,0,FIXAMT*10,0,20,0,20,GUY_SHROOM,InitJurassicLevel,{30,30,20,0},{0,0}},
			{"graphics\\juras1.bmp",
			0,1,0,10,20,FIXAMT*8,FIXAMT*10,12,12,20,GUY_SHROOM,InitJurassicLevel,{30,100,20,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,1800,0,0,0,0,0,0,0,GUY_SHROOM,InitAsteroidLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Globulos",
		{
			{"graphics\\glob1.bmp",
			0,1,0,0,0,FIXAMT*4,0,16,0,20,GUY_SHROOM,InitGlobLevel,{0,0,0,0},{0,0}},
			{"graphics\\glob1.bmp",
			0,1,0,0,0,FIXAMT*6,0,16,0,20,GUY_SHROOM,InitGlobLevel,{120,0,0,0},{0,0}},
			{"graphics\\glob1.bmp",
			0,1,0,0,0,FIXAMT*6,0,20,0,20,GUY_SHROOM,InitGlobLevel,{30,0,0,0},{0,0}},
			{"graphics\\glob1.bmp",
			0,1,0,0,0,FIXAMT*4,FIXAMT*4,12,12,20,GUY_SHROOM,InitGlobLevel,{30,0,0,0},{0,0}},
			{"graphics\\space.bmp",
			1,0,1800,0,0,0,0,0,0,0,GUY_SHROOM,InitTerritoryLevel,{0,0,0,0},{0,0}},
		}
	},
	{"Xykulix",
		{
			{"graphics\\final1.bmp",
			0,1,0,15,0,FIXAMT*8,0,16,0,20,GUY_SHROOM,NULL,{0,0,0,0},{0,0}},
			{"graphics\\final1.bmp",
			0,1,0,15,0,FIXAMT*10,0,16,0,20,GUY_SHROOM,NULL,{120,0,0,0},{0,0}},
			{"graphics\\final1.bmp",
			0,1,0,15,0,FIXAMT*12,0,20,0,20,GUY_SHROOM,NULL,{30,0,0,0},{0,0}},
			{"graphics\\final1.bmp",
			0,1,0,5,25,FIXAMT*10,FIXAMT*10,8,8,20,GUY_SHROOM,NULL,{30,0,0,0},{0,0}},
			{"graphics\\final1.bmp",
			0,1,0,0,0,0,0,0,0,0,GUY_SHROOM,InitAlienQueen,{40,0,0,0},{0,0}},
		}
	}
};

byte curLvl,curWorld;
int lIndex,wIndex;

static int clock[4];	// four clocks for timing level events
static byte value[2];	// two values for level flags and such
byte levelMode;			// how is this level won?
int levelClock;
byte bonus;
static char name[32];

byte GetLIndex(void)
{
	return lIndex;
}

void InitAsteroidLevel(void)
{
	int i;

	strcpy(name,"Asteroid Field");
	bonus=BNS_ASTEROID;

	for(i=0;i<20;i++)
		AddGuy(GUY_BIGROID+GetRand(3),GetRand(MAP_WIDTH),MAP_HEIGHT/2-GetRand(MAP_HEIGHT/2+3),0);
}

void InitHordeLevel(void)
{
	int i;

	switch(wIndex+1)
	{
#ifdef DEMO
		case WORLD_VARAKKIS:
#else
		case WORLD_EARTH:
#endif
			// checkerboard of squids and double-barrels
			for(i=0;i<6;i++)
			{
				if(i&1)
				{
					AddGuy(GUY_DOUBLESHIP,6+i*3,10,0);
					AddGuy(GUY_SQUIDSHIP,6+i*3,14,0);
				}
				else
				{
					AddGuy(GUY_SQUIDSHIP,6+i*3,10,0);
					AddGuy(GUY_DOUBLESHIP,6+i*3,14,0);
				}
			}
			break;
		case WORLD_PROMETHEUS:

			for(i=0;i<4;i++)
			{
				AddGuy(GUY_FBALLSHIP,3+i*6,10,0);
				AddGuy(GUY_DOUBLESHIP,6+i*6,14,0);
			}
			// double-thick wall of squids
			for(i=0;i<8;i++)
			{
				AddGuy(GUY_SQUIDSHIP,3+i*3,18,0);
				AddGuy(GUY_SQUIDSHIP,3+i*3,22,0);
			}
			break;
#ifndef DEMO
		case WORLD_VARAKKIS:
#else
		case WORLD_EARTH:
#endif
			for(i=0;i<8;i++)
			{
				AddGuy(GUY_DOUBLESHIP,3+i*3,6,0);
				AddGuy(GUY_FBALLSHIP,3+i*3,10,0);
				AddGuy(GUY_DOUBLESHIP,3+i*3,14,0);
				AddGuy(GUY_SQUIDSHIP,3+i*3,18,0);
			}
			break;
	}
	strcpy(name,"Galactic Horde");
	bonus=BNS_HORDE;
}

void InitTerritoryLevel(void)
{
	strcpy(name,"Hostile Territory");
	bonus=BNS_HOSTILE;
}

void InitPrometheusLevel(void)
{
	int i,numTurrets;

	if(lIndex==0 || lIndex==1)
		numTurrets=4;
	else if(lIndex==2)
		numTurrets=6;
	else
		numTurrets=8;
	for(i=0;i<numTurrets;i++)
		PlantMonster2(GUY_TURRET,GetRand(MAP_WIDTH),GetRand(MAP_HEIGHT/2)+2);
}

void InitTartarusLevel(void)
{
	int i,numVolcanoes;

	if(lIndex==0 || lIndex==1)
		numVolcanoes=4;
	else if(lIndex==2)
		numVolcanoes=6;
	else
		numVolcanoes=8;
	for(i=0;i<numVolcanoes;i++)
		PlantMonster2(GUY_VOLCANO,GetRand(MAP_WIDTH),GetRand(MAP_HEIGHT/2)+2);
}

void InitOtherAsteroidLevel(void)
{
	MakeLobster((MAP_WIDTH/2)*TILE_WIDTH*FIXAMT,(-10*TILE_HEIGHT*FIXAMT));
}

void InitJurassicLevel(void)
{
	int i,numVolcanoes;

	if(lIndex==0 || lIndex==1)
		numVolcanoes=3;
	else if(lIndex==2)
		numVolcanoes=4;
	else
		numVolcanoes=5;
	for(i=0;i<numVolcanoes;i++)
		PlantMonster2(GUY_VOLCANO,GetRand(MAP_WIDTH),GetRand(MAP_HEIGHT/2)+2);
}

void InitGlobLevel(void)
{
	switch(lIndex)
	{
		case 0:
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2,MAP_HEIGHT/4);
			break;
		case 1:
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2-8,MAP_HEIGHT/4);
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2+8,MAP_HEIGHT/4);
			break;
		case 2:
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2-8,MAP_HEIGHT/4);
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2+8,MAP_HEIGHT/4);
			break;
		case 3:
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2-8,MAP_HEIGHT/4);
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2+8,MAP_HEIGHT/4);
			PlantMonster2(GUY_EYEBALL1,MAP_WIDTH/2,MAP_HEIGHT/2);
			break;
	}
}

void InitAlienQueen(void)
{
	int i;

	MakeQueen();
	bonus=0;	// not a bonus level
	for(i=0;i<20;i++)
		PlantShroom(MAP_WIDTH/2-6+GetRand(13),MAP_HEIGHT/3+4+GetRand(5));
}

void SetupLevel(byte lvl)
{
	int i;
	levelDef_t *lDef;

	StopSong();

	wIndex=((lvl-1)/5);
	lIndex=(lvl-1)-(wIndex*5);

	curLvl=lvl;
	curWorld=wIndex+1;

	lDef=&world[wIndex].level[lIndex];

	SetInvincible(0);

	// always reset shrooms on first level and bonus level of each world
	if(lIndex>0 && lIndex<4 && GetShroomReset()==0 && lDef->shroomType==GUY_SHROOM)
		InitGuysLeaveShrooms(lDef->shroomType);
	else
		InitGuys();

	InitMission();
	SetBackScroll(lDef->backscroll);	// most levels have non-scrolling backgrounds
	GravitySwitch(lDef->gravity);	// most levels have gravity

	if(lIndex<4 || wIndex==9)	// last world has no bonus, just a final Eddie level
	{
		strcpy(name,world[wIndex].name);
		bonus=0;
	}
	else
		bonus=1;

	// player is the same every level
	AddGuy(GUY_PLAYER,MAP_WIDTH/2,MAP_HEIGHT-2,0);
	ClearParticles();
	
	if(lDef->cent1size)
	{
		i=lDef->cent1speed;
		if(GetDifficulty()==DIFF_EASY)
			i-=2*FIXAMT;
		else if(GetDifficulty()==DIFF_HARD)
			i+=2*FIXAMT;
		MakeCentipede(lDef->cent1x,-1,i,lDef->cent1size);
	}
	if(lDef->cent2size)
	{
		i=lDef->cent2speed;
		if(GetDifficulty()==DIFF_EASY)
			i-=2*FIXAMT;
		else if(GetDifficulty()==DIFF_HARD)
			i+=2*FIXAMT;
		MakeCentipede(lDef->cent2x,-1,i,lDef->cent2size);
	}

	if(lDef->levelInit)
		lDef->levelInit();

	// if shrooms are to be reset each level, or this is the first level of the world,
	// or these aren't ordinary shrooms
	if(GetShroomReset()>0 || lIndex==0 || lDef->shroomType!=GUY_SHROOM)
	{
		if(lDef->shroomType==GUY_SHROOM)
			for(i=0;i<lDef->numShrooms;i++)
			{
				PlantShroom(GetRand(MAP_WIDTH),GetRand(MAP_HEIGHT-MAP_HEIGHT/4));
			}
		else
			for(i=0;i<lDef->numShrooms;i++)
			{
				PlantMonster(lDef->shroomType,GetRand(MAP_WIDTH),GetRand(MAP_HEIGHT-MAP_HEIGHT/4));
			}
	}
	for(i=0;i<4;i++)
		clock[i]=lDef->clock[i];
	value[0]=lDef->value[0];
	value[1]=lDef->value[1];

	if(lIndex==0)
		DoIntro(curWorld);

	levelClock=lDef->lvlclock;
	if(lDef->lvlclock==0)
		levelMode=LVL_BATTLE;
	else
		levelMode=LVL_TIMED;

	GameLevelSetup(name,lIndex+1);
	LoadBackgd(lDef->backgd);

#ifndef DEMO
	if(wIndex!=9 || lIndex!=4)
		PlaySong(SND_SONG1+lIndex);
	else
		PlaySong(SND_SONG1+7);		// for the final battle, play the final music
#else
	switch(lIndex)
	{
		case 0:
			PlaySong(SND_SONG1);
			break;
		case 1:
			PlaySong(SND_SONG1+2);
			break;
		case 2:
			PlaySong(SND_SONG1+3);
			break;
		case 3:
			PlaySong(SND_SONG1);
			break;
		case 4:
			PlaySong(SND_SONG1+2);
			break;
	}
#endif
}

byte IsBonusStage(void)
{
	return bonus;
}

void StartTicking(int clock)
{
	levelMode=LVL_TIMED;
	levelClock=clock;
}

byte LevelMode(void)
{
	return levelMode;
}

byte TimeIsUp(void)
{
	if(levelClock)
	{
		levelClock--;
		return 0;
	}
	else
		return 1;
}

//--------------------------------------------------------------------------------
// Level updaters

void UpdateLevelEarth(void)
{
	switch(lIndex)
	{
		case 0:
			// level 1 has no events
			break;
		case 1:
		case 2:
		case 3:
			if(clock[0])
			{
				clock[0]--;
				if(clock[0]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_SPIDER,-1,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
					else
						AddGuy(GUY_SPIDER,MAP_WIDTH,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
				}
			}
			break;
	}
}

void UpdateLevelRigel(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
			if(clock[1])	// snowman timer
			{
				clock[1]--;
				if(!clock[1])
				{
					AddGuy(GUY_SNOWMAN,GetRand(MAP_WIDTH),-1,FIXAMT*6+(FIXAMT*4*(curLvl==6)));
				}
			}
		case 0:	// level 6 lacks snowmen
			if(clock[0])	// spider timer
			{
				clock[0]--;
				if(clock[0]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_ICESPIDER,-1,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
					else
						AddGuy(GUY_ICESPIDER,MAP_WIDTH,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
				}
			}
			break;
	}
}

void UpdateLevelAgua(void)
{
	switch(lIndex)
	{
		case 2:
		case 3:
			if(clock[3])	// strider #2 timer!!
			{
				clock[3]--;
				if(clock[3]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_STRIDER,-1,MAP_HEIGHT-2-GetRand(3),FIXAMT*6);
					else
						AddGuy(GUY_STRIDER,MAP_WIDTH,MAP_HEIGHT-2-GetRand(3),FIXAMT*6);
				}
			}
		case 1:
			if(clock[2])	// fly timer
			{
				clock[2]--;
				if(clock[2]==0)
				{
					AddGuy(GUY_FLY,GetRand(MAP_WIDTH),-1,0);
					clock[2]=120+GetRand(240);
				}
			}
		case 0:
			if(clock[1])	// frog timer
			{
				clock[1]--;
				if(clock[1]==0)
				{
					AddGuy(GUY_FROG,GetRand(MAP_WIDTH),-GetRand(4),FIXAMT*6);
				}
			}
			if(clock[0])	// strider timer
			{
				clock[0]--;
				if(clock[0]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_STRIDER,-1,MAP_HEIGHT-2-GetRand(3),FIXAMT*6);
					else
						AddGuy(GUY_STRIDER,MAP_WIDTH,MAP_HEIGHT-2-GetRand(3),FIXAMT*6);
				}
			}
			break;
	}
}

void UpdateLevelPrometheus(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
			if(clock[1])	// repairbot timer
			{
				clock[1]--;
				if(clock[1]==0)
				{
					AddGuy(GUY_REPAIR,GetRand(MAP_WIDTH),-1,FIXAMT*6);
				}
			}
		case 0:
			if(clock[0])	// spiderbot timer
			{
				clock[0]--;
				if(clock[0]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_SPIDERBOT,-1,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
					else
						AddGuy(GUY_SPIDERBOT,MAP_WIDTH,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
				}
			}
			break;
	}
}

void UpdateLevelTartarus(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
			if(clock[1])	// sparky timer
			{
				clock[1]--;
				if(clock[1]==0)
				{
					AddGuy(GUY_SPARKY,-2,GetRand(MAP_HEIGHT-MAP_HEIGHT/4),FIXAMT*4);
				}
			}
		case 0:
			if(clock[0])	// ant timer
			{
				clock[0]--;
				if(clock[0]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_FIREANT,-1,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
					else
						AddGuy(GUY_FIREANT,MAP_WIDTH,MAP_HEIGHT-2-GetRand(3),FIXAMT*4);
				}
			}
			break;
	}
}

void UpdateLevelJurassic(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
			// stuff level 1 doesn't have
			if(clock[2])
			{
				clock[2]--;
				if(clock[2]==0)
				{
					AddGuy(GUY_PTERODACTYL,GetRand(MAP_WIDTH),-3,0);
					clock[2]=120-(lIndex*30)+GetRand(60);
				}
			}
		case 0:
			if(clock[0])	// triceratops timer
			{
				clock[0]--;
				if(clock[0]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_TRICERATOPS,-3,MAP_HEIGHT-1-GetRand(2),FIXAMT*(3+GetRand(4)));
					else
						AddGuy(GUY_TRICERATOPS,MAP_WIDTH+2,MAP_HEIGHT-1-GetRand(2),-FIXAMT*(3+GetRand(4)));
					clock[0]=120-(lIndex*10)+GetRand(60);
				}
			}
			if(clock[1])	// triceratops timer
			{
				clock[1]--;
				if(clock[1]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_TRICERATOPS,-3,MAP_HEIGHT-3-GetRand(3),FIXAMT*(3+GetRand(4)));
					else
						AddGuy(GUY_TRICERATOPS,MAP_WIDTH+2,MAP_HEIGHT-3-GetRand(3),-FIXAMT*(3+GetRand(4)));
					clock[1]=120-(lIndex*10)+GetRand(60);
				}
			}
			break;
	}
}

void UpdateLevelFinal(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
		case 4:
			// stuff level 1 doesn't have
			if(clock[0])	// flying alien timer
			{
				clock[0]--;
				if(clock[0]==0)
				{
					AddGuy(GUY_FLYALIEN,GetRand(MAP_WIDTH),-1,0);
					clock[0]=120+GetRand(240);
				}
			}
		case 0:
			break;
	}
}

void UpdateLevelGlobulus(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
			// stuff level 1 doesn't have
			if(clock[0])
			{
				clock[0]--;
				if(clock[0]==0)
				{
					AddGuy(GUY_EYEGUY,3+GetRand(MAP_WIDTH-6),-2,0);
				}
			}
		case 0:
			// nothing
			break;
	}
}

void UpdateLevelVarakkis(void)
{
	switch(lIndex)
	{
		case 1:
		case 2:
		case 3:
			// stuff level 1 doesn't have
			// sandworm timer
			if(clock[2])
			{
				clock[2]--;
				if(clock[2]==0)
				{
					if(GetRand(2))
						AddGuy(GUY_SANDWORM,0,MAP_HEIGHT-3-GetRand(3),0);
					else
						AddGuy(GUY_SANDWORM,MAP_WIDTH,MAP_HEIGHT-3-GetRand(3),0);
				}
			}
		case 0:
			// tumbleweed timer
			if(clock[1])
			{
				clock[1]--;
				if(clock[1]==0)
				{
					AddGuy(GUY_TUMBLEWEED,GetRand(MAP_WIDTH),-1,0);
				}
			}
			// Scorpion timer
			if(clock[0])
			{
				clock[0]--;
				if(clock[0]==0)
				{
					AddGuy(GUY_SCORPION,GetRand(MAP_WIDTH),-2,FIXAMT*(3+lIndex));
				}
			}
			break;
	}
}

void UpdateLevelAsteroid(void)
{
	// nothing updates
}
void UpdateLevelBonus(void)
{
	switch(bonus)
	{
		case BNS_ASTEROID:
			if(levelClock>5*30)	// for last 5 seconds, no more rocks
			{
				if(GetRand(6-GetWorld()/2+(2-GetDifficulty())*5)==0)
					AddGuy(GUY_BIGROID+GetRand(3),GetRand(MAP_WIDTH),-GetRand(3),0);
				if(GetRand(5)==0 && (levelClock%(3*30)==0))
					AddGuy(GUY_POWERUP,GetRand(MAP_WIDTH),-GetRand(3),0);
			}
			break;
		case BNS_HORDE:
			if(clock[0])
			{
				clock[0]--;
				if(clock[0]==0)
					if(GetRand(2))
						AddGuy(GUY_PWRUPUFO,-3,4,FIXAMT*4);
					else
						AddGuy(GUY_PWRUPUFO,MAP_WIDTH+3,4,FIXAMT*4);
			}
			break;
		case BNS_HOSTILE:
			if(levelClock<5*30)
				return;	// for last 5 seconds, no more monsters

			if(clock[0])
				clock[0]--;
			else
			{
				// launch some new menace, chosen at random
				switch(GetRand(26))
				{
					case 0:
					case 1:
					case 17:
					case 18:
						AddGuy(GUY_SPINSHIP,5,-4,FIXAMT);
						AddGuy(GUY_SPINSHIP,25,-4,-FIXAMT);
						clock[0]=30;
						break;
					case 2:
					case 19:
					case 20:
					case 21:
						AddGuy(GUY_SPINSHIP,5,-4,FIXAMT/2);
						AddGuy(GUY_SPINSHIP,15,-4,0);
						AddGuy(GUY_SPINSHIP,25,-4,-FIXAMT/2);
						clock[0]=30;
						break;
					case 3:
					case 22:
					case 23:
						AddGuy(GUY_SPINSHIP,30,-4,-FIXAMT/2);
						AddGuy(GUY_SPINSHIP,28,-5,-FIXAMT/2);
						AddGuy(GUY_SPINSHIP,26,-6,-FIXAMT/2);
						AddGuy(GUY_SPINSHIP,24,-7,-FIXAMT/2);
						AddGuy(GUY_SPINSHIP,22,-8,-FIXAMT/2);
						clock[0]=90;
						break;
					case 4:
					case 24:
					case 25:
						AddGuy(GUY_SPINSHIP,0,-4,FIXAMT/2);
						AddGuy(GUY_SPINSHIP,2,-5,FIXAMT/2);
						AddGuy(GUY_SPINSHIP,4,-6,FIXAMT/2);
						AddGuy(GUY_SPINSHIP,6,-7,FIXAMT/2);
						AddGuy(GUY_SPINSHIP,8,-8,FIXAMT/2);
						clock[0]=90;
						break;
					case 5:
					case 6:
						AddGuy(GUY_CLAWSHIP,GetRand(MAP_WIDTH-2)+1,-4,30);
						clock[0]=120;
						break;
					case 7:
						AddGuy(GUY_CLAWSHIP,MAP_WIDTH/5,-4,60);
						AddGuy(GUY_CLAWSHIP,(2*MAP_WIDTH)/5,-4,70);
						AddGuy(GUY_CLAWSHIP,(3*MAP_WIDTH)/5,-4,80);
						AddGuy(GUY_CLAWSHIP,(4*MAP_WIDTH)/5,-4,90);
						clock[0]=140;
						break;
					case 8:
					case 9:
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/2,-2,GetRand(2));
						clock[0]=20;
						break;
					case 10:
					case 11:
					case 12:
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/4,-2,1);
						AddGuy(GUY_SMALLSHIP,3*MAP_WIDTH/4,-3,0);
						clock[0]=20;
						break;
					case 13:
					case 14:
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/2,-2,0);
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/2,-4,1);
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/2,-6,0);
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/2,-8,1);
						clock[0]=30;
						break;
					case 15:
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/4,-2,1);
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/4+2,-4,1);
						AddGuy(GUY_SMALLSHIP,MAP_WIDTH/4+4,-6,1);
						AddGuy(GUY_SMALLSHIP,3*MAP_WIDTH/4,-1,0);
						AddGuy(GUY_SMALLSHIP,3*MAP_WIDTH/4-2,-3,0);
						AddGuy(GUY_SMALLSHIP,3*MAP_WIDTH/4-4,-5,0);
						clock[0]=60;
						break;
					case 16:
						if(GetRand(2))
							AddGuy(GUY_PWRUPUFO,-3,4,FIXAMT*4);
						else
							AddGuy(GUY_PWRUPUFO,MAP_WIDTH+3,4,FIXAMT*4);
						clock[0]=10;
						break;
				}
				clock[0]+=(2-GetDifficulty())*10;	// lower difficulties have enemies less often
			}
			break;
		default:
			break;
	}
}

void UpdateLevel(void)
{
	if(bonus)
		UpdateLevelBonus();
	else
	{
		// this triggers spiders, etc.
		switch(curWorld)
		{
			case WORLD_EARTH:
				UpdateLevelEarth();
				break;
			case WORLD_RIGEL:
				UpdateLevelRigel();
				break;
			case WORLD_AGUA:
				UpdateLevelAgua();
				break;
			case WORLD_PROMETHEUS:
				UpdateLevelPrometheus();
				break;
			case WORLD_TARTARUS:
				UpdateLevelTartarus();
				break;
			case WORLD_ASTEROID:
				UpdateLevelAsteroid();
				break;
			case WORLD_JURASSIC:
				UpdateLevelJurassic();
				break;
			case WORLD_GLOBULUS:
				UpdateLevelGlobulus();
				break;
			case WORLD_VARAKKIS:
				UpdateLevelVarakkis();
				break;
			case WORLD_FINAL:
				UpdateLevelFinal();
				break;
			default:
				break;
		}
	}
}

//----------------------------------------------------------------------------------
// TellXXXAboutIt

void TellEarthAboutIt(byte event,byte val)
{
	if(event==EVT_MONSDIE && val==GUY_SPIDER)
	{
		clock[0]=30+GetRand(120);
	}
}

void TellRigelAboutIt(byte event,byte val)
{
	if(event==EVT_MONSDIE && val==GUY_ICESPIDER)
	{
		clock[0]=30+GetRand(120);
	}
	if((event==EVT_MONSDIE || event==EVT_MONSGONE) && val==GUY_SNOWMAN)
	{
		if(lIndex==1)
			clock[1]=60+GetRand(160);
		else
			clock[1]=30+GetRand(100);
	}
}

void TellAguaAboutIt(byte event,byte val)
{
	if(event==EVT_MONSDIE && val==GUY_STRIDER)
	{
		if(lIndex==2 || lIndex==3)
		{
			if(clock[0])
				clock[3]=30+GetRand(120);
			else
				clock[0]=30+GetRand(120);
		}
		else
			clock[0]=30+GetRand(120);
	}
	if((event==EVT_MONSDIE || event==EVT_MONSGONE) && val==GUY_FROG)
	{
		clock[1]=60+GetRand(240);
	}
}

void TellPrometheusAboutIt(byte event,byte val)
{
	if(event==EVT_MONSDIE && val==GUY_SPIDERBOT)
	{
		clock[0]=30+GetRand(120);
	}
	if((event==EVT_MONSDIE || event==EVT_MONSGONE) && val==GUY_REPAIR)
	{
		clock[1]=60+GetRand(240);
	}
}

void TellTartarusAboutIt(byte event,byte val)
{
	if((event==EVT_MONSDIE || event==EVT_MONSGONE) && val==GUY_SPARKY)
	{
		clock[1]=60+GetRand(240);
	}
	if(event==EVT_MONSDIE && val==GUY_FIREANT)
	{
		clock[0]=30+GetRand(120);
	}
}

void TellJurassicAboutIt(byte event,byte val)
{
	// nothing of interest yet
}

void TellGlobulusAboutIt(byte event,byte val)
{
	if(event==EVT_MONSDIE && val==GUY_EYEGUY)
	{
		clock[0]=30+GetRand(120);
	}
}

void TellVarakkisAboutIt(byte event,byte val)
{
	if((event==EVT_MONSDIE || event==EVT_MONSGONE) && val==GUY_SCORPION)
	{
		clock[0]=80+GetRand(120)+80*(lIndex<2);
	}
	if((event==EVT_MONSDIE || event==EVT_MONSGONE) && val==GUY_TUMBLEWEED)
	{
		clock[1]=60+GetRand(60);
	}
	if(event==EVT_MONSDIE && val==GUY_SANDWORM)
	{
		clock[2]=60+GetRand(60);
	}
}

void TellAsteroidAboutIt(byte event,byte val)
{
	// nothing of interest
}

void TellBonusAboutIt(byte event,byte val)
{
	switch(bonus)
	{
		case BNS_ASTEROID:
			break;
		case BNS_HORDE:
			if(event==EVT_MONSDIE && (val==GUY_SQUIDSHIP || val==GUY_DOUBLESHIP || val==GUY_FBALLSHIP))
				clock[1]++;
			if(clock[1]>4)
			{
				// summon a powerup ship for taking out the monsters
				clock[1]=0;
				clock[0]=2;
			}
			break;
	}
}

void TellLevelAboutIt(byte event,byte value)
{
	TellMissionAboutIt(event,value);
	if(lIndex==4)	// on a bonus stage
	{
		TellBonusAboutIt(event,value);
		return;
	}
	switch(curWorld)
	{
		case WORLD_EARTH:
			TellEarthAboutIt(event,value);
			break;
		case WORLD_RIGEL:
			TellRigelAboutIt(event,value);
			break;
		case WORLD_AGUA:
			TellAguaAboutIt(event,value);
			break;
		case WORLD_PROMETHEUS:
			TellPrometheusAboutIt(event,value);
			break;
		case WORLD_TARTARUS:
			TellTartarusAboutIt(event,value);
			break;
		case WORLD_ASTEROID:
			TellAsteroidAboutIt(event,value);
			break;
		case WORLD_JURASSIC:
			TellJurassicAboutIt(event,value);
			break;
		case WORLD_GLOBULUS:
			TellGlobulusAboutIt(event,value);
			break;
		case WORLD_VARAKKIS:
			TellVarakkisAboutIt(event,value);
			break;
		default:
			break;
	}
}
