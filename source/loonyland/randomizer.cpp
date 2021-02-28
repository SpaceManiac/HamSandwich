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
#include <charconv>
#include <string>

static byte cursor;
static byte oldc;
static dword oldBtn;
static byte optMode;
static std::string seed;
std::set<int> ownedItems;
static int genTries = 0;

auto rng = std::default_random_engine(std::random_device{}());

#define MAX_SEED_LENGTH 32
#define R_NUM_LOCATIONS 105

location basic_locations[R_NUM_LOCATIONS] = {
	{false, "Halloween Hill", 0, 194, 5, 24, 25, "Swamp Mud Path", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS); }},
	{false, "Halloween Hill",  0, 187, 112, 11, 12, "Bog Beast Home", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill",  0, 2, 46, 69, 70, "Rocky Cliffs below Upper Caverns", [](const std::set<int>& inv) { return HaveAnyBigGem(inv); }},
	{false, "Halloween Hill",  0, 131, 26, 10, 9, "Sapling Shrine", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS); }},
	{false, "Halloween Hill",  0, 83, 145, 33, 34, "Terror Glade", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill",  0, 1, 73, 47, 48, "Rocky Cliffs Vine", [](const std::set<int>& inv) { return inv.count(VAR_FERTILIZER); }},
	{false, "Halloween Hill",  0, 30, 39, 73, 74, "Rocky Cliffs Grand Pharoh", [](const std::set<int>& inv) { return HaveAnyBigGem(inv); }},
	{false, "Halloween Hill",  0, 69, 59, 66, 67, "Rocky Cliffs Rock Corner", [](const std::set<int>& inv) { return HaveAnyBigGem(inv) && inv.count(VAR_WEAPON); }},
	{false, "Halloween Hill",  0, 115, 70, 18, 19, "Mushroom outside town", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill",  0, 159, 85, 20, 21, "mushroom east", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill", 0, 117, 33, 22, 23, "Top left mushroom spot", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill", 0, 168, 98, 28, 29, "mushroom south east", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill", 0, 197, 161, 56, 57, "East Woods", [](const std::set<int>& inv) { return true; }},
	{false, "Halloween Hill", 0, 16, 61, 49, 50, "Rocky Cliffs Ledge", [](const std::set<int>& inv) { return HaveAnyBigGem(inv); }},
	{false, "Halloween Hill", 0, 28, 10, 63, 65, "Rocky Cliffs Peak", [](const std::set<int>& inv) { return HaveAnyBigGem(inv); }},
	{false, "Halloween Hill", 0, 134, 182, 97, 98, "Cat Tree", [](const std::set<int>& inv) { return true; }},
	{false, "The Witch's Cabin", 2, 27, 16, 1, 2, "Backroom", [](const std::set<int>& inv) { return true; }},
	{false, "The Witch's Cabin", 2, 22, 7, 3, 4, "Bedroom", [](const std::set<int>& inv) { return HaveLightSource(inv); }},
	{false, "Bonita's Cabin", 3, 10, 8, 1, 3, "Barrel Maze", [](const std::set<int>& inv) { return true; }},
	{false, "The Bog Pit", 4, 19, 5, 1, 2, "Top Door", [](const std::set<int>& inv) { return inv.count(VAR_SKULLKEY); }},
	{false, "The Bog Pit", 4, 39, 38, 3, 4, "Post Room", [](const std::set<int>& inv) { return true; }},
	{false, "The Bog Pit", 4, 33, 06, 9, 10, "Window Drip", [](const std::set<int>& inv) { return true; }},
	{false, "The Bog Pit", 4, 3, 33, 7, 8, "Green room", [](const std::set<int>& inv) { return true; }},
	{false, "The Bog Pit", 4, 25, 48, 5, 11, "Arena", [](const std::set<int>& inv) { return true; }},
	{false, "The Bog Pit", 4, 2, 58, 17, 18, "Kill Wall", [](const std::set<int>& inv) { return true; }},
	{false, "Underground Tunnel", 5, 53, 41, 0, 1, "Swampdog Door", [](const std::set<int>& inv) { return inv.count(VAR_PUMPKINKEY); }},
	{false, "Underground Tunnel", 5, 57, 37, 13, 14, "Scribble Wall", [](const std::set<int>& inv) { return true; }},
	{false, "Underground Tunnel", 5, 26, 25, 4, 5, "Tiny Passage", [](const std::set<int>& inv) { return true; }},
	{false, "Underground Tunnel", 5, 50, 26, 7, 8, "fire frogs", [](const std::set<int>& inv) { return true; }},
	{false, "Underground Tunnel", 5, 17, 64, 9, 10, "Torch Island", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS); }},
	{false, "Underground Tunnel", 5, 44, 49, 11, 12, "Small Room", [](const std::set<int>& inv) { return true; }},
	{false, "Swamp Gas Cavern", 6, 32, 45, 2, 3, "Scratch Wall", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS); }},
	{false, "Swamp Gas Cavern", 6, 44, 37, 15, 17, "Bat Mound", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS) && inv.count(VAR_BATKEY); }},
	{false, "Swamp Gas Cavern", 6, 3, 7, 23, 24, "Stair room", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS); }},
	{false, "Swamp Gas Cavern", 6, 43, 58, 18, 19, "Rock Prison", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS) && inv.count(VAR_WEAPON); }},
	{false, "A Tiny Cabin", 7, 11, 16, 1, 2, "Tiny Cabin", [](const std::set<int>& inv) { return inv.count(VAR_SKULLKEY); }},
	{false, "A Cabin", 8, 19, 15, 3, 4, "Bedside ", [](const std::set<int>& inv) { return true; }},
	{false, "Dusty Crypt", 10, 2, 44, 2, 3, "Pumpkin Door", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Dusty Crypt", 10, 5, 36, 4, 5, "Maze", [](const std::set<int>& inv) { return HaveLightSource(inv); }},
	{false, "Musty Crypt", 11, 51, 82, 11, 12, "Big Closed Room", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_BOOTS); }},
	{false, "Rusty Crypt", 12, 49, 73, 15, 16, "Spike Vine", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_FERTILIZER); }},
	{false, "Rusty Crypt", 12, 7, 97, 0, 1, "Boulders", [](const std::set<int>& inv) { return HaveLightSource(inv); }},
	{false, "A Messy Cabin", 13, 13, 6, 1, 2, "Barrel Mess", [](const std::set<int>& inv) { return true; }},
	{false, "Under the Lake", 14, 40, 13, 6, 7, "Lightning Rod Secret", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, "Under the Lake", 14, 73, 20, 8, 9, "Bat Door", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_BATKEY); }},
	{false, "Deeper Under the Lake", 15, 58, 77, 12, 13, "SE corner", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, "Deeper Under the Lake", 15, 51, 71, 10, 11, "Rhombus", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, "Frankenjulie's Laboratory", 16, 15, 6, 3, 4, "Boss Reward", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAllOrbs(inv); }},
	{false, "Haunted Tower", 17, 59, 29, 3, 4, "Barracks", [](const std::set<int>& inv) { return inv.count(VAR_POTION) && inv.count(VAR_BATKEY); }},
	{false, "Haunted Tower, Floor 2", 18, 8, 5, 6, 7, "Top Left", [](const std::set<int>& inv) { return inv.count(VAR_POTION); }},
	{false, "Haunted Tower Roof", 20, 13, 37, 1, 2, "Boss Reward", [](const std::set<int>& inv) { return inv.count(VAR_POTION); }},
	{false, "Haunted Basement", 21, 20, 13, 1, 2, "DoorDoorDoorDoorDoorDoor", [](const std::set<int>& inv) { return inv.count(VAR_POTION) && HaveLightSource(inv) && inv.count(VAR_BATKEY) && inv.count(VAR_SKULLKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Abandoned Mines", 22, 39, 2, 1, 2, "Shaft", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "The Shrine of Bombulus", 23, 12, 3, 2, 3, "Bombulus", [](const std::set<int>& inv) { return HaveAnyBigGem(inv); }},
	{false, "A Gloomy Cavern", 24, 50, 44, 2, 3, "Lockpick", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Happy Stick Woods", 25, 61, 2, 5, 6, "Happy Stick Hidden", [](const std::set<int>& inv) { return inv.count(VAR_TALISMAN); }},
	{false, "Happy Stick Woods", 25, 35, 34, 0, 1, "Happy Stick Reward", [](const std::set<int>& inv) { return inv.count(VAR_TALISMAN); }},
	{false, "The Wolf Den", 26, 2, 5, 1, 2, "Wolf Top Left", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING); }},
	{false, "The Wolf Den", 26, 6, 14, 6, 7, "Pumpkin Door", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING) && inv.count(VAR_PUMPKINKEY); }},
	{false, "The Wolf Den", 26, 40, 88, 3, 4, "Grow Room", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING); }},
	{false, "Upper Creepy Cavern", 28, 88, 38, 3, 4, "The three hombres", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Under the Ravine", 29, 35, 39, 1, 2, "Left Vine", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_FERTILIZER); }},
	{false, "Under the Ravine", 29, 103, 22, 3, 4, "Right Vine", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_FERTILIZER); }},
	{false, "Creepy Caverns", 30, 117, 76, 3, 4, "Pharoh bat Room", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_BATKEY); }},
	{false, "Creepy Caverns", 30, 135, 6, 10, 11, "2 blue Pharos", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Creepy Caverns", 30, 84, 75, 5, 6, "GARGOYLE ROOM", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Castle Vampy", 31, 63, 12, 11, 12, "Vampire Guard", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Castle Vampy", 31, 27, 21, 13, 14, "maze top left", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Castle Vampy", 31, 105, 16, 15, 16, "Top Right Gauntlet", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Castle Vampy", 31, 9, 92, 22, 23, "Bat Closet", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv); }},
	{false, "Castle Vampy II", 32, 22, 37, 6, 7, "Candle Room", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, "Castle Vampy II", 32, 89, 22, 8, 9, "Top Right Top", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, "Castle Vampy II", 32, 89, 89, 2, 3, "Bottom Right Middle", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, "Castle Vampy II", 32, 52, 106, 4, 5, "Bat room", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY); }},
	{false, "Cabin in the woods", 33, 18, 6, 1, 2, "Gold Skull", [](const std::set<int>& inv) { return true; }},
	{false, "Castle Vampy III", 34, 63, 64, 13, 14, "Middle", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, "Castle Vampy III", 34, 94, 33, 17, 18, "Behind the Pews", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, "Castle Vampy III", 34, 101, 110, 5, 6, "AMBUSH!", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, "Castle Vampy III", 34, 103, 19, 10, 9, "Halloween", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, "Castle Vampy III", 34, 18, 11, 23, 24, "So many bats", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY); }},
	{false, "Castle Vampy IV", 35, 94, 117, 8, 9, "Right Path", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Castle Vampy IV", 35, 34, 117, 6, 7, "Left Path", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Castle Vampy IV", 35, 104, 68, 3, 4, "Ballroom Right", [](const std::set<int>& inv) { return inv.count(VAR_SILVERSLING) && HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Castle Vampy IV", 35, 111, 102, 10, 11, "Right Secret Wall", [](const std::set<int>& inv) { return inv.count(VAR_SILVERSLING) && HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Castle Vampy IV", 35, 23, 68, 1, 2, "Ballroom Left", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY); }},
	{false, "Castle Vampy Roof", 37, 13, 18, 3, 4, "Boss", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv) && HaveAnySpecialWeapon(inv); }},
	{false, "Castle Vampy Roof", 38, 13, 18, 3, 4, "Boss", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv); }},
	{false, "Castle Vampy Roof", 39, 13, 18, 3, 4, "Boss", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv); }},
	{false, "Castle Vampy Roof", 40, 13, 18, 3, 4, "Boss", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && HaveAllBats(inv); }},
	{false, "Heart of Terror", 42, 16, 28, 3, 4, "Bonkula", [](const std::set<int>& inv) { return HaveLightSource(inv) && HaveAnyBigGem(inv) && inv.count(VAR_SKULLKEY) && inv.count(VAR_BATKEY) && inv.count(VAR_PUMPKINKEY) && HaveAllVamps(inv); }},
	{false, "A Hidey Hole", 43, 5, 22, 1, 2, "Bat Door", [](const std::set<int>& inv) { return inv.count(VAR_BATKEY); }},
	{false, "A Hidey Hole", 43, 18, 22, 4, 5, "Pebbles", [](const std::set<int>& inv) { return true; }},
	{false, "Swampdog Lair", 45, 55, 49, 0, 1, "Entrance", [](const std::set<int>& inv) { return inv.count(VAR_BOOTS); }},
	{false, "Swampdog Lair", 45, 55, 36, 5, 6, "End", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_FERTILIZER); }},
	{true, "Ghostbusting", 0, 0, 0, 0, 0, "Ghostbusting", [](const std::set<int>& inv) { return HaveAnyBigGem(inv) && inv.count(VAR_DAISY) && HaveAllMushrooms(inv); }},
	{true, "Hairy Larry", 1, 0, 0, 0, 0, "Hairy Larry", [](const std::set<int>& inv) { return HaveLightSource(inv) && inv.count(VAR_SILVERSLING); }},
	{true, "Scaredy Cat", 2, 0, 0, 0, 0, "Scaredy Cat", [](const std::set<int>& inv) { return inv.count(VAR_CAT); }},
	{true, "Silver Bullet", 3, 0, 0, 0, 0, "Silver Bullet", [](const std::set<int>& inv) { return inv.count(VAR_SILVER) && CanCleanseCrypts(inv); }},
	{true, "Smashing Pumpkins", 4, 0, 0, 0, 0, "Smashing Pumpkins", [](const std::set<int>& inv) { return CanCleanseCrypts(inv); }},
	{true, "Sticky Shoes", 5, 0, 0, 0, 0, "Sticky Shoes", [](const std::set<int>& inv) { return true; }},
	{true, "The Collection", 6, 0, 0, 0, 0, "The Collection", [](const std::set<int>& inv) { return inv.count(VAR_SILVERSLING) && inv.count(VAR_POTION) && HaveAnyBigGem(inv) && HaveLightSource(inv); }},
	{true, "The Rescue", 7, 0, 0, 0, 0, "The Rescue", [](const std::set<int>& inv) { return HaveLightSource(inv); }},
	{true, "Tree Trimming", 8, 0, 0, 0, 0, "Tree Trimming", [](const std::set<int>& inv) { return true; }},
	{true, "Witch Mushrooms", 9, 0, 0, 0, 0, "Witch Mushrooms", [](const std::set<int>& inv) { return HaveAllMushrooms(inv); }},
	{true, "Zombie Stomp", 10, 0, 0, 0, 0, "Zombie Stomp", [](const std::set<int>& inv) { return CanCleanseCrypts(inv); }}};


