#ifndef SPECIAL_H
#define SPECIAL_H

#include "mgldraw.h"
#include <stdio.h>

#define MAX_SPECIAL	128
#define MAX_EVENT   128

//special triggers
#define TRG_NONE		0
#define TRG_STEP		1	// true if creature step within N tiles of X,Y (N=0, exactly X,Y)
#define TRG_STEPRECT	2	// true if creature step in rect
#define TRG_HAVEITEM	3	// true if player has N item M or less/more/exactly
#define TRG_SHOOT		4	// true if shoot item/wall at X,Y
#define TRG_MONSTER		5	// true if there are N creature or less/more/exactly
#define TRG_KILL		6	// true at the moment a creature of type N dies
#define TRG_FLOOR		7	// true if floor/roof at X,Y is N
#define TRG_PASSLEVELS	8	// true if passed N levels or less/more/exactly
#define TRG_PASSLEVEL	9	// true if passed level N
#define TRG_VAR			10	// true if var V is N or less/more/exactly
#define TRG_TIMED		11	// true once every F seconds
#define TRG_DELAY		12	// true endlessly after F seconds have passed
#define TRG_RANDOM		13	// true if Random(100)<=N
#define TRG_CHAIN		14	// true if special at X,Y is triggered
#define TRG_FLOORRECT	15	// true if all tiles within rect are floor N
#define TRG_LIFE		16	// true if monster of type N at X,Y has life M or less/more/exactly
#define TRG_STEPTILE	17	// true if creature step on tile N
#define TRG_GETITEM		18	// true if get item at X,Y
#define TRG_ITEM		19	// true if certain item is at X,Y
#define TRG_AWAKE		20	// true if monster of type N at X,Y is awake
#define TRG_ITEMS		21	// true if there are N of item M or less/more/exactly in the level
#define TRG_COMPMAP		22	// true if the region (x,y-value/,value%) is identical to (value2/,value2%,same size), including badguys
#define TRG_COMPVAR		23	// true if var1 is less/more/exactly var2
#define TRG_MONSINRECT	24	// true if there are any of monster X in rect
#define TRG_ITEMRECT	25	// true if there are any of item X in rect
#define TRG_DIFFICULTY	26	// true if the game is set to the chosen difficulty or less/more/exactly
#define TRG_KEYPRESS	27	// true if player presses/holds/releases button N
#define TRG_PLAYAS		28	// true if playing as character X
#define TRG_MONSCOLOR	29	// true if monster of type N at X,Y has toColor M
#define TRG_EQUATION	30	// true if N is EQUATION or less/more/exactly (equation is stored in corresponding effect slot!)
#define TRG_EQUVAR		31	// true if var V1 is EQUATION or less/more/exactly (equation is stored in corresponding effect slot!)
#define TRG_BULLETRECT  32  // true if there are any of bullet X in rect
#define TRG_TIMER 		33  // true if timer equals X
#define TRG_HURT 		34  // true if monster of type N is HURT! OUCH
#define TRG_XTRAMODE 	35  // true if player is playing on supreme/hyper mode
#define TRG_CONDITION 	36  // true if monster of type N at X,Y has condition M
#define TRG_RAGED		37  // true if player has RAGED!
#define TRG_SHOTBULLET	38  // true if shoot item/wall at X,Y but with a specific bullet.
#define TRG_MONSAGE		39  // true if monster of type N at X,Y is M seconds old
#define TRG_STANDTILE	40  // true if creature is standing on tile
#define MAX_TRIGGER		41

// effects
#define EFF_NONE		0
#define EFF_MESSAGE		1	// show message M
#define EFF_SOUND		2	// play sound #N
#define EFF_SONG		3	// play song #N
#define EFF_WINLEVEL	4	// win this level, going to map N, at coords X,Y
#define EFF_GOTOMAP		5	// just go to map N, at coords X,Y
#define EFF_TELEPORT	6	// Teleport person N to X,Y (N=player, spcl target)
#define EFF_CHANGETILE	7	// change single/contiguous tile at X,Y to wall N, floor Z... toggle flag available
#define EFF_SUMMON		8	// summon monster N at X,Y
#define EFF_LIGHT		9	// permanent/temporary light at X,Y, brightness N (negative=darkness), radius Z (toggle flag determines permanent/temp)
#define EFF_PICTURE		10	// show picture/flic/text file M
#define EFF_ITEM		11	// set single/contiguous item at X,Y to N.  Toggle available
#define EFF_SWAPMAP		12	// swap/copy rect (x,y-value/,value%) with rect (value2/,value2%,same size)
#define EFF_KILLMONS	13	// kill monsters of type N at X,Y (or any X,Y), N can be "goodguy, badguy", etc
#define EFF_CHANGEMONS	14	// change monsters of type N at X,Y (or any X,Y) to type Z, N can be 'any', etc, Z must be specific
#define EFF_CHANGETEAM	15	// set monsters of type N at X,Y (or any X,Y) to team Z (good, evil, or toggle).
#define EFF_DELETESPCL	16	// delete special at X,Y
#define EFF_VAR			17	// set var N to equation M
#define EFF_LIGHTRECT	18	// light up rectangle (x,y-value/,value%) to value2
#define EFF_LEVELFLAG	19	// change level flags
#define EFF_OLDTOGGLE	20	// toggle the old Dr. L way - just picking a wall/floor tile to swap to
#define EFF_LIFE		21	// set monster of type N at X,Y life to Z (maximum life, or current life)
#define EFF_WEAPON		22	// force weapon to N
#define EFF_TAGTARGET	23	// tag the target
#define EFF_TAGMONS		24	// tag the monster of type N at X,Y
#define EFF_MONSITEM	25	// change monster of type N at X,Y to be holding item Z
#define EFF_TILEVAR		26	// change single/contiguous/allsame tile at X,Y to floor V, where V is a variable.
#define EFF_LIFEAMT		27	// change life of monster type N at X,Y by N (+ or -)
#define EFF_AI			28	// change monster on type N at X,Y to AI of monster M
#define EFF_NAME		29	// change monster of type N at X,Y name to S
#define EFF_COLOR		30	// change monster of type N at X,Y coloration
#define EFF_MONSBRIGHT	31	// change monster of type N at X,Y to brightness M
#define EFF_PLAYAS		32	// set the player's Play As for this level only
#define EFF_MONSGRAPHICS	33	// change monster of type N at X,Y to graphics M
#define EFF_ITEMGRAPHICS	34	// set the current custom item graphics to the given jsp
#define EFF_VARBAR		35	// edit the varbar below the rage bar
#define EFF_MAKEBULLET	36	// summon a bullet at a location
#define EFF_CHANGEBULLET	37	// change bullet of type N at X,Y (or any X,Y) to type Z, N can be any or specific, Z must be specific
#define EFF_CHANGETIMER	38	// add timer by N OR set timer to N.
#define EFF_CAMPOINT	39	// focus camera on point x,y
#define EFF_CAMINST		40	// focus camera on monster of type N at X,Y
#define EFF_DYNAMICCOL	41	// change color of dynamic water
#define EFF_DYNAMICSCRN	42	// change image of dynamic water
#define EFF_CHARTRANS	43	// transform player into monster of type N with M/M HP
#define EFF_TAGBOSS		44	// set the target as boss
#define EFF_GOTOMAP2	45	// just go to map N, at coords X,Y
#define EFF_MAX			46

