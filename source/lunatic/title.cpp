#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "lsdir.h"
#include "palettes.h"
#include "appdata.h"

#define VERSION "Version 3.0"
#define COPYRIGHT "Copyright 1998-2011, Hamumu Software"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][48] = {
	"SPISPOPD II",
	"@DR. LUNATIC",
	"",
	"",
	"Copyright 1998-2011, Hamumu Software",
	"#",
	"Original Concept",
	"Mike Hommel",
	"%",
	"Programming",
	"Mike Hommel",
	"%",
	"Modernizing",
	"Tad \"SpaceManiac\" Hardesty",
	"%",
	"Character Design",
	"Mike Hommel",
	"%",
	"Level Design",
	"Mike Hommel",
	"%",
	"3D Graphics",
	"Mike Hommel",
	"%",
	"2D Graphics",
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Brent Christian",
	"Mike Hommel",
	"(Surprise!)",
	"%",
	"Music",
	"Brent Christian",
	"%",
	"Producer/Designer/Director",
	"Mike Hommel",
	"%",
	"Gaffer",
	"Mike Hommel",
	"%",
	"QA Director",
	"Angela Finer",
	"%",
	"Testing",
	"Baba",
	"Brent Christian",
	"Jim Crawford",
	"Chris Dillman",
	"Angela Finer",
	"Tim Finer",
	"Dawn Genge",
	"Mattie Goodman",
	"Matt Guest",
	"Suzanne Hommel",
	"Solange Hunt",
	"Brad Kasten",
	"Geoff Michell",
	"Britt Morris",
	"Trevor Strohman",
	"Peter Young",
	"%",
	"Ideas & Hamumu Theme",
	"Mattie Goodman",
	"%",
	"Technical Assistance",
	"Trevor Strohman",
	"%",
	"Special Thanks",
	"Junebug Superspy &",
	"Too Much Hot Sauce",
	"All the SpisFans",
	"(both of them that is)",
	"Ketmany Bouapha (NO, Bouapha!)",
	"Rinley \"Dirty\" Deeds",
	"%",
	"Henry G., wherefore art thou?",
	"%",
	"\"it's certainly pointless",
	"and annoying, but i say you",
	"should keep it anyways.\"",
	"The words of a true SpisFan",
	"%",
	"#",
	"Stop by www.hamumu.com!",
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	"@THE END",
	"$"
};

char victoryTxt[][64] = {
	"@With Dr. Lunatic vanquished, the",
	"",
	"",
	"@zombie menace was ended. Never again",
	"",
	"",
	"@would someone take the brains out of",
	"",
	"",
	"@zombies and put them into other",
	"",
	"",
	"@zombies to create a race of super",
	"",
	"",
	"@zombies.",
	"",
	"",
	"#",
	"",
	"@Bouapha was the hero of the hour,",
	"",
	"",
	"@loved and respected by all. There",
	"",
	"",
	"@were parades and parties for days.",
	"",
	"",
	"@Until the president got himself into",
	"",
	"",
	"@another madcap scandal, and everyone",
	"",
	"",
	"@forgot about Bouapha and the zombies",
	"",
	"",
	"@altogether.",
	"",
	"",
	"#",
	"$"
};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
const int END_OF_CREDITS = 480 * 4 + 180;
const int END_OF_VICTORY = 480 * 2;

struct title_t
{
	int bouaphaX, doctorX;
	int blueY;
	char titleBright;
	char titleDir;
	int expando;
	char dexpando;
	byte cursor;
	byte savecursor;
	float percent[3];
};

sprite_set_t *planetSpr;
static int numRunsToMakeUp;
byte pickerpos;
char pickeroffset;
byte offsetdir;
byte curCustom;

static byte oldc = 0;
mfont_t pickerFont;

static byte keyAnim = 0;
char lvlName[32];

byte starColorTable[] = {214, 81, 63, 49, 33, 21, 32, 83, 93};

byte demoTextCounter;

byte HandleTitleKeys(MGLDraw *mgl)
{
	char k;

	k = mgl->LastKeyPressed();

	if (k == 'e')
		return 1; // go to the editor
	if (k == 27)
		return 2; // exit
	else
		return 0; // play the game
}

byte LunaticTitle(MGLDraw *mgl)
{
	mgl->LoadBMP("graphics/title.bmp");
	mgl->Flip();
	while (!mgl->LastKeyPeek())
	{
		if (!mgl->Process())
			return 2;
		mgl->Flip();
	}
	return HandleTitleKeys(mgl);
}

byte HandleWorldPickerKeys(MGLDraw *mgl)
{
	char k;

	k = mgl->LastKeyPressed();
	if (k == 27)
	{
		return 253; // pause menu
	}
	else
		return 254;
}

