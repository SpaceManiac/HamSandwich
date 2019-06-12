#include "pause.h"
#include "player.h"
#include "options.h"
#include "quest.h"
#include "skill.h"
#include "equip.h"
#include "help.h"
#include "glasses.h"
#include "title.h"
#include "alchemy.h"
#include "madcap.h"
#include "music.h"
#include "achieve.h"
#include "leveldef.h"

char pauseOption[PAUSE_CHOICES][32]={
	"Resume",
	"Items",
	"Skills",
	"Talents",
	"Quests",
	"Message Log",
	"Madcap Items",
	"Tips",
	"Options",
	"Save & Quit",
};

byte cursor=0;
static byte subcursor=0,skillPage=0,questPage=0,talentPage=0,yesno=0,asking,tipPage=0;
static char lastKey=0;
byte subMode;
static byte darkness;
static int offX;
static byte oldc;
static byte skillPtr[10];
static char helpTxt[4][64];
static char askingTxt[64];
static char statText[6][32],statNum[6];
static byte statClock[3];

void NewStatText(byte n)
{
	int i,j;
	byte b;

	while(1)
	{
		b=rand()%25;
		j=0;
		for(i=0;i<6;i++)
			if(i!=n && statNum[i]==b)
				j=1;
		if(j!=1)
			break;
	}
	statNum[n]=b;
	switch(b)
	{
		case 0:
			sprintf(statText[n],"%d Loonies lost.",player.var[VAR_DEATHS]);
			break;
		case 1:
			sprintf(statText[n],"%d Bodzhas beaten.",player.var[VAR_KILLCOUNTS+KC_BODZHA]+player.var[VAR_KILLCOUNTS+KC_BODZHA+1]*256);
			break;
		case 2:
			sprintf(statText[n],"%d Ghosts busted.",player.var[VAR_KILLCOUNTS+KC_GHOST]+player.var[VAR_KILLCOUNTS+KC_GHOST+1]*256);
			break;
		case 3:
			sprintf(statText[n],"%d Ice beasts iced.",player.var[VAR_KILLCOUNTS+KC_ICE]+player.var[VAR_KILLCOUNTS+KC_ICE+1]*256);
			break;
		case 4:
			sprintf(statText[n],"%d Bears bonked.",player.var[VAR_KILLCOUNTS+KC_BEAR]+player.var[VAR_KILLCOUNTS+KC_BEAR+1]*256);
			break;
		case 5:
			sprintf(statText[n],"%d Mice mashed.",player.var[VAR_KILLCOUNTS+KC_MICE]+player.var[VAR_KILLCOUNTS+KC_MICE+1]*256);
			break;
		case 6:
			sprintf(statText[n],"%d Puppets popped.",player.var[VAR_KILLCOUNTS+KC_PUPPET]+player.var[VAR_KILLCOUNTS+KC_PUPPET+1]*256);
			break;
		case 7:
			sprintf(statText[n],"%d Monkeys socked.",player.var[VAR_KILLCOUNTS+KC_MONKEY]+player.var[VAR_KILLCOUNTS+KC_MONKEY+1]*256);
			break;
		case 8:
			sprintf(statText[n],"%d Stone beasts stomped.",player.var[VAR_KILLCOUNTS+KC_STONE]+player.var[VAR_KILLCOUNTS+KC_STONE+1]*256);
			break;
		case 9:
			sprintf(statText[n],"%d Shrooms shattered.",player.var[VAR_KILLCOUNTS+KC_SHROOM]+player.var[VAR_KILLCOUNTS+KC_SHROOM+1]*256);
			break;
		case 10:
			sprintf(statText[n],"%d Soldiers sorted.",player.var[VAR_KILLCOUNTS+KC_SOLDIER]+player.var[VAR_KILLCOUNTS+KC_SOLDIER+1]*256);
			break;
		case 11:
			sprintf(statText[n],"%d Plants placated.",player.var[VAR_KILLCOUNTS+KC_PLANT]+player.var[VAR_KILLCOUNTS+KC_PLANT+1]*256);
			break;
		case 12:
			sprintf(statText[n],"%d Gangsters ganked.",player.var[VAR_KILLCOUNTS+KC_ONION]+player.var[VAR_KILLCOUNTS+KC_ONION+1]*256);
			break;
		case 13:
			sprintf(statText[n],"%d Knights knifed.",player.var[VAR_KILLCOUNTS+KC_BUNNY]+player.var[VAR_KILLCOUNTS+KC_BUNNY+1]*256);
			break;
		case 14:
			j=0;
			for(i=0;i<50;i++)
				if(player.var[VAR_CHEST+i])
					j++;
			sprintf(statText[n],"%d Chests churned.",j);
			break;
		case 15:
			sprintf(statText[n],"%d Arena victories.",player.var[VAR_ARENAWINS]);
			break;
		case 16:
			sprintf(statText[n],"%d Axes thrown/swung.",player.shotsFired);
			break;
		case 17:
			sprintf(statText[n],"%d Spells cast.",player.specialShotsFired);
			break;
		case 18:
			sprintf(statText[n],"%d Damage taken.",player.hitsTaken);
			break;
		case 19:
			sprintf(statText[n],"%d Damage dealt.",player.damageDone);
			break;
		case 20:
			sprintf(statText[n],"$%d collected.",player.gemsGotten);
			break;
		case 21:
			sprintf(statText[n],"%d Herbs collected.",player.herbsPicked);
			break;
		case 22:
			sprintf(statText[n],"%d Holes dug.",player.holesDug);
			break;
		case 23:
			sprintf(statText[n],"%d Monsters slain.",player.kills);
			break;
		case 24:
			sprintf(statText[n],"%d Potions poured.",player.potionsDrunk);
			break;
	}
}

void SwapStatText(byte b)
{
	strcpy(statText[b],statText[b+1]);
	statNum[b]=statNum[b+1];
	NewStatText(b+1);
}

void BuildStatText(void)
{
	int i;
	for(i=0;i<6;i++)
		statNum[i]=(byte)255;
	NewStatText(1);
	SwapStatText(0);
	NewStatText(3);
	SwapStatText(2);
	NewStatText(5);
	SwapStatText(4);
}

