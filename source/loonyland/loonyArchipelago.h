#ifndef Archipelago_H
#define Archipelago_H

const int AP_BADGEMOD = 1000;
const int AP_MODEMOD = 1000;

#include <string>
#include "player.h"
#include <set>
#include "options.h"

int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password);
void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y);
void SendCheckedLocQuest(int questVar);
void UpdateArchipelago();
void SendArchipelagoPlayerVar(int v, int val);
void SendDeathLink();
void WinArchipelago();
void ArchipelagoLoadPlayer();
std::string ConnectionStatus();
void DebugAPCommand();

void Disconnect();
void GetInfoFromAP();

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
	std::list<int> chatCodes;
};

struct itemData
{
	std::string Name;
	int ingame_ID;
	int sound_ID;
};

struct chatData {
	std::string orig;
	std::string updated;
};


extern byte ap_cheatsAvail[40];
extern byte ap_modesAvail[5];
extern  std::unordered_map<int, bool> locsFound;
extern std::unordered_map<int, int> itemsFound;
extern bool ArchipelagoMode;
extern bool locationWait;
extern std::unordered_map<int, chatData> chat_table;
extern std::string ArchipelagoSeed;

const std::set<int> DataStorageVars = { VAR_ORBSTAND, VAR_BOOTSALES, VAR_WITCHTALK, VAR_BRIDGEOPEN, VAR_ZOMBIE, VAR_ZOMBIEREWARD, VAR_VAMPYWALL, VAR_BATSTAND, VAR_VAMPSTAND,
VAR_KNOWWOLVES, VAR_DOLL, VAR_DOLLGIVEN, VAR_TOWNOPEN, VAR_HELPERBAT, VAR_TALKBAT, VAR_TALKBRIDGE, VAR_PAIDBRIDGE, VAR_PORTALOPEN, VAR_DEADELDER, VAR_GATEOUT, VAR_DEADBONK,
VAR_SEERTALK, VAR_MINERTALK,VAR_WITCHREWARD, VAR_LANTERN, VAR_TREEREWARD, VAR_SILVERSLING, VAR_LARRYREWARD, VAR_CROPSREWARD };

#include "loonyArchipelagoData.h"
#endif
