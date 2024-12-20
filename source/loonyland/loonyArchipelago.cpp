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
#include <queue>
#include "quest.h"


const int MAX_RETRIES = 5;

using json = nlohmann::json;

int loonyland_base_id = 2876900;
bool locationWait = false;
bool varsWait = false;
int player_id = 255;
std::string ArchipelagoSeed = "";

void SendCheckedItem(int loc_id);
void GetLocationScouts(std::vector<AP_NetworkItem>);
void ItemsClear();
void ItemReceived(int64_t  item_id, bool notif);
void SetLocationChecked(int64_t  loc_id);
void DeathLinkReceived();
void GetInfoFromAP();
void SetupWorld();
void Disconnect();
void GetArchipelagoPlayerVar(int var);
void GivePlayerItem(int64_t item_id, bool loud);
std::string ConnectionStatus();

std::deque<AP_GetServerDataRequest*> GetVarRequests;


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
	AP_Init(IPAddress.c_str(), "Loonyland", SlotName.c_str(), Password.c_str());
	AP_SetItemClearCallback(ItemsClear);
	AP_SetItemRecvCallback(ItemReceived);
	AP_SetLocationCheckedCallback(SetLocationChecked);
	AP_SetDeathLinkSupported(true);
	AP_SetDeathLinkRecvCallback(DeathLinkReceived);
	AP_SetLocationInfoCallback(GetLocationScouts);


	SetupWorld();

	//GetDataPackage?

	AP_Start();

	AP_RoomInfo info;
	while (AP_GetRoomInfo(&info)) {}
	ArchipelagoSeed = info.seed_name;
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
		for (auto const& i : loc.chatCodes) {
			chat_table[i].updated += item.itemName;
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

void SendCheckedTalkReward(int talkVar)
{

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
	AP_StoryComplete();
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
		AP_DeathLinkSend();
	}
	ExpectingDeath = false;
}

void Disconnect()
{

	AP_Shutdown();
}

void SendArchipelagoPlayerVar(int v, int val)
{
	AP_SetServerDataRequest setRequest;
	setRequest.key = AP_GetPrivateServerDataPrefix() + "PLAYER_VAR_" + std::to_string(v);
	AP_DataStorageOperation replac;
	replac.operation = "replace";
	replac.value = &val;
	std::vector<AP_DataStorageOperation> operations;
	operations.push_back(replac);
	setRequest.operations = operations;
	setRequest.default_value = 0;
	setRequest.type = AP_DataType::Int;
	setRequest.want_reply = false;
	AP_SetServerData(&setRequest);
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
	AP_GetServerDataRequest* getRequest = new AP_GetServerDataRequest;
	getRequest->key = AP_GetPrivateServerDataPrefix() + "PLAYER_VAR_" + std::to_string(var);
	getRequest->value = new int; //?????????
	getRequest->type = AP_DataType::Int;
	AP_GetServerData(getRequest);

	//add to queue, delete and pop in updateArchipelago
	GetVarRequests.push_front(getRequest);
}

std::string ConnectionStatus()
{
	int retries = AP_GetRetryCount();
	if (retries >= MAX_RETRIES)
	{
		return "Failed";
	}

	AP_ConnectionStatus status = AP_GetConnectionStatus();
	switch (status) {
	case AP_ConnectionStatus::Disconnected:
		if (retries < 0)
		{
			return "Disconnected";
		}
		else
		{
			return "Connecting...";
		}
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

	GetArchipelagoPlayerVars();
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
		/*if (message->type == AP_MessageType::Plaintext)
		{
			NewMessage(message->text.c_str(), 30 * 5);
			messageCooldown = 30 * 5;
		}*/

		AP_ClearLatestMessage();
	}
	for (std::deque<AP_GetServerDataRequest*>::iterator itr = GetVarRequests.begin(); itr != GetVarRequests.end();) {
		if ((*itr)->status == AP_RequestStatus::Done)
		{
			//itr->type yada yada
			//(*itr)->key
			std::string key = (*itr)->key.substr((*itr)->key.find("PLAYER_VAR_") + 11);
			(*itr)->key.erase(0, AP_GetPrivateServerDataPrefix().length() + strlen("PLAYER_VAR_"));
			if ((int*)(*itr)->value != NULL)
			{
				player.var[std::stoi(key)] = *(int*)(*itr)->value;
			}
			std::cout << "nice!34";
			delete (*itr)->value;
			delete (*itr);
			itr = GetVarRequests.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	if (messageCooldown > 0)
	{
		messageCooldown--;
	}

}

void DebugAPCommand() {
	std::cout << "Do stuff";


}
