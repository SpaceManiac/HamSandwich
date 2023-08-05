#ifndef BULLET_H
#define BULLET_H

#include "jamulspr.h"
#include "map.h"
#include "world.h"
#include "display.h"
#include "cossin.h"
#include "sound.h"
#include "particle.h"

// SERIALIZED in the Summon/Change Bullet special effects. Add to end, don't reorder.
enum : byte
{
	BLT_NONE,
	BLT_HAMMER,
	BLT_HAMMER2, // this is a hammer with reflection
	BLT_MISSILE,
	BLT_FLAME,
	BLT_LASER,
	BLT_ACID,
	BLT_BOMB,
	BLT_BOOM,
	BLT_ENERGY,
	BLT_MEGABEAM, // this is a ball of energy that launches off a megabeam1
	BLT_MEGABEAM1, // this is a huge laser beam (downward)
	BLT_MEGABEAM2, // this is the laser hitting an object (just a visual effect)
	BLT_FLAME2, // just like flame, except it is anti-Bouapha
	BLT_SPORE,
	BLT_SHROOM,
	BLT_GRENADE, // energy grenade, an enemy weapon
	BLT_YELBOOM, // yellow explosion made by energy grenade
	BLT_SHOCKWAVE, // purple shockwave, for Super Zombie stomp
	BLT_LILBOOM, // explosion made by missile
	BLT_SNOWBALL,
	BLT_BIGSNOW,
	BLT_ICESPIKE, // spike juts out of the ground
	BLT_ROCK,
	BLT_SPINE, // cactus spine
	BLT_EVILHAMMER, // a grey hammer that is anti-bouapha
	BLT_BIGSHELL, // Bouapha's power armor shoots these
	BLT_BIGAXE, // Bouapha weapon
	BLT_LIGHTNING, // Bouapha weapon
	BLT_SPEAR, // Bouapha's version of the pygmy spear
	BLT_SLASH, // Bouapha's machete slash
	BLT_MINE, // Bouapha's mines
	BLT_BADSPEAR, // pygmy-thrown spear
	BLT_ORBITER, // guy that flies around Bouapha shooting
	BLT_GREEN, // friendly green bullets
	BLT_BALLLIGHTNING,
	BLT_LIGHTNING2,
	BLT_MINDWIPE,
	BLT_REFLECT,
	BLT_SWAP,
	BLT_SHARK,
	BLT_ORBITER2, // orbit bomber
	BLT_HARPOON, // a spear, underwater - only difference is it sinks much more slowly
	BLT_SCANNER, // what the scanner scans with
	BLT_SCANSHOT, // the shots the scanner fires after scanning
	BLT_TORPEDO, // Bouapha's minisub shoots them
	BLT_DIRTSPIKE, // ice spike in brown
	BLT_PAPER,
	BLT_SCANLOCK, // the scanner locked onto a guy
	BLT_BUBBLE,
	BLT_FREEZE,
	BLT_BUBBLEPOP, // a bubble that is popping, just visual effect
	BLT_LILBOOM2, // a harmless lilboom
	BLT_CHEESEHAMMER, // enhanced hammers
	BLT_FREEZE2, // a freeze bullet that drops like acid bullets and splats
	BLT_LUNA, // lunachick's bullets
	BLT_LUNA2, // lunachick's bullets with wall-bounce power
};

// the special hammer flags for different powerups
enum : byte
{
	HMR_REVERSE   = 1 << 0,
	HMR_REFLECT   = 1 << 1,
	HMR_WATERWALK = 1 << 2, // not really a hammer powerup, it's a cheat code, but this is a good spot for it
	HMR_SHIELD    = 1 << 3, // also cheat code
	HMR_SPEED     = 1 << 4, // another cheat
	HMR_OXYGEN    = 1 << 5, // another cheat
	HMR_NOSKID    = 1 << 6, // another cheat
	HMR_LIGHT     = 1 << 7, // another cheat
};

struct bullet_t
{
	int x,y,z;
	int dx,dy,dz;
	int timer;
	word target;
	byte anim;
	byte facing;
	byte type;
	char bright;
	byte friendly;
};

void InitBullets(void);
void ExitBullets(void);

void UpdateBullets(Map *map,world_t *world);
void RenderBullets(void);
void RenderSmoke(int x,int y,int z,char bright,byte frm);
void RenderBoom(int x,int y,int z,char bright,byte frm);
void RenderBubble(int x,int y,int z,char bright,byte frm);
void RenderMindControl(int x,int y,int z,char bright,byte frm);
void RenderStinky(int x,int y,int z,char bright,byte frm);

void QuadMissile(int x,int y,byte facing,byte friendly);
void LaunchMegabeam(int x,int y,word owner);
void FireBullet(int x,int y,byte facing,byte type,byte friendly);
void FireBulletAfter(int x,int y,byte facing,byte type,bullet_t *thisone,byte friendly);
void FireExactBullet(int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type,byte friendly);
void HammerLaunch(int x,int y,byte facing,byte count,byte flags);
byte Bulletable(Map *map,int x,int y);
void BulletRanOut(bullet_t *me,Map *map,world_t *world);	// prototyped for interior use
void ReflectShot(void);
void BulletSwap(int sx,int sy,int width,int height,int dx,int dy);
void SpitAcid(int x,int y,byte facing,byte type,byte friendly);
void FireBulletZ(int x,int y,int z,byte facing,byte type,byte friendly);
void HappyLaunch(int x,int y,byte facing,byte count,byte flags);
void ShroomSpew(int x,int y,byte facing,byte count,byte flags);
void ReflectBullets(int x,int y,byte size,byte friendly);
void RemoveOrbiters(int n,byte f,byte t);
void FireScanShots(Guy *victim);
void MakeRadar(int rx,int ry,byte w);
byte GetBulletAttackType(void);

int CountBullets(byte type); // For specials to check orbiters
int CountBulletsInRect(byte type,int x,int y,int x2,int y2); // for specials to check for bullets
void ChangeBullet(byte fx,int x,int y,int type,int newtype); // for specials to change bullets
byte BulletFacingType(byte type); // To check if a type of bullet uses none, 0-7, or 0-255 facings

void GetBulletDeltas(int *bdx,int *bdy);

#endif
