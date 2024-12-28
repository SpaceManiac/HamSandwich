#ifndef Archipelago_H
#define Archipelago_H

const int AP_BADGEMOD = 1000;
const int AP_MODEMOD = 2000;

#include <string>
#include <string_view>
#include <set>
#include "options.h"
#include <vector>
#include <unordered_map>
#include "player.h"
#include <map>

int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password);
void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y, int itm_id);
void SendCheckedLocQuest(int questVar);
void SendCheckedLocBadge(int badgeID);
void SendCheckedLocReward(int badgeID);
void SendCheckedLocDoll(int dollId);
void SendLocationScout(int locId, bool createHint);
void UpdateArchipelago();
void SendArchipelagoPlayerVar(int v, int val);
void SendDeathLink();
void WinArchipelago();
void ArchipelagoLoadPlayer();
std::string_view ConnectionStatus();
void DebugAPCommand();
void GetRoomInfo();


void Disconnect();

struct SlotData {
	int difficulty;
	int long_checks;
	int multi_save;
	int remix;
	int badges;
	int dolls;
	int deathlink;
};

#define AP_OP_DISABLED 0
#define AP_OP_ENABLED 1

#define AP_OP_FULL 0
#define AP_OP_VANILLA 1
#define AP_OP_NONE 2

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
	std::set<std::string> flags;
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
	bool hint = false;
	int location_id = 0;
};



extern byte ap_cheatsAvail[40];
extern byte ap_modesAvail[6];
extern std::unordered_map<int, bool> locsFound;
extern bool ArchipelagoMode;
extern bool locationWait;
extern std::unordered_map<int, chatData> chat_table;
extern std::string ArchipelagoSeed;
extern int ArchipelagoSlotNum;
extern SlotData apSlotData;
extern std::map<std::string, int> scouted_items;

const std::set<int> DataStorageVars = { VAR_ORBSTAND, VAR_BOOTSALES, VAR_WITCHTALK, VAR_BRIDGEOPEN, VAR_ZOMBIE, VAR_ZOMBIEREWARD, VAR_VAMPYWALL, VAR_BATSTAND, VAR_VAMPSTAND,
VAR_KNOWWOLVES, VAR_DOLL, VAR_DOLLGIVEN, VAR_TOWNOPEN, VAR_HELPERBAT, VAR_TALKBAT, VAR_TALKBRIDGE, VAR_PAIDBRIDGE, VAR_PORTALOPEN, VAR_DEADELDER, VAR_GATEOUT, VAR_DEADBONK,
VAR_SEERTALK, VAR_MINERTALK,VAR_WITCHREWARD, VAR_LANTERN, VAR_TREEREWARD, VAR_SILVERSLING, VAR_LARRYREWARD, VAR_CROPSREWARD };

#include "loonyArchipelagoData.h"
#endif
