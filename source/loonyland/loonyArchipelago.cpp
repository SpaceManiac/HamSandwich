#include "loonyArchipelago.h"
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <appdata.h>
#include <ioext.h>
#include "player.h"
#include <chrono>
#include <thread>
#include <queue>
#include <aegea.h>
#include <json.h>
#include "quest.h"

static std::unique_ptr<ArchipelagoClient> ap;

constexpr int loonyland_base_id = 2876900;

bool locationWait = false;
std::string ArchipelagoSeed = "";

void SendCheckedItem(int loc_id);
void ItemsClear();
void ItemReceived(int64_t  item_id, bool notif);
void SetLocationChecked(int64_t  loc_id);
void DeathLinkReceived();
void SetupWorld();
void Disconnect();
void GetArchipelagoPlayerVar(int var);
void GivePlayerItem(int64_t item_id, bool loud);
std::string ConnectionStatus();


std::unordered_map<int, bool> locsFound;
std::unordered_map<int, int> itemsFound;
bool ArchipelagoMode = true;
bool ExpectingDeath = false;


std::unordered_map<int, chatData> chat_table = {
	{2, { "Super Heart", ""}}, //trees
	{8, { "booties", ""}}, //biff
	{9, { "boots", ""}}, //biff2
	{10, { "boots", ""}}, //biff3
	{14, { "cactus", ""}}, //bonita
	{15, { "cactus", ""}}, //bonita2
	{26, { "stick", ""}}, //cat lady
	{27, { "stick", ""}}, //cat lady
	{29, { "potion", ""}}, //zizwalda
	{31, { "Super Heart", ""}}, //zizwalda2
	{42, { "super growth", ""}}, //farmer
	{43, { "fertilizer", ""}}, //farmer2
	{56, { "100 Gem", ""}}, //mayor
	{60, { "Ghost Slaying Potion", ""}}, //zizwalda potion
	{61, { "Ghost Slaying Potion", ""}}, //zizwalda potion
	{62, { "Ghost Slaying Potion", ""}}, //zizwalda potion
	{68, { "Bat Key", ""}}, //doofy little twerp
	{70, { "slingshot", ""}}, //silver man
	{75, { "slingshot", ""}}, //silver man2
	{79, { "lantern", ""}}, //lost girl
	{85, { "key", ""}}, //larry wife
	{92, { "toy", ""}}, //collection
	{96, { "Reflect Gem", ""}}, //collection
	{130, { "crystal ball", ""}}, //seer
	{164, { "ghost potion", ""}}, //zizwalda
	{167, { "muck walking boots", ""}}, //boots guy
	{191, { "Super Heart", ""}} //zizwalda
	//{192, { "Hot Pants", ""}}, //remix hot pants guy
	//{193, { "Hot Pants", ""}}, //remix hot pants guy
	//{194, { "hot pants", ""}}, //remix hot pants guy
	//{198, { "Vampire Bust", ""}}, //arms dealer
};


int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password) {
	ap = std::make_unique<ArchipelagoClient>("Loonyland", IPAddress, SlotName, Password);
	//AP_SetLocationCheckedCallback(SetLocationChecked);
	ap->death_link_enable();

	SetupWorld();

	ArchipelagoSeed = "";
	return 0;
}

void ItemsClear()
{
	//InitPlayer(INIT_ARCHIPELAGO, 0, 0);
	itemsFound.clear();
}
void ItemReceived(int64_t  item_id, bool notif)
{
	item_id -= loonyland_base_id;
	++itemsFound[item_id];
	GivePlayerItem(item_id, true);
}

void ArchipelagoLoadPlayer()
{
	for (const auto& [item_id, count] : itemsFound)
	{
		GivePlayerItem(item_id, false);
	}
}

