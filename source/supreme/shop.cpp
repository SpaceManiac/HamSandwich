#include "winpch.h"
#include "shop.h"
#include "cheat.h"
#include "items.h"
#include "dialogbits.h"
#include "scanner.h"
#include "theater.h"
#include "arcade.h"
#include "moron.h"
#include "gallery.h"
#include "goal.h"
#include "progress.h"
#include <iostream>

#define NUMSHOPITEMS		(202)
#define NUMBUILTINWORLDS	(95)

char to_lowercase(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c + 32;

	return c;
}

int lockers[60]={};
void RandomizeLockers(void)
{
	int i,j;
	char m[sizeof(profile.name)];
	std::string str = profile.name;

	for (char &c: str) {
		c = to_lowercase(c);
	}	
	
	if(str == "bouapha")
	{
		for(i=0;i<60;i++)
		lockers[i] = i;
		return;
	}	
	
	int lockerCombo[60] = {};
	for (i = 0; i < (int)(sizeof(m)); i++)
		m[i] = profile.name[i];
	for (i = 0; i < 60; i++)
		lockerCombo[i] = i;
	for (i = 0; i < (int)(sizeof(m)); i++)
	{
		j += (m[i]-32)*pow(2,i);
	}
	srand(j);
	
	for(i=0;i<60;i++)
	{
		int k = -1, r;
		while(k == -1)
		{
			r = rand()%60;
			k = lockerCombo[r];
		}
    lockerCombo[r] = -1;
    lockers[i] = k;
	}
}

word moneyAmts[11]={25,75,1500,5000,2222,250,17,300,2,716,42};

shopItem_t shop[NUMSHOPITEMS]={

	// bones, bats, and beyond
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 0},	// mansion
	{SHOP_SPOOKY,
	 300,
	 SHOP_WORLD,
	 1},	// haunted house
	{SHOP_SPOOKY,
	 600,
	 SHOP_WORLD,
	 2},	// loony halloween
	{SHOP_SPOOKY,
	 200,
	 SHOP_WORLD,
	 3},	// halloween horror 1 BP
	{SHOP_SPOOKY,
	 200,
	 SHOP_WORLD,
	 4},	// halloween horror 1 MS
	{SHOP_SPOOKY,
	 200,
	 SHOP_WORLD,
	 5},	// halloween horror 1 POP
	{SHOP_SPOOKY,
	 200,
	 SHOP_WORLD,
	 6},	// halloween horror 1 MH
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 7},	// halloween horror 2 TDM
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 8},	// halloween horror 2 BP
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 9},	// halloween horror 2 LK
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 10},	// halloween horror 2 RZ
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 11},	// halloween horror 2 POP
	{SHOP_SPOOKY,
	 400,
	 SHOP_WORLD,
	 12},	// halloween horror 2 MH
	{SHOP_SPOOKY,
	 800,
	 SHOP_WORLD,
	 13},	// halloween horror 3 BP
	{SHOP_SPOOKY,
	 800,
	 SHOP_WORLD,
	 14},	// halloween horror 3 LK
	{SHOP_SPOOKY,
	 800,
	 SHOP_WORLD,
	 15},	// halloween horror 3 POP
	{SHOP_SPOOKY,
	 800,
	 SHOP_WORLD,
	 16},	// halloween horror 3 TDM
	{SHOP_SPOOKY,
	 500,
	 SHOP_CHEAT,
	 CHEAT_BRAINS},	// get all brains cheat

	 // sputnik's
	{SHOP_GADGET,
	 1000,
	 SHOP_ABILITY,
	 ABIL_HYPER},	// hyper mode ability
	{SHOP_SPACE,
	 600,
	 SHOP_WORLD,
	 17},	// space station
	{SHOP_SPACE,
	 100,
	 SHOP_WORLD,
	 18},	// space battle
	{SHOP_SPACE,
	 400,
	 SHOP_WORLD,
	 29},	// ez-8675
	{SHOP_SPACE,
	 500,
	 SHOP_WORLD,
	 20},	// spaced out
	{SHOP_SPACE,
	 300,
	 SHOP_WORLD,
	 21},	// spaced station
	{SHOP_SPACE,
	 200,
	 SHOP_WORLD,
	 22},	// star ramble
	{SHOP_SPACE,
	 250,
	 SHOP_WORLD,
	 23},	// intergalactic lunacy
	{SHOP_SPACE,
	 500,
	 SHOP_CHEAT,
	 CHEAT_BLACKHOLE},	// energy barrier cheat
	{SHOP_SPACE,
	 2500,
	 SHOP_ABILITY,
	 ABIL_BRAIN},	// brain radar ability
	{SHOP_SPACE,
	 600,
	 SHOP_MAJOR,
	 MAJOR_ARCADE},	// arcade game fun
	{SHOP_SPACE,
	 1000,
	 SHOP_PLAYABLE,
	 PLAY_MECHA},	// play as mechabouapha

	 // pier 2
	{SHOP_WATER,
	 300,
	 SHOP_WORLD,
	 24},	// citanul
	{SHOP_WATER,
	 100,
	 SHOP_WORLD,
	 25},	// island adventure
	{SHOP_WATER,
	 400,
	 SHOP_WORLD,
	 26},	//seahunt
	{SHOP_WATER,
	 500,
	 SHOP_WORLD,
	 27},	// underwater antics
	{SHOP_WATER,
	 300,
	 SHOP_WORLD,
	 28},	// water world
	{SHOP_WATER,
	 100,
	 SHOP_WORLD,
	 39},	// sea quest
	{SHOP_WATER,
	 400,
	 SHOP_WORLD,
	 30},	// prehistoric peril
	{SHOP_WATER,
	 400,
	 SHOP_WORLD,
	 31},	// pygmy island
	{SHOP_WATER,
	 500,
	 SHOP_CHEAT,
	 CHEAT_WATER},	// waterwalk cheat
	{SHOP_WATER,
	 500,
	 SHOP_CHEAT,
	 CHEAT_OXYGEN},	// oxygen cheat
	{SHOP_WATER,
	 1000,
	 SHOP_MAJOR,
	 MAJOR_GALLERY},	// picture gallery
	 // caves
	{SHOP_CAVE,
	 400,
	 SHOP_WORLD,
	 32},	// tunnel vision
	{SHOP_CAVE,
	 300,
	 SHOP_WORLD,
	 33},	// rocky cliffs
	{SHOP_CAVE,
	 250,
	 SHOP_WORLD,
	 34},	// rocky cliffs
	{SHOP_CAVE,
	 250,
	 SHOP_WORLD,
	 35},	// creepy castle
	{SHOP_CAVE,
	 400,
	 SHOP_WORLD,
	 36},	// middle earth
	{SHOP_CAVE,
	 500,
	 SHOP_CHEAT,
	 CHEAT_LIGHT},	// light cheat
	{SHOP_CAVE,
	 500,
	 SHOP_CHEAT,
	 CHEAT_SCANNER},	// scanner cheat (now at rocketshack)
	{SHOP_CAVE,
	 800,
	 SHOP_CHEAT,
	 CHEAT_STRENGTH},	// steelskin
	{SHOP_GADGET,
	 500,
	 SHOP_MAJOR,
	 MAJOR_CHEATMENU},	// cheat menu
	{SHOP_CAVE,
	 300,
	 SHOP_MAJOR,
	 MAJOR_RECORDBOOK},	// record book
	{SHOP_CAVE,
	 200,
	 SHOP_ABILITY,
	 ABIL_RAGE},	// rage ability
	{SHOP_CAVE,
	 1000,
	 SHOP_PLAYABLE,
	 PLAY_WOLF},	// wolfman mystery character

	 // forest
	{SHOP_FOREST,
	 200,
	 SHOP_WORLD,
	 37},	// spooky woods
	{SHOP_FOREST,
	 300,
	 SHOP_WORLD,
	 38},	// wacky woods
	{SHOP_FOREST,
	 100,
	 SHOP_WORLD,
	 39},	// creepy meadows
	{SHOP_FOREST,
	 400,
	 SHOP_WORLD,
	 40},	// loonies goonies baboonies
	{SHOP_FOREST,
	 500,
	 SHOP_WORLD,
	 41},	// wasted wetlands
	{SHOP_FOREST,
	 500,
	 SHOP_WORLD,
	 42},	// goofy grove
	{SHOP_FOREST,
	 500,
	 SHOP_WORLD,
	 43},	// goofy grove 2
	{SHOP_FOREST,
	 500,
	 SHOP_WORLD,
	 44},	// tulipton
	{SHOP_FOREST,
	 500,
	 SHOP_CHEAT,
	 CHEAT_LIFE},	// life cheat
	{SHOP_FOREST,
	 500,
	 SHOP_CHEAT,
	 CHEAT_AMMO},	// ammo crate cheat

	 // weird
	{SHOP_WEIRD,
	 300,
	 SHOP_WORLD,
	 45},	// bunny world
	{SHOP_WEIRD,
	 200,
	 SHOP_WORLD,
	 46},	// loony labs
	{SHOP_WEIRD,
	 300,
	 SHOP_WORLD,
	 47},	// dimension x
	{SHOP_WEIRD,
	 500,
	 SHOP_WORLD,
	 48},	// double trouble
	{SHOP_WEIRD,
	 500,
	 SHOP_WORLD,
	 49},	// new world disorder
	{SHOP_WEIRD,
	 400,
	 SHOP_WORLD,
	 50},	// split level lunacy
	{SHOP_WEIRD,
	 400,
	 SHOP_WORLD,
	 51},	// nuclear zone
	{SHOP_WEIRD,
	 400,
	 SHOP_WORLD,
	 52},	// bouapha in wonderland
	{SHOP_WEIRD,
	 500,
	 SHOP_CHEAT,
	 CHEAT_CONFUSE},	// confusion cheat
	{SHOP_WEIRD,
	 1000,
	 SHOP_PLAYABLE,
	 PLAY_WIZ},	// play as wild wizard
	{SHOP_WEIRD,
	 500,
	 SHOP_CHEAT,
	 CHEAT_RAGE},	// max rage cheat

	 // adventure
	{SHOP_ADVENTURE,
	 400,
	 SHOP_WORLD,
	 53},	// field trip
	{SHOP_ADVENTURE,
	 300,
	 SHOP_WORLD,
	 54},	// field trip 2
	{SHOP_ADVENTURE,
	 1000,
	 SHOP_WORLD,
	 55},	// world of lunacy
	{SHOP_ADVENTURE,
	 200,
	 SHOP_WORLD,
	 56},	// drl halloween hill
	{SHOP_ADVENTURE,
	 100,
	 SHOP_WORLD,
	 57},	// excellent expedition
	{SHOP_ADVENTURE,
	 250,
	 SHOP_WORLD,
	 58},	// bouapha's quest
	{SHOP_ADVENTURE,
	 100,
	 SHOP_WORLD,
	 59},	// sleepier hollow
	{SHOP_ADVENTURE,
	 250,
	 SHOP_WORLD,
	 60},	// jungle quest
	{SHOP_ADVENTURE,
	 500,
	 SHOP_CHEAT,
	 CHEAT_KEYS},	// all keys cheat
	{SHOP_ADVENTURE,
	 3000,
	 SHOP_ABILITY,
	 ABIL_CANDLE},	// candle radar ability
	{SHOP_ADVENTURE,
	 150,
	 SHOP_MAJOR,
	 MAJOR_EDITOR},	// level editor!
	{SHOP_ADVENTURE,
	 800,
	 SHOP_MAJOR,
	 MAJOR_ARCADE2},	// dig man!

	 // yeti bauer
	{SHOP_ICE,
	 200,
	 SHOP_WORLD,
	 61},	// winter wacky 1 LK
	{SHOP_ICE,
	 200,
	 SHOP_WORLD,
	 62},	// winter wacky 1 RK
	{SHOP_ICE,
	 200,
	 SHOP_WORLD,
	 63},	// winter wacky 1 BP
	{SHOP_ICE,
	 400,
	 SHOP_WORLD,
	 64},	// winter wacky 2 BP
	{SHOP_ICE,
	 400,
	 SHOP_WORLD,
	 65},	// winter wacky 2 LK
	{SHOP_ICE,
	 400,
	 SHOP_WORLD,
	 66},	// winter wacky 2 TDM
	{SHOP_ICE,
	 300,
	 SHOP_WORLD,
	 67},	// seasons beatings
	{SHOP_ICE,
	 250,
	 SHOP_WORLD,
	 68},	// icy adventures
	{SHOP_ICE,
	 500,
	 SHOP_CHEAT,
	 CHEAT_FREEZE},	// freeze enemies cheat
	{SHOP_ICE,
	 500,
	 SHOP_MAJOR,
	 MAJOR_GALLERY2},	// premium gallery
	{SHOP_ICE,
	 1000,
	 SHOP_MAJOR,
	 MAJOR_EDPACK1},	// loonyland 2 editor pack

	 // kids we be
	{SHOP_KIDS,
	 150,
	 SHOP_WORLD,
	 69},	// happy ice cream land
	{SHOP_KIDS,
	 150,
	 SHOP_WORLD,
	 70},	// itsy bitsy world
	{SHOP_KIDS,
	 150,
	 SHOP_WORLD,
	 71},	// small world
	{SHOP_KIDS,
	 150,
	 SHOP_WORLD,
	 72},	// stockboy
	{SHOP_KIDS,
	 150,
	 SHOP_WORLD,
	 73},	// amazin spispopd
	{SHOP_KIDS,
	 250,
	 SHOP_WORLD,
	 74},	// luigi's
	{SHOP_KIDS,
	 250,
	 SHOP_WORLD,
	 75},	// toybox
	{SHOP_KIDS,
	 400,
	 SHOP_WORLD,
	 76},	// angelo's party pizza edition
	{SHOP_KIDS,
	 500,
	 SHOP_CHEAT,
	 CHEAT_HAMMERUP},	// max hammer cheat
	{SHOP_KIDS,
	 350,
	 SHOP_MAJOR,
	 MAJOR_BESTIARY},	// bestiary
	{SHOP_KIDS,
	 500,
	 SHOP_CHEAT,
	 CHEAT_NOSKID},	// no skid boots cheat
	{SHOP_KIDS,
	 2000,
	 SHOP_ABILITY,
	 ABIL_KEYCHAIN},	// keychain help ability
	{SHOP_KIDS,
	 1000,
	 SHOP_MAJOR,
	 MAJOR_QUIZ},	// quiz game - originally from ice store

	 // desert
	{SHOP_DESERT,
	 200,
	 SHOP_WORLD,
	 77},	// desert rats
	{SHOP_DESERT,
	 300,
	 SHOP_WORLD,
	 78},	// desert arena
	{SHOP_DESERT,
	 100,
	 SHOP_WORLD,
	 79},	// desert toils
	{SHOP_DESERT,
	 400,
	 SHOP_WORLD,
	 80},	// deserted desert dessert
	{SHOP_DESERT,
	 400,
	 SHOP_WORLD,
	 81},	// just deserts
	{SHOP_DESERT,
	 400,
	 SHOP_WORLD,
	 82},	// desert arena 2
	{SHOP_DESERT,
	 500,
	 SHOP_CHEAT,
	 CHEAT_WINLEVEL},	// win level cheat
	{SHOP_DESERT,
	 500,
	 SHOP_CHEAT,
	 CHEAT_SPEED},	// super speed cheat
	{SHOP_DESERT,
	 700,
	 SHOP_MAJOR,
	 MAJOR_THEATER},	// movie theater
	{SHOP_DESERT,
	 1000,
	 SHOP_ABILITY,
	 ABIL_MOVESHOOT},	// move and shoot ability
	{SHOP_DESERT,
	 1000,
	 SHOP_PLAYABLE,
	 PLAY_LUNACHIK},	// play as LunaChick
	 
	 //rocketshack
	{SHOP_GADGET,
	 500,
	 SHOP_CHEAT,
	 CHEAT_PRESTO},	// no skid boots cheat
	{SHOP_GADGET,
	 500,
	 SHOP_CHEAT,
	 CHEAT_SHIELD},	// shield cheat
	{SHOP_GADGET,
	 500,
	 SHOP_CHEAT,
	 CHEAT_SCANNER},	// scanner cheat
	{SHOP_GADGET,
	 400,
	 SHOP_WORLD,
	 83,},	// cyberia
	{SHOP_GADGET,
	 400,
	 SHOP_WORLD,
	 84},	// hamumu: the world
	{SHOP_GADGET,
	 400,
	 SHOP_WORLD,
	 85},	// lunatic research facility
	{SHOP_GADGET,
	 400,
	 SHOP_WORLD,
	 86},	// deja vu
	{SHOP_GADGET,
	 1000,
	 SHOP_MAJOR,
	 MAJOR_CARDBOOK},	// card book
	{SHOP_GADGET,
	 200,
	 SHOP_MAJOR,
	 MAJOR_ARCADE3},	// arcade game 3
	{SHOP_GADGET,
	 1000,
	 SHOP_ABILITY,
	 ABIL_SUPREME},	// move and shoot ability
	 
	 //whatsaburger
	{SHOP_RANDOM,
	 300,
	 SHOP_WORLD,
	 87},	// potluck
	{SHOP_RANDOM,
	 200,
	 SHOP_WORLD,
	 88},	// bouapha's world
	{SHOP_RANDOM,
	 200,
	 SHOP_WORLD,
	 89},	// oregon trail
	{SHOP_RANDOM,
	 300,
	 SHOP_WORLD,
	 90},	// urban world (NEW!)
	{SHOP_RANDOM,
	 400,
	 SHOP_WORLD,
	 91},	// loony stew
	{SHOP_RANDOM,
	 400,
	 SHOP_WORLD,
	 92},	// old factory
	{SHOP_RANDOM,
	 500,
	 SHOP_WORLD,
	 93},	// obscurity clearance
	{SHOP_RANDOM,
	 500,
	 SHOP_WORLD,
	 94},	// spamumu
	{SHOP_RANDOM,
	 1000,
	 SHOP_MAJOR,
	 MAJOR_LOCKERS},	// card book
	{SHOP_RANDOM,
	 1000,
	 SHOP_MAJOR,
	 MAJOR_GREEDY},	// greedy mode
	

	 // lockers
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 0},		// #1 - discount card #0
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},		// #2 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 1},		// #3 - discount card #1
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 0},		// #4 - 25 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_DISCO}, // #5 - disco mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 1},			// #6 - 75c
	{SHOP_LOCKERS,
	 0,
	 SHOP_GOAL,
	 68},			// #7 - goal #68
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 2},		// #8 - discount card #2
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 2},		// #9 - 1500 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_PLAYABLE,
	 PLAY_HAPPY}, // #10 - playas happy stick

	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_RASTER}, // #11 - TV mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 3},			// #12 - 5000 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_WORLD,
	 70},			// #13 - world: Bouapha: HUD Inspector
	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_VIRTUAL}, // #14 - virtual mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 4},			// #15 - 2222 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #16 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 3},			// #17 - discount card #3
	{SHOP_LOCKERS,
	 0,
	 SHOP_WORLD,
	 65},			// #18 - world: TV Lunacy
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #19 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 5},			// #20 - 250 coins

	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 6},			// #21 - 17 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_WORLD,
	 66},			// #22 - world: Blockbusters
	{SHOP_LOCKERS,
	 0,
	 SHOP_PLAYABLE,
	 PLAY_MYSTIC}, // #23 - kid mystic
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 4},			// #24 - discount card #4
	{SHOP_LOCKERS,
	 0,
	 SHOP_WORLD,
	 73},			// #25 - world: Sprungfield
	{SHOP_LOCKERS,
	 0,
	 SHOP_GOAL,
	 69},			// #26 - goal #69
	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_LUDICROUS}, // #27 - ludicrous mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 5},			// #28 - discount card #5
	{SHOP_LOCKERS,
	 0,
	 SHOP_GOAL,
	 29},			// #29 - gallery goal #29
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #30 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_GOAL,
	 59},			// #31 - gallery goal #59
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 7},			// #32 - 300 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 6},			// #33 - discount card #6
	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_REVERSE}, // #34 - reverse mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #35 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_SPLATTER},		// #36 - splatter mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #37 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 7},			// #38 - discount card #7
	{SHOP_LOCKERS,
	 0,
	 SHOP_WORLD,
	 17},			// #39 - world: Treasure Hunting
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 8},			// #40 - discount card #8

	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_MANIC},	// #41 - manic mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_ABILITY,
	 ABIL_HYPER}, 	// #42 - hyper mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 10},			// #43 - 42 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_PLAYABLE,
	 PLAY_LUNATIC},	// #44 - Dr. Lunatic
	{SHOP_LOCKERS,
	 0,
	 SHOP_DISCOUNT,
	 9},			// #45 - discount card #9
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 8},			// #46 - 2 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 6},			// #47 - XXXXXXXXXXXXXXXX
	{SHOP_LOCKERS,
	 0,
	 SHOP_MONEY,
	 9},			// #48 - 716 coins
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #49 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_MODE,
	 MODE_TEENY},		// #50 - teeny mode
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #51 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #52 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #53 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #54 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #55 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #56 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #57 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #58 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #59 - empty
	{SHOP_LOCKERS,
	 0,
	 SHOP_EMPTY,
	 0},			// #60 - empty

	 // the hunger sign
	{SHOP_HUNGER,
	 10000,
	 SHOP_DONATION,
	 0},
};

