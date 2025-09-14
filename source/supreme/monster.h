#ifndef MONSTER_H
#define MONSTER_H

/* this file does the AI and everything else for each monster type.
   It knows all about the different types, as opposed to guy.cpp which
   just sort of keeps track of the list of guys. */

#include "jamulspr.h"
#include "display.h"

// the monster types
#define MONS_NONE	  0
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

#define MONS_SANTA	  59

// expansion "monsters"
#define MONS_MINECART 60	// mine cart Bouapha rides on
#define MONS_RAFT	  61	// raft Bouapha rides on
#define MONS_PWRBOUAPHA 62	// Bouapha in power armor

// expansion monsters
#define MONS_VAMPIRE  63
#define MONS_COFFIN	  64
#define MONS_GHOST	  65
#define MONS_BURNER	  66
#define MONS_LEFTY	  67
#define MONS_PYGMY2	  68
#define MONS_PYGMY3	  69
#define MONS_PKSTEIN  70
#define MONS_KNIGHT	  71
#define MONS_TRICEROID 72
#define MONS_COUNTESS 73
#define MONS_ALIENEGG 74
#define MONS_BABYALIEN 75
#define MONS_ALIEN	  76
#define MONS_ROBOPK	  77
#define MONS_SHOCKTR  78
#define MONS_ROBOT1	  79
#define MONS_ROBOT2	  80
#define MONS_ROBOFACTORY 81
#define MONS_TURRET	  82
#define MONS_BUNNY	  83
#define MONS_KONGOR	  84
#define MONS_SQUASH	  85
#define MONS_MINIPYGMY 86
#define MONS_LOONYBOT 87
#define MONS_LOONYCORE 88
#define MONS_LOONYGUN 89
#define MONS_LOONYSHIP 90

// -- fun pack
#define MONS_FRIENDLY   91
#define MONS_GOODTURRET 92
#define MONS_ROLLER2	93
#define MONS_ALIENEGG2	94

#define MONS_MAT2HEAD	95
#define MONS_MAT2SKULL	96
#define MONS_MAT2BRAIN	97
#define MONS_MAT2BODY	98
#define MONS_MAT2TAIL	99

#define MONS_SHAMAN2	100
#define MONS_JALAPENO	101

#define MONS_GENERATOR1	102
#define MONS_GENERATOR2	103
#define MONS_GENERATOR3	104
#define MONS_GENERATOR4	105