void SetHelpText(void)
{
	if(subMode==0)
	{
		strcpy(helpTxt[0],"Up & Down to choose options.  Press Fire to select,");
		strcpy(helpTxt[1],"Jump or ESC to resume play.");
		strcpy(helpTxt[2],"");
		strcpy(helpTxt[3],"");
	}
	if(subMode==SUBMODE_SKILL)
	{
		strcpy(helpTxt[0],"Up & Down to choose skills.  Press Fire to add a point");
		strcpy(helpTxt[1],"to a skill if you have any.  Left & Right will take you");
		strcpy(helpTxt[2],"to more pages of skills.  Jump or ESC exits.");
		strcpy(helpTxt[3],"WARNING: Skill points are permanent once placed!");
	}
	if(subMode==SUBMODE_TALENT)
	{
		strcpy(helpTxt[0],"Up & Down to choose talents.  Left and Right flip to");
		strcpy(helpTxt[1],"to different pages.  Jump or ESC exits.");
		strcpy(helpTxt[2],"The meter indicates how close the current talent is");
		strcpy(helpTxt[3],"to gaining a level.");
	}
	if(subMode==SUBMODE_EQUIP)
	{
		strcpy(helpTxt[0],"Browse with arrow keys.  Press Fire to equip the selected");
		strcpy(helpTxt[1],"item.  Press Magic to PERMANENTLY discard the selected");
		strcpy(helpTxt[2],"item.  Press Jump or ESC to exit this menu.");
		strcpy(helpTxt[3],"");
	}
	if(subMode==SUBMODE_EQUIPYESNO)
	{
		strcpy(helpTxt[0],"Left and Right to choose.  Fire to accept, Jump to");
		strcpy(helpTxt[1],"cancel.");
		strcpy(helpTxt[2],"");
		strcpy(helpTxt[3],"");
	}
	if(subMode==SUBMODE_QUEST)
	{
		strcpy(helpTxt[0],"Up & Down to flip through your quests.  Left & Right");
		strcpy(helpTxt[1],"change pages.  Handy info on the quest is displayed");
		strcpy(helpTxt[2],"at the bottom.  Press Fire or Jump to exit.");
		strcpy(helpTxt[3],"");
	}
	if(subMode==SUBMODE_MADCAP)
	{
		strcpy(helpTxt[0],"Up & Down to choose Crystals.  Fire to buy a Crystal,");
		strcpy(helpTxt[1],"Jump to exit.");
		strcpy(helpTxt[2],"");
		strcpy(helpTxt[3],"");
	}
	if(subMode==SUBMODE_TIPS)
	{
		strcpy(helpTxt[0],"Up & Down to flip through tips.  Left & Right change");
		strcpy(helpTxt[1],"pages.  Press Fire or Jump to exit.");
		strcpy(helpTxt[2],"");
		strcpy(helpTxt[3],"");
	}
	if(subMode==SUBMODE_OPTIONS)
	{
		strcpy(helpTxt[0],"Up, Down, Left and Right to select.  Fire to adjust");
		strcpy(helpTxt[1],"an option.  Jump to exit.  Changes made here affect");
		strcpy(helpTxt[2],"only this profile. To change newly-made profiles and");
		strcpy(helpTxt[3],"the title screen, use the title options menu.");
	}
}

void SetSubCursor(byte s)
{
	subcursor=s;
}

void PauseBox(int x,int y,int x2,int y2,byte c)
{
	byte c2;

	c2=(31-(c&31))/2+(c&31);
	c2+=(c&(~31));

	// grey outline
	DrawFillBox(x+2,y,x2-2,y,26);
	DrawFillBox(x+2,y2,x2-2,y2,26);
	DrawFillBox(x,y+2,x,y2-2,26);
	DrawFillBox(x2,y+2,x2,y2-2,26);
		// grey outline dots
	DrawFillBox(x+1,y+1,x+1,y+1,26);
	DrawFillBox(x2-1,y+1,x2-1,y+1,26);
	DrawFillBox(x+1,y2-1,x+1,y2-1,26);
	DrawFillBox(x2-1,y2-1,x2-1,y2-1,26);
	// white outline
	DrawFillBox(x+1,y+2,x+1,y2-2,31);
	DrawFillBox(x2-1,y+2,x2-1,y2-2,31);
	DrawFillBox(x+2,y+1,x2-2,y+1,31);
	DrawFillBox(x+2,y2-1,x2-2,y2-1,31);
		// white outline dots
	DrawFillBox(x+2,y+2,x+2,y+2,31);
	DrawFillBox(x2-2,y+2,x2-2,y+2,31);
	DrawFillBox(x+2,y2-2,x+2,y2-2,31);
	DrawFillBox(x2-2,y2-2,x2-2,y2-2,31);
	// bright outline
	DrawFillBox(x+2,y+3,x+2,y2-3,c2);
	DrawFillBox(x2-2,y+3,x2-2,y2-3,c2);
	DrawFillBox(x+3,y+2,x2-3,y+2,c2);
	DrawFillBox(x+3,y2-2,x2-3,y2-2,c2);
	// and finally dark/normal interior
	DrawFillBox(x+3,y+3,x2-3,y2-3,c);
}

void RenderMadcapMenu(int x,int y)
{
	int i;
	byte c;
	char s[8];

	PauseBox(x+202,y+5,x+638,y+377,234);
	PauseBox(x+202,y+379,x+638,y+428,234);

	PrintMadcapInfo(x,y,subcursor);

	PrintGlow(x+251,y+8,"Madcap Crystals",0,0);

	x+=206;
	y+=34;
	for(i=0;i<11;i++)
	{
		if(subcursor!=i || subMode!=SUBMODE_MADCAP)
			PauseBox(x,y,x+105,y+28,240);
		else if(subMode==SUBMODE_MADCAP)
			PauseBox(x,y,x+105,y+28,32*5+16);
		PrintDark(x+12,y+2,MadcapName(i),0);

		c=234+(21*player.var[VAR_MADGEM+i]/100);

		PauseBox(x+108,y+1,x+108+(310*player.var[VAR_MADGEM+i]/100)+10,y+27,c);

		sprintf(s,"%d",player.var[VAR_MADGEM+i]);
		CenterPrint(x+108+320/2-1,y+3-1,s,-32,0);
		CenterPrint(x+108+320/2+1,y+3-1,s,-32,0);
		CenterPrint(x+108+320/2+1,y+3+1,s,-32,0);
		CenterPrint(x+108+320/2-1,y+3+1,s,-32,0);
		CenterPrintColor(x+108+320/2,y+3,s,0,0,0);

		y+=31;
	}
}

void OptionButton(int x,int y,int wid,const char *txt,byte spot)
{
	if(subcursor!=spot || subMode!=SUBMODE_OPTIONS)
		PauseBox(x,y,x+wid,y+28,240);
	else if(subMode==SUBMODE_OPTIONS)
		PauseBox(x,y,x+wid,y+28,32*5+16);
	CenterPrintDark(x+wid/2,y+2,txt,0);
}