char worldFName[NUMBUILTINWORLDS][32]={
	//bones bats & beyond (17)
	"mansion.hsw",
	"hauntedh.hsw",
	"loonyhalloween.hsw",
	"halloweenBP.hsw",
	"halloweenMS.hsw",
	"halloweenPOP.hsw",
	"halloweenMH.hsw",
	"halloween2TDM.hsw",
	"halloween2BP.hsw",
	"halloween2LK.hsw",
	"halloween2RZ.hsw",
	"halloween2POP.hsw",
	"halloween2MH.hsw",
	"halloween3BP.hsw",
	"halloween3LK.hsw",
	"halloween3POP.hsw",
	"halloween3TDM.hsw",
	//sputnik's (7)
	"space.hsw",
	"spacebattle.hsw",
	"ez8675.hsw",
	"spacedout.hsw",
	"spaced.hsw",
	"starramble.hsw",
	"interluna.hsw",
	//pier 2 (8)
	"citanul.hsw",
	"islandadventure.hsw",
	"seahunt.hsw",
	"underwaterantics.hsw",
	"waterworld.hsw",
	"seaquest.hsw",
	"preperilcc.hsw",
	"pygmy.hsw",
	//cave & peril (5) - 32
	"tunnelvision.hsw",
	"cavernhavoc.hsw",
	"rockycliffs.hsw",
	"creepycastle.hsw",
	"middleearth.hsw",
	//jc piney (8) - 37
	"Woods.hsw",
	"wackywoods.hsw",
	"CreepyMeadows.hsw",
	"Looniesgooniesbaboonies.hsw",
	"wastedwls.hsw",
	"goofyg.hsw",
	"goofygrove2.hsw",
	"plains.hsw",
	//oddness depot (8) - 45
	"bunnyWorld.hsw",
	"loonylabs.hsw",
	"dimensionx.hsw",
	"doubletrouble.hsw",
	"disorder.hsw",
	"splitlevellunacy.hsw",
	"Nuclear.hsw",
	"wonderland.hsw",
	//gac (8) - 53
	"fieldtrip.hsw",
	"fieldtrip2.hsw",
	"final.hsw",
	"halloweenhill.hsw",
	"excellent.hsw",
	"BouaphaQ.hsw",
	"sleepier.hsw",
	"junglequest.hsw",
	//yeti bauer (8) - 61
	"winterwackinessbp.hsw",
	"WinterWackinessLK.hsw",
	"WinterWackinessRK.hsw",
	"winterwackiness2bp.hsw",
	"winterwackiness2lk.hsw",
	"WinterWackiness2TDM.hsw",
	"seasonsbeatings.hsw",
	"icyadv.hsw",
	//kids we be (8) - 69
	"HPPYICL.hsw",
	"ItsyBitsyW.hsw",
	"smallworld.hsw",
	"stockboy.hsw",
	"Amazin' Spispopd.hsw",
	"Luigis.hsw",
	"toys.hsw",
	"AngelosPizzaPartyEdition.hsw",
	//cleopatra's secret (6) - 77
	"desertrats.hsw",
	"desertarena.hsw",
	"deserttoils.hsw",
	"deserted.hsw",
	"justdeserts.hsw",
	"desertarena2.hsw",
	//rocketshack (4) - 83 
	"cyberia.hsw",
	"BouaphasNotSoTextAdventure.hsw",
	"Hamumu.hsw",
	"precur_FAC.hsw",
	//whatsaburger (8) - 87
	"potluck.hsw",
	"bouworld.hsw",
	"oregon.hsw",
	"urbania.hsw",
	"loonystew.hsw",
	"factory.hsw",
	"ObsurityClearance.hsw",
	"Spamumu.hsw"
};

static char shopName[][32]={
	"Bones, Bats & Beyond",
	"Sputnik's",
	"Pier 2",
	"Cave&Peril",
	"JCPiney",
	"Oddness Depot",
	"G.A.C.",
	"Yeti Bauer",
	"Kids 'We' Be",
	"Cleopatra's Secret",
	"RocketShack",
	"Whatsaburger"
};

static byte shopContents[13][4];

static byte itemCoords[]={
		0,0,0,0,		// lockers (not handled here)
		78,130,82,133,	// bonesbats&beyond
		150,131,154,135, // sputnik's
		125,131,131,134, // pier 2
		183,62,187,66,	// cave&peril
		182,14,185,16,	// JCPiney
		202,131,207,134, // oddness depot
		173,130,177,134, // GAC
		103,131,106,134, // yeti bauer
		183,84,189,89,	// kids we be
		191,37,196,40,	// cleopatra
		208,61,212,65,	// rocketshack
		209,92,213,96,	// whatsaburger
	};

static byte modeToToggle;

byte CanPlayWorld(const char *fname)
{
	int i;

	for(i=0;i<NUMBUILTINWORLDS;i++)
	{
		if(!strcasecmp(fname,worldFName[i]))
		{
			// this is one of the built in worlds
			// so find which purchase item relates to it, and see if it's been bought
			return ItemPurchased(SHOP_WORLD,i);
		}
	}

	// if it didn't match any of the built-in names, it's an add-on, so it's okay
	return 1;
}

byte AllOfTypePurchased(byte type)
{
	int j;

	for(j=0;j<NUMSHOPITEMS;j++)
	{
		if(shop[j].type==type && !(profile.progress.purchase[j]&SIF_BOUGHT))
		{
			return 0;
		}
	}
	return 1;	// they've all been bought!
}

byte AllLockersOpen(void)
{
	int j;

	for(j=0;j<NUMSHOPITEMS;j++)
	{
		if(shop[j].shop==SHOP_LOCKERS && !(profile.progress.purchase[j]&SIF_BOUGHT))
		{
			return 0;
		}
	}
	return 1;	// they've all been opened
}

byte AllPurchased(void)
{
	int j;

	for(j=0;j<NUMSHOPITEMS;j++)
	{
		if(!(profile.progress.purchase[j]&SIF_BOUGHT))
		{
			return 0;
		}
	}
	return 1;	// they've all been bought!
}

float ShopPercent(void)
{
	int j;
	int bought;

	bought=0;
	for(j=0;j<NUMSHOPITEMS;j++)
		if(profile.progress.purchase[j]&SIF_BOUGHT)
			bought++;

	return ((float)bought*100.0f/(float)NUMSHOPITEMS);
}

byte ItemPurchased(byte type,byte num)
{
	return ((profile.progress.purchase[ShopItemNumber(type,num)]&SIF_BOUGHT)>0);	// true if you've bought it
}

