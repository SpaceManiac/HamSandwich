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
#include "options.h"
#include "badge.h"

static std::unique_ptr<ArchipelagoClient> ap;

constexpr int loonyland_base_id = 2876900;

bool locationWait = false;
std::string ArchipelagoSeed = "";
int ArchipelagoSlotNum = -1;

void SendCheckedItem(int loc_id);
void ItemsClear();
void ItemReceived(int64_t  item_id, bool notif);
void SetLocationChecked(int64_t  loc_id);
void DeathLinkReceived();
void SetupWorld();
void Disconnect();
void GivePlayerItem(int64_t item_id, bool loud);


std::unordered_map<int, bool> locsFound;
std::unordered_map<int, int> itemsFound;
std::map<std::string, int> scouted_items;
bool ArchipelagoMode = false;
bool ExpectingDeath = false;
byte ap_cheatsAvail[40] = { 0 };
byte ap_modesAvail[6] = { 0 };
SlotData apSlotData = { 0 };
std::set<locationData*> in_logic_locs;

byte cheat_to_badge[NUM_BADGES] = {
	BADGE_DARKNESS, BADGE_BOWLING, BADGE_HUNDREDPCT, BADGE_LOONYBALL, BADGE_EVILIZER, BADGE_GEMS,
	BADGE_MONSTERPTS2, BADGE_POLTERGUY, BADGE_BESTRANK, BADGE_GRAVES, BADGE_BONKULA, BADGE_ANNOY,
	BADGE_WINGAME, BADGE_COMBO, BADGE_ROCKS, BADGE_HIDDEN, BADGE_TERRORSRV, BADGE_KILLALL,
	BADGE_SNEAK, BADGE_MONSTERPTS, BADGE_SURVIVAL, BADGE_LARRY, BADGE_FRANKENJ, BADGE_BOSSBASH,
	BADGE_CATTAILS, BADGE_WITCH, BADGE_WOLFDEN, BADGE_RESCUE, BADGE_TERROR, BADGE_SURVCOMBO,
	BADGE_REMIX, BADGE_WITCHCRAFT, BADGE_THIEF, BADGE_SURV100, BADGE_RIOT, BADGE_SWAMPDOG,
	BADGE_SCORE, BADGE_BRAWL, BADGE_HIDDEN2, BADGE_MASTER,
};

std::unordered_map<int, chatData> chat_table = {
	{2, { "Super Heart", ""}}, //trees
	{8, { "booties", "", true}}, //biff
	{9, { "boots", ""}}, //biff2
	{10, { "boots", ""}}, //biff3
	{14, { "cactus", "", true}}, //bonita
	{15, { "cactus", ""}}, //bonita2
	{26, { "stick", "",}}, //cat lady
	{27, { "stick", "",}}, //cat lady
	{29, { "potion", "", true}}, //zizwalda
	{31, { "Super Heart", ""}}, //zizwalda2
	{42, { "super growth", ""}}, //farmer
	{43, { "fertilizer", ""}}, //farmer2
	{56, { "100 Gem", ""}}, //mayor
	{60, { "Ghost Slaying Potion", "", true}}, //zizwalda potion
	{61, { "Ghost Slaying Potion", ""}}, //zizwalda potion
	{62, { "Ghost Slaying Potion", ""}}, //zizwalda potion
	{68, { "Bat Key", ""}}, //doofy little twerp
	{70, { "slingshot", "", true}}, //silver man
	{75, { "slingshot", ""}}, //silver man2
	{79, { "lantern", ""}}, //lost girl
	{85, { "key", ""}}, //larry wife
	{92, { "toy", "", true}}, //collection
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

void ArchipelagoDisconnect()
{
	ArchipelagoMode = false;
	ap.reset();
}

int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password)
{
	static ArchipelagoCache::FileSystem cache { "appdata/archipelago/" };
	ap = std::make_unique<ArchipelagoClient>("Loonyland", IPAddress, SlotName, Password);
	ap->use_cache(&cache);
	//AP_SetLocationCheckedCallback(SetLocationChecked);


	ArchipelagoMode = true;
	ArchipelagoSeed = "";
	ArchipelagoSlotNum = -1;
	return 0;
}

void ItemsClear()
{
	itemsFound.clear();
}
void ItemReceived(int64_t  item_id, bool notif)
{
	item_id -= loonyland_base_id;
	++itemsFound[item_id];
	//if (item_id != 3000) {
		GivePlayerItem(item_id, true);
	//}
}

