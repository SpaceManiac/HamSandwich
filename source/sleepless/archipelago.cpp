#include "archipelago.h"
#include <aegea.h>
#include "game.h"
#include "player.h"
#include "string_extras.h"

namespace
{
	class Archipelago gArchipelagoManager;
	std::unique_ptr<ArchipelagoClient> ap;

	std::string serverAddress = "localhost:38281"; //"archipelago.gg:";
	std::string slotName = "SpaceManiac"; //"";
	std::string password;

	constexpr int BASE_ID = 10000;  // TODO

	constexpr int MESSAGE_TIME = 30 * 5;
	int messageCooldown = MESSAGE_TIME;
	bool countdownDone = false;

	std::string saveName;
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

const char* Archipelago::SaveName()
{
	return saveName.c_str();
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
		ap->scout_locations();

		std::string newSaveName = "";
		newSaveName += ap->seed_name();
		newSaveName += "_";
		newSaveName += std::to_string(ap->player_id());
		if (saveName != newSaveName)
		{
			saveName = newSaveName;

			// Copy control and sound settings to avoid interruptions.
			// Controls are not even configurable from within AP.
			byte control[2][6];
			byte joyCtrl[2];
			byte sound;
			byte music;
			memcpy(control, profile.control, 2 * 6);
			memcpy(joyCtrl, profile.joyCtrl, 2);
			sound = profile.sound;
			music = profile.music;

			LoadProfile("Archipelago");

			memcpy(profile.control, control, 2 * 6);
			memcpy(profile.joyCtrl, joyCtrl, 2);
			profile.sound = sound;
			profile.music = music;
			ApplyControlSettings();
			JamulSoundVolume(profile.sound);
			SetMusicVolume(profile.music);
		}
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		//int64_t item_id = item->item - BASE_ID;
	}

	// Messages
	if (GetGameMode() == GAMEMODE_PLAY)
	{
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
					//int64_t item_id = message->item.item - BASE_ID;

					std::string text = "Got ";
					text += ap->item_name(message->item.item);
					if (message->item.player != ap->player_id())
					{
						text += " from ";
						text += ap->slot_player_alias(message->item.player);
					}
				}
				else if (message->item.player == ap->player_id())
				{
					// Sent [item] to [player].
					std::string text = "Sent ";
					text += ap->item_name(message->item.item);
					text += " to ";
					text += ap->slot_player_alias(message->receiving);
					NewMessage(text.c_str(), MESSAGE_TIME, 1);
					MakeNormalSound(SND_MESSAGE);
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

	// ------------------------------------------------------------------------
	// Init
	std::string oldProfile = profile.name;
	saveName = "error";
	InitMessage();

	while (running)
	{
		// --------------------------------------------------------------------
		// Update
		bool playOk = ap && ap->is_active() && ap->error_message().empty();
		// Handle disconnects by bumping cursor back up.
		while (cursor >= 5 && cursor < 7 && !playOk)
		{
			--cursor;
		}
		int typingY = -1;

		byte c = GetControls() | GetArrows();
		byte k = mgl->LastKeyPressed();
		if (!typing)
		{
			if (c & ~oldc & CONTROL_UP)
			{
				if (cursor == 0)
				{
					cursor = 5;
				}
				else
				{
					--cursor;
				}
				while (cursor >= 5 && cursor < 6 && !playOk)
				{
					--cursor;
				}
			}
			if (c & ~oldc & CONTROL_DN)
			{
				++cursor;
				while (cursor >= 5 && cursor < 6 && !playOk)
				{
					++cursor;
				}
				if (cursor > 5)
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
					static ArchipelagoCache::FileSystem cache { "appdata/archipelago/" };
					ap = std::make_unique<ArchipelagoClient>("Sleepless Hollow", serverAddress, slotName, password);
					ap->use_cache(&cache);
				}
				else if (cursor == 5)
				{
					AWAIT PlayWorld(mgl, "hollow.shw");
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
		if (countdownDone)
		{
			AWAIT PlayWorld(mgl, "hollow.shw");
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

		if (playOk) CenterPrint(320,9*20,"Play",0,1);

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
	LoadProfile(oldProfile.c_str());
}
