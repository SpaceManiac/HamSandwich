#ifndef Archipelago_H
#define Archipelago_H

#include <string>
#include "player.h"
#include <set>

int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password);
void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y);
void SendCheckedLocQuest(int questVar);
void UpdateArchipelago();
void SendArchipelagoPlayerVar(int v, int val);
std::string ConnectionStatus();

void Disconnect();
void GetInfoFromAP();

extern  std::unordered_map<int, bool> locsFound;
extern std::unordered_map<int, int> itemsFound;
extern bool ArchipelagoMode;
extern bool locationWait;


const std::unordered_map<int, int> item_frequencies = {
	{VAR_HEART, 20},
	{VAR_LIGHTNING, 10},
	{VAR_ARROW, 10},
	{VAR_PANTS, 10},
	{VAR_MUSHROOM, 10},
	{VAR_MYSORB,  4},
	{VAR_VAMPBUST, 8},
	{VAR_GEM, 6},
	{VAR_BATSTATUE, 4}
};

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
};

struct itemData {

	int item_ID;
	std::string Name;
	int ingame_ID;
	
};

const std::set<int> DataStorageVars = { VAR_ORBSTAND, VAR_BOOTSALES, VAR_WITCHTALK, VAR_BRIDGEOPEN, VAR_ZOMBIE, VAR_ZOMBIEREWARD, VAR_VAMPYWALL, VAR_BATSTAND, VAR_VAMPSTAND,
VAR_KNOWWOLVES, VAR_DOLL, VAR_DOLLGIVEN, VAR_TOWNOPEN, VAR_HELPERBAT, VAR_TALKBAT, VAR_TALKBRIDGE, VAR_PAIDBRIDGE, VAR_PORTALOPEN, VAR_DEADELDER, VAR_GATEOUT, VAR_DEADBONK,
VAR_SEERTALK, VAR_MINERTALK,VAR_WITCHREWARD, VAR_LANTERN, VAR_TREEREWARD, VAR_SILVERSLING, VAR_LARRYREWARD, VAR_CROPSREWARD };

