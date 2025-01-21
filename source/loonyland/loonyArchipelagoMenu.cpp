#include "loonyArchipelagoMenu.h"
#include "plasma.h"
#include "mgldraw.h"
#include <control.h>
#include "loonyArchipelago.h"
#include "badge.h"
#include "options.h"

static std::string IPAddress = "";
static std::string SlotName = "";
static std::string Password = "";

static byte cursor;
static byte oldc;
static byte optMode;
static bool quit = false;

static std::string status;
static std::string oldStatus = "";

#define CURSOR_IPADDRESS	0
#define CURSOR_SLOTNAME		1
#define CURSOR_PASSWORD		2
#define CURSOR_CONNECT		3

#define CURSOR_START	0
#define CURSOR_END		3

void InitArchipelagoMenu(void)
{

	oldc = 255;
	cursor = CURSOR_START;
	InitPlasma(7);
	IPAddress = "Archipelago.gg:";
	//AP_Init("Archipelago.gg:0000", "Loonyland", "AutoFrenzy", "");
}


void ExitArchipelagoMenu(void)
{
	ExitPlasma();
}

TASK(byte)
UpdateArchipelagoMenu(int* lastTime, MGLDraw* mgl)
{
	char c;
	byte c2 = 255;
	dword btn, j;
	int i;

	UpdateArchipelago();

	if (*lastTime > TIME_PER_FRAME * 30)
		*lastTime = TIME_PER_FRAME * 30;
	while (*lastTime >= TIME_PER_FRAME)
	{
		switch (optMode)
		{
		case 0: // just going through options
			c = mgl->LastKeyPressed();
			c2 = GetControls() | GetArrows();

			if (c == SDLK_ESCAPE)
			{
				quit = true;
				CO_RETURN 1;
			}

			if (c == SDLK_j)
			{
				quit = false;
				ap_modesAvail[0] = 1;
				ap_modesAvail[1] = 1;
				ap_modesAvail[2] = 1;
				ap_modesAvail[3] = 1;
				ap_modesAvail[4] = 1;
				ap_modesAvail[5] = 1;
				CO_RETURN 1;
			}

			if (c2 & ~oldc & CONTROL_UP)
			{
				cursor--;
				if (cursor > CURSOR_END)
					cursor = CURSOR_END;
				MakeNormalSound(SND_MENUCLICK);
			}
			if (c2 & ~oldc & CONTROL_DN)
			{
				cursor++;
				if (cursor > CURSOR_END)
					cursor = 0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if (c2 & ~oldc & CONTROL_LF)
			{

			}
			if (c2 & ~oldc & CONTROL_RT)
			{

			}
			if (c2 & ~oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))
			{
				switch (cursor)
				{
				case CURSOR_IPADDRESS: //seed entry
					MakeNormalSound(SND_MENUSELECT);
					LastScanCode();
					optMode = 1;
					break;
				case CURSOR_SLOTNAME: //seed entry
					MakeNormalSound(SND_MENUSELECT);
					LastScanCode();
					optMode = 1;
					break;
				case CURSOR_PASSWORD: //seed entry
					MakeNormalSound(SND_MENUSELECT);
					LastScanCode();
					optMode = 1;
					break;
				case CURSOR_CONNECT: //generate
					optMode = 2;
					break;
				}
			}
			break;
		case 1: // Typing in something
		{
			std::string * activeString = &IPAddress;
			switch (cursor)
			{
			case CURSOR_IPADDRESS: //address
				activeString = &IPAddress;
				break;
			case CURSOR_SLOTNAME: //slot
				activeString = &SlotName;
				break;
			case CURSOR_PASSWORD: //pass
				activeString = &Password;
				break;
			}


			c = mgl->LastKeyPressed();
			if (c == SDLK_ESCAPE || c == SDLK_RETURN) // ESC key
			{
				optMode = 0;
				c2 = 255;
				oldc = 255;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUSELECT);
				quit = false;
				CO_RETURN 0;
			}
			else if (c == SDLK_BACKSPACE && activeString->length() > 0)
			{
				activeString->pop_back();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if (c != 0)
			{
				if (c >= ' ' && c <= '~')
				{
					*activeString += c;
					MakeNormalSound(SND_MENUSELECT);

				}
			}

			c2 = 255;
			break;
		}
		case 2: //connecting
			ArchipelagoConnect(IPAddress, SlotName, Password);
			optMode = 3;
			break;
		case 3: //waiting on status
			if (mgl->LastKeyPressed() == SDLK_ESCAPE)
			{
				optMode = 0;
				Disconnect();
				break;
			}

			status = ConnectionStatus();

			if (status != oldStatus) {
				oldStatus = status;
				printf("DEBUG: Status: %s\n", status.c_str());
			}

			if (status == "Active")
			{
				optMode = 4;
			}
			else if (status == "Failed"|| status == "ConnectionRefused")
			{
				optMode = 0;
				Disconnect();
				MakeNormalSound(SND_BONKOUCH);
			}
			break;
		case 4: //waiting on status
			if (!locationWait) {
				CO_RETURN 1;
			}
			break;
		}

		oldc = c2;


		UpdatePlasma();

		*lastTime -= TIME_PER_FRAME;
	}
	CO_RETURN 0;
}

void RenderArchipelagoMenu(MGLDraw* mgl)
{
	int wid;
	byte* pos;
	int i;
	std::string passwordHidden;

	wid = mgl->GetWidth();
	pos = mgl->GetScreen() + 40 * wid;
	for (i = 40; i < 480 - 40; i++)
	{
		memset(pos, 7 * 32 + 2, 640);
		pos += wid;
	}

	RenderPlasma(mgl, 1);
	RenderPlasma(mgl, 0);

	CenterPrintGlow(320, 2, "Archipelago", 0, 2);

	PrintColor(160, 60 + CURSOR_IPADDRESS * 30, "IP Address: ", 7, -10, 0);
	PrintColor(310, 60 + CURSOR_IPADDRESS * 30, IPAddress.c_str(), 7, -10, 0);
	if (cursor == CURSOR_IPADDRESS)
	{
		PrintColor(159, 59 + CURSOR_IPADDRESS * 30, "IP Address: ", 0, 0, 0);
		if (optMode == 1) {
			IPAddress.push_back('_');
			PrintColor(309, 59 + CURSOR_IPADDRESS * 30, IPAddress.c_str(), 0, 0, 0);
			IPAddress.pop_back();
		}
		else {
			PrintColor(309, 59 + CURSOR_IPADDRESS * 30, IPAddress.c_str(), 0, -5, 0);
		}
	}

	PrintColor(160, 60 + CURSOR_SLOTNAME * 30, "Slot Name: ", 7, -10, 0);
	PrintColor(310, 60 + CURSOR_SLOTNAME * 30, SlotName.c_str(), 7, -10, 0);
	if (cursor == CURSOR_SLOTNAME)
	{
		PrintColor(159, 59 + CURSOR_SLOTNAME * 30, "Slot Name: ", 0, 0, 0);
		if (optMode == 1) {
			SlotName.push_back('_');
			PrintColor(309, 59 + CURSOR_SLOTNAME * 30, SlotName.c_str(), 0, 0, 0);
			SlotName.pop_back();
		}
		else {
			PrintColor(309, 59 + CURSOR_SLOTNAME * 30, SlotName.c_str(), 0, -5, 0);
		}
	}

	passwordHidden.clear();
	for (int passLength = 0; passLength < Password.length(); passLength++)
	{
		passwordHidden.push_back('o');
	}

	PrintColor(160, 60 + CURSOR_PASSWORD * 30, "Password: ", 7, -10, 0);
	PrintColor(310, 60 + CURSOR_PASSWORD * 30, passwordHidden.c_str(), 7, -10, 0);
	if (cursor == CURSOR_PASSWORD)
	{
		PrintColor(159, 59 + CURSOR_PASSWORD * 30, "Password: ", 0, 0, 0);
		if (optMode == 1) {
			passwordHidden.push_back('_');
			PrintColor(309, 59 + CURSOR_PASSWORD * 30, passwordHidden.c_str(), 0, 0, 0);
			passwordHidden.pop_back();
		}
		else {
			PrintColor(309, 59 + CURSOR_PASSWORD * 30, passwordHidden.c_str(), 0, -5, 0);
		}
	}

	PrintColor(160, 60 + CURSOR_CONNECT * 30 + 20, "Connect!", 7, -10, 0);
	if (cursor == CURSOR_CONNECT)
		PrintColor(159, 59 + CURSOR_CONNECT * 30 + 20, "Connect!", 0, 0, 0);

	PrintGlowRect(160 - 2, 60 + 5 * 30 + 20, SCRWID - 160 + 2, SCRHEI, ConnectionStatus(), 30, -12, 0);
}

//----------------

TASK(bool)
ArchipelagoMenu(MGLDraw* mgl)
{
	byte done = 0;
	int lastTime;

	mgl->LoadBMP("graphics/title.bmp");

	InitArchipelagoMenu();
	lastTime = 1;

	while (!done)
	{
		lastTime += TimeLength();
		StartClock();
		done = AWAIT UpdateArchipelagoMenu(&lastTime, mgl);
		RenderArchipelagoMenu(mgl);
		AWAIT mgl->Flip();
		EndClock();

		if (!mgl->Process())
			done = 1;
	}

	ExitArchipelagoMenu();
	CO_RETURN quit;
}
