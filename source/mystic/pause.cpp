#include "pause.h"
#include "player.h"
#include "shop.h"
#include "options.h"
#include "challenge.h"
#include "appdata.h"
#include "spell.h"
#include "skills.h"
#include "runes.h"

#define SUBMODE_NONE	 0
#define SUBMODE_SLOTPICK 1
#define SUBMODE_SKILLS	 2
#define SUBMODE_QOL		 3
#define SUBMODE_RUNES	 4

byte cursor=0;
static byte subcursor=0,effCursor=0;
static char lastKey=0;
byte subMode;
static byte giveUp=0;	// which text should be shown for "Give Up"
int pauseX=-250,subX=-250;
byte saveLevel[5],saveNightmare[5];
byte saveChapter[5];
byte saveHour[5],saveMin[5],savePct[5];
Difficulty saveDiff[5];
static byte armaBrt=0;

void ResetPauseMenu(void)
{
	pauseX=-250;
	subX=-250;
	cursor=0;
}

void SetSubCursor(byte s)
{
	subcursor=s;
}

void PrintPauseOption(int y, byte curSpot, const char* s)
{
	if(cursor==curSpot)
		PrintBright(pauseX + 10+2, y+2, s, -31, 2);
	PrintBrightGlow(pauseX + 10, y, s, -16 + (cursor == curSpot) * 16, 2);
}

void RenderPauseMenu(void)
{
	char onoff[6][8] = { "Off","I","II","III","IV","V" };
	if(pauseX==-250)
		return;

	if(Challenging())
		PauseChallengeRender(pauseX);

	RenderPlayerGear(440-pauseX*2,armaBrt);

	GetShopSpr(34)->Draw(pauseX,230,GetDisplayMGL());
	if (ClassicMode())
	{
		PrintBrightGlow(pauseX + 10, 240, "Cancel", -16 + (cursor == 0) * 16, 0);
		if (!Challenging())
		{
			PrintBrightGlow(pauseX + 10, 280, "Load", -16 + (cursor == 1) * 16, 0);
		}
		if (giveUp == 0)
			PrintBrightGlow(pauseX + 10, 320, "Save", -16 + (cursor == 2) * 16, 0);
		else if (giveUp == 2)	// random battle
			PrintBrightGlow(pauseX + 10, 320, "Run Away", -16 + (cursor == 2) * 16, 0);
		else 	// regular level
			PrintBrightGlow(pauseX + 10, 320, "Give Up", -16 + (cursor == 2) * 16, 0);

		PrintBrightGlow(pauseX + 10, 360, "Music:", -16 + (cursor == 3) * 16, 0);
		PrintBrightGlow(pauseX + 10 + 160, 360, onoff[opt.musicVol], -16 + (cursor == 3) * 16, 0);
		PrintBrightGlow(pauseX + 10, 400, "Quit Game", -16 + (cursor == 4) * 16, 0);
		RenderSlotPickMenu();
	}
	else // modern menu
	{
		int hgt = 23;
		PrintPauseOption(240, 0, "Cancel");
		char s[32];
		sprintf(s, "Skills (%d)", player.skillPts);
		PrintPauseOption(240 + hgt * 1, 1, s);
		PrintPauseOption(240 + hgt * 2, 2, "Runes");
		if(!Challenging())
			PrintPauseOption(240 + hgt * 3, 3, "Load");

		if (giveUp == 0)
			PrintPauseOption(240 + hgt * 4, 4, "Save");
		else if (giveUp == 2)	// random battle
			PrintPauseOption(240 + hgt * 4, 4, "Run Away");
		else 	// regular level
			PrintPauseOption(240 + hgt * 4, 4, "Give Up");

		sprintf(s, "Sound: %s", onoff[opt.soundVol]);
		PrintPauseOption(240 + hgt * 5, 5, s);
		sprintf(s, "Music: %s", onoff[opt.musicVol]);
		PrintPauseOption(240 + hgt * 6, 6, s);
		PrintPauseOption(240 + hgt * 7, 7, "Weird Settings");
		PrintPauseOption(240 + hgt * 8, 8, "Quit Game");

		RenderSlotPickMenu();
		if(subMode==SUBMODE_SKILLS)
			RenderSkillMenu();
		if (subMode == SUBMODE_QOL)
			RenderWeirdMenu();
		if (subMode == SUBMODE_RUNES)
			RenderRuneMenu();
	}
}

