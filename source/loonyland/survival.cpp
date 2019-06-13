#include "survival.h"
#include "monster.h"
#include "message.h"
#include "player.h"
#include "items.h"
#include "badge.h"
#include "log.h"

#define  SURV_LEVELS 25

typedef struct surv_t
{
	char name[32];
	byte mons[10];
} surv_t;

byte survLevel;
byte survWin;

surv_t sLevel[SURV_LEVELS]={
	{"Frog Frenzy", // #1
	{MONS_FROG,MONS_FROG,MONS_FROG,MONS_FROG,MONS_FROG,
	 MONS_FROG,MONS_FROG,MONS_FROG,MONS_FROG,MONS_FROG}
	},
	{"Flaming Frogs!", // #2
	{MONS_FROG4,MONS_FROG3,MONS_FROG3,MONS_FROG3,MONS_FROG4,
	 MONS_FROG3,MONS_FROG4,MONS_FROG3,MONS_FROG4,MONS_FROG3}
	},
	{"Terror Glade Revisited", // #3
	{MONS_EVILTREE,0,MONS_EVILTREE,0,MONS_EVILTREE,
	 0,MONS_EVILTREE,0,MONS_EVILTREE,0}
	},
	{"Boney Maroney", // #4
	{MONS_BONEHEAD,0,0,MONS_BONEHEAD,0,
	 0,0,MONS_BONEHEAD,MONS_BONEHEAD,0}
	},
	{"Braaiinnss!!", // #5
	{MONS_ZOMBIE,0,0,MONS_ZOMBIE,0,
	 0,0,MONS_ZOMBIE,0,MONS_ZOMBIE}
	},
	{"TERROR Glade", // #6
	{MONS_EVILTREE,MONS_EVILTREE,MONS_EVILTREE,MONS_EVILTREE,MONS_EVILTREE3,
	 MONS_EVILTREE,MONS_EVILTREE,MONS_EVILTREE3,MONS_EVILTREE,MONS_EVILTREE}
	},
	{"Ding Dong", // #7
	{MONS_BAT,MONS_BAT,MONS_BAT,MONS_BAT,MONS_BAT,
	 MONS_BAT,MONS_BAT,MONS_BAT,MONS_BAT,MONS_BAT}
	},
	{"The Boneyard", // #8
	{MONS_BONEHEAD,MONS_BONEHEAD2,MONS_BONEHEAD3,MONS_BONEHEAD,MONS_BONEHEAD,
	 MONS_BONEHEAD,MONS_BONEHEAD2,MONS_BONEHEAD3,MONS_BONEHEAD,MONS_BONEHEAD}
	},
	{"Poison Jam", // #9
	{MONS_BONEHEAD3,MONS_FROG2,MONS_FROG2,MONS_BAT4,0,
	 MONS_BONEHEAD3,MONS_FROG2,MONS_FROG2,MONS_BAT4,0}
	},
	{"Ragtime", // #10
	{MONS_MUMMY,MONS_MUMMY,MONS_MUMMY,MONS_MUMMY2,MONS_MUMMY3,
	 MONS_MUMMY,MONS_MUMMY,MONS_MUMMY,MONS_MUMMY2,MONS_MUMMY4}
	},
	{"Howlin' Good Time", // #11
	{MONS_WOLFMAN,MONS_WOLFMAN,MONS_WOLFMAN2,MONS_WOLFMAN,MONS_WOLFMAN3,
	 MONS_WOLFMAN,MONS_WOLFMAN,MONS_WOLFMAN2,MONS_WOLFMAN,MONS_WOLFMAN2}
	},
	{"The Spook Zone", // #12
	{MONS_GHOST,MONS_GHOST,MONS_GHOST,MONS_GHOST2,MONS_GHOST3,
	 MONS_GHOST,MONS_GHOST,MONS_GHOST,MONS_GHOST2,0}
	},
	{"Ka-boom!", // #13
	{MONS_ZOMBIE2,MONS_ZOMBIE2,MONS_ZOMBIE2,MONS_ZOMBIE2,MONS_ZOMBIE2,
	 MONS_ZOMBIE2,MONS_ZOMBIE2,MONS_ZOMBIE2,MONS_ZOMBIE2,MONS_ZOMBIE2}
	},
	{"Gone Batty", // #14
	{MONS_BAT3,MONS_BAT3,MONS_BAT3,MONS_BAT3,MONS_BAT3,
	 MONS_BAT3,MONS_BAT3,MONS_BAT3,MONS_BAT3,MONS_BAT3}
	},
	{"Vamp Attack", // #15
	{MONS_VAMPIRE,MONS_VAMPIRE2,MONS_VAMPIRE,MONS_VAMPIRE2,MONS_VAMPIRE2,
	 MONS_VAMPIRE,MONS_VAMPIRE2,MONS_VAMPIRE,MONS_VAMPIRE2,MONS_VAMPIRE4}
	},
	{"Swamp Stomp", // #16
	{MONS_SWAMPDOG,MONS_SWAMPDOG,MONS_SWAMPDOG2,MONS_SWAMPDOG3,MONS_SWAMPDOG2,
	 MONS_SWAMPDOG,MONS_SWAMPDOG,MONS_SWAMPDOG2,MONS_SWAMPDOG3,MONS_SWAMPDOG4}
	},
	{"Dem Bones", // #17
	{MONS_BONEHEAD2,MONS_BONEHEAD2,MONS_BONEHEAD2,MONS_BONEHEAD3,MONS_BONEHEAD4,
	 MONS_BONEHEAD2,MONS_BONEHEAD2,MONS_BONEHEAD2,MONS_BONEHEAD4,MONS_BONEHEAD5}
	},
	{"Pumpkin Patch", // #18
	{MONS_PUMPKIN,MONS_PUMPKIN3,MONS_PUMPKIN,MONS_PUMPKIN3,MONS_PUMPKIN2,
	 MONS_PUMPKIN,MONS_PUMPKIN2,MONS_PUMPKIN,MONS_PUMPKIN2,MONS_PUMPKIN3}
	},
	{"Hands-On", // #19
	{MONS_HAND,MONS_HAND,0,MONS_HAND,0,
	 MONS_HAND,MONS_HAND,0,MONS_HAND,0}
	},
	{"They're Here", // #20
	{MONS_GHOST3,MONS_GHOST2,MONS_GHOST2,MONS_GHOST3,MONS_GHASTLY,
	 MONS_GHOST3,MONS_GHOST2,MONS_GHOST2,MONS_GHOST3,MONS_GHOST}
	},
	{"The Factory", // #21
	{MONS_BHOMATIC,MONS_BHOMATIC,MONS_BHOMATIC,MONS_RBOMATIC,MONS_GGOMATIC,
	 MONS_BHOMATIC,MONS_BHOMATIC,MONS_GGOMATIC,MONS_RBOMATIC,MONS_NSOMATIC}
	},
	{"Wolf It Down", // #22
	{MONS_WOLFMAN3,MONS_WOLFMAN3,MONS_WOLFMAN3,0,MONS_HARRY,
	 MONS_WOLFMAN3,MONS_WOLFMAN3,MONS_WOLFMAN3,0,MONS_HARRY}
	},
	{"Violet Violence", // #23
	{MONS_SWAMPDOG4,MONS_MUMMY5,MONS_ZOMBIE3,MONS_BONEHEAD5,MONS_FROG2,
	 MONS_VAMPIRE4,MONS_ZOMBIE3,MONS_VAMPIRE4,0,MONS_ZOMBIE3}
	},
	{"It's Alive!!", // #24
	{MONS_FRANKENJ,0,0,0,MONS_MINIFRANK,
	 0,0,0,0,MONS_MINIFRANK}
	},
	{"Respect Your Elders", // #25
	{MONS_ELDER,MONS_VAMPIRE4,MONS_VAMPIRE4,MONS_VAMPIRE4,MONS_MECHABONK,
	 MONS_VAMPIRE3,MONS_VAMPIRE3,MONS_VAMPIRE3,MONS_VAMPIRE3,MONS_VAMPIRE3}
	},
};

