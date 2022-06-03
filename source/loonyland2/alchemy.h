#ifndef ALCHEMY_H
#define ALCHEMY_H

#include "winpch.h"
#include "map.h"
#include "items.h"
#include "equip.h"

#define POT_BRAINJUICE	(IT_POT1)
#define POT_MUDMILK		(IT_POT2)
#define POT_STONESOUP	(IT_POT3)
#define POT_LIFEADE		(IT_POT4)
#define POT_REDCOW		(IT_POT5)
#define POT_SOULA		(IT_POT6)
#define POT_DEFLECTOR	(IT_POT7)
#define POT_ALIENBLOOD	(IT_POT8)
#define POT_FEARBEER	(IT_POT9)
#define POT_JOLTING		(IT_POT10)
#define POT_SPARKLING	(IT_POT11)
#define POT_SMARTSAUCE	(IT_POT12)
#define POT_LUCKENBRAU	(IT_POT13)
#define POT_MONSTER		(IT_POT14)
#define POT_HEALENBRAU	(IT_POT15)
#define POT_MAGICOLA	(IT_POT16)
#define POT_STAMINO		(IT_POT17)
#define POT_ROCKETSAUCE	(IT_POT18)
#define POT_EXPLOSIVO	(IT_POT19)
#define POT_COOLADE		(IT_POT20)

void PlantHerbs(Map *map,world_t *world);
void GuyDropPotion(byte level,byte *type,int *val1,int *val2);
int PotionEffect(byte type,byte lvl);
int PotionDuration(byte type,byte lvl);
void DrinkPotion(byte type,byte lvl);
word PotionCost(equip_t *e);
void MixPotion(byte ing1,byte ing2,byte ing3);

#endif
