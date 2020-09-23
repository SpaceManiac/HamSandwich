#ifndef CARD_H
#define CARD_H

#include "mgldraw.h"
#include "monster.h"
//aaaa

typedef struct cardsType_t
{
	word id;
	byte rarity;
} cardsType_t;

void CardMenu(MGLDraw *mgl);
cardsType_t *GetCardID(dword type);
int GetMonsterCard(dword g);
int GetMonsterCardChance(dword g);
int GetMonsterCardRarity(dword g);

//CARD LIST:
#define NUM_CARDS 332

#endif