byte ShopItemNumber(byte type,byte num)
{
	int j;

	for(j=0;j<NUMSHOPITEMS;j++)
	{
		if(shop[j].type==type && shop[j].item==num)
		{
			// here it is!
			return j;
		}
	}
	return 255;	// if you couldn't find that item at all... then how about #255
}

byte ItemsInShop(byte shopNum)
{
	int j,num;

	num=0;
	for(j=0;j<NUMSHOPITEMS;j++)
	{
		if(shop[j].shop==shopNum && (profile.progress.purchase[j]&SIF_AVAILABLE))
		{
			num++;
		}
	}

	return num;
}

byte GetRandomItemFromShop(byte shopNum,byte invalid1,byte invalid2,byte invalid3)
{
	int i;

	while(1)
	{
		i=Random(NUMSHOPITEMS);
		if(shop[i].shop==shopNum && (profile.progress.purchase[i]&SIF_AVAILABLE) && i!=invalid1 && i!=invalid2 && i!=invalid3)
			return i;
	}
	return 0;	// will never occur, just here to prevent warning
}

void AddMapShopItem(Map *map,byte item,byte x,byte y,byte *itmnum)
{
	if(item!=255)
		map->map[x+y*map->width].item=itmnum[shop[item].type];
	else
		map->map[x+y*map->width].item=ITM_NONE;
}

void SetupShops(Map *map)
{
	int i,count,x,y;
	byte itm[4];
	byte itmnum[10];
	byte lockStart;
	char statname[16];

	i=profile.progress.purchase[ShopItemNumber(SHOP_DONATION,0)];
	if(i>0)
		sprintf(statname,"bstatue%d",i+1);
	else
		strcpy(statname,"bstatue");

	map->map[87+49*map->width].item=FindItemByName(statname);

	itmnum[SHOP_WORLD]=FindItemByName("shopworld");
	itmnum[SHOP_CHEAT]=FindItemByName("shopcheat");
	itmnum[SHOP_ABILITY]=FindItemByName("shopabil");
	itmnum[SHOP_MAJOR]=FindItemByName("shopmajor");
	itmnum[SHOP_GOAL]=FindItemByName("shoppic");
	itmnum[SHOP_PLAYABLE]=FindItemByName("shopplay");

	for(i=1;i<13;i++)	// go through each shop, not counting the lockers
	{
		count=ItemsInShop(i);
		if(count>3)
		{
			itm[0]=GetRandomItemFromShop(i,255,255,255);
			itm[1]=GetRandomItemFromShop(i,itm[0],255,255);
			itm[2]=GetRandomItemFromShop(i,itm[0],itm[1],255);
			itm[3]=GetRandomItemFromShop(i,itm[0],itm[1],itm[2]);
		}
		else
		{
			itm[0]=itm[1]=itm[2]=itm[3]=255;
			if(count>=1)
				itm[0]=GetRandomItemFromShop(i,255,255,255);
			if(count>=2)
				itm[1]=GetRandomItemFromShop(i,itm[0],255,255);
			if(count>=3)
				itm[2]=GetRandomItemFromShop(i,itm[0],itm[1],255);
		}

		// now we have our list of items to install
		AddMapShopItem(map,itm[0],itemCoords[i*4+0],itemCoords[i*4+1],itmnum);
		AddMapShopItem(map,itm[1],itemCoords[i*4+0],itemCoords[i*4+3],itmnum);
		AddMapShopItem(map,itm[2],itemCoords[i*4+2],itemCoords[i*4+1],itmnum);
		AddMapShopItem(map,itm[3],itemCoords[i*4+2],itemCoords[i*4+3],itmnum);
		shopContents[i][0]=itm[0];
		shopContents[i][1]=itm[1];
		shopContents[i][2]=itm[2];
		shopContents[i][3]=itm[3];
	}

	// now open/mess with the lockers
	lockStart=ShopItemNumber(SHOP_DISCOUNT,0);	// this is what's in locker #0, so this is where the locker list starts
	x=124;
	y=9;
	for(i=0;i<60;i++)
	{
		int j = lockers[i];
		//std::cout << "Locker #" << i << " w/ ID " << j << "\n"; 
		if(profile.progress.purchase[j+lockStart]&SIF_BOUGHT)
		{
			// you own this one
			if(shop[j+lockStart].type==SHOP_MODE)
			{
				if(profile.progress.purchase[j+lockStart]&SIF_ACTIVE)
				{
					map->map[x+y*map->width].floor=369;
					map->map[x+y*map->width].wall=371;
				}
				else
				{
					map->map[x+y*map->width].floor=370;
					map->map[x+y*map->width].wall=372;
				}
			}
			else
			{
				map->map[x+y*map->width].floor=373;
				map->map[x+y*map->width].wall=374;
			}
		}
		x+=3;
		if((i%12)==11)
		{
			x=124;
			y+=4;
		}
	}

	if(ItemPurchased(SHOP_MAJOR,MAJOR_BESTIARY))
	{
		map->ContiguousItemChange(71,8,ITM_NONE,0);
	}
	if(ItemPurchased(SHOP_MAJOR,MAJOR_THEATER))
	{
		map->ContiguousItemChange(121,33,ITM_NONE,0);
	}
	if(ItemPurchased(SHOP_MAJOR,MAJOR_GALLERY))
	{
		map->ContiguousItemChange(120,40,ITM_NONE,0);
	}
	if(ItemPurchased(SHOP_MAJOR,MAJOR_ARCADE))
	{
		map->ContiguousItemChange(56,35,ITM_NONE,0);
	}
	if(ItemPurchased(SHOP_MAJOR,MAJOR_ARCADE2))
	{
		map->ContiguousItemChange(61,44,ITM_NONE,0);
	}
	if(ItemPurchased(SHOP_MAJOR,MAJOR_QUIZ))
	{
		map->ContiguousItemChange(55,40,ITM_NONE,0);
	}
	if(ItemPurchased(SHOP_MAJOR,MAJOR_GALLERY2))
	{
		map->ContiguousItemChange(148,63,ITM_NONE,0);
	}

	if(profile.progress.goal[86])
		map->map[11+20*map->width].item=0;
	if(profile.progress.goal[87])
		map->map[13+113*map->width].item=0;
}

void DefaultShopAvailability(void)
{
	int i;

	for(i=0;i<NUMSHOPITEMS;i++)
	{
		if(shop[i].type==SHOP_CHEAT || shop[i].type==SHOP_DONATION)
			profile.progress.purchase[i]=0;	// not available!
		else
			profile.progress.purchase[i]=SIF_AVAILABLE;

		if(shop[i].shop==SHOP_LOCKERS)	// all locker items are available, deal with it!
			profile.progress.purchase[i]=SIF_AVAILABLE;
	}
}

const char *GetAbilityShortDesc(byte w)
{
	switch(w)
	{
		case ABIL_RAGE:
			return "Rage Power";
		case ABIL_BRAIN:
			return "Brain Radar";
		case ABIL_CANDLE:
			return "Candle Radar";
		case ABIL_MOVESHOOT:
			return "Move n' Shoot";
		case ABIL_KEYCHAIN:
			return "Keychain Helper";
	}
	return "Unknown!?";
}