void RenderOptionsMenu(int x,int y)
{
	int i,j;
	char s[64];

	PauseBox(x+202,y+5,x+638,y+377,234);
	PauseBox(x+202,y+379,x+638,y+428,234);

	PrintGlow(x+251,y+8,"Game Options",0,0);

	x+=206;
	y+=34;

	// player stats
	sprintf(s,"Profile: %s",player.profile);
	Print(x+2,y+350,s,0,1);
	sprintf(s,"Hours Played: %d:%02d",player.playClock/(30*60*60),(player.playClock/(30*60))%60);
	RightPrint(x+428,y+350,s,0,1);
	i=0;
	strcpy(s,"Modifiers: ");
	if(player.var[VAR_MODIFIER+0])
	{
		i++;
		strcat(s,ModifierName(player.var[VAR_MODIFIER+0]));
	}
	if(player.var[VAR_MODIFIER+1])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,ModifierName(player.var[VAR_MODIFIER+1]));
	}
	if(player.var[VAR_MODIFIER+2])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,ModifierName(player.var[VAR_MODIFIER+2]));
	}
	if(player.var[VAR_MADCAP])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,"MADCAP!");
	}
	if(i==0)
		strcat(s,"None");
	Print(x+2,y+364,s,0,1);
	for(j=0;j<3;j++)
	{
		if(statClock[j]<30*2)
			i=0;
		else if(statClock[j]<30*3)
			i=-(statClock[j]-30*2);
		if(j==0)
			PrintGlow(x+2,y+378,statText[j*2+0],i,1);
		else if(j==1)
			RightPrintGlow(x+428,y+378,statText[j*2+0],i,1);
		if(statClock[j]>=30*2)
		{
			i=-(30*3-statClock[j]);
			if(j==0)
				PrintGlow(x+2,y+378,statText[j*2+1],i,1);
			else if(j==1)
				RightPrintGlow(x+428,y+378,statText[j*2+1],i,1);
		}
	}

	OptionButton(x,y,180,"Sound",0);
	PauseBox(x+185,y+4,x+185+133,y+24,234);
	DrawFillBox(x+188,y+8,x+188+player.var[VAR_SOUND],y+20,31);
	y+=31;
	OptionButton(x,y,180,"Music",1);
	PauseBox(x+185,y+4,x+185+133,y+24,234);
	DrawFillBox(x+188,y+8,x+188+player.var[VAR_MUSIC],y+20,31);
	y+=31;
#ifdef DIRECTORS
	if(opt.modifier[29])
	{
		OptionButton(x,y,180,"Commentary",2);
		if(player.var[VAR_COMMENTARY])
			PrintGlow(x+190,y+4,"On",0,0);
		else
			PrintGlow(x+190,y+4,"Off",0,0);
	}
#else
	CenterPrintDark(x+78+1*116+112/2,y+2,"Controls",0);
#endif
	y+=31;

	CenterPrintDark(x+78+0*116+112/2,y+2,"Key 1",0);
	CenterPrintDark(x+78+1*116+112/2,y+2,"Key 2",0);
	CenterPrintDark(x+78+2*116+112/2,y+2,"Gamepad",0);
	y+=31;

	PrintDark(x+2,y+0*31+2,"Up",0);
	PrintDark(x+2,y+1*31+2,"Down",0);
	PrintDark(x+2,y+2*31+2,"Left",0);
	PrintDark(x+2,y+3*31+2,"Right",0);
	PrintDark(x+2,y+4*31+2,"Fire",0);
	PrintDark(x+2,y+5*31+2,"Jump",0);
	PrintDark(x+2,y+6*31+2,"Spell",0);

	CenterPrintDark(x+78+2*116+112/2,y+0*31+2,"Up",0);
	CenterPrintDark(x+78+2*116+112/2,y+1*31+2,"Down",0);
	CenterPrintDark(x+78+2*116+112/2,y+2*31+2,"Left",0);
	CenterPrintDark(x+78+2*116+112/2,y+3*31+2,"Right",0);
	for(i=0;i<3;i++)
		for(j=0;j<7;j++)
		{
			if(i<2)
				OptionButton(x+78+i*116,y+j*31,112,ScanCodeText(player.var[VAR_CONTROLS+i*8+j]),3+i+j*3);
			else if(j>3)
			{
				sprintf(s,"Btn %d",player.var[VAR_CONTROLS+16+j-4]);
				OptionButton(x+78+i*116,y+j*31,112,s,3+i+j*3);
			}
		}
}

void RenderSetKey(int x,int y,byte t)
{
	char actName[7][16]={
		"Up",
		"Down",
		"Left",
		"Right",
		"Fire",
		"Jump",
		"Spell",
	};

	PauseBox(x+130,y+80,x+630,y+240,234);

	if(t==0)
		CenterPrintGlow(x+380,y+100,"Press a key to assign to",0,0);
	if(t==1)
		CenterPrintGlow(x+380,y+100,"Press a gamepad button to assign to",0,0);
	CenterPrintGlow(x+380,y+150,actName[((subcursor-3)/3)],0,0);

	CenterPrintGlow(x+380,y+204,"Or ESC to cancel",0,0);
}

void RenderSkillMenu(int x,int y)
{
	int i,j;
	byte c,bigLvl;

	PauseBox(x+202,y+5,x+638,y+345,234);
	PauseBox(x+202,y+347,x+638,y+428,234);

	if(skillPtr[subcursor]!=255 && player.skillHave[skillPtr[subcursor]])
	{
		Print(x+208,y+352,GetSkill(player.skillHave[skillPtr[subcursor]])->desc[0],0,1);
		Print(x+208,y+352+12,GetSkill(player.skillHave[skillPtr[subcursor]])->desc[1],0,1);
		Print(x+208,y+352+24,GetSkill(player.skillHave[skillPtr[subcursor]])->desc[2],0,1);
		Print(x+208,y+352+36,GetSkill(player.skillHave[skillPtr[subcursor]])->desc[3],0,1);
		Print(x+208,y+352+48,GetSkill(player.skillHave[skillPtr[subcursor]])->desc[4],0,1);
		if(player.skillHave[skillPtr[subcursor]]>=SKILL_SHROOM && player.skillHave[skillPtr[subcursor]]<=SKILL_LIFEBERRY)
			PrintColor(x+208,y+352+60,"This is a Nature Spell.",1,-8,1);
		else if(player.skillHave[skillPtr[subcursor]]>=SKILL_BURNINATE && player.skillHave[skillPtr[subcursor]]<=SKILL_HEATSHIELD)
			PrintColor(x+208,y+352+60,"This is a Fire Spell.",4,-8,1);
		else if(player.skillHave[skillPtr[subcursor]]>=SKILL_SHOCK && player.skillHave[skillPtr[subcursor]]<=SKILL_TORNADO)
			PrintColor(x+208,y+352+60,"This is a Wind Spell.",7,-8,1);
		else if(player.skillHave[skillPtr[subcursor]]>=SKILL_RESTORE && player.skillHave[skillPtr[subcursor]]<=SKILL_ICECUBE)
			PrintColor(x+208,y+352+60,"This is an Ice Spell.",3,0,1);
		else if(player.skillHave[skillPtr[subcursor]]>=SKILL_DRAIN && player.skillHave[skillPtr[subcursor]]<=SKILL_PLAGUE)
			PrintColor(x+208,y+352+60,"This is a Death Spell.",6,-8,1);
	}

	// list skills
	switch(skillPage)
	{
		case 0:
			PrintGlow(x+251,y+8,"Passive Skills 1/5",0,0);
			break;
		case 10:
			PrintGlow(x+251,y+8,"Attack Skills 2/5",0,0);
			break;
		case 20:
			PrintGlow(x+251,y+8,"Throwing Skills 3/5",0,0);
			break;
		case 30:
			PrintGlow(x+251,y+8,"Magic Skills 4/5",0,0);
			break;
		case 40:
			PrintGlow(x+251,y+8,"More Magic Skills 5/5",0,0);
			break;
	}


	x+=206;
	y+=34;
	for(i=0;i<10;i++)
	{
		if(skillPtr[i]!=255)
		{
			if(subcursor!=i || subMode!=SUBMODE_SKILL)
				PauseBox(x,y,x+305,y+28,240);
			else if(subMode==SUBMODE_SKILL)
				PauseBox(x,y,x+305,y+28,32*5+16);
			PrintDark(x+12,y+2,GetSkill(player.skillHave[skillPtr[i]])->name,0);
			bigLvl=player.skillLvl[skillPtr[i]]+EquipSkillBoost(player.skillHave[skillPtr[i]]);
			if(player.potionType==POT_SPARKLING)
				bigLvl+=PotionEffect(player.potionType,player.potionPower);
			if(bigLvl>18)
				bigLvl=18;

			for(j=0;j<10;j++)
			{
				if(j<player.skillLvl[skillPtr[i]])
					c=234+21;
				else if(j<bigLvl)
					c=5*32+16;
				else
					c=234;
				PauseBox(x+308+j*12,y+1,x+308+j*12+10,y+29,c);
			}
			if(bigLvl>10)
			{
				char s[8];

				sprintf(s,"%d",bigLvl);
				CenterPrint(x+308+5*12-1,y+3-1,s,-32,0);
				CenterPrint(x+308+5*12+1,y+3-1,s,-32,0);
				CenterPrint(x+308+5*12+1,y+3+1,s,-32,0);
				CenterPrint(x+308+5*12-1,y+3+1,s,-32,0);
				CenterPrintColor(x+308+5*12,y+3,s,5,-8,0);
			}
		}
		else	// don't have a skill in this slot
		{
			if(subcursor!=i || subMode!=SUBMODE_SKILL)
				PauseBox(x+2,y+2,x+305-2,y+28-2,230);
			else
				PauseBox(x+2,y+2,x+305-2,y+28-2,32*5+6);

			PrintDark(x+12,y+2,"- - - - - -",0);
		}

		y+=31;
	}
}

