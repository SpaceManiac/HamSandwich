#include "archipelago.h"
#include <map>
#include <memory>
#include <string>
#include <aegea.h>
#include <json.h>
#include "sound.h"
#include "display.h"
#include "control.h"
#include "game.h"
#include "spell.h"
#include "shop.h"

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

	std::map<int64_t, byte> appearance_map = {
		{BASE_ID + 0 * 50 + 5, ITM_KEYCH1},
		{BASE_ID + 1 * 50 + 4, ITM_KEYCH2},
		{BASE_ID + 2 * 50 + 7, ITM_KEYCH3},
		{BASE_ID + 3 * 50 + 8, ITM_KEYCH4},
		{BASE_ID + 0 * 50 + 11, ITM_SPELLBOOK},
		{BASE_ID + 0 * 50 + 2, ITM_SPELLBOOK},
		{BASE_ID + 0 * 50 + 3, ITM_SPELLBOOK},
		{BASE_ID + 0 * 50 + 4, ITM_SPELLBOOK},
		{BASE_ID + 1 * 50 + 2, ITM_SPELLBOOK},
		{BASE_ID + 1 * 50 + 6, ITM_SPELLBOOK},
		{BASE_ID + 1 * 50 + 11, ITM_SPELLBOOK},
		{BASE_ID + 1 * 50 + 5, ITM_SPELLBOOK},
		{BASE_ID + 1 * 50 + 3, ITM_SPELLBOOK},
		{BASE_ID + 2 * 50 + 11, ITM_SPELLBOOK},
		{BASE_ID + 2 * 50 + 6, ITM_SPELLBOOK},
		{BASE_ID + 2 * 50 + 13, ITM_SPELLBOOK},
		{BASE_ID + 2 * 50 + 16, ITM_SPELLBOOK},
		{BASE_ID + 2 * 50 + 9, ITM_SPELLBOOK},
		{BASE_ID + 3 * 50 + 6, ITM_SPELLBOOK},
		{BASE_ID + 3 * 50 + 7, ITM_SPELLBOOK},
		{BASE_ID + 3 * 50 + 9, ITM_SPELLBOOK},
		{BASE_ID + 3 * 50 + 10, ITM_SPELLBOOK},
		{BASE_ID + 0 * 50 + 12, ITM_FAIRYBELL},
		{BASE_ID + 0 * 50 + 15, ITM_FAIRYBELL},
		{BASE_ID + 0 * 50 + 16, ITM_FAIRYBELL},
		{BASE_ID + 0 * 50 + 17, ITM_FAIRYBELL},
		{BASE_ID + 1 * 50 + 13, ITM_FAIRYBELL},
		{BASE_ID + 1 * 50 + 14, ITM_FAIRYBELL},
		{BASE_ID + 1 * 50 + 15, ITM_FAIRYBELL},
		{BASE_ID + 1 * 50 + 16, ITM_FAIRYBELL},
		{BASE_ID + 2 * 50 + 17, ITM_FAIRYBELL},
		{BASE_ID + 2 * 50 + 18, ITM_FAIRYBELL},
		{BASE_ID + 2 * 50 + 19, ITM_FAIRYBELL},
		{BASE_ID + 2 * 50 + 20, ITM_FAIRYBELL},
		{BASE_ID + 3 * 50 + 12, ITM_FAIRYBELL},
		{BASE_ID + 3 * 50 + 13, ITM_FAIRYBELL},
		{BASE_ID + 3 * 50 + 14, ITM_FAIRYBELL},
		{BASE_ID + 3 * 50 + 15, ITM_FAIRYBELL},
		{BASE_ID + 4 * 50 + 0, 128 + 14},
		{BASE_ID + 4 * 50 + 1, 128 + 14},
		{BASE_ID + 4 * 50 + 2, 128 + 14},
		{BASE_ID + 4 * 50 + 3, 128 + 14},
		{BASE_ID + 4 * 50 + 4, 128 + 32},
		{BASE_ID + 4 * 50 + 5, 128 + 4},
		{BASE_ID + 4 * 50 + 6, 128 + 4},
		{BASE_ID + 4 * 50 + 7, 128 + 4},
		{BASE_ID + 4 * 50 + 8, 128 + 4},
		{BASE_ID + 4 * 50 + 9, 128 + 27},
		{BASE_ID + 4 * 50 + 10, 128 + 9},
		{BASE_ID + 4 * 50 + 11, 128 + 9},
		{BASE_ID + 4 * 50 + 12, 128 + 9},
		{BASE_ID + 4 * 50 + 13, 128 + 9},
		{BASE_ID + 4 * 50 + 14, 128 + 29},
		{BASE_ID + 4 * 50 + 15, 128 + 18},
		{BASE_ID + 4 * 50 + 16, 128 + 19},
		{BASE_ID + 4 * 50 + 17, 128 + 20},
		{BASE_ID + 4 * 50 + 18, 128 + 28},
		{BASE_ID + 4 * 50 + 19, 128 + 30},
		{BASE_ID + 4 * 50 + 20, 128 + 25},
		{BASE_ID + 4 * 50 + 21, 128 + 23},
		{BASE_ID + 4 * 50 + 22, 128 + 21},
		{BASE_ID + 4 * 50 + 23, 128 + 31},
	};
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

