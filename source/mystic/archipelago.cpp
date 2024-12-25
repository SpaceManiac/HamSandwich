#include "archipelago.h"
#include <memory>
#include <string>
#include <aegea.h>
#include "sound.h"
#include "display.h"
#include "control.h"
#include "game.h"

namespace
{
	ArchipelagoManager gArchipelagoManager;
	std::unique_ptr<ArchipelagoClient> ap;

	std::string serverAddress = "localhost:38281"; //"archipelago.gg:";
	std::string slotName = "Spac";
	std::string password;
}

// ----------------------------------------------------------------------------

ArchipelagoManager* Archipelago()
{
	return ap ? &gArchipelagoManager : nullptr;
}

std::string_view ArchipelagoManager::Status()
{
	return
		!ap ? "Not connected" :
		!ap->error_message().empty() ? ap->error_message() :
		!ap->is_active() ? ap->connection_status() :
		//locationWait ? "Scouting locations..." :
		"Active";
}

// ----------------------------------------------------------------------------

TASK(void)
ArchipelagoMenu(MGLDraw* mgl)
{
	bool running = true;

	mgl->LastKeyPressed();
	byte oldc = ~0;
	byte cursor = 1;
	bool typing = false;

	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);

	// ------------------------------------------------------------------------
	// Init

	while (running)
	{
		// --------------------------------------------------------------------
		// Update
		byte chapterLimit = ap && ap->is_active() ? 1 : 0;
		int typingY = -1;

		byte c = GetControls() | GetArrows();
		byte k = mgl->LastKeyPressed();
		if (!typing)
		{
			if (c & ~oldc & CONTROL_UP)
			{
				if (cursor == 0)
				{
					cursor = 4 + chapterLimit;
				}
				else
				{
					--cursor;
				}
			}
			if (c & ~oldc & CONTROL_DN)
			{
				++cursor;
				if (cursor > 4 + chapterLimit)
				{
					cursor = 0;
				}
			}
			if (c & ~oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))
			{
				if (cursor == 0)
				{
					running = false;
				}
				else if (cursor == 1 || cursor == 2 || cursor == 3)
				{
					typing = true;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 4)
				{
					ap = std::make_unique<ArchipelagoClient>("Kid Mystic", serverAddress, slotName, password);
				}
				else if (cursor == 5 || cursor == 6 || cursor == 7 || cursor == 8)
				{
					// TODO: warp to the desired chapter
					AWAIT LunaticGame(mgl, 0);
				}
			}
			if (k == SDLK_ESCAPE)
			{
				running = false;
			}
		}
		else
		{
			std::string* activeString;
			switch (cursor)
			{
			case 1:
				activeString = &serverAddress;
				break;
			case 2:
				activeString = &slotName;
				break;
			case 3:
				activeString = &password;
				break;
			}

			if (k == SDLK_ESCAPE || k == SDLK_RETURN) // ESC key
			{
				k = 0;
				typing = false;
				oldc = ~0;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if (k == SDLK_BACKSPACE && !activeString->empty())
			{
				activeString->pop_back();
			}
			else if (k >= ' ' && k <= '~')
			{
				*activeString += k;
			}

			typingY = GetStrLength(activeString == &password ? std::string(password.size(), '*') : *activeString, 1);
		}
		oldc = c;

		if (ap)
			ap->update();

		// --------------------------------------------------------------------
		// Render
		mgl->ClearScreen();
		CenterPrint(320,2,"Archipelago Randomizer",0,0);

		DrawFillBox(250-80,40-3+20*(cursor+1+(cursor>4)),390+80,40+17-3+20*(cursor+1+(cursor>4)),10);

		CenterPrint(320,3*20,"Exit To Main Menu",0,1);

		RightPrint(320,4*20,"Server address: ",0,1);
		Print(320,4*20,serverAddress,0,1);
		RightPrint(320,5*20,"Slot name: ",0,1);
		Print(320,5*20,slotName,0,1);
		RightPrint(320,6*20,"Password: ",0,1);
		Print(320,6*20,std::string(password.size(), '*'),0,1);
		if (typing && typingY >= 0)
			Print(320 + typingY, (cursor + 3)*20, "_", 0, 1);

		CenterPrint(320,7*20,"Connect",0,1);

		if (chapterLimit >= 1) CenterPrint(320,9*20,"Chapter 1",0,1);
		if (chapterLimit >= 2) CenterPrint(320,10*20,"Chapter 2",0,1);
		if (chapterLimit >= 3) CenterPrint(320,11*20,"Chapter 3",0,1);
		if (chapterLimit >= 4) CenterPrint(320,12*20,"Chapter 4",0,1);

		CenterPrint(320, 14*20, gArchipelagoManager.Status(), 0, 1);

		// --------------------------------------------------------------------
		AWAIT mgl->Flip();
		if (!mgl->Process())
		{
			break;
		}
	}

	// ------------------------------------------------------------------------
	// Exit
	ap.reset();
}
