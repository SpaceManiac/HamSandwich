#ifndef BULLET_H
#define BULLET_H

#include "jamulspr.h"
#include "map.h"
#include "world.h"
#include "display.h"
#include "cossin.h"
#include "sound.h"
#include "particle.h"

#define BLT_NONE	0
#define BLT_HAMMER  1
#define BLT_HAMMER2 2	// this is a hammer with reflection
#define BLT_MISSILE 3
#define BLT_FLAME   4
#define BLT_LASER	5
#define BLT_ACID	6
#define BLT_BOMB	7
#define BLT_BOOM	8
#define BLT_ENERGY  9
#define BLT_MEGABEAM 10		// this is a ball of energy that launches off a megabeam1
#define BLT_MEGABEAM1 11	// this is a huge laser beam (downward)
#define BLT_MEGABEAM2 12	// this is the laser hitting an object (just a visual effect)
#define BLT_FLAME2	13	// just like flame, except it is anti-Bouapha
#define BLT_SPORE	14
#define BLT_SHROOM  15
#define BLT_GRENADE 16	// energy grenade, an enemy weapon
#define BLT_YELBOOM 17	// yellow explosion made by energy grenade
#define BLT_SHOCKWAVE 18	// purple shockwave, for Super Zombie stomp
#define BLT_LILBOOM 19	// explosion made by missile
#define BLT_SNOWBALL 20
#define BLT_BIGSNOW  21
#define BLT_ICESPIKE 22	// spike juts out of the ground
#define BLT_ROCK	 23
#define BLT_SPINE	 24 // cactus spine
#define BLT_EVILHAMMER 25 // a grey hammer that is anti-bouapha
#define BLT_BIGSHELL 26	// Bouapha's power armor shoots these
#define BLT_BIGAXE	 27	// Bouapha weapon
#define BLT_LIGHTNING 28 // Bouapha weapon
#define BLT_SPEAR	 29	// Bouapha's version of the pygmy spear
#define BLT_SLASH	 30	// Bouapha's machete slash
#define BLT_MINE	 31	// Bouapha's mines
#define BLT_BADSPEAR 32	// pygmy-thrown spear
#define BLT_ORBITER  33	// guy that flies around Bouapha shooting
#define BLT_GREEN	 34	// friendly green bullets
#define BLT_BALLLIGHTNING 35
#define BLT_LIGHTNING2 36
#define BLT_MINDWIPE	37
#define BLT_REFLECT	 38
#define BLT_SWAP	 39
#define BLT_SHARK	 40
#define BLT_ORBITER2 41	// orbit bomber
#define BLT_HARPOON	 42	// a spear, underwater - only difference is it sinks much more slowly
#define BLT_SCANNER	 43	// what the scanner scans with
#define BLT_SCANSHOT 44	// the shots the scanner fires after scanning
#define BLT_TORPEDO	 45	// Bouapha's minisub shoots them
#define BLT_DIRTSPIKE 46	// ice spike in brown
#define BLT_PAPER			47
#define BLT_SCANLOCK 		48	// the scanner locked onto a guy
#define BLT_BUBBLE			49
#define BLT_FREEZE			50
#define BLT_BUBBLEPOP 		51	// a bubble that is popping, just visual effect
#define BLT_LILBOOM2 		52		// a harmless lilboom
#define BLT_CHEESEHAMMER 	53	// enhanced hammers
#define BLT_FREEZE2			54		// a freeze bullet that drops like acid bullets and splats
#define BLT_LUNA			55		// lunachick's bullets
#define BLT_LUNA2			56		// lunachick's bullets with wall-bounce power
//NEW BULLETS
#define BLT_FLAME3			57	// floaty flame, drifts up
#define BLT_SITFLAME 		58 	//these flames sit!
#define BLT_BADSITFLAME		59	//^^
#define BLT_LASER2 			60 	// the cool lasers - very deadly
#define BLT_BADGREEN 		61	// bad green bullets
#define BLT_ORBITER3 		62	// weather orbiter
#define BLT_SHARKGOOD		63
#define BLT_ORBITER4 		64	// not a player weapon, just swamp gas
#define BLT_WIND	  		65	// wind elder's missiles
#define BLT_EVILFACE  		66	// evil faces rising from the evilizer
#define BLT_LUNAX1			67 	// evil lunachick boolet (cheese orb)
#define BLT_LUNAX2			68 	// evil lunachick boolet (purple mine)
#define BLT_YELWAVE  		69	// wavey thing
#define BLT_BEAM 			70	// clone of icebeam, but deadly to player
#define BLT_BEAM2 			71	// the dud version for when it hits walls
#define BLT_SLIME	 		72	// slug slime
#define BLT_REDNADE 		73	// red energy grenade, an enemy weapon
#define BLT_REDBOOM 		74	// red explosion made by red energy grenade
#define BLT_BIGYELLOW 		75	// big yellow bullet (same as grenade, but not lobbed or explosive)
#define BLT_MEGABOOM 		76	// big yellow bullet (same as grenade, but not lobbed or explosive)
#define BLT_ENERGY_BOUNCE	77		// bouncy energy boolets!!
#define BLT_ROCKET			78		// rocket launcher	
#define BLT_SKULL			79		// armageddon sword skulls...
#define BLT_WAND			80		// kid mystic's wand
#define BLT_BOOMERANG		81		// boomerang
#define BLT_FART			82		// pfffrrft
#define BLT_PUMPKIN			83		// cucurbinator bullets
#define BLT_FLAME4			84		// small tiny flame
#define BLT_FLAME5			85		// flame that can be fired in any 256 directions
#define BLT_WAND2			86		// bouncy kid mystic wand
#define BLT_HOLESHOT 		87		// black hole shot flying
#define BLT_BLACKHOLE 		88		// black hole existing
#define BLT_EGG				89		// egg projectile that can be fired in any 256 directions - spawns egg
#define BLT_PIE				90		// chef liem's famous pumpkin pies
//cool stuff
#define BLT_ENERGY_WAVE  	91		//these move back and forth, like a wave!
#define BLT_ENERGY_CIRCLS1  92		//these rotate clockwise
#define BLT_ENERGY_CIRCLS2  93		//these rotate counter-clockwise
#define BLT_ENERGY_CIRCLF1  94		//these rotate clockwise, but are FAST!
#define BLT_ENERGY_CIRCLF2  95		//these rotate counter-clockwise, but are also FAST
#define BLT_BOMB_HOMING		96		//homing red bomb
// back to other stuff
#define BLT_FLAMEWALL		97 	// flame wall
#define BLT_CLAWS			98 	// claws
#define BLT_ICESHARD		99 	// ice shard
#define BLT_HOTPANTS		100		// hotpants
#define BLT_WITCH			101		// witch wand
#define BLT_PNKWAVE			102 	// confusion bullets
#define BLT_SPOREGOOD		103 	// healer mushroom spores
#define BLT_ROCKBIG			104 	// gorka rocks
#define BLT_SHOCKWAVE2 		105		// red shockwave, for megaphone
#define BLT_BIGSNOW2  		106 	// hans solo's special attack
#define BLT_BIGLUNA			107 	// evil lunachick boolet (purple mine)
#define BLT_AXE				108 	// axe
#define MAX_BULTYPES	 	109

