#ifndef ITEMS_H
#define ITEMS_H

#include "winpch.h"
#include "jamulspr.h"

#define ITM_NONE     0
#define ITM_HAMMERUP 1
#define ITM_PANTS	 2
#define ITM_REVERSE  3
#define ITM_REFLECT  4
#define ITM_SPELLBOOK 5
#define ITM_HEALTHPOT 6
#define ITM_MANAPOT  7
#define ITM_BIGCOIN  8
#define ITM_COIN	 9
#define ITM_FLAME	 10
#define ITM_BRAIN	 11
#define ITM_KEYCH1	 12
#define ITM_KEYCH2	 13
#define ITM_KEYCH3	 14
#define ITM_KEYCH4	 15
#define ITM_KEY		 16
#define ITM_KEYR     17
#define ITM_KEYG	 18
#define ITM_KEYB	 19
#define ITM_LOONYKEY 20
#define ITM_FAIRYBELL 21
#define ITM_LETTERM  22
#define ITM_LETTERY	 23
#define ITM_LETTERS  24
#define ITM_LETTERT	 25
#define ITM_LETTERI  26
#define ITM_LETTERC	 27
#define ITM_CHLGCRYSTAL 28

#define ITM_ARCHIPELAGO 29

#define ITM_SMLROCKS 30
#define ITM_HOLETREE 31
#define ITM_IGLOO	 32

#define ITM_BOX		 50
#define ITM_STUMP	 51
#define ITM_BUSH	 52
#define ITM_BIGROCKS 53
#define ITM_POST	 54
#define ITM_CHAIR1	 55
#define ITM_CHAIR2   56

#define ITM_SKLDOOR	 69
#define ITM_TREE	 70
#define ITM_DOOR1	 71
#define ITM_DOOR1R	 72
#define ITM_DOOR1G	 73
#define ITM_DOOR1B	 74
#define ITM_DOOR2	 75
#define ITM_DOOR2R	 76
#define ITM_DOOR2G	 77
#define ITM_DOOR2B	 78
#define ITM_SIGN	 79
#define ITM_SKLDOOR2 80
#define ITM_HUT	     81
#define ITM_SHOP     82
#define ITM_TARGET	 83

#define MAX_ITMS	 84

// these defines are 1 greater than the highest item of that type
// (that is, items 1 - X-1 are things the player can pick up, and items
// X - Y-1 are walkable things that don't obstruct movement, and items
// Y - Z-1 are not walkable but don't stop projectiles, and the remaining
// items are obstacles.
#define MAX_PICKUP_ITMS    30
#define MAX_WALKABLE_ITMS  50
#define MAX_SHOOTABLE_ITMS 69

void InitItems(void);
void ExitItems(void);
void RenderItem(int x,int y,byte type,char bright);
void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl);
void DrawRedX(int x,int y,MGLDraw *mgl);
void UpdateItems(void);
sprite_t *GetItemSprite(word spr);

#endif
