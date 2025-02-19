#include "pause.h"
#include "player.h"
#include "shop.h"
#include "options.h"
#include "challenge.h"
#include "appdata.h"
#include "spell.h"
#include "skills.h"

#define SUBMODE_NONE	 0
#define SUBMODE_SLOTPICK 1
#define SUBMODE_SKILLS	 2
#define SUBMODE_QOL		 3

byte cursor=0;
static byte subcursor=0,effCursor=0;
static char lastKey=0;
byte subMode;
static byte giveUp=0;	// which text should be shown for "Give Up"
int pauseX=-250,subX=-250;
byte saveLevel[5],saveNightmare[5];
byte saveChapter[5];
byte saveHour[5],saveMin[5];
Difficulty saveDiff[5];
static byte armaBrt=0;
static byte oldc = 255;

void ResetPauseMenu(void)
{
	pauseX=-250;
	subX=-250;
	cursor=0;
	oldc = 255;
}

void SetSubCursor(byte s)
{
	subcursor=s;
}

void RenderPauseMenu(void)
{
	char onoff[6][8] = { "Off","I","II","III","IV","V" };
	if(pauseX==-250)
		return;

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
		PrintBrightGlow(pauseX + 10, 240, "Cancel", -16 + (cursor == 0) * 16, 2);
		char s[32];
		sprintf(s, "Skills (%d)", player.skillPts);
		PrintBrightGlow(pauseX + 10, 240+26*1, s, -16 + (cursor == 1) * 16, 2);
		if (!Challenging())
			PrintBrightGlow(pauseX + 10, 240+26*2, "Load", -16 + (cursor == 2) * 16, 2);

		if (giveUp == 0)
			PrintBrightGlow(pauseX + 10, 240+26*3, "Save", -16 + (cursor == 3) * 16, 2);
		else if (giveUp == 2)	// random battle
			PrintBrightGlow(pauseX + 10, 240+26*3, "Run Away", -16 + (cursor == 3) * 16, 2);
		else 	// regular level
			PrintBrightGlow(pauseX + 10, 240+26*3, "Give Up", -16 + (cursor == 3) * 16, 2);

		PrintBrightGlow(pauseX + 10, 240+26*4, "Sound:", -16 + (cursor == 4) * 16, 2);
		PrintBrightGlow(pauseX + 10 + 90, 240+26*4, onoff[opt.soundVol], -16 + (cursor == 4) * 16, 2);

		PrintBrightGlow(pauseX + 10, 240+26*5, "Music:", -16 + (cursor == 5) * 16, 2);
		PrintBrightGlow(pauseX + 10 + 90, 240+26*5, onoff[opt.musicVol], -16 + (cursor == 5) * 16, 2);
		PrintBrightGlow(pauseX + 10, 240 + 26 * 6, "Weird Settings", -16 + (cursor == 6) * 16, 2);
		PrintBrightGlow(pauseX + 10, 240+26*7, "Quit Game", -16 + (cursor == 7) * 16, 2);
		RenderSlotPickMenu();
		if(subMode==SUBMODE_SKILLS)
			RenderSkillMenu();
		if (subMode == SUBMODE_QOL)
			RenderWeirdMenu();
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
				sprintf(s,"%02d:%02d  Lvl: %02d",saveHour[i],saveMin[i],saveLevel[i]);
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
				sprintf(s,"%02d:%02d  Lvl: %02d",saveHour[i],saveMin[i],saveLevel[i]);
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
	PrintBrightGlow(SCRWID - 80, SCRHEI - 30 - 75, s, 31, 1);

	if (giveUp == 0)	// only in the overworld
	{
		// reset skills
		if (subcursor == 36)
			RenderSkillBox(SCRWID - 80, SCRHEI - 30 - 60, SCRWID - 10, SCRHEI - 30 - 40, 32 * 5 + 31, 32 * 5 + 10);
		else
			RenderSkillBox(SCRWID - 80, SCRHEI - 30 - 60, SCRWID - 10, SCRHEI - 30 - 40, 32 * 5 + 16, 32 * 5 + 6);
		PrintBrightGlow(SCRWID - 80 + 4 + 6, SCRHEI - 30 - 60, "Reset", (subcursor == 36) * 31, 2);
	}

	// exit
	if (subcursor == 37)
		RenderSkillBox(SCRWID - 80, SCRHEI - 30 - 35, SCRWID - 10, SCRHEI - 30 - 15, 32 * 5 + 31, 32 * 5 + 10);
	else
		RenderSkillBox(SCRWID - 80, SCRHEI - 30 - 35, SCRWID - 10, SCRHEI - 30 - 15, 32 * 5 + 16, 32 * 5 + 6);
	PrintBrightGlow(SCRWID - 80 + 4+12, SCRHEI - 30 - 35, "Exit", (subcursor == 37) * 31, 2);
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
	RenderWeirdOption(x, y + 260, "Quick Cast:", player.enableQuickCast, (subcursor == 13), true);
	PrintBrightGlow(x, y + 280, "Exit", -16 + 32 * (subcursor==14), 2);

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
			PrintBrightGlow(x, SCRHEI - 50, "Cast spells by pressing their number!", 0, 1);
			break;
	}
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

