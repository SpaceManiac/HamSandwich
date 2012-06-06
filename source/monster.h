#ifndef MONSTER_H
#define MONSTER_H

/* this file does the AI and everything else for each monster type.
   It knows all about the different types, as opposed to guy.cpp which
   just sort of keeps track of the list of guys. */

#include "winpch.h"
#include "jamulspr.h"
#include "display.h"

// the monster types
enum {
    MONS_NONE = 0,
    MONS_BOUAPHA = 1,
    MONS_BONEHEAD = 2,
    MONS_BAT = 3,
    MONS_SPIDER = 4,
    MONS_BIGSPDR = 5,
    MONS_ZOMBIE = 6,
    MONS_EGGSAC = 7,
    MONS_MAMASPDR = 8,
    MONS_PYGMY = 9,
    MONS_SERPENT = 10,
    // the parts of Mattie
    MONS_MATHEAD = 11,
    MONS_MATSKULL = 12,
    MONS_MATBRAIN = 13,
    MONS_MATBODY = 14,
    MONS_MATCLAW1 = 15,
    MONS_MATCLAW2 = 16,
    MONS_MATTAIL = 17,
    // more normal monsters
    MONS_GINGER = 18,
    MONS_PUMPKIN = 19,
    MONS_BABYTHING = 20,
    MONS_MOSS = 21,
    MONS_MOSSGRANDE = 22,
    MONS_MAGMAZOID = 23,
    MONS_SHROOM = 24,
    MONS_MUSH = 25,
    // the parts of The Thing
    MONS_THING = 26,
    MONS_THINGTENT = 27,
    MONS_THINGTENTTIP = 28,
    // normal monsters again
    MONS_SUPERZOMBIE = 29,
    MONS_STICKMAN = 30,
    MONS_BABYSEAL = 31,
    MONS_ISOZOID = 32,
    MONS_SNOWGUY = 33,
    MONS_PENGUIN = 34,
    MONS_ZOMBONI = 35,
    // the Yeti Bros.
    MONS_SVEN = 36,
    MONS_BJORN = 37,
    // normal monsters again
    MONS_GEOZOID = 38,
    MONS_MUMBLE = 39,
    MONS_DJINNI = 40,
    MONS_LAMP = 41,
    MONS_CACTUS = 42,
    MONS_ROLLER = 43,
    MONS_LICH = 44,
    MONS_DUSTDEVIL = 45,
    MONS_MECHABOUAPHA = 46,
    // Sphinx parts
    MONS_SPHXARM1 = 47,
    MONS_SPHXARM2 = 48,
    MONS_SPHINX = 49,
    // more normal monsters
    MONS_FREAKAZOID = 50,
    MONS_CENTIBODY = 51,
    MONS_CENTIHEAD = 52,
    MONS_WACKO = 53,
    MONS_BOILER = 54,
    MONS_GREATPK = 55,
    MONS_ULTRAZOID = 56,
    // the Asylum bosses
    MONS_DRL = 57,
    MONS_SDZL = 58,

    MONS_SANTA = 59,

    // expansion "monsters"
    MONS_MINECART = 60,		// mine cart Bouapha rides on
    MONS_RAFT = 61,			// raft Bouapha rides on
    MONS_PWRBOUAPHA = 62,	// Bouapha in power armor

    // expansion monsters
    MONS_VAMPIRE = 63,
    MONS_COFFIN = 64,
    MONS_GHOST = 65,
    MONS_BURNER = 66,
    MONS_LEFTY = 67,
    MONS_PYGMY2 = 68,
    MONS_PYGMY3 = 69,
    MONS_PKSTEIN = 70,
    MONS_KNIGHT = 71,
    MONS_TRICEROID = 72,
    MONS_COUNTESS = 73,
    MONS_ALIENEGG = 74,
    MONS_BABYALIEN = 75,
    MONS_ALIEN = 76,
    MONS_ROBOPK = 77,
    MONS_SHOCKTR = 78,
    MONS_ROBOT1 = 79,
    MONS_ROBOT2 = 80,
    MONS_ROBOFACTORY = 81,
    MONS_TURRET = 82,
    MONS_BUNNY = 83,
    MONS_KONGOR = 84,
    MONS_SQUASH = 85,
    MONS_MINIPYGMY = 86,
    MONS_LOONYBOT = 87,
    MONS_LOONYCORE = 88,
    MONS_LOONYGUN = 89,
    MONS_LOONYSHIP = 90,