void InitSurvival(void)
{
	survLevel=0;
	player.numSaves=survLevel;
}

void InfSurvMonsters(int level,Map *map,world_t *world)
{
	byte badTable[]={
		MONS_NONE,MONS_LOONY,MONS_LOONYTOAD,MONS_BIGGHOST,
		MONS_LARRY,MONS_HUMANLARRY,MONS_HELPERBAT,MONS_VILLAGER,MONS_VILLAGER2,
		MONS_VILLAGER3,MONS_VILLAGER4,MONS_VILLAGER5,MONS_VILLAGER6,
		MONS_ELDER2,MONS_ELDER3,MONS_ELDER4,MONS_TUMBLEROCK,
		MONS_TUMBLEROCK2,MONS_TUMBLEROCK3,MONS_TUMBLEROCK4,MONS_LIGHTBALL,
		MONS_LIGHTBALL2,MONS_LIGHTBALL3,MONS_LIGHTBALL4,MONS_LIGHTBALL5,
		MONS_EVILTREE,MONS_EVILTREE2,MONS_BUBBLE,MONS_JUNK,
		MONS_JUNK2,MONS_JUNK3,MONS_PLYRBONKULA,MONS_PLYRSWAMPDOG,MONS_PLYRWITCH,
		MONS_BONKULA,MONS_EVILIZER,MONS_PLYRWOLF,MONS_LOONY2,
		MONS_PLYRSUMMON,MONS_PLYRTHIEF,MONS_SUMBOMBIE,MONS_SUMGHOST,MONS_SUMWOLF,MONS_SUMBAT,
		MONS_SUMMUMMY,MONS_SUMFROG,MONS_SUMDOG,255,
	};

	int amount;
	int x,y;
	byte myType;
	int i;

	DBG("ISM");
	amount=level-level/4+Random(level/2+1);
	if(amount<1)
		amount=1;

	DBG("ISM2");
	while(amount>0)
	{
		DBG("ISM3A");
		myType=(byte)Random(NUM_MONSTERS);

		DBG("ISM3W");
		if(MonsterLevel(myType)>amount)
			continue;

		// make sure it is an ok monster
		i=0;
		while(1)
		{
			if(myType==badTable[i])
			{
				myType=0;
				break;
			}
			i++;
			if(badTable[i]==255)
				break;
		}
		if(myType==0)
			continue;

		DBG("ISM3Q");
		amount-=MonsterLevel(myType);
		x=Random(map->width);
		y=Random(map->height);
		DBG("ISM3B");
		if(level>240)
			if(Random(480)<(dword)(level-240))
				myType=MONS_STICKWITCH;
		SurprisePlace(myType,0,x,y,map,world);
		DBG("ISM3C");
	}
}

