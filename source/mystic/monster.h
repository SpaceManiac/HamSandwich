#ifndef MONSTER_H
#define MONSTER_H

/* this file does the AI and everything else for each monster type.
   It knows all about the different types, as opposed to guy.cpp which
   just sort of keeps track of the list of guys. */

#include "winpch.h"
#include "jamulspr.h"
#include "display.h"

// the monster types
#define MONS_NONE     0
#define MONS_BOUAPHA  1
#define MONS_BONEHEAD 2
#define MONS_BAT	  3
#define MONS_SPIDER	  4
#define MONS_BIGSPDR  5
#define MONS_ZOMBIE	  6
#define MONS_EGGSAC	  7
#define MONS_MAMASPDR 8
#define MONS_PYGMY	  9
#define MONS_SERPENT  10
#define MONS_FRIENDLY 11
#define MONS_PTERO    12
#define MONS_EYEGUY   13
#define MONS_PEEPER   14
#define MONS_TOWER    15
#define MONS_LOOKEYLOO 16
#define MONS_PINKEYE 17
// more normal monsters
#define MONS_SHRMLORD 18
#define MONS_MICRO  19
#define MONS_GOLEM 20
#define MONS_MOSS	  21
#define MONS_MOSSGRANDE 22
#define MONS_MAGMAZOID 23
#define MONS_SHROOM	  24
#define MONS_MUSH	  25
// the parts of The Thing
#define MONS_THING	  26
#define MONS_THINGTENT 27
#define MONS_THINGTENTTIP 28
// normal monsters again
#define MONS_HUGEBAT 29
#define MONS_FAIRY 30
#define MONS_BIGBAT 31
#define MONS_BOBBY  32
#define MONS_FAIRY2 33
#define MONS_CRAZYBUSH 34
#define MONS_OCTOPUS  35
// the Yeti Bros.
#define MONS_INCABOT  36
#define MONS_INCAGEN  37
// normal monsters again
#define MONS_INCAGOLD 38
#define MONS_INCAGOLD2 39
#define MONS_LOG	  40
#define MONS_INCABOSS  41
#define MONS_INCATONGUE 42
#define MONS_PEEPBOMB  43
#define MONS_LICH	  44
#define MONS_TRAPAZOID 45
#define MONS_TRAPAZOID2 46	// trapazoid in dud mode, not gonna pop up
// Sphinx parts
#define MONS_PEEPBOMB2	47	// peepbomb that turns the other direction
#define MONS_WHACKAZOID	48
#define MONS_SLUG   49
// more normal monsters
#define MONS_SNAIL	 50
#define MONS_SNAIL2 51	// in shell
#define MONS_BALL	 52
#define MONS_OCTOBOSS  53
#define MONS_OCTOTENT  54
#define MONS_OCTOTENT2  55
#define MONS_GOAT1	 56	// rammy
#define MONS_GOAT1B	  57	// rammy stunned/vulnerable
#define MONS_GOAT2  58	// hoppy
#define MONS_GOAT2B	  59	// hoppy airborne/invincible
#define MONS_GOAT3	 60	// gruffy
#define MONS_STICKSHROOM 61
#define MONS_STICKSPIDER 62
#define MONS_STICKCORPSE 63
#define MONS_STICKBAT	 64
#define MONS_DANCER		65

#define NUM_MONSTERS  66

// the animations
#define ANIM_IDLE	0
#define ANIM_MOVE	1
#define ANIM_ATTACK 2
#define ANIM_DIE	3
#define ANIM_A1		4
#define ANIM_A2		5
#define ANIM_A3		6
#define ANIM_A4		7
#define ANIM_A5		8
#define NUM_ANIMS	9

#define ANIM_LENGTH	32

// flags
#define MF_FLYING	 1
#define MF_WATERWALK 2
#define MF_ONEFACE	 4
#define MF_ENEMYWALK 8	// other enemies can stomp all over this one (but not Bouapha)
#define MF_NOMOVE	 16 // doesn't move when hit
#define MF_AQUATIC	 32	// can ONLY move on water/lava, not land
#define MF_INVINCIBLE 64 // totally invulnerable to harm
#define MF_SPRITEBOX 128 // use the sprite's rect for collision checks instead of standard size-box method
#define MF_FACECMD	 256 // this monster's "facing" value should just be added to the sprite number,
						 // it's calculated by his AI (only useful for MF_ONEFACE monsters)
#define MF_NOGRAV	 512
#define MF_FREEWALK	 1024 // Bouapha can walk right through it
#define MF_WALLWALK	 2048 // walk through walls
#define MF_NOSHADOW	 4096 // doesn't cast a shadow
#define MF_GHOST	 8192 // draw using ghost draw
#define MF_GOODGUY   16384	// non-evil
#define MF_NOHIT	 32768	// shots go through it

typedef struct monsterType_t
{
	char name[32];
	byte size;
	byte framesPerDir;
	word hp;
	word points;
	char sprName[32];
	sprite_set_t *spr;
	word flags;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
} monsterType_t;

extern monsterType_t monsType[NUM_MONSTERS];

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

byte MonsterSize(byte type);
byte *MonsterAnim(byte type,byte anim);
word MonsterFlags(byte type);
byte MonsterFrames(byte type);
word MonsterHP(byte type);
word MonsterPoints(byte type);
char *MonsterName(byte type);
void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte mind1,byte ouch,word frozen);
void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl);
void InstaRenderMonster2(int x,int y,byte type,byte seq,byte frm,byte facing,byte mind1,char bright,MGLDraw *mgl);
sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing);
void SetWaterwalk(word on);
//int RangeToTarget(Guy *me,Guy *goodguy);
sprite_t *GetMonsterSprite2(byte type,byte frm);
byte GetMonsterFrameNum(byte type,byte seq,byte frm,byte facing);
void SetKidSprite(byte armageddon);

void FaceGoodguy(Guy *me,Guy *goodguy);

// ai functions for each monster type
void AI_Overworld(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ptero(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EggSac(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MamaSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Serpent(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shroomlord(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Microfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Magmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mush(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Isozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Penguin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Geozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mumble(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cactus(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lich(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Boiler(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Santa(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Peeper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Eyeguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Tower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pinkeye(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lookeyloo(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Friendly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Golem(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HugeBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BobbyKhan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_TheThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ThingTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Fairy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Fairy2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrazyBush(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Octopus(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incabot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incagen(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incagold(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Log(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incaboss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incatongue(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Trapazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PeepBomb(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Whackazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Slug(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ball(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_OctoBoss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_OctoTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy);
// the goats
void AI_Rammy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hoppy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Gruffy(Guy *me,Map *map,world_t *world,Guy *goodguy);
// stick things
void AI_StickShroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickCorpse(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Dancer(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif
