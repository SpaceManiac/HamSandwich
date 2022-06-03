#include "mission.h"
#include "player.h"
#include "guy.h"
#include "level.h"
#include "random.h"
#include "status.h"
#include <stdio.h>
#include "sound.h"
#include "display.h"
#include "tile.h"
#include "options.h"

#define MISSION_NONE 0
#define MISSION_KILL 1	// kill X number of a type of monster
#define MISSION_SIT	 2	// don't shoot anything for X time
#define MISSION_WIN	 3
#define MISSION_PRESENTS 4	// collect presents

typedef struct mission_t
{
	byte type;
	char name[16];
	byte target;
	byte min,max;	// min and max # of targets
	dword clock;	// avg. # of milliseconds per target
	byte level1;	// can this mission be given on level 1 of the world?

} mission_t;

#define EARTH_MISSIONS 4
mission_t earthMissions[]={
		{MISSION_KILL,"Mushrooms",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Spiders",GUY_SPIDER,1,5,3000,0},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define RIGEL_MISSIONS 5
mission_t rigelMissions[]={
		{MISSION_KILL,"Icebergs",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Spiders",GUY_ICESPIDER,1,5,3000,1},
		{MISSION_KILL,"Snowmen",GUY_SNOWMAN,1,4,4000,0},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define AGUA_MISSIONS 6
mission_t aguaMissions[]={
		{MISSION_KILL,"Lilypads",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Striders",GUY_STRIDER,2,5,3000,1},
		{MISSION_KILL,"Frogs",GUY_FROG,2,6,4000,0},
		{MISSION_KILL,"Flies",GUY_FLY,2,5,4000,0},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define PROM_MISSIONS 6
mission_t promMissions[]={
		{MISSION_KILL,"Computers",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Turrets",GUY_TURRET,1,3,3000,1},
		{MISSION_KILL,"Repairbots",GUY_REPAIR,1,3,4000,0},
		{MISSION_KILL,"Spiderbots",GUY_SPIDERBOT,2,5,3000,1},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define TART_MISSIONS 6
mission_t tartMissions[]={
		{MISSION_KILL,"Rocks",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Volcano",GUY_VOLCANO,1,1,5000,1},
		{MISSION_KILL,"Sparkies",GUY_SPARKY,2,5,3500,0},
		{MISSION_KILL,"Fire Ants",GUY_FIREANT,2,5,3000,1},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define VARAK_MISSIONS 6
mission_t varakMissions[]={
		{MISSION_KILL,"Cacti",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Tumblers",GUY_TUMBLEWEED,1,3,5000,1},
		{MISSION_KILL,"Scorpions",GUY_SCORPION,2,5,3500,1},
		{MISSION_KILL,"Sandworm",GUY_SANDWORM,1,1,6000,0},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define JURAS_MISSIONS 6
mission_t jurasMissions[]={
		{MISSION_KILL,"Palms",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Triceroids",GUY_TRICERATOPS,2,6,6000,1},
		{MISSION_KILL,"Pterrors",GUY_PTERODACTYL,2,5,3500,0},
		{MISSION_KILL,"Volcano",GUY_VOLCANO,1,1,5000,1},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define GLOB_MISSIONS 4
mission_t globMissions[]={
		{MISSION_KILL,"Bubbles",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Eye Guy",GUY_EYEGUY,1,1,6000,0},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

#define FINAL_MISSIONS 6
mission_t finalMissions[]={
		{MISSION_KILL,"Eggs",GUY_SHROOM,2,8,1800,1},
		{MISSION_KILL,"Squirmies",GUY_BABYALIEN,2,6,3000,1},
		{MISSION_KILL,"Fliers",GUY_FLYALIEN,1,4,3000,0},
		{MISSION_KILL,"Alienpede",GUY_ALIENHEAD,1,1,2000,0},
		{MISSION_PRESENTS,"",GUY_PRESENT,4,10,500,1},
		{MISSION_SIT,"",0,0,0,8000,1}
	};

byte curMission;
byte curTarget;	// the monster type # you must kill
byte numTargets;
dword missionTimer;
int targetX,targetY;
int specificTarget;
int leftX,rightX,topY,bottomY;

void InitMission(void)
{
	curMission=MISSION_NONE;
	MonitorSwitch(0);
	specificTarget=-1;
}

void AssignSpecificMission(mission_t *m,byte max)
{
	int i;
	int count;
	dword timeShift;
	char s[32];

	curMission=MISSION_NONE;
	while(curMission==MISSION_NONE)
	{
		i=GetRand(max);
		if(GetLevel()!=1+(GetWorld()-1)*5 || (m[i].level1))
		{
			curMission=m[i].type;
			curTarget=m[i].target;
			if(m[i].max>0)
			{
				if(m[i].max>m[i].min)
					numTargets=m[i].min+GetRand(m[i].max-m[i].min);
				else
					numTargets=m[i].min;
			}
			else
				numTargets=0;
			if(numTargets>0)
				count=numTargets;
			else
				count=1;

			timeShift=(count*m[i].clock)/8;

			// the time alloted may be anywhere from 1/8 less than the right amount to 1/8 more
			missionTimer=timeShift+GetLRand(timeShift*2)+(count*m[i].clock);

			// add bonus time for easier difficulty settings
			// unless it is a Hold Fire mission, in which case, subtract time
			if(curMission!=MISSION_SIT)
				missionTimer+=(count*m[i].clock/4)*(2-GetDifficulty());
			else
				missionTimer-=(count*m[i].clock/4)*(2-GetDifficulty());

			missionTimer=(missionTimer*30)/1000;

			SetMonitorText(1,"");
			if(curMission==MISSION_SIT)
			{
				SetMonitorText(2,"    Hold your fire!");
			}
			else if(curMission==MISSION_PRESENTS)
			{
				SetMonitorText(2,"    Scavenger hunt!");
			}
			else
			{
				sprintf(s,"   Destroy %d %s.",numTargets,m[i].name);
				SetMonitorText(2,s);
			}
			SetMonitorText(3,"");
			SetMonitorText(4,"");
		}
	}
}

void AssignMission(void)
{
	MakeSound(SND_MISSION,1600);
	SetMonitorText(0,"    Special Mission");
	switch(GetWorld())
	{
		case WORLD_EARTH:
			AssignSpecificMission(earthMissions,EARTH_MISSIONS);
			break;
		case WORLD_RIGEL:
			AssignSpecificMission(rigelMissions,RIGEL_MISSIONS);
			break;
		case WORLD_AGUA:
			AssignSpecificMission(aguaMissions,AGUA_MISSIONS);
			break;
		case WORLD_PROMETHEUS:
			AssignSpecificMission(promMissions,PROM_MISSIONS);
			break;
		case WORLD_TARTARUS:
			AssignSpecificMission(tartMissions,TART_MISSIONS);
			break;
		case WORLD_VARAKKIS:
			AssignSpecificMission(varakMissions,VARAK_MISSIONS);
			break;
		case WORLD_GLOBULUS:
			AssignSpecificMission(globMissions,GLOB_MISSIONS);
			break;
		case WORLD_JURASSIC:
			AssignSpecificMission(jurasMissions,JURAS_MISSIONS);
			break;
		case WORLD_FINAL:
			AssignSpecificMission(finalMissions,FINAL_MISSIONS);
			break;
	}
	if(curMission==MISSION_PRESENTS)
		PutPresents(numTargets);

	targetX=0;
	targetY=0;
	leftX=0;
	rightX=SCR_WIDTH;
	topY=0;
	bottomY=SCR_HEIGHT;
	specificTarget=-1;
	
	MonitorSwitch(1);
}

void MissionOver(void)
{
	char s[32];

	sprintf(s,">%d<            t-00.0",numTargets);
	SetMonitorText(4,s);
	SetMonitorText(0,"    Mission Failed!");
	curMission=MISSION_WIN;
	MakeSound(SND_LOSEMISSION,1600);
	missionTimer=30;
	specificTarget=-1;
}

void MissionWin(byte cause)
{
	int reward;

	specificTarget=-1;

	if(cause==0)	// actually won a mission
	{
		MakeSound(SND_WINMISSION,1600);
		SetMonitorText(0,"  Mission Accomplished!");
	}
	else if(cause==1)	// cheated
	{
		MakeSound(SND_CHEAT,1600);
		SetMonitorText(0,"  Fully Just Cheated!");
	}
	else if(cause==2)	// picked up a space powerup
	{
		MakeSound(SND_POWERUP,1600);
		SetMonitorText(0,"  Power-Up Obtained!");
	}

	SetMonitorText(1,"");
	curMission=MISSION_WIN;
	missionTimer=60;
	// this just makes it sit on for another second displaying your prize
	if(cause!=1)	// no points for a cheat
		PlayerGetPoints(500);	// 500 pts for passing any mission

	SetMonitorText(4,"");

	while(1)
	{
		reward=GetRand(14);
		switch(reward)
		{
			case 0:
				SetWeapon(WPN_MISSILE,20);
				SetMonitorText(2," Got Guided Missile!");
				return;
				break;
			case 1:
				SetWeapon(WPN_SHOTGUN,10);
				SetMonitorText(2," Got Shotgun!");
				return;
				break;
			case 2:
				SetWeapon(WPN_CHAINGUN,30);
				SetMonitorText(2," Got Autocannon!");
				return;
				break;
			case 3:
				SetWeapon(WPN_LASER,5);
				SetMonitorText(2," Got Neutron Beam!");
				return;
				break;
			case 4:
				SetWeapon(WPN_NUKE,1);
				SetMonitorText(2," Got Nuclear Warhead!");
				return;
				break;
			case 5:
				if(!GetShield())
				{
					SetShield(1);
					SetMonitorText(2," Got Energy Shield!");
					return;
				}
				break;
			case 6:
				if(GetLives()<99)	// 99 life maximum, which no one will ever notice
				{
					AddLife(1);
					SetMonitorText(2," Got Bonus Life!");
					return;
				}
				break;
			case 7:
				if(GetWingmen()<2)
				{
					GiveWingman();
					SetMonitorText(2," Got Wingman!");
					return;
				}
				break;
			default:	// 8,9,10,11,12,13
				if(GetPower()<2)
				{
					BoostPower();
					SetMonitorText(2," Got Power Boost!");
					return;
				}
				break;
		}
	}
}

void UpdateMission(void)
{
	char s[32];
	int oldTarget;

	if(curMission==MISSION_NONE)
		return;

	if(curMission==MISSION_KILL)
	{
		oldTarget=specificTarget;
		specificTarget=FindTarget(curTarget,&targetX,&targetY);
		if(oldTarget!=specificTarget && specificTarget!=-1)
		{
			leftX=targetX-64;
			rightX=targetX+64;
			topY=targetY-64;
			bottomY=targetY+64;
		}
		if(targetX || targetY)
		{
			if(leftX<targetX)
				leftX+=8;
			if(leftX>targetX)
				leftX=targetX;
			if(rightX>targetX)
				rightX-=8;
			if(rightX<targetX)
				rightX=targetX;
			if(topY<targetY)
				topY+=8;
			if(topY>targetY)
				topY=targetY;
			if(bottomY>targetY)
				bottomY-=8;
			if(bottomY<targetY)
				bottomY=targetY;
		}
		else
		{
			topY=0;
			bottomY=SCR_HEIGHT;
			leftX=0;
			rightX=SCR_WIDTH;
		}
	}
	else
	{
		topY=0;
		bottomY=SCR_HEIGHT;
		leftX=0;
		rightX=SCR_WIDTH;
	}

	missionTimer--;
	if(!missionTimer)
	{
		// time's up
		if(curMission==MISSION_SIT)
		{
			MissionWin(0);
			return;
		}
		if(curMission==MISSION_WIN)
		{
			MonitorSwitch(0);
			curMission=MISSION_NONE;
			return;
		}
		MissionOver();
		return;
	}
	else if(curMission!=MISSION_WIN)
	{
		sprintf(s,">%d<            t-%02.1f",numTargets,(float)missionTimer/30);
		SetMonitorText(4,s);
	}
}

void TellMissionAboutIt(byte event,byte value)
{
	if(curMission==MISSION_SIT && event==EVT_FIRE)
	{
		// not allowed to shoot!
		MissionOver();
		return;
	}
	if(curMission==MISSION_KILL && event==EVT_MONSDIE && value==curTarget)
	{
		MakeSound(SND_MISSIONGOOD,1600);
		numTargets--;
		if(numTargets==0)	// YAY
		{
			MissionWin(0);
		}
	}
	if(curMission==MISSION_PRESENTS && event==EVT_GET && value==curTarget)
	{
		MakeSound(SND_MISSIONGOOD,1600);
		numTargets--;
		if(numTargets==0)	// YAY
		{
			MissionWin(0);
		}
	}
}

byte NoMission(void)
{
	return (curMission==MISSION_NONE);
}

void RenderMissionTarget(void)
{
	if(specificTarget!=-1)
	{
		GlowSprite(SPR_STATUS,21,targetX,targetY,0);
		GlowSprite(SPR_STATUS,22,rightX,targetY,0);
		GlowSprite(SPR_STATUS,23,targetX,bottomY,0);
		GlowSprite(SPR_STATUS,24,leftX,targetY,0);
		GlowSprite(SPR_STATUS,25,targetX,topY,0);
	}
}