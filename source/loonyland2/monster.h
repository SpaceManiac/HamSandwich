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
#define MONS_TEDDY	   2
#define MONS_VILLAGER  3
#define MONS_AXETRAP   4	// Axe Trap skill makes these
#define MONS_BONEHEAD  5
#define MONS_MOUSE	   6
#define MONS_PUPPET	   7
#define MONS_CRYOZOID  8
#define MONS_SHROOM	   9	// the shrooms that enemies can be turned into
#define MONS_PUPPET2   10	// pulls squeakers out of his hat
#define MONS_TINGUY	   11
#define MONS_MOUSE2	   12	// Rat-A-Tat
#define MONS_TEDDY2	   13	// Cave Bear
#define MONS_MOUSE3	   14	// Ratatouille miniboss
#define MONS_MONKEY	   15	// sock monkey
#define MONS_KLONK	   16
#define MONS_TOASTY	   17	// Toasties!  for the Toasties spell
#define MONS_KLUNK	   18	// Klonk's brother
#define MONS_ICEMICE   19
#define MONS_ROSE	   20	// Stone Rose
#define MONS_HOUND	   21	// ice hound
#define MONS_ROSE2	   22	// rosie o bomber
#define MONS_MONKEY2   23	// Stinky Sock
#define MONS_PUPPET3   24	// KO Punch
#define MONS_TEDDY3	   25	// Berserker Bear, leader of the cave bears
#define MONS_TOYMAKER  26
#define MONS_TOYPOWER  27	// power generator
#define MONS_TOYCRYSTAL 28	// crystals that orbit generator
#define MONS_TINGUY2	29	// Tin Soldier
#define MONS_LIFEBERRY		30	// lifeberry bush
#define MONS_MOUSE4		31		// Plague Rat
#define MONS_LION	   32	// Dandy Lion
#define MONS_BRUISER   33	// Klonk-like guys, working for Onion
#define MONS_BURNINGBUSH 34
#define MONS_THUG		35	// Onion Thug
#define MONS_GHOST		36	// ghost for Spookybones
#define MONS_TRIGUN		37	// Trigun, soldier with triple fire
#define MONS_JUNKSMITH  38
#define MONS_CARAVAN	39
#define MONS_JUNKTRAP	40
#define MONS_BUNNYTROOP 41
#define MONS_BUNNYMAJOR 42
#define MONS_BUNNYBOSS	43
#define MONS_EVILBOT	44	// clockwork robot as an enemy
#define MONS_YOURBOT	45	// clockwork robot you built
#define MONS_MONKEY3	46	// Shock Monkey
#define MONS_GHOSTDOG	47
#define MONS_GHOSTMONKEY 48
#define MONS_CRAB		49
#define MONS_BADGER		50
#define MONS_TITAN		51
#define MONS_BIGPLANT	52
#define MONS_FURNACE	53
#define MONS_ROSE3		54	// Yellow Rose
#define MONS_BADGER2	55	// gophers for the forest
#define MONS_LION2		56	// advanced dandy lions
#define MONS_STONEWALL	57
#define MONS_BOKBOK		58
#define MONS_BOKBOK2	59	// nuclear bokbok
#define MONS_BUNNYBOSS2 60
#define MONS_TITAN2		61	// titanic titan
#define MONS_BADGER3	62	// stinkin' bodzha
#define MONS_ARENACRYSTAL 63
#define MONS_TURRET		64	// turrets in frostburn
#define MONS_PROTOTYPE	65
#define MONS_FROSTGATE	66
#define MONS_MOUSE5		67	// Many Mouse
#define MONS_TINPOT		68	// tinpot general
#define MONS_TEDDY4		69	// grizzly bear
#define MONS_MONKEY4	70	// sock gorilla
#define MONS_MIMIC		71
#define MONS_ARMYONE	72	// Army Of One
#define MONS_BARON		73
#define MONS_UBERTITAN	74
#define MONS_TEDDY5		75	// Golden Bear
#define MONS_ROSE4		76	// Power Plant
#define MONS_ROSE5		77	// doom daisy
#define MONS_BIGBADGER	78
#define MONS_BIRD		79
#define MONS_KILLBOR	80
#define MONS_GORKA2		81
#define MONS_LILBADGER	82
#define MONS_PLAYTHUG	83	// play as thug
#define MONS_PLAYBUNNY  84	// play as bunny
#define MONS_BADGHOST	85	// lvl 5 ghost
#define MONS_BADGHOST2  86	// lvl 15 ghost
#define MONS_BADGHOST3	87	// lvl 25 ghost
#define MONS_BADGHOST4	88	// lvl 35 ghost
#define MONS_BADGHOST5	89	// lvl 45 ghost
#define MONS_SOCKCRAB	90
#define MONS_HOTDOG		91
#define MONS_ICEBOX		92
#define MONS_DEATHBERRY	93
#define MONS_FRUITPUNCH 94
#define MONS_REDBARON	95
#define MONS_JACK		96
#define NUM_MONSTERS   97

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
#define MF_NONE		 0
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
#define MF_ICEONLY	 8192 // can only move on ice
#define MF_NOHIT	 16384	// bullets pass through it
#define MF_GLOW		 32768 // draw using glow draw

