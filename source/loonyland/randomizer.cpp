#include "randomizer.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "plasma.h"
#include "appdata.h"
#include <filesystem>
//#include <bits/stdc++.h>
#include "map.h"
#include <set>
#include <vector>
#include "quest.h"
#include <iostream>
#include <random>
#include <fstream>
#include "ioext.h"

static byte cursor;
static byte oldc;
static dword oldBtn;
static byte optMode;
static std::string seed;
std::set<int> ownedItems;

unsigned seed2 = time(0);
auto rng = std::default_random_engine(seed2);

#define MAX_SEED_LENGTH 32
#define R_NUM_LOCATIONS 105

location basic_locations[R_NUM_LOCATIONS] = {
	{false, 0, 194, 5, 24, 25, "Swamp Mud Path", [](std::set<int> inv) { return inv.count(VAR_BOOTS); }},
	{false, 0, 187, 112, 11, 12, "Bog Beast Home", [](std::set<int> inv) { return true; }},
	{false, 0, 2, 46, 69, 70, "Rocky Cliffs below Upper Caverns", [](std::set<int> inv) { return HaveAnyBigGem(inv); }},
	{false, 0, 131, 26, 10, 9, "Sapling Shrine", [](std::set<int> inv) { return inv.count(VAR_BOOTS); }},
	{false, 0, 83, 145, 33, 34, "Terror Glade", [](std::set<int> inv) { return true; }},
	{false, 0, 1, 73, 47, 48, "Rocky Cliffs Vine", [](std::set<int> inv) { return inv.count(VAR_FERTILIZER); }},
	{false, 0, 30, 39, 73, 74, "Rocky Cliffs Grand Pharoh", [](std::set<int> inv) { return HaveAnyBigGem(inv); }},
	{false, 0, 69, 59, 66, 67, "Rocky Cliffs Rock Corner", [](std::set<int> inv) { return HaveAnyBigGem(inv) && inv.count(VAR_WEAPON); }},
	{false, 0, 115, 70, 18, 19, "Mushroom outside town", [](std::set<int> inv) { return true; }},
	{false, 0, 159, 85, 20, 21, "mushroom east", [](std::set<int> inv) { return true; }},
	{false, 0, 117, 33, 22, 23, "Top left mushroom spot", [](std::set<int> inv) { return true; }},
	{false, 0, 168, 98, 28, 29, "mushroom south east", [](std::set<int> inv) { return true; }},
	{false, 0, 197, 161, 56, 57, "East Woods", [](std::set<int> inv) { return true; }},
	{false, 0, 16, 61, 49, 50, "Rocky Cliffs Ledge", [](std::set<int> inv) { return HaveAnyBigGem(inv); }},
	{false, 0, 28, 10, 63, 65, "Rocky Cliffs Peak", [](std::set<int> inv) { return HaveAnyBigGem(inv); }},
	{false, 0, 134, 182, 97, 98, "Cat Tree", [](std::set<int> inv) { return true; }},
	{false, 2, 27, 16, 1, 2, "Backroom", [](std::set<int> inv) { return true; }},
	{false, 2, 22, 7, 3, 4, "Bedroom", [](std::set<int> inv) { return true; }},
	{false, 3, 10, 8, 1, 3, "Barrel Maze", [](std::set<int> inv) { return true; }},
	{false, 4, 19, 5, 1, 2, "Top Door", [](std::set<int> inv) { return inv.count(VAR_SKULLKEY); }},
	{false, 4, 39, 38, 3, 4, "Post Room", [](std::set<int> inv) { return true; }},
	{false, 4, 33, 06, 9, 10, "Window Drip", [](std::set<int> inv) { return true; }},
	{false, 4, 3, 33, 7, 8, "Green room", [](std::set<int> inv) { return true; }},
	{false, 4, 25, 48, 5, 11, "Arena", [](std::set<int> inv) { return true; }},
	{false, 4, 2, 58, 17, 18, "Kill Wall", [](std::set<int> inv) { return true; }},
	{false, 5, 53, 41, 0, 1, "Swampdog Door", [](std::set<int> inv) { return inv.count(VAR_PUMPKINKEY); }},
	{false, 5, 57, 37, 13, 14, "Scribble Wall", [](std::set<int> inv) { return true; }},
	{false, 5, 26, 25, 4, 5, "Tiny Passage", [](std::set<int> inv) { return true; }},
	{false, 5, 50, 26, 7, 8, "fire frogs", [](std::set<int> inv) { return true; }},
	{false, 5, 17, 64, 9, 10, "Torch Island", [](std::set<int> inv) { return inv.count(VAR_BOOTS); }},
	{false, 5, 44, 49, 11, 12, "Small Room", [](std::set<int> inv) { return true; }},
	{false, 6, 32, 45, 2, 3, "Scratch Wall", [](std::set<int> inv) { return inv.count(VAR_BOOTS); }},
	{false, 6, 44, 37, 15, 17, "Bat Mound", [](std::set<int> inv) { return inv.count(VAR_BOOTS) && inv.count(VAR_BATKEY); }},
	{false, 6, 3, 7, 23, 24, "Stair room", [](std::set<int> inv) { return inv.count(VAR_BOOTS); }},
	{false, 6, 43, 58, 18, 19, "Rock Prison", [](std::set<int> inv) { return inv.count(VAR_BOOTS) && inv.count(VAR_WEAPON); }},
	{false, 7, 11, 16, 1, 2, "Tiny Cabin", [](std::set<int> inv) { return inv.count(VAR_SKULLKEY); }},
	{false, 8, 19, 15, 3, 4, "Bedside ", [](std::set<int> inv) { return true; }},
	{false, 10, 2, 44, 2, 3, "Pumpkin Door", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_PUMPKINKEY); }},
	{false, 10, 5, 36, 4, 5, "Maze", [](std::set<int> inv) { return HaveLightSource(inv); }},
	{false, 11, 51, 82, 11, 12, "Big Closed Room", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_BOOTS); }},
	{false, 12, 49, 73, 15, 16, "Spike Vine", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_FERTILIZER); }},
	{false, 12, 7, 97, 0, 1, "Boulders", [](std::set<int> inv) { return HaveLightSource(inv); }},
	{false, 13, 13, 6, 1, 2, "Barrel Mess", [](std::set<int> inv) { return true; }},
	{false, 14, 40, 13, 6, 7, "Lightning Rod Secret", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, 14, 73, 20, 8, 9, "Bat Door", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_BATKEY); }},
	{false, 15, 58, 77, 12, 13, "SE corner", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, 15, 51, 71, 10, 11, "Rhombus", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, 16, 15, 6, 3, 4, "Boss Reward", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, 17, 59, 29, 3, 4, "Barracks", [](std::set<int> inv) { return inv.count(VAR_POTION) && inv.count(VAR_BATKEY); }},
	{false, 18, 8, 5, 6, 7, "Top Left", [](std::set<int> inv) { return inv.count(VAR_POTION); }},
	{false, 20, 13, 37, 1, 2, "Boss Reward", [](std::set<int> inv) { return inv.count(VAR_POTION); }},
	{false, 21, 20, 13, 1, 2, "DoorDoorDoorDoorDoorDoor", [](std::set<int> inv) { return inv.count(VAR_POTION) && HaveLightSource(inv) && inv.count(VAR_BATKEY) &&
																						 inv.count(VAR_SKULLKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, 22, 39, 2, 1, 2, "Shaft", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 23, 12, 3, 2, 3, "Bombulus", [](std::set<int> inv) { return HaveAnyBigGem(inv); }},
	{false, 24, 50, 44, 2, 3, "Lockpick", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 25, 61, 2, 5, 6, "Happy Stick Hidden", [](std::set<int> inv) { return inv.count(VAR_TALISMAN); }},
	{false, 25, 35, 34, 0, 1, "Happy Stick Reward", [](std::set<int> inv) { return inv.count(VAR_TALISMAN); }},
	{false, 26, 2, 5, 1, 2, "Wolf Top Left", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING); }},
	{false, 26, 6, 14, 6, 7, "Pumpkin Door", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING) && inv.count(VAR_PUMPKINKEY); }},
	{false, 26, 40, 88, 3, 4, "Grow Room", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING); }},
	{false, 28, 88, 38, 3, 4, "The three hombres", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 29, 35, 39, 1, 2, "Left Vine", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_FERTILIZER); }},
	{false, 29, 103, 22, 3, 4, "Right Vine", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_FERTILIZER); }},
	{false, 30, 117, 76, 3, 4, "Pharoh bat Room", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_BATKEY); }},
	{false, 30, 135, 6, 10, 11, "2 blue Pharos", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 30, 84, 75, 5, 6, "GARGOYLE ROOM", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 31, 63, 12, 11, 12, "Vampire Guard", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 31, 27, 21, 13, 14, "maze top left", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 31, 105, 16, 15, 16, "Top Right Gauntlet", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 31, 9, 92, 22, 23, "Bat Closet", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, 32, 22, 37, 6, 7, "Candle Room", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, 32, 89, 22, 8, 9, "Top Right Top", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, 32, 89, 89, 2, 3, "Bottom Right Middle", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, 32, 52, 106, 4, 5, "Bat room", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, 33, 18, 6, 1, 2, "Gold Skull", [](std::set<int> inv) { return true; }},
	{false, 34, 63, 64, 13, 14, "Middle", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, 34, 94, 33, 17, 18, "Behind the Pews", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, 34, 101, 110, 5, 6, "AMBUSH!", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, 34, 103, 19, 10, 9, "Halloween", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, 34, 18, 11, 23, 24, "So many bats", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, 35, 94, 117, 8, 9, "Right Path", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, 35, 34, 117, 6, 7, "Left Path", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, 35, 104, 68, 3, 4, "Ballroom Right", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, 35, 111, 102, 10, 11, "Right Secret Wall", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, 35, 23, 68, 1, 2, "Ballroom Left", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, 37, 13, 18, 3, 4, "Boss", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv) && HaveAnySpecialWeapon(inv); }},
	{false, 38, 13, 18, 3, 4, "Boss", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv); }},
	{false, 39, 13, 18, 3, 4, "Boss", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv); }},
	{false, 40, 13, 18, 3, 4, "Boss", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv); }},
	{false, 42, 16, 28, 3, 4, "Bonkula", [](std::set<int> inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY) && HaveAllVamps(inv); }},
	{false, 43, 5, 22, 1, 2, "Bat Door", [](std::set<int> inv) { return inv.count(VAR_BATKEY); }},
	{false, 43, 18, 22, 4, 5, "Pebbles", [](std::set<int> inv) { return true; }},
	{false, 45, 55, 49, 0, 1, "Entrance", [](std::set<int> inv) { return inv.count(VAR_BOOTS); }},
	{false, 45, 55, 36, 5, 6, "End", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_FERTILIZER); }},
	{true, 0, 0, 0, 0, 0, "Ghostbusting", [](std::set<int> inv) { return HaveAnyBigGem(inv) && inv.count(VAR_DAISY); }},
	{true, 1, 0, 0, 0, 0, "Hairy Larry", [](std::set<int> inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING); }},
	{true, 2, 0, 0, 0, 0, "Scaredy Cat", [](std::set<int> inv) { return inv.count(VAR_CAT); }},
	{true, 3, 0, 0, 0, 0, "Silver Bullet", [](std::set<int> inv) { return inv.count(VAR_SILVER) && CanCleanseCrypts(inv); }},
	{true, 4, 0, 0, 0, 0, "Smashing Pumpkins", [](std::set<int> inv) { return CanCleanseCrypts(inv); }},
	{true, 5, 0, 0, 0, 0, "Sticky Shoes", [](std::set<int> inv) { return true; }},
	{true, 6, 0, 0, 0, 0, "The Collection", [](std::set<int> inv) { return inv.count(VAR_SILVERSLING) && inv.count(VAR_POTION) && HaveAnyBigGem(inv) && HaveLightSource(inv); }},
	{true, 7, 0, 0, 0, 0, "The Rescue", [](std::set<int> inv) { return HaveLightSource(inv); }},
	{true, 8, 0, 0, 0, 0, "Tree Trimming", [](std::set<int> inv) { return true; }},
	{true, 9, 0, 0, 0, 0, "Witch Mushrooms", [](std::set<int> inv) { return HaveAllMushrooms(inv); }},
	{true, 10, 0, 0, 0, 0, "Zombie Stomp", [](std::set<int> inv) { return CanCleanseCrypts(inv); }}};