    // -- fun pack
    MONS_FRIENDLY = 91,
    MONS_GOODTURRET = 92,
    MONS_ROLLER2 = 93,
    MONS_ALIENEGG2 = 94,

    MONS_MAT2HEAD = 95,
    MONS_MAT2SKULL = 96,
    MONS_MAT2BRAIN = 97,
    MONS_MAT2BODY = 98,
    MONS_MAT2TAIL = 99,

    MONS_SHAMAN2 = 100,
    MONS_JALAPENO = 101,

    MONS_GENERATOR1 = 102,
    MONS_GENERATOR2 = 103,
    MONS_GENERATOR3 = 104,
    MONS_GENERATOR4 = 105,

    MONS_SHARK = 106,
    MONS_MADBUG = 107,
    MONS_WIZARD = 108,
    MONS_EVILCLONE = 109,
    MONS_BOB = 110,
    MONS_MULTIMOSS = 111,
    MONS_MOSS2 = 112,
    MONS_SNOWBALL = 113,
    MONS_SNOWBALL2 = 114,
    MONS_SNOWBLOW = 115,
    MONS_BOOMKIN = 116,
    MONS_MUMBLE2 = 117,
    MONS_GOODROBOT = 118,
    MONS_GOODROBOT2 = 119,
    MONS_XENOMAMA = 120,
    MONS_ROLLER3 = 121,
    MONS_ROLLER4 = 122,
    MONS_DARKVAMP = 123,
    MONS_GNOME = 124,
    MONS_NOBODY = 125,
    MONS_FRIENDLY2 = 126,
    MONS_TROOPER2 = 127,
    MONS_PUMPKIN2 = 128,
    MONS_CRAZYBONE = 129,
    MONS_CREEPAZOID = 130
};

#ifdef EXPANDO
const int NUM_MONSTERS = 161;
#else	// lacking the expansion pack
const int NUM_MONSTERS = 60;
#endif

// the animations
enum {
	ANIM_IDLE = 0,
	ANIM_MOVE,
	ANIM_ATTACK,
	ANIM_DIE,
	ANIM_A1,
	ANIM_A2,
	ANIM_A3,
	ANIM_A4,
	ANIM_A5,
	NUM_ANIMS
};

const int ANIM_LENGTH = 24;

// flags
enum {
	MF_FLYING = 1,
	MF_WATERWALK = 2,
	MF_ONEFACE = 4,
	MF_ENEMYWALK = 8,	// other enemies can stomp all over this one (but not Bouapha)
	MF_NOMOVE = 16,		// doesn't move when hit
	MF_AQUATIC = 32,	// can ONLY move on water/lava, not land
	MF_INVINCIBLE = 64, // totally invulnerable to harm
	MF_SPRITEBOX = 128, // use the sprite's rect for collision checks instead of standard size-box method
	MF_FACECMD = 256,	// this monster's "facing" value should just be added to the sprite number,
						// it's calculated by his AI (only useful for MF_ONEFACE monsters)
	MF_NOGRAV = 512,
	MF_FREEWALK = 1024, // Bouapha can walk right through it
	MF_WALLWALK = 2048, // walk through walls
	MF_NOSHADOW = 4096, // doesn't cast a shadow
	MF_GHOST = 8192,	// draw using ghost draw
	MF_NOHIT = 16384,	// bullets pass through it
	MF_GLOW = 32768		// draw using glow draw
};

typedef void (*monsterAi_t)(Guy *me, Map *map, world_t *world, Guy *goodguy);

struct monsterType_t
{
	char name[32];
	byte fromCol, toCol;
	char brtChg;
	byte size;
	byte framesPerDir;
	word hp;
	word points;
	char sprName[32];
	sprite_set_t *spr;
	word flags;
	monsterAi_t aiFunc;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
};

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