void RenderSlotPickMenu(void)
{
	int i;
	char s[32];

	GetShopSpr(34)->Draw(subX,40,GetDisplayMGL());

	for(i=0;i<5;i++)
	{
		s[0]='1'+i;
		s[1]='\0';
		if(subcursor==i)
		{
			PrintBrightGlow(subX+10,50+i*40,s,0,0);
			if(saveChapter[i]==0 || saveLevel[i]==0)
			{
				PrintBrightGlow(subX+40,50+i*40,"Empty Slot",0,2);
			}
			else
			{
				if(!saveNightmare[i])
					sprintf(s,"Chapter %d",saveChapter[i]);
				else
					sprintf(s,"Chapter %d!!!",saveChapter[i]);
				if (saveDiff[i] == Difficulty::CLASSIC)
					strcat(s, " [C]");
				else if (saveDiff[i] == Difficulty::MODERN)
					strcat(s, " [M]");
				else if (saveDiff[i] == Difficulty::BRUTAL_CLASSIC)
					strcat(s, " [BC]");
				else if (saveDiff[i] == Difficulty::BRUTAL_MODERN)
					strcat(s, " [BM]");
				PrintBrightGlow(subX+40,50+i*40,s,0,2);
				sprintf(s,"%02d:%02d  L%02d %d%%",saveHour[i],saveMin[i],saveLevel[i],savePct[i]);
				PrintBrightGlow(subX+40,50+i*40+20,s,0,2);
			}
		}
		else
		{
			PrintBright(subX+10,50+i*40,s,-32,0);
			if(saveChapter[i]==0 || saveLevel[i]==0)
			{
				PrintBright(subX+40,50+i*40,"Empty Slot",-32,2);
			}
			else
			{
				if(!saveNightmare[i])
					sprintf(s,"Chapter %d",saveChapter[i]);
				else
					sprintf(s,"Chapter %d!!!",saveChapter[i]);
				if (saveDiff[i] == Difficulty::CLASSIC)
					strcat(s, " [C]");
				else if (saveDiff[i] == Difficulty::MODERN)
					strcat(s, " [M]");
				else if (saveDiff[i] == Difficulty::BRUTAL_CLASSIC)
					strcat(s, " [BC]");
				else if (saveDiff[i] == Difficulty::BRUTAL_MODERN)
					strcat(s, " [BM]");
				PrintBright(subX+40,50+i*40,s,-32,2);
				sprintf(s,"%02d:%02d  L%02d %d%%",saveHour[i],saveMin[i],saveLevel[i],savePct[i]);
				PrintBright(subX+40,50+i*40+20,s,-32,2);
			}
		}
	}
}

void RenderSkillBox(int x, int y, int x2, int y2, byte outC, byte inC)
{
	DrawBox(x + 1, y, x2 - 1, y2, outC);
	DrawBox(x, y+1, x2, y2-1, outC);
	DrawFillBox(x + 1, y + 1, x2 - 1, y2 - 1, inC);
}

void RenderSkillMenu(void)
{
	int i;
	char s[32];

	RenderSkillBox(SCRWID / 2, 30, SCRWID - 2, SCRHEI - 30, 5*32+20, 5*32+8);
	RenderSkillBox(SCRWID / 2 + 2, 30 + 2, SCRWID - 2 - 2, SCRHEI - 30 - 2, 5*32+16, 5*32+8);

	PrintBrightGlow(SCRWID / 2 + 15, 32, "Mystic Skills",0,0);
	int x, y;
	int spacing = 50;
	x = SCRWID / 2 + SCRWID / 4 - spacing*3+5;
	y = 70;

	for (int j = 0; j < 6; j++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (skillList[i + j * 6].spellReq != 255 && player.spell[skillList[i + j * 6].spellReq] == 0)
			{
				if (subcursor == i + j * 6)
				{
					RenderSkillBox(x-2, y-2, x + 41, y + 41, 32 * 5 + 31, 32*5+8);
					RenderSkillBox(x, y, x + 39, y + 39, 32 * 5 + 31, 3);
					BlitIconGlow(36, x + 4, y + 4, 0);

					DescribeSkill(i + j * 6, SCRWID / 2 + 10, SCRHEI - 30 - 85);
				}
				else
				{
					RenderSkillBox(x, y, x + 39, y + 39, 32 * 5 + 16, 0);
					BlitIconGlow(36, x + 4, y + 4, -15);
				}
			}
			else
			{
				if (subcursor == i + j * 6)
				{
					RenderSkillBox(x - 2, y - 2, x + 41, y + 41, 32 * 7 + 31, 32 * 7 + 8);
					RenderSkillBox(x, y, x + 39, y + 39, 32 * 5 + 31, 32 * 7 + 3 + player.skill[i + j * 6] * 2);
					BlitIconGlow(i + j * 6, x + 4, y + 4, 0);

					DescribeSkill(i + j * 6, SCRWID / 2 + 10, SCRHEI - 30 - 85);
				}
				else
				{
					RenderSkillBox(x, y, x + 39, y + 39, 32 * 5 + 16, player.skill[i + j * 6] ? 32 * 7 + player.skill[i + j * 6] * 2 : 0);
					BlitIconGlow(i + j * 6, x + 4, y + 4, -15);
				}
				for (int k = 0; k < 5; k++)
				{
					int hgt = abs(2 - k) * 2;
					if (k == 2) hgt = 1;
					RenderSkillBox(x + 1 + k * 8, y + 39 - hgt, x + 1 + k * 8 + 5, y + 39 - hgt + 5, 32 * 5 + 16 + 15 * (subcursor == i + j * 6), 32 * 7 + 3 + (player.skill[i + j * 6] > k) * (10 + 8 * (subcursor == i + j * 6)));
				}
			}
			
			x += spacing;
		}
		x = SCRWID / 2 + SCRWID / 4 - spacing * 3+5;
		y += spacing;
	}

	sprintf(s, "Points: %02d", player.skillPts);
	PrintBrightGlow(SCRWID - 80, SCRHEI - 35 - 75, s, 31, 1);

	if (giveUp == 0)	// only in the overworld
	{
		// reset skills
		if (subcursor == 36)
			RenderSkillBox(SCRWID - 80, SCRHEI - 35 - 60, SCRWID - 10, SCRHEI - 35 - 40, 32 * 5 + 31, 32 * 5 + 10);
		else
			RenderSkillBox(SCRWID - 80, SCRHEI - 35 - 60, SCRWID - 10, SCRHEI - 35 - 40, 32 * 5 + 16, 32 * 5 + 6);
		PrintBrightGlow(SCRWID - 80 + 4 + 6, SCRHEI - 35 - 60, "Reset", (subcursor == 36) * 31, 2);
	}

	// exit
	if (subcursor == 37)
		RenderSkillBox(SCRWID - 80, SCRHEI - 35 - 35, SCRWID - 10, SCRHEI - 35 - 15, 32 * 5 + 31, 32 * 5 + 10);
	else
		RenderSkillBox(SCRWID - 80, SCRHEI - 35 - 35, SCRWID - 10, SCRHEI - 35 - 15, 32 * 5 + 16, 32 * 5 + 6);
	PrintBrightGlow(SCRWID - 80 + 4+12, SCRHEI - 35 - 35, "Exit", (subcursor == 37) * 31, 2);
}

