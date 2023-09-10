#include "nameentry.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "progress.h"
#include "appdata.h"
#include "title.h"
#include "steam.h"

namespace
{
	byte* backgd;
	int textBright;
	char entry[16];
	char cursorBright, dCBright;
	byte curLine, error;
	int errorBright;

	dword lastGamepad;

	bool gamepadMode;
	enum class KeyboardShift {
		None,
		One,
		//Many,
	} keyboardShift;
	int curRow = 0, curCol = 0;

	constexpr int KEYBOARD_WIDTH = 10;
	constexpr int KEYBOARD_HEIGHT = 4;
	const char KEYBOARD[KEYBOARD_HEIGHT][KEYBOARD_WIDTH+1] = {
		"1234567890",
		"qwertyuiop",
		"asdfghjkl ", // backspace
		" zxcvbnm  ", // shift, done
	};
}

void InitNameEntry(MGLDraw *mgl)
{
	int i;

	textBright=-250;
	curLine=0;

	entry[0]='\0';
	dCBright=1;
	cursorBright=-31;
	error=0;
	errorBright=-200;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	lastGamepad = ~0;
	gamepadMode = false;
	keyboardShift = KeyboardShift::None;
}

void ExitNameEntry()
{
	MGLDraw::StopTextInput();
	free(backgd);
}

static bool CheckForExistingName(const char *name)
{
	char s[64];
	sprintf(s,"profiles/%s.prf",name);
	return AssetOpen_SDL_Owned(s) != nullptr;
}

byte UpdateNameEntry(int *lastTime,MGLDraw *mgl)
{
	bool startTextInput = false;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		if(textBright<40 && curLine<9)
		{
			if(textBright<0 && Random(2)==0)
				MakeNormalSound(SND_BULLETHIT);
			textBright+=8;
			if(textBright>=40 && curLine<9)
			{
				curLine++;
				textBright=-250;
			}
		}
		if(error && errorBright<40)
		{
			if(errorBright<0 && Random(2)==0)
				MakeNormalSound(SND_BULLETHIT);
			errorBright+=8;
			if (errorBright >= 40)
				startTextInput = true;
		}
		cursorBright+=dCBright;
		if(cursorBright>8)
			dCBright=-2;
		else if(cursorBright<-30)
			dCBright=2;

		*lastTime-=TIME_PER_FRAME;
	}

	dword gamepad = GetGamepadButtons();

	if (curLine == 9)
	{
		startTextInput = true;
		curLine = 10;
	}

	if (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_X))
		startTextInput = true;
	if (gamepad)
		gamepadMode = true;

	char c = mgl->LastKeyPressed();
	byte taps = GetTaps();
	if (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_Y))
	{
		switch (keyboardShift)
		{
			case KeyboardShift::None:
				keyboardShift = KeyboardShift::One;
				break;
			case KeyboardShift::One:
				keyboardShift = KeyboardShift::None;
				break;
		}
	}
	if (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_A))
	{
		if (curRow == 2 && curCol == 9)
		{
			c = SDLK_BACKSPACE;
		}
		else if (curRow == 3 && curCol == 0)
		{
			switch (keyboardShift)
			{
				case KeyboardShift::None:
					keyboardShift = KeyboardShift::One;
					break;
				case KeyboardShift::One:
					keyboardShift = KeyboardShift::None;
					break;
			}
		}
		else if (curRow == 3 && curCol == 8)
		{
			c = SDLK_RETURN;
		}
		else if (KEYBOARD[curRow][curCol] != ' ')
		{
			c = KEYBOARD[curRow][curCol];
			if (keyboardShift != KeyboardShift::None)
				c = toupper(c);
			if (keyboardShift == KeyboardShift::One)
				keyboardShift = KeyboardShift::None;
		}
	}
	if (taps & CONTROL_LF)
	{
		if (curCol > 0)
			--curCol;
	}
	if (taps & CONTROL_RT)
	{
		if (curCol < KEYBOARD_WIDTH - 1)
			++curCol;
	}
	if (taps & CONTROL_UP)
	{
		if (curRow > 0)
			--curRow;
	}
	if (taps & CONTROL_DN)
	{
		if (curRow < KEYBOARD_HEIGHT - 1)
			++curRow;
	}
	if (curRow == 3 && curCol == 9)
		curCol = 8;

	if (startTextInput)
	{
		SDL_SetHint(SDL_HINT_RETURN_KEY_HIDES_IME, "0");
		mgl->StartTextInput(15, 315, 640-15, 315+28);
	}

	if(c==SDLK_RETURN)	// enter
	{
		MakeNormalSound(SND_MENUSELECT);
		if(strlen(entry)>2)
		{
			if(CheckForExistingName(entry))
			{
				error=3;
				errorBright=-200;
			}
			else
				return 1;
		}
		else
		{
			error=1;
			errorBright=-200;
		}
	}
	else if(c == SDLK_ESCAPE || (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_BACK)))
	{
		return 255;  // cancel
	}
	else if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
	{
		if(strlen(entry)<15)
		{
			entry[strlen(entry)+1]='\0';
			entry[strlen(entry)]=c;
			MakeNormalSound(SND_MENUCLICK);
		}
	}
	else if(c==SDLK_BACKSPACE || (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_B)))
	{
		if(strlen(entry)>0)
		{
			entry[strlen(entry)-1]='\0';
			MakeNormalSound(SND_MENUCLICK);
		}
	}
	else if(c!=0)
	{
		error=2;
		errorBright=-200;
	}

	lastGamepad = gamepad;
	return 0;
}