// trigger flags
#define TF_NOT		1	// NOT this one
#define TF_AND		2	// AND this one with the next (if not selected, it's OR)
#define TF_LESS		4	// in triggers with "less/more/exactly", less \ both off means exactly
#define TF_MORE		8	// in triggers with "less/more/exactly", more / both on is not valid!

// effect flags
#define EF_CONTIGUOUS	1	// contiguous effect, instead of single
#define EF_PLAYER		1	// uses the same slot - affects player rather than special target
#define EF_PERMLIGHT	1	// uses the same slot - permanent light rather than temporary
#define EF_ALL			2	// instead of contiguous or single, this hits ALL tiles/items of the same type
#define EF_TAGGED		2	// uses the same slot - affects the tagged monster instead of player or target
#define EF_TOGGLE		4	// automatically modifies itself for repeat uses to toggle
#define EF_NOFX			8	// doesn't show visual or audio effects when done

// numbers in one special
#define NUM_TRIGGERS	7
#define NUM_EFFECTS		31

// events that can trigger specials
#define EVT_STEP		1	// somebody just entered a new square
#define EVT_SHOOT		2	// a shot just hit something
#define EVT_DIE			3	// a monster just died
#define EVT_CHANGE		4	// a monster changed
#define EVT_SPECIAL		5	// a special occurred
#define EVT_GET			6	// an item was picked up
#define EVT_STAND		7	// an item was picked up
#define MAX_EVENTTYPES	8

typedef struct trigger_t
{
	byte flags;		// whether it must AND or OR with the next one, and if it's NOT
	byte type;
	byte x,y;	// coordinates of where it's triggered
	int value;	// value of "N" in the trigger
	int value2;	// another value for some triggers
} trigger_t;

typedef struct effect_t
{
	byte flags;
	byte type;
	byte x,y;
	int value;
	int value2;
	char  text[32];
} effect_t;

typedef struct special_t
{
	byte x,y;	// where this special is actually kept
	byte uses;
	trigger_t trigger[NUM_TRIGGERS];
	effect_t effect[NUM_EFFECTS];
} special_t;

class Guy;

typedef struct sEvent_t
{
	byte type;
	int  value;	// guy #, bullet #, var #, item #
	int  x,y;
	int guyType;
	byte guyFriendly;
	Guy *victim;
} sEvent_t;

struct world_t;

void InitSpecials(special_t *list);
void GetSpecialsFromMap(special_t *list);
int NewSpecial(byte x,byte y);
void DefaultTrigger(trigger_t *trig,int x,int y);
void DefaultEffect(effect_t *eff,int x,int y,byte savetext);

int GetSpecial(byte x,byte y);
special_t *GetSpecial(int i);
void SaveSpecials(FILE *f);
void LoadSpecials(FILE *f,special_t *list);
void DeleteSpecial(int i);

class Map;

void SlideSpecials(Map *map,int dx,int dy);
int SlideCoord(int coord,int delta,int max);
void SlideCombinedCoord(int *combo,int dx,int dy,int width,int height);
void RepairSpecialToItem(special_t *list,int n);	// repair all references to items in
													// specials when item N is deleted
void RepairSpecialToSound(special_t *list,int n);	// repair all references to sounds in
													// specials when sound N is deleted
void RepairSpecialToLevel(special_t *list);	// repair all references to levels in
											// specials when the levels are moved around
void RepairSpecialToTile(special_t *list);	// repair tile references

byte CheckSpecial(special_t *me);	// returns 0 if the special has no triggers, 1 otherwise

special_t *SpecialPointer(void);

void PrintSpecialComment(int x,int y,int mx,int my);

// --- Gameplay
void InitSpecialsForPlay(void);
void CheckSpecials(Map *map);
void EventOccur(byte type,int value,int x,int y,Guy *victim);
void RenderSpecialXes(Map *map);
void AdjustSpecialCoords(special_t *me,int dx,int dy);
void AdjustSpecialEffectCoords(special_t *me,int dx,int dy);
Guy *TaggedMonster(void);
Guy *BossMonster(void);

#endif
