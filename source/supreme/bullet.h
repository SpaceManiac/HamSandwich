#ifndef BULLET_H
#define BULLET_H

#include "cossin.h"
#include "display.h"
#include "jamulspr.h"
#include "map.h"
#include "particle.h"
#include "sound.h"
#include "world.h"

// SERIALIZED in the Summon/Change Bullet special effects. Add to end, don't reorder.
enum BulletType : byte
{
	BLT_NONE          = 0,
	BLT_HAMMER        = 1,
	BLT_HAMMER2       = 2, // this is a hammer with reflection
	BLT_MISSILE       = 3,
	BLT_FLAME         = 4,
	BLT_LASER         = 5,
	BLT_ACID          = 6,
	BLT_BOMB          = 7,
	BLT_BOOM          = 8,
	BLT_ENERGY        = 9,
	BLT_MEGABEAM      = 10, // this is a ball of energy that launches off a megabeam1
	BLT_MEGABEAM1     = 11, // this is a huge laser beam (downward)
	BLT_MEGABEAM2     = 12, // this is the laser hitting an object (just a visual effect)
	BLT_FLAME2        = 13, // just like flame, except it is anti-Bouapha
	BLT_SPORE         = 14,
	BLT_SHROOM        = 15,
	BLT_GRENADE       = 16, // energy grenade, an enemy weapon
	BLT_YELBOOM       = 17, // yellow explosion made by energy grenade
	BLT_SHOCKWAVE     = 18, // purple shockwave, for Super Zombie stomp
	BLT_LILBOOM       = 19, // explosion made by missile
	BLT_SNOWBALL      = 20,
	BLT_BIGSNOW       = 21,
	BLT_ICESPIKE      = 22, // spike juts out of the ground
	BLT_ROCK          = 23,
	BLT_SPINE         = 24, // cactus spine
	BLT_EVILHAMMER    = 25, // a grey hammer that is anti-bouapha
	BLT_BIGSHELL      = 26, // Bouapha's power armor shoots these
	BLT_BIGAXE        = 27, // Bouapha weapon
	BLT_LIGHTNING     = 28, // Bouapha weapon
	BLT_SPEAR         = 29, // Bouapha's version of the pygmy spear
	BLT_SLASH         = 30, // Bouapha's machete slash
	BLT_MINE          = 31, // Bouapha's mines
	BLT_BADSPEAR      = 32, // pygmy-thrown spear
	BLT_ORBITER       = 33, // guy that flies around Bouapha shooting
	BLT_GREEN         = 34, // friendly green bullets
	BLT_BALLLIGHTNING = 35,
	BLT_LIGHTNING2    = 36,
	BLT_MINDWIPE      = 37,
	BLT_REFLECT       = 38,
	BLT_SWAP          = 39,
	BLT_SHARK         = 40,
	BLT_ORBITER2      = 41, // orbit bomber
	BLT_HARPOON       = 42, // a spear, underwater - only difference is it sinks much more slowly
	BLT_SCANNER       = 43, // what the scanner scans with
	BLT_SCANSHOT      = 44, // the shots the scanner fires after scanning
	BLT_TORPEDO       = 45, // Bouapha's minisub shoots them
	BLT_DIRTSPIKE     = 46, // ice spike in brown
	BLT_PAPER         = 47,
	BLT_SCANLOCK      = 48, // the scanner locked onto a guy
	BLT_BUBBLE        = 49,
	BLT_FREEZE        = 50,
	BLT_BUBBLEPOP     = 51, // a bubble that is popping, just visual effect
	BLT_LILBOOM2      = 52, // a harmless lilboom
	BLT_CHEESEHAMMER  = 53, // enhanced hammers
	BLT_FREEZE2       = 54, // a freeze bullet that drops like acid bullets and splats
	BLT_LUNA          = 55, // lunachick's bullets
	BLT_LUNA2         = 56, // lunachick's bullets with wall-bounce power

	NUM_BULLETS // how many bullets there are in total. Put new types BEFORE this one!
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
	int x, y, z;
	int dx, dy, dz;
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

void UpdateBullets(Map *map, world_t *world);
void RenderBullets(void);
void RenderSmoke(int x, int y, int z, char bright, byte frm);
void RenderBoom(int x, int y, int z, char bright, byte frm);
void RenderBubble(int x, int y, int z, char bright, byte frm);
void RenderMindControl(int x, int y, int z, char bright, byte frm);
void RenderStinky(int x, int y, int z, char bright, byte frm);

void QuadMissile(int x, int y, byte facing, byte friendly);
void LaunchMegabeam(int x, int y, word owner);
void FireBullet(int x, int y, byte facing, byte type, byte friendly);
void FireBulletAfter(int x, int y, byte facing, byte type, bullet_t *thisone, byte friendly);
void FireExactBullet(int x, int y, int z, int dx, int dy, int dz, byte anim, byte timer, byte facing, byte type, byte friendly);
void HammerLaunch(int x, int y, byte facing, byte count, byte flags);
byte Bulletable(Map *map, int x, int y);
void BulletRanOut(bullet_t *me, Map *map, world_t *world); // prototyped for interior use
void ReflectShot(void);
void BulletSwap(int sx, int sy, int width, int height, int dx, int dy);
void SpitAcid(int x, int y, byte facing, byte type, byte friendly);
void FireBulletZ(int x, int y, int z, byte facing, byte type, byte friendly);
void HappyLaunch(int x, int y, byte facing, byte count, byte flags);
void ShroomSpew(int x, int y, byte facing, byte count, byte flags);
void ReflectBullets(int x, int y, byte size, byte friendly);
void RemoveOrbiters(int n, byte f, byte t);
void FireScanShots(Guy *victim);
void MakeRadar(int rx, int ry, byte w);
byte GetBulletAttackType(void);

int CountBullets(byte type);                                     // For specials to check orbiters
int CountBulletsInRect(byte type, int x, int y, int x2, int y2); // for specials to check for bullets
void ChangeBullet(byte fx, int x, int y, int type, int newtype); // for specials to change bullets
byte BulletFacingType(byte type);                                // To check if a type of bullet uses none, 0-7, or 0-255 facings
const char* BulletName(int type);

void GetBulletDeltas(int *bdx, int *bdy);

#endif