rItem itemList[R_NUM_LOCATIONS] = {
	{3, VAR_HEART + 0, "Heart"},
	{3, VAR_HEART + 1, "Heart"},
	{3, VAR_HEART + 2, "Heart"},
	{3, VAR_HEART + 3, "Heart"},
	{3, VAR_HEART + 4, "Heart"},
	{3, VAR_HEART + 5, "Heart"},
	{3, VAR_HEART + 6, "Heart"},
	{3, VAR_HEART + 7, "Heart"},
	{3, VAR_HEART + 8, "Heart"},
	{3, VAR_HEART + 9, "Heart"},
	{3, VAR_HEART + 10, "Heart"},
	{3, VAR_HEART + 11, "Heart"},
	{3, VAR_HEART + 12, "Heart"},
	{3, VAR_HEART + 13, "Heart"},
	{3, VAR_HEART + 14, "Heart"},
	{3, VAR_HEART + 15, "Heart"},
	{3, VAR_HEART + 16, "Heart"},
	{3, VAR_HEART + 17, "Heart"},
	{3, VAR_HEART + 18, "Heart"},
	{3, VAR_HEART + 19, "Heart"},
	{7, VAR_LIGHTNING + 0, "Lightning"},
	{7, VAR_LIGHTNING + 1, "Lightning"},
	{7, VAR_LIGHTNING + 2, "Lightning"},
	{7, VAR_LIGHTNING + 3, "Lightning"},
	{7, VAR_LIGHTNING + 4, "Lightning"},
	{7, VAR_LIGHTNING + 5, "Lightning"},
	{7, VAR_LIGHTNING + 6, "Lightning"},
	{7, VAR_LIGHTNING + 7, "Lightning"},
	{7, VAR_LIGHTNING + 8, "Lightning"},
	{7, VAR_LIGHTNING + 9, "Lightning"},
	{8, VAR_ARROW + 0, "Arrow"},
	{8, VAR_ARROW + 1, "Arrow"},
	{8, VAR_ARROW + 2, "Arrow"},
	{8, VAR_ARROW + 3, "Arrow"},
	{8, VAR_ARROW + 4, "Arrow"},
	{8, VAR_ARROW + 5, "Arrow"},
	{8, VAR_ARROW + 6, "Arrow"},
	{8, VAR_ARROW + 7, "Arrow "},
	{8, VAR_ARROW + 8, "Arrow"},
	{8, VAR_ARROW + 9, "Arrow"},
	{9, VAR_PANTS + 0, "Pants"},
	{9, VAR_PANTS + 1, "Pants"},
	{9, VAR_PANTS + 2, "Pants"},
	{9, VAR_PANTS + 3, "Pants"},
	{9, VAR_PANTS + 4, "Pants"},
	{9, VAR_PANTS + 5, "Pants"},
	{9, VAR_PANTS + 6, "Pants"},
	{9, VAR_PANTS + 7, "Pants"},
	{9, VAR_PANTS + 8, "Pants"},
	{9, VAR_PANTS + 9, "Pants"},
	{81, VAR_MUSHROOM + 0, "Mushroom"},
	{81, VAR_MUSHROOM + 1, "Mushroom"},
	{81, VAR_MUSHROOM + 2, "Mushroom"},
	{81, VAR_MUSHROOM + 3, "Mushroom"},
	{81, VAR_MUSHROOM + 4, "Mushroom"},
	{81, VAR_MUSHROOM + 5, "Mushroom"},
	{81, VAR_MUSHROOM + 6, "Mushroom"},
	{81, VAR_MUSHROOM + 7, "Mushroom"},
	{81, VAR_MUSHROOM + 8, "Mushroom"},
	{81, VAR_MUSHROOM + 9, "Mushroom"},
	{44, VAR_MYSORB + 0, "Orb"},
	{44, VAR_MYSORB + 1, "Orb"},
	{44, VAR_MYSORB + 2, "Orb"},
	{44, VAR_MYSORB + 3, "Orb"},
	{14, VAR_WEAPON + 0, "Bombs"},
	{15, VAR_WEAPON + 1, "Shock Wand"},
	{16, VAR_WEAPON + 2, "Ice spear"},
	{17, VAR_WEAPON + 3, "Cactus"},
	{18, VAR_WEAPON + 4, "Boomerang"},
	{19, VAR_WEAPON + 5, "Whoopee"},
	{20, VAR_WEAPON + 6, "Hot Pants"},
	{11, VAR_SKULLKEY, "Skull Key"},
	{12, VAR_BATKEY, "Bat Key"},
	{13, VAR_PUMPKINKEY, "Pumpkin Key"},
	{130, VAR_BOOTS, "Boots"},
	{136, VAR_STICK, "Stick"},
	{131, VAR_FERTILIZER, "Fertilizer"},
	{83, VAR_SILVER, "Silver"},
	{82, VAR_DAISY, "Doom Daisy"},
	{132, VAR_POTION, "Ghost Potion"},
	{113, VAR_VAMPBUST + 0, "Vamp Statue"},
	{113, VAR_VAMPBUST + 1, "Vamp Statue"},
	{113, VAR_VAMPBUST + 2, "Vamp statue"},
	{113, VAR_VAMPBUST + 3, "Vamp Statue"},
	{113, VAR_VAMPBUST + 4, "Vamp Statue"},
	{113, VAR_VAMPBUST + 5, "Vamp Statue"},
	{113, VAR_VAMPBUST + 6, "Vamp Statue"},
	{113, VAR_VAMPBUST + 7, "Vamp Statue"},
	{128, VAR_VAMPBUST + 8, "Cat"},
	{6, VAR_GEM + 0, "Big Gem"},
	{6, VAR_GEM + 1, "Big Gem"},
	{6, VAR_GEM + 2, "Big Gem"},
	{6, VAR_GEM + 3, "Big Gem"},
	{6, VAR_GEM + 4, "Big Gem"},
	{6, VAR_GEM + 5, "Big Gem"},
	{129, VAR_ZOMBIEREWARD, "Zombie Reward"},
	{111, VAR_TRIPLEFIRE, "3 way"},
	{110, VAR_TALISMAN, "Happy Stick"},
	{116, VAR_BATSTATUE + 0, "Bat Statue"},
	{116, VAR_BATSTATUE + 1, "Bat Statue"},
	{116, VAR_BATSTATUE + 2, "Bat Statue"},
	{116, VAR_BATSTATUE + 3, "Bat Statue"},
	{134, VAR_REFLECT, "Reflect"},
	{133, VAR_LANTERN, "Lantern"},
	{135, VAR_SILVERSLING, "Silver Sling"},
};

