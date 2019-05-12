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
#define BLT_ENERGY  1
#define BLT_BALLLIGHTNING 2
#define BLT_FLAMEWALL 3
#define BLT_EARTHSPIKE 4
#define BLT_GASBLAST 5
#define BLT_MEGABOOM 6
#define BLT_ITEM	 7	// a powerup item dropped by a monster

// player bullets for special weapons
#define BLT_WHOOPEE	 8	// whoopee cushion gas
#define BLT_WHOOPEE2 9	// whoopee cushion level 2
#define BLT_WHOOPEE3 10	// whoopee level 3
#define BLT_CACTUS	 11	// cactus needle
#define BLT_BOOMERANG 12	// boomerang
#define BLT_ICE		 13	// ice wand blast
#define BLT_BOMB	 14 // bomb
#define BLT_GOODBOOM 15	// small bomb blast
#define BLT_GOODBOOM2 16	// huge bomb blast
#define BLT_HOTPANTS  17	// flames from the hotpants

// not player bullets
#define BLT_SWAMPGAS  18	// not a player weapon, just swamp gas
#define BLT_ORBGRENADE 19	// mystery orb grenade - frankenjulie
#define BLT_MISSILE	  20	// guided missile - frankenjulie
#define BLT_BADBOOM	  21	// evil clone of goodboom
#define BLT_MISLSMOKE 22
// player bullet again
#define BLT_ICE2	  23	// more ice bolts, shoot off when a high level ice bolt hits someone
#define BLT_LIGHTNING 24	// player lightning bolts
#define BLT_SLINGPOW  25	// the pow of the slingshot in Slingshot Mode (shooting gallery)
#define BLT_BATSHOT	  26	// what the bat spits
// not player bullets again
#define BLT_WIND	  27	// wind elder's missiles
#define BLT_EARTHSPIKE2 28	// earth elder's earthspikes (triple normal damage)
#define BLT_ICESHARD  29	// ice elder's cactus shots
#define BLT_WATER	  30	// water sprays for fighting Toasty
#define BLT_FOUNTAIN  31	// the thing that sprays the water
#define BLT_FLAMESHOT 32	// Toasty fire shot
#define BLT_PORTAL	  33	// used to generate the effects of the portal forming
// player again
#define BLT_PSDSHOT	  34	// psd=player swamp dog
#define BLT_LOONYBALL 35	// the ball for playing Loonyball!!
#define BLT_BOWLINGBALL 36	// ball for bowling
#define BLT_BOWLBOOM 37		// explosion for bowling pins
// not player again
#define BLT_SWAMPGAS2	38	// swampgas that explodes the pipe it goes through
#define BLT_EVILFACE	39	// evil faces rising from the evilizer
#define BLT_BIGSHOT		40	// like energy bolt but bigger and meaner
// witch spells
#define BLT_WITCHBLAST	41	// like ball lightning
#define BLT_WITCHBUZZSAW 42	// buzzsaw spell, made of boomerangs that circle her
#define BLT_WITCHSPEED	43	// control bullet that makes fire particles and keeps player fast
#define BLT_WITCHSHOCK	44	// control bullet that shocks nearby enemies
#define BLT_WITCHICE	45	// ice bolt, homes in
#define BLT_WITCHPLAGUE	46	// control bullet, flies along dropping gas clouds
#define BLT_WITCHGAS	47	// the gas that plague drops
#define BLT_WITCHKABOOM	48	// control bullet that launches explosions
// werewolf
#define BLT_WOLFSHOT	49	// for player werewolf
#define BLT_WOLFSHOCK	50
// larry
#define BLT_CLAW		51	// flying claws from Larry
// cactus
#define BLT_CACTUS2		52	// super-powered cactus at level 3

// bullets for the summoned guys
#define BLT_SUMBOOM		53	// explosion for bombie
#define BLT_SUMGAS		54	// poison gas for croaker
#define BLT_SUMFLAME	55	// hot dog flame
#define BLT_SUMFROST	56	// wolf ice
#define BLT_SUMSHOCK	57	// ghost lightning
#define BLT_SUMHEAL		58	// healer shot to heal other summons

#define BLT_DEVILDOLL	59	// the no weapon shot for Summony
#define BLT_BATGAS		60	// poison gas for stinky farley
#define BLT_AURA		61	// thorns aura

#define BLT_THFKNIFE	62	// throwing dagger
#define BLT_KNIFESHRP	63	// shrapnel from knife
#define BLT_THFFIRE		64	// scorcher fire for the knife

#define MAX_BULLETS 400

// the special fire flags for different powerups
#define FF_REVERSE 1
#define FF_REFLECT 2
#define FF_WPNLOCK 4	// when on, you can't switch special weapons
#define FF_HOMING  8
#define FF_GUIDED  16
#define	FF_TRIPLE  32
#define FF_HELPER  64	// if you have a helper bat or not
#define FF_HELPERHERE 128	// is the helper bat present?

typedef struct bullet_t
{
	int x,y,z;
	int dx,dy,dz;
	int timer;
	int target;
	byte anim;
	byte facing;
	byte type;
	byte bright;
} bullet_t;

extern byte bulletHittingType;

void InitBullets(void);
void ExitBullets(void);

void UpdateBullets(Map *map,world_t *world);
void RenderBullets(void);

bullet_t *FireBullet(int x,int y,byte facing,byte type);
void FireBulletAfter(int x,int y,byte facing,byte type,bullet_t *thisone);
void FireExactBullet(int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type);
byte Bulletable(bullet_t *me,Map *map,int x,int y);
void BulletRanOut(bullet_t *me,Map *map,world_t *world);	// prototyped for interior use
void ReflectShot(void);
void BulletSwap(int sx,int sy,int width,int height,int dx,int dy);
void SpitAcid(int x,int y,byte facing,byte type);
void FireBulletZ(int x,int y,int z,byte facing,byte type);
byte BulletCanGo(bullet_t *me,int xx,int yy,Map *map,byte size);
void BombRanOut(bullet_t *me);
void GetKicked(bullet_t *me);
void GetBallCoords(int *x,int *y,int *z);
void CatchBall(word id);
void ThrowBall(void);
byte Intersect(int rx,int ry,int rx2,int ry2,int r2x,int r2y,int r2x2,int r2y2);
void CheckBoomRocks(Map *map,int xx,int yy,int size);
bullet_t *FindItemBullet(int x,int y);

#endif