void ArchipelagoLoadPlayer()
{
	player.maxMoney = 50;
	for (const auto& [item_id, count] : itemsFound)
	{
		GivePlayerItem(item_id, false);
	}
	for (const auto& [loc_id, data] : locsFound)
	{
		SetLocationChecked(loc_id);
	}
}

void GivePlayerItem(int64_t item_id, bool loud)
{
	if (item_id == AP_FILLERMOD && loud && goodguy != NULL)
	{
		NewBigMessage("MAX LIFE & GEMS!", 90);
		player.hearts = player.maxHearts;
		player.money = player.maxMoney;
		goodguy->hp = player.hearts;
		MakeNormalSound(SND_POWERUP);
		return;
	}
	if (item_id >= MODE_SURVIVAL + AP_MODEMOD && item_id <= MODE_REMIX + AP_MODEMOD)
	{
		ap_modesAvail[item_id - AP_MODEMOD] = 1;
	}
	else if (item_id >= CH_LIGHT+ AP_BADGEMOD && item_id <= CH_NOFARLEY + AP_BADGEMOD)
	{
		ap_cheatsAvail[cheat_to_badge[item_id - AP_BADGEMOD]] = 1;
	}
	else
	{
		int count = 0;
		if (itemsFound.count(item_id) > 0)
		{
			count = itemsFound[item_id];
		}
		if (item_frequencies.count(item_id) == 0)
		{
			player.var[item_id] = 1;
		}
		else
		{
			for (int i = 0; i < count; i++)
			{
				if (i + 1 <= item_frequencies.at(item_id))
				{
					player.var[item_id + (i)] = 1;
				}
			}
		}
	}
	if (item_id == VAR_ZOMBIEGEM)
	{
		if (player.money < player.maxMoney)
		{
			player.money += 100;
			if (player.money > player.maxMoney)
			{
				player.money = player.maxMoney;
			}
		}
	}
	player.gemsGotten += 100;
	if (item_id == VAR_DAISY)
	{
		player.var[VAR_QUESTASSIGN + QUEST_DAISY] = 1;
		player.var[VAR_QUESTDONE + QUEST_DAISY] = 1;
		SendCheckedLocQuest(QUEST_DAISY);
	}

	if (item_id >= VAR_MUSHROOM && item_id < VAR_MUSHROOM + 10) //update quest
	{
		if (ShroomCount() == 10)
		{
			player.var[VAR_QUESTASSIGN + QUEST_SHROOM] = 1;
			player.var[VAR_QUESTDONE + QUEST_SHROOM] = 1;
			SendCheckedLocQuest(QUEST_SHROOM);
		}
	}
	if (item_id == VAR_CAT)
	{
		player.var[VAR_QUESTASSIGN + QUEST_CAT] = 1;
	}

	if (item_id >= VAR_GEM && item_id < VAR_GEM + 6)
	{
		if (goodguy != NULL && player.maxMoney < MAX_MONEY)
		{
			player.maxMoney += 25;
			if (loud)
			{
				player.money += 25;
				player.gemsGotten += 25;
			}
		}
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
		if (goodguy != NULL && loud)
		{
			player.hearts = player.maxHearts;
			goodguy->hp = player.hearts;
		}
	}
	if (loud && goodguy != NULL)
	{
		MakeNormalSound(basic_items.at(item_id).sound_ID);
	}
}