int NextLegal(int now, byte dir)
{
	byte tries;

	tries = 0;
	if (dir == 1)
	{
		do
		{
			now++;
			if (now > MAX_CUSTOM - 1)
				now = 5;
			tries++;
		}
		while (player.customName[now][0] == '\0' && tries < MAX_CUSTOM - 5);
	}
	else
	{
		do
		{
			now--;
			if (now < 5)
				now = MAX_CUSTOM - 1;
			tries++;
		}
		while (player.customName[now][0] == '\0' && tries < MAX_CUSTOM - 5);
	}
	return now;
}

byte PickerRun(int *lastTime, MGLDraw *mgl)
{
	byte c;
	int i;
	static byte ticktock = 0;
	static byte flipper = 0;
	byte movedCursor;

	movedCursor = 0;
	numRunsToMakeUp = 0;
	while (*lastTime > 0)
	{
		if (!mgl->Process())
		{
			return 255;
		}
		keyAnim++;
		if (keyAnim > 63)
			keyAnim = 0;

		if ((c = HandleWorldPickerKeys(mgl)) != 254)
			return c;

		c = GetControls() | GetArrows();

		if (ticktock)
			ticktock--;

		if (demoTextCounter)
			demoTextCounter--;

		if (!(oldc & (CONTROL_UP | CONTROL_DN)))
			ticktock = 0;

		if (pickeroffset == 0) // only listens to keys when it is on a spot
		{
			if (pickerpos < 4)
			{
				if (c & CONTROL_UP)
				{
					pickerpos--;
					if (pickerpos == 255)
						pickerpos = 3;
					pickeroffset = 3;
					offsetdir = 0;
					MakeNormalSound(SND_WORLDTURN);
					movedCursor = 1;
				}
				if (c & CONTROL_DN)
				{
					pickerpos++;
					if (pickerpos == 4)
						pickerpos = 0;
					pickeroffset = -3;
					offsetdir = 0;
					MakeNormalSound(SND_WORLDTURN);
					movedCursor = 1;
				}
				if (c & CONTROL_LF)
				{
					pickerpos = 5;
					pickeroffset = -3;
					offsetdir = 0;
					MakeNormalSound(SND_WORLDTURN);
					movedCursor = 1;
					curCustom = NextLegal(curCustom - 1, 1);
				}
				if (c & CONTROL_RT)
				{
					pickerpos = 4;
					pickeroffset = -3;
					offsetdir = 0;
					MakeNormalSound(SND_WORLDTURN);
					movedCursor = 1;
				}
			}
			else
			{
				if (pickerpos == 5 && (c & CONTROL_RT))
				{
					pickerpos = 0;
					pickeroffset = 3;
					offsetdir = 1;
					MakeNormalSound(SND_WORLDTURN);
					movedCursor = 1;
				}
				if (pickerpos == 4 && (c & CONTROL_LF))
				{
					pickerpos = 0;
					pickeroffset = 3;
					offsetdir = 2;
					MakeNormalSound(SND_WORLDTURN);
					movedCursor = 1;
				}
				if (pickerpos == 5 && (c & CONTROL_UP) && (!ticktock))
				{
					i = curCustom;
					curCustom = NextLegal(curCustom, 0);
					if (curCustom != i)
						MakeNormalSound(SND_FOOD);
					ticktock = 20;
					movedCursor = 1;
				}
				if (pickerpos == 5 && (c & CONTROL_DN) && (!ticktock))
				{
					i = curCustom;
					curCustom = NextLegal(curCustom, 1);
					if (curCustom != i)
						MakeNormalSound(SND_FOOD);
					ticktock = 20;
					movedCursor = 1;
				}
			}
		}
		else
		{
			flipper++;
			if (flipper == 2)
			{
				if (pickeroffset > 0)
					pickeroffset--;
				else
					pickeroffset++;
				flipper = 0;
			}
			if (pickeroffset == 0)
				offsetdir = 0;
		}
		if (((c & CONTROL_B1) && (!(oldc & CONTROL_B1))) ||
				((c & CONTROL_B2) && (!(oldc & CONTROL_B2))))
		{
			MakeNormalSound(SND_WORLDPICK);
			if (pickerpos < 5)
				return pickerpos;
			else
			{
				if (player.customName[curCustom][0] != '\0')
					return curCustom;
				else // can't pick a nonexistent level
					MakeNormalSound(SND_BOUAPHAOUCH);
			}
		}
		oldc = c;

		*lastTime -= TIME_PER_FRAME;
		numRunsToMakeUp++;
	}
	JamulSoundUpdate();

	if (movedCursor == 1)
	{
		demoTextCounter = 0;

		if (pickerpos < 5)
			GetWorldPoints(player.customName[pickerpos]);
		else
		{
			GetWorldName(player.customName[curCustom], lvlName);
			GetWorldPoints(player.customName[curCustom]);
		}
	}

	return 254;
}

