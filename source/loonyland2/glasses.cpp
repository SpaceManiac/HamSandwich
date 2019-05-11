#include "glasses.h"
#include "player.h"
#include "display.h"

glassEff_t glassEff[NUM_GLASSES]={
	{{0,1},GE_STAMINA,1},
	{{0,2},GE_STAMGEN,5},
	{{1,2},GE_MAGIC,1},
	{{0,3},GE_LIFE,1},
	{{1,3},GE_ARMOR,1},
	{{2,3},GE_SPEED,1},
	{{0,4},GE_FREEZE,5},
	{{1,4},GE_POISON,5},
	{{2,4},GE_DAMAGE,1},
	{{0,5},GE_MAGICDAM,5},
	{{1,5},GE_MAGICGEN,5},
	{{3,4},GE_STAMINA,2},
	{{2,5},GE_STAMGEN,10},
	{{0,6},GE_MAGIC,2},
	{{3,5},GE_LIFE,2},
	{{1,6},GE_ARMOR,2},
	{{2,6},GE_SPEED,2},
	{{4,5},GE_FREEZE,10},
	{{0,7},GE_POISON,10},
	{{3,6},GE_DAMAGE,2},
	{{1,7},GE_MAGICDAM,10},
	{{2,7},GE_MAGICGEN,10},
	{{4,6},GE_LIFEGEN,30},
	{{3,7},GE_SPELLCOST,1},
	{{0,8},GE_SPELLCOST,0},
	{{1,8},GE_SPELLCOST,3},
	{{5,6},GE_SPELLCOST,4},
	{{4,7},GE_SPELLCOST,2},
	{{2,8},GE_STAMINA,3},
	{{3,8},GE_STAMGEN,15},
	{{5,7},GE_MAGIC,3},
	{{0,9},GE_LIFE,3},
	{{1,9},GE_ARMOR,3},
	{{4,8},GE_SPEED,3},
	{{2,9},GE_FREEZE,15},
	{{6,7},GE_POISON,15},
	{{3,9},GE_DAMAGE,3},
	{{5,8},GE_MAGICDAM,15},
	{{0,10},GE_MAGICGEN,15},
	{{1,10},GE_STUN,5},
	{{4,9},GE_KNOCK,8},
	{{2,10},GE_LIFEGEN,20},
	{{6,8},GE_STAMINA,4},
	{{5,9},GE_STAMGEN,20},
	{{3,10},GE_MAGIC,4},
	{{0,11},GE_LIFE,4},
	{{7,8},GE_ARMOR,4},
	{{4,10},GE_SPEED,4},
	{{1,11},GE_FREEZE,20},
	{{6,9},GE_POISON,20},
	{{2,11},GE_DAMAGE,4},
	{{5,10},GE_MAGICDAM,20},
	{{3,11},GE_MAGICGEN,20},
	{{7,9},GE_STUN,10},
	{{4,11},GE_KNOCK,12},
	{{0,12},GE_STAMINA,5},
	{{6,10},GE_STAMGEN,25},
	{{1,12},GE_MAGIC,5},
	{{2,12},GE_LIFE,5},
	{{5,11},GE_ARMOR,5},
	{{8,9},GE_SPEED,5},
	{{3,12},GE_FREEZE,25},
	{{7,10},GE_POISON,25},
	{{6,11},GE_SPELLCOST,5},
	{{4,12},GE_DAMAGE,5},
	{{8,10},GE_MAGICDAM,25},
	{{5,12},GE_MAGICGEN,25},
	{{7,11},GE_LIFEGEN,10},
	{{6,12},GE_LUCK,1},
	{{9,10},GE_TALENT,0},
	{{8,11},GE_TALENT,2},
	{{7,12},GE_TALENT,1},
	{{9,11},GE_MONSTER,2},
	{{8,12},GE_SKILL,2},
	{{10,11},GE_SKILL,1},
	{{9,12},GE_SKILL,3},
	{{10,12},GE_SKILL,0},
	{{11,12},GE_SKILL,4},
};

byte WhichGlass(void)
{
	int i;

	for(i=0;i<NUM_GLASSES;i++)
	{
		if((player.lens[0]==glassEff[i].lens[0] && player.lens[1]==glassEff[i].lens[1]) ||
			(player.lens[1]==glassEff[i].lens[0] && player.lens[0]==glassEff[i].lens[1]))
			return (byte)i;
	}
	return 255;
}

byte GlassEffect(void)
{
	byte eff;

	eff=WhichGlass();
	if(eff==255)
		return 255;
	else
		return glassEff[eff].effect;
}

byte GlassAmount(void)
{
	byte eff;

	eff=WhichGlass();
	if(eff==255)
		return 0;
	else
		return glassEff[eff].amount;
}

char lensName[14][10]={
	"Avok",
	"Balki",
	"Ciao",
	"D'oh",
	"Eep",
	"Fonzee",
	"Grok",
	"Haw",
	"Izzi",
	"Jenkins",
	"Kwyjibo",
	"Leeeroy",
	"Mumu",
	"",
};

char *LensName(byte lens)
{
	if(lens==255)
		return lensName[13];
	else
		return lensName[lens];
}