#define MONS_SHARK		106
#define MONS_MADBUG		107
#define MONS_WIZARD		108
#define MONS_EVILCLONE	109
#define MONS_BOB		110
#define MONS_MULTIMOSS  111
#define MONS_MOSS2		112
#define MONS_SNOWBALL	113
#define MONS_SNOWBALL2	114
#define MONS_SNOWBLOW	115
#define MONS_BOOMKIN	116
#define MONS_MUMBLE2	117
#define MONS_GOODROBOT	118
#define MONS_GOODROBOT2 119
#define MONS_XENOMAMA	120
#define MONS_ROLLER3	121
#define MONS_ROLLER4	122
#define MONS_DARKVAMP	123
#define MONS_GNOME		124
#define MONS_NOBODY		125
#define MONS_FRIENDLY2	126
#define MONS_TROOPER2	127
#define MONS_PUMPKIN2	128
#define MONS_CRAZYBONE	129
#define MONS_CREEPAZOID 130
#define MONS_LAZYBONE	131
#define MONS_STAREYBAT	132
#define MONS_MADMAN		133
#define MONS_PYGMYACCT	134
#define MONS_CRAZYPANTS	135
#define MONS_YERFDOG	136
#define MONS_ELITESEAL	137
#define MONS_CONE		138
#define MONS_PYGMYDIVER	139
#define MONS_PUFFYFISH	140
#define MONS_PUFFYFISH2 141
#define MONS_OLAF		142
#define MONS_SPIKEY		143
#define MONS_BOMBOT		144
#define MONS_FOLLOWBUNNY 145
#define MONS_AUTOZOID	146
#define MONS_CHEST		147
#define MONS_JELLOFISH	148
#define MONS_KINGCONE	149
#define MONS_MAILBOXER	150
#define MONS_PIRANHA	151
#define MONS_YUGO		152
#define MONS_MINISUB	153	// bouapha in minisub
#define MONS_PUNKIN		154
#define MONS_SEACUKE	155
#define MONS_SNKYSHRK2	156
#define MONS_STARFISH	157
#define MONS_UNDERZOID	158
#define MONS_WETSEAL	159
#define MONS_WETWILLY	160
#define MONS_CRABPUFF	161
#define MONS_PATCH		162
#define MONS_PATCH2		163	// quickly producing patch
#define MONS_PATCH3		164	// boomkin patch
#define MONS_PATCH4		165	// squash patch
#define MONS_PATTY		166
#define MONS_DOZER		167
#define MONS_DARKCOFFIN	168
#define MONS_MUTANT		169
#define MONS_WHATSIT	170
#define MONS_SHOCKGHOST 171
#define MONS_MECHAZOID	172
#define MONS_SUCKER1	173	// sucks from anywhere in the world
#define MONS_SUCKER2	174	// sucks harder
#define MONS_SUCKER3	175	// sucks over limited range
#define MONS_SUCKER4	176	// sucks harder
#define MONS_BLOWER1	177	// blows from anywhere in the world
#define MONS_BLOWER2	178	// blows harder
#define MONS_BLOWER3	179	// blows over limited range
#define MONS_BLOWER4	180	// blows harder
#define MONS_BEETLE		181
#define MONS_SCARAB		182
#define MONS_FROSTBITER 183
#define MONS_MISLTURRET 184
#define MONS_DEATHTURRET 185
#define MONS_UNDERMAGMA	186
#define MONS_PARKEDCAR	187
#define MONS_PARKED2	188
#define MONS_TRAFFIC	189
#define MONS_TRAFFIC2	190
#define MONS_PLAYMECHA	191
#define MONS_SHROOMLORD	192
#define MONS_BIGBAT		193
#define MONS_HUGEBAT	194
#define MONS_WOLF		195
#define MONS_WOLF2		196
#define MONS_PATROLLR	197
#define MONS_PATROLUD	198
#define MONS_DPATROLLR	199
#define MONS_DPATROLUD	200
#define MONS_WEATHERMAN	201
#define MONS_XENOEGG	202
#define MONS_PLAYSHROOM 203
#define MONS_PUNKBUNNY  204
#define MONS_LUNACHICK	205
#define MONS_SKELETON	206
#define MONS_HOTSHROOM	207
#define MONS_MINIZOID	208
#define MONS_MINIWACKO	209
#define MONS_JACKFROST	210

#define NUM_MONSTERS  211    // Can grow as needed
#define NUM_PROFILE_MONSTERS 211    // Do not change without redoing the profile format

// fake monster codes
#define MONS_GOODGUY	(-1)	// any goodguy
#define MONS_BADGUY		(-2)	// any badguy
#define MONS_ANYBODY	(-3)	// anybody
#define MONS_NONPLAYER	(-4)	// anybody but the player
#define MONS_TAGGED		(-5)	// the tagged monster
#define MONS_PLAYER		(-6)	// the player

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

constexpr byte FRAME_INVIS = 254;
constexpr byte FRAME_END = 255;

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

