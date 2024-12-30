#include "winpch.h"
#include "fishing.h"
#include "player.h"
#include "bullet.h"
#include "guy.h"
#include "goal.h"
#include "customworld.h"
#include "archipelago.h"

static char fishName[10][20]={
		"Bonefish", // caught in graveyard, gives energy
		"Brayka Bass", // caught in brayka, heals you some
		"Silverfish",	// caught in mirrors, gives 10 weapon XP to all weapons
		"Glarch",	// caught in darkness candle room, just drops sitflames to burn you
		"Darkwallow", // caught in darkness, summons a The Darkness near you
		"Electric Eel",	// caught in sparks, electrocutes you on catch
		"Screecher",	// caught in din, gives 200 XP
		"Napfish",		// caught in sleepy woods, levels up a random hammer by 1
		"Hollowfin",	// caught in underhollow, gives energy and health
		"Mudglopper",	// caught in mud anywhere, poisons you when caught
	};

static byte whichFish[]={
	0,	// 0 - back roads
	255, // 1 - hollow
	0,	// 2 - sleep inn
	0,	// 3 - yonder barn
	0, // 4 - graveyard
	0,	// 5 - town hall
	1,	// 6 - brayka lake
	0,	// 7 - tomb of flames
	2, // 8 - tomb of mirrors
	4, // 9 - tomb of darkness
	5, // 10 - tomb of sparks
	6, // 11 - tomb of noise
	0,	// 12 - tomb of bones
	7, // 13 - sleepy woods
	0,	// 14 - dumb side
	8,	// 15 - underhollow
	0,	// 16 - cabin
	0,	// 17 - horseradish hill
	0,	// 18 - happy glade
};

static float sizeMult[10]={
	1.5f,	// bonefish
	0.7f,	// bass
	1.2f,	// silverfish
	2.0f,	// glarch
	1.6f,	// darkwallow
	1.1f,	// eel
	1.4f,	// screecher
	3.0f,	// napfish
	1.3f,	// hollowfin
	2.0f,	// mudglopper
};

static byte fishingDiff[]={
	0,	// 0 - back roads
	20, // 1 - hollow
	0,	// 2 - sleep inn
	0,	// 3 - yonder barn
	30, // 4 - graveyard
	0,	// 5 - town hall
	0,	// 6 - brayka lake
	0,	// 7 - tomb of flames
	10, // 8 - tomb of mirrors
	20, // 9 - tomb of darkness
	25, // 10 - tomb of sparks
	30, // 11 - tomb of noise
	0,	// 12 - tomb of bones
	80, // 13 - sleepy woods
	0,	// 14 - dumb side
	40,	// 15 - underhollow
	0,	// 16 - cabin
	0,	// 17 - horseradish hill
	0,	// 18 - happy glade
};

// player.journal[30-39] hold how many of each fish type you have caught
// [40-49] show the largest size of each type
// 50,51,52,53 are used when fishing
// 54 is 1 if you have caught the unwanted goldfish
// 55 is your fishing skill

char *FishName(byte n)
{
	if (IsCustomWorld() && strlen(CustomFishName(n)))
		return CustomFishName(n);
	return fishName[n];
}

static float SizeMult(byte n)
{
	if (IsCustomWorld() && CustomFishSize(n) != -1)
		return CustomFishSize(n);
	return sizeMult[n];
}

static byte FishEffect(byte n)
{
	if (IsCustomWorld() && CustomFishEffect(n) != UINT8_MAX)
		return CustomFishEffect(n);
	return n;
}

static byte WhichFish(byte map)
{
	if (IsCustomWorld())
		return CustomFishWhich(map);
	return whichFish[map];
}

static byte FishingDiff(byte map)
{
	if (IsCustomWorld())
		return CustomFishDiff(map);
	return fishingDiff[map];
}

byte ShouldBob(void)
{
	int total;
	int maxOdds;

	if(WhichFish(player.levelNum)==255 && player.journal[54]==1)
		return 0;	// can only catch goldfish once

	total=player.journal[55]-FishingDiff(player.levelNum);
	if(total<0)
		return 0;

	maxOdds=300+player.journal[50]*70;	// the longer it's out there, the better the odds
	if(Random(maxOdds)<=(word)total)
		return 1;
	else
		return 0;

}

void CatchFish(int mapx,int mapy,Map *map,world_t *world)
{
	char s[32];
	byte size,type;
	int i;
	Guy *g;

	mapx=(mapx/TILE_WIDTH)/FIXAMT;
	mapy=(mapy/TILE_HEIGHT)/FIXAMT;
	if(player.journal[55]<100)
		player.journal[55]++;
	else
		CompleteGoal(23);

	if(Random(20)==0 && WhichFish(player.levelNum)!=255)	// 5% chance of getting an aquazoid instead
	{
		NewMessage("Yikes! Aquazoid!!",90,1);
		MakeNormalSound(SND_SERPENTSPIT);
		g=AddGuy((mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_SERPENT,0);
		g->seq=ANIM_A1;
		g->action=ACTION_BUSY;
		g->frmAdvance=128;
		player.journal[50]=0;
		player.journal[51]=0;
		player.journal[52]=0;
		player.journal[53]=0;
		player.wpnReload=20;
		return;
	}

	MakeNormalSound(SND_FISHCAUGHT);
	size=(byte)Random(10+player.journal[55]/2);	// 0-60 max
	while(Random(3)==0 && size<255)
		size++;	// increasingly unlikely chance to add bonus pounds all the way up to 255

	if(size==0)
		size=1;

	type=WhichFish(player.levelNum);

	if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_MUD)
		type=9;	// always a mudglopper

	if(type==255)
	{
		NewMessage("3 oz. Unwanted Goldfish!",120,1);
		player.journal[54]=1;
		player.xp=player.needXP;
		PlayerGetXP(1);	// level up!
		CompleteGoal(21);
	}
	else
	{
		float f;

		f=(float)size*SizeMult(type);

		if(f>255)
			f=255;
		if(f<1)
			f=1;

		size=(byte)f;

		if(player.levelNum==9 && !IsCustomWorld())	// tomb of darkness, 2 different possible catches
		{
			if(mapx>=71 && mapx<=81 && mapy>=20 && mapy<=30)	// the super bonus room
				type=3;
		}
		sprintf(s,"%d lb. %s!",size,FishName(type));
		NewMessage(s,120,1);
		if(player.journal[30+type]<255)
			player.journal[30+type]++;	// caught one more
		if(size>player.journal[40+type])	// a record size!
			player.journal[40+type]=size;
		if(size>=150)
			CompleteGoal(22);

		switch(FishEffect(type))
		{
			case 0:
				player.ammo+=20;
				if(player.ammo>100)
					player.ammo=100;
				break;
			case 1:
				PlayerHeal(20);
				break;
			case 2:
				for(i=0;i<6;i++)
					PlayerGetWeaponXP(i+1,10);
				break;
			case 3:
				if(player.journal[30+3]==1)	// your first glarch!
				{
					if (auto ap = Archipelago())
					{
						ap->GetGlarch();
					}
					else
					{
						map->map[goodguy->mapx+goodguy->mapy*map->width].item=ITM_CANDLE;
						NewMessage("It had swallowed a candle!",90,1);
					}
				}
				else
					FireBullet(goodguy->x,goodguy->y,0,BLT_BADSITFLAME,0);
				break;
			case 4:
				AddGuy(goodguy->x,goodguy->y,0,MONS_DARKNESS,0);
				break;
			case 5:
				g=AddGuy(goodguy->x,goodguy->y,0,MONS_SPARK,0);
				if(g)
					g->facing=6;
				break;
			case 6:
				PlayerGetXP(200);
				break;
			case 7:
				i=Random(6);
				if(player.weaponLvl[i]<10)
				{
					player.wpnXP[i]=player.weaponLvl[i]*100-1;
					PlayerGetWeaponXP(i+1,1);
				}
				break;
			case 8:
				player.ammo=100;
				PlayerHeal(128);
				break;
			case 9:
				MakeNormalSound(SND_THINGVOMIT);
				goodguy->poison=30;
				break;
		}
	}
	player.journal[50]=0;
	player.journal[51]=0;
	player.journal[52]=0;
	player.journal[53]=0;
	player.wpnReload=20;

	byte bigType,bigSize;

	size=0;
	type=0;
	bigType=0;
	bigSize=0;
	for(i=0;i<10;i++)
	{
		if(player.journal[30+i]>0)
			type++;	// type=how many types you've caught
		if(player.journal[40+i]>bigSize)
		{
			bigSize=player.journal[40+i];
			bigType=(byte)i;
		}
		size+=player.journal[30+i];	// size=how many total fish you've caught
	}
	profile.progress.goal[98]=bigType;
	profile.progress.goal[99]=bigSize;
	if(player.journal[54])
		size++;	// don't forget the goldfish!

	if(size==200)
		CompleteGoal(24);
	if(type==10)
		CompleteGoal(20);
}