void GetLocationScouts()
{
	world_t world;
	LoadWorld(&world, ("Archipelago/" + ArchipelagoSeed + "_" + std::to_string(ArchipelagoSlotNum) + "/ap.llw").c_str());

	bool allGood = true;

	for (auto loc : in_logic_locs)
	{
		//store in location data
		if (loc->Name == "Q: Save Halloween Hill")
		{
			continue;
		}

		const ArchipelagoClient::ScoutedItem* item = ap->item_at_location(loc->ID + loonyland_base_id);

		if (item == nullptr)
		{
			allGood = false;
			continue;
		}
		if (item->player == ap->player_id())
		{
			scouted_items[loc->Name] = item->item - loonyland_base_id;
		}
		else
		{
			scouted_items[loc->Name] = ITM_ARCHIPELAGO;
		}

		if (loc->Type == "Pickup" || (loc->Type == "Badge" && loc->MapID == 20))
		{

			Map* tempMap;
			if (loc->Type == "Badge" && loc->MapID == 20)
			{
				tempMap = world.map[26];
			}
			else
			{
				tempMap = world.map[loc->MapID];
			}
			int item_id = item->item - loonyland_base_id;
			int game_id = 0;
			if (item_id != 3000)
			{
				if (item->player == ap->player_id())
				{
					game_id = basic_items.at(item_id).ingame_ID;
				}
				else
				{
					game_id = ITM_ARCHIPELAGO;
				}
				tempMap->map[loc->Xcoord + loc->Ycoord * tempMap->width].item = game_id; //from var to placeable item
				if (game_id == ITM_BADGE)
				{
					tempMap->map[loc->Xcoord + loc->Ycoord * tempMap->width].tag = cheat_to_badge[item_id - AP_BADGEMOD] + 1;
				}
				if (game_id == ITM_KEY)
				{
					int tag = 0;
					switch (item_id)
					{
					case MODE_BOWLING + AP_MODEMOD:
						tag = 5;
						break;
					case MODE_SURVIVAL + AP_MODEMOD:
						tag = 2;
						break;
					case MODE_BOSSBASH + AP_MODEMOD:
						tag = 7;
						break;
					case MODE_LOONYBALL + AP_MODEMOD:
						tag = 4;
						break;
					case MODE_REMIX + AP_MODEMOD:
						tag = 8;
						break;
					}
					tempMap->map[loc->Xcoord + loc->Ycoord * tempMap->width].tag = tag;
				}
			}
			else
				tempMap->map[loc->Xcoord + loc->Ycoord * tempMap->width].item = ITM_ARCHIPELAGO; //from var to placeable item
		}
		for (auto const& i : loc->chatCodes)
		{
			chat_table[i].updated += ap->item_name(*item);
			chat_table[i].location_id = loc->ID;
		}
	}

	if (allGood)
	{
		locationWait = false;
		SaveWorld(&world, ("Archipelago/" + ArchipelagoSeed + "_" + std::to_string(ArchipelagoSlotNum) + "/ap.llw").c_str());

	}
	FreeWorld(&world);
}

void SendLocationScout(int locId, bool createHint)
{

	ap->scout_locations({ locId+loonyland_base_id }, createHint);

}

void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y, int item_id)
{

	if (mapName == "The Wolf Den" && x == 35 && y == 26)
	{
		EarnBadge(BADGE_HIDDEN);
		return;
	}
	if (mapName == "Abandoned Mines" && x == 14 && y == 18)
	{
		EarnBadge(BADGE_REMIX);
		return;
	}
	if (mapName == "The Witch's Cabin" && x == 27 && y == 16)
	{
		EarnBadge(BADGE_DARKNESS);
	}
	//skip survival for now
	if (mapName == "Survival!")
	{
		return;
	}
	for (locationData* loc : in_logic_locs)
	{
		if (loc->MapID == mapNum && loc->Map == mapName && loc->Xcoord == x && loc->Ycoord == y)
		{
			SendCheckedItem(loc->ID);
			return;
		}
	}
	std::cout << "AP ITEM MISS: " << mapName << " " << x << " " << y << std::endl;
	//give player the raw item
	if (item_id == ITM_KEY)
	{
		if (player.keys[0] < 9)
			player.keys[0]++;
		MakeNormalSound(SND_KEYGET);
		return;
	}
	for (auto item_data : basic_items)
	{
		if (item_data.second.ingame_ID == item_id)
		{
			GivePlayerItem(item_data.first, true);
			return;
		}
	}



}
void SendCheckedLocQuest(int questVar)
{
	for (locationData* loc : in_logic_locs)
	{
		if (loc->Type == "Quest" && loc->MapID == questVar) //map ids are where the quest var is stored for those locations
		{
			SendCheckedItem(loc->ID);
			return;
		}
	}
	std::cout << "AP QUEST MISS: " << questVar << std::endl;
}

void SendCheckedLocBadge(int badgeID)
{
	for (locationData* loc : in_logic_locs)
	{
		if (loc->Type == "Badge" && loc->MapID == badgeID) //map ids are where the quest var is stored for those locations
		{
			SendCheckedItem(loc->ID);
			return;
		}
	}
	std::cout << "AP BADGE MISS: " << (int16_t)badgeID << std::endl;
	//EarnBadge(badgeID);
}

