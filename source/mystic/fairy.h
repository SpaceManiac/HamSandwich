#ifndef FAIRY_H
#define FAIRY_H

#include "mgldraw.h"

#define FAIRY_NONE		(0)
// chapter I
#define FAIRY_GRABBY	(1)	// picks up coins that touch him, costs you XP
#define FAIRY_RUNNY		(2)	// makes you run faster than normal, but take 25% more damage when hit.
#define FAIRY_CASTY		(4)	// makes your spells act like they are 5 levels higher than they are, but cost 1 HP per cast as well as the mana
#define FAIRY_BLOCKY	(8)	// take 75% damage when hit, but only inflict 75% damage.
// chapter II
#define FAIRY_VAMPY		(16) // gain 1 HP each time you hit someone with a fireball, but lose 1 HP per 3 seconds
#define FAIRY_SMASHY	(32) // inflict 1.5x damage, but each fireball costs 1 MP to shoot (can't fire at all when out, or maybe normal damage?)
#define FAIRY_CHEAPY	(64) // spells cost half as much, but you can't fire fireballs
#define FAIRY_TOUGHY	(128) // take 50% damage, but move half speed
// chapter III
#define FAIRY_RICHEY	(256)	// enemies drop more coins than normal
#define FAIRY_SMARTY	(512)	// you earn 25% more XP per kill, but lose mana when hit as well as life (1/2 as much)
#define FAIRY_HEALEY	(1024)	// if your life is below 50%, it recovers slowly
#define FAIRY_SAVEY		(2048)	// if you die, he resurrects you where you died, with 1/4 health and mana, invincible for 1 second (only one resurrect per level)
// chapter IV
#define FAIRY_SHOOTY	(4096)	// fires to help you out, when you do (so you need hammers)
#define FAIRY_HAGGLY	(8192)	// you get 10% off at the shop
#define FAIRY_SCARY		(16384)	// makes monsters run away from you in the overworld if you get near
#define FAIRY_MIGHTY	(32768)	// your life and mana drain semi-slowly, but you move 25% faster, and do 25% more damage, and take 25% less damage, and spells cost 10% less

void EnterFairyBox(void);
void LeaveFairyBox(void);
void RenderFairyBox(void);
byte UpdateFairyBox(MGLDraw *mgl);
void FairyCaughtText(byte w);

#endif