void RenderRuneMenu(void)
{
	int i;
	char s[32];

	RenderSkillBox(SCRWID / 2, 30, SCRWID - 2, SCRHEI - 30, 5 * 32 + 20, 5 * 32 + 8);
	RenderSkillBox(SCRWID / 2 + 2, 30 + 2, SCRWID - 2 - 2, SCRHEI - 30 - 2, 5 * 32 + 16, 5 * 32 + 8);

	PrintBrightGlow(SCRWID / 2 + 15, 32, "Mystic Runes", 0, 0);
	int x, y;
	int spacing = 50;
	x = SCRWID / 2 + SCRWID / 4 - spacing * 3 + 5;
	y = 70;

	for (int i = 0; i < (int)Rune::NUM_RUNES; i++)
	{
		
		if (RuneLevel((Rune)i) ==RUNE_EMPTY)
		{
			if (subcursor == i)
			{
				BlitIcon(40+(i%4), x + 4, y + 4 - 2, 0, -31);
				DescribeRune((Rune)i, SCRWID / 2 + 10, SCRHEI - 30 - 85);
			}
			else
			{
				BlitIcon(40 + (i % 4), x + 4, y + 4, 0, -31);
			}
		}
		else
		{
			byte rank = RuneLevel((Rune)i);
			byte equipped = RuneEquipped((Rune)i);
			byte glow = (byte)abs(Cosine((byte)(armaBrt+i*4)) * 8 >> FIXSHIFT);
			byte col = 0;
			if(rank==RUNE_RANK1)
				col = 1;
			if (rank == RUNE_RANK2)
				col = 5;
			else if (rank == RUNE_RANK3)
				col = 6;
			
			byte stones = 1;
			if (rank == RUNE_RANK2)
				stones = 3;
			else if (rank == RUNE_RANK3)
				stones = 6;

			if(equipped)
				BlitIconBit(601,441,640,480, x+2, y+2 - 2*(subcursor==i), col, glow);
			if (rank >= RUNE_RANK1)
			{
				for (byte j = 0; j < stones; j++)
				{
					byte a = (armaBrt+i*4) * (1 + (stones == 3) + (stones == 6)) + j * (256 / stones);
					
					int srcx, srcy;
					srcx = 16 * (j % 3);
					srcy = 480 - 16;
					if(Sine(a)<0)
						BlitIconBit(srcx, srcy, srcx + 15, srcy + 15, x + 20 - 8 + (Cosine(a) * 20 >> FIXSHIFT), y + 20 - 8 + (Sine(a) * 12 >> FIXSHIFT), equipped*col, glow*equipped - 6 * (subcursor != i));
				}
			}
			if (subcursor == i)
			{
				BlitIcon(40 + (i % 4), x + 4, y + 4-2, 0, 0);
				
				DescribeRune((Rune)i, SCRWID / 2 + 10, SCRHEI - 30 - 85);
			}
			else
			{
				BlitIcon(40 + (i % 4), x + 4, y + 4, 0, -6);
			}
			
			BlitIcon(100+i, x + 4, y + 4 - 2*(subcursor==i), col, -(col==0)*8+((col!=0)*glow) - 6 * (subcursor != i));
			if (rank >= RUNE_RANK1)
			{
				for (byte j = 0; j < stones; j++)
				{
					byte a = (armaBrt+i*4)*(1+(stones==3)+(stones==6)) + j * (256 / stones);
					
					int srcx, srcy;
					srcx = 16 * (j % 3);
					srcy = 480 - 16;
					if(Sine(a)>=0)
						BlitIconBit(srcx, srcy, srcx+15, srcy+15, x + 20-8 + (Cosine(a) * 20 >> FIXSHIFT), y + 20-8 + (Sine(a) * 12 >> FIXSHIFT),equipped*col, equipped*glow - 6 * (subcursor != i));
				}
			}
		}

		// put the selection marker
		if (subcursor == i)
			BlitIconBit(558, 437, 558 + 41, 437 + 42, x, y-2, 255, 0);

		x += spacing;
		if (x > SCRWID - spacing)
		{
			x= SCRWID / 2 + SCRWID / 4 - spacing * 3 + 5;
			y += spacing;
		}
	}

	sprintf(s, "%02d", player.runeStones);
	int len=GetStrLength(s, 1);
	BlitIconBit(2, 467, 13, 477, SCRWID - 10 - len - 14, SCRHEI - 30 - 55, 255, 0);
	PrintBrightGlow(SCRWID - 10-len, SCRHEI - 30 - 55, s, 31, 1);

	// exit
	if (subcursor == 37)
	{
		RenderSkillBox(SCRWID - 80, SCRHEI - 30 - 35, SCRWID - 10, SCRHEI - 30 - 15, 32 * 5 + 31, 32 * 5 + 10);
		PrintBrightGlow(SCRWID / 2 + 10, SCRHEI - 30 - 85, "Find Silent Runes hidden in the", 0, 1);
		PrintBrightGlow(SCRWID / 2 + 10, SCRHEI - 30 - 85+ 14, "world. Press Spell to awaken", 0, 1);
		PrintBrightGlow(SCRWID / 2 + 10, SCRHEI - 30 - 85 + 14*2, "and upgrade them with", 0, 1);
		PrintBrightGlow(SCRWID / 2 + 10, SCRHEI - 30 - 85 + 14*3, "Runestones. Press Fire to equip", 0, 1);
		PrintBrightGlow(SCRWID / 2 + 10, SCRHEI - 30 - 85 + 14 * 4, "Awakened Runes.", 0, 1);
		PrintBrightGlow(SCRWID / 2 + 10, SCRHEI - 30 - 85 + 14 * 5, "3 Runes may be equipped.", 0, 1);
	}
	else
		RenderSkillBox(SCRWID - 80, SCRHEI - 30 - 35, SCRWID - 10, SCRHEI - 30 - 15, 32 * 5 + 16, 32 * 5 + 6);
	PrintBrightGlow(SCRWID - 80 + 4 + 12, SCRHEI - 30 - 35, "Exit", (subcursor == 37) * 31, 2);
}

void RenderWeirdOption(int x, int y, const char* txt, bool on, byte cursorOn,bool active)
{
	if (!active)
	{
		char txt2[32];
		strcpy(txt2, txt);
		int len = strlen(txt2);
		for (int i = 0; i < len; i++)
			txt2[i] = (rand() % 2) ? '?' : ' ';
		PrintBrightGlow(x, y, txt2, -16+32 * cursorOn, 2);
		return;
	}
	PrintBrightGlow(x, y, txt, -16+32 * cursorOn, 2);
	if(on)
		PrintBrightGlow(x+200, y, "On", -16+32 * cursorOn, 2);
	else
		PrintBrightGlow(x+200, y, "Off", -16+32 * cursorOn, 2);
}