float CalcTotalPercent(player_t *p)
{
	int i,amt,total;

	amt=0;
	total=0;
	for(i=0;i<1;i++)
	{
		total+=p->totalCompletion[i];
		amt+=p->complete[i];
	}
	return (float)amt/(float)total;
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
	byte c;
	static byte reptCounter=0;

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

	c=GetControls();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	byte maxCursor = 4 + (!ClassicMode()) * 3;
	byte loadSpot = 1 + (!ClassicMode());
	if(subMode==SUBMODE_NONE)	// not in any submenu
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			cursor--;
			if(cursor==255)
				cursor=maxCursor;
			if(Challenging() && cursor==loadSpot)
				cursor=loadSpot-1;

			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && (!reptCounter))
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
			// convert the cursor position into the classic equivalent if there is one, a new slot if not
			if (cursor == 1)
				effCursor = 5;
			else if (cursor == 4)
				effCursor = 6;
			else if (cursor == 5)
				effCursor = 3;
			else if (cursor == 6)
				effCursor = 7;
			else if (cursor == 7)
				effCursor = 4;
			else
				if (cursor > 0)
					effCursor--;
		}

		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			MakeNormalSound(SND_MENUSELECT);

			switch(effCursor)
			{
				case 0: // cancel
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
			}
		}
	}
	else if(subMode==SUBMODE_SLOTPICK)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor--;
			if(subcursor==255)
				subcursor=4;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor++;
			if(subcursor==5)
				subcursor=0;
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
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
		if ((c & CONTROL_UP) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor--;
			if (subcursor == 255)
				subcursor = 14;
		}
		if ((c & CONTROL_DN) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor++;
			if (subcursor > 14)
				subcursor = 0;
		}
		if ((c & CONTROL_B1) && (!(oldc & CONTROL_B1)))
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
					player.enableQuickCast = 1 - player.enableQuickCast;
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
		if ((c & CONTROL_UP) && (!reptCounter))
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
		if ((c & CONTROL_DN) && (!reptCounter))
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
		if ((c & CONTROL_LF) && (!reptCounter))
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
		if ((c & CONTROL_RT) && (!reptCounter))
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
		if ((c & CONTROL_B1) && (!(oldc & CONTROL_B1)))
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

	oldc = c;
	HandlePauseKeyPresses(mgl);
	UpdateGamepadStartAndSelect();

	if(((c&CONTROL_B2) && !(oldc&CONTROL_B2)) || lastKey==27 || GamepadSelectTapped() || GamepadStartTapped())	// hit ESC to exit pause menu
	{
		MakeNormalSound(SND_MENUSELECT);
		if(subMode==SUBMODE_NONE)
			return 0;
		else
			subMode=SUBMODE_NONE;
		lastKey=0;
	}
	
	return 1;
}
