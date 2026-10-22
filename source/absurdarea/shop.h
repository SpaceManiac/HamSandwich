#ifndef SHOP_H
#define SHOP_H

#include "mgldraw.h"
#pragma pack(1)

#define SHOP_WID		(4)
#define SHOP_HEI		(6)
#define SHOP_SIZE		(SHOP_WID*SHOP_HEI)	// shops hold 24 items

#define SHOP_BLACKMARKET (0)
#define SHOP_AXES		(1)
#define SHOP_TAILOR		(2)
#define SHOP_MAGIC		(3)	// amulets and potions
#define SHOP_ROPES		(4)
#define SHOP_SHROOM		(5)	// assorted stuff, like black market but cheap and unamazing
#define SHOP_STASH		(6)	// stash where you can stash things!
#define SHOP_MAX		(10)

void RenderShop(MGLDraw *mgl);
byte UpdateShop(MGLDraw *mgl);
void InitShop(byte type);

void SortShop(byte shop);
void SetupShopLists(void);
void UpdateShopInventories(void);

#endif