///------------------------------------------

static char shopTxt[256];
static byte buying,buyMode;
static byte shopSize;
static byte cursor;

void OpenLocker(int x,int y)
{
	int lock,xx;
	char modeName[][16]={"Disco","TV","Ludicrous","Subliminal","Splatter","Manic","Handheld","Virtual"};
	char modeDesc[][64]={
		"Enjoy the hideous discoloration of disco lighting!",
		"Like playing on your old Atari!",
		"This makes one hit all it takes to finish you off!",
		"!!yebo dna raeH",
		"Make things much messier in combat!",
		"No time for a long game of Dr. L?  Then speed things up!",
		"No, you can't take it with you!",
		"Don't stare for too long!"
	};

	x-=124;
	y-=9;
	x/=3;
	y/=4;

	xx = x+y*12;	// x=locker ##
	lock = xx;
	xx = lockers[xx] + ShopItemNumber(SHOP_DISCOUNT,0);
	
	if(!(profile.progress.purchase[x]&SIF_BOUGHT))
	{
		if(profile.progress.loonyKeys-profile.progress.loonyKeysUsed<=0)
		{
			MakeNormalSound(SND_TURRETBZZT);
			return;
		}
		profile.progress.loonyKeysUsed++;
	}

	if(shop[xx].type==SHOP_MODE)
	{
		if(profile.progress.purchase[xx]&SIF_BOUGHT)
		{
			if(profile.progress.purchase[xx]&SIF_ACTIVE)
				sprintf(shopTxt,"You close Locker #%i, deactivating %s Mode.",lock+1,modeName[shop[xx].item]);
			else
				sprintf(shopTxt,"You open Locker #%i, activating %s Mode. (%s)",lock+1,modeName[shop[xx].item],modeDesc[shop[xx].item]);
			RestoreGameplayGfx();
		}
		else
		{
			sprintf(shopTxt,"You open Locker #%i to reveal..... %s Mode! %s Bump the locker again to deactivate it.",lock+1,modeName[shop[xx].item],modeDesc[shop[xx].item]);
			MakeNormalSound(SND_LOCKER);
			RestoreGameplayGfx();
		}
		modeToToggle=xx;
	}
	else
	{
		if(profile.progress.purchase[xx]&SIF_BOUGHT)
			sprintf(shopTxt,"You've already opened Locker #%i, and found: ",lock+1);
		else
		{
			sprintf(shopTxt,"You open Locker #%i to reveal..... ",lock+1);
			MakeNormalSound(SND_LOCKER);
		}

		switch(shop[xx].type)
		{
			case SHOP_DISCOUNT:
				strcat(shopTxt,"A Discount Card for 10% off of all purchases at ");
				strcat(shopTxt,shopName[shop[xx].item]);
				strcat(shopTxt,"!  Go shop 'til you drop!");
				break;
			case SHOP_EMPTY:
				strcat(shopTxt,"Nothing!! ABSOLUTELY NOTHING!!!");
				break;
			case SHOP_MONEY:
				sprintf(&shopTxt[strlen(shopTxt)],"%d coins!  Don't spend 'em all in one place!",moneyAmts[shop[xx].item]);
				if(!(profile.progress.purchase[xx]&SIF_BOUGHT))
					profile.progress.totalCoins+=moneyAmts[shop[xx].item];
				break;
			case SHOP_PLAYABLE:
				if(shop[xx].item==PLAY_HAPPY)
					strcat(shopTxt,"The playable character Happy Stick Man!  You can choose to play as him on the Profile menu.");
				else
					strcat(shopTxt,"The playable character Dr. Lunatic!  You can choose to play as him on the Profile menu.");
				break;
			case SHOP_GOAL:
				strcat(shopTxt,"A lovely piece of artwork to view in the Mall Gallery!");
				break;
			case SHOP_WORLD:
				char tmp[32],tmp2[32],tmp3[64];

				sprintf(tmp3,"worlds/%s",worldFName[shop[xx].item]);
				GetWorldName(tmp3,tmp,tmp2);

				sprintf(&shopTxt[strlen(shopTxt)],"The secret bonus world \"%s\" by %s!  Play it from the World Select screen!",tmp,tmp2);
				break;
		}
	}
	profile.progress.purchase[xx]|=SIF_BOUGHT;
	SetupShops(curMap);
	SendMessageToGame(MSG_SHOPNOW,0);
	shopSize=0;
	buying=255;
	cursor=0;
	buyMode=1;
	GetTaps();
	GetArrowTaps();
	if(shop[xx].type==SHOP_GOAL)
		CompleteGoal(shop[xx].item);
	GoalPurchase();
}

void HungerSign(void)
{
	SendMessageToGame(MSG_SHOPNOW,0);
	shopSize=0;
	buying=ShopItemNumber(SHOP_DONATION,0);
	if(profile.progress.purchase[buying]==7)
	{
		strcpy(shopTxt,"Thank you for your donations!  With your help, we've completely "
					   "fixed world hunger!  On the minus side, this is only a video game.");
		buying=255;
		buyMode=1;
	}
	else
	{
		strcpy(shopTxt,"Would you care to donate 10,000 coins to world hunger relief?");
		buyMode=0;
	}
	modeToToggle=0;
	cursor=0;
	GetTaps();
	GetArrowTaps();
}

