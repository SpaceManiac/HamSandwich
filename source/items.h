#ifndef ITEMS_H
#define ITEMS_H

#include "winpch.h"
#include "jamulspr.h"

enum {
    ITM_NONE = 0,
    ITM_HAMMERUP = 1,
    ITM_PANTS = 2,
    ITM_REVERSE = 3,
    ITM_REFLECT = 4,
    ITM_MISSILES = 5,
    ITM_AK8087 = 6,
    ITM_TAKEOUT = 7,
    ITM_SHIELD = 8,
    ITM_BOMBS = 9,
    ITM_FLAME = 10,
    ITM_BRAIN = 11,
    ITM_KEYCH1 = 12,
    ITM_KEYCH2 = 13,
    ITM_KEYCH3 = 14,
    ITM_KEYCH4 = 15,
    ITM_KEY = 16,
    ITM_KEYR = 17,
    ITM_KEYG = 18,
    ITM_KEYB = 19,
    ITM_LOONYKEY = 20,
    ITM_BIGAXE = 21,
    ITM_PWRARMOR = 22,
    ITM_LIGHTNING = 23,
    ITM_SPEAR = 24,
    ITM_MACHETE = 25,
    ITM_MINES = 26,
    ITM_GARLIC = 27,
    ITM_ORBITER = 28,
    ITM_ACCEL = 29,

    ITM_SMLROCKS = 30,
    ITM_HOLETREE = 31,
    ITM_IGLOO = 32,
    ITM_WEB = 33,
    ITM_WEB2 = 34,
    ITM_WEB3 = 35,
    ITM_WEB4 = 36,
    ITM_GRASS = 37,
    ITM_GRASS2 = 38,
    ITM_VILLAGE = 39,

    ITM_BOX = 50,
    ITM_STUMP = 51,
    ITM_BUSH = 52,
    ITM_BIGROCKS = 53,
    ITM_POST = 54,
    ITM_CHAIR1 = 55,
    ITM_CHAIR2 = 56,
    ITM_WALLGRASS = 57,
    ITM_BARREL = 58,
    ITM_BARREL2 = 59,
    ITM_BARREL3 = 60,
    ITM_TRASHCAN = 61,
    ITM_TRASHCAN2 = 62,
    ITM_CRATE = 63,
    ITM_CRATE2 = 64,
    ITM_BUSH2 = 65,
    ITM_BUSH3 = 66,
    ITM_BRWNROCK = 67,

    ITM_PALM = 69,
    ITM_TREE = 70,
    ITM_DOOR1 = 71,
    ITM_DOOR1R = 72,
    ITM_DOOR1G = 73,
    ITM_DOOR1B = 74,
    ITM_DOOR2 = 75,
    ITM_DOOR2R = 76,
    ITM_DOOR2G = 77,
    ITM_DOOR2B = 78,
    ITM_SIGN = 79,
    ITM_PINE = 80,
    ITM_DEADTREE = 81,
    ITM_DEADTREE2 = 82,
    ITM_FATPALM = 83,
    ITM_TREE2 = 84,
    ITM_MINEBLOCK = 85,
    ITM_SIGN2 = 86,
    ITM_SIGN3 = 87,
    ITM_SIGN4 = 88,
    ITM_SIGN5 = 89,
    ITM_SIGN6 = 90,
    ITM_BIGROCK = 91,

    ITM_TURRETWPN = 92,
    ITM_MINDCONTROL = 93,
    ITM_REFLECTOR = 94,
    ITM_INVIS = 95,
    ITM_JETPACK = 96,
    ITM_UNHAMMER = 97,
    ITM_UNPANTS = 98,
    ITM_SWAPGUN = 99,
    ITM_BADCHINESE = 100
};

const int MAX_ITMS = 128;

// these defines are 1 greater than the highest item of that type
// (that is, items 1 - X-1 are things the player can pick up, and items
// X - Y-1 are walkable things that don't obstruct movement, and items
// Y - Z-1 are not walkable but don't stop projectiles, and the remaining
// items are obstacles.
const int MAX_PICKUP_ITMS = 30;
const int MAX_WALKABLE_ITMS = 50;
const int MAX_SHOOTABLE_ITMS = 69;
const int NEW_PICKUP_ITMS = 92;

void InitItems(void);
void ExitItems(void);
void RenderItem(int x, int y, byte type, char bright);
void InstaRenderItem(int x, int y, byte type, char bright, MGLDraw *mgl);
void DrawRedX(int x, int y, MGLDraw *mgl);
void ItemLightUp(void);

#endif