void ChangeOffColor(byte type, byte from, byte to);
byte MonsterSize(byte type);
byte *MonsterAnim(byte type, byte anim);
word MonsterFlags(byte type);
void SetMonsterFlags(byte type, word flags);
byte MonsterFrames(byte type);
word MonsterHP(byte type);
word MonsterPoints(byte type);
char *MonsterName(byte type);
monsterAi_t MonsterAi(byte type);
void MonsterDraw(int x, int y, int z, byte type, byte seq, byte frm, byte facing, char bright, byte ouch, byte poison);
void InstaRenderMonster(int x, int y, byte type, char bright, MGLDraw *mgl);
sprite_t *GetMonsterSprite(byte type, byte seq, byte frm, byte facing);
int RangeToTarget(Guy *me, Guy *goodguy);

// ai functions for each monster type
void AI_Bonehead(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Bat(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Spider(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_BigSpider(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Zombie(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_EggSac(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MamaSpider(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Pygmy(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Serpent(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Ginger(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Pumpkin(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_BabyThing(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Moss(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MossGrande(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Magmazoid(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Shroom(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Mush(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_SuperZombie(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_StickMan(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_BabySeal(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Isozoid(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Snowguy(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Penguin(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Zomboni(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Geozoid(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Mumble(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Djinni(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MagicLamp(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Cactus(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Roller(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Lich(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_DustDevil(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MechaBouapha(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Freakazoid(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_CentiBody(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_CentiHead(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Wacko(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Boiler(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_GreatPumpkin(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Ultrazoid(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Santa(Guy *me, Map *map, world_t *world, Guy *goodguy);

// Matilda's AIs
void AI_MattieBrain(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MattieSkullOrHead(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MattieClaw(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MattieTail(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MattieBody(Guy *me, Map *map, world_t *world, Guy *goodguy);
// The Thing's AIs
void AI_TheThing(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_ThingTentacle(Guy *me, Map *map, world_t *world, Guy *goodguy);
// Sven & Bjorn's AI
void AI_Yeti(Guy *me, Map *map, world_t *world, Guy *goodguy);
// Sphinxter's AIs
void AI_SphinxArm(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Sphinx(Guy *me, Map *map, world_t *world, Guy *goodguy);
// Asylum bosses' AIs
void AI_DrLunatic(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_SDZL(Guy *me, Map *map, world_t *world, Guy *goodguy);

#ifdef EXPANDO

// expansion monsters
void AI_MineCart(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Raft(Guy *me, Map *map, world_t *world, Guy *goodguy);

void AI_Vampire(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Coffin(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Ghost(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Burner(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Lefty(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Pygmy2(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Pygmy3(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Pumpkinstein(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Knight(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Triceroid(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Countess(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_AlienEgg(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_BabyAlien(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Alien(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Robopumpkin(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_ShockTrooper(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Minibot(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Meaniebot(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Robofactory(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Turret(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Bunny(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Kongor(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Squash(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_UltraPygmy(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_LoonyShip(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_LoonyGun(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_LoonyBot(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_LoonyCore(Guy *me, Map *map, world_t *world, Guy *goodguy);

// fun pack monsters
void AI_GoodTurret(Guy *me, Map *map, world_t *world, Guy *goodguy);

void AI_Mattie2Brain(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Mattie2SkullOrHead(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Mattie2Tail(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Mattie2Body(Guy *me, Map *map, world_t *world, Guy *goodguy);

void AI_BuddyBunny(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_PygmyQueen(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Jalapeno(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Generator(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Shark(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MadBug(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Wizard(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_EvilClone(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Bob(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MultiMoss(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_MossRapido(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Snowball(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Snowblower(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_XenoMama(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Gnome(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Nobody(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_BigPumpkin(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_RockTrooper(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_CrazyBone(Guy *me, Map *map, world_t *world, Guy *goodguy);
void AI_Creepazoid(Guy *me, Map *map, world_t *world, Guy *goodguy);

#endif	// expando monsters

#endif