void InitShopping(int x,int y)
{
	int i;
	char tmp[32],tmp2[32],tmp3[64];

	modeToToggle=0;

	if(x==11 && y==20)
	{
		CompleteGoal(86);
		SetupShops(curMap);
	}
	if(x==13 && y==113)
	{
		CompleteGoal(87);
		SetupShops(curMap);
	}
	if(x==59 && y==6)
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		Bestiary(GetDisplayMGL());
		RestoreGameplayGfx();
		return;
	}
	if(x==135 && y==33)
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		Theater(GetDisplayMGL());
		RestoreGameplayGfx();
		return;
	}
	if(y==33 && (x==57 || x==59 || x==61))
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		if(x==59 && y==33)
			Arcade(GetDisplayMGL(),2);
		else
			Arcade(GetDisplayMGL(),0);
		RestoreGameplayGfx();
		return;
	}
	if(x>=62 && x<=65 && y>=41 && y<=44)
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		if(x==62 && y==44)
			Arcade(GetDisplayMGL(),2);
		else
			Arcade(GetDisplayMGL(),1);
		RestoreGameplayGfx();
		return;
	}
	if(x==39 && y==60)
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		Moron(GetDisplayMGL());
		player.var[7]=1;	// to clear the mumbles and change the sign
		RestoreGameplayGfx();
		return;
	}
	if(x>=124 && y>=9 && x<=157 && y<=30)
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		OpenLocker(x,y);
		RestoreGameplayGfx();
		return;
	}
	if(x==87 && y==54)
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		HungerSign();
		return;
	}

	if(GalleryBump(x,y))
	{
		goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		goodguy->dx=0;
		goodguy->dy=0;
		ShowGalleryPic();
		RestoreGameplayGfx();
		return;
	}
	shopSize=0;
	buying=255;
	cursor=0;
	buyMode=0;

	for(i=1;i<13;i++)
	{
		if(itemCoords[i*4]==x)
		{
			if(itemCoords[i*4+1]==y)
			{
				// upper left
				buying=shopContents[i][0];
			}
			else if(itemCoords[i*4+3]==y)
			{
				// lower left
				buying=shopContents[i][1];
			}
		}
		else if(itemCoords[i*4+2]==x)
		{
			if(itemCoords[i*4+1]==y)
			{
				// upper right
				buying=shopContents[i][2];
			}
			else if(itemCoords[i*4+3]==y)
			{
				// lower right
				buying=shopContents[i][3];
			}
		}
	}

	if(buying!=255)
	{
		SendMessageToGame(MSG_SHOPNOW,0);

		strcpy(shopTxt,"Would you like to buy ");
		switch(shop[buying].type)
		{
			case SHOP_WORLD:
				sprintf(tmp3,"worlds/%s",worldFName[shop[buying].item]);
				GetWorldName(tmp3,tmp,tmp2);
				sprintf(&shopTxt[strlen(shopTxt)],"Access to the world \"%s\" by %s",tmp,tmp2);
				break;
			case SHOP_CHEAT:
				sprintf(&shopTxt[strlen(shopTxt)],"the  \"%s\" Cheat",CheatName(shop[buying].item));
				break;
			case SHOP_PLAYABLE:
				strcpy(&shopTxt[strlen(shopTxt)],"a Mysterious Playable Character");
				break;
			case SHOP_ABILITY:
				sprintf(&shopTxt[strlen(shopTxt)],"a Frozen Yogurt Of Fury (%s Flavor)",GetAbilityShortDesc(shop[buying].item));
				break;
			case SHOP_MAJOR:
				switch(shop[buying].item)
				{
					case MAJOR_THEATER:
						strcpy(&shopTxt[strlen(shopTxt)],"Magic Theater Tickets");
						break;
					case MAJOR_GALLERY:
						strcpy(&shopTxt[strlen(shopTxt)],"An Art Gallery Season Pass");
						break;
					case MAJOR_CHEATMENU:
						strcpy(&shopTxt[strlen(shopTxt)],"The Cheat Menu");
						break;
					case MAJOR_RECORDBOOK:
						strcpy(&shopTxt[strlen(shopTxt)],"A Handy Record Book");
						break;
					case MAJOR_EDITOR:
						strcpy(&shopTxt[strlen(shopTxt)],"The World Editor");
						break;
					case MAJOR_QUIZ:
						strcpy(&shopTxt[strlen(shopTxt)],"The Game Show \"Who Doesn't Want To Be A Moron?\"");
						break;
					case MAJOR_ARCADE:
						strcpy(&shopTxt[strlen(shopTxt)],"The Arcade Game \"Zappo Raiders\"");
						break;
					case MAJOR_ARCADE2:
						strcpy(&shopTxt[strlen(shopTxt)],"The Arcade Game \"Dig Man IV\"");
						break;
					case MAJOR_BESTIARY:
						strcpy(&shopTxt[strlen(shopTxt)],"A Zoo Pass");
						break;
					case MAJOR_LOCKERS:
						strcpy(&shopTxt[strlen(shopTxt)],"Premium Locker Storage");
						break;
					case MAJOR_GALLERY2:
						strcpy(&shopTxt[strlen(shopTxt)],"An Art Gallery Premium Pass");
						break;
					case MAJOR_CARDBOOK:
						strcpy(&shopTxt[strlen(shopTxt)],"The HamSandwich Card Collector");
						break;
					case MAJOR_GREEDY:
						strcpy(&shopTxt[strlen(shopTxt)],"Arcade Mode");
						break;
					case MAJOR_EDPACK1:
						strcpy(&shopTxt[strlen(shopTxt)],"Loonyland 2 Content Pack of Capitalism");
						break;
					case MAJOR_ARCADE3:
						strcpy(&shopTxt[strlen(shopTxt)],"The Arcade Game \"Cave Zoomer\"");
						break;
				}
				break;
		}
		if(ItemPurchased(SHOP_DISCOUNT,shop[buying].shop-1))
			sprintf(&shopTxt[strlen(shopTxt)]," for %d Coins? (10%% off!)",shop[buying].cost*9/10);
		else
			sprintf(&shopTxt[strlen(shopTxt)]," for %d Coins?",shop[buying].cost);
	}
	GetTaps();
	GetArrowTaps();
}