void RenderTalentMenu(int x,int y)
{
	int i,j;
	byte c,bigLvl;

	PauseBox(x+202,y+5,x+638,y+345,234);
	PauseBox(x+202,y+347,x+638,y+428,234);
	if(player.talentLevel[subcursor+talentPage]!=255)
	{
		Print(x+208,y+352   ,GetTalent(subcursor+talentPage)->desc[0],0,1);
		Print(x+208,y+352+12,GetTalent(subcursor+talentPage)->desc[1],0,1);
		Print(x+208,y+352+24,GetTalent(subcursor+talentPage)->desc[2],0,1);
		Print(x+208,y+352+36,GetTalent(subcursor+talentPage)->desc[3],0,1);

		PauseBox(x+208,y+352+58,x+628,y+352+58+12,234);
		DrawFillBox(x+212,y+352+58+4,x+212+(player.talentPts[subcursor+talentPage]*(628-212-4)/TalentPointsNeeded(subcursor+talentPage)),
					y+352+58+12-4,31);
	}

	// list talents
	switch(talentPage)
	{
		case 0:
			PrintGlow(x+251,y+8,"Combat Talents 1/3",0,0);
			break;
		case 10:
			PrintGlow(x+251,y+8,"Magic Talents 2/3",0,0);
			break;
		case 20:
			PrintGlow(x+251,y+8,"Craft Talents 3/3",0,0);
			break;
	}


	x+=206;
	y+=34;
	for(i=0;i<10;i++)
	{
		if(player.talentLevel[i+talentPage]!=255)
		{
			if(subcursor!=i || subMode!=SUBMODE_TALENT)
				PauseBox(x,y,x+305,y+28,240);
			else if(subMode==SUBMODE_TALENT)
				PauseBox(x,y,x+305,y+28,32*5+16);
			PrintDark(x+12,y+2,TalentName(i+talentPage),0);
			for(j=0;j<10;j++)
			{
				bigLvl=player.talentLevel[i+talentPage];
				if(GlassEffect()==GE_TALENT)
				{
					if(GlassAmount()==3)
						bigLvl++;
					else if(GlassAmount()==0 && (i+talentPage)>=TLT_CHOPPING && (i+talentPage)<=TLT_GLADIATOR)
						bigLvl++;
					else if(GlassAmount()==1 && (i+talentPage)>=TLT_FOCUS && (i+talentPage)<=TLT_WINDBAG)
						bigLvl++;
					else if(GlassAmount()==2 && (i+talentPage)>=TLT_MOBILITY && (i+talentPage)<=TLT_HERBALISM)
						bigLvl++;
				}
				if(j<player.talentLevel[i+talentPage])
					c=234+21;
				else if(j<bigLvl)
					c=5*32+16;
				else
					c=234;
				PauseBox(x+308+j*12,y+1,x+308+j*12+10,y+29,c);
			}
		}
		else	// don't have this talent yet
		{
			if(subcursor!=i || subMode!=SUBMODE_TALENT)
				PauseBox(x+2,y+2,x+305-2,y+28-2,230);
			else
				PauseBox(x+2,y+2,x+305-2,y+28-2,32*5+6);

			PrintDark(x+12,y+2,"- - - - - -",0);
		}

		y+=31;
	}
}

void RenderQuestMenu(int x,int y)
{
	int i;
	char s[32];

	PauseBox(x+202,y+5,x+638,y+345,234);
	PauseBox(x+202,y+347,x+638,y+428,234);
	if(player.var[VAR_QUESTASSIGN+subcursor+questPage*10] && subMode==SUBMODE_QUEST)
	{
		PrintQuestInfo(subcursor+questPage*10,x,y);
	}


	sprintf(s,"Quests %d/5",questPage+1);
	PrintGlow(x+251,y+8,s,0,0);

	x+=206;
	y+=34;
	for(i=0;i<10;i++)
	{
		if(player.var[VAR_QUESTASSIGN+i+questPage*10])
		{
			byte c;

			if(subcursor!=i || subMode!=SUBMODE_QUEST)
				c=240;
			else if(subMode==SUBMODE_QUEST)
				c=32*5+16;
			if(player.var[VAR_QUESTDONE+i+questPage*10])
				PauseBox(x,y,x+405,y+28,c-4);
			else
				PauseBox(x,y,x+405,y+28,c);

			if(player.var[VAR_QUESTDONE+i+questPage*10])
			{
				PrintDark(x+12,y+2,QuestName(i+questPage*10),0);
				DrawBox(x+8,y+15,x+400,y+16,0);
			}
			else
				PrintDark(x+12,y+2,QuestName(i+questPage*10),0);
		}
		else	// don't have this quest yet
		{
			if(subcursor!=i || subMode!=SUBMODE_QUEST)
				PauseBox(x+2,y+2,x+405-2,y+28-2,230);
			else
				PauseBox(x+2,y+2,x+405-2,y+28-2,32*5+6);

			PrintDark(x+12,y+2,"- - - - - -",0);
		}

		y+=31;
	}
}