void RenderWeirdOption2(int x, int y, const char* txt, byte on, byte cursorOn, bool active)
{
	if (!active)
	{
		char txt2[32];
		strcpy(txt2, txt);
		int len = strlen(txt2);
		for (int i = 0; i < len; i++)
			txt2[i] = (rand() % 2) ? '?' : ' ';
		PrintBrightGlow(x, y, txt2, -16 + 32 * cursorOn, 2);
		return;
	}
	PrintBrightGlow(x, y, txt, -16 + 32 * cursorOn, 2);
	if (on==QUICKCAST_CASTANDSELECT)
		PrintBrightGlow(x + 200, y, "Both", -16 + 32 * cursorOn, 2);
	else if(on==QUICKCAST_CASTONLY)
		PrintBrightGlow(x + 200, y, "Cast", -16 + 32 * cursorOn, 2);
	else if (on == QUICKCAST_SELECTONLY)
		PrintBrightGlow(x + 200, y, "Select", -16 + 32 * cursorOn, 2);
}

void RenderWeirdMenu(void)
{
	int i;
	char s[32];

	RenderSkillBox(SCRWID / 2, 30, SCRWID - 2, SCRHEI - 30, 5 * 32 + 20, 5 * 32 + 8);
	RenderSkillBox(SCRWID / 2 + 2, 30 + 2, SCRWID - 2 - 2, SCRHEI - 30 - 2, 5 * 32 + 16, 5 * 32 + 8);

	PrintBrightGlow(SCRWID / 2 + 20, 32, "Weird Stuff", 0, 0);
	int x, y;
	x = SCRWID / 2 + 15;
	y = 75;

	RenderWeirdOption(x, y, "Dmg Numbers:", !player.disableDmgNumbers, (subcursor == 0),true);
	RenderWeirdOption(x, y + 20, "Shootin' Socks:", !player.disableMoveNShoot, (subcursor == 1), (player.gear & GEAR_SOCKS));
	RenderWeirdOption(x, y + 40, "Pointy Hat:", !player.disableThorns, (subcursor == 2), (player.gear & GEAR_POINTY));
	RenderWeirdOption(x, y + 60, "Energy Storm:", !player.downgradeSpell[SPL_ENERGY], (subcursor == 3), player.spell[SPL_ENERGY]==2);
	RenderWeirdOption(x, y + 80, "Liquify:", !player.downgradeSpell[SPL_FLAME], (subcursor == 4), player.spell[SPL_FLAME]==2);
	RenderWeirdOption(x, y + 100, "Seeker Swarm:", !player.downgradeSpell[SPL_SEEKER], (subcursor == 5), player.spell[SPL_SEEKER] == 2);
	RenderWeirdOption(x, y + 120, "Ice Beam:", !player.downgradeSpell[SPL_ICE], (subcursor == 6), player.spell[SPL_ICE] == 2);
	RenderWeirdOption(x, y + 140, "Hyper Inferno:", !player.downgradeSpell[SPL_INFERNO], (subcursor == 7), player.spell[SPL_INFERNO] == 2);
	RenderWeirdOption(x, y + 160, "Summon Golem:", !player.downgradeSpell[SPL_SUMMON], (subcursor == 8), player.spell[SPL_SUMMON] == 2);
	RenderWeirdOption(x, y + 180, "Steelskin:", !player.downgradeSpell[SPL_ARMOR], (subcursor == 9), player.spell[SPL_ARMOR] == 2);
	RenderWeirdOption(x, y + 200, "Insane Rage:", !player.downgradeSpell[SPL_BERSERK], (subcursor == 10), player.spell[SPL_BERSERK] == 2);
	RenderWeirdOption(x, y + 220, "Life Spell:", !player.downgradeSpell[SPL_HEAL], (subcursor == 11), player.spell[SPL_HEAL] == 2);
	RenderWeirdOption(x, y + 240, "Sword Skulls:", !player.disableSword, (subcursor == 12), PlayerHasSword());
	RenderWeirdOption2(x, y + 260, "Quick Cast:", opt.quickCast, (subcursor == 13), true);
	PrintBrightGlow(x, y+280, "Exit", -16 + 32 * (subcursor==14), 2);

	switch (subcursor)
	{
		case 1:
			if (!(player.gear & GEAR_SOCKS))
				PrintBrightGlow(x, SCRHEI - 50, "Buy Shootin' Socks to enable this option.", 0, 1);
			break;
		case 2:
			if (!(player.gear & GEAR_POINTY))
				PrintBrightGlow(x, SCRHEI - 50, "Buy the Pointy Hat to enable this option.", 0, 1);
			break;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			if (player.spell[subcursor-3]<2)
				PrintBrightGlow(x, SCRHEI - 50, "Find an advanced spell for this option.", 0, 1);
			break;
		case 12:
			if (!PlayerHasSword())
				PrintBrightGlow(x, SCRHEI - 50, "You'll need something special for this!", 0, 1);
			break;
		case 13:
			PrintBrightGlow(x, SCRHEI - 50, "A shortcut to the Quick Cast setting.", 0, 1);
			break;
	}
}