void InitRandomizerMenu(void)
{
	oldc = 255;
	cursor = 0;
	InitPlasma(7);
	seed = "";
}

void ExitRandomizerMenu(void)
{
	ExitPlasma();
}

TASK(byte)
UpdateRandomizerMenu(int *lastTime, MGLDraw *mgl)
{
	char c;
	byte c2;
	dword btn, j;
	int i;

	if (*lastTime > TIME_PER_FRAME * 30)
		*lastTime = TIME_PER_FRAME * 30;
	while (*lastTime >= TIME_PER_FRAME)
	{
		switch (optMode)
		{
		case 0: // just going through options
			c = mgl->LastKeyPressed();
			c2 = GetControls() | GetArrows();

			if (c == 27)
			{
				CO_RETURN 1;
			}

			if ((c2 & CONTROL_UP) && (!(oldc & CONTROL_UP)))
			{
				cursor--;
				if (cursor > 4)
					cursor = 4;
				MakeNormalSound(SND_MENUCLICK);
			}
			if ((c2 & CONTROL_DN) && (!(oldc & CONTROL_DN)))
			{
				cursor++;
				if (cursor > 4)
					cursor = 0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if ((c2 & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3)) && (!(oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))))
			{
				switch (cursor)
				{
				case 0: //play
					MakeNormalSound(SND_MENUSELECT);
					AWAIT LunaticGame(mgl,0,WORLD_RANDOMIZER);
					SetSongRestart(1);
					break;
				case 1: //randomize
					MakeNormalSound(SND_MENUSELECT);
					break;
				case 2: //seed entry
					MakeNormalSound(SND_MENUSELECT);
					LastScanCode();
					optMode = 1;
					break;
				case 3: //generate
					while(!CheckBeatable()){
						MakeNormalSound(SND_MENUCANCEL);
					}
					MakeNormalSound(SND_POWERUP);
					break;

				case 4: //exit
					MakeNormalSound(SND_MENUSELECT);
					CO_RETURN 1;
					break;
				}
			}
			if (c == SDLK_j)
			{
				AssumedFill();
			}
			break;
		case 1: // Typing in something
			c = mgl->LastKeyPressed();
			if (c == SDLK_ESCAPE) // ESC key
			{
				optMode = 0;
				c2 = 255;
				oldc = 255;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUCANCEL);
				CO_RETURN 0;
			}
			else if (c == SDLK_BACKSPACE && seed.length() > 0)
			{
				seed.pop_back();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if (c != 0)
			{
				if (seed.length() >= 32)
				{
					MakeNormalSound(SND_MENUCANCEL);
				}
				else
				{
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
					{
						seed += c;
						MakeNormalSound(SND_MENUSELECT);
					}
				}
			}
			c2 = 255;
			break;
		}

		oldc = c2;

		UpdatePlasma();

		*lastTime -= TIME_PER_FRAME;
	}

	CO_RETURN 0;
}