void PickerDraw(MGLDraw *mgl)
{
	int frm = 0; // warning prevention
	int i;
	byte *scrn;
	float f;

	char txt[32];

	mgl->ClearScreen();

	MGL_srand(123);
	scrn = mgl->GetScreen();

	for (i = 0; i < 221; i++)
		scrn[MGL_random(640) + MGL_random(480)*640] = starColorTable[MGL_random(9)];

	if (offsetdir == 0)
	{
		if (pickerpos < 4)
		{
			frm = pickerpos * 4;
			frm += pickeroffset;
			if (frm < 0)
				frm += 16;
		}
		else
		{
			if (pickerpos == 5)
				frm = 19 + pickeroffset;
			if (pickerpos == 4)
				frm = 23 + pickeroffset;
		}
	}
	else if (offsetdir == 1)
	{
		frm = 16 + pickeroffset;
	}
	else if (offsetdir == 2)
	{
		frm = 19 + pickeroffset;
	}

	planetSpr->GetSprite(frm)->Draw(320, 240, mgl);

	switch (pickerpos) {
		case 0:
		case 1:
		case 2:
		case 3:
			// regular places
			planetSpr->GetSprite(44)->Draw(14, 34, mgl);
			FontPrintString(25, 2, "Previous World", &pickerFont);
			planetSpr->GetSprite(42)->Draw(14, 54, mgl);
			FontPrintString(25, 22, "Next World", &pickerFont);
			planetSpr->GetSprite(43)->Draw(14, 74, mgl);
			FontPrintString(25, 42, "Custom Worlds", &pickerFont);
			planetSpr->GetSprite(41)->Draw(14, 94, mgl);
			FontPrintString(25, 62, "Asylum", &pickerFont);
			break;
		case 4:
			// Asylum
			planetSpr->GetSprite(43)->Draw(14, 34, mgl);
			FontPrintString(25, 2, "Normal Worlds", &pickerFont);
			break;
		case 5:
			// Custom
			planetSpr->GetSprite(41)->Draw(14, 34, mgl);
			FontPrintString(25, 2, "Normal Worlds", &pickerFont);
			planetSpr->GetSprite(44)->Draw(14, 54, mgl);
			FontPrintString(25, 22, "Previous Custom", &pickerFont);
			planetSpr->GetSprite(42)->Draw(14, 74, mgl);
			FontPrintString(25, 42, "Next Custom", &pickerFont);
			break;
	}

	switch (pickerpos) {
		case 0:
			FontPrintString(2, 460, "Cavernous Caves", &pickerFont);
			break;
		case 1:
			FontPrintString(2, 460, "Icy Mountain", &pickerFont);
			break;
		case 2:
			FontPrintString(2, 460, "Spooky Forest", &pickerFont);
			break;
		case 3:
			FontPrintString(2, 460, "Dusty Desert", &pickerFont);
			break;
		case 4:
			FontPrintString(2, 460, "Crazy Asylum Of Madness", &pickerFont);
			break;
		case 5:
			FontPrintString(2, 440, "Custom World", &pickerFont);
			break;
	}

	if (pickerpos < 5)
	{
		f = PlayerGetPercent(pickerpos)*100;
		if (PlayerHasLunacyKey(pickerpos))
		{
			planetSpr->GetSprite(24 + (keyAnim / 4))->Draw(570, 400, mgl);
		}
	}
	else
	{
		f = PlayerGetPercent(curCustom)*100;
		if (PlayerHasLunacyKey(curCustom))
		{
			planetSpr->GetSprite(24 + (keyAnim / 4))->Draw(570, 400, mgl);
		}
	}
	if (f > 99.9)
		sprintf(txt, "Complete: 100%%");
	else
		sprintf(txt, "Complete: %02.1f%%", f);
	FontPrintString(388, 460, txt, &pickerFont);

	if (pickerpos == 5) // customs show which custom world is selected
	{
		if (player.customName[curCustom][0] == '\0')
			FontPrintString(2, 460, "None Available", &pickerFont);
		else
			FontPrintString(2, 460, lvlName, &pickerFont);
	}
	f = PlayerGetGamePercent()*100;
	if (f > 99.9)
		sprintf(txt, "Total: 100%%");
	else
		sprintf(txt, "Total: %03.1f%%", f);
	FontPrintString(446, 2, txt, &pickerFont);
	mgl->Flip();
}

