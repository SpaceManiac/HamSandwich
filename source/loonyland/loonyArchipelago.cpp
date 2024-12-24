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
#include "badge.h"

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
void GivePlayerItem(int64_t item_id, bool loud);


std::unordered_map<int, bool> locsFound;
std::unordered_map<int, int> itemsFound;
bool ArchipelagoMode = true;
bool ExpectingDeath = false;
byte ap_cheatsAvail[40] = { 0 };
byte ap_modesAvail[6] = { 0 };

byte cheat_to_badge[NUM_BADGES] = {
	BADGE_DARKNESS, BADGE_BOWLING, BADGE_HUNDREDPCT, BADGE_LOONYBALL, BADGE_EVILIZER, BADGE_GEMS,
	BADGE_MONSTERPTS2, BADGE_POLTERGUY, BADGE_BESTRANK, BADGE_GRAVES, BADGE_BONKULA, BADGE_ANNOY,
	BADGE_WINGAME, BADGE_COMBO, BADGE_ROCKS, BADGE_HIDDEN, BADGE_TERRORSRV, BADGE_KILLALL,
	BADGE_SNEAK, BADGE_MONSTERPTS, BADGE_SURVIVAL, BADGE_LARRY, BADGE_FRANKENJ, BADGE_BOSSBASH,
	BADGE_CATTAILS, BADGE_WITCH, BADGE_WOLFDEN, BADGE_RESCUE, BADGE_TERROR, BADGE_SURVCOMBO,
	BADGE_REMIX, BADGE_WITCHCRAFT, BADGE_THIEF, BADGE_SURV100, BADGE_RIOT, BADGE_SWAMPDOG,
	BADGE_SCORE, BADGE_BRAWL, BADGE_HIDDEN2, BADGE_MASTER,
};

locationData basic_locations[] = { {"Swamp: Mud Path","Pickup",0,"Halloween Hill",0,194,5,24,25,"Slurpy Swamp Mud",{}},
{"Swamp: Bog Beast","Pickup",1,"Halloween Hill",0,187,112,11,12,"Halloween Hill",{}},
{"Rocky Cliffs: Upper Ledge","Pickup",2,"Halloween Hill",0,2,46,69,70,"Rocky Cliffs",{}},
{"Swamp: Sapling Shrine","Pickup",3,"Halloween Hill",0,131,26,10,9,"Slurpy Swamp Mud",{}},
{"Terror Glade: South Trees","Pickup",4,"Halloween Hill",0,83,145,33,34,"Halloween Hill",{}},
{"Rocky Cliffs: Vine","Pickup",5,"Halloween Hill",0,1,73,47,48,"Rocky Cliffs",{}},
{"Rocky Cliffs: Grand Pharoh","Pickup",6,"Halloween Hill",0,30,39,73,74,"Rocky Cliffs",{}},
{"Rocky Cliffs: Rock Corner","Pickup",7,"Halloween Hill",0,69,59,66,67,"Rocky Cliffs",{}},
{"Swamp: Outside Luniton","Pickup",8,"Halloween Hill",0,115,70,18,19,"Halloween Hill",{}},
{"Swamp: East 1","Pickup",9,"Halloween Hill",0,159,85,20,21,"Halloween Hill",{}},
{"Swamp: Top Left dry","Pickup",10,"Halloween Hill",0,117,33,22,23,"Halloween Hill",{}},
{"Swamp: East 2","Pickup",11,"Halloween Hill",0,168,98,28,29,"Halloween Hill",{}},
{"Woods: Above Castle","Pickup",12,"Halloween Hill",0,197,161,56,57,"Halloween Hill",{}},
{"Rocky Cliffs: Entrance Ledge","Pickup",13,"Halloween Hill",0,16,61,49,50,"Rocky Cliffs",{}},
{"Rocky Cliffs: Peak","Pickup",14,"Halloween Hill",0,28,10,63,65,"Rocky Cliffs",{}},
{"Woods: SW of Cabin","Pickup",15,"Halloween Hill",0,134,182,97,98,"Halloween Hill",{}},
{"Witch's Cabin: Bedroom","Pickup",16,"The Witch's Cabin",2,27,16,1,2,"The Witch's Cabin",{}},
{"Witch's Cabin: Backroom","Pickup",17,"The Witch's Cabin",2,22,7,3,4,"The Witch's Cabin Back",{}},
{"Bonita's Cabin: Barrel Maze","Pickup",18,"Bonita's Cabin",3,10,8,1,3,"Bonita's Cabin",{14,15}},
{"Bog Pit: Top Door","Pickup",19,"The Bog Pit",4,19,5,1,2,"The Bog Pit",{}},
{"Bog Pit: Posts Room","Pickup",20,"The Bog Pit",4,39,38,3,4,"The Bog Pit",{}},
{"Bog Pit: Drippy Window","Pickup",21,"The Bog Pit",4,33,6,9,10,"The Bog Pit",{}},
{"Bog Pit: Green room","Pickup",22,"The Bog Pit",4,3,33,7,8,"The Bog Pit",{}},
{"Bog Pit: Arena","Pickup",23,"The Bog Pit",4,25,48,5,11,"The Bog Pit",{}},
{"Bog Pit: Sw Switch","Pickup",24,"The Bog Pit",4,2,58,17,18,"The Bog Pit",{}},
{"Tunnel: Swampdog Pumpkin Door","Pickup",25,"Underground Tunnel",5,53,41,0,1,"Underground Tunnel",{}},
{"Tunnel: Scratch Wall","Pickup",26,"Underground Tunnel",5,57,37,13,14,"Underground Tunnel",{}},
{"Tunnel: Narrow Passage","Pickup",27,"Underground Tunnel",5,26,25,4,5,"Underground Tunnel",{}},
{"Tunnel: Top Frogs","Pickup",28,"Underground Tunnel",5,50,26,7,8,"Underground Tunnel",{}},
{"Tunnel: Torch Island","Pickup",29,"Underground Tunnel",5,17,64,9,10,"Underground Tunnel Mud",{}},
{"Tunnel: Small Room","Pickup",30,"Underground Tunnel",5,44,49,11,12,"Underground Tunnel",{}},
{"Swamp Gas: Scratch Wall","Pickup",31,"Swamp Gas Cavern",6,32,45,2,3,"Swamp Gas Cavern",{}},
{"Swamp Gas: Bat Door","Pickup",32,"Swamp Gas Cavern",6,44,37,15,17,"Swamp Gas Cavern",{}},
{"Swamp Gas: Stair room","Pickup",33,"Swamp Gas Cavern",6,3,7,23,24,"Swamp Gas Cavern Back",{}},
{"Swamp Gas: Rock Prison","Pickup",34,"Swamp Gas Cavern",6,43,58,18,19,"Swamp Gas Cavern",{}},
{"A Tiny Cabin","Pickup",35,"A Tiny Cabin",7,11,16,1,2,"A Tiny Cabin",{}},
{"Seer: Bedside","Pickup",36,"A Cabin",8,19,15,3,4,"A Cabin Seer",{130}},
{"Dusty Crypt: Pumpkin Door","Pickup",37,"Dusty Crypt",10,2,44,2,3,"Dusty Crypt",{}},
{"Dusty Crypt: Maze","Pickup",38,"Dusty Crypt",10,5,36,4,5,"Dusty Crypt",{}},
{"Musty Crypt: Maze Room","Pickup",39,"Musty Crypt",11,51,82,11,12,"Musty Crypt",{}},
{"Rusty Crypt: Vine","Pickup",40,"Rusty Crypt",12,49,73,15,16,"Rusty Crypt",{}},
{"Rusty Crypt: Boulders","Pickup",41,"Rusty Crypt",12,7,97,0,1,"Rusty Crypt",{}},
{"A Messy Cabin","Pickup",42,"A Messy Cabin",13,13,6,1,2,"A Messy Cabin",{}},
{"Under The Lake: Behind Lightning Rod","Pickup",43,"Under The Lake",14,40,13,6,7,"Under The Lake",{}},
{"Under The Lake: Bat Door","Pickup",44,"Under The Lake",14,73,20,8,9,"Under The Lake",{}},
{"Deeper Lake: Corner","Pickup",45,"Deeper Under The Lake",15,58,77,12,13,"Deeper Under The Lake",{}},
{"Deeper Lake: Rhombus","Pickup",46,"Deeper Under The Lake",15,51,71,10,11,"Deeper Under The Lake",{}},
{"Frankenjulie's Reward","Pickup",47,"Frankenjulie's Laboratory",16,15,6,3,4,"Frankenjulie's Laboratory",{}},
{"Tower: Barracks","Pickup",48,"Haunted Tower",17,59,29,3,4,"Haunted Tower",{}},
{"Tower F2: Skull Puzzle","Pickup",49,"Haunted Tower Floor 2",18,8,5,6,7,"Haunted Tower Floor 2",{}},
{"PolterGuy's Reward","Pickup",50,"Haunted Tower Roof",20,13,37,1,2,"Haunted Tower Roof",{68}},
{"Tower Basement: DoorDoorDoorDoorDoorDoor","Pickup",51,"Haunted Basement",21,20,13,1,2,"Haunted Basement",{}},
{"Abandoned Mine: Shaft","Pickup",52,"Abandoned Mines",22,39,2,1,2,"Abandoned Mines",{}},
{"Shrine Of Bombulus: Prize","Pickup",53,"The Shrine Of Bombulus",23,12,3,2,3,"The Shrine Of Bombulus",{}},
{"Gloomy Cavern: Lockpick","Pickup",54,"A Gloomy Cavern",24,50,44,2,3,"A Gloomy Cavern",{}},
{"Happy Stick: Hidden","Pickup",55,"Happy Stick Woods",25,61,2,5,6,"Happy Stick Woods",{}},
{"Happy Stick: Reward","Pickup",56,"Happy Stick Woods",25,35,34,0,1,"Happy Stick Woods",{}},
{"Wolf Den: Top Left","Pickup",57,"The Wolf Den",26,2,5,1,2,"The Wolf Den",{}},
{"Wolf Den: Pumpkin Door","Pickup",58,"The Wolf Den",26,6,14,6,7,"The Wolf Den",{}},
{"Wolf Den: Vine","Pickup",59,"The Wolf Den",26,40,88,3,4,"The Wolf Den",{}},
{"Upper Cavern: Three Gold Skeletons","Pickup",60,"Upper Creepy Caverns",28,88,38,3,4,"Upper Creepy Caverns",{}},
{"Under The Ravine: Left Vine","Pickup",61,"Under The Ravine",29,35,39,1,2,"Under The Ravine",{}},
{"Under The Ravine: Right Vine","Pickup",62,"Under The Ravine",29,103,22,3,4,"Under The Ravine",{}},
{"Creepy Caverns M: Pharaoh Bat Door","Pickup",63,"Creepy Caverns",30,117,76,3,4,"Creepy Caverns Middle",{}},
{"Creepy Caverns R: Top Pharaohs","Pickup",64,"Creepy Caverns",30,135,6,10,11,"Creepy Caverns Right",{}},
{"Creepy Caverns M: Gargoyles","Pickup",65,"Creepy Caverns",30,84,75,5,6,"Creepy Caverns Middle",{}},
{"Castle Vampy: Top Room","Pickup",66,"Castle Vampy",31,63,12,11,12,"Castle Vampy",{}},
{"Castle Vampy: Maze","Pickup",67,"Castle Vampy",31,27,21,13,14,"Castle Vampy",{}},
{"Castle Vampy: Gauntlet","Pickup",68,"Castle Vampy",31,105,16,15,16,"Castle Vampy",{}},
{"Castle Vampy: Bat Closet","Pickup",69,"Castle Vampy",31,9,92,22,23,"Castle Vampy",{}},
{"Castle Vampy II: Candle Room","Pickup",70,"Castle Vampy II",32,22,37,6,7,"Castle Vampy II",{}},
{"Castle Vampy II: Bloodsucker Room","Pickup",71,"Castle Vampy II",32,89,22,8,9,"Castle Vampy II",{}},
{"Castle Vampy II: Vampire Lord","Pickup",72,"Castle Vampy II",32,89,89,2,3,"Castle Vampy II",{}},
{"Castle Vampy II: Bat Room","Pickup",73,"Castle Vampy II",32,52,106,4,5,"Castle Vampy II",{}},
{"Cabin in the Woods: Gold Skull","Pickup",74,"Cabin In The Woods",33,18,6,1,2,"Cabin In The Woods",{}},
{"Castle Vampy III: Center","Pickup",75,"Castle Vampy III",34,63,64,13,14,"Castle Vampy III",{}},
{"Castle Vampy III: Behind the Pews","Pickup",76,"Castle Vampy III",34,94,33,17,18,"Castle Vampy III",{}},
{"Castle Vampy III: AMBUSH!","Pickup",77,"Castle Vampy III",34,101,110,5,6,"Castle Vampy III",{}},
{"Castle Vampy III: Halloween","Pickup",78,"Castle Vampy III",34,103,19,10,9,"Castle Vampy III",{}},
{"Castle Vampy III: Bat room","Pickup",79,"Castle Vampy III",34,18,11,23,24,"Castle Vampy III",{}},
{"Castle Vampy IV: Right Path","Pickup",80,"Castle Vampy IV",35,94,117,8,9,"Castle Vampy IV",{}},
{"Castle Vampy IV: Left Path","Pickup",81,"Castle Vampy IV",35,34,117,6,7,"Castle Vampy IV",{}},
{"Castle Vampy IV: Ballroom Right","Pickup",82,"Castle Vampy IV",35,104,68,3,4,"Castle Vampy IV",{}},
{"Castle Vampy IV: Right Secret Wall","Pickup",83,"Castle Vampy IV",35,111,102,10,11,"Castle Vampy IV",{}},
{"Castle Vampy IV: Ballroom Left","Pickup",84,"Castle Vampy IV",35,23,68,1,2,"Castle Vampy IV",{}},
{"Roof NW: Gutsy the Elder","Pickup",85,"Castle Vampy Roof",37,13,18,3,4,"Castle Vampy Roof NW",{}},
{"Roof NE: Stoney the Elder","Pickup",86,"Castle Vampy Roof",38,13,18,3,4,"Castle Vampy Roof NE",{}},
{"Roof SW: Drippy the Elder","Pickup",87,"Castle Vampy Roof",39,13,18,3,4,"Castle Vampy Roof SW",{}},
{"Roof SE: Toasty the Elder","Pickup",88,"Castle Vampy Roof",40,13,18,3,4,"Castle Vampy Roof SE",{}},
{"Bonkula Item","Pickup",89,"The Heart Of Terror",42,16,28,3,4,"The Heart Of Terror",{}},
{"Hidey-Hole: Bat Door","Pickup",90,"A Hidey-Hole",43,5,22,1,2,"A Hidey-Hole",{}},
{"Hidey-Hole: Pebbles","Pickup",91,"A Hidey-Hole",43,18,22,4,5,"A Hidey-Hole",{}},
{"Swampdog Lair: Entrance","Pickup",92,"Swampdog Lair",45,55,49,0,1,"Swampdog Lair",{}},
{"Swampdog Lair: End","Pickup",93,"Swampdog Lair",45,55,36,5,6,"Swampdog Lair",{}},
{"Q: Save Halloween Hill","Quest",94,"",0,0,0,0,0,"The Evilizer",{}},
{"Q: Tree Trimming","Quest",95,"",1,0,0,0,0,"Halloween Hill",{2}},
{"Q: Scaredy Cat","Quest",96,"",2,0,0,0,0,"Halloween Hill",{26,27}},
{"Q: Sticky Shoes","Quest",97,"",3,0,0,0,0,"Halloween Hill",{8,9,10,167}},
{"Q: Mushroom Hunt","Quest",98,"",4,0,0,0,0,"The Witch's Cabin",{31,191}},
{"Q: Picking Daisies","Quest",99,"",5,0,0,0,0,"Menu",{}},
{"Q: Curse The Darkness","Quest",100,"",6,0,0,0,0,"Swamp Gas Cavern",{}},
{"Q: Zombie Stomp","Quest",101,"",7,0,0,0,0,"Zombiton",{56}},
{"Q: Smashing Pumpkins","Quest",102,"",8,0,0,0,0,"Halloween Hill",{42,43}},
{"Q: Mystery Lake","Quest",103,"",9,0,0,0,0,"Halloween Hill",{}},
{"Q: Beneath The Lake","Quest",104,"",10,0,0,0,0,"Frankenjulie's Laboratory",{}},
{"Q: Happy Stick?!?","Quest",105,"",11,0,0,0,0,"Happy Stick Woods",{}},
{"Q: A True Hero","Quest",106,"",12,0,0,0,0,"Under The Ravine",{}},
{"Q: Silver Bullet","Quest",107,"",13,0,0,0,0,"Zombiton",{70,75}},
{"Q: Hairy Larry","Quest",108,"",14,0,0,0,0,"The Wolf Den",{85}},
{"Q: Ghostbusting","Quest",109,"",15,0,0,0,0,"The Witch's Cabin",{29,60,61,62,164}},
{"Q: The Haunted Tower","Quest",110,"",16,0,0,0,0,"Haunted Tower Roof",{}},
{"Q: The Last Gate","Quest",111,"",17,0,0,0,0,"Castle Vampy IV",{}},
{"Q: The Rescue","Quest",112,"",18,0,0,0,0,"A Gloomy Cavern",{79}},
{"Q: The Collection","Quest",113,"",19,0,0,0,0,"A Cabin Collector",{92,96}},
{"Evil Smashin'","Badge",114,"",0,0,0,0,0,"The Evilizer",{}},
{"Bowlin'","Badge",115,"",1,0,0,0,0,"Bowling",{}},
{"Vandalizin'","Badge",116,"",2,0,0,0,0,"Zombiton",{}},
{"Greed","Badge",117,"",3,0,0,0,0,"Menu",{}},
{"Monster Slayin'","Badge",118,"",4,0,0,0,0,"Menu",{}},
{"Monster Poundin'","Badge",119,"",5,0,0,0,0,"Menu",{}},
{"Ghostbustin'","Badge",120,"",6,0,0,0,0,"Haunted Tower Roof",{}},
{"Lookin'","Badge",121,"",7,0,0,0,0,"The Witch's Cabin",{}},
{"Ultimate Victory","Badge",122,"",8,0,0,0,0,"The Evilizer",{}},
{"Kickin'","Badge",123,"",9,0,0,0,0,"Loony Ball",{}},
{"Boss Bashin'","Badge",124,"",10,0,0,0,0,"Boss Bash",{}},
{"Vampire Slayin'","Badge",125,"",11,0,0,0,0,"The Heart Of Terror",{}},
{"Sneakin'","Badge",126,"",12,0,0,0,0,"Haunted Tower",{}},
{"Witch Hunt","Badge",127,"",13,0,0,0,0,"Happy Stick Woods",{}},
{"Animal Control","Badge",128,"",14,0,0,0,0,"The Wolf Den",{}},
{"Annoyance","Badge",129,"",15,0,0,0,0,"The Witch's Cabin",{}},
{"Civic Duty","Badge",130,"",16,0,0,0,0,"A Gloomy Cavern",{}},
{"Winnin'","Badge",131,"",17,0,0,0,0,"The Evilizer",{}},
{"Combo Plate","Badge",132,"",18,0,0,0,0,"Menu",{}},
{"Cleanin' Up","Badge",133,"",19,0,0,0,0,"Rocky Cliffs",{}},
{"Findin'","Badge",134,"",20,0,0,0,0,"The Wolf Den",{}},
{"Extreme Survivin'","Badge",135,"",21,0,0,0,0,"Survival",{}},
{"Huntin'","Badge",136,"",22,0,0,0,0,"Halloween Hill",{}},
{"Advanced Winnin'","Badge",137,"",23,0,0,0,0,"Menu",{}},
{"Survivin'","Badge",138,"",24,0,0,0,0,"Survival",{}},
{"Puppy Poundin'","Badge",139,"",25,0,0,0,0,"Larry's Lair",{}},
{"Mad Skeelz","Badge",140,"",26,0,0,0,0,"The Evilizer",{}},
{"Frankenfryin'","Badge",141,"",27,0,0,0,0,"Frankenjulie's Laboratory",{}},
{"Combo Magic","Badge",142,"",28,0,0,0,0,"Survival",{}},
{"Hedge Clippin'","Badge",143,"",29,0,0,0,0,"Slurpy Swamp Mud",{}},
{"R-R-R-Remix!","Badge",144,"",30,0,0,0,0,"Remix",{}},
{"Witchcraft","Badge",145,"",31,0,0,0,0,"Menu",{}},
{"Ninjitsu","Badge",146,"",32,0,0,0,0,"Menu",{}},
{"Stayin' Alive","Badge",147,"",33,0,0,0,0,"Survival",{}},
{"Hooliganin'","Badge",148,"",34,0,0,0,0,"Loony Ball",{}},
{"Swampdoggin'","Badge",149,"",35,0,0,0,0,"Menu",{}},
{"Scorin'","Badge",150,"",36,0,0,0,0,"The Evilizer",{}},
{"Brawlin'","Badge",151,"",37,0,0,0,0,"Benny's Cocktails",{}},
{"Minin' For Treasure","Badge",152,"",38,0,0,0,0,"Remix",{}},
{"Digitally Masterin'","Badge",153,"",39,0,0,0,0,"Menu",{}},
{"Tree Trimming Reward","Reward",154,"A Cabin",VAR_TREEREWARD,0,0,0,0,"A Cabin Trees",{}},
{"Larry's Wife Reward","Reward",155,"A Cabin",VAR_LARRYREWARD,0,0,0,0,"A Cabin Larry",{}},
{"Farmer Reward","Reward",156,"Halloween Hill",VAR_CROPSREWARD,0,0,0,0,"Halloween Hill",{}},
{"Mayor Reward","Reward",157,"Halloween Hill",VAR_ZOMBIEREWARD,0,0,0,0,"Zombiton",{}},
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


int ArchipelagoConnect(std::string IPAddress, std::string SlotName, std::string Password) {
	for (int i = 0; i < 40; i++) {
		opt.meritBadge[i] = MERIT_NO;
	}
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
	if (item_id != 3000) {
		GivePlayerItem(item_id, true);
	}
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
		int count = itemsFound[item_id];
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
	if (item_id == VAR_DAISY)
	{
		player.var[VAR_QUESTASSIGN + QUEST_DAISY] = 1;
		player.var[VAR_QUESTDONE + QUEST_DAISY] = 1;
	}

	if (item_id >= VAR_MUSHROOM && item_id < VAR_MUSHROOM + 10) //update quest
	{
		if (ShroomCount() == 10)
		{
			player.var[VAR_QUESTASSIGN + QUEST_SHROOM] = 1;
			player.var[VAR_QUESTDONE + QUEST_SHROOM] = 1;
		}
	}
	if (item_id == VAR_CAT)
	{
		player.var[VAR_QUESTASSIGN + QUEST_CAT] = 1;
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
	if (loud && goodguy != NULL)
	{
		MakeNormalSound(basic_items.at(item_id).sound_ID);
	}
}

void GetLocationScouts()
{
	world_t world;
	LoadWorld(&world, "ap.llw");

	bool allGood = true;

	for (auto &loc : basic_locations)
	{
		//store in location data
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
		if (item->player == ap->player_id())
		{
			loc.scouted_item = item->item - loonyland_base_id;
		}
		else
		{
			loc.scouted_item = ITM_ARCHIPELAGO;
		}

		if (loc.Type == "Pickup" || (loc.Type == "Badge" && loc.MapID == 20))
		{

			Map* tempMap;
			if (loc.Type == "Badge" && loc.MapID == 20)
			{
				tempMap = world.map[26];
			}
			else
			{
				tempMap = world.map[loc.MapID];
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
				tempMap->map[loc.Xcoord + loc.Ycoord * tempMap->width].item = game_id; //from var to placeable item
				if (game_id == ITM_BADGE)
				{
					tempMap->map[loc.Xcoord + loc.Ycoord * tempMap->width].tag = cheat_to_badge[item_id - AP_BADGEMOD] + 1;
				}
			}
			else
				tempMap->map[loc.Xcoord + loc.Ycoord * tempMap->width].item = ITM_BARREL; //from var to placeable item
		}
		for (auto const& i : loc.chatCodes)
		{
			chat_table[i].updated += ap->item_name(item->item);
			chat_table[i].location_id = loc.ID;
		}
	}

	if (allGood)
	{
		locationWait = false;
		SaveWorld(&world, "ap.llw");
	}

	FreeWorld(&world);
}

void SendLocationScout(int locId, bool createHint)
{
	
	ap->scout_locations({ locId+loonyland_base_id }, createHint);
}

void SendCheckedLocPickup(std::string mapName, int mapNum, int x, int y)
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
	//skip survival for now
	if (mapName == "Survival!")
	{
		return;
	}
	for (locationData loc : basic_locations)
	{
		if (loc.MapID == mapNum && loc.Map == mapName && loc.Xcoord == x && loc.Ycoord == y)
		{
			SendCheckedItem(loc.ID);
			if (loc.ID == 17)
			{
				EarnBadge(BADGE_DARKNESS);
			}
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

void SendCheckedLocBadge(int badgeID)
{
	for (locationData loc : basic_locations)
	{
		if (loc.Type == "Badge" && loc.MapID == badgeID) //map ids are where the quest var is stored for those locations
		{
			SendCheckedItem(loc.ID);
			return;
		}
	}
	std::cout << "AP BADGE MISS: " << badgeID << std::endl;
}

void SendCheckedLocReward(int rewardID)
{
	for (locationData loc : basic_locations)
	{
		if (loc.Type == "Reward" && loc.MapID == rewardID) //map ids are where the quest var is stored for those locations
		{
			SendCheckedItem(loc.ID);
			return;
		}
	}
	std::cout << "AP REWARD MISS: " << rewardID << std::endl;
}

void SendCheckedLocDoll(int locID)
{

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
		//EarnBadge(basic_locations[loc_id].MapID);
	}
	else if (basic_locations[loc_id].Type == "Reward")
	{
		opt.meritBadge[basic_locations[loc_id].MapID] = MERIT_EARNED;
		//EarnBadge(basic_locations[loc_id].MapID);
	}
	else if (basic_locations[loc_id].Type == "Drop")
	{
		opt.meritBadge[basic_locations[loc_id].MapID] = MERIT_EARNED;
		//EarnBadge(basic_locations[loc_id].MapID);
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

	world.map[0]->map[83 + 146 * world.map[0]->width].item = 0; //remove tree blocking vision of small items in terror glade

	ham_sprintf(world.map[18]->name, "Haunted Tower Floor 2");
	ham_sprintf(world.map[19]->name, "Haunted Tower Floor 3");

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
	static bool gottenAnything = false;
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
		gottenAnything = true;
	}
	if (gottenAnything)
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
				text += ap->item_name(message->item.item);
				text += " from ";
				text += ap->slot_player_alias(message->item.player);
				NewMessage(text.c_str(), MESSAGE_TIME);
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
				messageCooldown = MESSAGE_TIME;
			}
		}
	}

	// Storage
	std::string prefix = ap->storage_private("PLAYER_VAR_");
	while (auto change = ap->pop_storage_change())
	{
		auto [key, value, _] = *change;
		if (value->isLong() && key.size() > prefix.size() && key.substr(0, prefix.size()) == prefix)
		{
			int var = std::stoi(std::string(key.substr(prefix.size())));
			player.var[var] = value->getLong();
		}
	}
}

void DebugAPCommand() {
	std::cout << "Do stuff";


}
