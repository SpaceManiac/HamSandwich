#ifndef SHOP_H
#define SHOP_H

#include "jamultypes.h"
#include "jamulspr.h"

void InitShop(void);
void ExitShop(void);
sprite_t *GetShopSpr(byte spr);
void RenderPlayerGear(int x,byte brt);

byte UpdateShop(MGLDraw *mgl);
void RenderShop(void);

void EnterShop(void);
void LeaveShop(void);

#endif
