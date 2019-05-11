#include "madcap.h"
#include "display.h"
#include "player.h"
#include "skill.h"

char madName[12][8]={
	"Might",
	"Life",
	"Shield",
	"Curse",
	"Health",
	"Shock",
	"Fire",
	"Ice",
	"Wind",
	"Nature",
	"Death",
};

char *MadcapName(byte w)
{
	return madName[w];
}

void PrintMadcapInfo(int x,int y,byte w)
{
	char s[64];

	switch(w)
	{
		case MC_MIGHT:
			sprintf(s,"%d Might Crystals gives you +%d%% Axe damage.",player.var[VAR_MADGEM+MC_MIGHT],10*player.var[VAR_MADGEM+MC_MIGHT]);
			Print(x+208,y+384+12,"All allies' direct attacks are boosted as well.",0,1);
			break;
		case MC_LIFE:
			sprintf(s,"%d %s Crystals gives you +%d%% Life.",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			Print(x+208,y+384+12,"All allies are boosted as well.",0,1);
			break;
		case MC_SHIELD:
			sprintf(s,"%d %s Crystals gives you +%d%% Defense.",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			Print(x+208,y+384+12,"All allies are boosted as well.",0,1);
			break;
		case MC_CURSE:
			sprintf(s,"%d %s Crystals gives you +%d%% damage to poison, fire,",player.var[VAR_MADGEM+w],madName[w],20*player.var[VAR_MADGEM+w]);
			Print(x+208,y+384+12,"and the Freezing Talent.",0,1);
			break;
		case MC_HEALTH:
			sprintf(s,"%d %s Crystals gives you +%d%% healing to things that",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			Print(x+208,y+384+12,"heal you besides spells (Survival, potions, etc).",0,1);
			break;
		case MC_SHOCK:
			sprintf(s,"%d %s Crystals gives you +%d%% damage to shocks,",player.var[VAR_MADGEM+w],madName[w],20*player.var[VAR_MADGEM+w]);
			Print(x+208,y+384+12,"except for Shock Orb.",0,1);
			break;
		case MC_FIRE:
			sprintf(s,"%d %s Crystals gives you +%d%% power to Fire Magic.",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			break;
		case MC_ICE:
			sprintf(s,"%d %s Crystals gives you +%d%% power to Ice Magic.",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			break;
		case MC_WIND:
			sprintf(s,"%d %s Crystals gives you +%d%% power to Wind Magic.",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			break;
		case MC_NATURE:
			sprintf(s,"%d %s Crystals gives you +%d%% power to Nature",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			Print(x+208,y+384+12,"Magic.",0,1);
			break;
		case MC_DEATH:
			sprintf(s,"%d %s Crystals gives you +%d%% power to Death Magic.",player.var[VAR_MADGEM+w],madName[w],10*player.var[VAR_MADGEM+w]);
			break;
	}
	Print(x+208,y+384,s,0,1);

	if(player.var[VAR_MADGEM+w]<100)
	{
		char s2[4]="";

		if(CrystalCost(w)==1)
			strcpy(s2,".");
		else
			strcpy(s2,"s.");

		if(player.money>=MADCAP_PRICE*(player.var[VAR_MADGEM+w]+1) && player.var[VAR_CPOINTS]>=CrystalCost(w))
		{
			sprintf(s,"You can buy another for $%d and %d Crystal Point%s",MADCAP_PRICE*(player.var[VAR_MADGEM+w]+1),CrystalCost(w),s2);
		}
		else
		{
			sprintf(s,"You need $%d and %d Crystal Point%s",MADCAP_PRICE*(player.var[VAR_MADGEM+w]+1),CrystalCost(w),s2);
		}
	}
	else
		sprintf(s,"You have the maximum number of %s Crystals!",madName[w]);
	Print(x+208,y+384+12*2,s,0,1);
	// add 12 to y per line
}

word DamageAmt(word dmg,byte team)
{
	if(player.var[VAR_MADCAP]==0)
		return dmg;	// normal damage
	else if(team==EVIL)
		return dmg*MADCAP_DAMBOOST;
	else
		return dmg*(100+player.var[VAR_MADGEM+MC_MIGHT]*10)/100;
}

word CurseDmg(word dmg,byte team)
{
	if(player.var[VAR_MADCAP])
	{
		if(team==EVIL)
		{
			if(PlagueOn())
				return dmg;
			else
				return dmg*MADCAP_CURSEBOOST;
		}
		else
		{
			return dmg*(100+player.var[VAR_MADGEM+MC_CURSE]*20)/100;
		}
	}
	else
		return dmg;
}

word ShockDmg(word dmg,byte team)
{
	if(player.var[VAR_MADCAP])
	{
		if(team==EVIL)
			return dmg*MADCAP_CURSEBOOST;
		else
			return dmg*(100+player.var[VAR_MADGEM+MC_SHOCK]*20)/100;
	}
	else
		return dmg;
}

word RecoverAmt(word dmg)
{
	if(player.var[VAR_MADCAP])
	{
		return dmg*(100+player.var[VAR_MADGEM+MC_HEALTH]*10)/100;
	}
	else
		return dmg;
}

byte CrystalCost(byte w)
{
	int i;
	int t;

	t=0;
	for(i=0;i<11;i++)
		t+=player.var[VAR_MADGEM+i];

	return (t/30+1);
}
