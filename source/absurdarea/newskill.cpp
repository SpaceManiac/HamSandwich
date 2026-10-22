#include "newskill.h"
#include "player.h"
#include "options.h"
#include "quest.h"
#include "skill.h"
#include "equip.h"
#include "ch_loony.h"

static char lastKey=0;
static byte btnwait;
static byte skill;

void RenderNewSkill(MGLDraw *mgl)
{
	char s[32];

	// 436 wide
	PauseBox(102,100,538,200,234);

	if(skill>=100)	// talent, not skill
	{
		if(skill<110)
			strcpy(s,"New Combat Talent!");
		else if(skill<120)
			strcpy(s,"New Magic Talent!");
		else
			strcpy(s,"New Craft Talent!");
	}
	else
	{
		if(GetSkill(skill)->type==ST_MAGIC || GetSkill(skill)->type==ST_MAGICP)
			strcpy(s,"New Magic Skill!");
		if(GetSkill(skill)->type==ST_ATTACK || GetSkill(skill)->type==ST_ATTACKP)
			strcpy(s,"New Attack Skill!");
		if(GetSkill(skill)->type==ST_THROW || GetSkill(skill)->type==ST_THROWP)
			strcpy(s,"New Throwing Skill!");
		if(GetSkill(skill)->type==ST_CRAFT || GetSkill(skill)->type==ST_CRAFTP)
			strcpy(s,"New Crafting Skill!");
		if(GetSkill(skill)->type==ST_PASSIVE)
			strcpy(s,"New Passive Skill!");
	}

	CenterPrintColor(320-1,70-1,s,0,-32,0);
	CenterPrintColor(320+1,70-1,s,0,-32,0);
	CenterPrintColor(320-1,70+1,s,0,-32,0);
	CenterPrintColor(320+1,70+1,s,0,-32,0);
	CenterPrintGlow(320,70,s,0,0);

	if(skill>=100)
	{
		CenterPrintGlow(320,104,TalentName(skill-100),0,0);

		Print(107,134,GetTalent(skill-100)->desc[0],0,1);
		Print(107,134+12,GetTalent(skill-100)->desc[1],0,1);
		Print(107,134+24,GetTalent(skill-100)->desc[2],0,1);
		Print(107,134+36,GetTalent(skill-100)->desc[3],0,1);
	}
	else
	{
		CenterPrintGlow(320,104,GetSkillName(skill,1),0,0);

		Print(107,134,GetSkill(skill)->desc[0],0,1);
		Print(107,134+12,GetSkill(skill)->desc[1],0,1);
		Print(107,134+24,GetSkill(skill)->desc[2],0,1);
		Print(107,134+36,GetSkill(skill)->desc[3],0,1);
		Print(107,134+48,GetSkill(skill)->desc[4],0,1);
	}
}

void InitNewSkill(byte sk)
{
	skill=sk;

	btnwait=30;
	MakeNormalSound(SND_NEWSKILL);
}

byte UpdateNewSkill(MGLDraw *mgl)
{
	byte c;

	if(btnwait)
	{
		btnwait--;
		c=GetTaps();
		mgl->LastKeyPressed();
		return 1;
	}

	c=GetTaps();

	if((c&(CONTROL_B1|CONTROL_B2|CONTROL_B3)))	// pressed a button
	{
		return 0;
	}
	return 1;
}