void RenderTipMenu(int x,int y)
{
	int i;
	char s[32];

	PauseBox(x+202,y+5,x+638,y+345,234);
	if(subMode==SUBMODE_TIPS)
		PauseBox(x+152,y+347,x+638,y+428,234);
	else
		PauseBox(x+202,y+347,x+638,y+428,234);
	if(((subcursor+tipPage*10)==0 || player.var[VAR_TIP+subcursor-1+tipPage*10]) && subMode==SUBMODE_TIPS)
	{
		PauseShowTip(subcursor+tipPage*10,x+156,y+352);
	}

	sprintf(s,"Tips %d/2",tipPage+1);
	PrintGlow(x+251,y+8,s,0,0);

	x+=206;
	y+=34;
	for(i=0;i<10;i++)
	{
		if((i+tipPage*10)==0 || player.var[VAR_TIP+i-1+tipPage*10])
		{
			byte c;

			if(subcursor!=i || subMode!=SUBMODE_TIPS)
				c=240;
			else if(subMode==SUBMODE_TIPS)
				c=32*5+16;
			PauseBox(x,y,x+405,y+28,c);

			PrintDark(x+12,y+2,PauseTipName(i+tipPage*10),0);
		}
		else	// don't have this tip yet
		{
			if(subcursor!=i || subMode!=SUBMODE_TIPS)
				PauseBox(x+2,y+2,x+405-2,y+28-2,230);
			else
				PauseBox(x+2,y+2,x+405-2,y+28-2,32*5+6);

			PrintDark(x+12,y+2,"? ? ? ? ? ?",0);
		}

		y+=31;
	}
}

void RenderInvItem(int x,int y,byte type,byte count,byte on)
{
	PauseBox(x,y,x+39,y+39,228*(on==0)+166*(on==1));
	if(type!=IT_NONE)
	{
		InstaRenderItem2(x+20,y+20,type,0,(on==1)*4,GetDisplayMGL());
		if(count>1)
		{
			char s[4];

			sprintf(s,"%d",count);
			RightPrintDark(x+39,y+25,s,1);
			RightPrintDark(x+37,y+23,s,1);
			RightPrint(x+38,y+24,s,0,1);
		}
	}
}

void RenderUnusableInvItem(int x,int y,byte type,byte count,byte on)
{
	PauseBox(x,y,x+39,y+39,225*(on==0)+166*(on==1));
	if(type!=IT_NONE)
	{
		InstaRenderItem2(x+20,y+20,type,0,-20+(on==1)*4,GetDisplayMGL());
		if(count>1)
		{
			char s[4];

			sprintf(s,"%d",count);
			RightPrintDark(x+39,y+25,s,1);
			RightPrintDark(x+37,y+23,s,1);
			RightPrint(x+38,y+24,s,0,1);
		}
	}
}

void RenderEquipMenu(int x,int y)
{
	int i,j;

	PauseBox(x+202,y+5,x+638,y+345,234);
	PauseBox(x+202,y+347,x+638,y+428,234);
	if(subMode==SUBMODE_EQUIP)
	{
		switch(subcursor)
		{
			case 252:
				DescribeItem(x,y,&player.axe);
				break;
			case 253:
				DescribeItem(x,y,&player.amulet);
				break;
			case 254:
				DescribeItem(x,y,&player.parka);
				break;
			case 255:
				// glasses
				DescribeGlasses(x,y);
				break;
			default:
				if(player.items[subcursor].type!=IT_NONE)
					DescribeItem(x,y,&player.items[subcursor]);
				break;
		}
	}

	PrintGlow(x+251,y+8,"Items",0,0);
	PrintGlow(x+251+139,y+43,"<- Equipped",0,0);

	x+=206;
	y+=36;

	ConvertEquipmentForShow(1);
	RenderInvItem(x,y,player.axe.type,player.axe.count,(subcursor==252));
	RenderInvItem(x+43,y,player.amulet.type,player.amulet.count,(subcursor==253));
	RenderInvItem(x+43*2,y,player.parka.type,player.parka.count,(subcursor==254));

	RenderInvItem(x+43*3,y,IT_LAMEGLASSES+(player.var[VAR_GLASSES]),1,(subcursor==255));

	for(i=0;i<10;i++)
		for(j=0;j<6;j++)
		{
			RenderInvItem(x+i*43,y+48+j*43,player.items[i+j*10].type,player.items[i+j*10].count,(subcursor==i+j*10));
		}
	ConvertEquipmentForShow(0);
}

void RenderYesNo(int x,int y)
{
	PauseBox(x+130,y+80,x+630,y+240,234);

	CenterPrintGlow(x+380,y+100,askingTxt,0,0);

	if(yesno==0)
		PauseBox(x+240,y+200,x+370,y+235,32*5+16);
	else
		PauseBox(x+240,y+200,x+370,y+235,240);
	CenterPrintDark(x+305,y+204,"Yes",0);
	if(yesno==1)
		PauseBox(x+390,y+200,x+520,y+235,32*5+16);
	else
		PauseBox(x+390,y+200,x+520,y+235,240);
	CenterPrintDark(x+455,y+204,"No",0);
}

void PlayerStat(const char *txt,int val,int y)
{
	char s[32];
	int x1,x2,wid,i;

	sprintf(s,"%d",val);
	x1=((8+GetStrLength(txt,1)+4)/6)*6;
	x2=((196-GetStrLength(s,1))/6)*6;
	wid=6;
	PrintGlow(8,y,txt,0,1);
	RightPrintGlow(196,y,s,0,1);
	for(i=x1;i<x2;i+=wid)
		PrintGlow(i,y,".",0,1);
}

void PlayerSlashStat(const char *txt,int val,int val2,int y)
{
	char s[32];
	int x1,x2,wid,i;

	sprintf(s,"%d/%d",val,val2);
	x1=((8+GetStrLength(txt,1)+4)/6)*6;
	x2=((196-GetStrLength(s,1))/6)*6;
	wid=6;
	PrintGlow(8,y,txt,0,1);
	RightPrintGlow(196,y,s,0,1);
	for(i=x1;i<x2;i+=wid)
		PrintGlow(i,y,".",0,1);
}

void RenderPlayerStats(MGLDraw *mgl)
{
	word d;

	PauseBox(0,0,200,173,230);

	PlayerStat("Level",player.level,4);
	if(player.level<50)
		PlayerSlashStat("XP",player.xp,player.xpNeed,16);
	else
		PlayerStat("XP",player.xp,16);
	PlayerStat("Skill Pts",player.skillPts,28);
	if(player.axeMode==1)
	{
		d=((((float)player.damage)/2.0f+0.5f));
		if(d<1)
			d=1;
	}
	else
		d=player.damage;
	d=DamageAmt(d,GOOD);

	PlayerStat("Damage",d,40+6);
	d=(player.armor*(100+10*player.var[VAR_MADGEM+MC_SHIELD])/100);
	if(player.parka.magic==EM_OLDLACE && goodguy->poison)
		d+=5;
	d=(word)(((float)d*(100.0f+TalentBonus(TLT_STONESKIN))/100.0f)+0.5f);
	PlayerStat("Defense",d,52+6);
	PlayerStat("Axe Speed",player.axeSpeed,64+6);
	PlayerStat("Attack Cost",StaminaCost(),76+6);
	PlayerSlashStat("Life",player.hearts,player.maxHearts,88+6*2);
	PlayerSlashStat("Stamina",player.stamina,player.maxStamina,100+6*2);
	PlayerSlashStat("Magic",player.magic,player.maxMagic,112+6*2);
	PlayerStat("Money",player.money,124+6*3);
	if(player.var[VAR_MADCAP])
		PlayerStat("Crystal Pts",player.var[VAR_CPOINTS],136+6*3);
}

void RenderPauseMenu(MGLDraw *mgl)
{
	int i;

	DarkenScreen(darkness);

	RenderPlayerStats(mgl);

	PauseBox(0,175,200,478,234);

	for(i=0;i<PAUSE_CHOICES;i++)
	{
		if(cursor==i)
			PrintGlow(10,180+i*30,pauseOption[i],0-16*(subMode!=0),0);
		else
			PrintDark(10,180+i*30,pauseOption[i],0);
	}

	if(cursor==PAUSE_QUESTS)
		RenderQuestMenu(0+offX,-offX-5);
	if(cursor==PAUSE_TIPS)
		RenderTipMenu(0+offX,-offX-5);
	if(cursor==PAUSE_SKILLS)
		RenderSkillMenu(offX,-offX-5);
	if(cursor==PAUSE_ITEMS)
	{
		RenderEquipMenu(offX,-offX-5);
		if(subMode==SUBMODE_EQUIPYESNO)
			RenderYesNo(offX,-offX-5);
	}
	if(cursor==PAUSE_MESSAGES)
		RenderMessagesPause(offX,-offX-5);
	if(cursor==PAUSE_TALENTS)
		RenderTalentMenu(offX,-offX-5);
	if(cursor==PAUSE_MADCAP)
		RenderMadcapMenu(offX,-offX-5);
	if(cursor==PAUSE_OPTIONS)
	{
		RenderOptionsMenu(offX,-offX-5);
		if(subMode==SUBMODE_SETKEY)
			RenderSetKey(offX,-offX-5,0);
		else if(subMode==SUBMODE_SETBTN)
			RenderSetKey(offX,-offX-5,1);
	}

	Print(210,430,helpTxt[0],0,1);
	Print(210,442,helpTxt[1],0,1);
	Print(210,454,helpTxt[2],0,1);
	Print(210,466,helpTxt[3],0,1);
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void LoadGame(void)
{
	FILE *f;
	char txt[64];


	sprintf(txt,"profiles/char%02d.loony",gameToLoad+1);
	f=fopen(txt,"rb");
	if(!f)
	{
		InitPlayer(INIT_GAME,0,0);
	}
	else
	{
		fread(&player,sizeof(player_t),1,f);
		if(player.addonName[0]=='\0')
		{
			ResetLevelDefs();
			LoadWorld(&curWorld,"winter.llw");
		}
		else
		{
			sprintf(txt,"addons/%s.llw",player.addonName);
			LoadLevelDefs(player.addonName);
			LoadWorld(&curWorld,txt);
		}
		LoadGuys(f);
		LoadBullets(f);
		if(!curMap)
			curMap=new Map(20,20,"hi");
		curMap->LoadFromProgress(f);
		fclose(f);
		ResetInterface();
		PlayerCalcStats();
		if(player.invinc<30)
			player.invinc=30;	// and make you invincible briefly
	}
}

void SaveGame(void)
{
	FILE *f;
	char txt[64];

	sprintf(txt,"profiles/char%02d.loony",gameToLoad+1);
	f=fopen(txt,"wb");
	if(!f)
	{
		return;
	}
	else
	{
		player.destx=goodguy->mapx;
		player.desty=goodguy->mapy;
		fwrite(&player,sizeof(player_t),1,f);
		player.destx=0;
		player.desty=0;
		SaveGuys(f);
		SaveBullets(f);
		curMap->SaveProgress(f);
		fclose(f);
	}
}

void SetupSkillPage(void)
{
	int i;
	byte p;

	for(i=0;i<10;i++)
	{
		skillPtr[i]=255;	// empty all slots
		for(p=0;p<PLAYER_SKILLS;p++)
			if(player.skillHave[p]==(i+skillPage+1))
			{
				skillPtr[i]=p;
			}
	}
}

void InitPauseMenu(void)
{
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	subMode=0;
	cursor=0;
	darkness=0;
	offX=400;
	oldc=255;
	if(!player.var[VAR_MADCAP])
		strcpy(pauseOption[PAUSE_MADCAP],"");
	else
		strcpy(pauseOption[PAUSE_MADCAP],"Madcap Items");

	statClock[0]=0;
	statClock[1]=30;
	statClock[2]=30*2;
	BuildStatText();
	SetupSkillPage();
	SetHelpText();
	TipEvent(TIP_QUIT);
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;
	int i,j;

	if(darkness<30)
		darkness++;
	if(offX>0)
	{
		offX-=32;
		if(offX<0)
			offX=0;
	}

	for(i=0;i<3;i++)
	{
		statClock[i]++;
		if(statClock[i]==30*3)
		{
			SwapStatText(i*2);
			statClock[i]=0;
		}
	}

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if(subMode==SUBMODE_NONE)	// not in any submenu
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			cursor--;
			subcursor=0;
			if(cursor==255)
				cursor=PAUSE_CHOICES-1;
			if(cursor==PAUSE_MADCAP && player.var[VAR_MADCAP]==0)
				cursor--;
			offX=400;
			SetHelpText();
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			cursor++;
			subcursor=0;
			if(cursor==PAUSE_CHOICES)
				cursor=0;
			if(cursor==PAUSE_MADCAP && player.var[VAR_MADCAP]==0)
				cursor++;
			offX=400;
			SetHelpText();
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))))
		{
			switch(cursor)
			{
				case PAUSE_RESUME: // cancel
					return PAUSE_RESUME;
					break;
				case PAUSE_SKILLS:
					subMode=SUBMODE_SKILL;
					subcursor=0;
					SetHelpText();
					break;
				case PAUSE_TALENTS:
					subMode=SUBMODE_TALENT;
					subcursor=0;
					SetHelpText();
					break;
				case PAUSE_ITEMS:
					subMode=SUBMODE_EQUIP;
					subcursor=0;
					SetHelpText();
					break;
				case PAUSE_QUESTS:
					subMode=SUBMODE_QUEST;
					subcursor=0;
					SetHelpText();
					break;
				case PAUSE_TIPS:
					subMode=SUBMODE_TIPS;
					subcursor=0;
					SetHelpText();
					break;
				case PAUSE_MADCAP:
					if(player.var[VAR_MADCAP])
					{
						subMode=SUBMODE_MADCAP;
						subcursor=0;
						SetHelpText();
					}
					break;
				case PAUSE_OPTIONS:
					subMode=SUBMODE_OPTIONS;
					subcursor=0;
					SetHelpText();
					break;
				case PAUSE_QUIT: // quit game
					SaveGame();
					return PAUSE_QUIT;
			}
		}
		if(((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			return PAUSE_RESUME;
		}
	}
	else if(subMode==SUBMODE_SKILL)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==255)
				subcursor=9;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==10)
				subcursor=0;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			skillPage-=10;
			if(skillPage>PLAYER_SKILLS)
				skillPage=PLAYER_SKILLS-10;
			SetupSkillPage();
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			skillPage+=10;
			if(skillPage>=PLAYER_SKILLS)
				skillPage=0;
			SetupSkillPage();
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			// add a point to a skill
			if(skillPtr[subcursor]!=255 && player.skillHave[skillPtr[subcursor]] && player.skillPts>0 && player.skillLvl[skillPtr[subcursor]]<10)
			{
				player.skillClock[skillPtr[subcursor]]=0;
				player.skillLvl[skillPtr[subcursor]]++;
				player.skillPts--;
				if(skillPtr[subcursor]==SKILL_THROWING)
					TipEvent(TIP_THROWING);
				if(skillPtr[subcursor]>=SKILL_SHROOM && skillPtr[subcursor]<=SKILL_BONEBOMB)
					TipEvent(TIP_SPELL);
				MakeNormalSound(SND_MENUSELECT);
				PlayerCalcStats();
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the skill menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
	}
	else if(subMode==SUBMODE_OPTIONS)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
#ifdef DIRECTORS
			if(opt.modifier[29]==0 && subcursor==2)
				subcursor--;
#else
			if(subcursor==2)
				subcursor--;
#endif
			if(subcursor>2)
				subcursor-=2;
			if(subcursor==5 || subcursor==8 || subcursor==11 || subcursor==14)	// invalid spots
				subcursor--;
			if(subcursor>23)
				subcursor=21;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
#ifdef DIRECTORS
			if(opt.modifier[29]==0 && subcursor==2)
				subcursor++;
#else
			if(subcursor==2)
				subcursor++;
#endif
			if(subcursor>3)
				subcursor+=2;
			if(subcursor>23)
				subcursor=0;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			if(subcursor>2 && subcursor<15)	// in the arrow key area
			{
				if(subcursor%3)
					subcursor--;
				else
					subcursor++;
			}
			else if(subcursor>14)
			{
				if(!(subcursor%3))
					subcursor+=2;
				else
					subcursor--;
			}
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			if(subcursor>2 && subcursor<15)	// in the arrow key area
			{
				if(subcursor%3)
					subcursor--;
				else
					subcursor++;
			}
			else if(subcursor>14)
			{
				if((subcursor%3)==2)
					subcursor-=2;
				else
					subcursor++;
			}
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			switch(subcursor)
			{
				case 0:	// sound
					if(player.var[VAR_SOUND]>=127)
						player.var[VAR_SOUND]=0;
					else
					{
						player.var[VAR_SOUND]+=26;
						if(player.var[VAR_SOUND]>127)
							player.var[VAR_SOUND]=127;
					}
					JamulSoundVolume(player.var[VAR_SOUND]*2);
					MakeNormalSound(SND_MENUSELECT);
					break;
				case 1:	// music
					if(player.var[VAR_MUSIC]>=127)
						player.var[VAR_MUSIC]=0;
					else
					{
						player.var[VAR_MUSIC]+=26;
						if(player.var[VAR_MUSIC]>127)
							player.var[VAR_MUSIC]=127;
					}
					SetMusicVolume(player.var[VAR_MUSIC]*2);
					MakeNormalSound(SND_MENUSELECT);
					break;
				case 2:	// commentary
#ifdef DIRECTORS
					if(opt.modifier[29])
					{
						player.var[VAR_COMMENTARY]=1-player.var[VAR_COMMENTARY];
						MakeNormalSound(SND_MENUSELECT);
						StopSong();
						Song(levelDef[player.levelNum].song);
					}
#endif
					break;
				default:	// key setting
					if((subcursor%3)==2)	// joystick
					{
						subMode=SUBMODE_SETBTN;
						mgl->ClearKeys();
						i=0;
						while(i<10000 && GetJoyButtons())
						{
							i++;
							SDL_Delay(1);
						}
						MakeNormalSound(SND_MENUSELECT);
					}
					else
					{
						subMode=SUBMODE_SETKEY;
						mgl->ClearKeys();
						mgl->LastKeyPressed();
						LastScanCode();
						lastKey=0;
						MakeNormalSound(SND_MENUSELECT);
					}
					break;
			}
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the options menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
	}
	else if(subMode==SUBMODE_SETKEY)
	{
		byte r;

		r=LastScanCode();
		if(r==59)
		{
			subMode=SUBMODE_OPTIONS;	// escape
			MakeNormalSound(SND_MENUCANCEL);
			mgl->LastKeyPressed();
		}
		else if(r!=0)
		{
			for(i=0;i<7;i++)
				for(j=0;j<2;j++)
					if(player.var[VAR_CONTROLS+j*8+i]==r)
						player.var[VAR_CONTROLS+j*8+i]=0;
			player.var[VAR_CONTROLS+(subcursor%3)*8+((subcursor-3)/3)]=r;
			subMode=SUBMODE_OPTIONS;
			MakeNormalSound(SND_MENUSELECT);
			ApplyControlSettingsMem(&player.var[VAR_CONTROLS]);
			SaveOptions();
		}
	}
	else if(subMode==SUBMODE_SETBTN)
	{
		dword btn;

		btn=GetJoyButtons();

		j=1;
		for(i=0;i<16;i++)
		{
			if((btn&j))
			{
				player.var[VAR_CONTROLS+16+(subcursor-17)/3]=i;
				subMode=SUBMODE_OPTIONS;
				MakeNormalSound(SND_MENUSELECT);
				ApplyControlSettingsMem(&player.var[VAR_CONTROLS]);
				SaveOptions();
			}
			j*=2;
		}
	}
	else if(subMode==SUBMODE_MADCAP)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==255)
				subcursor=10;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==11)
				subcursor=0;
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			// add a madcap crystal
			if(player.money>=MADCAP_PRICE*(player.var[VAR_MADGEM+subcursor]+1) && player.var[VAR_CPOINTS]>=CrystalCost(subcursor) && player.var[VAR_MADGEM+subcursor]<100)
			{
				player.var[VAR_CPOINTS]-=CrystalCost(subcursor);
				player.money-=(MADCAP_PRICE*(player.var[VAR_MADGEM+subcursor]+1));
				player.var[VAR_MADGEM+subcursor]++;
				MakeNormalSound(SND_MENUSELECT);
				PlayerCalcStats();
				j=0;
				for(i=0;i<11;i++)
				{
					if(player.var[VAR_MADGEM+i]==100)
						j++;
				}
				if(j==11)
					AchieveNow(99);
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the madcap menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
	}
	else if(subMode==SUBMODE_TALENT)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==255)
				subcursor=9;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==10)
				subcursor=0;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			talentPage-=10;
			if(talentPage>20)
				talentPage=20;
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			talentPage+=10;
			if(talentPage>20)
				talentPage=0;
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the talent menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
	}
	else if(subMode==SUBMODE_EQUIP)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			if(subcursor<10)
			{
				if(subcursor>=3)
					subcursor=255;
				else
					subcursor=252+subcursor;
			}
			else if(subcursor>=252)
				subcursor=50+subcursor-252;
			else
				subcursor-=10;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			if(subcursor>=252)
				subcursor=subcursor-252;
			else if(subcursor>=50)
			{
				if(subcursor>=53)
					subcursor=255;
				else
					subcursor=252+subcursor-50;
			}
			else
				subcursor+=10;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			if(subcursor>=252)
			{
				subcursor--;
				if(subcursor==251)
					subcursor=255;
			}
			else if((subcursor%10)==0)
				subcursor=subcursor+9;
			else
				subcursor--;
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			if(subcursor>=252)
			{
				subcursor++;
				if(subcursor==0)
					subcursor=252;
			}
			else if((subcursor%10)==9)
				subcursor=subcursor-9;
			else
				subcursor++;
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			equip_t e;
			if(subcursor<60)
			{
				if(player.items[subcursor].type==IT_AXE || player.items[subcursor].type==IT_MAGICAXE || player.items[subcursor].type==IT_GOLDENAXE)
				{
					e=player.items[subcursor];
					player.items[subcursor].count--;
					if(player.items[subcursor].count==0)
						player.items[subcursor].type=IT_NONE;

					if(!SimpleEquipAdd(&player.axe))
					{
						// no room to unequip!
						player.items[subcursor]=e;
						MakeNormalSound(SND_MENUCANCEL);
					}
					else
					{
						player.axe=e;
						player.axe.count=1;
						MakeNormalSound(SND_MENUSELECT);
					}
					MakeNormalSound(SND_EQUIP);
					subcursor=252;
					SortEquipment();
					PlayerCalcStats();
				}
				else if(player.items[subcursor].type==IT_AMULET || player.items[subcursor].type==IT_MAGICAMULET || player.items[subcursor].type==IT_GOLDENAMULET)
				{
					e=player.items[subcursor];
					player.items[subcursor].count--;
					if(player.items[subcursor].count==0)
						player.items[subcursor].type=IT_NONE;

					if(!SimpleEquipAdd(&player.amulet))
					{
						// no room to unequip!
						player.items[subcursor]=e;
						MakeNormalSound(SND_MENUCANCEL);
					}
					else
					{
						player.amulet=e;
						player.amulet.count=1;
						MakeNormalSound(SND_MENUSELECT);
					}
					MakeNormalSound(SND_EQUIP);
					subcursor=253;
					SortEquipment();
					PlayerCalcStats();
				}
				else if(player.items[subcursor].type==IT_PARKA || player.items[subcursor].type==IT_MAGICPARKA || player.items[subcursor].type==IT_GOLDENPARKA)
				{
					e=player.items[subcursor];
					player.items[subcursor].count--;
					if(player.items[subcursor].count==0)
						player.items[subcursor].type=IT_NONE;

					if(!SimpleEquipAdd(&player.parka))
					{
						// no room to unequip!
						player.items[subcursor]=e;
						MakeNormalSound(SND_MENUCANCEL);
					}
					else
					{
						player.parka=e;
						player.parka.count=1;
						MakeNormalSound(SND_MENUSELECT);
					}
					MakeNormalSound(SND_EQUIP);
					subcursor=254;
					SortEquipment();
					PlayerCalcStats();
				}
				else if(player.items[subcursor].type>=IT_LENSA && player.items[subcursor].type<=IT_LENSM && player.var[VAR_GLASSES])
				{
					MakeNormalSound(SND_MENUSELECT);
					if(AddLens(player.items[subcursor].type-IT_LENSA))
					{
						DropItem(subcursor);
						SortEquipment();
						MakeNormalSound(SND_EQUIP);
						if(player.lens[0]!=255 && player.lens[1]!=255)
							subcursor=255;	// move cursor to the glasses to see what you've wrought
						PlayerCalcStats();
					}
					else	// can't add it
					{
						subMode=SUBMODE_EQUIPYESNO;
						SetHelpText();
						sprintf(askingTxt,"Break equipped lenses for new ones?");
						asking=1;
						yesno=1;
					}
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
				if(player.axe.magic>=EM_ARTSTART && player.axe.magic<=EM_ARTEND &&
				   player.amulet.magic>=EM_ARTSTART && player.amulet.magic<=EM_ARTEND &&
				   player.parka.magic>=EM_ARTSTART && player.parka.magic<=EM_ARTEND)
					AchieveNow(55);
				if(player.lens[0]==7 && player.lens[1]==12)
					AchieveNow(77);

			}
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the equip menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
		if((c&CONTROL_B3) && (!(oldc&CONTROL_B3)))
		{
			if(subcursor<MAX_EQUIP && ItemCost(&player.items[subcursor],1))
			{
				subMode=SUBMODE_EQUIPYESNO;
				SetHelpText();
				sprintf(askingTxt,"Destroy %s?",EquipName(&player.items[subcursor]));
				asking=0;
				yesno=1;
				MakeNormalSound(SND_MENUSELECT);
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}
	}
	else if(subMode==SUBMODE_EQUIPYESNO)
	{
		if((c&CONTROL_LF) && (!reptCounter))
		{
			yesno=1-yesno;
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			yesno=1-yesno;
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			if(yesno==0)	// yes!
			{
				if(asking==0)	// delete the item?
				{
					MakeNormalSound(SND_BOOM);
					DropItem(subcursor);
					SortEquipment();
				}
				else	// trash your glasses and equip this new lens
				{
					player.lens[0]=255;
					player.lens[1]=255;
					AddLens(player.items[subcursor].type-IT_LENSA);
					DropItem(subcursor);
					MakeNormalSound(SND_EQUIP);
					SortEquipment();
				}
			}
			subMode=SUBMODE_EQUIP;
			SetHelpText();
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the yesno
			subMode=SUBMODE_EQUIP;
			SetHelpText();
		}
	}
	else if(subMode==SUBMODE_QUEST)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==255)
				subcursor=9;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==10)
				subcursor=0;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			questPage--;
			if(questPage>4)
				questPage=4;
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			questPage++;
			if(questPage>4)
				questPage=0;
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) || ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			// exit the quest menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
	}
	else if(subMode==SUBMODE_TIPS)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==255)
				subcursor=9;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==10)
				subcursor=0;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			tipPage=1-tipPage;
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			tipPage=1-tipPage;
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) || ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			// exit the tip menu
			subMode=SUBMODE_NONE;
			SetHelpText();
		}
	}

	oldc=c;

	HandlePauseKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit pause menu
	{
		if(subMode==SUBMODE_NONE)
			return PAUSE_RESUME;
		else if(subMode==SUBMODE_SETKEY || subMode==SUBMODE_SETBTN)
			subMode=SUBMODE_OPTIONS;
		else if(subMode==SUBMODE_EQUIPYESNO)
			subMode=SUBMODE_EQUIP;
		else
			subMode=SUBMODE_NONE;
		lastKey=0;
	}
	return PAUSE_PAUSED;
}
