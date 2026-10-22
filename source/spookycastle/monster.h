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
// the parts of Mattie
#define MONS_MATHEAD  11
#define MONS_MATSKULL 12
#define MONS_MATBRAIN 13
#define MONS_MATBODY  14
#define MONS_MATCLAW1 15
#define MONS_MATCLAW2 16
#define MONS_MATTAIL  17
// more normal monsters
#define MONS_GINGER	  18
#define MONS_PUMPKIN  19
#define MONS_BABYTHING 20
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
#define MONS_SUPERZOMBIE 29
#define MONS_STICKMAN 30
#define MONS_BABYSEAL 31
#define MONS_ISOZOID  32
#define MONS_SNOWGUY  33
#define MONS_PENGUIN  34
#define MONS_ZOMBONI  35
// the Yeti Bros.
#define MONS_SVEN	  36
#define MONS_BJORN	  37
// normal monsters again
#define MONS_GEOZOID  38
#define MONS_MUMBLE	  39
#define MONS_DJINNI	  40
#define MONS_LAMP	  41
#define MONS_CACTUS	  42
#define MONS_ROLLER	  43
#define MONS_LICH	  44
#define MONS_DUSTDEVIL 45
#define MONS_MECHABOUAPHA 46
// Sphinx parts
#define MONS_SPHXARM1 47
#define MONS_SPHXARM2 48
#define MONS_SPHINX   49
// more normal monsters
#define MONS_FREAKAZOID 50
#define MONS_CENTIBODY 51
#define MONS_CENTIHEAD 52
#define MONS_WACKO	  53
#define MONS_BOILER	  54
#define MONS_GREATPK  55
#define MONS_ULTRAZOID 56
// the Asylum bosses
#define MONS_DRL	  57
#define MONS_SDZL	  58
// and the final monster of them all
#define MONS_SANTA	  59

#define NUM_MONSTERS  60

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

#define ANIM_LENGTH	24

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
void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte ouch);
void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl);
sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing);

// ai functions for each monster type
void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EggSac(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MamaSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Serpent(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ginger(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabyThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Magmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);	
void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mush(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SuperZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickMan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabySeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Isozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Penguin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zomboni(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Geozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mumble(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Djinni(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MagicLamp(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cactus(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Roller(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lich(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_DustDevil(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MechaBouapha(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Freakazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CentiBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CentiHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Boiler(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_GreatPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ultrazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Santa(Guy *me,Map *map,world_t *world,Guy *goodguy);

	//Matilda's AIs
void AI_MattieBrain(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieSkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieClaw(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieTail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// The Thing's AIs
void AI_TheThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ThingTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Sven & Bjorn's AI
void AI_Yeti(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Sphinxter's AIs
void AI_SphinxArm(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sphinx(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Asylum bosses' AIs
void AI_DrLunatic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SDZL(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif