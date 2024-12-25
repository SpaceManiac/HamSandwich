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
	class Archipelago gArchipelagoManager;
	std::unique_ptr<ArchipelagoClient> ap;

	std::string serverAddress = "localhost:38281"; //"archipelago.gg:";
	std::string slotName = "Spac";
	std::string password;

	constexpr int BASE_ID = 1234;  // TODO

	constexpr int MESSAGE_TIME = 30 * 5;
	int messageCooldown = MESSAGE_TIME;
	bool countdownDone = false;
}

// ----------------------------------------------------------------------------

class Archipelago* Archipelago()
{
	return ap ? &gArchipelagoManager : nullptr;
}

std::string_view Archipelago::Status()
{
	return
		!ap ? "Not connected" :
		!ap->error_message().empty() ? ap->error_message() :
		!ap->is_active() ? ap->connection_status() :
		//locationWait ? "Scouting locations..." :
		"Active";
}

byte Archipelago::MysticItemAtLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	if (ap->checked_locations().find(location_id) != ap->checked_locations().end())
	{
		// It's been collected.
		return ITM_NONE;
	}

	if (auto item = ap->item_at_location(location_id))
	{
		int64_t km_item_id = item->item - BASE_ID;
		// TODO: check that km_item_id is in range
	}
	// Problem with the scouting.
	return ITM_ARCHIPELAGO;
}

std::string_view Archipelago::ItemNameAtLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	if (auto item = ap->item_at_location(location_id))
	{
		return ap->item_name(item->item);
	}
	// Problem with the scouting.
	return "Unknown";
}

void Archipelago::PickupItem(int chapter, int levelNum)
{
	int id = BASE_ID + chapter * 50 + levelNum;
	ap->check_location(id);
}

void Archipelago::CompleteChapter(int chapter)
{
	if (chapter == 3)
	{
		ap->check_goal();
	}
}

void Archipelago::Update()
{
	if (!ap)
	{
		return;
	}

	ap->update();

	// Handle reconnection by refreshing stuff.
	if (ap->pop_connected())
	{
		std::vector<int64_t> all_locations;
		all_locations.insert(all_locations.end(), ap->checked_locations().begin(), ap->checked_locations().end());
		all_locations.insert(all_locations.end(), ap->missing_locations().begin(), ap->missing_locations().end());
		ap->scout_locations(all_locations);
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		//ItemReceived(item->item, true);
	}

	// Location checks
	// TODO: track already-checked locations better
	for (int64_t loc : ap->checked_locations())
	{
		loc -= BASE_ID;
		/*if (!locsFound[loc])
		{
			SetLocationChecked(loc);
		}*/
	}

	// Messages
	if (messageCooldown > 0)
	{
		messageCooldown--;
	}
	else if (auto message = ap->pop_message())
	{
		if (message->type == ArchipelagoClient::Message::ItemSend)
		{
			if (message->receiving == ap->player_id())
			{
				// Got [item] from [player].
				std::string text = "Got ";
				text += ap->item_name(message->item.item);
				text += " from ";
				text += ap->slot_player_alias(message->item.player);
				NewMessage(text.c_str(), MESSAGE_TIME);
				messageCooldown = MESSAGE_TIME;
			}
			else if (message->item.player == ap->player_id())
			{
				// Sent [item] to [player].
				std::string text = "Sent ";
				text += ap->item_name(message->item.item);
				text += " to ";
				text += ap->slot_player_alias(message->receiving);
				NewMessage(text.c_str(), MESSAGE_TIME);
				messageCooldown = MESSAGE_TIME;
			}
		}
		else if (message->type == ArchipelagoClient::Message::Countdown)
		{
			if (message->countdown == 0)
			{
				countdownDone = true;
			}
		}
	}
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
	InitPlayer(INIT_GAME, 0, 0);

	while (running)
	{
		// --------------------------------------------------------------------
		// Update
		byte chapterLimit = ap && ap->is_active() ? 4 : 0;
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
					player.worldNum = cursor - 5;
					player.overworldX = -2000;
					AWAIT ShowVictoryAnim(player.worldNum);
					AWAIT LunaticGame(mgl, 1);
					oldc = ~0;
					continue;
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

		countdownDone = false;
		gArchipelagoManager.Update();
		if (countdownDone && chapterLimit == 1)
		{
			player.worldNum = 0;
			player.overworldX = -2000;
			AWAIT ShowVictoryAnim(0);
			AWAIT LunaticGame(mgl, 1);
			oldc = ~0;
			continue;
		}

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
		{
			Print(320 + typingY, (cursor + 3)*20, "_", 0, 1);
		}

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