static locationData basic_locations[] = {
{"Swamp: Mud Path","Pickup",0,"Halloween Hill",0,194,5,24,25,"Slurpy Swamp Mud"},
{"Swamp: Bog Beast","Pickup",1,"Halloween Hill",0,187,112,11,12,"Halloween Hill"},
{"Rocky Cliffs: Upper Ledge","Pickup",2,"Halloween Hill",0,2,46,69,70,"Rocky Cliffs"},
{"Swamp: Sapling Shrine","Pickup",3,"Halloween Hill",0,131,26,10,9,"Slurpy Swamp Mud"},
{"Terror Glade: South Trees","Pickup",4,"Halloween Hill",0,83,145,33,34,"Halloween Hill"},
{"Rocky Cliffs: Vine","Pickup",5,"Halloween Hill",0,1,73,47,48,"Rocky Cliffs"},
{"Rocky Cliffs: Grand Pharoh","Pickup",6,"Halloween Hill",0,30,39,73,74,"Rocky Cliffs"},
{"Rocky Cliffs: Rock Corner","Pickup",7,"Halloween Hill",0,69,59,66,67,"Rocky Cliffs"},
{"Swamp: Outside Luniton","Pickup",8,"Halloween Hill",0,115,70,18,19,"Halloween Hill"},
{"Swamp: East 1","Pickup",9,"Halloween Hill",0,159,85,20,21,"Halloween Hill"},
{"Swamp: Top Left dry","Pickup",10,"Halloween Hill",0,117,33,22,23,"Halloween Hill"},
{"Swamp: East 2","Pickup",11,"Halloween Hill",0,168,98,28,29,"Halloween Hill"},
{"Woods: Above Castle","Pickup",12,"Halloween Hill",0,197,161,56,57,"Halloween Hill"},
{"Rocky Cliffs: Entrance Ledge","Pickup",13,"Halloween Hill",0,16,61,49,50,"Rocky Cliffs"},
{"Rocky Cliffs: Peak","Pickup",14,"Halloween Hill",0,28,10,63,65,"Rocky Cliffs"},
{"Woods: SW of Cabin","Pickup",15,"Halloween Hill",0,134,182,97,98,"Halloween Hill"},
{"Witch's Cabin: Bedroom","Pickup",16,"The Witch's Cabin",2,27,16,1,2,"The Witch's Cabin Front"},
{"Witch's Cabin: Backroom","Pickup",17,"The Witch's Cabin",2,22,7,3,4,"The Witch's Cabin Back"},
{"Bonita's Cabin: Barrel Maze","Pickup",18,"Bonita's Cabin",3,10,8,1,3,"Bonita's Cabin"},
{"Bog Pit: Top Door","Pickup",19,"The Bog Pit",4,19,5,1,2,"The Bog Pit"},
{"Bog Pit: Posts Room","Pickup",20,"The Bog Pit",4,39,38,3,4,"The Bog Pit"},
{"Bog Pit: Drippy Window","Pickup",21,"The Bog Pit",4,33,6,9,10,"The Bog Pit"},
{"Bog Pit: Green room","Pickup",22,"The Bog Pit",4,3,33,7,8,"The Bog Pit"},
{"Bog Pit: Arena","Pickup",23,"The Bog Pit",4,25,48,5,11,"The Bog Pit"},
{"Bog Pit: Sw Switch","Pickup",24,"The Bog Pit",4,2,58,17,18,"The Bog Pit"},
{"Tunnel: Swampdog Pumpkin Door","Pickup",25,"Underground Tunnel",5,53,41,0,1,"Underground Tunnel Top"},
{"Tunnel: Scratch Wall","Pickup",26,"Underground Tunnel",5,57,37,13,14,"Underground Tunnel Top"},
{"Tunnel: Narrow Passage","Pickup",27,"Underground Tunnel",5,26,25,4,5,"Underground Tunnel Top"},
{"Tunnel: Top Frogs","Pickup",28,"Underground Tunnel",5,50,26,7,8,"Underground Tunnel Top"},
{"Tunnel: Torch Island","Pickup",29,"Underground Tunnel",5,17,64,9,10,"Underground Tunnel Mud"},
{"Tunnel: Small Room","Pickup",30,"Underground Tunnel",5,44,49,11,12,"Underground Tunnel Top"},
{"Swamp Gas: Scratch Wall","Pickup",31,"Swamp Gas Cavern",6,32,45,2,3,"Swamp Gas Cavern Front"},
{"Swamp Gas: Bat Door","Pickup",32,"Swamp Gas Cavern",6,44,37,15,17,"Swamp Gas Cavern Front"},
{"Swamp Gas: Stair room","Pickup",33,"Swamp Gas Cavern",6,3,7,23,24,"Swamp Gas Cavern Back"},
{"Swamp Gas: Rock Prison","Pickup",34,"Swamp Gas Cavern",6,43,58,18,19,"Swamp Gas Cavern Front"},
{"A Tiny Cabin","Pickup",35,"A Tiny Cabin",7,11,16,1,2,"A Tiny Cabin"},
{"Seer: Bedside","Pickup",36,"A Cabin",8,19,15,3,4,"A Cabin Seer"},
{"Dusty Crypt: Pumpkin Door","Pickup",37,"Dusty Crypt",10,2,44,2,3,"Dusty Crypt"},
{"Dusty Crypt: Maze","Pickup",38,"Dusty Crypt",10,5,36,4,5,"Dusty Crypt"},
{"Musty Crypt: Maze Room","Pickup",39,"Musty Crypt",11,51,82,11,12,"Musty Crypt"},
{"Rusty Crypt: Vine","Pickup",40,"Rusty Crypt",12,49,73,15,16,"Rusty Crypt"},
{"Rusty Crypt: Boulders","Pickup",41,"Rusty Crypt",12,7,97,0,1,"Rusty Crypt"},
{"A Messy Cabin","Pickup",42,"A Messy Cabin",13,13,6,1,2,"A Messy Cabin"},
{"Under The Lake: Behind Lightning Rod","Pickup",43,"Under The Lake",14,40,13,6,7,"Under The Lake"},
{"Under The Lake: Bat Door","Pickup",44,"Under The Lake",14,73,20,8,9,"Under The Lake"},
{"Deeper Lake: Corner","Pickup",45,"Deeper Under The Lake",15,58,77,12,13,"Deeper Under The Lake"},
{"Deeper Lake: Rhombus","Pickup",46,"Deeper Under The Lake",15,51,71,10,11,"Deeper Under The Lake"},
{"Frankenjulie's Reward","Pickup",47,"Frankenjulie's Laboratory",16,15,6,3,4,"Frankenjulie's Laboratory"},
{"Tower: Barracks","Pickup",48,"Haunted Tower",17,59,29,3,4,"Haunted Tower"},
{"Tower F2: Skull Puzzle","Pickup",49,"Haunted Tower, Floor 2",18,8,5,6,7,"Haunted Tower, Floor 2"},
{"PolterGuy's Reward","Pickup",50,"Haunted Tower Roof",20,13,37,1,2,"Haunted Tower Roof"},
{"Tower Basement: DoorDoorDoorDoorDoorDoor","Pickup",51,"Haunted Basement",21,20,13,1,2,"Haunted Basement"},
{"Abandoned Mine: Shaft","Pickup",52,"Abandoned Mines",22,39,2,1,2,"Abandoned Mines"},
{"Shrine of Bombulus: Prize","Pickup",53,"The Shrine Of Bombulus",23,12,3,2,3,"The Shrine Of Bombulus"},
{"Gloomy Cavern: Lockpick","Pickup",54,"A Gloomy Cavern",24,50,44,2,3,"A Gloomy Cavern"},
{"Happy Stick: Hidden","Pickup",55,"Happy Stick Woods",25,61,2,5,6,"Happy Stick Woods"},
{"Happy Stick: Reward","Pickup",56,"Happy Stick Woods",25,35,34,0,1,"Happy Stick Woods"},
{"Wolf Den: Top Left","Pickup",57,"The Wolf Den",26,2,5,1,2,"The Wolf Den"},
{"Wolf Den: Pumpkin Door","Pickup",58,"The Wolf Den",26,6,14,6,7,"The Wolf Den"},
{"Wolf Den: Vine","Pickup",59,"The Wolf Den",26,40,88,3,4,"The Wolf Den"},
{"Upper Cavern: Three Gold Skeletons","Pickup",60,"Upper Creepy Caverns",28,88,38,3,4,"Upper Creepy Caverns"},
{"Under The Ravine: Left Vine","Pickup",61,"Under The Ravine",29,35,39,1,2,"Under The Ravine"},
{"Under The Ravine: Right Vine","Pickup",62,"Under The Ravine",29,103,22,3,4,"Under The Ravine"},
{"Creepy Caverns M: Pharaoh Bat Door","Pickup",63,"Creepy Caverns",30,117,76,3,4,"Creepy Caverns Middle"},
{"Creepy Caverns E: Top Pharaohs","Pickup",64,"Creepy Caverns",30,135,6,10,11,"Creepy Caverns Right"},
{"Creepy Caverns M: Gargoyles","Pickup",65,"Creepy Caverns",30,84,75,5,6,"Creepy Caverns Middle"},
{"Castle Vampy: Top Room","Pickup",66,"Castle Vampy",31,63,12,11,12,"Castle Vampy"},
{"Castle Vampy: Maze","Pickup",67,"Castle Vampy",31,27,21,13,14,"Castle Vampy"},
{"Castle Vampy: Gauntlet","Pickup",68,"Castle Vampy",31,105,16,15,16,"Castle Vampy"},
{"Castle Vampy: Bat Closet","Pickup",69,"Castle Vampy",31,9,92,22,23,"Castle Vampy"},
{"Castle Vampy II: Candle Room","Pickup",70,"Castle Vampy II",32,22,37,6,7,"Castle Vampy II Main"},
{"Castle Vampy II: Bloodsucker Room","Pickup",71,"Castle Vampy II",32,89,22,8,9,"Castle Vampy II Main"},
{"Castle Vampy II: Vampire Lord","Pickup",72,"Castle Vampy II",32,89,89,2,3,"Castle Vampy II Main"},
{"Castle Vampy II: Bat Room","Pickup",73,"Castle Vampy II",32,52,106,4,5,"Castle Vampy II Main"},
{"Cabin in the Woods: Gold Skull","Pickup",74,"Cabin In The Woods",33,18,6,1,2,"Cabin In The Woods"},
{"Castle Vampy III: Center","Pickup",75,"Castle Vampy III",34,63,64,13,14,"Castle Vampy III Main"},
{"Castle Vampy III: Behind the Pews","Pickup",76,"Castle Vampy III",34,94,33,17,18,"Castle Vampy III Main"},
{"Castle Vampy III: AMBUSH!","Pickup",77,"Castle Vampy III",34,101,110,5,6,"Castle Vampy III Main"},
{"Castle Vampy III: Halloween","Pickup",78,"Castle Vampy III",34,103,19,10,9,"Castle Vampy III Main"},
{"Castle Vampy III: Bat room","Pickup",79,"Castle Vampy III",34,18,11,23,24,"Castle Vampy III Main"},
{"Castle Vampy IV: Right Path","Pickup",80,"Castle Vampy IV",35,94,117,8,9,"Castle Vampy IV Main"},
{"Castle Vampy IV: Left Path","Pickup",81,"Castle Vampy IV",35,34,117,6,7,"Castle Vampy IV Main"},
{"Castle Vampy IV: Ballroom Right","Pickup",82,"Castle Vampy IV",35,104,68,3,4,"Castle Vampy IV Main"},
{"Castle Vampy IV: Right Secret Wall","Pickup",83,"Castle Vampy IV",35,111,102,10,11,"Castle Vampy IV Main"},
{"Castle Vampy IV: Ballroom Left","Pickup",84,"Castle Vampy IV",35,23,68,1,2,"Castle Vampy IV Main"},
{"Roof NW: Gutsy the Elder","Pickup",85,"Castle Vampy Roof",37,13,18,3,4,"Castle Vampy Roof NW"},
{"Roof NE: Stoney the Elder","Pickup",86,"Castle Vampy Roof",38,13,18,3,4,"Castle Vampy Roof NE"},
{"Roof SW: Drippy the Elder","Pickup",87,"Castle Vampy Roof",39,13,18,3,4,"Castle Vampy Roof SW"},
{"Roof SE: Toasty the Elder","Pickup",88,"Castle Vampy Roof",40,13,18,3,4,"Castle Vampy Roof SE"},
{"Bonkula","Pickup",89,"The Heart of Terror",42,16,28,3,4,"The Heart Of Terror"},
{"Hidey-Hole: Bat Door","Pickup",90,"A Hidey-Hole",43,5,22,1,2,"A Hidey-Hole"},
{"Hidey-Hole: Pebbles","Pickup",91,"A Hidey-Hole",43,18,22,4,5,"A Hidey-Hole"},
{"Swampdog Lair: Entrance","Pickup",92,"Swampdog Lair",45,55,49,0,1,"Swampdog Lair"},
{"Swampdog Lair: End","Pickup",93,"Swampdog Lair",45,55,36,5,6,"Swampdog Lair"},
{"Q: Save Halloween Hill","Quest",94,"Save Halloween Hill",0,0,0,0,0,"The Evilizer"},
{"Q: Tree Trimming","Quest",95,"Tree Trimming",1,0,0,0,0,"A Cabin Trees"},
{"Q: Scaredy Cat","Quest",96,"Scaredy Cat",2,0,0,0,0,"Halloween Hill"},
{"Q: Sticky Shoes","Quest",97,"Sticky Shoes",3,0,0,0,0,"Halloween Hill"},
{"Q: Mushroom Hunt","Quest",98,"Witch Mushrooms",4,0,0,0,0,"The Witch's Cabin Front"},
{"Q: Zombie Stomp","Quest",99,"Zombie Stomp",8,0,0,0,0,"Zombiton"},
{"Q: Smashing Pumpkins","Quest",100,"Smashing Pumpkins",9,0,0,0,0,"Halloween Hill"},
{"Q: Silver Bullet","Quest",101,"Silver Bullet",14,0,0,0,0,"Halloween Hill"},
{"Q: Hairy Larry","Quest",102,"Hairy Larry",15,0,0,0,0,"A Cabin Larry"},
{"Q: Ghostbusting","Quest",103,"Ghostbusting",16,0,0,0,0,"The Witch's Cabin Front"},
{"Q: The Rescue","Quest",104,"The Rescue",19,0,0,0,0,"A Gloomy Cavern"},
{"Q: The Collection","Quest",105,"The Collection",20,0,0,0,0,"A Cabin Collector"}
};