rItem itemList[R_NUM_LOCATIONS] = {
	{ITM_SUPERHEART, VAR_HEART + 0, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 1, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 2, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 3, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 4, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 5, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 6, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 7, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 8, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 9, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 10, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 11, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 12, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 13, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 14, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 15, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 16, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 17, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 18, "Heart"},
	{ITM_SUPERHEART, VAR_HEART + 19, "Heart"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 0, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 1, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 2, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 3, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 4, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 5, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 6, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 7, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 8, "Lightning"},
	{ITM_FIREPOWERUP, VAR_LIGHTNING + 9, "Lightning"},
	{ITM_RANGEUP, VAR_ARROW + 0, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 1, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 2, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 3, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 4, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 5, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 6, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 7, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 8, "Arrow"},
	{ITM_RANGEUP, VAR_ARROW + 9, "Arrow"},
	{ITM_FIRERATEUP, VAR_PANTS + 0, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 1, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 2, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 3, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 4, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 5, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 6, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 7, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 8, "Pants"},
	{ITM_FIRERATEUP, VAR_PANTS + 9, "Pants"},
	{ITM_SHROOM, VAR_MUSHROOM + 0, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 1, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 2, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 3, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 4, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 5, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 6, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 7, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 8, "Mushroom"},
	{ITM_SHROOM, VAR_MUSHROOM + 9, "Mushroom"},
	{ITM_MYSORB, VAR_MYSORB + 0, "Orb"},
	{ITM_MYSORB, VAR_MYSORB + 1, "Orb"},
	{ITM_MYSORB, VAR_MYSORB + 2, "Orb"},
	{ITM_MYSORB, VAR_MYSORB + 3, "Orb"},
	{ITM_WBOMB, VAR_WEAPON + 0, "Bombs"},
	{ITM_WLIGHTNING, VAR_WEAPON + 1, "Shock Wand"},
	{ITM_WICE, VAR_WEAPON + 2, "Ice spear"},
	{ITM_WCACTUS, VAR_WEAPON + 3, "Cactus"},
	{ITM_WBOOMERANG, VAR_WEAPON + 4, "Boomerang"},
	{ITM_WWHOOPEE, VAR_WEAPON + 5, "Whoopee"},
	{ITM_WHOTPANTS, VAR_WEAPON + 6, "Hot Pants"},
	{ITM_KEY2, VAR_SKULLKEY, "Skull Key"},
	{ITM_KEY3, VAR_BATKEY, "Bat Key"},
	{ITM_KEY4, VAR_PUMPKINKEY, "Pumpkin Key"},
	{ITM_BOOTS, VAR_BOOTS, "Boots"},
	{ITM_STICK, VAR_STICK, "Stick"},
	{ITM_FERTILIZER, VAR_FERTILIZER, "Fertilizer"},
	{ITM_SILVER, VAR_SILVER, "Silver"},
	{ITM_DAISY, VAR_DAISY, "Doom Daisy"},
	{ITM_POTION, VAR_POTION, "Ghost Potion"},
	{ITM_BUST, VAR_VAMPBUST + 0, "Vamp Statue"},
	{ITM_BUST, VAR_VAMPBUST + 1, "Vamp Statue"},
	{ITM_BUST, VAR_VAMPBUST + 2, "Vamp statue"},
	{ITM_BUST, VAR_VAMPBUST + 3, "Vamp Statue"},
	{ITM_BUST, VAR_VAMPBUST + 4, "Vamp Statue"},
	{ITM_BUST, VAR_VAMPBUST + 5, "Vamp Statue"},
	{ITM_BUST, VAR_VAMPBUST + 6, "Vamp Statue"},
	{ITM_BUST, VAR_VAMPBUST + 7, "Vamp Statue"},
	{ITM_CAT, VAR_CAT, "Cat"},
	{ITM_SUPERGEM, VAR_GEM + 0, "Big Gem"},
	{ITM_SUPERGEM, VAR_GEM + 1, "Big Gem"},
	{ITM_SUPERGEM, VAR_GEM + 2, "Big Gem"},
	{ITM_SUPERGEM, VAR_GEM + 3, "Big Gem"},
	{ITM_SUPERGEM, VAR_GEM + 4, "Big Gem"},
	{ITM_SUPERGEM, VAR_GEM + 5, "Big Gem"},
	{ITM_ZOMBGEM, VAR_ZOMBIEREWARD, "Zombie Reward"},
	{ITM_TRIPLEFIRE, VAR_TRIPLEFIRE, "3 way"},
	{ITM_TALISMAN, VAR_TALISMAN, "Happy Stick"},
	{ITM_BAT, VAR_BATSTATUE + 0, "Bat Statue"},
	{ITM_BAT, VAR_BATSTATUE + 1, "Bat Statue"},
	{ITM_BAT, VAR_BATSTATUE + 2, "Bat Statue"},
	{ITM_BAT, VAR_BATSTATUE + 3, "Bat Statue"},
	{ITM_LANTERN, VAR_LANTERN, "Lantern"},
	{ITM_REFLECTGEM, VAR_REFLECT, "Reflect"},
	{ITM_SILVERSLING, VAR_SILVERSLING, "Silver Sling"},
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
					CO_RETURN 1;
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
					genTries=0;
					if (!seed.empty()){
						std::seed_seq seed2(seed.begin(), seed.end());
						rng = std::default_random_engine(seed2);
					}
					while(!CheckBeatable(RandomFill())){
						genTries++;
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
				//AssumedFill();
			}
			break;
		case 1: // Typing in something
			c = mgl->LastKeyPressed();
			if (c == SDLK_ESCAPE || c == SDLK_RETURN) // ESC key
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
	std::string strTries = std::to_string(genTries);
	
	

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
	PrintColor(350, 120, strTries.c_str(), 7, -10, 0);
	if (cursor == 3)
	{
		PrintColor(239, 119, "Generate", 0, 0, 0);
		PrintColor(349, 119, strTries.c_str(), 7, -10, 0);
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


/*void AssumedFill()
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
	}

	//world.map[i]->special[y]

	//prefill section, keys to bosses

	//pick a random item
	//pick a random location

	//if its a quest, set entry in quest reward to item id
	//if its a physical spot, edit specials to use new item id

	//go until out of items

	//verify beatable
}*/

std::vector<location> RandomFill()
{

	/*if (std::filesystem::exists("rando.llw"))
	{
		std::filesystem::remove("rando.llw");
	}
	std::filesystem::copy("loony.llw", "rando.llw");*/

	std::FILE* baseWorld = AppdataOpen("loony.llw", "rb");
	std::FILE* newWorld = AppdataOpen("rando.llw", "wb");
	
	char buf[4096];
	while (int n = fread(buf, 1, 4096, baseWorld))
	{
    	fwrite(buf, 1, 4096, newWorld);
	}

	fclose(baseWorld);
	fclose(newWorld);

	//asset fopen steam from one to the other

	std::vector<location> remaining;
	for (location l : basic_locations)
	{
		remaining.push_back(l);
	}

	std::shuffle(remaining.begin(), remaining.end(), rng);

	std::FILE* f = AppdataOpen("spoiler.txt", "w");
	FilePtrStream spoilerFile(f);

	bool shortcircuit = false;
	for (int i = 0; i < remaining.size(); i++)
	{
		//make sure bonkula isn't holing a vamp statue or key
		if (remaining[i].description.compare("Bonkula") == 0 &&
			((itemList[i].playerVarId >= VAR_VAMPBUST && itemList[i].playerVarId <= VAR_VAMPBUST + 7) ||
			 itemList[i].playerVarId == VAR_SKULLKEY || itemList[i].playerVarId == VAR_BATKEY || itemList[i].playerVarId == VAR_PUMPKINKEY))
		{
			//spoilerFile.close();
			//return;
		}
		remaining[i].item = itemList[i];
		spoilerFile << remaining[i].mapId << "\t\t" << remaining[i].mapName << "\t\t" << remaining[i].description << "\t\t" << itemList[i].playerVarId << "\t\t" << itemList[i].itemName << "\n";
	}
	//fclose(f);
	return remaining;
}

bool CheckBeatable(std::vector<location> remaining){
	std::set<int> collectedItems;
	std::set<int> tempItems;
	std::vector<location> visited;
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
		visited.insert( visited.end(), remaining.begin(), remaining.end());
		PlaceItems(visited);
	}else{
		remove("spoiler.txt");
	}
	return gotEvilizer;
}

void PlaceItems(const std::vector<location>& loclist)
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
	world.map[0]->special[98].value = 0;
	//for each, go into the world
	
  	//questFile.open ("quest.txt");
	std::FILE* f = AppdataOpen("quest.txt", "w");
	FilePtrStream stream(f);
	

	for (location loc : loclist)
	{
		if (loc.isQuest){
			//quest list of map id points to loc.item now
			stream << loc.mapId << "\t" << loc.item.playerVarId << "\t" << loc.item.itemId  << "\n";

		}
		else
		{
			Map* tempMap = world.map[loc.mapId];
			tempMap->special[loc.s1].value = loc.item.playerVarId;
			tempMap->special[loc.s1].effectTag = 1;

			tempMap->special[loc.s2].trigValue = loc.item.playerVarId;			
			tempMap->map[loc.xcoord+loc.ycoord*tempMap->width].item=loc.item.itemId;
		}
	}

	//fclose(f);


	SaveWorld(&world, "rando.llw");

	//if quest, update rando quest table

	//if physical, place item and update specials
}