void RenderRandomizerMenu(MGLDraw *mgl)
{

	int wid;
	byte *pos;
	int i;

	wid = mgl->GetWidth();
	pos = mgl->GetScreen() + 40 * wid;
	for (i = 40; i < 480 - 40; i++)
	{
		memset(pos, 7 * 32 + 2, 640);
		pos += wid;
	}

	RenderPlasma(mgl, 1);
	RenderPlasma(mgl, 0);

	CenterPrintGlow(320, 2, "Game Randomizer", 0, 2);

	PrintColor(240, 60, "Play!", 7, -10, 0);
	if (cursor == 0)
	{
		PrintColor(239, 59, "Play!", 0, 0, 0);

	}

	PrintColor(240, 80, "Randomize Seed", 7, -10, 0);
	if (cursor == 1)
	{
		PrintColor(239, 79, "Randomize Seed", 0, 0, 0);
	}

	PrintColor(240, 100, "Seed: ", 7, -10, 0);
	PrintColor(300, 100, seed.c_str(), 7, -10, 0);
	if (cursor == 2)
	{
		PrintColor(239, 99, "Seed: ", 0, 0, 0);
		PrintColor(299, 99, seed.c_str(), 0, 0, 0);
	}

	PrintColor(240, 120, "Generate", 7, -10, 0);
	if (cursor == 3)
	{
		PrintColor(239, 119, "Generate", 0, 0, 0);
	}

	PrintColor(240, 140, "Exit To Main Menu", 7, -10, 0);
	if (cursor == 4)
		PrintColor(239, 139, "Exit To Main Menu", 0, 0, 0);
}