void AddSurvivalMapGuys(Map *map)
{
	int i;

	if(player.cheatsOn&PC_INFSURV)
	{
		for(i=0;i<MAX_MAPMONS;i++)
			if(map->badguy[i].type==MONS_LOONY)
				AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					   0,player.monsType);

		InfSurvMonsters((survLevel+1)*2+(survLevel/10)*4,map,&curWorld);
		return;
	}

	for(i=0;i<MAX_MAPMONS;i++)
		if(map->badguy[i].type)
		{
			if(map->badguy[i].type==MONS_LOONY)
				AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					   0,player.monsType);
			else if(sLevel[survLevel].mons[i-1])
				AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					   0,sLevel[survLevel].mons[i-1]);
		}
}

void SurvivalBeginMsg(void)
{
	char s[64];

	if(player.cheatsOn&PC_INFSURV)
	{
		sprintf(s,"Infinity Wave %d",survLevel+1);
		NewBigMessage(s,90);
		if(survLevel==99)
			EarnBadge(BADGE_SURV100);
	}
	else
	{
		sprintf(s,"Wave %d: %s",survLevel+1,sLevel[survLevel].name);
		NewBigMessage(s,90);
	}

	survWin=0;
}

byte SurvivalPlaceItem(Map *map,int x,byte otr)
{
	byte itm;
	int cnt=0;

	while(cnt++<2000)
	{
		itm=(byte)Random(4);
		switch(itm)
		{
			case 0:
				if(player.monsType==MONS_PLYRSUMMON)
				{
					if(player.xtraByte<19 || (otr!=0 && player.xtraByte<20))
					{
						map->map[x+15*map->width].item=ITM_SUPERHEART;
						return 0;
					}
				}
				else
					if(player.maxHearts<player.maxMaxHearts-1 || (otr!=0 && player.maxHearts<player.maxMaxHearts))
					{
						map->map[x+15*map->width].item=ITM_SUPERHEART;
						return 0;
					}
				break;
			case 1:
				if(player.fireRate<9 || (otr!=1 && player.fireRate<10))
				{
					map->map[x+15*map->width].item=ITM_FIRERATEUP;
					return 1;
				}
				break;
			case 2:
				if(player.fireRange<9 || (otr!=2 && player.fireRange<10))
				{
					map->map[x+15*map->width].item=ITM_RANGEUP;
					return 2;
				}
				break;
			case 3:
				if(player.firePower<9 || (otr!=3 && player.firePower<10))
				{
					map->map[x+15*map->width].item=ITM_FIREPOWERUP;
					return 3;
				}
				break;
		}
	}
	return 255;
}

byte SurvivalWin(Map *map)
{
	if(survWin==0)
	{
		if(AnyMonsterExists())
			return 0;
		// if all monsters are dead, move on to the item-giving portion and do not win
		survWin=1;
		if(player.cheatsOn&PC_INFSURV)
			SurvivalPlaceItem(map,15,255);
		else
			SurvivalPlaceItem(map,15,SurvivalPlaceItem(map,14,255));
		return 0;
	}
	else
	{
		if(map->map[15+15*map->width].item || map->map[14+15*map->width].item)
			return 0;	// must collect reward still

		// it is collected!
		survLevel++;
		player.numSaves=survLevel;
		return 1;
	}
}

byte SurvivalOver(void)
{
	if(player.cheatsOn&PC_INFSURV)
		return 0;
	else
		return (survLevel>=SURV_LEVELS);
}

byte GetSurvLevel(void)
{
	return survLevel;
}