void SetObtainText(void)
{
	char tmp[32],tmp2[32],tmp3[64];

	switch(shop[buying].type)
	{
		case SHOP_DONATION:
			strcpy(shopTxt,"Thank you so very much for your generous donation!");
			break;
		case SHOP_WORLD:
			sprintf(tmp3,"worlds/%s",worldFName[shop[buying].item]);
			GetWorldName(tmp3,tmp,tmp2);
			sprintf(shopTxt,"%s by %s has now been added to your available World list!",tmp,tmp2);
			break;
		case SHOP_CHEAT:
			sprintf(shopTxt,"\"%s\" has been added to your cheat list!",CheatName(shop[buying].item));
			break;
		case SHOP_ABILITY:
			switch(shop[buying].item)
			{
				case ABIL_BRAIN:
					strcpy(shopTxt,"If you fail to get a brain for 30 seconds, Brain Radar will point you to the nearest one!  If it is too obnoxious,"
								   " turn it off in the Profile menu.");
					profile.brainRadar=1;
					break;
				case ABIL_CANDLE:
					strcpy(shopTxt,"If you fail to get a candle for 30 seconds, Candle Radar will point you to the nearest one!  You can turn this off "
								   "in the Profile menu if your brain can't take it anymore.");
					profile.candleRadar=1;
					break;
				case ABIL_RAGE:
					strcpy(shopTxt,"Your Rage Gauge fills up as you smash badguys. Whenever it's higher than your current health, press both "
								   "fire buttons at the same time to unleash your rage! Effects vary depending on your "
								   "current Special Weapon.");
					break;
				case ABIL_MOVESHOOT:
					strcpy(shopTxt,"You can now throw Hammers while moving!  You move much slower while throwing Hammers, but at least it doesn't stop you "
								   "anymore.  You can turn this off in the Profile menu if it drives you mad.");
					profile.moveNShoot=1;
					break;
				case ABIL_KEYCHAIN:
					strcpy(shopTxt,"Any level you have passed, but not collected a keychain from, will show a special symbol in the hub level.  Watch for it!");
					break;
			}
			break;
		case SHOP_PLAYABLE:
			switch(shop[buying].item)
			{
				case PLAY_HAPPY:
					strcpy(shopTxt,"You can now choose to play as Happy Stick Man from the Profile menu!");
					break;
				case PLAY_LUNATIC:
					strcpy(shopTxt,"You can now choose to play as Dr. Lunatic from the Profile menu!");
					break;
				case PLAY_MECHA:
					strcpy(shopTxt,"You can now choose to play as MechaBouapha from the Profile menu!");
					break;
				case PLAY_SHROOM:
					strcpy(shopTxt,"You can now choose to play as Stupid Shroom from the Profile menu! ... If you really want to.");
					break;
				case PLAY_LUNACHIK:
					strcpy(shopTxt,"You can now choose to play as LunaChick from the Profile menu!");
					break;
			}
			break;
		case SHOP_MAJOR:
			switch(shop[buying].item)
			{
				case MAJOR_THEATER:
					strcpy(shopTxt,"Please enjoy our Unoplex theater over on the east end of the mall!");
					break;
				case MAJOR_GALLERY:
					strcpy(shopTxt,"Come visit the Mall Gallery any time you like - it's in the southeast corner of the mall.");
					break;
				case MAJOR_CHEATMENU:
					strcpy(shopTxt,"Haha cheater! You got an empty cheat menu! Now you need to buy cheats from our stores.  If "
						           "you cheat on a level, you get zero points and zero coins for it. The Cheat Menu is available when you pause the game.");
					break;
				case MAJOR_RECORDBOOK:
					strcpy(shopTxt,"Check out your new detailed statistics from the Profile menu!");
					break;
				case MAJOR_EDITOR:
					strcpy(shopTxt,"Unleash your creative juices with the Dr. Lunatic World Editor! You'll find it on the game's main menu.");
					break;
				case MAJOR_QUIZ:
					strcpy(shopTxt,"Test your Dr. Lunatic knowledge in our fabulous quiz game - and win prizes!  It's located in our arcade, on the west end of the mall.");
					break;
				case MAJOR_ARCADE:
					strcpy(shopTxt,"Test your arcade skills in this fantastic shooter game!  Located in our arcade, on the west end of the mall.");
					break;
				case MAJOR_ARCADE2:
					strcpy(shopTxt,"Dig out diamonds and battle vicious ants armed only with a water cannon!  Try it in our arcade, on the west end of the mall.");
					break;
				case MAJOR_BESTIARY:
					strcpy(shopTxt,"Please enjoy our conveniently located Mall Zoo, in the northwest corner of our mall!");
					break;
				case MAJOR_LOCKERS:
					strcpy(shopTxt,"You can now access Lockers #37-#60. They look the same as our regular lockers, but contain cooler items!");
					break;
				case MAJOR_GALLERY2:
					strcpy(shopTxt,"Your art pass has been upgraded - now you can see the new art!");
					break;
				case MAJOR_CARDBOOK:
					strcpy(shopTxt,"Love collecting Monster Cards? Check your progress here! It provides fun flavor data of each monster as well."
								   " You'll find it on the game's main menu.");
					break;
				case MAJOR_GREEDY:
					strcpy(shopTxt,"In this game mode, luck and money are everything. You'll find it on the game's main menu.");
					break;
			}
			break;
	}
}

void BuyItem(void)
{
	int i;
	word cost;

	if(shop[buying].shop<13 && shop[buying].shop>0 && ItemPurchased(SHOP_DISCOUNT,shop[buying].shop-1))
		cost=shop[buying].cost*9/10;
	else
		cost=shop[buying].cost;

	buyMode=1;
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=cost)
	{
		profile.progress.coinsSpent+=cost;
		SetObtainText();
		if(shop[buying].type==SHOP_DONATION)
		{
			profile.progress.purchase[buying]++;
			if(profile.progress.purchase[buying]==7)	// solved world hunger!
			{
				CompleteGoal(88);
				strcpy(shopTxt,"With your incredible contributions, we've actually managed to wipe out world hunger!  "
					"Thanks, dude!  Let's celebrate with a packet of our new product, Soylent Orange!");
				MakeNormalSound(SND_VICTORY);
				char statue[] = "statue.flc";
				ShowImageOrFlic(statue,1,0);
			}
		}
		else
		{
			profile.progress.purchase[buying]&=(~SIF_AVAILABLE);
			profile.progress.purchase[buying]|=SIF_BOUGHT;
			MakeNormalSound(SND_BUYITEM);
			if(shop[buying].type==SHOP_MAJOR && shop[buying].item==MAJOR_CHEATMENU)
			{
				// if you bought the cheat menu, cheats are now buyable
				for(i=0;i<NUMSHOPITEMS;i++)
				{
					if(shop[i].type==SHOP_CHEAT)
						profile.progress.purchase[i]|=SIF_AVAILABLE;
				}
			}
		}

		SetupShops(curMap);
	}
	else
	{
		strcpy(shopTxt,"Hey buddy, you don't have enough coins to buy that!");
	}
	GoalPurchase();
}

byte UpdateShopping(MGLDraw *mgl)
{
	byte c;

	goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	goodguy->dx=0;
	goodguy->dy=0;

	if(shopSize<150)
		shopSize+=8;

	if(mgl->LastKeyPressed()==27)
		return 0;	// ESC to cancel shopping

	c=GetTaps()|GetArrowTaps();

	if(buyMode==0)
	{
		if(c&CONTROL_LF)
			cursor=0;
		if(c&CONTROL_RT)
			cursor=1;

		if(c&CONTROL_B1)
		{
			if(cursor==1)
				BuyItem();
			else
				return 0;
		}
	}
	else
	{
		if(c&CONTROL_B1)
		{
			if(modeToToggle)
			{
				profile.progress.purchase[modeToToggle]^=SIF_ACTIVE;
				if(shop[modeToToggle].item==MODE_TEENY && (profile.progress.purchase[modeToToggle]&SIF_ACTIVE))
				{
					GetDisplayMGL()->LoadBMP("graphics/gamepal.bmp");
					GetDisplayMGL()->Flip();
				}
				SetupShops(curMap);
			}
			return 0;
		}
	}

	return 1;
}

void RenderShopButton(byte on,int x,int y,int wid,const char *txt,MGLDraw *mgl)
{
	if(on)
	{
		mgl->Box(x,y,x+wid,y+19,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+19-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+19,32+16);

	PrintGlow(x+2,y+3,txt,0,2);
}

void RenderShopping(MGLDraw *mgl)
{
	mgl->FillBox(320-shopSize,240-shopSize/2,320+shopSize,240+shopSize/2,32*1+4);

	if(shopSize>2)
		mgl->Box(320-shopSize+1,240-shopSize/2+1,320+shopSize-1,240+shopSize/2-1,32*1+16);

	if(buyMode==0)
	{
		if(shopSize>=150)
		{
			PrintGlowRect(320-shopSize+4,240-shopSize/2+4,320+shopSize-4,240+shopSize/2-4,18,shopTxt,2);

			RenderShopButton((cursor==0),320-shopSize+10,240+shopSize/2-27,95,"No Thanks!",mgl);
			RenderShopButton((cursor==1),320+shopSize-105,240+shopSize/2-27,95,"Yes Please!",mgl);
		}
	}
	else
	{
		if(shopSize>=150)
		{
			PrintGlowRect(320-shopSize+4,240-shopSize/2+4,320+shopSize-4,240+shopSize/2-4,18,shopTxt,2);
			RenderShopButton(1,320-48,240+shopSize/2-27,95,"Thank you!",mgl);
		}
	}
}
