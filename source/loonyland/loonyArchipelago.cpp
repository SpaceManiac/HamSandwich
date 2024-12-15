#include "loonyArchipelago.h"
#include "../../external/APCpp/Archipelago.h"
#include <unordered_map>
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <appdata.h>
#include <ioext.h>
#include "player.h"
#include <chrono>
#include <thread>


using json = nlohmann::json;

int loonyland_base_id = 2876900;
bool locationWait = false;
int player_id = 255;

void SendCheckedItem(int loc_id);
void GetLocationScouts(std::vector<AP_NetworkItem>);
void ItemsClear();
void ItemReceived(int64_t  item_id, bool notif);
void SetLocationChecked(int64_t  loc_id);
void DeathLinkReceived();
void GetInfoFromAP();
void SetupWorld();
void Disconnect();
std::string ConnectionStatus();



std::unordered_map<int, bool> locsFound;
std::unordered_map<int, int> itemsFound;
bool ArchipelagoMode = true;


void to_json(json& j, const locationData& loc) {
	j = json{
		{"Name", loc.Name},
		{"Type", loc.Type},
		{"ID", loc.ID},
		{"Map", loc.Map},
		{"MapID", loc.MapID},
		{"Xcoord", loc.Xcoord},
		{"Ycoord", loc.Ycoord},
		{"Spec1ID", loc.Spec1ID},
		{"Spec2ID", loc.Spec2ID}
	};
}

void from_json(const json& j, locationData& loc) {
	j.at("Name").get_to(loc.Name);
	j.at("Type").get_to(loc.Type);
	j.at("ID").get_to(loc.ID);
	j.at("Map").get_to(loc.Map);
	j.at("MapID").get_to(loc.MapID);
	j.at("Xcoord").get_to(loc.Xcoord);
	j.at("Ycoord").get_to(loc.Ycoord);
	j.at("Spec1ID").get_to(loc.Spec1ID);
	j.at("Spec2ID").get_to(loc.Spec2ID);
}

void DaveTest() {
	SetupWorld();
	
}

int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password) {
	AP_Init(IPAddress.c_str(), "Loonyland", SlotName.c_str(), Password.c_str());
	AP_SetItemClearCallback(ItemsClear);
	AP_SetItemRecvCallback(ItemReceived);
	AP_SetLocationCheckedCallback(SetLocationChecked);
	AP_SetDeathLinkSupported(false);
	AP_SetDeathLinkRecvCallback(DeathLinkReceived);
	AP_SetLocationInfoCallback(GetLocationScouts);

	SetupWorld();

	//GetDataPackage?

	AP_Start();
	return 0;
}

void ItemsClear()
{
	InitPlayer(INIT_ARCHIPELAGO, 0, 0);
	itemsFound.clear();
}
void ItemReceived(int64_t  item_id, bool notif)
{
	item_id -= loonyland_base_id;
	int count = ++itemsFound[item_id];
	if (item_frequencies.count(item_id) == 0)
	{
		PlayerSetVar(item_id, 1);
	}
	else
	{
		if (count <= item_frequencies.at(item_id))
		{
			PlayerSetVar(item_id+(count-1), 1);
		}
	}
	if ((item_id >= VAR_HEART && item_id <= VAR_PANTS)
		|| item_id == VAR_GEM
		|| item_id == VAR_TRIPLEFIRE
		|| item_id >= (VAR_KEY && item_id <= VAR_KEY+2))
	{
		PlayerCalcStats();
	}
}

void GetLocationScouts(std::vector<AP_NetworkItem> vec_NetworkItems)
{
	world_t world;
	LoadWorld(&world, "ap.llw");

	for (auto item : vec_NetworkItems)
	{
		locationData loc = basic_locations[item.location - loonyland_base_id];
		if (loc.Type == "Pickup") {
			Map* tempMap = world.map[loc.MapID];
			int item_id = item.item - loonyland_base_id;
			if (player_id == item.player)
			{
				item_id = basic_items.at(item_id);
			}
			else
			{
				item_id = ITM_ARCHIPELAGO;
			}
			tempMap->map[loc.Xcoord + loc.Ycoord * tempMap->width].item = item_id; //from var to placeable item
		}
	}

	SaveWorld(&world, "ap.llw");
	FreeWorld(&world);

	locationWait = false;
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
	std::cout << "AP ITEM MISS: " << mapName << " " << x << " " << y;

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
	std::cout << "AP QUEST MISS: " << questVar;
}

void SendCheckedItem(int loc_id)
{
	//loc_id -= loonyland_base_id;
	//can get items from quests or from pickups
	//going to overwrite item pickup code, and jump out of PlayerSetVar for quests
	locsFound[loc_id] = true;

	std::set<int64_t> locs;
	locs.insert(loc_id + loonyland_base_id);
	AP_SendItem(locs);
}

void SetLocationChecked(int64_t  loc_id)
{
	loc_id -= loonyland_base_id;
	locsFound[loc_id] = true;
	//mark ap at that location
}

void DeathLinkReceived()
{
	//we kill the loony
	player.hearts = 0;
}

void Disconnect()
{	
	AP_Stop(AP_ConnectionStatus::Disconnected);
}

std::string ConnectionStatus()
{
	AP_ConnectionStatus status = AP_GetConnectionStatus();
	switch (status) {
	case AP_ConnectionStatus::Disconnected:
		return "Disconnected";
		break;
	case AP_ConnectionStatus::Connected:
		return "Connected";
		break;
	case AP_ConnectionStatus::Authenticated:
		return "Authenticated";
			break;
	case AP_ConnectionStatus::ConnectionRefused:
		return "ConnectionRefused";
			break;
	case AP_ConnectionStatus::Failed:
		return "Failed"; break;
	default:
		return "Undefined Status";
	}
}


void GetInfoFromAP()
{
	player_id = AP_GetPlayerID();

	std::set<int64_t> locationScouts;
	for (auto loc : basic_locations)
	{
		if (loc.Name != "Q: Save Halloween Hill")
		{
			locationScouts.insert(loc.ID + loonyland_base_id);
		}
	}
	locationWait = true;
	AP_SendLocationScouts(locationScouts, false);
}

void SetupWorld()
{
	char buff[4096];
	auto baseWorld = AppdataOpen("loony.llw");
	auto newWorld = AppdataOpen_Write("ap.llw");

	while (int n = SDL_RWread(baseWorld, buff, 1, 4096))
	{
		SDL_RWwrite(newWorld, buff, 1, 4096);
	}

	newWorld.reset();
	baseWorld.reset();

	world_t world;
	LoadWorld(&world, "ap.llw");

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
	static int messageCooldown = 30 * 5;

	if (AP_IsMessagePending() && messageCooldown == 0)
	{
		AP_Message *message = AP_GetLatestMessage();
		if (message->type == AP_MessageType::ItemRecv)
		{
			std::string str_message = message->text;
			//NewBigMessage(message->text.c_str(), 30 * 5);
			str_message.replace(0, 8, "Got");
			NewMessage(str_message.c_str(), 30 * 5);
			messageCooldown = 30 * 5;
		}
		if (message->type == AP_MessageType::ItemSend)
		{
			//NewBigMessage(message->text.c_str(), 30 * 5);
			NewMessage(message->text.c_str(), 30 * 5);
			messageCooldown = 30 * 5;
		}

		AP_ClearLatestMessage();
	}

	if (messageCooldown > 0)
	{
		messageCooldown--;
	}

}

