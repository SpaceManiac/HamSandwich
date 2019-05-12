#ifndef MONSTER_H
#define MONSTER_H

/* this file does the AI and everything else for each monster type.
   It knows all about the different types, as opposed to guy.cpp which
   just sort of keeps track of the list of guys. */

#include "winpch.h"
#include "jamulspr.h"
#include "display.h"

// the monster types
#define MONS_NONE      0
#define MONS_LOONY     1
#define MONS_BONEHEAD  2
#define MONS_BONEHEAD2 3
#define MONS_BONEHEAD3 4
#define MONS_BONEHEAD4 5
#define MONS_ZOMBIE    6
#define MONS_ZOMBIE2   7
#define MONS_ZOMBIE3   8
#define MONS_VILLAGER  9
#define MONS_VILLAGER2 10
#define MONS_VILLAGER3 11
#define MONS_VILLAGER4 12
#define MONS_VILLAGER5 13
#define MONS_VILLAGER6 14
#define MONS_FROG	   15
#define MONS_FROG2	   16
#define MONS_FROG3	   17
#define MONS_MUMMY	   18
#define MONS_MUMMY2	   19
#define MONS_MUMMY3	   20
#define MONS_MUMMY4	   21
#define MONS_EVILTREE  22
#define MONS_EVILTREE2 23
#define MONS_BAT	   24
#define MONS_BAT2	   25
#define MONS_BAT3	   26
#define MONS_WOLFMAN   27
#define MONS_WOLFMAN2  28
#define MONS_WOLFMAN3  29
#define MONS_LARRY	   30	// Special wolfman
#define MONS_SWAMPDOG  31
#define MONS_SWAMPDOG2 32
#define MONS_SWAMPDOG3 33
#define MONS_GHOST	   34
#define MONS_GHOST2	   35
#define MONS_GHOST3	   36
#define MONS_PUMPKIN   37
#define MONS_PUMPKIN2  38
#define MONS_PUMPKIN3  39
#define MONS_VAMPIRE   40
#define MONS_VAMPIRE2  41
#define MONS_VAMPIRE3  42
#define MONS_VAMPIRE4  43
#define MONS_STICKWITCH 44
#define MONS_LOONYTOAD 45
#define MONS_TUMBLEROCK 46
#define MONS_TUMBLEROCK2 47
#define MONS_TUMBLEROCK3 48
#define MONS_TUMBLEROCK4 49
#define MONS_LIGHTBALL	50	// blinks on for 2, off for 5
#define MONS_LIGHTBALL2	51	// blinks on for 5, off for 2
#define MONS_LIGHTBALL3	52	// can be destroyed
#define MONS_LIGHTBALL4	53	// permanently on (can be switched off presumably)
#define MONS_LIGHTBALL5 54	// permanently off (a permanently on, switched off)
#define MONS_BHOMATIC	55	// Boneheadomatic
#define MONS_FRANKENJ	56	// Frankenjulie
#define MONS_HAND		57	// Frankenjulie's hand
#define MONS_BIGGHOST	58	// polterguy!
#define MONS_JUNK		59	// polterguy's junk
#define MONS_JUNK2		60	// polterguy's junk
#define MONS_JUNK3		61	// polterguy's junk
#define MONS_HELPERBAT	62	// Farley the ghost bat
#define MONS_ELDER		63	// wind elder vampire
#define MONS_ELDER2		64	// earth elder vampire
#define MONS_ELDER3		65	// water elder vampire
#define MONS_ELDER4		66	// fire elder vampire
#define MONS_BUBBLE		67	// bubble for water elder to shoot
#define MONS_PLYRBONKULA 68	// Bonkula when the player is controlling him
#define MONS_PLYRSWAMPDOG 69	// swampdog controlled by player
#define MONS_PLYRWITCH	70	// stick witch playable
#define MONS_BONKULA	71	// evil Bonkula
#define MONS_EVILIZER	72
#define MONS_EVILPUMP	73
#define MONS_PLYRWOLF	74
#define MONS_LOONY2		75	// slightly different looking loony, for wolf loony
#define MONS_HUMANLARRY 76	// larry in human form
#define MONS_BONEHEAD5	77
#define MONS_ELDER5		78
#define MONS_HARRY		79
#define MONS_FROG4		80
#define MONS_EVILTREE3	81
#define MONS_BAT4		82
#define MONS_SWAMPDOG4	83
#define MONS_MUMMY5		84
#define MONS_GHASTLY	85
#define MONS_ZOMBIE4	86
#define MONS_PUMPKIN4	87
#define MONS_RBOMATIC	88
#define MONS_GGOMATIC	89
#define MONS_NSOMATIC	90
#define MONS_MINIFRANK	91
#define MONS_MECHABONK	92
#define MONS_PLYRSUMMON 93	// playable summony
#define MONS_PLYRTHIEF	94	// playable thief girl
// summons
#define MONS_SUMBOMBIE	95	// bombie summoned by summony
#define MONS_SUMGHOST	96	// grabby bat
#define MONS_SUMWOLF	97	// croaker
#define MONS_SUMBAT		98	// hot dog
#define MONS_SUMMUMMY	99	// healer
#define MONS_SUMFROG	100	// arctic wolf
#define MONS_SUMDOG		101	// shocking horror

#define NUM_MONSTERS   102

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
#define MF_NOHIT	 16384	// bullets pass through it
#define MF_GLOW		 32768 // draw using glow draw

typedef struct monsterType_t
{
	char name[32];
	int offc[3];
	byte level;
	byte size;
	byte framesPerDir;
	word hp;
	byte damage;
	int  speed;
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
void SetMonsterFlags(byte type,word flags);
byte MonsterFrames(byte type);
word MonsterHP(byte type);
byte MonsterDamage(byte type);
char *MonsterName(byte type);
void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte ouch,int ice);
void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl);
sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing);
int RangeToTarget(Guy *me,Guy *goodguy);
void LoadMonsterSprite(byte type);
void UpdateMonsterTypes(void);
void FaceGoodguy(Guy *me,Guy *goodguy);
void FaceGoodguyVillager(Guy *me,Guy *goodguy);
void TurnLoonyIntoToad(void);
int MonsterLevel(byte type);
void DoMove(Guy *me,int move,int frmAdv,byte busy,int dx,int dy);
void MakeSuperLoony(void);

// ai functions for each monster type
void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Villager(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Frog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mummy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilTree(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wolfman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Larry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HumanLarry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Swampdog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickWitch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Tumblerock(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lightball(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Boneheadomatic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Frankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hand(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Polterguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Junk(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HelperBat(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_WindElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EarthElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WaterElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FireElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SummonElder(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Bubble(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bonkula(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Evilizer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilPump(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Harry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghastly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MiniFrankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MechaBonkula(Guy *me,Map *map,world_t *world,Guy *goodguy);

// AI for the summons
void AI_Summon_Bombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Summon_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Summon_Frog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Summon_Wolf(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Summon_HotDog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Summon_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Summon_Mummy(Guy *me,Map *map,world_t *world,Guy *goodguy);

// special AIs for other game modes
void AI_Goalie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BowlingPin(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif
