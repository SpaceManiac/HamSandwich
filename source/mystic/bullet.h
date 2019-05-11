#ifndef BULLET_H
#define BULLET_H

#include "winpch.h"
#include "jamulspr.h"
#include "map.h"
#include "world.h"
#include "display.h"
#include "cossin.h"
#include "sound.h"
#include "particle.h"

#define BLT_NONE    0
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
#define BLT_COIN	 26	// a coin!  Good!
#define BLT_ICECLOUD 27
#define BLT_MINIFBALL 28	// a tiny friendly fireball, for pterodactyls
#define BLT_YELWAVE  29	// wavey thing
#define BLT_BIGYELLOW 30	// big yellow bullet (same as grenade, but not lobbed or explosive)
#define BLT_BIGCOIN  31
#define BLT_LIQUIFY  32 // mega-flame
#define BLT_ICEBEAM  33
#define BLT_ICEBEAM2 34 // dud ice beam, for when it hits walls
#define BLT_GOODSHOCK 35 // goodguy version of the shockwave
#define BLT_SKULL	 36
#define BLT_DEATHBEAM 37	// clone of icebeam, but deadly to player
#define BLT_DEATHBEAM2 38	// the dud version for when it hits walls
#define BLT_SLIME	 39		// slug slime
#define BLT_COMET	 40	// armageddon drops these from the sky
#define BLT_COMETBOOM 41	// and they explode like this

#define MAX_BULLETS 256

// the special hammer flags for different powerups
#define HMR_REVERSE 1
#define HMR_REFLECT 2
#define HMR_WATERWALK 4	// not really a hammer powerup, it's a cheat code, but this is a good spot for it

typedef struct bullet_t
{
	int x,y,z;
	int dx,dy,dz;
	int timer;
	word target;
	word lastHit;
	byte anim;
	byte facing;
	byte type;
	byte bright;
} bullet_t;

void InitBullets(void);
void ExitBullets(void);

void UpdateBullets(Map *map,world_t *world);
void RenderBullets(void);
void RenderSmoke(int x,int y,int z,char bright,byte frm);
void RenderBoom(int x,int y,int z,char bright,byte frm);
void RenderLvlUp(int x,int y,int z);

void LaunchMegabeam(int x,int y,word owner);
void FireBullet(int x,int y,byte facing,byte type);
void FireBulletAfter(int x,int y,byte facing,byte type,bullet_t *thisone);
void FireExactBullet(int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type);
void HammerLaunch(int x,int y,byte facing,byte count,byte flags);
byte Bulletable(Map *map,int x,int y);
void BulletRanOut(bullet_t *me,Map *map,world_t *world);	// prototyped for interior use
void SkullLaunch(int x,int y,byte facing,byte count,byte flags);
void Armageddon(Map *map,int x,int y);

byte BulletHittingType(void);

#endif