void SendCheckedLocReward(int rewardID)
{
	for (locationData* loc : in_logic_locs)
	{
		if (loc->Type == "Reward" && loc->MapID == rewardID) //map ids are where the quest var is stored for those locations
		{
			SendCheckedItem(loc->ID);
			return;
		}
	}
	std::cout << "AP REWARD MISS: " << rewardID << std::endl;
}

void SendCheckedLocDoll(int dollId)
{
	for (locationData* loc : in_logic_locs)
	{
		if (loc->Name == "Bat Drop")
		{
			SendCheckedItem(loc->ID + dollId);
			return;
		}
	}
	std::cout << "AP DOLL MISS: " << dollId << std::endl;
	NewBigMessage("Got a Monster Doll!", 90);
	GivePlayerItem(VAR_BATDOLL + dollId, true);
}


void SendCheckedItem(int loc_id)
{
	//loc_id -= loonyland_base_id;
	//can get items from quests or from pickups
	//going to overwrite item pickup code, and jump out of PlayerSetVar for quests
	locsFound[loc_id] = true;
	if (ap)
	{
		ap->check_location(loc_id + loonyland_base_id);
	}
}

void SetLocationChecked(int64_t  loc_id)
{
	if (basic_locations[loc_id].Type == "Quest")
	{
		player.var[basic_locations[loc_id].MapID + VAR_QUESTASSIGN] = 1;
		player.var[basic_locations[loc_id].MapID + VAR_QUESTDONE] = 1;
	}
	else if (basic_locations[loc_id].Type == "Badge")
	{
		opt.meritBadge[basic_locations[loc_id].MapID] = MERIT_EARNED;
	}
	else if (basic_locations[loc_id].Type == "Reward")
	{
		player.var[basic_locations[loc_id].MapID] = 1;
	}
	else if (basic_locations[loc_id].Type == "Drop")
	{
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
	if (apSlotData.deathlink)
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
}

void SendDeathLink()
{
	if (apSlotData.deathlink)
	{
		if (!ExpectingDeath)
		{
			ap->death_link_send();
		}
		ExpectingDeath = false;
	}
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
	std::vector<std::string> strings;
	std::vector<std::string_view> views;
	for (int var : DataStorageVars)
	{
		//std::string& string = strings.emplace_back(ap->storage_private("PLAYER_VAR_"));
		//string += std::to_string(var);
		//views.push_back(string);
	}
	ap->storage_get(views);
}

std::string_view ConnectionStatus()
{
	return
		!ap ? "Not connected" :
		!ap->error_message().empty() ? ap->error_message() :
		!ap->is_active() ? ap->connection_status() :
		locationWait ? "Scouting locations..." :
		"Active";
}

void GetInfoFromAP()
{
	std::vector<int64_t> locationScouts;
	for (auto &loc : basic_locations)
	{
		if (loc.Name == "Q: Save Halloween Hill" && apSlotData.win_condition == AP_WIN_EVILIZER)
		{
			continue;
		}
		if (apSlotData.badges != AP_OP_FULL && loc.Type == "Badge")
		{
			continue;
		}
		if (apSlotData.dolls != AP_OP_FULL && (loc.Type == "Doll" || loc.flags.count("DOLL") > 0))
		{
			continue;
		}
		if (apSlotData.long_checks == AP_OP_DISABLED && loc.flags.count("LONG"))
		{
			continue;
		}
		if (apSlotData.long_checks == AP_OP_DISABLED && apSlotData.badges == AP_OP_VANILLA
			&& loc.flags.count("LONG_VANILLA"))
		{
			continue;
		}
		if (apSlotData.remix == AP_OP_DISABLED && loc.flags.count("REMIX"))
		{
			continue;
		}
		if (apSlotData.multi_save == AP_OP_DISABLED && loc.flags.count("MULTISAVE"))
		{
			continue;
		}
		if (apSlotData.win_condition == AP_WIN_EVILIZER && (loc.flags.count("POSTGAME") || (loc.flags.count("VANILLA_POSTGAME") && apSlotData.badges == AP_OP_VANILLA)))
		{
			continue;
		}
		if (apSlotData.overpowered_cheats == AP_OP_DISABLED && loc.flags.count("OP"))
		{
			continue;
		}

		locationScouts.push_back(loc.ID + loonyland_base_id);
		in_logic_locs.insert(&loc);
		if (loc.Type == "BADGE") {
			opt.meritBadge[loc.MapID] = MERIT_NO;
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

	world.map[0]->map[83 + 146 * world.map[0]->width].item = 0; //remove tree blocking vision of small items in terror glade

	ham_sprintf(world.map[18]->name, "Haunted Tower Floor 2");
	ham_sprintf(world.map[19]->name, "Haunted Tower Floor 3");

	world.map[23]->special[5].trigger = 0;

	world.map[46]->special[4].trigger = TRG_TIMED;
	world.map[46]->special[4].trigValue = 0;

	//DELETE ALL THE ITEM SPECIALS
	for (auto loc : basic_locations)
	{
		if (loc.Type == "Pickup")
		{
			world.map[loc.MapID]->special[loc.Spec1ID].trigger = 0;
			world.map[loc.MapID]->special[loc.Spec2ID].trigger = 0;
		}
	}

	SaveWorld(&world, ("Archipelago/" + ArchipelagoSeed + "_" + std::to_string(ArchipelagoSlotNum) + "/ap.llw").c_str());
	FreeWorld(&world);
}

void UpdateArchipelago()
{
	static bool gottenAnything = false;
	bool reconnected = false;
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
		GetRoomInfo();
		GetInfoFromAP();
		ArchipelagoSeed = ap->seed_name();
		ArchipelagoSlotNum = ap->player_id();
		SetupWorld();
		ap->death_link_enable(apSlotData.deathlink);
		reconnected = true;

		/*for (int i = 0; i < NUM_BADGES; i++)
		{
			opt.cheats[i] &= ap_cheatsAvail[cheat_to_badge[i]];
		}*/
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		ItemReceived(item->item, true);
		gottenAnything = true;
	}
	if (reconnected)
	{
		for (int i = 0; i < NUM_BADGES; i++)
		{
			opt.cheats[i] &= ap_cheatsAvail[cheat_to_badge[i]];
		}
	}

	// Location checks
	// TODO: track already-checked locations better
	for (int64_t loc : ap->checked_locations())
	{
		loc -= loonyland_base_id;
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
				text += ap->item_name(*message);
				text += " from ";
				text += ap->slot_player_alias(message->item.player);
				NewMessage(text.c_str(), MESSAGE_TIME);
				messageCooldown = MESSAGE_TIME;
			}
			else if (message->item.player == ap->player_id())
			{
				// Sent [item] to [player].
				std::string text = "Sent ";
				text += ap->item_name(*message);
				text += " to ";
				text += ap->slot_player_alias(message->receiving);
				NewMessage(text.c_str(), MESSAGE_TIME);
				messageCooldown = MESSAGE_TIME;
			}
		}
	}

	// Storage
	std::string prefix = ap->storage_private("PLAYER_VAR_");
	while (auto change = ap->pop_storage_change())
	{
		auto [key, value, _] = *change;
		auto var = ap->starts_with_storage_private(key, "PLAYER_VAR_");
		if (value->isLong() && !var.empty())
		{
			player.var[std::stoi(std::string(var))] = value->getLong();
		}
	}
}

void DebugAPCommand() {
	std::cout << "Do stuff";
}

void GetRoomInfo() {
	apSlotData.win_condition = ap->room_info("slot_data")["WinCondition"].getNumber();
	apSlotData.badges_required = ap->room_info("slot_data")["BadgesRequired"].getNumber();
	apSlotData.difficulty = ap->room_info("slot_data")["Difficulty"].getNumber();
	apSlotData.long_checks = ap->room_info("slot_data")["LongChecks"].getNumber();
	apSlotData.multi_save = ap->room_info("slot_data")["MultipleSaves"].getNumber();
	apSlotData.remix = ap->room_info("slot_data")["Remix"].getNumber();
	apSlotData.overpowered_cheats = ap->room_info("slot_data")["OverpoweredCheats"].getNumber();
	apSlotData.badges = ap->room_info("slot_data")["Badges"].getNumber();
	apSlotData.dolls = ap->room_info("slot_data")["Dolls"].getNumber();
	apSlotData.deathlink = ap->room_info("slot_data")["DeathLink"].getNumber();

	opt.difficulty = apSlotData.difficulty;

	//if dolls none, update scout text, and make it not hint for the collection
}