byte WorldPicker(MGLDraw *mgl)
{
	byte exitcode = 254;
	int lastTime = 1;
	int i;

	demoTextCounter = 0;

	for (i = 0; i < 5; i++)
	{
		player.totalCompletion[i] = GetWorldPoints(player.customName[i]);
	}

	if (PlayerGetMusicSettings() == MUSIC_ON)
		CDPlay(3); // world picker theme

	mgl->ClearScreen();
	mgl->Flip();
	planetSpr = new sprite_set_t("graphics/planet.jsp");
	mgl->LoadBMP("graphics/picker.bmp");
	FontLoad("graphics/gillsans4.jft", &pickerFont);

	pickerpos = 0;
	pickeroffset = 0;
	offsetdir = 0;
	curCustom = 5;
	oldc = GetControls() | GetArrows();

	numRunsToMakeUp = 0;
	while (exitcode == 254)
	{
		lastTime += TimeLength();
		StartClock();
		exitcode = PickerRun(&lastTime, mgl);
		if (numRunsToMakeUp > 0)
			PickerDraw(mgl);

		if (!mgl->Process())
		{
			exitcode = 255;
		}
		EndClock();
	}
	if (pickerpos == 5) // custom world
		player.worldNum = curCustom;

	mgl->ClearScreen();
	mgl->Flip();
	mgl->LoadBMP("graphics/title.bmp");
	GammaCorrect(mgl, GetGamma());
	delete planetSpr;
	FontFree(&pickerFont);
	SeedRNG();
	return exitcode;
}

void ScanWorldNames(void)
{
	int i;
	int numFiles = 0;

	for (i = 5; i < MAX_CUSTOM; i++)
		player.customName[i][0] = '\0';

	for (const char* name : filterdir("worlds", ".dlw", 32))
	{
		// rule out the regular game worlds, so they don't show up as custom worlds
		if ((strcmp(name, "forest.dlw")) &&
				(strcmp(name, "desert.dlw")) &&
				(strcmp(name, "icymount.dlw")) &&
				(strcmp(name, "caverns.dlw")) &&
				(strcmp(name, "asylum.dlw")) &&
				(strcmp(name, "backup_load.dlw")) &&
				(strcmp(name, "backup_exit.dlw")))
		{
			strncpy(player.customName[numFiles + 5], name, 32);
			numFiles++;
			if (numFiles + 5 >= MAX_CUSTOM)
				break;
		}
	}
}

void ReScanWorldNames(void)
{
	int i;
	byte okay[MAX_CUSTOM];

	for (i = 5; i < MAX_CUSTOM; i++)
	{
		if (player.customName[i][0] == '\0')
			okay[i] = 1;
		else
			okay[i] = 0;
	}

	for (const char* name : filterdir("worlds", ".dlw", 32))
	{
		// rule out the regular game worlds, so they don't show up as custom worlds
		if ((strcmp(name, "forest.dlw")) &&
				(strcmp(name, "desert.dlw")) &&
				(strcmp(name, "icymount.dlw")) &&
				(strcmp(name, "caverns.dlw")) &&
				(strcmp(name, "asylum.dlw")) &&
				(strcmp(name, "backup_load.dlw")) &&
				(strcmp(name, "backup_exit.dlw")))
		{
			for (i = 5; i < MAX_CUSTOM; i++)
			{
				if (!strcmp(name, player.customName[i]))
				{
					okay[i] = 1;
					break;
				}
			}
			if (i == MAX_CUSTOM) // none of the files matched, this is a new one
			{
				// add it in, if there's room
				for (i = 5; i < MAX_CUSTOM; i++)
				{
					if (player.customName[i][0] == '\0')
					{
						strncpy(player.customName[i], name, 32);
						break;
					}
				}
			}
		}
	}

	// remove any that aren't valid
	for (i = 5; i < MAX_CUSTOM; i++)
	{
		if (okay[i] == 0)
			player.customName[i][0] = '\0';
	}
}

void MainMenuDisplay(MGLDraw *mgl, title_t title)
{
	int i, color, deltaColor;
	byte *scrn;

	color = 0;
	deltaColor = (12 * 65536) / (480 - title.blueY);
	scrn = mgl->GetScreen();
	if (title.blueY > 0)
		memset(scrn, 0, 640 * title.blueY);
	scrn += 640 * title.blueY;
	for (i = title.blueY; i < 480; i++)
	{
		memset(scrn, color / 65536 + 96, 640);
		scrn += 640;
		color += deltaColor;
	}

	// draw Dr. L & Bouapha
	planetSpr->GetSprite(0)->Draw(640 - title.doctorX, 480, mgl);
	planetSpr->GetSprite(1)->Draw(title.bouaphaX, 480, mgl);

	// draw the title parts
	// SPISPOPD II:
	planetSpr->GetSprite(2)->DrawBright(240,30,mgl,title.titleBright);
	// DR. LUNATIC:
	planetSpr->GetSprite(3)->DrawBright(290,125,mgl,title.titleBright);
	// version #:
	Print(536,146,VERSION,1,1);
	Print(535,145,VERSION,0,1);
	// Copyright:
	Print(3,467,COPYRIGHT,1,1);
	Print(2,466,COPYRIGHT,0,1);
#ifdef DEMO
	// DEMO!! message
	planetSpr->GetSprite(23)->DrawBright(280,190+title.titleBright*5-79*2+title.expando*2,mgl,title.titleBright);
#elif defined(EXPANDO)
	// expandopak
	planetSpr->GetSprite(4+title.expando/16)->DrawBright(270,180,mgl,title.titleBright);
	planetSpr->GetSprite(24)->DrawBright(190,185,mgl,title.titleBright);
#endif

	// now the menu options
	planetSpr->GetSprite(9 + (title.cursor == 0))->Draw(240, 270, mgl);
	planetSpr->GetSprite(11 + (title.cursor == 1))->Draw(260, 300, mgl);
	planetSpr->GetSprite(13 + (title.cursor == 2))->Draw(280, 330, mgl);
	planetSpr->GetSprite(15 + (title.cursor == 3))->Draw(300, 360, mgl);
	planetSpr->GetSprite(19 + (title.cursor == 5))->Draw(340, 420, mgl);
	planetSpr->GetSprite(21 + (title.cursor == 6))->Draw(360, 450, mgl);
}

byte MainMenuUpdate(MGLDraw *mgl, title_t *title)
{
	byte c;
	static byte reptCounter = 0;

	// update graphics
	title->titleBright += title->titleDir;
	if (title->titleBright > 31)
	{
		title->titleDir = -1;
		title->titleBright = 31;
	}
	if (title->titleDir < 0 && title->titleBright == 0)
		title->titleDir = 0;

	if (title->bouaphaX < 0)
		title->bouaphaX += 8;
	if (title->doctorX < 0)
		title->doctorX += 8;

	if (title->blueY > 0)
		title->blueY -= 8;
	if (title->blueY < 0)
		title->blueY = 0;

	title->expando += title->dexpando;
	if (title->expando > 79)
	{
		title->dexpando = -title->dexpando * 13 / 16;
		title->expando = 79;
	}
	else
		title->dexpando++;

	// now real updating
	c = GetControls() | GetArrows();

	reptCounter++;
	if ((!oldc) || (reptCounter > 10))
		reptCounter = 0;

	if ((c & CONTROL_UP) && (!reptCounter))
	{
		(title->cursor)--;
		if (title->cursor == 255)
			title->cursor = 6;
#ifndef DEMO
		if (title->cursor == 4) // ordering is not a viable option in the non-shareware
			title->cursor = 3;
#endif
		MakeNormalSound(SND_MENUCLICK);
	}
	if ((c & CONTROL_DN) && (!reptCounter))
	{
		(title->cursor)++;
		if (title->cursor == 7)
			title->cursor = 0;
#ifndef DEMO
		if (title->cursor == 4) // ordering is not a viable option in the non-shareware
			title->cursor = 5;
#endif
		MakeNormalSound(SND_MENUCLICK);
	}
	if (((c & CONTROL_B1) && (!(oldc & CONTROL_B1))) ||
			((c & CONTROL_B2) && (!(oldc & CONTROL_B2))))
	{
		MakeNormalSound(SND_MENUSELECT);
		return 1;
	}
	oldc = c;

	c = mgl->LastKeyPressed();
	if (c == 27)
	{
		MakeNormalSound(SND_MENUSELECT);
		return 2;
	}

	return 0;
}

byte MainMenu(MGLDraw *mgl)
{
	dword startTime, now;
	dword runStart, runEnd;

	byte b = 0;
	title_t title;

	if (opt.music == MUSIC_ON)
		CDPlay(2); // the title theme

	mgl->LoadBMP("graphics/title.bmp");
	mgl->LastKeyPressed();
	mgl->ClearScreen();
	oldc = CONTROL_B1 | CONTROL_B2;
	planetSpr = new sprite_set_t("graphics/titlespr.jsp");

	title.bouaphaX = -320;
	title.doctorX = -320;
	title.titleBright = -32;
	title.titleDir = 4;
	title.cursor = 0;
	title.blueY = 479;
	title.expando = 0;
	title.dexpando = 0;
	startTime = timeGetTime();
	while (b == 0)
	{
		runStart = timeGetTime();
		b = MainMenuUpdate(mgl, &title);
		MainMenuDisplay(mgl, title);
		mgl->Flip();
		runEnd = timeGetTime();

		if (runEnd - runStart < (1000 / 50))
			SDL_Delay((1000 / 50)-(runEnd - runStart));

		if (!mgl->Process())
		{
			CDStop();
			delete planetSpr;
			return 255;
		}
		if (b == 1 && title.cursor == 1) // selected Load Game
		{
			if (!GameSlotPicker(mgl, &title)) // pressed ESC on the slot picker
			{
				b = 0;
			}
			startTime = timeGetTime();
		}
		if (b == 1 && title.cursor == 2) // options
		{
			OptionsMenu(mgl);
			startTime = timeGetTime();
		}
		if (b == 1 && title.cursor == 5) // help
		{
			HelpScreens(mgl);
			startTime = timeGetTime();
		}
		now = timeGetTime();
		if (now - startTime > 1000 * 20)
		{
			Credits(mgl);
			startTime = timeGetTime();
		}
	}
	delete planetSpr;
	if (b == 1) // something was selected
	{
		if (title.cursor == 6) // exit
			return 255;
		else
			return title.cursor;
	}
	else
		return 255; // ESC was pressed
}