void GivePlayerItem(int64_t item_id, bool loud)
{
	int count = itemsFound[item_id];
	if (item_frequencies.count(item_id) == 0)
	{
		player.var[item_id] = 1;
		//PlayerSetVar(item_id, 1);
	}
	else
	{
		if (count <= item_frequencies.at(item_id))
		{
			player.var[item_id + (count - 1)] = 1;
		}
	}

	if (item_id >= VAR_MUSHROOM && item_id < VAR_MUSHROOM + 10) //update quest
	{
		if (ShroomCount() == 10)
		{
			PlayerSetVar(VAR_QUESTDONE + QUEST_SHROOM, 1);
		}
	}
	if (item_id == VAR_CAT)
	{
		PlayerSetVar(item_id + (count - 1), 1);
	}
	if ((item_id >= VAR_HEART && item_id <= VAR_PANTS)
		|| item_id == VAR_GEM
		|| item_id == VAR_REFLECT
		|| item_id == VAR_TRIPLEFIRE
		|| (item_id >= VAR_KEY && item_id <= VAR_KEY + 2))
	{
		PlayerCalcStats();
	}

	if (item_id >= VAR_HEART && item_id < VAR_HEART+20)
	{
		if (goodguy != NULL)
		{
			if (player.maxHearts < player.maxMaxHearts)
			{
				player.maxHearts++;
			}
			player.hearts = player.maxHearts;
			goodguy->hp = player.hearts;
		}
	}
	if (item_id >= VAR_GEM && item_id < VAR_GEM + 6)
	{
		if (player.maxMoney < MAX_MONEY)
		{
			player.maxMoney += 25;
			player.money += 25;
			player.gemsGotten += 25;
		}
		else
		{
			if (player.money < player.maxMoney)
			{
				player.money += 50;
				if (player.money > player.maxMoney)
				{
					player.money = player.maxMoney;
				}
			}
			player.gemsGotten += 50;
		}
	}
}

void GetLocationScouts()
{
	world_t world;
	LoadWorld(&world, "ap.llw");

	bool allGood = true;

	for (auto loc : basic_locations)
	{
		if (loc.Name == "Q: Save Halloween Hill")
		{
			continue;
		}

		const ArchipelagoClient::Item* item = ap->item_at_location(loc.ID + loonyland_base_id);
		if (item == nullptr)
		{
			allGood = false;
			continue;
		}

		if (loc.Type == "Pickup")
		{
			Map* tempMap = world.map[loc.MapID];
			int item_id = item->item - loonyland_base_id;
			if (item->player == ap->player_id())
			{
				item_id = basic_items.at(item_id);
			}
			else
			{
				item_id = ITM_ARCHIPELAGO;
			}
			tempMap->map[loc.Xcoord + loc.Ycoord * tempMap->width].item = item_id; //from var to placeable item
		}
		for (auto const& i : loc.chatCodes)
		{
			chat_table[i].updated += ap->item_name(item->item);
		}
	}

	if (allGood)
	{
		locationWait = false;
		SaveWorld(&world, "ap.llw");
	}

	FreeWorld(&world);
}

void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y)
{
	for (locationData loc : basic_locations)
	{
		if (loc.MapID == mapNum && loc.Map == mapName && loc.Xcoord == x && loc.Ycoord == y)
		{
			SendCheckedItem(loc.ID);
			return;
		}
	}
	std::cout << "AP ITEM MISS: " << mapName << " " << x << " " << y << std::endl;

}
void SendCheckedLocQuest(int questVar)
{
	for (locationData loc : basic_locations)
	{
		if (loc.Type == "Quest" && loc.MapID == questVar) //map ids are where the quest var is stored for those locations
		{
			SendCheckedItem(loc.ID);
			return;
		}
	}
	std::cout << "AP QUEST MISS: " << questVar << std::endl;
}

void SendCheckedTalkReward(int talkVar)
{

}

void SendCheckedItem(int loc_id)
{
	//loc_id -= loonyland_base_id;
	//can get items from quests or from pickups
	//going to overwrite item pickup code, and jump out of PlayerSetVar for quests
	locsFound[loc_id] = true;

	ap->check_location(loc_id + loonyland_base_id);
}

void SetLocationChecked(int64_t  loc_id)
{
	loc_id -= loonyland_base_id;
	if (loc_id > 93)
	{
		player.var[basic_locations[loc_id].MapID + VAR_QUESTASSIGN] = 1;
		player.var[basic_locations[loc_id].MapID + VAR_QUESTDONE] = 1;
	}
	locsFound[loc_id] = true;

	//mark ap at that location
}