//----------------

TASK(void)
RandomizerMenu(MGLDraw *mgl)
{
	byte done = 0;
	int lastTime;

	InitRandomizerMenu();
	lastTime = 1;

	while (!done)
	{
		lastTime += TimeLength();
		StartClock();
		done =  AWAIT UpdateRandomizerMenu(&lastTime, mgl);
		RenderRandomizerMenu(mgl);
		AWAIT mgl->Flip();
		EndClock();

		if (!mgl->Process())
			done = 1;
	}

	ExitRandomizerMenu();
}


void AssumedFill()
{

	ownedItems = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
				  90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
				  160, 161, 162, 164, 165, 166, 167, 189, 218, 220};
	//const . copyOptions = std::filesystem::copy_options::update_existing;
	if (std::filesystem::exists("rando.llw"))
	{
		std::filesystem::remove("rando.llw");
	}
	std::filesystem::copy("loony.llw", "rando.llw");

	world_t world;
	LoadWorld(&world, "rando.llw");

	//fix that awful, singular, flipped special
	//Castle Vampy III "halloween" specials
	special_t tempSpecial = world.map[34]->special[9];
	world.map[34]->special[9].x = world.map[34]->special[10].x;
	world.map[34]->special[9].y = world.map[34]->special[10].y;
	world.map[34]->special[10].x = tempSpecial.x;
	world.map[34]->special[10].y = tempSpecial.y;

	//fix trigger for cat tree
	world.map[0]->special[97].trigger = TRG_GETITEM;

	SaveWorld(&world, "rando.llw");

	/*for (int i = 0; i < 94; i++)
	{
		int s1 = doublecheck[i][0];
		int s2 = doublecheck[i][1];
		int x = doublecheck[i][2];
		int y = doublecheck[i][3];
		int mapnum = doublecheck[i][4];
		if (!(world.map[mapnum]->special[s1].x == x && world.map[mapnum]->special[s1].y == y)){
			printf("issue with map %d, %d isn't right x, y\n", mapnum, s1);
		}
		if (!(world.map[mapnum]->special[s2].x + 1 == x && world.map[mapnum]->special[s1].y == y)){
			printf("issue with map %d, s2 %d isn't right x, y\n", mapnum, s2);
		}
		if (!(world.map[mapnum]->special[s1].trigger==TRG_GETITEM)){
			printf("issue with map %d, s1 %d isn't right trigger\n", mapnum, s1);
		}
		if (!(world.map[mapnum]->special[s2].trigger==TRG_VARON)){
			printf("issue with map %d, s2 %d isn't right trigger\n", mapnum, s2);
		}
	}*/

	/*
	for(int i = 0; i < world.numMaps; i++)
	{
		if (world.map[i] == NULL)
		{
			continue;
		}
		Map m = world.map[i];
		printf("%s\n", m.name);
		int specialNum = 0;
		for (special_t s : m.special)
		{
			if (s.trigger == TRG_GETITEM){
				printf("\t%d, %X, %d, %d, %d\n", specialNum, s.effect, s.x, s.y, s.value);
				
			}
			specialNum++;
		}
	}*/

	//world.map[i]->special[y]

	//prefill section, keys to bosses

	//pick a random item
	//pick a random location

	//if its a quest, set entry in quest reward to item id
	//if its a physical spot, edit specials to use new item id

	//go until out of items

	//verify beatable
}