// the special hammer flags for different powerups
#define HMR_REVERSE 1
#define HMR_REFLECT 2

// the cheat flags
#define HMR_WATERWALK 	1	// not really a hammer powerup, it's a cheat code, but this is a good spot for it
#define HMR_SHIELD		2	// also cheat code
#define HMR_SPEED		4	// another cheat
#define HMR_OXYGEN		8	// another cheat
#define HMR_NOSKID		16	// another cheat
#define HMR_LIGHT		32	// another cheat
#define HMR_STRENGTH 	64	// another cheat

typedef struct bullet_t
{
	int x,y,z;
	int dx,dy,dz;
	int ax,ay;
	int timer;
	word target;
	byte anim;
	byte facing;
	byte type;
	char bright;
	byte friendly;
	byte extra;
	Guy *shoot;
	byte reverse;
} bullet_t;

void InitBullets(void);
void ExitBullets(void);

void UpdateBullets(Map *map,world_t *world);
void RenderBullets(void);
void RenderSmoke(int x,int y,int z,char bright,byte frm);
void RenderBoom(int x,int y,int z,char bright,byte frm);
void RenderBubble(int x,int y,int z,char bright,byte frm);
void RenderMindControl(int x,int y,int z,char bright,byte frm);
void RenderStinky(int x,int y,int z,char bright,byte frm);

void QuadMissile(Guy *me,int x,int y,byte facing,byte friendly);
void LaunchMegabeam(Guy *me,int x,int y,word owner);
void FireBullet(Guy *me,int x,int y,byte facing,byte type,byte friendly);
void FireBulletAfter(Guy *me,int x,int y,byte facing,byte type,bullet_t *thisone,byte friendly);
void FireExactBullet(Guy *me,int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type,byte friendly);
void HammerLaunch(Guy *me,int x,int y,byte facing,byte count,byte flags);
byte Bulletable(Map *map,int x,int y);
void BulletRanOut(bullet_t *me,Map *map,world_t *world);	// prototyped for interior use
void ReflectShot(void);
void BulletSwap(int sx,int sy,int width,int height,int dx,int dy);
void SpitAcid(Guy *me,int x,int y,byte facing,byte type,byte friendly);
void FireBulletZ(Guy *me,int x,int y,int z,byte facing,byte type,byte friendly);
void HappyLaunch(Guy *me,int x,int y,byte facing,byte count,byte flags);
void ShroomSpew(Guy *me,int x,int y,byte facing,byte count,byte flags);
void ReflectBullets(Guy *me,int x,int y,byte size,byte friendly);
void RemoveOrbiters(int n,byte f,byte t);
void FireScanShots(Guy *me, Guy *victim);
void MakeRadar(int rx,int ry,byte w);
byte GetBulletAttackType(void);
//new
void BurnHay(Guy *me,int x,int y);
void WolfSpew(Guy *me,int x,int y,byte facing,byte count,byte flags);
void SkullLaunch(Guy *me,int x,int y,byte facing,byte count,byte flags);

int CountBullets(byte type); // For specials to check orbiters
int CountBulletsInRect(byte type,int x,int y,int x2,int y2); // for specials to check for bullets
void ChangeBullet(byte fx,int x,int y,int type,int newtype); // for specials to change bullets
byte BulletFacingType(byte type); // To check if a type of bullet uses none, 0-7, or 0-255 facings

void GetBulletDeltas(int *bdx,int *bdy);

#endif