const std::unordered_map<int, int> basic_items = {
{ VAR_HEART	, ITM_SUPERHEART},
{ VAR_LIGHTNING	, 	ITM_FIREPOWERUP},
{ VAR_ARROW	, 	ITM_RANGEUP},
{ VAR_PANTS	, 	ITM_FIRERATEUP},
{ VAR_MUSHROOM	, 	ITM_SHROOM},
{ VAR_MYSORB 	, 	ITM_MYSORB},
{ VAR_WEAPON 	, 	ITM_WBOMB},
{ VAR_WEAPON + 1	, 	ITM_WLIGHTNING},
{ VAR_WEAPON + 2	, 	ITM_WICE},
{ VAR_WEAPON + 3	, 	ITM_WCACTUS},
{ VAR_WEAPON + 4	, 	ITM_WBOOMERANG},
{ VAR_WEAPON + 5	, 	ITM_WWHOOPEE},
{ VAR_WEAPON + 6	, 	ITM_WHOTPANTS},
{ VAR_SKULLKEY	, 	ITM_KEY2},
{ VAR_BATKEY	, 	ITM_KEY3},
{ VAR_PUMPKINKEY	, 	ITM_KEY4},
{ VAR_BOOTS	, 	ITM_BOOTS},
{ VAR_STICK	, 	ITM_STICK},
{ VAR_FERTILIZER	, 	ITM_FERTILIZER},
{ VAR_SILVER	, 	ITM_SILVER},
{ VAR_DAISY	, 	ITM_DAISY},
{ VAR_POTION	, 	ITM_GHOSTPOTION},
{ VAR_VAMPBUST	, 	ITM_BUST},
{ VAR_CAT	, 	ITM_CAT},
{ VAR_GEM	, 	ITM_SUPERGEM},
{ VAR_ZOMBIEGEM	, 	ITM_ZOMBGEM},
{ VAR_TRIPLEFIRE	, 	ITM_TRIPLEFIRE},
{ VAR_TALISMAN	, 	ITM_TALISMAN},
{ VAR_BATSTATUE	, 	ITM_BAT},
{ VAR_LANTERN	, 	ITM_LANTERN},
{ VAR_REFLECT	, 	ITM_REFLECTGEM},
{ VAR_SILVERSLING	, 	ITM_SILVERSLING}
};


#endif
