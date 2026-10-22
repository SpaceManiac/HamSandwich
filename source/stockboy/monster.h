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
#define MONS_BALL	   2
#define MONS_BOMB	   3
#define MONS_BOOM	   4
#define MONS_BLOBBY	   5
#define MONS_BUNNY	   6
#define MONS_INCHY	   7
#define MONS_PIGGY	   8

// playable characters
#define MONS_SURFBOY   9
#define MONS_GOTHGIRL  10
#define MONS_ROBOT	   11
#define MONS_STICK	   12
#define MONS_SANTA	   13
#define MONS_CHERUB	   14
#define MONS_SHROOM	   15
#define MONS_PIG	   16
#define MONS_FREAK	   17
#define MONS_HIPPIE	   18
#define NUM_MONSTERS   19

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
	byte srcColor;
	byte framesPerDir;
	char sprName[32];
	sprite_set_t *spr;
	word flags;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
} monsterType_t;

class Guy;

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

byte *MonsterAnim(byte type,byte anim);
word MonsterFlags(byte type);
void SetMonsterFlags(byte type,word flags);
byte MonsterFrames(byte type);
byte MonsterColor(byte type);
char *MonsterName(byte type);
void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,byte dstColor,char bright,byte ouch,int ice);
void InstaRenderMonster(int x,int y,byte type,byte facing,byte color,char bright,MGLDraw *mgl);
void InstaRenderMonsterShadow(int x,int y,byte type,byte facing,MGLDraw *mgl);
sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing);
int RangeToTarget(Guy *me,Guy *goodguy);
void LoadMonsterSprite(byte type);
void UpdateMonsterTypes(void);
void FaceGoodguy(Guy *me,Guy *goodguy);
int MonsterLevel(byte type);
void DoMove(Guy *me,int move,int frmAdv,byte busy,int dx,int dy);
void SetupPlayerSprite(void);

// 'monster' AIs
void AI_Ball(Guy *me,Map *map,world_t *world);
void AI_Bomb(Guy *me,Map *map,world_t *world);
void AI_Boom(Guy *me,Map *map,world_t *world);
void AI_Blobby(Guy *me,Map *map,world_t *world);
void AI_Bunny(Guy *me,Map *map,world_t *world);
void AI_Inchy(Guy *me,Map *map,world_t *world);
void AI_Piggy(Guy *me,Map *map,world_t *world);

#endif