void GameSlotPickerDisplay(MGLDraw *mgl, title_t title)
{
	int i,color,deltaColor;
	byte *scrn;
	char txt[18];

	color=0;
	deltaColor=(12*65536)/(480-title.blueY);
	scrn=mgl->GetScreen();
	if(title.blueY>0)
		memset(scrn,0,640*title.blueY);
	scrn+=640*title.blueY;
	for(i=title.blueY;i<480;i++)
	{
		memset(scrn,color/65536+96,640);
		scrn+=640;
		color+=deltaColor;
	}
	// draw Dr. L & Bouapha
	planetSpr->GetSprite(0)->Draw(640-title.doctorX,480,mgl);
	planetSpr->GetSprite(1)->Draw(title.bouaphaX,480,mgl);

	// draw the title parts
	// SPISPOPD II:
	planetSpr->GetSprite(2)->DrawBright(240,30,mgl,title.titleBright);
	// DR. LUNATIC:
	planetSpr->GetSprite(3)->DrawBright(290,125,mgl,title.titleBright);
	// version #:
	Print(536,146,VERSION,1,1);
	Print(535,145,VERSION,0,1);
	// Copyright:
	Print(3,467,COPYRIGHT,1,1);
	Print(2,466,COPYRIGHT,0,1);
#ifdef DEMO
	// DEMO!! message
	planetSpr->GetSprite(23)->DrawBright(280,190+title.titleBright*5-79*2+title.expando*2,mgl,title.titleBright);
#elif defined(EXPANDO)
	// expandopak
	planetSpr->GetSprite(4+title.expando/16)->DrawBright(270,180,mgl,title.titleBright);
	planetSpr->GetSprite(24)->DrawBright(190,185,mgl,title.titleBright);
#endif

	// now the game slots
	for (i = 0; i < 3; i++)
	{
		if (title.percent[i] > 99.9)
			sprintf(txt, "Slot %d - 100%%", i + 1);
		else
			sprintf(txt, "Slot %d - %03.1f%%", i + 1, title.percent[i]);

		Print(180 + 30 * i, 220 + 70 * i, txt, -6 + 12 * (title.savecursor == i), 0);
	}
}

byte GameSlotPickerUpdate(MGLDraw *mgl, title_t *title)
{
	byte c;
	static byte reptCounter = 0;

	// update graphics
	title->titleBright += title->titleDir;
	if (title->titleBright > 31)
	{
		title->titleDir = -1;
		title->titleBright = 31;
	}
	if (title->titleDir < 0 && title->titleBright == 0)
		title->titleDir = 0;

	if (title->bouaphaX<-60)
		title->bouaphaX += 8;
	if (title->bouaphaX>-60)
		title->bouaphaX -= 8;
	if (title->doctorX<-40)
		title->doctorX += 8;
	if (title->doctorX>-40)
		title->doctorX -= 8;
	if (title->blueY > 0)
		title->blueY -= 8;
	if (title->blueY < 0)
		title->blueY = 0;

	title->expando += title->dexpando;
	if (title->expando > 79)
	{
		title->dexpando = -title->dexpando * 13 / 16;
		title->expando = 79;
	}
	else
		title->dexpando++;

	// now real updating
	c = GetControls() | GetArrows();

	reptCounter++;
	if ((!oldc) || (reptCounter > 10))
		reptCounter = 0;

	if ((c & CONTROL_UP) && (!reptCounter))
	{
		(title->savecursor)--;
		if (title->savecursor == 255)
			title->savecursor = 2;
		MakeNormalSound(SND_MENUCLICK);
	}
	if ((c & CONTROL_DN) && (!reptCounter))
	{
		(title->savecursor)++;
		if (title->savecursor == 3)
			title->savecursor = 0;
		MakeNormalSound(SND_MENUCLICK);
	}
	if (((c & CONTROL_B1) && (!(oldc & CONTROL_B1))) ||
			((c & CONTROL_B2) && (!(oldc & CONTROL_B2))))
	{
		MakeNormalSound(SND_MENUSELECT);
		return 1;
	}
	oldc = c;

	if (mgl->LastKeyPressed() == 27)
	{
		MakeNormalSound(SND_MENUSELECT);
		return 2;
	}

	return 0;
}

void InitGameSlotPicker(MGLDraw *mgl, title_t *title)
{
	FILE *f;
	player_t p;

	f = AppdataOpen("loony.sav", "rb");
	if (!f)
	{
		title->percent[0] = 0.0;
		title->percent[1] = 0.0;
		title->percent[2] = 0.0;
	}
	else
	{
		fread(&p, sizeof (player_t), 1, f);
		title->percent[0] = CalcTotalPercent(&p)*100;
		fread(&p, sizeof (player_t), 1, f);
		title->percent[1] = CalcTotalPercent(&p)*100;
		fread(&p, sizeof (player_t), 1, f);
		title->percent[2] = CalcTotalPercent(&p)*100;
		fclose(f);
	}
	mgl->LastKeyPressed();
	oldc = CONTROL_B1 | CONTROL_B2;
}

byte GameSlotPicker(MGLDraw *mgl, title_t *title)
{
	byte b = 0;
	dword runEnd, runStart;

	title->savecursor = 0;
	InitGameSlotPicker(mgl, title);

	while (b == 0)
	{
		runStart = timeGetTime();

		b = GameSlotPickerUpdate(mgl, title);
		GameSlotPickerDisplay(mgl, *title);
		mgl->Flip();
		runEnd = timeGetTime();

		if (runEnd - runStart < (1000 / 50))
			SDL_Delay((1000 / 50)-(runEnd - runStart));

		if (!mgl->Process())
			return 0;
	}
	if (b == 1) // something was selected
	{
		InitPlayer(INIT_GAME, 0, 0);
		PlayerLoadGame(title->savecursor);
		// make it remember which was picked so the pause menu will start on the same
		SetSubCursor(title->savecursor);
		return 1;
	}
	else
		return 0;
}

void CreditsRender(int y)
{
	int i, ypos;
	char *s;

	i = 0;

	ypos = 0;
	while (credits[i][0] != '$')
	{
		s = credits[i];
		if (ypos - y>-60)
		{
			if (s[0] == '@')
			{
				CenterPrint(320, ypos - y, &s[1], 0, 0);
			}
			else if (s[0] == '#')
			{
				DrawFillBox(320 - 200, ypos - y + 8, 320 + 200, ypos - y + 11, 255);
			}
			else if (s[0] == '%')
			{
				DrawFillBox(320 - 70, ypos - y + 8, 320 + 70, ypos - y + 9, 255);
			}
			else
				CenterPrint(320, ypos - y, s, 0, 1);
		}
		ypos += 20;
		i++;
		if (ypos - y >= 480)
			return;
	}
}

void Credits(MGLDraw *mgl)
{
	int y = -470;
	static byte flip = 0;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	while (1)
	{
		mgl->ClearScreen();
		CreditsRender(y);

		// only scroll every other frame
		flip = 1 - flip;
		if (flip)
			y += 1;

		mgl->Flip();
		if (!mgl->Process())
			return;
		if (mgl->LastKeyPressed())
			return;
		if (y == END_OF_CREDITS)
			return;
	}
}

void VictoryTextRender(int y)
{
	int i, ypos;
	char *s;

	i = 0;

	ypos = 0;
	while (victoryTxt[i][0] != '$')
	{
		s = victoryTxt[i];
		if (ypos - y>-60)
		{
			if (s[0] == '@')
			{
				CenterPrint(320, ypos - y, &s[1], 0, 0);
			}
			else if (s[0] == '#')
			{
				DrawFillBox(320 - 200, ypos - y + 8, 320 + 200, ypos - y + 11, 255);
			}
			else if (s[0] == '%')
			{
				DrawFillBox(320 - 70, ypos - y + 8, 320 + 70, ypos - y + 9, 255);
			}
			else
				CenterPrint(320, ypos - y, s, 0, 1);
		}

		ypos += 20;
		i++;
		if (ypos - y >= 480)
			return;
	}
}

void VictoryText(MGLDraw *mgl)
{
	int y = -470;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	while (1)
	{
		mgl->ClearScreen();
		VictoryTextRender(y);
		y += 1;
		mgl->Flip();
		if (!mgl->Process())
			return;
		if (mgl->LastKeyPressed() == 27)
			return;
		if (y == END_OF_VICTORY)
			return;
	}
}