bool Archipelago::HasCheckedLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	return ap->checked_locations().find(location_id) != ap->checked_locations().end();
}

byte Archipelago::MysticItemAtLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	if (auto item = ap->item_at_location(location_id))
	{
		byte appearance = appearance_map[item->item];
		if (appearance == 128 + 14)
		{
			return appearance + std::min(player.hat, (byte)3);
		}
		else if (appearance == 128 + 4)
		{
			return appearance + std::min(player.staff, (byte)3);
		}
		else if (appearance == 128 + 9)
		{
			return appearance + std::min(player.boots, (byte)3);
		}
		else if (appearance != 0)
		{
			return appearance;
		}
	}
	// Problem with the scouting, or it belongs to another world.
	return ITM_ARCHIPELAGO;
}

static std::string_view ItemName(const ArchipelagoClient::Item& item, int offset)
{
	auto name = ap->item_name(item.item);
	if (item.player == ap->player_id())
	{
		if (name == "Hat Upgrade")
		{
			name =
				(player.hat + offset) == 0 ? "Felt Hat" :
				(player.hat + offset) == 1 ? "Moon Hat" :
				(player.hat + offset) == 2 ? "Checkered Hat" :
				"Steel Hat";
		}
		else if (name == "Staff Upgrade")
		{
			name =
				(player.staff + offset) == 0 ? "Pine Staff" :
				(player.staff + offset) == 1 ? "Oak Staff" :
				(player.staff + offset) == 2 ? "Ironwood Staff" :
				"Mystical Staff";
		}
		else if (name == "Boots Upgrade")
		{
			name =
				(player.boots + offset) == 0 ? "Leather Boots" :
				(player.boots + offset) == 1 ? "Doc Merlins" :
				(player.boots + offset) == 2 ? "Lucky Boots" :
				"Air Lancelots";
		}
		else if (name == "Energy Barrage")
		{
			if (player.spell[0])
			{
				name = "Energy Storm";
			}
		}
		else if (name == "Dragon's Flame")
		{
			if (player.spell[1])
			{
				name = "Liquify";
			}
		}
		else if (name == "Seeker Bolt")
		{
			if (player.spell[2])
			{
				name = "Seeker Swarm";
			}
		}
		else if (name == "Ice Blast")
		{
			if (player.spell[3])
			{
				name = "Ice Beam";
			}
		}
		else if (name == "Inferno")
		{
			if (player.spell[4])
			{
				name = "Hyper Inferno";
			}
		}
		else if (name == "Summon Ptero")
		{
			if (player.spell[5])
			{
				name = "Summon Golem";
			}
		}
		else if (name == "Stoneskin")
		{
			if (player.spell[6])
			{
				name = "Steelskin";
			}
		}
		else if (name == "Berserk")
		{
			if (player.spell[7])
			{
				name = "Insane Rage";
			}
		}
		else if (name == "Healing")
		{
			if (player.spell[8])
			{
				name = "Life";
			}
		}
	}
	return name;
}