void WinArchipelago()
{
	ap->check_goal();
}

void DeathLinkReceived()
{
	//we kill the loony
	ExpectingDeath = true;
	player.hearts = 0;
	if (goodguy != NULL)
	{
		goodguy->hp = 0;
		goodguy->ouch = 4;
	}
}

void SendDeathLink()
{
	if (!ExpectingDeath)
	{
		ap->death_link_send();
	}
	ExpectingDeath = false;
}

void Disconnect()
{
	ap.reset();
}

void SendArchipelagoPlayerVar(int v, int val)
{
	ap->storage_set(ap->storage_private("PLAYER_VAR_") += std::to_string(v), val, false);
}

void GetArchipelagoPlayerVars()
{
	for (int var : DataStorageVars)
	{
		GetArchipelagoPlayerVar(var);
	}
}

void GetArchipelagoPlayerVar(int var)
{
	ap->storage_get({ ap->storage_private("PLAYER_VAR_") += std::to_string(var) });
}

std::string ConnectionStatus()
{
	return !ap ? std::string("Not connected") :
		!ap->error_message().empty() ? std::string(ap->error_message()) :
		std::string(ap->connection_status());
}


void GetInfoFromAP()
{
	std::vector<int64_t> locationScouts;
	for (auto loc : basic_locations)
	{
		if (loc.Name != "Q: Save Halloween Hill")
		{
			locationScouts.push_back(loc.ID + loonyland_base_id);
		}
	}
	locationWait = true;
	ap->scout_locations(locationScouts);

	GetArchipelagoPlayerVars();
}

void SetupWorld()
{
	world_t world;
	LoadWorld(&world, "loony.llw");

	//add some hearts to loonyton
	world.map[0]->map[91 + 90 * world.map[0]->width].item = 2;
	world.map[0]->map[92 + 90 * world.map[0]->width].item = 2;

	world.map[23]->special[5].trigger = 0;

	//DELETE ALL THE ITEM SPECIALS
	for (auto loc : basic_locations)
	{
		if (loc.Type == "Pickup")
		{
			world.map[loc.MapID]->special[loc.Spec1ID].trigger = 0;
			world.map[loc.MapID]->special[loc.Spec2ID].trigger = 0;
		}
	}

	SaveWorld(&world, "ap.llw");
	FreeWorld(&world);
}

void UpdateArchipelago()
{
	if (!ap)
	{
		return;
	}

	constexpr int MESSAGE_TIME = 30 * 5;
	static int messageCooldown = MESSAGE_TIME;

	ap->update();

	// Handle reconnection by refreshing stuff.
	if (ap->pop_connected())
	{
		ItemsClear();
		GetInfoFromAP();
		ArchipelagoSeed = ap->seed_name();
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		ItemReceived(item->item, true);
	}

	// Location checks
	// TODO: track already-checked locations better
	for (int64_t loc : ap->checked_locations())
	{
		if (!locsFound[loc])
		{
			SetLocationChecked(loc);
		}
	}

	// Location scouts
	if (locationWait)
	{
		GetLocationScouts();
	}

	// DeathLink
	if (ap->pop_death_link())
	{
		DeathLinkReceived();
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
				text += ap->slot_player_alias(message->slot);
				NewMessage(text.c_str(), MESSAGE_TIME);
				messageCooldown = MESSAGE_TIME;
			}
			else if (message->slot == ap->player_id())
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
	}

	// Storage
	std::string prefix = ap->storage_private("PLAYER_VAR_");
	for (auto [key, _] : ap->storage_changes)
	{
		auto value = ap->storage(key);
		if (value.isLong() && key.size() > prefix.size() && std::string_view(key).substr(0, prefix.size()) == prefix)
		{
			int var = std::stoi(key.substr(prefix.size()));
			player.var[var] = value.getLong();
		}
	}
	ap->storage_changes.clear();
}

void DebugAPCommand() {
	std::cout << "Do stuff";


}