// monster locations	- places where this monster should be created
#define ML_NONE		(0)
#define ML_TUNDRA	(1)
#define ML_WOODS	(2)
#define ML_CLIFFS	(4)
#define ML_LAKE		(8)
#define ML_WALLBUMP	(16)	// nothing to do with location, just means it wants to know when it bumps a wall
#define ML_BOSS		(32)	// also nothing to do with location, but does mean don't summon randomly among other things
#define ML_ALWAYSACTIVE (64)	// also nothing to do with location, means this monster doesn't deactivate when far away
#define ML_NOMIND	(128)	// can't be mindcontrolled
#define ML_GOOD		(256)	// on the goodguys team
#define ML_CAVE		(512)	// in normal caves

#define PUPPETPAL_HEALTH	(150)

// all monsters that should get summoned randomly at all
#define ML_RANDOMSUMMON (ML_TUNDRA|ML_WOODS|ML_CLIFFS|ML_LAKE|ML_CAVE)

typedef void AIFunc(Guy *me,Map *map,world_t *world,Guy *goodguy);

typedef struct monsterType_t
{
	char name[32];
	int offc[3];
	byte level;
	byte size;
	byte height;
	byte framesPerDir;
	word hp;
	byte armor;
	word damage;
	int  speed;
	char sprName[32];
	sprite_set_t *spr;
	word flags;
	word location;
	AIFunc *aiFunc;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
} monsterType_t;

extern monsterType_t monsType[NUM_MONSTERS];

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

byte MonsterSize(byte type);
byte *MonsterAnim(byte type,byte anim);
word MonsterFlags(byte type);
void SetMonsterFlags(byte type,word flags);
byte MonsterFrames(byte type);
word MonsterHP(byte type,byte team);
byte MonsterDamage(byte type);
char *MonsterName(byte type);
void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte ouch,int ice,byte poison);
void MonsterDrawShadow(int x,int y,int z,byte type,byte seq,byte frm,byte facing);
void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl);
sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing);
int RangeToTarget(Guy *me,Guy *goodguy);
void LoadMonsterSprite(byte type);
void UpdateMonsterTypes(void);
void FaceGoodguy(Guy *me,Guy *goodguy);
void FaceGoodguyVillager(Guy *me,Guy *goodguy);
int MonsterLevel(byte type);
void DoMove(Guy *me,int move,int frmAdv,byte busy,int dx,int dy);
byte AlwaysActiveMons(byte type);
byte RandomSummonMons(byte type);
byte WallBumpMons(byte type);
byte IsBoss(byte type);

// ai functions for each monster type
void AI_Villager(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Teddy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_AxeTrap(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_Mouse(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_Puppet(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_Cryozoid(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_TinGuy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ratatouille(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SockMonkey(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Klonk(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Toasty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Rose(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hound(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Toymaker(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Toypower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Toycrystal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lifeberry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lion(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Thug(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Junksmith(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Caravan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BunnyTroop(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BunnyGuy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BunnyBoss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ClockBotEvil(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ClockBot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Crab(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Badger(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Titan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigPlant(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Furnace(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Stonewall(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bokbok(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bokbok2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BunnyBoss2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_TitanBig(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ArenaCrystal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Turret(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PuppetPal(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_FrostGate(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_ManyMouse(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mimic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Baron(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UberTitan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigBadger(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bird(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Killbor(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ToyBadger(Guy *me,Map *map,world_t *world,Guy *badguy);
void AI_GhostEvil(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HotDog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Icebox(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Deathberry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FruitPunch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_RedBaron(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jack(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif
