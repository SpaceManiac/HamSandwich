#ifndef GIFTSHOP_H
#define GIFTSHOP_H

#include "mgldraw.h"

#define GIFT_EDITOR		0
#define GIFT_CLEARANCE	1
#define GIFT_BLOWOUT	2
#define GIFT_PEST		3
#define GIFT_PIG		4
#define GIFT_GOTH		5
#define GIFT_CHERUB		6
#define GIFT_ROBOT		7
#define GIFT_SANTA		8
#define GIFT_SHROOM		9
#define GIFT_SMARCH		10
#define GIFT_FREAKO		11
#define GIFT_MUSIC		12
#define GIFT_PARALLEL	13
#define GIFT_STICK		14
#define GIFT_EXIT		15

TASK(void) GiftShop(MGLDraw *mgl);

#endif