byte SpeedSplash(MGLDraw *mgl, const char *fname)
{
	int i, j, clock;
	RGB desiredpal[256], curpal[256];
	byte mode, done;
	byte c, oldc;


	for (i = 0; i < 256; i++)
	{
		curpal[i].r = 0;
		curpal[i].g = 0;
		curpal[i].b = 0;
	}
	mgl->SetPalette(curpal);

	mgl->LastKeyPressed();
	oldc = GetControls() | GetArrows();

	if (!mgl->LoadBMP(fname, desiredpal))
		return 0;

	mode = 0;
	clock = 0;
	done = 0;
	while (!done)
	{
		mgl->Flip();
		if (!mgl->Process())
			return 0;
		c = mgl->LastKeyPressed();

		if (c == 27)
			return 0;
		else if (c)
			mode = 2;

		c = GetControls() | GetArrows();
		if ((c & (CONTROL_B1 | CONTROL_B2)) && (!(oldc & (CONTROL_B1 | CONTROL_B2))))
			mode = 2;
		oldc = c;

		clock++;
		switch (mode) {
			case 0: // fading in
				for (j = 0; j < 16; j++)
					for (i = 0; i < 256; i++)
					{
						if (curpal[i].r < desiredpal[i].r)
							curpal[i].r++;
						if (curpal[i].g < desiredpal[i].g)
							curpal[i].g++;
						if (curpal[i].b < desiredpal[i].b)
							curpal[i].b++;
					}
				mgl->SetPalette(curpal);
				if (clock > 16)
				{
					mode = 1;
					clock = 0;
				}
				break;
			case 1:
				// sit around
				break;
			case 2: // fading out
				clock = 0;
				for (j = 0; j < 16; j++)
					for (i = 0; i < 256; i++)
					{
						if (curpal[i].r > 0)
							curpal[i].r--;
						else
							clock++;
						if (curpal[i].g > 0)
							curpal[i].g--;
						else
							clock++;
						if (curpal[i].b > 0)
							curpal[i].b--;
						else
							clock++;
					}
				mgl->SetPalette(curpal);
				if (clock == 256 * 3 * 16)
					done = 1;
				break;
		}
	}
	mgl->ClearScreen();
	mgl->Flip();
	return 1;
}

void HelpScreens(MGLDraw *mgl)
{
	int i;
	char name[32];

	for (i = 0; i < 5; i++)
	{
		sprintf(name, "docs/help%d.bmp", i + 1);
		if (!SpeedSplash(mgl, name))
			return;
	}
}

void DemoSplashScreens(MGLDraw *mgl)
{
	if (!SpeedSplash(mgl, "docs/demosplash.bmp"))
		return;
	if (!SpeedSplash(mgl, "docs/demosplash2.bmp"))
		return;
}

void SplashScreen(MGLDraw *mgl, const char *fname, int delay, byte sound)
{
	int i, j, clock;
	RGB desiredpal[256], curpal[256];
	byte mode, done;

	for (i = 0; i < 256; i++)
	{
		curpal[i].r = 0;
		curpal[i].g = 0;
		curpal[i].b = 0;
	}
	mgl->SetPalette(curpal);

	mgl->LastKeyPressed();

	if (!mgl->LoadBMP(fname, desiredpal))
		return;

	mode = 0;
	clock = 0;
	done = 0;
	while (!done)
	{
		mgl->Flip();
		if (!mgl->Process())
			return;
		if (mgl->LastKeyPressed())
			mode = 2;

		clock++;
		switch (mode) {
			case 0: // fading in
				for (j = 0; j < 8; j++)
					for (i = 0; i < 256; i++)
					{
						if (curpal[i].r < desiredpal[i].r)
							curpal[i].r++;
						if (curpal[i].g < desiredpal[i].g)
							curpal[i].g++;
						if (curpal[i].b < desiredpal[i].b)
							curpal[i].b++;
					}
				mgl->SetPalette(curpal);
				if (clock == 32)
				{
					if (sound == 2)
						MakeNormalSound(SND_HAMUMU);
				}
				if (clock > 64)
				{
					mode = 1;
					clock = 0;
				}
				break;
			case 1:
				if (clock > delay)
				{
					mode = 2;
					clock = 0;
				}
				break;
			case 2: // fading out
				clock = 0;
				for (j = 0; j < 8; j++)
					for (i = 0; i < 256; i++)
					{
						if (curpal[i].r > 0)
							curpal[i].r--;
						else
							clock++;
						if (curpal[i].g > 0)
							curpal[i].g--;
						else
							clock++;
						if (curpal[i].b > 0)
							curpal[i].b--;
						else
							clock++;
					}
				mgl->SetPalette(curpal);
				if (clock == 256 * 3 * 8)
					done = 1;
				break;
		}
	}
	mgl->ClearScreen();
	mgl->Flip();
}
