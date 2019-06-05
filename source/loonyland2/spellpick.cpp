#include "spellpick.h"
#include "player.h"
#include "options.h"
#include "quest.h"
#include "skill.h"
#include "equip.h"
#include "ch_loony.h"
#include "alchemy.h"

typedef struct pick_t
{
	byte pos;	// where on the wheel it currently is
	byte rightpos;
	byte spell;	// which skill it's tied to
	byte pic;
	byte updown;	// should it have updown arrows
} pick_t;

byte numPicks;
byte circleSize;
static char lastKey=0;
static byte pickwait,vertwait,spellpick;

#define MAX_PICKS	(8)

byte spellSkill[5]={
	SKILL_SHROOM,
	SKILL_BURNINATE,
	SKILL_SHOCK,
	SKILL_RESTORE,
	SKILL_DRAIN};	// that one is for potions

byte spellPic[5][4]={
	{7,10,8,9},
	{4,3,5,6},
	{12,14,11,13},
	{15,16,18,17},
	{21,20,19,22},
};

pick_t picks[MAX_PICKS];
byte potionChoice=255;

void CenterPrintOutline(int x,int y,const char *s)
{
	CenterPrintDark(x-1,y-1,s,0);
	CenterPrintDark(x+1,y+1,s,0);
	CenterPrint(x,y,s,0,0);
}

void RenderSpellPick(MGLDraw *mgl)
{
	int i,x,y;
	char s[64],verb[16];

	for(i=0;i<numPicks;i++)
	{
		x=Cosine(picks[i].pos+192)*circleSize;
		y=Sine(picks[i].pos+192)*circleSize;
		x/=FIXAMT;
		y/=FIXAMT;
		x+=320;
		y+=240;
		RenderIntfaceSprite(x,y,picks[i].pic,8*(spellpick==i),mgl);
		if(picks[i].spell==255)	// potion
		{
			InstaRenderItem(x-2,y+5,player.items[potionChoice].type,8*(spellpick==i),mgl);
		}
	}

	verb[0]='\0';
	switch(picks[spellpick].spell)
	{
		case SKILL_NONE:
			strcpy(s,"Nevermind");
			break;
		case SKILL_THROWING:
			if(player.axeMode==0)
			{
				strcpy(verb,"Throw");
				strcpy(s,"Axes");
			}
			else
			{
				strcpy(verb,"Swing");
				strcpy(s,"Axe");
			}
			break;
		case 255:
			// potions
			strcpy(verb,"Use");
			strcpy(s,ItemName(player.items[potionChoice].type));
			CenterPrintDark(320+1,260+1,s,0);
			CenterPrintDark(320-1,260-1,s,0);
			CenterPrint(320,260,s,0,0);
			strcpy(s,PotionPrefix(&player.items[potionChoice]));
			break;
		default:
			strcpy(verb,"Cast");
			strcpy(s,GetSkillName(picks[spellpick].spell,1));
			break;
	}

	if(picks[spellpick].updown)
	{
		x=Cosine(192)*circleSize/FIXAMT+320;
		y=Sine(192)*circleSize/FIXAMT+240;
		RenderIntfaceSprite(x,y,26,0,mgl);
		RenderIntfaceSprite(x,y,27,0,mgl);
	}

	CenterPrintDark(320+1,200+1,verb,0);
	CenterPrintDark(320-1,200-1,verb,0);
	CenterPrint(320,200,verb,0,0);
	CenterPrintDark(320+1,230+1,s,0);
	CenterPrintDark(320-1,230-1,s,0);
	CenterPrint(320,230,s,0,0);

	if(circleSize>=120)
	{
		CenterPrintOutline(320,400,"Press Left or Right to choose a Skill or Potion");
		if(picks[spellpick].spell==255)	// potion
		{
			CenterPrintOutline(320,425,"Press Up or Down to cycle through Potions");
			CenterPrintOutline(320,450,"Release A to use the selected Potion");
		}
		else if(picks[spellpick].spell==SKILL_THROWING)
		{
			if(player.axeMode==0)
				CenterPrintOutline(320,425,"Release A to change to Throwing");
			else
				CenterPrintOutline(320,425,"Release A to change to Swinging");
		}
		else
		{
			if(picks[spellpick].spell>=SKILL_SHROOM && picks[spellpick].spell<=SKILL_LIFEBERRY)
				CenterPrintOutline(320,425,"Press Up or Down to cycle through Nature Spells");
			else if(picks[spellpick].spell>=SKILL_BURNINATE && picks[spellpick].spell<=SKILL_HEATSHIELD)
				CenterPrintOutline(320,425,"Press Up or Down to cycle through Fire Spells");
			else if(picks[spellpick].spell>=SKILL_SHOCK && picks[spellpick].spell<=SKILL_TORNADO)
				CenterPrintOutline(320,425,"Press Up or Down to cycle through Wind Spells");
			else if(picks[spellpick].spell>=SKILL_RESTORE && picks[spellpick].spell<=SKILL_ICECUBE)
				CenterPrintOutline(320,425,"Press Up or Down to cycle through Ice Spells");
			else if(picks[spellpick].spell>=SKILL_DRAIN && picks[spellpick].spell<=SKILL_PLAGUE)
				CenterPrintOutline(320,425,"Press Up or Down to cycle through Death Spells");
			CenterPrintOutline(320,450,"Release A to cast the selected Spell");
		}
	}
}

void InitSpellPick(void)
{
	int i,j;
	byte ok,picked;

	MakeNormalSound(SND_SPELLMENU);
	picks[0].spell=SKILL_NONE;
	picks[0].pic=2;	// blank
	numPicks=1;

	// add throwing button if have throwing skill
	if(SpecificSkillLevel(SKILL_THROWING))
	{
		picks[numPicks].spell=SKILL_THROWING;
		picks[numPicks].updown=0;
		if(player.axeMode==0)
			picks[numPicks].pic=0;
		else
			picks[numPicks].pic=1;
		numPicks++;
	}

	for(j=0;j<5;j++)
	{
		if(player.var[VAR_SPELLSET+j]==0)
			player.var[VAR_SPELLSET+j]=spellSkill[j];
		ok=0;
		for(i=0;i<4;i++)
		{
			if(SpecificSkillLevel(spellSkill[j]+i))
			{
				picked=i;
				ok++;
			}
		}
		if(ok>0)
		{
			picks[numPicks].spell=spellSkill[j]+picked;
			picks[numPicks].pic=spellPic[j][picked];
			picks[numPicks].updown=(ok>1);
			numPicks++;
		}
		if(SpecificSkillLevel(player.var[VAR_SPELLSET+j]))
		{
			picks[numPicks-1].spell=player.var[VAR_SPELLSET+j];
			picks[numPicks-1].pic=spellPic[j][player.var[VAR_SPELLSET+j]-spellSkill[j]];
		}
	}

	// add potion button if have any potions
	ok=0;
	for(j=0;j<MAX_EQUIP;j++)
		if(player.items[j].type>=IT_POT1 && player.items[j].type<=IT_POT20)
		{
			ok++;
			if(potionChoice==255 || player.items[potionChoice].type<IT_POT1 || player.items[potionChoice].type>IT_POT20)
				potionChoice=j;	// you don't currently have a legitimate potion chosen, so choose this one
		}

	// you do have potions!
	if(ok>0)
	{
		picks[numPicks].spell=255;
		picks[numPicks].pic=2;	// blank
		picks[numPicks].updown=(ok>1);
		numPicks++;
	}

	spellpick=0;
	ok=0;
	for(i=0;i<numPicks;i++)
		if(player.spell==picks[i].spell)
		{
			ok=1;
			spellpick=i;
		}

	if(ok==0)
		player.spell=SKILL_NONE;	// put it on Nevermind if it was on something previous

	for(i=0;i<numPicks;i++)
	{
		picks[(i+spellpick)%numPicks].pos=(byte)(i*255/numPicks);
		picks[(i+spellpick)%numPicks].rightpos=picks[(i+spellpick)%numPicks].pos;
	}
	pickwait=10;
	vertwait=0;
	circleSize=0;
}

void MoveSpellPick(void)
{
	int i;

	for(i=0;i<numPicks;i++)
	{
		picks[(i+spellpick)%numPicks].rightpos=(byte)(i*255/numPicks);
	}
	player.spell=picks[spellpick].spell;
	MakeNormalSound(SND_SPELLTURN);
}