void SetGiveUpText(byte gu)
{
	giveUp=gu;
}

void InitPauseMenu(void)
{
	player_t p;
	int i;

	lastKey=0;
	subMode=0;

	auto f = AppdataOpen("mystic.sav");
	if(!f)
	{
		for(i=0;i<5;i++)
		{
			saveLevel[i]=0;
			saveChapter[i]=0;
			saveHour[i]=0;
			saveMin[i]=0;
			saveDiff[i] = Difficulty::UNUSED;
			savePct[i] = 0;
		}
	}
	else
	{
		for(i=0;i<5;i++)
		{
			SDL_RWread(f,&p,sizeof(player_t),1);
			saveLevel[i]=p.level;
			saveChapter[i]=p.worldNum+1;
			saveHour[i]=(byte)(p.gameClock/(30*60*60));
			saveMin[i]=(byte)((p.gameClock/(30*60))%60);
			saveDiff[i] = p.difficulty;
			savePct[i] = CalcGamePercent(&p);
			if(p.nightmare)
				saveNightmare[i]=1;
			else
				saveNightmare[i]=0;
		}

		f.reset();
	}
	MakeNormalSound(SND_PAUSE);
	if(cursor==4 && (!giveUp))
		cursor=0;
	if(cursor==2 && (giveUp))
		cursor=0;

	pauseX=-250;
	subX=-250;
}