void DescribeGlasses(int x,int y)
{
	char s[64],s2[16];

	if(player.var[VAR_GLASSES]==0)	// have old glasses
	{
		Print(x+208,y+352,"Old Scratched-Up Glasses",0,1);

		Print(x+208,y+352+24,"Allows you to see things.",0,1);
	}
	else
	{
		Print(x+208,y+352,"Super Magical Glasses",0,1);

		if(player.lens[0]==255 && player.lens[1]==255)
			sprintf(s,"Lenses are not inscribed.");
		else
			sprintf(s,"Lenses inscribed %s %s.",LensName(player.lens[0]),LensName(player.lens[1]));
		Print(x+208,y+352+24,s,0,1);
		switch(GlassEffect())
		{
			case GE_STAMINA:
				sprintf(s,"Maximum Stamina +%d.",GlassAmount());
				break;
			case GE_LIFE:
				sprintf(s,"Maximum Life +%d.",GlassAmount());
				break;
			case GE_ARMOR:
				sprintf(s,"Armor +%d.",GlassAmount());
				break;
			case GE_DAMAGE:
				sprintf(s,"Damage +%d.",GlassAmount());
				break;
			case GE_MAGIC:
				sprintf(s,"Maximum Magic +%d.",GlassAmount());
				break;
			case GE_SPEED:
				sprintf(s,"Axe Speed +%d.",GlassAmount());
				break;
			case GE_STAMGEN:
				sprintf(s,"Stamina recovers %d%% faster.",GlassAmount());
				break;
			case GE_MAGICGEN:
				sprintf(s,"Magic recovers %d%% faster.",GlassAmount());
				break;
			case GE_FREEZE:
				sprintf(s,"Freezing enemies lasts %d%% longer.",GlassAmount());
				break;
			case GE_POISON:
				sprintf(s,"Poisoning enemies lasts %d%% longer.",GlassAmount());
				break;
			case GE_MAGICDAM:
				sprintf(s,"Attack spells do %d%% more damage.",GlassAmount());
				break;
			case GE_LIFEGEN:
				sprintf(s,"Heal 1 Life every %d seconds.",GlassAmount());
				break;
			case GE_SPELLCOST:
				switch(GlassAmount())
				{
					case 0:
						strcpy(s2,"Fire");
						break;
					case 1:
						strcpy(s2,"Wind");
						break;
					case 2:
						strcpy(s2,"Ice");
						break;
					case 3:
						strcpy(s2,"Nature");
						break;
					case 4:
						strcpy(s2,"Death");
						break;
					case 5:
						strcpy(s2,"All");
						break;
				}
				sprintf(s,"%s spells cost 1 less Magic.",s2);
				break;
			case GE_TALENT:
				switch(GlassAmount())
				{
					case 0:
						strcpy(s2,"Combat");
						break;
					case 1:
						strcpy(s2,"Magic");
						break;
					case 2:
						strcpy(s2,"Craft");
						break;
					case 3:
						strcpy(s2,"all");
						break;
				}
				sprintf(s,"+1 to %s Talents.",s2);
				break;
			case GE_SKILL:
				switch(GlassAmount())
				{
					case 0:
						strcpy(s2,"Passive");
						break;
					case 1:
						strcpy(s2,"Attack");
						break;
					case 2:
						strcpy(s2,"Throwing");
						break;
					case 3:
						strcpy(s2,"Magic");
						break;
					case 4:
						strcpy(s2,"all");
						break;
				}
				sprintf(s,"+1 to %s Skills.",s2);
				break;
			case GE_STUN:
				if(GlassAmount()<10)
					strcpy(s,"Stun enemies slightly on hit.");
				else
					strcpy(s,"Stun enemies strongly on hit.");
				break;
			case GE_KNOCK:
				if(GlassAmount()<10)
					strcpy(s,"Knock enemies away slightly on hit.");
				else
					strcpy(s,"Knock enemies away strongly on hit.");
				break;
			case GE_LUCK:
				sprintf(s,"Increases your luck!");
				break;
			case GE_MONSTER:
				sprintf(s,"Doubles your number of enemies.");
				break;
			default:
				sprintf(s,"No effect.");
				break;
		}
		Print(x+208,y+352+36,s,0,1);
	}
}

void GuyDropLens(byte level,byte *type,int *val1,int *val2)
{
	int i;
	int total,chose;
	int odds[13]={	// adds up to 1000%
		180,
		167,
		150,
		140,
		120,
		100,
		60,
		30,
		20,
		15,
		10,
		6,
		2,
	};

	// decide which lens
	total=0;
	chose=rand()%1000;
	for(i=0;i<13;i++)
	{
		total+=odds[i];
		if(chose<total)
		{
			(*type)=IT_LENSA+i;
			break;
		}
	}
	if(i>=13)
		(*type)=IT_LENSA;	// this should never happen

	(*val1)=0;
	(*val2)=0;
}

byte AddLens(byte type)
{
	if(player.lens[0]==255)
	{
		player.lens[0]=type;
		return 1;
	}
	else if(player.lens[1]==255)
	{
		player.lens[1]=type;
		return 1;
	}
	else
		return 0;	// can't add it!
}
