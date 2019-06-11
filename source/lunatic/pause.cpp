#include "pause.h"
#include "player.h"
#include "options.h"
#include "appdata.h"

enum {
	SUBMODE_NONE = 0,
	SUBMODE_SLOTPICK
};

byte cursor = 0;
static byte subcursor = 0;
static char lastKey = 0;
byte subMode;
float percent[3]; // the percentages in each save slot
static byte giveUp = 0; // which text should be shown for "Give Up"

void SetSubCursor(byte s)
{
	subcursor = s;
}

void RenderPauseMenu(void)
{
	char txt[32] = "Music: ";

	DrawBox(208, 44, 432, 422, 128 + 10);
	DrawBox(209, 45, 431, 421, 128 + 16);
	DrawFillBox(210, 46, 430, 420, 0);
	DrawBox(211, 47, 429, 419, 128 + 10);

	Print(320 - GetStrLength("Cancel") / 2, 50, "Cancel", (cursor == 0)*16, 0);
	Print(320 - GetStrLength("Load Game") / 2, 110, "Load Game", (cursor == 1)*16, 0);
	Print(320 - GetStrLength("Save Game") / 2, 170, "Save Game", (cursor == 2)*16, 0);

	if (PlayerGetMusicSettings() == MUSIC_OFF)
		strcat(txt, "Off");
	else if (PlayerGetMusicSettings() == MUSIC_ON)
		strcat(txt, "On");
	else
		strcat(txt, "Rnd");
	Print(320 - GetStrLength(txt) / 2, 230, txt, (cursor == 3)*16, 0);

	// giveUp==2 means don't draw the give up option at all (for world picker pause)
	if (giveUp == 1)
		Print(320 - GetStrLength("Give Up") / 2, 290, "Give Up", (cursor == 4)*16, 0);
	else if (giveUp == 0)
		Print(320 - GetStrLength("World Select") / 2, 290, "World Select", (cursor == 4)*16, 0);
	Print(320 - GetStrLength("Quit Game") / 2, 350, "Quit Game", (cursor == 5)*16, 0);
	if (subMode == SUBMODE_SLOTPICK)
		RenderSlotPickMenu();
}

void RenderSlotPickMenu(void)
{
	char txt[20];

	DrawBox(258, 104, 492, 294, 128 + 10);
	DrawBox(259, 105, 491, 293, 128 + 16);
	DrawFillBox(260, 106, 490, 292, 0);
	DrawBox(261, 107, 489, 291, 128 + 10);

	if (percent[0] > 99.9)
		sprintf(txt, "Slot 1 - 100%%");
	else
		sprintf(txt, "Slot 1 - %03.1f%%", percent[0]);
	Print(375 - GetStrLength(txt) / 2, 110, txt, (subcursor == 0)*16, 0);
	if (percent[1] > 99.9)
		sprintf(txt, "Slot 2 - 100%%");
	else
		sprintf(txt, "Slot 2 - %03.1f%%", percent[1]);
	Print(375 - GetStrLength(txt) / 2, 170, txt, (subcursor == 1)*16, 0);
	if (percent[2] > 99.9)
		sprintf(txt, "Slot 3 - 100%%");
	else
		sprintf(txt, "Slot 3 - %03.1f%%", percent[2]);
	Print(375 - GetStrLength(txt) / 2, 230, txt, (subcursor == 2)*16, 0);

}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k = mgl->LastKeyPressed();
	if (k)
		lastKey = k;
}

float CalcTotalPercent(player_t *p)
{
	int i, amt, total;

	amt = 0;
	total = 0;
	for (i = 0; i < 5; i++)
	{
		total += p->totalCompletion[i];
		amt += p->complete[i];
	}
	return (float) amt / (float) total;
}

void SetGiveUpText(byte gu)
{
	giveUp = gu;
}

void InitPauseMenu(void)
{
	FILE *f;
	player_t p;

	lastKey = 0;
	subMode = 0;

	f = AppdataOpen("loony.sav", "rb");
	if (!f)
	{
		percent[0] = 0.0;
		percent[1] = 0.0;
		percent[2] = 0.0;
	}
	else
	{
		fread(&p, sizeof (player_t), 1, f);
		percent[0] = CalcTotalPercent(&p)*100;
		fread(&p, sizeof (player_t), 1, f);
		percent[1] = CalcTotalPercent(&p)*100;
		fread(&p, sizeof (player_t), 1, f);
		percent[2] = CalcTotalPercent(&p)*100;
		fclose(f);
	}
	MakeNormalSound(SND_PAUSE);
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	static byte oldc = 0;
	static byte reptCounter = 0;

	if (giveUp == 2 && cursor == 4) // not allowed in world picker pause
		cursor = 0;

	c = GetControls() | GetArrows();

	reptCounter++;
	if ((!oldc) || (reptCounter > 10))
		reptCounter = 0;

	if (subMode == SUBMODE_NONE) // not in any submenu
	{
		if ((c & CONTROL_UP) && (!reptCounter))
		{
			cursor--;
			if (cursor == 255)
				cursor = 5;

			if (giveUp == 2 && cursor == 4) // world picker pause has no option 4
				cursor = 3;

			MakeNormalSound(SND_MENUCLICK);
		}
		if ((c & CONTROL_DN) && (!reptCounter))
		{
			cursor++;
			if (cursor == 6)
				cursor = 0;
			if (giveUp == 2 && cursor == 4) // world picker pause has no option 4
				cursor = 5;

			MakeNormalSound(SND_MENUCLICK);
		}
		if (((c & CONTROL_B1) && (!(oldc & CONTROL_B1))) ||
				((c & CONTROL_B2) && (!(oldc & CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			switch (cursor) {
				case 0: // cancel
					return 0;
				case 1: // Load
					subMode = SUBMODE_SLOTPICK;
					break;
				case 2: // Save
					subMode = SUBMODE_SLOTPICK;
					break;
				case 3: // music
					CDStop();
					PlayerSetMusicSettings((PlayerGetMusicSettings() + 1) % 3);
					if (PlayerGetMusicSettings() == MUSIC_ON)
						CDPlay(GetCurSong());
					else
						ChooseNextSong();
					opt.music = PlayerGetMusicSettings();
					break;
				case 4: // give up
					return 2;
				case 5: // quit game
					return 3;
			}
		}
	}
	else if (subMode == SUBMODE_SLOTPICK)
	{
		if ((c & CONTROL_UP) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor--;
			if (subcursor == 255)
				subcursor = 2;
		}
		if ((c & CONTROL_DN) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor++;
			if (subcursor == 3)
				subcursor = 0;
		}
		if (((c & CONTROL_B1) && (!(oldc & CONTROL_B1))) ||
				((c & CONTROL_B2) && (!(oldc & CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			if (cursor == 1) // Load
			{
				SendMessageToGame(MSG_LOADGAME, 0);
				PlayerLoadGame(subcursor);
				MakeNormalSound(SND_LOADGAME);
				return 0;
			}
			else if (cursor == 2) // Save
			{
				PlayerSaveGame(subcursor);
				MakeNormalSound(SND_SAVEGAME);
				return 0;
			}
			subMode = SUBMODE_NONE;
		}
	}
	oldc = c;

	HandlePauseKeyPresses(mgl);
	if (lastKey == 27) // hit ESC to exit pause menu
	{
		MakeNormalSound(SND_MENUSELECT);
		if (subMode == SUBMODE_NONE)
			return 0;
		else
			subMode = SUBMODE_NONE;
		lastKey = 0;
	}
	return 1;
}
