#ifndef ITEMS_H
#define ITEMS_H

#include "winpch.h"
#include "jamulspr.h"

#define ITM_NONE     0
#define ITM_HAMMERUP 1
#define ITM_PANTS	 2
#define ITM_REVERSE  3
#define ITM_REFLECT  4
#define ITM_MISSILES 5
#define ITM_AK8087	 6
#define ITM_TAKEOUT  7
#define ITM_SHIELD   8
#define ITM_BOMBS	 9
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
#define ITM_BIGAXE	 21
#define ITM_PWRARMOR 22
#define ITM_LIGHTNING 23
#define ITM_SPEAR	 24
#define ITM_MACHETE	 25
#define ITM_MINES	 26
#define ITM_GARLIC	 27
#define ITM_ORBITER  28
#define ITM_ACCEL	 29

#define ITM_SMLROCKS 30
#define ITM_HOLETREE 31
#define ITM_IGLOO	 32
#define ITM_WEB		 33
#define ITM_WEB2	 34
#define ITM_WEB3	 35
#define ITM_WEB4	 36
#define ITM_GRASS	 37
#define ITM_GRASS2	 38
#define ITM_VILLAGE  39

#define ITM_BOX		 50
#define ITM_STUMP	 51
#define ITM_BUSH	 52
#define ITM_BIGROCKS 53
#define ITM_POST	 54
#define ITM_CHAIR1	 55
#define ITM_CHAIR2   56
#define ITM_WALLGRASS 57
#define ITM_BARREL	 58
#define ITM_BARREL2	 59
#define ITM_BARREL3	 60
#define ITM_TRASHCAN 61
#define ITM_TRASHCAN2 62
#define ITM_CRATE	 63
#define ITM_CRATE2	 64
#define ITM_BUSH2	 65
#define ITM_BUSH3	 66
#define ITM_BRWNROCK 67

#define ITM_PALM	 69
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
#define ITM_PINE	 80
#define ITM_DEADTREE 81
#define ITM_DEADTREE2 82
#define ITM_FATPALM	 83
#define ITM_TREE2	 84
#define ITM_MINEBLOCK 85
#define ITM_SIGN2	 86
#define ITM_SIGN3	 87
#define ITM_SIGN4	 88
#define ITM_SIGN5	 89
#define ITM_SIGN6	 90
#define ITM_BIGROCK	 91

#define ITM_TURRETWPN 92
#define ITM_MINDCONTROL 93
#define ITM_REFLECTOR 94
#define ITM_INVIS	  95
#define ITM_JETPACK	  96
#define ITM_UNHAMMER  97
#define ITM_UNPANTS	  98
#define ITM_SWAPGUN	  99
#define ITM_BADCHINESE 100

#define MAX_ITMS	 128

// these defines are 1 greater than the highest item of that type
// (that is, items 1 - X-1 are things the player can pick up, and items
// X - Y-1 are walkable things that don't obstruct movement, and items
// Y - Z-1 are not walkable but don't stop projectiles, and the remaining
// items are obstacles.
#define MAX_PICKUP_ITMS    30
#define MAX_WALKABLE_ITMS  50
#define MAX_SHOOTABLE_ITMS 69
#define NEW_PICKUP_ITMS	   92

void InitItems(void);
void ExitItems(void);
void RenderItem(int x,int y,byte type,char bright);
void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl);
void DrawRedX(int x,int y,MGLDraw *mgl);
void ItemLightUp(void);

#endif