void RenderLine(int y,const char *txt,int num)
{
	if(curLine==num)
		PrintProgressiveGlow(20,y,txt,textBright,2);
	else if(curLine>num)
		PrintGlow(20,y,txt,0,2);
}

void RenderNameEntry(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	RenderLine(20,"LunaticOS v3.7b",0);
	RenderLine(40,"Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY, 1);

	RenderLine(100,"SPISPOPD United Plan for Really Evil Maniac Extermination",2);
	RenderLine(120,"(Project S.U.P.R.E.M.E.) *** TOP SECRET ***",3);
	RenderLine(140,"with Colossal Hordes of Extremely Evil Spooky Enemies (C.H.E.E.S.E.)",4);
	RenderLine(180,"Please enter your login name, and press Enter for system access.",5);

	RenderLine(220,"(Note: pick a name you like, because you can't change it without",6);
	RenderLine(240,"losing all of your progress in the game!)",7);

	RenderLine(260,"(Note2: Inappropriate names will be banned from online high score list!)",8);

	PrintGlow(20,320,"LOGIN>",0,2);
	PrintGlow(75,320,entry,0,2);
	PrintGlow(75+GetStrLength(entry,2)+1,320,"_",cursorBright,2);

	if(error==1)
		PrintProgressiveGlow(20,350,"Login name must be at least 3 characters long.",errorBright,2);
	else if(error==2)
		PrintProgressiveGlow(20,350,"Login name may only contain letters and numbers.",errorBright,2);
	else if(error==3)
		PrintProgressiveGlow(20,350,"That name's already in use on this computer!",errorBright,2);

	if (gamepadMode && !SDL_HasScreenKeyboardSupport())
	{
		for (int row = 0; row < KEYBOARD_HEIGHT; ++row)
		{
			for (int col = 0; col < KEYBOARD_WIDTH; ++col)
			{
				char ch = KEYBOARD[row][col];
				std::string_view msg;
				int w = 40-4;
				if (ch != ' ')
				{
					if (keyboardShift != KeyboardShift::None)
						ch = toupper(ch);
					msg = { &ch, 1 };
				}
				else if (row == 2)
				{
					msg = "Del";
				}
				else if (row == 3 && col == 0)
				{
					msg = "Shift";
				}
				else if (row == 3 && col == 8)
				{
					msg = "Enter";
					w = 80-4;
				}

				if (!msg.empty())
				{
					int x = 100 + col * 40, y = 375 + row * 23;
					if (curRow == row && curCol == col)
					{
						mgl->FillBox(x, y, x+w, y+20, 32+8);
						mgl->Box(x, y, x+w, y+20, 32+31);
					}
					else
						mgl->Box(x, y, x+w, y+20, 32+16);

					PrintGlow(x + (w - GetStrLength(msg, 2)) / 2, y + 2, msg, 0, 2);
				}
			}
		}
	}
}

//----------------

TASK(void) NameEntry(MGLDraw *mgl,byte makeNew)
{
	byte done=0;
	int lastTime=1;

	InitNameEntry(mgl);
	if (FirstTime())
		SteamManager::Get()->GetUsername(entry, sizeof entry);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=UpdateNameEntry(&lastTime,mgl);
		RenderNameEntry(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=255;
		EndClock();
	}

	ExitNameEntry();
	if(FirstTime() && done == 255)
	{
		exit(0);
	}
	if(makeNew && done != 255)
	{
		FreeProfile();
		LoadProfile(entry);
		SaveProfile();
	}
}

const char *GetEnteredName()
{
	return entry;
}