void UpdateUnPausedMenu(void)
{
	if(pauseX>-250)
		pauseX-=25;
	if(subX>-250)
		subX-=25;
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	armaBrt++;
	if(pauseX<0)
		pauseX+=25;

	if(subMode==SUBMODE_SLOTPICK)
	{
		if(subX<0)
			subX+=25;
	}
	else
	{
		if(subX>-250)
			subX-=25;
	}
	
	byte maxCursor = 4 + (!ClassicMode()) * 4;
	byte loadSpot = 1 + (!ClassicMode())*2;
	if(subMode==SUBMODE_NONE)	// not in any submenu
	{
		if(AutoRepeatTapped(CONTROL_UP))
		{
			cursor--;
			if(cursor==255)
				cursor=maxCursor;
			if(Challenging() && cursor==loadSpot)
				cursor=loadSpot-1;

			MakeNormalSound(SND_MENUCLICK);
		}
		if(AutoRepeatTapped(CONTROL_DN))
		{
			cursor++;
			if(cursor>maxCursor)
				cursor=0;
			if(Challenging() && cursor==loadSpot)
				cursor=loadSpot+1;

			MakeNormalSound(SND_MENUCLICK);
		}
		effCursor = cursor;
		if (!ClassicMode())
		{
			byte conversions[] = { 0,5,8,1,2,6,3,7,4 };
			// convert the cursor position into the classic equivalent if there is one, a new slot if not
			effCursor = conversions[cursor];
		}

		if(ButtonTapped(CONTROL_B1))
		{
			MakeNormalSound(SND_MENUSELECT);

			switch(effCursor)
			{
				case 0: // cancel
					LockOutControl(CONTROL_B1, true);
					return 0;
				case 1:	// Load
					subMode=SUBMODE_SLOTPICK;
					subcursor = 0;
					break;
				case 2: // Save
					if(giveUp)
						return 2;
					else
					{
						subMode = SUBMODE_SLOTPICK;
						subcursor = 0;
					}
					break;
				case 3:	// music
					opt.musicVol--;
					if(opt.musicVol>5)
						opt.musicVol=5;
					if(opt.musicVol==0)
						StopSong();
					else
						VolumeSong(opt.musicVol);
					if(opt.musicVol==5)
						ReplaySong();
					break;
				case 4:	// quit game
					return 3;
				case 5:	// skills
					subMode = SUBMODE_SKILLS;
					subcursor = 0;
					break;
				case 6:	// sound
					opt.soundVol--;
					if (opt.soundVol > 5)
						opt.soundVol = 5;
					VolumeSound(opt.soundVol);
					MakeNormalSound(SND_GOATSHOOT);
					break;
				case 7:	// weird settings
					subMode = SUBMODE_QOL;
					subcursor = 0;
					MakeNormalSound(SND_BERSERK);
					break;
				case 8:	// runes
					subMode = SUBMODE_RUNES;
					subcursor = 37;
					break;
			}
		}
	}
	else if(subMode==SUBMODE_SLOTPICK)
	{
		if(AutoRepeatTapped(CONTROL_UP))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor--;
			if(subcursor==255)
				subcursor=4;
		}
		if(AutoRepeatTapped(CONTROL_DN))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor++;
			if(subcursor==5)
				subcursor=0;
		}
		if (ButtonTapped(CONTROL_B2))
		{
			MakeNormalSound(SND_MENUSELECT);
			subMode = SUBMODE_NONE;
		}
		if(ButtonTapped(CONTROL_B1))
		{
			if(effCursor==1)	// Load
			{
				if(saveLevel[subcursor]==0 || saveChapter[subcursor]==0)
				{
					MakeNormalSound(SND_ACIDSPLAT);
				}
				else
				{
					SendMessageToGame(MSG_LOADGAME,0);
					PlayerLoadGame(subcursor);
					MakeNormalSound(SND_LOADGAME);
					subMode=SUBMODE_NONE;
					LockOutControl(CONTROL_B1, true);
					return 0;
				}
			}
			else if(effCursor==2)	// Save
			{
				PlayerSaveGame(subcursor);
				MakeNormalSound(SND_SAVEGAME);
				subMode=SUBMODE_NONE;
				return 0;
			}
		}
	}
	else if (subMode == SUBMODE_QOL)
	{
		if (AutoRepeatTapped(CONTROL_UP))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor--;
			if (subcursor == 255)
				subcursor = 14;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor++;
			if (subcursor > 14)
				subcursor = 0;
		}
		if (ButtonTapped(CONTROL_B2))
		{
			MakeNormalSound(SND_MENUSELECT);
			subMode = SUBMODE_NONE;
		}
		if (ButtonTapped(CONTROL_B1))
		{
			switch (subcursor)
			{
				case 0:
					MakeNormalSound(SND_MENUSELECT);
					player.disableDmgNumbers = 1 - player.disableDmgNumbers;
					break;
				case 1:
					if (player.gear & GEAR_SOCKS)
					{
						MakeNormalSound(SND_MENUSELECT);
						player.disableMoveNShoot = 1 - player.disableMoveNShoot;
					}
					else
						MakeNormalSound(SND_UNAVAILABLE);
					break;
				case 2:
					if (player.gear & GEAR_POINTY)
					{
						MakeNormalSound(SND_MENUSELECT);
						player.disableThorns = 1 - player.disableThorns;
					}
					else
						MakeNormalSound(SND_UNAVAILABLE);
					break;
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
					if (player.spell[subcursor - 3] == 2)
					{
						MakeNormalSound(SND_MENUSELECT);
						player.downgradeSpell[subcursor - 3] = 1 - player.downgradeSpell[subcursor - 3];
						if (subcursor - 3 == 6 && player.downgradeSpell[subcursor - 3] == 0)	// you are disabling the downgrade for steelskin. Need to turn off your stoneskin if this is the case, because it's super cheating
							player.stoneskin = 0;
					}
					else
						MakeNormalSound(SND_UNAVAILABLE);
					break;
				case 12:
					if (PlayerHasSword())
					{
						MakeNormalSound(SND_MENUSELECT);
						player.disableSword = 1 - player.disableSword;
					}
					else
						MakeNormalSound(SND_UNAVAILABLE);
					break;
				case 13:
					MakeNormalSound(SND_MENUSELECT);
					opt.quickCast++;
					if (opt.quickCast > QUICKCAST_CASTONLY)
						opt.quickCast = QUICKCAST_SELECTONLY;
					break;
				case 14:
					MakeNormalSound(SND_MENUSELECT);
					subMode = SUBMODE_NONE;
					break;
			}
		}
	}
	else if (subMode == SUBMODE_SKILLS)
	{
		if (AutoRepeatTapped(CONTROL_UP))
		{
			MakeNormalSound(SND_MENUCLICK);
			if (subcursor < 6)
				subcursor = 37;	// the exit button
			else if (subcursor == 37)
				subcursor--;
			else if (subcursor == 36)
				subcursor = 35;
			else
				subcursor-=6;
			if (giveUp != 0 && subcursor == 36)
				subcursor = 35;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			MakeNormalSound(SND_MENUCLICK);
			if (subcursor == 37)
				subcursor = 0;
			else if (subcursor == 36)
				subcursor++;
			else if (subcursor >= 30)
				subcursor=36;
			else
				subcursor+=6;
			if (giveUp != 0 && subcursor == 36)
				subcursor = 37;
		}
		if (AutoRepeatTapped(CONTROL_LF))
		{
			if (subcursor < 36)	// not the two bottom buttons
			{
				MakeNormalSound(SND_MENUCLICK);
				if ((subcursor % 6) == 0)
					subcursor += 5;
				else
					subcursor--;
			}
		}
		if (AutoRepeatTapped(CONTROL_RT))
		{
			if (subcursor < 36)
			{
				MakeNormalSound(SND_MENUCLICK);
				if ((subcursor % 6) == 5)
					subcursor -= 5;
				else
					subcursor++;
			}
		}
		if (ButtonTapped(CONTROL_B1))
		{
			if (subcursor == 36)
			{
				// reset skills
				ResetSkills();
				MakeNormalSound(SND_BOBBYSPIN);
			}
			else if (subcursor == 37)
			{
				MakeNormalSound(SND_MENUSELECT);
				subMode = SUBMODE_NONE;
			}
			else // a skill
			{
				if (player.skillPts > 0 && player.skill[subcursor] < MAX_SKILL_LVL && (skillList[subcursor].spellReq == 255 || player.spell[skillList[subcursor].spellReq] > 0))
				{
					MakeNormalSound(SND_GETBRAIN);
					player.skill[subcursor]++;
					player.skillPts--;
					PlayerUpdateLife();
				}
				else
					MakeNormalSound(SND_UNAVAILABLE);
			}
		}
	}
	else if (subMode == SUBMODE_RUNES)
	{
		if (AutoRepeatTapped(CONTROL_UP))
		{
			MakeNormalSound(SND_MENUCLICK);
			if (subcursor < 6)
				subcursor = 37;	// the exit button
			else if (subcursor == 37)
				subcursor=(int)Rune::NUM_RUNES-1;
			else
				subcursor -= 6;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			MakeNormalSound(SND_MENUCLICK);
			if (subcursor == 37)
				subcursor = 0;
			else if (subcursor >= (int)Rune::NUM_RUNES-6)
				subcursor = 37;
			else
				subcursor += 6;
		}
		if (AutoRepeatTapped(CONTROL_LF))
		{
			if (subcursor < 37)
			{
				MakeNormalSound(SND_MENUCLICK);
				if ((subcursor % 6) == 0)
					subcursor += 5;
				else
					subcursor--;
			}
		}
		if (AutoRepeatTapped(CONTROL_RT))
		{
			if (subcursor < 37)
			{
				MakeNormalSound(SND_MENUCLICK);
				if ((subcursor % 6) == 5)
					subcursor -= 5;
				else
					subcursor++;
			}
		}
		if (ButtonTapped(CONTROL_B1))
		{
			if (subcursor == 37)
			{
				MakeNormalSound(SND_MENUSELECT);
				subMode = SUBMODE_NONE;
			}
			else // a rune
			{
				if(RuneLevel((Rune)subcursor)==RUNE_EMPTY || RuneLevel((Rune)subcursor)==RUNE_ASLEEP)
					MakeNormalSound(SND_UNAVAILABLE);
				else
				{
					if (RuneEquipped((Rune)subcursor))
					{
						player.runes[subcursor] &= (~RUNE_EQUIPPED);
						MakeNormalSound(SND_MENUSELECT);
					}
					else
					{
						if (RunesEquipped() >= 3)
							MakeNormalSound(SND_UNAVAILABLE);
						else
							player.runes[subcursor] |= RUNE_EQUIPPED;
					}
				}
			}
		}
		if (ButtonTapped(CONTROL_B2))
		{
			if (subcursor == 37)
			{
				MakeNormalSound(SND_MENUSELECT);
				subMode = SUBMODE_NONE;
			}
			else // a rune
			{
				if (RuneLevel((Rune)subcursor) == RUNE_EMPTY || RuneLevel((Rune)subcursor) == RUNE_RANK3)
					MakeNormalSound(SND_UNAVAILABLE);
				else
				{
					word price = 10;
					if (RuneLevel((Rune)subcursor) == RUNE_RANK1)
						price = 20;
					else if (RuneLevel((Rune)subcursor) == RUNE_RANK2)
						price = 30;
					if (player.runeStones >= price)
					{
						player.runes[subcursor]++;
						byte maxRunes = 0;
						for (int i = 0; i < (int)Rune::NUM_RUNES; i++)
							if (player.runes[i] == RUNE_RANK3)
								maxRunes++;
						if (maxRunes == (int)Rune::NUM_RUNES)
							EarnAchieve(Achievement::MAX_RUNES);

						player.runeStones -= price;
						MakeNormalSound(SND_FOOD);
					}
					else
						MakeNormalSound(SND_UNAVAILABLE);
				}
			}
		}
	}

	if(ButtonTapped(CONTROL_ESCAPE) || (ButtonTapped(CONTROL_B2) && subMode!=SUBMODE_RUNES))
	{
		MakeNormalSound(SND_MENUSELECT);
		if (subMode == SUBMODE_NONE)
		{
			LockOutControl(CONTROL_B2, true);
			return 0;
		}
		else
			subMode=SUBMODE_NONE;
		lastKey=0;
	}
	
	return 1;
}