std::string_view Archipelago::ItemNameAtLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	if (auto item = ap->item_at_location(location_id))
	{
		return ItemName(*item, 0);
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
		ap->scout_locations();
		ap->storage_get({ ap->storage_private("xp"), ap->storage_private("money") });
		// Server will send ReceivedItems soon, so we need to reset progression.
		memset(player.spell, 0, 9);
		player.hat = player.staff = player.boots = 0;
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		int64_t item_id = item->item - BASE_ID;
		// TODO: messages and effects should be in pop_message block, not here.

		// Armageddon Sword pieces
		Guy *me = GetGoodguy();
		int x = me ? me->x : 0, y = me ? me->y : 0;
		if (item_id == 0*50+5 && !player.keychain[0])
		{
			player.keychain[0] = 1;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
		}
		else if (item_id == 1*50+4 && !player.keychain[1])
		{
			player.keychain[1] = 1;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
		}
		else if (item_id == 2*50+7 && !player.keychain[2])
		{
			player.keychain[2] = 1;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
		}
		else if (item_id == 3*50+8 && !player.keychain[3])
		{
			player.keychain[3] = 1;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
		}
		if (player.keychain[0] && player.keychain[1] && player.keychain[2] && player.keychain[3] && !player.spell[9])
		{
			// Unlike vanilla, does not max out staff
			player.spell[9] = 1;
			NewBigMessage("The sword is complete!",30);
			SendMessageToGame(MSG_SHOWANIM, 10);
			SetKidSprite(1);
		}

		// Spellbooks
		byte spellbook = SpellBookForThisLevel(item_id);
		if (spellbook != 255)
		{
			if(player.spell[spellbook]==1)
			{
				NewBigMessage(spellName[spellbook*2+1],75);
				player.spell[spellbook]=2;
			}
			else if(player.spell[spellbook]==0)
			{
				NewBigMessage(spellName[spellbook*2],75);
				player.spell[spellbook]=1;
			}
			// Unlike vanilla, does not set curSpell
		}

		// Fairies
		byte fairy = FairyForThisLevel(item_id);
		if (fairy != 0)
		{
			// TODO: set chaseFairy instead?
			player.haveFairy |= (1 << (fairy - 1));
		}

		// Progressive hat, staff, and boots
		if (item_id == 4 * 50 + 3)
		{
			player.hat++;
		}
		else if (item_id == 4 * 50 + 8)
		{
			player.staff++;
		}
		else if (item_id == 4 * 50 + 13)
		{
			player.boots++;
		}
		// Other shop upgrades
		else if (item_id / 50 == 4)
		{
			BuyGear(item_id - 4 * 50);
		}
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
				text += ItemName(message->item, -1);
				if (message->item.player != ap->player_id())
				{
					text += " from ";
					text += ap->slot_player_alias(message->item.player);
				}
				NewMessage(text.c_str(), MESSAGE_TIME);
				MakeNormalSound(SND_MESSAGE);
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

	while (auto change = ap->pop_storage_change())
	{
		if (ap->is_storage_private(change->key, "xp"))
		{
			if (change->new_value->isLong())
			{
				player.prevScore = player.score = change->new_value->getLong();
			}
		}
		else if (ap->is_storage_private(change->key, "money"))
		{
			if (change->new_value->isLong())
			{
				player.prevMoney = player.money = std::min(change->new_value->getLong(), (long long)UINT16_MAX);
			}
		}
	}

	// Re-use prevScore and prevMoney since we're not resetting them on death.
	if (player.score != player.prevScore)
	{
		player.prevScore = player.score;
		ap->storage_set(ap->storage_private("xp"), player.score, false);
	}

	if (player.money != player.prevMoney)
	{
		player.prevMoney = player.money;
		ap->storage_set(ap->storage_private("money"), player.money, false);
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
		bool chapters[4] = { ap && ap->is_active(), false, false, false };
		int typingY = -1;

		byte c = GetControls() | GetArrows();
		byte k = mgl->LastKeyPressed();
		if (!typing)
		{
			if (c & ~oldc & CONTROL_UP)
			{
				if (cursor == 0)
				{
					cursor = 8;
				}
				else
				{
					--cursor;
				}
				while (cursor >= 5 && cursor < 9 && !chapters[cursor - 5])
				{
					--cursor;
				}
			}
			if (c & ~oldc & CONTROL_DN)
			{
				++cursor;
				while (cursor >= 5 && cursor < 9 && !chapters[cursor - 5])
				{
					++cursor;
				}
				if (cursor > 8)
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
		if (countdownDone && !chapters[1] && !chapters[2] && !chapters[3])
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

		if (chapters[0]) CenterPrint(320,9*20,"Chapter 1",0,1);
		if (chapters[1]) CenterPrint(320,10*20,"Chapter 2",0,1);
		if (chapters[2]) CenterPrint(320,11*20,"Chapter 3",0,1);
		if (chapters[3]) CenterPrint(320,12*20,"Chapter 4",0,1);

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
