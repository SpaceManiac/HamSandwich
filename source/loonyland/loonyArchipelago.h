#ifndef Archipelago_H
#define Archipelago_H

#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <unordered_map>
#include "player.h"

int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password);
void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y);
void SendCheckedLocQuest(int questVar);
void UpdateArchipelago();
void SendArchipelagoPlayerVar(int v, int val);
void SendDeathLink();
void WinArchipelago();
void ArchipelagoLoadPlayer();
std::string_view ConnectionStatus();
void DebugAPCommand();

void Disconnect();

struct locationData {
	std::string Name;
	std::string Type;
	int ID;
	std::string Map;
	int MapID;
	int Xcoord;
	int Ycoord;
	int Spec1ID;
	int Spec2ID;
	std::string Region;
	std::vector<int> chatCodes;
};

struct itemData {

	int item_ID;
	std::string Name;
	int ingame_ID;
};

struct chatData {
	std::string orig;
	std::string updated;
};



extern std::unordered_map<int, bool> locsFound;
extern bool ArchipelagoMode;
extern bool locationWait;
extern std::unordered_map<int, chatData> chat_table;
extern std::string ArchipelagoSeed;

const std::set<int> DataStorageVars = { VAR_ORBSTAND, VAR_BOOTSALES, VAR_WITCHTALK, VAR_BRIDGEOPEN, VAR_ZOMBIE, VAR_ZOMBIEREWARD, VAR_VAMPYWALL, VAR_BATSTAND, VAR_VAMPSTAND,
VAR_KNOWWOLVES, VAR_DOLL, VAR_DOLLGIVEN, VAR_TOWNOPEN, VAR_HELPERBAT, VAR_TALKBAT, VAR_TALKBRIDGE, VAR_PAIDBRIDGE, VAR_PORTALOPEN, VAR_DEADELDER, VAR_GATEOUT, VAR_DEADBONK,
VAR_SEERTALK, VAR_MINERTALK,VAR_WITCHREWARD, VAR_LANTERN, VAR_TREEREWARD, VAR_SILVERSLING, VAR_LARRYREWARD, VAR_CROPSREWARD };

#include "loonyArchipelagoData.h"
#endif
