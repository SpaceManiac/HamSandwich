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
#include "guy.h"

#define BLT_NONE    0
#define BLT_SMOKE	1	// for acid burn or freeze
#define BLT_AQUA	2	// what aqua monsters shoot

#define MAX_BULLETS 128

typedef struct bullet_t
{
	int x,y,z;
	int dx,dy,dz;
	int timer;
	int target;
	int mapx,mapy,oldmapx,oldmapy;
	byte anim;
	byte facing;
	byte type;
	byte bright;
	Guy *owner;
} bullet_t;

extern byte bulletHittingType;

void InitBullets(void);
void ExitBullets(void);

void UpdateBullets(Map *map,world_t *world);
void RenderBullets(void);

bullet_t *FireBullet(int x,int y,byte facing,byte type);
void FireExactBullet(int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type);
byte Bulletable(bullet_t *me,Map *map,int x,int y);
void BulletRanOut(bullet_t *me,Map *map,world_t *world);	// prototyped for interior use
byte BulletCanGo(bullet_t *me,int xx,int yy,Map *map,byte size);
void BombRanOut(bullet_t *me);
byte Intersect(int rx,int ry,int rx2,int ry2,int r2x,int r2y,int r2x2,int r2y2);
byte BulletExists(byte type);
void BlowSmoke(int x,int y,byte color);
byte AcidSplash(Map *map,byte color,int x,int y);

#endif