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

#ifndef NDEBUG
	std::string serverAddress = "localhost:38281";
#else
	std::string serverAddress = "archipelago.gg:";
#endif
	std::string slotName;
	std::string password;

	constexpr int BASE_ID = 1234;  // TODO

	constexpr int MESSAGE_TIME = 30 * 5;
	int messageCooldown = MESSAGE_TIME;
	bool countdownDone = false;

	bool chapters[4];
	byte levelsPassed[4];
	byte spell = 255;
	dword fairy = 0;

	const std::map<int64_t, byte> appearance_map = {
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
		{BASE_ID + 5 * 50 + 49, ITM_LETTERM},
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

void Archipelago::HintShop()
{
	std::vector<int64_t> items;
	for (int i = 0; i < 24; ++i)
	{
		items.push_back(BASE_ID + 4 * 50 + i);
	}
	ap->scout_locations(items, true);
}

bool Archipelago::HasCheckedLocation(int chapter, int levelNum)
{
	return ap->location_is_checked(BASE_ID + chapter * 50 + levelNum);
}

byte Archipelago::MysticItemAtLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	if (auto item = ap->item_at_location(location_id))
	{
		auto iter = appearance_map.find(item->item);
		if (iter != appearance_map.end())
		{
			byte appearance = iter->second;
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
	}
	// Problem with the scouting, or it belongs to another world.
	return ITM_ARCHIPELAGO;
}

static std::string_view ItemName(int slot, int64_t item, int offset)
{
	auto name = ap->item_name(ap->slot_game_name(slot), item);
	if (slot == ap->player_id())
	{
		if (name == "Hat Upgrade")
		{
			name =
				(player.hat - offset) == 0 ? "Felt Hat" :
				(player.hat - offset) == 1 ? "Moon Hat" :
				(player.hat - offset) == 2 ? "Checkered Hat" :
				"Steel Hat";
		}
		else if (name == "Staff Upgrade")
		{
			name =
				(player.staff - offset) == 0 ? "Pine Staff" :
				(player.staff - offset) == 1 ? "Oak Staff" :
				(player.staff - offset) == 2 ? "Ironwood Staff" :
				"Mystical Staff";
		}
		else if (name == "Boots Upgrade")
		{
			name =
				(player.boots - offset) == 0 ? "Leather Boots" :
				(player.boots - offset) == 1 ? "Doc Merlins" :
				(player.boots - offset) == 2 ? "Lucky Boots" :
				"Air Lancelots";
		}
		else if (name == "Energy Barrage")
		{
			if (player.spell[0] > offset)
			{
				name = "Energy Storm";
			}
		}
		else if (name == "Dragon's Flame")
		{
			if (player.spell[1] > offset)
			{
				name = "Liquify";
			}
		}
		else if (name == "Seeker Bolt")
		{
			if (player.spell[2] > offset)
			{
				name = "Seeker Swarm";
			}
		}
		else if (name == "Ice Blast")
		{
			if (player.spell[3] > offset)
			{
				name = "Ice Beam";
			}
		}
		else if (name == "Inferno")
		{
			if (player.spell[4] > offset)
			{
				name = "Hyper Inferno";
			}
		}
		else if (name == "Summon Ptero")
		{
			if (player.spell[5] > offset)
			{
				name = "Summon Golem";
			}
		}
		else if (name == "Stoneskin")
		{
			if (player.spell[6] > offset)
			{
				name = "Steelskin";
			}
		}
		else if (name == "Berserk")
		{
			if (player.spell[7] > offset)
			{
				name = "Insane Rage";
			}
		}
		else if (name == "Healing")
		{
			if (player.spell[8] > offset)
			{
				name = "Life";
			}
		}
	}
	return name;
}

std::string Archipelago::ItemNameAtLocation(int chapter, int levelNum)
{
	int64_t location_id = BASE_ID + chapter * 50 + levelNum;
	if (auto item = ap->item_at_location(location_id))
	{
		std::string result { ItemName(item->player, item->item, 0) };
		if (item->player != ap->player_id())
		{
			result += " ^ ^ For: ";
			result += ap->slot_player_alias(item->player);
		}
		return result;
	}
	// Problem with the scouting.
	return "Unknown";
}

bool Archipelago::PickupItem(int chapter, int levelNum)
{
	return ap->check_location(BASE_ID + chapter * 50 + levelNum);
}

void Archipelago::PassLevel(int chapter, int levelNum)
{
	if (chapter == 3 && levelNum == 11)
	{
		ap->check_goal();
	}
	else
	{
		ap->check_location(BASE_ID + chapter * 50 + 20 + levelNum);
	}
	// Fast update in case we go right into a movie.
	ap->update();
}

bool Archipelago::LevelPassed(int chapter, int levelNum)
{
	return ap->location_is_checked(BASE_ID + chapter * 50 + 20 + levelNum);
}

int Archipelago::LevelsPassed(int chapter)
{
	return levelsPassed[chapter];
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
		ap->storage_get({
			ap->storage_private("xp"),
			ap->storage_private("money"),
			ap->storage_private("spell"),
			ap->storage_private("fairy"),
		});
		// Server will send ReceivedItems soon, so we need to reset progression.
		memset(chapters, 0, 4);
		memset(levelsPassed, 0, 4);
		memset(player.spell, 0, 9);
		player.hat = player.staff = player.boots = 0;
		player.level = 0;
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		int64_t item_id = item->item - BASE_ID;

		// Armageddon Sword pieces
		if (item_id == 0*50+5 && !player.keychain[0])
		{
			player.keychain[0] = 1;
		}
		else if (item_id == 1*50+4 && !player.keychain[1])
		{
			player.keychain[1] = 1;
		}
		else if (item_id == 2*50+7 && !player.keychain[2])
		{
			player.keychain[2] = 1;
		}
		else if (item_id == 3*50+8 && !player.keychain[3])
		{
			player.keychain[3] = 1;
		}
		if (player.keychain[0] && player.keychain[1] && player.keychain[2] && player.keychain[3] && !player.spell[9])
		{
			// Unlike vanilla, does not max out staff
			player.spell[9] = 1;
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
			if (player.hat < 255)
			{
				player.hat++;
				SetPlayerDefense();
			}
		}
		else if (item_id == 4 * 50 + 8)
		{
			if (player.staff < 255)
			{
				player.staff++;
				SetPlayerDamage();
			}
		}
		else if (item_id == 4 * 50 + 13)
		{
			if (player.boots < 255)
			{
				player.boots++;
				SetPlayerSpeed();
			}
		}
		// Other shop upgrades
		else if (item_id / 50 == 4)
		{
			BuyGear(item_id - 4 * 50);
		}
		// Chapter unlocks
		else if (item_id == 5 * 50 + 0)
		{
			chapters[0] = true;
		}
		else if (item_id == 0 * 50 + 20 + 14)
		{
			chapters[1] = true;
		}
		else if (item_id == 1 * 50 + 20 + 12)
		{
			chapters[2] = true;
		}
		else if (item_id == 2 * 50 + 20 + 15)
		{
			chapters[3] = true;
		}
		// Progressive level-ups
		else if (item_id / 50 == 5)
		{
			// Level cap increased from 50 to 99.
			// 51+ with amulets: HP & mana exceed bar.
			// 100+: level number starts displaying oddly.
			// 114+ without amulets, HP & mana exceed bar.
			// 114+ with amulets, HP & mana wrap to 0.
			// 242+ without amulets, HP & mana wrap to 0.
			if (player.level < 99)
			{
				player.level++;
				player.maxLife=14+player.level;
				if(player.gear&GEAR_HEART)
					player.maxLife*=2;
				player.maxMana=14+player.level;
				if(player.gear&GEAR_MOON)
					player.maxMana*=2;
			}
		}
		// Chapter progression
		else if (item_id == 0 * 50 + 37)
		{
			if (levelsPassed[0] < 255)
			{
				levelsPassed[0]++;
			}
		}
		else if (item_id == 1 * 50 + 36)
		{
			if (levelsPassed[1] < 255)
			{
				levelsPassed[1]++;
			}
		}
		else if (item_id == 2 * 50 + 40)
		{
			if (levelsPassed[2] < 255)
			{
				levelsPassed[2]++;
			}
		}
		else if (item_id == 3 * 50 + 35)
		{
			if (levelsPassed[3] < 255)
			{
				levelsPassed[3]++;
			}
		}
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
					int64_t item_id = message->item.item - BASE_ID;

					std::string text = "Got ";
					text += ItemName(message->receiving, message->item.item, 1);
					if (message->item.player != ap->player_id())
					{
						text += " from ";
						text += ap->slot_player_alias(message->item.player);
					}

					// Don't show Nothing or Chapter X Progress items.
					if (!(item_id == 0 * 50 + 37
						|| item_id == 1 * 50 + 36
						|| item_id == 2 * 50 + 40
						|| item_id == 3 * 50 + 35
						|| item_id == 3 * 50 + 28))
					{
						NewMessage(text.c_str(), MESSAGE_TIME);
						MakeNormalSound(SND_MESSAGE);
						messageCooldown = MESSAGE_TIME;
					}

					Guy *me = GetGoodguy();
					int x = me ? me->x : 0, y = me ? me->y : 0;
					// Armageddon sword pieces
					if (item_id == 0*50+5 || item_id == 1*50+4 || item_id == 2*50+7 || item_id == 3*50+8)
					{
						FloaterParticles(x,y,5,32,-1,16);
						FloaterParticles(x,y,5,10,2,16);
						FloaterParticles(x,y,5,64,-3,16);
						FloaterParticles(x,y,5,1,4,16);
						if (player.spell[9])
						{
							NewBigMessage("The sword is complete!",30);
							SendMessageToGame(MSG_SHOWANIM, 10);
						}
					}

					// Spellbooks
					byte spellbook = SpellBookForThisLevel(item_id);
					if (spellbook != 255)
					{
						if(player.spell[spellbook]==2)
						{
							NewBigMessage(spellName[spellbook*2+1],75);
						}
						else if(player.spell[spellbook]==1)
						{
							NewBigMessage(spellName[spellbook*2],75);
						}
					}

					// Level-ups
					if (item_id / 50 == 5)
					{
						// Particle is for getting enough XP, sound is for getting a level.
						MakeNormalSound(SND_LEVELUP);
					}
				}
				else if (message->item.player == ap->player_id())
				{
					// Sent [item] to [player].
					std::string text = "Sent ";
					text += ap->item_name(*message);
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
		else if (ap->is_storage_private(change->key, "spell"))
		{
			if (change->new_value->isLong())
			{
				spell = player.curSpell = change->new_value->getLong();
			}
		}
		else if (ap->is_storage_private(change->key, "fairy"))
		{
			if (change->new_value->isLong())
			{
				fairy = player.fairyOn = change->new_value->getLong();
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

	if (player.curSpell != spell)
	{
		spell = player.curSpell;
		ap->storage_set(ap->storage_private("spell"), player.curSpell, false);
	}

	if (player.fairyOn != fairy)
	{
		fairy = player.fairyOn;
		ap->storage_set(ap->storage_private("fairy"), player.fairyOn, false);
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
	std::string* typing = nullptr;

	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);

	// ------------------------------------------------------------------------
	// Init
	InitPlayer(INIT_GAME, 0, 0);
	InitMessage();

	while (running)
	{
		// --------------------------------------------------------------------
		// Update
		bool chaptersOk = ap && ap->is_active() && ap->error_message().empty();
		// Handle disconnects by bumping cursor back up.
		while (cursor >= 5 && cursor < 9 && (!chaptersOk || !chapters[cursor - 5]))
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
					cursor = 8;
				}
				else
				{
					--cursor;
				}
				while (cursor >= 5 && cursor < 9 && (!chaptersOk || !chapters[cursor - 5]))
				{
					--cursor;
				}
			}
			if (c & ~oldc & CONTROL_DN)
			{
				++cursor;
				while (cursor >= 5 && cursor < 9 && (!chaptersOk || !chapters[cursor - 5]))
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
				else if (cursor == 1)
				{
					typing = &serverAddress;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 2)
				{
					typing = &slotName;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 3)
				{
					typing = &password;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 4)
				{
					static ArchipelagoCache::FileSystem cache { "appdata/archipelago/" };
					ap = std::make_unique<ArchipelagoClient>("Kid Mystic", serverAddress, slotName, password);
					ap->use_cache(&cache);
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
			if (k == SDLK_ESCAPE || k == SDLK_RETURN) // ESC key
			{
				k = 0;
				typing = nullptr;
				oldc = ~0;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if (k == SDLK_BACKSPACE && !typing->empty())
			{
				typing->pop_back();
			}
			else if (k >= ' ' && k <= '~')
			{
				*typing += k;
			}

			typingY = typing ? GetStrLength(typing == &password ? std::string(password.size(), '*') : *typing, 1) : 0;
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