bool CheckBeatable()
{
	int runs = 0;
	int evilizers = 0;
	//unsigned seed = time(0);
	//auto rng = std::default_random_engine(seed);
	//for(int count = 0; count < 1000; count++)
	//{
	std::set<int> collectedItems;
	std::set<int> tempItems;
	std::vector<location> remaining;
	std::vector<location> visited;

	for (location l : basic_locations)
	{
		remaining.push_back(l);
	}

	std::shuffle(remaining.begin(), remaining.end(), rng);

	//FILE *fp = fopen("spoiler.txt", "w+");
	std::ofstream spoilerFile;
  	spoilerFile.open ("spoiler.txt");
	//fprintf(fp, "START OF WORLD\n");
	//super simple quick logic, make sure bonkula isn't holing a vamp statue
	bool shortcircuit = false;
	for (int i = 0; i < remaining.size(); i++)
	{
		if (remaining[i].description.compare("Bonkula") == 0 &&
			((itemList[i].playerVarId >= VAR_VAMPBUST && itemList[i].playerVarId <= VAR_VAMPBUST + 7) ||
			 itemList[i].playerVarId == VAR_SKULLKEY || itemList[i].playerVarId == VAR_BATKEY || itemList[i].playerVarId == VAR_PUMPKINKEY))
		{
			//std::cout << "short circuit" << remaining[i].description;
			shortcircuit = true;
			break;
		}
		remaining[i].item = itemList[i];
		spoilerFile << remaining[i].mapId << "\t\t" << remaining[i].description << "\t\t" << itemList[i].playerVarId << "\t\t" << itemList[i].itemName << "\n";
	}
	spoilerFile.close();
	//if (shortcircuit)
	//{
	//	continue;
	//}
	//fprintf(fp, "END OF WORLD\n\n\n");
	//fclose(fp);

	bool gotEvilizer = false;
	int foundItems = 0;
	/*for (int i = 0; i < VAR_SILVERSLING + 1; i++){
		collectedItems.insert(i);
	}*/
	//collectedItems.insert(VAR_BOOTS);

	do
	{
		foundItems = 0;
		for (int i = 0; i < remaining.size(); i++)
		{
			location l = remaining[i];
			if (l.requirements(collectedItems))
			{
				//visited.insert( l.newItem.playerVarId );
				foundItems++;
				if (l.description.compare("Bonkula") == 0)
				{
					gotEvilizer = true;
				}

				tempItems.insert(l.item.playerVarId);

				visited.push_back(l);
				remaining.erase(remaining.begin() + i);
				i--;
			}
			else
			{
				//std::cout << "cant get " << l.description << "\n";
			}
		}
		//printf("%d locations\n", foundItems);
		collectedItems.insert(tempItems.begin(), tempItems.end());
		tempItems.clear();

	} while (foundItems > 0);
	/*if (remaining.size() == 0){
		gotEvilizer = true;
	}*/

	//printf("Evilizer: %d\n", gotEvilizer);
	if (gotEvilizer)
	{
		evilizers++;
		std::cout << "GO TIME BAYBEEEEEEEEEEEEEEEEEEEEEEE\n";
		visited.insert( visited.end(), remaining.begin(), remaining.end());
		PlaceItems(visited);
	}else{
		remove("spoiler.txt");
	}
	runs++;

	//printf("End inv: ");
	//for(int i : collectedItems){
	//	printf("%d, ", i);
	//}
	//printf("\n");

	//printf("Remaining locations: ");

	/*if (remaining.size() < 10)
	{
	for(location l : remaining){
		std::cout << l.description << " " << l.item.playerVarId << " " << l.item.itemName << "\n";
	}
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
	}*/
	//printf("\n");
	/*if (count % 100 == 0)
	{
		std::cout << "count : " << count << "skipped" << count - runs << "evilizers: " << evilizers << "\n";
	}

	}
	std::cout << "runs : " << runs << "evilizers: " << evilizers << "\n";*/
	return gotEvilizer;
}

void PlaceItems(std::vector<location> loclist)
{

	world_t world;
	LoadWorld(&world, "rando.llw");

	//fix that awful, singular, flipped special
	//Castle Vampy III "halloween" specials
	special_t tempSpecial = world.map[34]->special[9];
	world.map[34]->special[9].x = world.map[34]->special[10].x;
	world.map[34]->special[9].y = world.map[34]->special[10].y;
	world.map[34]->special[10].x = tempSpecial.x;
	world.map[34]->special[10].y = tempSpecial.y;
	

	//fix trigger for cat tree
	world.map[0]->special[97].trigger = TRG_GETITEM;
	//for each, go into the world
	//std::ofstream questFile;
	
  	//questFile.open ("quest.txt");
	std::FILE* f = AppdataOpen("quest.txt", "w");
	FilePtrStream stream(f);
	

	for (location loc : loclist)
	{
		if (loc.isQuest){
			//quest list of map id points to loc.item now
			//randoReward[loc.mapId] = loc.item;
			stream << loc.mapId << "\t" << loc.item.playerVarId << "\t" << loc.item.itemId  << "\n";

		}
		else
		{
			Map* tempMap = world.map[loc.mapId];
			tempMap->special[loc.s1].value = loc.item.playerVarId;
			tempMap->special[loc.s1].effectTag = 1;

			tempMap->special[loc.s2].trigValue = loc.item.playerVarId;

			
			//curmap->map[4+7*curmap->width].item=loc.item.itemId;
			tempMap->map[loc.xcoord+loc.ycoord*tempMap->width].item=loc.item.itemId;
		}
	}

	fclose(f);


	SaveWorld(&world, "rando.llw");

	//if quest, update rando quest table

	//if physical, place item and update specials
}