// themese
#define MT_NONE		(0)
#define MT_GOOD		(1<<0)
#define MT_EVIL		(1<<1)
#define MT_SPOOKY	(1<<2)
#define MT_ZOMBIE	(1<<3)
#define MT_VAMPIRE	(1<<4)
#define MT_SPIDER	(1<<5)
#define MT_PYGMY	(1<<6)
#define MT_ZOID		(1<<7)
#define MT_BOSS		(1<<8)
#define MT_MINIBOSS (1<<9)
#define MT_WACKY	(1<<10)
#define MT_PUMPKIN	(1<<11)
#define MT_THINGY	(1<<12)
#define MT_VEGGIE	(1<<13)
#define MT_ARCTIC	(1<<14)
#define MT_DESERT   (1<<15)
#define MT_VEHICLE	(1<<16)
#define MT_GENERATE	(1<<17)
#define MT_TRAP		(1<<18)
#define MT_ALIEN	(1<<19)
#define MT_HIGHTECH	(1<<20)
#define MT_ANIMAL	(1<<21)
#define MT_HUMAN	(1<<22)
#define MT_URBAN	(1<<23)
#define MT_AQUATIC	(1<<24)
#define MT_UNDERSEA (1<<25)
#define MT_FLYING	(1<<26)
#define MT_BITS		(1<<27)
#define NUM_MONSTHEMES	(28)

typedef void (*Monster_AIFunc)(Guy *,Map *,world_t *,Guy *);

struct monsterType_t
{
	char name[32];
	byte fromCol,toCol;
	char brtChg;
	Monster_AIFunc AI;
	byte size;
	byte framesPerDir;
	word hp;
	word points;
	char sprName[32];
	sprite_set_t *spr;
	word flags;
	dword theme;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
};

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

void ChangeOffColor(dword type,byte from,byte to);
byte MonsterSize(dword type);
const byte *MonsterAnim(dword type,byte anim);
word MonsterFlags(dword type,byte aiType);
dword MonsterTheme(dword type);
byte MonsterFrames(dword type,byte aiType);
word MonsterHP(dword type);
word MonsterPoints(dword type);
const char *MonsterName(short type);
Monster_AIFunc MonsterAI(dword type);
int8_t MonsterBrightnessChange(dword type);

void MonsterDraw(
	int x, int y, int z,
	dword type, bool isBouapha,
	byte seq, byte frm, byte facing,
	char bright,
	byte ouch, byte poison, byte frozen,
	byte fromCol, byte toCol, uint8_t brtChg,
	const sprite_set_t* set
);
void InstaRenderMonster(int x,int y,dword type,char bright,MGLDraw *mgl);
int InstaRenderScannedMonster(int x,int y,dword type,char bright,MGLDraw *mgl);
const sprite_t *GetMonsterSprite(dword type,byte seq,byte frm,byte facing);
int RangeToTarget(Guy *me,Guy *goodguy);

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

// expansion monsters
void AI_MineCart(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Raft(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Coffin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Burner(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lefty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy3(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkinstein(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Knight(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Triceroid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Countess(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_AlienEgg(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabyAlien(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Alien(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Robopumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Minibot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Meaniebot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Robofactory(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Turret(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Kongor(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Squash(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UltraPygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyShip(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyGun(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyBot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyCore(Guy *me,Map *map,world_t *world,Guy *goodguy);

// fun pack monsters
void AI_GoodTurret(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Mattie2Brain(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2SkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2Tail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2Body(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_BuddyBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyQueen(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jalapeno(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Generator(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shark(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MadBug(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wizard(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilClone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bob(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MultiMoss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossRapido(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowball(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowblower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_XenoMama(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Gnome(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Nobody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_RockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrazyBone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Creepazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lazybones(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StareyBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Madman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyAcct(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Crazypants(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yerfdog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EliteSeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyDiver(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Puffyfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yeti2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spikey(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bombot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FollowBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Autozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Chest(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jellofish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_KingCone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mailboxer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Piranha(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yugo(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Punkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SeaCuke(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sneaky2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Starfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Underzoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WetSeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WetWilly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrabPuff(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Dozer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MutantZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Whatsit(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShockGhost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mechazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sucker(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Blower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Beetle(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Scarab(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Frostbiter(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UnderMagmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ParkedCar(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Traffic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShroomLord(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HugeBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Werewolf(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patrol(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Weatherman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HunterEgg(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PunkBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Skeleton(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Microzoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HotShroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MiniWacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_JackFrost(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif
