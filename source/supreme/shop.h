#ifndef SHOP_H
#define SHOP_H

#include "mgldraw.h"

#define SHOP_LOCKERS	0
#define SHOP_SPOOKY		1
#define SHOP_SPACE		2
#define SHOP_WATER		3
#define SHOP_CAVE		4
#define SHOP_FOREST		5
#define SHOP_WEIRD		6
#define SHOP_ADVENTURE	7
#define SHOP_ICE		8
#define SHOP_KIDS		9
#define SHOP_DESERT		10
#define SHOP_GADGET		11
#define SHOP_RANDOM		12
#define SHOP_HUNGER		13

// purchase types
#define SHOP_WORLD		0
#define SHOP_MONEY		1
#define SHOP_CHEAT		2
#define SHOP_EMPTY		3
#define SHOP_ABILITY	4
#define SHOP_MAJOR		5	// major things like modes, the gallery, etc
#define SHOP_GOAL		6	// gallery goal to open this locker
#define SHOP_PLAYABLE	7
#define SHOP_MODE		8	// toggleable goofy things in the lockers
#define SHOP_DISCOUNT	9	// discount card for a store
#define SHOP_DONATION	10	// the donation at the hunger sign

// abilities & menu modes
#define ABIL_BRAIN		1	// brain radar
#define ABIL_CANDLE		2	// candle radar
#define ABIL_RAGE		3	// rage
#define ABIL_MOVESHOOT	4	// move n' shoot
#define ABIL_KEYCHAIN	5	// keychain helper
#define ABIL_HYPER		6	// hyper mode
#define ABIL_SUPREME	7	// supreme mode

// major items
#define MAJOR_THEATER	1
#define MAJOR_GALLERY	2
#define MAJOR_CHEATMENU	3
#define MAJOR_RECORDBOOK 4
#define MAJOR_ARCADE2	5
#define MAJOR_EDITOR	6
#define MAJOR_QUIZ		7
#define MAJOR_ARCADE	8
#define MAJOR_BESTIARY	9
// new major items
#define MAJOR_LOCKERS	10
#define MAJOR_GALLERY2	11
#define MAJOR_CARDBOOK	12
#define MAJOR_GREEDY	13
#define MAJOR_EDPACK1	14
#define MAJOR_ARCADE3	15

// playables
#define PLAY_BOUAPHA	0
#define PLAY_HAPPY		1
#define PLAY_LUNATIC	2
#define PLAY_SHROOM		3
#define PLAY_LUNACHIK	4
#define PLAY_MECHA		5
#define PLAY_WOLF		6
#define PLAY_WIZ		7
#define PLAY_MYSTIC		8
#define PLAY_LOONY		9

// shop item flags (used in the player progress file)
#define SIF_AVAILABLE	1
#define SIF_BOUGHT		2
#define SIF_ACTIVE		4	// for locker items that can be toggled, is it on or off?

// modes you can buy in lockers
#define MODE_DISCO		0
#define MODE_RASTER		1
#define MODE_LUDICROUS	2
#define MODE_REVERSE	3
#define MODE_SPLATTER	4
#define MODE_MANIC		5
#define MODE_TEENY		6
#define MODE_VIRTUAL	7

typedef struct shopItem_t
{
	byte shop;
	word cost;
	byte type;
	byte item;	// which # of the items of "type" it is
} shopItem_t;

class Map;

byte CanPlayWorld(const char *fname);
byte ItemPurchased(byte type,byte num);
byte AllOfTypePurchased(byte type);
byte AllPurchased(void);
byte AllLockersOpen(void);

byte ShopItemNumber(byte type,byte num);
void SetupShops(Map *map);
void DefaultShopAvailability(void);

void InitShopping(int x,int y);
byte UpdateShopping(MGLDraw *mgl);
void RenderShopping(MGLDraw *mgl);

float ShopPercent(void);
void RandomizeLockers(void);

#endif