bool HaveLightSource(std::set<int> inv)
{
	return (inv.count(VAR_LANTERN) || inv.count(VAR_STICK) && inv.count(VAR_BOOTS) || inv.count(VAR_TORCH));
}

bool HaveAnyBigGem(std::set<int> inv)
{
	return (inv.count(VAR_GEM) || inv.count(VAR_GEM + 1) || inv.count(VAR_GEM + 2) ||
			inv.count(VAR_GEM + 3) || inv.count(VAR_GEM + 4) || inv.count(VAR_GEM + 5));
}

bool HaveAllOrbs(std::set<int> inv)
{
	return (inv.count(VAR_MYSORB) && inv.count(VAR_MYSORB + 1) && inv.count(VAR_MYSORB + 2) && inv.count(VAR_MYSORB + 3));
}

bool HaveAllBats(std::set<int> inv)
{
	return (inv.count(VAR_BATSTATUE) && inv.count(VAR_BATSTATUE + 1) && inv.count(VAR_BATSTATUE + 2) && inv.count(VAR_BATSTATUE + 3));
}

bool HaveAllVamps(std::set<int> inv)
{
	return (inv.count(VAR_VAMPBUST) && inv.count(VAR_VAMPBUST + 1) && inv.count(VAR_VAMPBUST + 2) && inv.count(VAR_VAMPBUST + 3) &&
			inv.count(VAR_VAMPBUST + 4) && inv.count(VAR_VAMPBUST + 5) && inv.count(VAR_VAMPBUST + 6) && inv.count(VAR_VAMPBUST + 7));
}

bool HaveAnySpecialWeapon(std::set<int> inv)
{
	return (inv.count(VAR_WEAPON) || inv.count(VAR_WEAPON + 1) || inv.count(VAR_WEAPON + 2) || inv.count(VAR_WEAPON + 4) ||
			inv.count(VAR_WEAPON + 5) || inv.count(VAR_WEAPON + 6));
}

bool HaveAllMushrooms(std::set<int> inv)
{
	return (inv.count(VAR_MUSHROOM) && inv.count(VAR_MUSHROOM + 1) && inv.count(VAR_MUSHROOM + 2) && inv.count(VAR_MUSHROOM + 3) &&
			inv.count(VAR_MUSHROOM + 4) && inv.count(VAR_MUSHROOM + 5) && inv.count(VAR_MUSHROOM + 6) && inv.count(VAR_MUSHROOM + 7) &&
			inv.count(VAR_MUSHROOM + 8) && inv.count(VAR_MUSHROOM + 9));
}

bool CanCleanseCrypts(std::set<int> inv)
{
	return (HaveLightSource(inv) && inv.count(VAR_BOOTS));
}