bool HaveLightSource(const std::set<int>& inv)
{
	return (inv.count(VAR_LANTERN) || inv.count(VAR_STICK) && inv.count(VAR_BOOTS) || inv.count(VAR_TORCH));
}

bool HaveAnyBigGem(const std::set<int>& inv)
{
	return (inv.count(VAR_GEM) || inv.count(VAR_GEM + 1) || inv.count(VAR_GEM + 2) ||
			inv.count(VAR_GEM + 3) || inv.count(VAR_GEM + 4) || inv.count(VAR_GEM + 5));
}

bool HaveAllOrbs(const std::set<int>& inv)
{
	return (inv.count(VAR_MYSORB) && inv.count(VAR_MYSORB + 1) && inv.count(VAR_MYSORB + 2) && inv.count(VAR_MYSORB + 3));
}

bool HaveAllBats(const std::set<int>& inv)
{
	return (inv.count(VAR_BATSTATUE) && inv.count(VAR_BATSTATUE + 1) && inv.count(VAR_BATSTATUE + 2) && inv.count(VAR_BATSTATUE + 3));
}

bool HaveAllVamps(const std::set<int>& inv)
{
	return (inv.count(VAR_VAMPBUST) && inv.count(VAR_VAMPBUST + 1) && inv.count(VAR_VAMPBUST + 2) && inv.count(VAR_VAMPBUST + 3) &&
			inv.count(VAR_VAMPBUST + 4) && inv.count(VAR_VAMPBUST + 5) && inv.count(VAR_VAMPBUST + 6) && inv.count(VAR_VAMPBUST + 7));
}

bool HaveAnySpecialWeapon(const std::set<int>& inv)
{
	return (inv.count(VAR_WEAPON) || inv.count(VAR_WEAPON + 1) || inv.count(VAR_WEAPON + 2) || inv.count(VAR_WEAPON + 4) ||
			inv.count(VAR_WEAPON + 5) || inv.count(VAR_WEAPON + 6));
}

bool HaveAllMushrooms(const std::set<int>& inv)
{
	return (inv.count(VAR_MUSHROOM) && inv.count(VAR_MUSHROOM + 1) && inv.count(VAR_MUSHROOM + 2) && inv.count(VAR_MUSHROOM + 3) &&
			inv.count(VAR_MUSHROOM + 4) && inv.count(VAR_MUSHROOM + 5) && inv.count(VAR_MUSHROOM + 6) && inv.count(VAR_MUSHROOM + 7) &&
			inv.count(VAR_MUSHROOM + 8) && inv.count(VAR_MUSHROOM + 9));
}

bool CanCleanseCrypts(const std::set<int>& inv)
{
	return (HaveLightSource(inv) && inv.count(VAR_BOOTS));
}