void NextPotion(char dir)
{
	potionChoice+=dir;
	if(potionChoice>=MAX_EQUIP)
	{
		if(dir==1)
			potionChoice=0;
		else
			potionChoice=MAX_EQUIP-1;
	}
	while(player.items[potionChoice].type<IT_POT1 || player.items[potionChoice].type>IT_POT20)
	{
		potionChoice+=dir;
		if(potionChoice>=MAX_EQUIP)
		{
			if(dir==1)
				potionChoice=0;
			else
				potionChoice=MAX_EQUIP-1;
		}
	}
	MakeNormalSound(SND_SPELLTURN);
}

void NextSpell(char dir)
{
	byte base,set;

	base=0;
	set=0;
	if(picks[spellpick].spell>=SKILL_SHROOM && picks[spellpick].spell<=SKILL_LIFEBERRY)
	{
		base=SKILL_SHROOM;
		set=0;
	}
	if(picks[spellpick].spell>=SKILL_BURNINATE && picks[spellpick].spell<=SKILL_HEATSHIELD)
	{
		base=SKILL_BURNINATE;
		set=1;
	}
	if(picks[spellpick].spell>=SKILL_SHOCK && picks[spellpick].spell<=SKILL_TORNADO)
	{
		base=SKILL_SHOCK;
		set=2;
	}
	if(picks[spellpick].spell>=SKILL_RESTORE && picks[spellpick].spell<=SKILL_ICECUBE)
	{
		base=SKILL_RESTORE;
		set=3;
	}
	if(picks[spellpick].spell>=SKILL_DRAIN && picks[spellpick].spell<=SKILL_PLAGUE)
	{
		base=SKILL_DRAIN;
		set=4;
	}

	if(base==0)
		return;

	picks[spellpick].spell+=dir;
	while(picks[spellpick].spell<base || picks[spellpick].spell>base+3 || SpecificSkillLevel(picks[spellpick].spell)==0)
	{
		if(picks[spellpick].spell<base)
			picks[spellpick].spell=base+3;
		else if(picks[spellpick].spell>base+3)
			picks[spellpick].spell=base;
		else
			picks[spellpick].spell+=dir;
	}
	player.var[VAR_SPELLSET+set]=picks[spellpick].spell;
	picks[spellpick].pic=spellPic[set][picks[spellpick].spell-base];
	player.spell=picks[spellpick].spell;
	MakeNormalSound(SND_SPELLTURN);
}

byte UpdateSpellPick(MGLDraw *mgl)
{
	byte c;
	int i;
	byte sp;

	if(pickwait)
		pickwait--;
	if(vertwait)
		vertwait--;

	if(circleSize<120)
		circleSize+=10;

	for(i=0;i<numPicks;i++)
	{
		picks[i].pos=TurnToward(picks[i].pos,picks[i].rightpos,20-numPicks);
	}

	c=GetControls();
	sp=spellpick;

	if(pickwait)
		c&=CONTROL_B3;

	if(c&CONTROL_LF)
	{
		if(picks[0].pos==picks[0].rightpos)
		{
			spellpick--;
			if(spellpick>=numPicks)
				spellpick=numPicks-1;
			MoveSpellPick();
		}
	}
	else if(c&CONTROL_RT)
	{
		if(picks[0].pos==picks[0].rightpos)
		{
			spellpick++;
			if(spellpick>=numPicks)
				spellpick=0;
			MoveSpellPick();
		}
	}
	if(c&(CONTROL_UP|CONTROL_DN))
	{
		if(vertwait==0)
		{
			if(c&CONTROL_UP)
			{
				if(picks[spellpick].spell==255)
				{
					NextPotion(-1);
					vertwait=10;
				}
				else
				{
					NextSpell(-1);
					vertwait=10;
				}
			}
			else
			{
				if(picks[spellpick].spell==255)
				{
					NextPotion(1);
					vertwait=10;
				}
				else
				{
					NextSpell(1);
					vertwait=10;
				}
			}
		}
	}
	else
		vertwait=0;


	if((c&CONTROL_B3)==0)	// let go of the button
	{
		switch(player.spell)
		{
			case SKILL_NONE:
				break;
			case SKILL_THROWING:
				player.axeMode=1-player.axeMode;
				PlayerCalcStats();
				break;
			case 255:	// potion
				DrinkPotion(player.items[potionChoice].type,player.items[potionChoice].value);
				DropItem(potionChoice);
				SortEquipment();
				break;
			default:
				CastSpell(goodguy);
				break;
		}
		return 0;
	}
	return 1;
}
