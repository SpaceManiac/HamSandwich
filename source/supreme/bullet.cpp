#include "winpch.h"
#include "bullet.h"
#include "guy.h"
#include "player.h"
#include "scanner.h"
#include "editor.h"
#include "shop.h"
#include "config.h"

#define SPR_FLAME   0
#define SPR_LASER   5
#define SPR_HAMMER  21
#define SPR_MISSILE 149
#define SPR_SMOKE	165
#define SPR_ACID	172
#define SPR_BOMB	228
#define SPR_ENERGY	236
#define SPR_BOOM	238
#define SPR_MEGABEAM 246
#define SPR_SPORE	254
#define SPR_SHROOM  258
#define SPR_GRENADE 266
#define SPR_YELBOOM 268
#define SPR_SHOCKWAVE 273
#define SPR_LILBOOM 277
#define SPR_SNOWBALL 282
#define SPR_BIGSNOW  283
#define SPR_ICESPIKE 286
#define SPR_ROCK	 290
#define SPR_SPINE	 294
#define SPR_BIGAXE	 310
#define SPR_SPEAR	 318
#define SPR_SLASH	 326
#define SPR_MINE	 350
#define SPR_STINKY	 355
#define SPR_GREEN	 358
#define SPR_ORBITER  359
#define SPR_PAPER	 367
#define SPR_BUBBLE	 375
#define SPR_SCANSHOT 384
#define SPR_SCANLOCK 387
#define SPR_ROCKET 388
#define SPR_WAND	 396
#define SPR_SKULL	 436
#define SPR_BOOMERANG 452
#define SPR_FART	 468
#define SPR_PUMPKIN 474
#define SPR_FLAMESML 482
#define SPR_WAVE	 485
#define SPR_BEAM  493
#define SPR_SLIME  498
#define SPR_REDNADE 510
#define SPR_REDBOOM 512
#define SPR_EVILFACE 517
#define SPR_MEGABOOM 523
#define SPR_EGG 535
#define SPR_PIE 536
#define SPR_AXE 538
#define SPR_SPIKE 548
#define SPR_CLAWS 772
#define SPR_FLAMEWALL 794
#define SPR_ROCKBIG 798

#define LASER_TIME   8
#define LASER_DELAY   2

bullet_t *bullet;
sprite_set_t *bulletSpr;
byte reflect=0;
byte attackType;
int activeBulDX,activeBulDY;

void GetBulletDeltas(int *bdx,int *bdy)
{
	*bdx=activeBulDX;
	*bdy=activeBulDY;
}

void InitBullets(void)
{
	bulletSpr=new sprite_set_t("graphics/bullets.jsp");

	bullet=(bullet_t *)malloc(sizeof(bullet_t)*config.numBullets);
	memset(bullet,0,config.numBullets*sizeof(bullet_t));
}

void ExitBullets(void)
{
	free(bullet);
	delete bulletSpr;
}

byte Bulletable(byte type,Map *map,int x,int y)
{
	mapTile_t *tile;

	tile=map->GetTile(x,y);
	if(tile->wall==65535)
		return 0;

	if(tile->wall ||
		!BulletHitItem(type,tile,x,y))
	{
		if(tile->wall)
		{
			if(curWorld.terrain[tile->wall].flags&TF_DESTRUCT)
				tile->wall=curWorld.terrain[tile->wall].next;
		}
		if(curWorld.terrain[tile->floor].flags&TF_DESTRUCT)
			tile->floor=curWorld.terrain[tile->floor].next;
		return 0;
	}

	return 1;
}

void OffScreenBulletDie(bullet_t *me,Map *map)
{
	if(me->x<0 || me->y<0 || me->x>=(map->width*TILE_WIDTH*FIXAMT) ||
		me->y>=(map->height*TILE_HEIGHT*FIXAMT))
		me->type=BLT_NONE;
}

byte BulletCanGo(byte type,int xx,int yy,Map *map,byte size,byte friendly)
{
	byte result;
	int mapx,mapy,mapx1,mapx2,mapy1,mapy2;

	if(xx<0 || yy<0)
		return 0;

	xx>>=FIXSHIFT;
	yy>>=FIXSHIFT;

	mapx=xx/TILE_WIDTH;
	mapy=yy/TILE_HEIGHT;
	mapx1=(xx-size)/TILE_WIDTH;
	mapy1=(yy-size)/TILE_HEIGHT;
	mapx2=(xx+size)/TILE_WIDTH;
	mapy2=(yy+size)/TILE_HEIGHT;

	result=(mapx1>=0 && mapy1>=0 && mapx2<map->width && mapy2<map->height &&
		(Bulletable(type,map,mapx,mapy1)) &&
		(Bulletable(type,map,mapx,mapy2)) &&
		(Bulletable(type,map,mapx1,mapy)) &&
		(Bulletable(type,map,mapx2,mapy)) &&
		(Bulletable(type,map,mapx,mapy)) &&
		(Bulletable(type,map,mapx1,mapy1)) &&
		(Bulletable(type,map,mapx2,mapy1)) &&
		(Bulletable(type,map,mapx2,mapy2)) &&
		(Bulletable(type,map,mapx1,mapy2)));

	if(!result)
		EventOccur(EVT_SHOOT,friendly,mapx,mapy,NULL);

	return result;
}

void LaserMirrorHit(bullet_t *me,Map *map,world_t *world)
{
	short mapx,mapy;
	char n[2];

	mapx=me->x/(TILE_WIDTH*FIXAMT);
	mapy=me->y/(TILE_HEIGHT*FIXAMT);

	if(mapx==(me->target%256) && mapy==(me->target/256))	// you've already hit/not hit this space
		return;

	if(mapx<0 || mapy<0 || mapx>=map->width || mapy>=map->height)
	{
		me->type=BLT_NONE;
		return;
	}

	if(me->dx<0)
	{
		me->dx=me->dx;
	}
	n[0]=GetItem(map->map[mapx+mapy*map->width].item)->name[0];
	n[1]=GetItem(map->map[mapx+mapy*map->width].item)->name[1];
	if(n[0]=='@')	// mirror items start with @
	{
		if(me->dx>0)	// heading right
		{
			if(n[1]=='1' || n[1]=='3')
			{
				me->type=BLT_NONE;
				ExplodeParticlesColor(5,me->x,me->y,me->z,1,2);
				return;	// blocked
			}
			else if(n[1]=='0')
			{
				me->dx=0;
				me->dy=-FIXAMT*24;
				me->facing=6*32;
			}
			else if(n[1]=='2')
			{
				me->dx=0;
				me->dy=FIXAMT*24;
				me->facing=2*32;
			}
		}
		else if(me->dx<0)	// heading left
		{
			if(n[1]=='0' || n[1]=='2')
			{
				me->type=BLT_NONE;
				ExplodeParticlesColor(4,me->x,me->y,me->z,1,2);
				return;	// blocked
			}
			else if(n[1]=='1')
			{
				me->dx=0;
				me->dy=-FIXAMT*24;
				me->facing=6*32;
			}
			else if(n[1]=='3')
			{
				me->dx=0;
				me->dy=FIXAMT*24;
				me->facing=2*32;
			}
		}
		else if(me->dy<0)	// heading up
		{
			if(n[1]=='0' || n[1]=='1')
			{
				me->type=BLT_NONE;
				ExplodeParticlesColor(4,me->x,me->y,me->z,1,2);
				return;	// blocked
			}
			else if(n[1]=='2')
			{
				me->dx=-FIXAMT*32;
				me->dy=0;
				me->facing=4*32;
			}
			else if(n[1]=='3')
			{
				me->dx=FIXAMT*32;
				me->dy=0;
				me->facing=0*32;
			}
		}
		else if(me->dy>0)	// heading down
		{
			if(n[1]=='2' || n[1]=='3')
			{
				me->type=BLT_NONE;
				ExplodeParticlesColor(4,me->x,me->y,me->z,1,2);
				return;	// blocked
			}
			else if(n[1]=='0')
			{
				me->dx=-FIXAMT*32;
				me->dy=0;
				me->facing=4*32;
			}
			else if(n[1]=='1')
			{
				me->dx=FIXAMT*32;
				me->dy=0;
				me->facing=0*32;
			}
		}
		me->target=(mapx+mapy*256);
		me->x=(mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		me->y=(mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	}
	else
	{
		ExplodeParticlesColor(4,me->x,me->y,me->z,1,2);
		me->type=BLT_NONE;
	}
}

void BulletHitWallX(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_HAMMER:
		case BLT_LUNA:
		case BLT_WAND:
		case BLT_WITCH:
		case BLT_BIGSHELL:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_FLAMEWALL:
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_ROCKET: //rocket
			BulletRanOut(me,map,world);
			break;
		case BLT_BOOMERANG: //boomerang
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_ORBITER:
		case BLT_ORBITER2:
		case BLT_ORBITER3:
		case BLT_ORBITER4:
		case BLT_SWAP:
		case BLT_SCANSHOT:
			me->x-=me->dx;
			me->dx=-me->dx;
			break;
		case BLT_SKULL:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->x-=me->dx;
				me->dx=-me->dx;
				me->facing=((byte)(8-me->facing))&15;
				MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			}
			else
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			}
			break;
		case BLT_HAMMER2:
		case BLT_WAND2:
		case BLT_LUNA2:
		case BLT_LUNAX1:
		case BLT_LUNAX2:
		case BLT_EVILHAMMER:	// reflects off walls
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_CHEESEHAMMER:
			me->x-=me->dx;
			if(me->facing<128)
				me->facing=128-me->facing;
			else
				me->facing=128+(256-me->facing);
			me->dx=Cosine(me->facing)*14;
			me->dy=Sine(me->facing)*14;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_BALLLIGHTNING:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->x-=me->dx;
				me->facing=84+Random(89);
				if(me->dx<0)
					me->facing=(me->facing+128)&255;
				me->dx=Cosine(me->facing)*11;
				me->dy=Sine(me->facing)*11;
			}
			else
				BulletRanOut(me,map,world);
			break;
		case BLT_GRENADE:
		case BLT_HOLESHOT:
		case BLT_BLACKHOLE:
			me->x-=me->dx;
			break;
		case BLT_ENERGY_BOUNCE:
		case BLT_BIGYELLOW:
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			//MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900); // ching!
			break;
		case BLT_LASER:	// reflects off walls
			MakeSound(SND_BULLETREFLECT,me->x,me->y,SND_CUTOFF,900);
			me->x-=me->dx;
			me->dx=-me->dx;
			me->dy+=-FIXAMT/4+Random(FIXAMT/2);
			me->facing=((byte)(8-me->facing))&15;
			break;
		case BLT_LASER2:	// reflects off mirrors
			LaserMirrorHit(me,map,world);
			break;
		case BLT_BOMB_HOMING:
		case BLT_BOMB:
			MakeSound(SND_BOMBREFLECT,me->x,me->y,SND_CUTOFF,600);
			me->x-=me->dx;
			me->dx=-me->dx;
			me->dy+=-FIXAMT/4+Random(FIXAMT/2);
			me->facing=((byte)(8-me->facing))&15;
			break;
		case BLT_SLASH:
		case BLT_CLAWS:
			MakeSound(SND_CLANG,me->x,me->y,SND_CUTOFF,950);
			// supposedly sparks
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,4,8);
			me->type=BLT_NONE;
			break;
		case BLT_MISSILE:
		case BLT_WIND:
		case BLT_SPINE:
		case BLT_BIGAXE:
		case BLT_AXE:
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_MINDWIPE:
		case BLT_REFLECT:
		case BLT_HARPOON:
		case BLT_SCANNER:
		case BLT_TORPEDO:
		case BLT_PAPER:
		case BLT_BUBBLE:
		case BLT_FREEZE:
		case BLT_FREEZE2:
		case BLT_ICESHARD:
			BulletRanOut(me,map,world);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_FLAME3:
		case BLT_FLAME5:
			me->x-=me->dx;
			me->dy=((3-Random(7))<<FIXSHIFT);
			me->dx=0;
			break;
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_FLAME4:
			me->dx=me->dy=0;
			break;
		case BLT_ROCK:	// reflects off walls
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ROCKBIG:	// reflects off walls
			ShakeScreen(floor(me->dx/2));
			me->x-=me->dx*0.75;
			me->dx=-me->dx*0.75;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_SPHINXSTOMP,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHARK:
		case BLT_SHARKGOOD:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_ENERGY:
		case BLT_ENERGY_WAVE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_GREEN:
		case BLT_BADGREEN:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
			me->type=BLT_MEGABEAM2;
			me->dy=0;
			me->timer=2;
			break;
		case BLT_SPORE:
		case BLT_SPOREGOOD:
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
			me->type=BLT_NONE;
			break;
		case BLT_SHROOM:
		case BLT_PUMPKIN:
		case BLT_EGG:
		case BLT_PIE:
			// detonate
			BulletRanOut(me,map,world);
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
		case BLT_BIGSNOW2:
			BulletRanOut(me,map,world);
			break;
		case BLT_EVILFACE:
		case BLT_FART:
			me->x-=me->dx;
			me->dx=0;
			if(me->timer>6)
				me->timer=6;
			break;
			//aaa
		case BLT_BEAM2:
		case BLT_YELWAVE:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,2);
			break;
		case BLT_PNKWAVE:
			BulletRanOut(me,map,world);
			break;
		case BLT_REDNADE:
			me->x=-me->dx;
			break;
		case BLT_HOTPANTS:
			me->dx=0;
			break;
	}
}

void BulletHitWallY(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_HAMMER:
		case BLT_LUNA:
		case BLT_WAND:
		case BLT_WITCH:
		case BLT_BIGSHELL:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_FLAMEWALL:
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_ROCKET: //rocket
			BulletRanOut(me,map,world);
			break;
		case BLT_BOOMERANG: //boomerang
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_ORBITER:
		case BLT_ORBITER2:
		case BLT_ORBITER3:
		case BLT_ORBITER4:
		case BLT_SWAP:
		case BLT_SCANSHOT:
			me->y-=me->dy;
			me->dy=-me->dy;
			break;
		case BLT_SKULL:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->y-=me->dy;
				me->dy=-me->dy;
				me->facing=(16-me->facing)&15;
				MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			}
			else
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			}
			break;
		case BLT_HAMMER2:
		case BLT_WAND2:
		case BLT_LUNA2:
		case BLT_LUNAX1:
		case BLT_LUNAX2:
		case BLT_EVILHAMMER:	// reflects off walls
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_CHEESEHAMMER:
			me->y-=me->dy;
			if(me->facing<128)
				me->facing=128+(128-me->facing);
			else
				me->facing=128-(me->facing-128);
			me->dx=Cosine(me->facing)*14;
			me->dy=Sine(me->facing)*14;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_BALLLIGHTNING:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->y-=me->dy;
				me->facing=20+Random(89);
				if(me->dy>0)
					me->facing+=128;
				me->dx=Cosine(me->facing)*11;
				me->dy=Sine(me->facing)*11;
			}
			else
				BulletRanOut(me,map,world);
			break;
		case BLT_GRENADE:
		case BLT_HOLESHOT:
		case BLT_BLACKHOLE:
			me->y-=me->dy;
			break;
		case BLT_ENERGY_BOUNCE:
		case BLT_BIGYELLOW:
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			//MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900); ching!
			break;
		case BLT_LASER:
			MakeSound(SND_BULLETREFLECT,me->x,me->y,SND_CUTOFF,900);
			me->y-=me->dy;
			me->dy=-me->dy;
			me->dx+=-FIXAMT/4+Random(FIXAMT/2);
			me->facing=(16-me->facing)&15;
			break;
		case BLT_LASER2:	// reflects off mirrors
			LaserMirrorHit(me,map,world);
			break;
		case BLT_BOMB_HOMING:
		case BLT_BOMB:
			MakeSound(SND_BOMBREFLECT,me->x,me->y,SND_CUTOFF,600);
			me->y-=me->dy;
			me->dy=-me->dy;
			me->dx+=-FIXAMT/4+Random(FIXAMT/2);
			me->facing=(16-me->facing)&15;
			break;
		case BLT_SLASH:
		case BLT_CLAWS:
			MakeSound(SND_CLANG,me->x,me->y,SND_CUTOFF,950);
			// supposedly sparks
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,4,8);
			me->type=BLT_NONE;
			break;
		case BLT_MISSILE:
		case BLT_WIND:
		case BLT_SPINE:
		case BLT_AXE:
		case BLT_BIGAXE:
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_MINDWIPE:
		case BLT_REFLECT:
		case BLT_HARPOON:
		case BLT_SCANNER:
		case BLT_TORPEDO:
		case BLT_PAPER:
		case BLT_BUBBLE:
		case BLT_FREEZE:
		case BLT_FREEZE2:
		case BLT_ICESHARD:
			BulletRanOut(me,map,world);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_FLAME3:
		case BLT_FLAME5:
			me->y-=me->dy;
			me->dx=((3-Random(7))<<FIXSHIFT);
			me->dy=0;
			break;
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_FLAME4:
			me->dx=me->dy=0;
			break;
		case BLT_ROCK:	// reflects off walls
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ROCKBIG:	// reflects off walls
			ShakeScreen(floor(me->dy/2));
			me->y-=me->dy*0.75;
			me->dy=-me->dy*0.75;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_SPHINXSTOMP,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHARK:
		case BLT_SHARKGOOD:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_ENERGY:
		case BLT_ENERGY_WAVE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_GREEN:
		case BLT_BADGREEN:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
			me->type=BLT_MEGABEAM2;
			me->dy=0;
			me->timer=2;
			break;
		case BLT_SPORE:
		case BLT_SPOREGOOD:
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
			me->type=BLT_NONE;
			break;
		case BLT_SHROOM:
		case BLT_PUMPKIN:
		case BLT_EGG:
		case BLT_PIE:
			// detonate
			BulletRanOut(me,map,world);
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
		case BLT_BIGSNOW2:
			BulletRanOut(me,map,world);
			break;
		case BLT_EVILFACE:
		case BLT_FART:
			me->y-=me->dy;
			me->dy=0;
			if(me->timer>6)
				me->timer=6;
			break;
		case BLT_REDNADE:
			me->y-=me->dy;
		case BLT_HOTPANTS:
			me->dy=0;
			break;
	}
}

void BulletHitFloor(bullet_t *me,Map *map,world_t *world)
{
	int x,y;

	switch(me->type)
	{
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_FLAME4:
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dx,FIXAMT/8);
			me->z=0;
			me->dz=0;
			break;
		case BLT_BIGSHELL:
		case BLT_BIGYELLOW:
			BulletRanOut(me,map,world);
			ExplodeParticles2(PART_YELLOW,me->x,me->y,me->z,6,3);
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:
		case BLT_LUNA:
		case BLT_LUNA2:
		case BLT_LUNAX1:
		case BLT_LUNAX2:
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,850);
			me->dz=-me->dz*3/4;
			me->z=0;
			x=(me->x>>FIXSHIFT)/TILE_WIDTH;
			y=(me->y>>FIXSHIFT)/TILE_HEIGHT;
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_WATER)
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
			else if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
			{
				ExplodeParticles(PART_LAVA,me->x,me->y,0,8);
				me->type=0;
			}
			break;
		case BLT_WAND:
		case BLT_WAND2:
		case BLT_WITCH:
		case BLT_SKULL:
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,850);
			me->dz=-me->dz*3/4;
			me->z=0;
			x=(me->x>>FIXSHIFT)/TILE_WIDTH;
			y=(me->y>>FIXSHIFT)/TILE_HEIGHT;
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_WATER)
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
			else if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
				ExplodeParticles(PART_LAVA,me->x,me->y,0,8);
			break;
		case BLT_BOMB:
		case BLT_BOMB_HOMING:
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
			{
				ExplodeParticles(PART_LAVA,me->x,me->y,0,8);
				me->type=BLT_BOOM;
			}
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_WATER)
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
			}
		case BLT_MINDWIPE:
			if(me->dz<-FIXAMT)	// don't make it on small bounces, because it'd be annoying
				MakeSound(SND_BOMBREFLECT,me->x,me->y,SND_CUTOFF,600);
			me->dz=-me->dz*3/4;
			me->z=0;
			break;
		case BLT_BUBBLE:
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
				me->type=0;
			me->dz=-me->dz*3/4;
			me->z=0;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_FLAME3:
		case BLT_FLAME5:
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_WATER)
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
				me->type=0;
			}
			me->z=0;
		break;
		case BLT_MINE:
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
			{
				ExplodeParticles(PART_LAVA,me->x,me->y,0,8);
				me->type=BLT_BOOM;
			}
			me->z=0;
		break;
		case BLT_CHEESEHAMMER:
		case BLT_MISSILE: // this really really should never happen
		case BLT_WIND:
		case BLT_FART:
		case BLT_BOOMERANG:
		case BLT_LASER:
		case BLT_LASER2:
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
		case BLT_MEGABEAM2:
		case BLT_SPORE:
		case BLT_SPOREGOOD:
		case BLT_YELBOOM:
		case BLT_REDBOOM:
		case BLT_LILBOOM:
		case BLT_MEGABOOM:
		case BLT_LILBOOM2:
		case BLT_SHOCKWAVE:
		case BLT_SHOCKWAVE2:
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
		case BLT_SPINE:
		case BLT_CLAWS:
		case BLT_AXE:
		case BLT_BIGAXE:
		case BLT_SLASH:
		case BLT_ORBITER:
		case BLT_ORBITER2:
		case BLT_ORBITER3:
		case BLT_ORBITER4:
		case BLT_BALLLIGHTNING:
		case BLT_REFLECT:
		case BLT_SWAP:
		case BLT_SCANNER:
		case BLT_SCANSHOT:
		case BLT_TORPEDO:
		case BLT_PAPER:
		case BLT_FREEZE:
		case BLT_ICESHARD:
		case BLT_HOLESHOT:
		case BLT_BLACKHOLE:
			me->z=0;
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_HARPOON:
		case BLT_FREEZE2:
			BulletRanOut(me,map,world);
			break;
		case BLT_ROCK:
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_WATER)
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
				me->type=0;
			}
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
			{
				ExplodeParticles(PART_LAVA,me->x,me->y,0,8);
				me->type=0;
			}
			if(me->dz<-FIXAMT)
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			me->dz=-me->dz*3/4;
			me->z=0;
			break;
		case BLT_ROCKBIG:
			ShakeScreen(floor(me->dz));
			me->dz=-me->dz*2/4;
			me->z=0;
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_WATER)
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
				me->type=0;
			}
			if(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_LAVA)
			{
				ExplodeParticles(PART_LAVA,me->x,me->y,0,8);
				me->type=0;
			}
			if(me->dz<-FIXAMT)
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ENERGY_BOUNCE:
		case BLT_ENERGY_WAVE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
		case BLT_ENERGY:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_GREEN:
		case BLT_BADGREEN:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHARK:
		case BLT_SHARKGOOD:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHROOM:
		case BLT_PUMPKIN:
		case BLT_EGG:
			// detonate
			BulletRanOut(me,map,world);
			break;
		case BLT_PIE:
			me->dx*=0.5;
			me->dy*=0.5;
			me->dz*=-0.75;
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_GRENADE:
			me->type=BLT_YELBOOM;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->anim=0;
			me->timer=9;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
		case BLT_BIGSNOW2:
			BulletRanOut(me,map,world);
			break;
		case BLT_EVILFACE:
		case BLT_BEAM2:
		case BLT_YELWAVE:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,2);
			break;
		case BLT_PNKWAVE:
			BulletRanOut(me,map,world);
			break;
		case BLT_REDNADE:
			if(me->dz<-FIXAMT)
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,600);
			me->dz=-me->dz*3/4;
			me->z=0;
			me->timer*=0.5;
			break;
		case BLT_HOTPANTS:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletRanOut(bullet_t *me,Map *map,world_t *world)
{
	int i;
	Guy *g;

	switch(me->type)
	{
		case BLT_WAND:
		case BLT_WAND2:
			ExplodeParticles2(PART_YELLOW,me->x,me->y,me->z,6,3);
			me->type=0;
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_WITCH:
		case BLT_LUNA:
		case BLT_LUNA2:
		case BLT_LUNAX1:
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_LILBOOM:
		case BLT_MEGABOOM:
		case BLT_LILBOOM2:
		case BLT_LASER:
		case BLT_LASER2:
		case BLT_BOOM:
		case BLT_ENERGY_BOUNCE:
		case BLT_ENERGY_WAVE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
		case BLT_ENERGY:
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
		case BLT_MEGABEAM2:
		case BLT_SPORE:
		case BLT_SPOREGOOD:
		case BLT_GRENADE:
		case BLT_YELBOOM:
		case BLT_REDBOOM:
		case BLT_SHOCKWAVE:
		case BLT_SHOCKWAVE2:
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
		case BLT_ROCK:
		case BLT_ROCKBIG:
		case BLT_SPINE:
		case BLT_CLAWS:
		case BLT_EVILHAMMER:
		case BLT_SLASH:
		case BLT_GREEN:
		case BLT_BADGREEN:
		case BLT_REFLECT:
		case BLT_BUBBLEPOP:
		case BLT_SCANLOCK:
		case BLT_EVILFACE:
		case BLT_FART:
		case BLT_BEAM:
		case BLT_BEAM2:
		case BLT_BLACKHOLE:
		case BLT_HOTPANTS:
			me->type=0;
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_MINDWIPE:
		case BLT_HARPOON:
			ExplodeParticles(PART_HAMMER,me->x,me->y,0,8);
			me->type=0;
			break;
		case BLT_HOLESHOT:
			me->type=BLT_BLACKHOLE;
			me->dx=0;
			me->dy=0;
			me->timer=30*4;
			me->anim=0;
			break;
		case BLT_BIGSNOW:
			// poof into snowballs
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			me->type=0;
			for(i=0;i<8;i++)
				FireBullet(me->shoot,me->x,me->y,(byte)i*32,BLT_SNOWBALL,me->friendly);
			break;
		case BLT_BIGSNOW2:
			// poof into snowballs
			MakeSound(SND_GLASSBREAK,me->x,me->y,SND_CUTOFF,1500);
			me->type=0;
			for(i=0;i<8;i++)
				FireBullet(me->shoot,me->x,me->y,(byte)i*32,BLT_FREEZE,me->friendly);
			ExplodeParticles2(PART_SNOW,me->x,me->y,me->z,6,12);
			break;
		case BLT_SNOWBALL:
		case BLT_FREEZE:
		case BLT_FREEZE2:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,6,12);
			break;
		case BLT_PAPER:
		case BLT_AXE:
		case BLT_ICESHARD:
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,6,12);
			break;
		case BLT_BIGAXE:
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,10,8);
			break;
		case BLT_BOOMERANG:
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,6,12);
			break;
		case BLT_MISSILE:
		case BLT_WIND:
		case BLT_CHEESEHAMMER:
		case BLT_LUNAX2:
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1500);
			me->type=BLT_LILBOOM;
			me->timer=9;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_PIE:
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1500);
			me->type=BLT_LILBOOM;
			me->timer=9;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_TORPEDO:
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1500);
			me->type=BLT_LILBOOM2;
			me->timer=9;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_ACID:
		case BLT_SWAP:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SCANNER:
		case BLT_SCANSHOT:
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHARK:
		case BLT_SHARKGOOD:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_BUBBLE:
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=BLT_BUBBLEPOP;
			me->anim=0;
			me->timer=10;
			me->dx=0;
			me->dy=0;
			MakeSound(SND_BUBBLEPOP,me->x,me->y,SND_CUTOFF,1);
			break;
		case BLT_ROCKET:
			me->type=BLT_BOOM;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->timer=7;
			me->anim=0;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_FLAME3:
		case BLT_FLAME4:
		case BLT_FLAME5:
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_FLAMEWALL:
			me->type=0;
			BlowSmoke(me->x,me->y,me->z,FIXAMT);
			break;
		case BLT_BOMB_HOMING:
		case BLT_BOMB:
		case BLT_MINE:
		case BLT_ORBITER:
		case BLT_ORBITER2:
		case BLT_ORBITER3:
		case BLT_ORBITER4:
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,2000);
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->type=BLT_BOOM;
			me->timer=7;
			me->anim=0;
			break;
		case BLT_SHROOM:
			for(i=0;i<256;i+=8)
			{
				FireExactBullet(me->shoot,me->x,me->y,me->z,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE,me->friendly);
				FireExactBullet(me->shoot,me->x,me->y,me->z,Cosine(i)*6,Sine(i)*6,0,0,16,i,BLT_SPORE,me->friendly);
			}
			g=AddGuy(me->x-me->dx*2,me->y-me->dy*2,me->z,MONS_SHROOM,me->friendly);	// become a living shroom
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);

			me->type=BLT_NONE;	// all gone
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_PUMPKIN:
			if(me->friendly==1)
				g=AddGuy(me->x-me->dx*2,me->y-me->dy*2,me->z,MONS_BUDDYPUMPKIN,1);	// become a living BUDDY PUMPKIN!
			else
				g=AddGuy(me->x-me->dx*2,me->y-me->dy*2,me->z,MONS_PUMPKIN,0);	// become a living PUMPKIN!
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				{RemoveGuy(g);
					for(i=0;i<8;i++)
					{
						FireExactBullet(me->shoot,me->x,me->y,me->z,Cosine(i*32)*12,Sine(i*32)*12,0,0,16,i,BLT_ENERGY,me->friendly);
					}
				}
			me->type=BLT_NONE;	// all gone
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_EGG:
			g=AddGuy(me->x-me->dx*2,me->y-me->dy*2,me->z,MONS_ALIENEGG,me->friendly);	// become a living EGG!
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
			RemoveGuy(g);
			for(i=0;i<8;i++)
			{
				FireExactBullet(me->shoot,me->x,me->y,me->z,Cosine(i*32)*8,Sine(i*8)*12,0,0,16,i,BLT_ACID,me->friendly);
			}
			me->type=BLT_NONE;	// all gone
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_BIGSHELL:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_BALLLIGHTNING:
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			me->type=BLT_NONE;
			break;
		case BLT_SKULL:
		case BLT_BIGYELLOW:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,8);
			break;
		case BLT_YELWAVE:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,2);
			break;
		case BLT_PNKWAVE:
			BlowSmoke(me->x,me->y,me->z,FIXAMT);
			me->type=BLT_NONE;
			break;
		case BLT_REDNADE:
			me->timer=9;
			me->type=BLT_REDBOOM;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->anim=0;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,950);
			break;
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int i;

	attackType=me->type;
	switch(me->type)
	{
		case BLT_BLACKHOLE:
			if(FindVictims2(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,0,0,1,map,world,me->friendly))
			{

			}
			break;
		case BLT_LUNAX1:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,8,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_LUNAX2:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,8,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_SCANNER:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,0,map,world,me->friendly))
			{
				if(GetLastGuyHit()->aiType!=MONS_BOUAPHA)
				{
					ScanGuy(me->shoot,GetLastGuyHit());
					me->type=BLT_SCANLOCK;
					me->timer=60;
					me->target=GetLastGuyHit()->ID;
				}
			}
			else if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,0,map,world,1-me->friendly))
			{
				if(GetLastGuyHit()->aiType!=MONS_BOUAPHA)
				{
					ScanGuy(me->shoot,GetLastGuyHit());
					me->type=BLT_SCANLOCK;
					me->timer=60;
					me->target=GetLastGuyHit()->ID;
				}
			}
			break;
		case BLT_LASER:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,1,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_BULLETHIT,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_LASER2:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,80,map,world,me->friendly))
			{
				ExplodeParticlesColor(4,me->x,me->y,me->z,2,5);
				MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,900);
			}
			if(me->timer<30*10-2 && FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,80,map,world,1-me->friendly))
			{
				ExplodeParticlesColor(4,me->x,me->y,me->z,2,5);
				MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,900);
			}
		case BLT_GREEN:
		case BLT_BADGREEN:
		case BLT_SCANSHOT:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,3,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SLIME,me->x,me->y,me->z,8);
				MakeSound(SND_BULLETHIT,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_BALLLIGHTNING:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
				MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,900);
				for(i=0;i<player.hammers-1;i++)
					FireBullet(me->shoot,me->x,me->y,0,BLT_LIGHTNING2,me->friendly);
				me->type=BLT_NONE;
			}
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_WAND:
		case BLT_WAND2:
		case BLT_LUNA:
		case BLT_LUNA2:
		case BLT_BIGSHELL:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,5,map,world,me->friendly))
			{
				if(!reflect)
				{
					me->type=BLT_NONE;
					ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
					MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
				}
				else
				{
					me->friendly=1-me->friendly;
					me->type=BLT_EVILHAMMER;
					me->facing=(me->facing+4)&7;
					me->dx=Cosine(me->facing*32)*12;
					me->dy=Sine(me->facing*32)*12;
					me->timer+=10;
				}
			}
			break;
		case BLT_FART:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx/2,me->dy/2,1,map,world,me->friendly))
			{
				PoisonVictim(GetLastGuyHit(),32);
			}
			break;
		case BLT_BOOMERANG:
			if(FindVictims2(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,(4-Random(9))<<FIXSHIFT,
				(4-Random(9))<<FIXSHIFT,1,map,world,me->friendly))
			{
				ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,10,8);
			}
			break;
		case BLT_ROCKET:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,16,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_CHEESEHAMMER:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,5,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
		case BLT_SKULL:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,8,map,world,me->friendly))
			{
				me->facing=(byte)Random(256);
				me->dx=Cosine(me->facing)*10;
				me->dy=Sine(me->facing)*10;
				me->facing=(me->facing+8)&255;
				me->facing/=16;
			}
			break;
		case BLT_PAPER:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,5,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_SPEAR:
		case BLT_HARPOON:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,5,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_BADSPEAR:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,3,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_MISSILE:
		case BLT_WIND:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,8,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_TORPEDO:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,2,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_LILBOOM:
			if(FindVictims(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,(8-Random(17))<<FIXSHIFT,
				(8-Random(16))<<FIXSHIFT,2,map,world,me->friendly))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_MEGABOOM:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,100,0,0,4,map,world,me->friendly))
			{
				// ouch
				GetLastGuyHit()->ignited=192;
			}
			break;
		case BLT_SLASH:
			if(FindVictims(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,Cosine(me->facing*32)*4,
				Sine(me->facing*32)*4,6,map,world,me->friendly))
			{
				// nothing needs to be done
			}
			break;
		case BLT_FLAME:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let them scream
				IgniteVictim(GetLastGuyHit(),16);
			}
			break;
		case BLT_FLAME2:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let him scream
				IgniteVictim(GetLastGuyHit(),16);
			}
			break;
		case BLT_FLAME5:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let him scream
				IgniteVictim(GetLastGuyHit(),16);
			}
			break;
		case BLT_FLAMEWALL:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let him scream
				IgniteVictim(GetLastGuyHit(),16);
			}
			break;
		case BLT_REDNADE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let him scream
			}
			break;
		case BLT_BIGYELLOW:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,16,map,world,me->friendly))
			{
				// hit noise
				BulletRanOut(me,map,world);
				IgniteVictim(GetLastGuyHit(),32);
			}
			break;
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
			if(FindVictims2(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				BlowSmoke(me->x,me->y,me->z,FIXAMT);
				IgniteVictim(GetLastGuyHit(),16);
				// no noise, just let them scream
			}
		case BLT_FLAME4:
			if(FindVictims2(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,me->dx,me->dy,1,map,world,me->friendly))
			{
				BlowSmoke(me->x,me->y,me->z,FIXAMT);
				IgniteVictim(GetLastGuyHit(),32);
				// no noise, just let them scream
			}
			break;
		case BLT_SPORE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/2,me->dy/2,1,map,world,me->friendly))
			{

				PoisonVictim(GetLastGuyHit(),30);
				me->type=BLT_NONE;	// go away
			}
			break;
		case BLT_SPOREGOOD:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/2,me->dy/2,0,map,world,me->friendly*-1+1))
			{
				ColorDrop(1,me->x,me->y,me->z);
				HealVictim(GetLastGuyHit(),GetLastGuyHit()->hp*0.10+1);
				me->type=BLT_NONE;	// go away
			}
			break;
		case BLT_CLAWS:
		case BLT_SPINE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/4,me->dy/4,4,map,world,me->friendly))
			{
				me->type=BLT_NONE;
			}
			break;
		case BLT_SHROOM:
		case BLT_EGG:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate, not to mention the 10 damage you already did
			}
			break;
		case BLT_PUMPKIN:
		case BLT_PIE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,20,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate, not to mention the 20 damage you already did
			}
			break;
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,
				6,map,world,me->friendly))
			{
				// nothing special happens
			}
			break;
		case BLT_BOMB_HOMING:
		case BLT_BOMB:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,0,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_MINE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,0,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_BOOM:
			if(FindVictims(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,64,(8-Random(17))<<FIXSHIFT,
				(8-Random(16))<<FIXSHIFT,4,map,world,me->friendly))
			{
				// nothing much to do here, the victim will scream quite enough
				IgniteVictim(GetLastGuyHit(),96);
			}
			break;
		case BLT_BIGAXE:
			if(FindVictims2(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,(4-Random(9))<<FIXSHIFT,
				(4-Random(9))<<FIXSHIFT,5,map,world,me->friendly))
			{
				ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,10,8);
			}
			break;
		case BLT_AXE:
			if(FindVictims2(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,(4-Random(9))<<FIXSHIFT,
				(4-Random(9))<<FIXSHIFT,5,map,world,me->friendly))
			{
				ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,5,8);
			}
			break;
		case BLT_YELBOOM:
			i=20*(5-(me->timer/2));	// size expands as boom expands
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,(8-Random(17))<<FIXSHIFT,
				(8-Random(16))<<FIXSHIFT,2,map,world,me->friendly))
			{
				// don't disappear because Bouapha needs to get multipounded
				IgniteVictim(GetLastGuyHit(),8);
			}
			break;
		case BLT_SHOCKWAVE:
			i=30*(3-(me->timer/2))+30;	// size expands as wave expands
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,0,0,4,map,world,me->friendly))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_SHOCKWAVE2:
			i=30*(3-(me->timer/2))+30;	// size expands as wave expands
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,0,0,6,map,world,me->friendly))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_ACID:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,1000);
				ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			}
			break;
		case BLT_SHARK:
		case BLT_SHARKGOOD:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,1000);
				ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
				GetLastGuyHit()->ignited=0;
			}
			break;
		case BLT_BUBBLE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_BUBBLEPOP;
				me->anim=0;
				me->timer=10;
				me->dx=0;
				me->dy=0;
				MakeSound(SND_BUBBLEPOP,me->x,me->y,SND_CUTOFF,1000);
				ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
				GetLastGuyHit()->ignited=GetLastGuyHit()->ignited/2;
			}
			break;
		case BLT_MEGABEAM1:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,40,0,12*FIXAMT,4,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_MEGABEAMHIT,me->x,me->y,SND_CUTOFF,1000);
				me->type=BLT_MEGABEAM2;
				me->dy=0;
				me->timer=2;
				IgniteVictim(GetLastGuyHit(),12);
			}
			break;
		case BLT_ENERGY_BOUNCE:
		case BLT_ENERGY_WAVE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
		case BLT_ENERGY:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,3,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,6);
			}
			break;
		case BLT_EVILHAMMER:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,950);
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			}
			break;
		case BLT_ROCK:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,8,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
				ExplodeParticles2(PART_DIRT,me->x,me->y,me->z,12,6);
			}
			break;
		case BLT_ROCKBIG:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,20,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
				ExplodeParticles2(PART_DIRT,me->x,me->y,me->z,20,6);
			}
			break;
		case BLT_SNOWBALL:
			if(me->z<FIXAMT*40)
			{
				if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,6,map,world,me->friendly))
				{
					BulletRanOut(me,map,world);
					GetLastGuyHit()->ignited=GetLastGuyHit()->ignited/2;
				}
			}
			break;
		case BLT_BIGSNOW:
			if(me->z<FIXAMT*40)
			{
				if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world,me->friendly))
				{
					BulletRanOut(me,map,world);
					GetLastGuyHit()->ignited=GetLastGuyHit()->ignited/2;
				}
			}
			break;
		case BLT_BIGSNOW2:
			if(me->z<FIXAMT*40)
			{
				if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,12,map,world,me->friendly))
				{
					BulletRanOut(me,map,world);
					GetLastGuyHit()->ignited=GetLastGuyHit()->ignited/2;
				}
			}
			break;
		case BLT_FREEZE:
		case BLT_FREEZE2:
		case BLT_ICESHARD:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,0,map,world,me->friendly))
			{
				if(FreezeGuy(GetLastGuyHit()))
				{
					MakeSound(SND_FREEZE,me->x,me->y,SND_CUTOFF,1400);
				}
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_MINDWIPE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,0,map,world,me->friendly))
			{
				if(ControlMind(GetLastGuyHit()))
				{
					MakeSound(SND_ROBOBOUAPHAON,me->x,me->y,SND_CUTOFF,1400);
				}
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_WITCH:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,0,map,world,me->friendly))
			{
				if(SwitchBody(GetLastGuyHit()))
				{
					MakeSound(SND_ROBOBOUAPHAON,me->x,me->y,SND_CUTOFF,1400);
				}
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_REFLECT:
			ReflectBullets(me->shoot,me->x,me->y,128,me->friendly);
			break;
		case BLT_SWAP:
			if(SwapMe(me->x,me->y,10,map))
				BulletRanOut(me,map,world);
			break;
		case BLT_BEAM:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,6,map,world,me->friendly))
			{
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
				MakeSound(SND_MEGABEAMHIT,me->x,me->y,SND_CUTOFF,900);
				me->type=BLT_BEAM2;
				me->timer=8;
			}
			break;
		case BLT_YELWAVE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,6,map,world,me->friendly))
			{
				WeakenVictim(GetLastGuyHit(),64);
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_PNKWAVE:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,0,map,world,me->friendly))
			{
				ConfuseVictim(GetLastGuyHit(),255);
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_REDBOOM:
			i=20*(5-(me->timer/2));	// size expands as boom expands
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,(8-Random(17))<<FIXSHIFT,
				(8-Random(16))<<FIXSHIFT,2,map,world,me->friendly))
			{
				WeakenVictim(GetLastGuyHit(),128);
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_SLIME:
			if(FindVictim(me->shoot,me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				if (me->anim%2==0)
				{ExplodeParticles(PART_WATER,me->x,me->y,me->z,4);
				MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,1000);}
				GetLastGuyHit()->ignited=GetLastGuyHit()->ignited/2;
			}
			break;
	}
	attackType=BLT_NONE;
}

inline void BulletFaceGuy(bullet_t *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

	if(goodguy==NULL)
		return;

	if(goodguy->x<me->x-FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=5;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=3;
		else
			desired=4;
	}
	else if(goodguy->x>me->x+FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=7;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=1;
		else
			desired=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=6;
		else
			desired=2;
	}

	if(desired==me->facing)
		return;

	if(desired>me->facing)
	{
		diff=desired-me->facing;
		if(diff>4)
		{
			dir=-1;
			diff=8-diff;
		}
		else
			dir=1;
	}
	else
	{
		diff=me->facing-desired;
		if(diff>4)
		{
			dir=1;
			diff=8-diff;
		}
		else
			dir=-1;
	}
	me->facing=(me->facing+dir)&7;
}

void UpdateBullet(bullet_t *me,Map *map,world_t *world)
{
	int mapx,mapy;
	int x,y,x2,y2,v,v2,rv;
	byte b;
	word w;
	
	if(profile.progress.purchase[modeShopNum[MODE_REVERSE]]&SIF_ACTIVE)
		rv = -1;
	else
		rv = 1;

	OffScreenBulletDie(me,map);
	if(!me->type)
		return;

	reflect=0;

	mapx=(me->x/TILE_WIDTH)>>FIXSHIFT;
	mapy=(me->y/TILE_HEIGHT)>>FIXSHIFT;
	me->bright=map->GetTile(mapx,mapy)->templight;

	activeBulDX=me->dx;
	activeBulDY=me->dy;
	b=0;

	if(map->flags&MAP_UNDERWATER)
	{
		if(!player.timeStop)
			me->x+=me->dx*3/4 + me->ax*3/4*rv;
		if(!BulletCanGo(me->type,me->x,me->y,map,8,me->friendly))
			BulletHitWallX(me,map,world);
		else
		{
			if(!player.timeStop)
			me->y+=me->dy*3/4 + me->ay*3/4*rv;
			if(!BulletCanGo(me->type,me->x,me->y,map,8,me->friendly))
				BulletHitWallY(me,map,world);
		}
		if(!player.timeStop)
		{
			if((GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_SPACE && me->timer%4==0)||!(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_SPACE))
				me->z+=me->dz*3/4;
		}
	}
	else
	{
		if(!player.timeStop)
			me->x+=me->dx + me->ax;
		if(!BulletCanGo(me->type,me->x,me->y,map,8,me->friendly))
			BulletHitWallX(me,map,world);
		else
		{
			if(!player.timeStop)
				me->y+=me->dy + me->ay;
			if(!BulletCanGo(me->type,me->x,me->y,map,8,me->friendly))
				BulletHitWallY(me,map,world);
		}
		if(!player.timeStop)
		{
			if((GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_SPACE && me->timer%4==0)||!(GetTerrain(world,map->GetTile(x,y)->floor)->flags&TF_SPACE))
				me->z+=me->dz;
		}
	}

	if(me->z<0)
		BulletHitFloor(me,map,world);

	// all gravity-affected bullets, get gravitized

	if(!player.timeStop)
	{
		if(me->type==BLT_HAMMER || me->type==BLT_HAMMER2 || me->type==BLT_BOMB || me->type==BLT_GRENADE
			|| me->type==BLT_ROCK || me->type==BLT_EVILHAMMER || me->type==BLT_SPEAR || me->type==BLT_BADSPEAR
			|| me->type==BLT_BUBBLE || me->type==BLT_SITFLAME || me->type==BLT_BADSITFLAME || me->type==BLT_FLAME4
			|| me->type==BLT_REDNADE || me->type==BLT_FLAME3 || me->type==BLT_PUMPKIN || me->type==BLT_EGG || me->type==BLT_BOMB_HOMING || me->type==BLT_ROCKBIG)
			me->dz-=FIXAMT;
		me->timer--;
		if(!me->timer)
			BulletRanOut(me,map,world);
	}

	if(!me->type)
		return;

	// special things like animation
	switch(me->type)
	{
		case BLT_HOLESHOT:
			me->anim=1-me->anim;
			me->dx=(me->dx*19)/20;
			me->dy=(me->dy*19)/20;
			break;
		case BLT_BLACKHOLE:
			SuckParticle(me->x,me->y,FIXAMT*20);
			map->BrightTorch(mapx,mapy,-10,2);
			SuckInEvil(me->x,me->y,me->friendly);
			if(Random(2)==0)
				HitBadguys(me,map,world);
			break;
		case BLT_LUNA:
		case BLT_LUNA2:
			me->dz=0;
			//ExplodeParticlesColor(4,me->x,me->y,me->z,1,1);
			ColorDrop(4,me->x,me->y,me->z);
			HitBadguys(me,map,world);
			break;
		case BLT_LUNAX1:
			me->dz=0;
			//ExplodeParticlesColor(4,me->x,me->y,me->z,1,1);
			ColorDrop(Random(8)-1,me->x,me->y,me->z);
			HitBadguys(me,map,world);
			break;
		case BLT_LUNAX2:
			me->dz=0;
			//ExplodeParticlesColor(4,me->x,me->y,me->z,1,1);
			ColorDropSize(5,me->x,me->y,me->z,50);
			me->facing+=6-Random(13);
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			HitBadguys(me,map,world);
			break;
		case BLT_ROCKET:
			me->dz=0;
			//ExplodeParticlesColor(4,me->x,me->y,me->z,1,1);
			ColorDrop(6,me->x,me->y,me->z);

			HitBadguys(me,map,world);
			break;
		case BLT_FART:
			if(me->timer>=10 && me->timer<=50)
			{
				if(!player.timeStop)
					me->anim++;
				if(me->anim>5)
					me->anim=3;
				Dampen(&me->dx,FIXAMT/16);
				Dampen(&me->dy,FIXAMT/16);
				HitBadguys(me,map,world);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_BOOMERANG: //it does stuff!
			if(!player.timeStop)
				me->anim++;
			if(me->anim>15)
				me->anim=0;
			HitBadguys(me,map,world);
			if(me->timer==179)
			{
				if(!me->friendly)
					me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				else
					me->target=LockOnGood(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			}
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if(me->x>mapx)
					me->dx-=FIXAMT;
				else
					me->dx+=FIXAMT;
				if(me->y>mapy)
					me->dy-=FIXAMT;
				else
					me->dy+=FIXAMT;

				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);

				if(me->dx>0)
				{
					if(me->facing>8)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dx<0)
				{
					if(me->facing>8)
						me->facing--;
					else
						me->facing++;
				}
				if(me->dy>0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dy<0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing--;
					else
						me->facing++;
				}

				if(me->facing>200)
					me->facing+=16;
				if(me->facing>15)
					me->facing-=16;
			}
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>15)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_WAND:
		case BLT_WAND2:
		case BLT_WITCH:
			if(me->anim>4)
			{
				me->anim=0;
				ExplodeParticles(PART_SHORTYELLOW,me->x,me->y,me->z,2);
			}
			else if(!player.timeStop)
				me->anim++;
			HitBadguys(me,map,world);
			break;
		case BLT_CHEESEHAMMER:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>7)
				me->anim=0;
			me->facing+=6-Random(13);
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			HitBadguys(me,map,world);
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
			me->dz+=FIXAMT/2;
			HitBadguys(me,map,world);
			break;
		case BLT_HARPOON:
			me->dz-=FIXAMT/8;
			HitBadguys(me,map,world);
			break;
		case BLT_BUBBLE:
			me->dz+=FIXAMT/2;	// less gravity
			HitBadguys(me,map,world);
			if(!player.timeStop)
				me->anim++;
			if(me->anim>=7*4)
				me->anim=7*4-1;
			if(me->timer==1)
			{
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_BUBBLEPOP:
			me->dz=0;
			if(!player.timeStop)
				me->anim++;
			if(me->anim==4)
				me->type=BLT_NONE;
			break;
		case BLT_PAPER:
			if(!player.timeStop)
				me->anim++;
			if(me->anim==16)
				me->anim=0;
			HitBadguys(me,map,world);
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);
			break;
		case BLT_SLASH:
			if(!player.timeStop)
				me->anim++;
			if(me->anim==4)
				BulletRanOut(me,map,world);
			else if(me->anim==2)
			{
				HitBadguys(me,map,world);
				if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_CHOP)
				{
					if(!editing && !player.cheated)
						profile.progress.grassChopped++;
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			}
			break;
		case BLT_BIGAXE:
		case BLT_AXE:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>7)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_EVILFACE:
			me->anim+=(byte)Random(3);
			if(me->anim>=6*16)
				me->anim=6*16-1;
			me->dx+=-FIXAMT/16+Random(FIXAMT/8);
			me->dy-=Random(FIXAMT/16);
			break;
		case BLT_BIGSHELL:
		case BLT_SPINE:
		case BLT_CLAWS:
			HitBadguys(me,map,world);
			break;
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
			if(me->timer>4)
				me->anim=(8-me->timer);
			else
				me->anim=me->timer-1;
			if(me->timer==5)
				HitBadguys(me,map,world);
			else if(me->timer==4)
			{
				b=(me->facing*32-32+Random(65))&255;
				mapx=(me->x+Cosine(b)*32);
				mapy=(me->y+Sine(b)*32);
				FireBullet(me->shoot,mapx,mapy,me->facing,me->type,me->friendly);
			}
			break;
		case BLT_ROCK:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>3)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_ROCKBIG:
			if(me->anim>5)
				me->anim=0;
			else if(!player.timeStop)
				me->anim++;
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY_WAVE:
			me->anim=1-me->anim;
			if(me->timer%15==0)
			{
				me->ax *= -1;
				me->ay *= -1;
			}
			//how to???
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY_CIRCLS1:
			me->anim=1-me->anim;
			if(me->timer%7==0){
				me->facing += 3;
				me->dx = Cosine(me->facing)*8;
				me->dy = Sine(me->facing)*8;
			}
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY_CIRCLS2:
			me->anim=1-me->anim;
			if(me->timer%7==0){
				me->facing -= 3;
				me->dx = Cosine(me->facing)*8;
				me->dy = Sine(me->facing)*8;
			}
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY_CIRCLF1:
			me->anim=1-me->anim;
			if(me->timer%7==0){
				me->facing += 6;
				me->dx = Cosine(me->facing)*8;
				me->dy = Sine(me->facing)*8;
			}
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY_CIRCLF2:
			me->anim=1-me->anim;
			if(me->timer%7==0){
				me->facing -= 6;
				me->dx = Cosine(me->facing)*8;
				me->dy = Sine(me->facing)*8;
			}
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY_BOUNCE:
		case BLT_ENERGY:
			me->anim=1-me->anim;	// flip-flop animation
			HitBadguys(me,map,world);
			break;
		case BLT_GREEN:
		case BLT_BADGREEN:
		case BLT_LILBOOM:
		case BLT_MEGABOOM:
			if(!player.timeStop)
				me->anim++;
			if(me->anim<2)
				HitBadguys(me,map,world);
			break;
		case BLT_LILBOOM2:
			break;
		case BLT_ACID:
		case BLT_SHARK:
		case BLT_SHARKGOOD:
		case BLT_FREEZE2:
			me->dz-=FIXAMT/2;	// less gravity than normal things
			if(!player.timeStop)
				me->anim++;
			if(me->anim>6)
				me->anim=0;
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_FROZEN)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_FREEZE:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>6)
				me->anim=0;
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_FROZEN)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_SPORE:
		case BLT_SPOREGOOD:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>3*4+3)
				me->anim=3*4+3;
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			HitBadguys(me,map,world);
			break;
		case BLT_SNOWBALL:
		case BLT_ICESHARD:
			me->dz-=FIXAMT/2;	// less gravity than normal things
			HitBadguys(me,map,world);
			break;
		case BLT_BIGSNOW:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>5)
				me->anim=0;
			me->dz-=FIXAMT/2;	// less gravity than normal things
			if(me->z<64*FIXAMT)
				HitBadguys(me,map,world);
			break;
		case BLT_BIGSNOW2:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>5)
				me->anim=0;
			me->dz-=FIXAMT/2;	// less gravity than normal things
			if(me->z<64*FIXAMT)
				HitBadguys(me,map,world);
			break;
		case BLT_GRENADE:
			me->anim=1-me->anim;
			break;
		case BLT_BIGYELLOW:
			me->anim=1-me->anim;
			HitBadguys(me,map,world);
			break;
		case BLT_SHOCKWAVE2:
			PushOutEvil(me->x,me->y,me->friendly);
		case BLT_SHOCKWAVE:
			if(!player.timeStop)
				me->anim++;
			HitBadguys(me,map,world);
			break;
		case BLT_REDNADE:
			me->anim=1-me->anim;
			if (me->z<10)
			HitBadguys(me,map,world);
			break;
		case BLT_YELBOOM:
			if(!player.timeStop)
				me->anim++;
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BOMBED)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_SHROOM:
		case BLT_PUMPKIN:
		case BLT_EGG:
			HitBadguys(me,map,world);
			break;
		case BLT_PIE:
			me->dz-=FIXAMT/2;	// less gravity than normal things
			if (me->anim==0)
			me->anim = 1;
			else
			me->anim = 0;
			HitBadguys(me,map,world);
			break;
		case BLT_MEGABEAM:
			if(me->anim<(4*5) && !player.timeStop)
				me->anim++;
			else	// FIRE!
				FireBulletAfter(me->shoot,me->x,me->y,0,BLT_MEGABEAM1,me,me->friendly);

			// keep this beam locked onto its launcher
			if(GetGuyPos(me->target,&me->x,&me->y))
			{
				if(GetGuy(me->target) && GetGuy(me->target)->aiType==MONS_DOZER)
					me->y+=FIXAMT*120;
				else
					me->y+=FIXAMT*8;
			}

			break;
		case BLT_MEGABEAM1:
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BOMBED)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_MEGABEAM2:
			// nothin
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_FLAME5:
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
				{
					if(!editing && !player.cheated)
						profile.progress.hayBurned++;
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			if(me->timer&1)	// every other frame
				HitBadguys(me,map,world);
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
			me->dz+=Random(FIXAMT/8);		//anti gravity
			me->dx+=Random(FIXAMT)-FIXAMT/2;
			me->dy+=Random(FIXAMT)-FIXAMT/2;
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			Clamp(&me->dx,FIXAMT*10);
			Clamp(&me->dy,FIXAMT*10);
			me->anim=((32-me->timer)/8)+1;
			if(me->anim>4)
				me->anim=4;
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_FLAMEWALL:
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
				{
					if(!editing && !player.cheated)
						profile.progress.hayBurned++;
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			if(me->timer>6)
				me->anim=(me->timer-6);
			else
				me->anim=(6-me->timer);
			if(me->anim>3)
				HitBadguys(me,map,world);
			
			if(me->timer&1)	// every other frame
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_HOTPANTS:
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
				{
					if(!editing && !player.cheated)
						profile.progress.hayBurned++;
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->anim>3)
				HitBadguys(me,map,world);
			
		case BLT_FLAME3:
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
				{
					if(!editing && !player.cheated)
						profile.progress.hayBurned++;
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
			me->dz+=Random(FIXAMT/8);		//anti gravity
			me->dx+=Random(FIXAMT)-FIXAMT/2;
			me->dy+=Random(FIXAMT)-FIXAMT/2;
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			Clamp(&me->dx,FIXAMT*10);
			Clamp(&me->dy,FIXAMT*10);
			me->anim=((me->timer)/8)+1;
			if(me->anim>4)
				me->anim=4;
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_FLAME4:
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
				{
					if(!editing && !player.cheated)
						profile.progress.hayBurned++;
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			if(me->timer&1)	// every other frame
			{
				HitBadguys(me,map,world);
				map->DimTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,10);
			}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
			me->dz+=FIXAMT/2;		//anti gravity
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);
			Clamp(&me->dx,FIXAMT*10);
			Clamp(&me->dy,FIXAMT*10);
			me->anim=Random(5);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_LASER:
		case BLT_LASER2:
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_BOMB_HOMING:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>15)
				me->anim=0;
			
			if(me->timer==40)
			{
				if(me->friendly)
					me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				else
					me->target=LockOnGood(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			}
			
			if(!player.timeStop)
				me->anim++;
			if(((me->anim>0) && (me->target!=65535)) ||
				((me->anim>2) && (me->target==65535)))
			{
				if(map->flags&MAP_UNDERWATER)
					BlowBubble(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				else
					BlowSmoke(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
			}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,12,4);
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if(me->x>mapx)
					me->dx-=FIXAMT;
				else
					me->dx+=FIXAMT;
				if(me->y>mapy)
					me->dy-=FIXAMT;
				else
					me->dy+=FIXAMT;

				Clamp(&me->dx,FIXAMT*8);
				Clamp(&me->dy,FIXAMT*8);

				if(me->dx>0)
				{
					if(me->facing>8)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dx<0)
				{
					if(me->facing>8)
						me->facing--;
					else
						me->facing++;
				}
				if(me->dy>0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dy<0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing--;
					else
						me->facing++;
				}

				if(me->facing>200)
					me->facing+=16;
				if(me->facing>15)
					me->facing-=16;
			}
			break;
		case BLT_BOMB:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>15)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_MINE:
			HitBadguys(me,map,world);
			me->anim+=me->facing-1;
			me->timer++;
			if(me->anim==0)
				me->facing=2;
			else if(me->anim==5*4-1)
			{
				me->timer--;
				me->facing=0;
			}
			if(map->flags&MAP_UNDERWATER)	// floating mines
			{
				if(me->z<FIXAMT*20)
					me->dz+=FIXAMT/4;
				else
					me->dz-=FIXAMT/4;
			}
			break;
		case BLT_BOOM:
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BOMBED)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_SCANSHOT:
			HitBadguys(me,map,world);
			if(!player.timeStop)
				me->anim++;
			if(me->anim==3)
				me->anim=0;
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if(me->x>mapx)
					me->dx-=FIXAMT;
				else
					me->dx+=FIXAMT;
				if(me->y>mapy)
					me->dy-=FIXAMT;
				else
					me->dy+=FIXAMT;

				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);
			}
			break;
		case BLT_WIND:
			if(me->timer==40)
			{
				if(me->friendly)
					me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				else
					me->target=LockOnGood(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			}
			if(!player.timeStop)
				me->anim++;
			if(((me->anim>0) && (me->target!=65535)) ||
				((me->anim>2) && (me->target==65535)))
			{
				if(map->flags&MAP_UNDERWATER)
					BlowBubble(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				else
					BlowSmoke(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				me->anim=0;
			}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,12,4);

			HitBadguys(me,map,world);
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if(me->x>mapx)
					me->dx-=FIXAMT;
				else
					me->dx+=FIXAMT;
				if(me->y>mapy)
					me->dy-=FIXAMT;
				else
					me->dy+=FIXAMT;

				Clamp(&me->dx,FIXAMT*8);
				Clamp(&me->dy,FIXAMT*8);

				if(me->dx>0)
				{
					if(me->facing>8)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dx<0)
				{
					if(me->facing>8)
						me->facing--;
					else
						me->facing++;
				}
				if(me->dy>0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dy<0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing--;
					else
						me->facing++;
				}

				if(me->facing>200)
					me->facing+=16;
				if(me->facing>15)
					me->facing-=16;
			}
			break;
		case BLT_MISSILE:
		case BLT_SKULL:
			if(me->timer==40)
			{
				if(me->friendly)
					me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				else
					me->target=LockOnGood(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			}
			if(!player.timeStop)
				me->anim++;
			if(((me->anim>0) && (me->target!=65535)) ||
				((me->anim>2) && (me->target==65535)))
			{
				if(map->flags&MAP_UNDERWATER)
					BlowBubble(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				else
					BlowSmoke(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				me->anim=0;
			}
			if (me->type==BLT_SKULL)
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT, (me->y/TILE_HEIGHT)>>FIXSHIFT,4,2);
			else
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT, (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);

			HitBadguys(me,map,world);
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if(me->x>mapx)
					me->dx-=FIXAMT;
				else
					me->dx+=FIXAMT;
				if(me->y>mapy)
					me->dy-=FIXAMT;
				else
					me->dy+=FIXAMT;

				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);

				if(me->dx>0)
				{
					if(me->facing>8)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dx<0)
				{
					if(me->facing>8)
						me->facing--;
					else
						me->facing++;
				}
				if(me->dy>0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing++;
					else
						me->facing--;
				}
				if(me->dy<0)
				{
					if(me->facing>11 || me->facing<4)
						me->facing--;
					else
						me->facing++;
				}

				if(me->facing>200)
					me->facing+=16;
				if(me->facing>15)
					me->facing-=16;
			}
			break;
		case BLT_TORPEDO:
			if(!player.timeStop)
				me->anim++;
			if(me->anim==9)
			{
				BlowBubble(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				me->anim=0;
			}
			me->z+=me->dz;	// don't actually move by dz, it is just used to tell it which way to veer
			HitBadguys(me,map,world);
			me->dx+=Cosine(me->target/16)/2;
			me->dy+=Sine(me->target/16)/2;
			me->target+=me->dz;
			if(me->target>65536-256)
				me->target+=256*16;
			if(me->target>=256*16)
				me->target-=256*16;
			me->facing=me->target/256;
			Clamp(&me->dx,FIXAMT*16);
			Clamp(&me->dy,FIXAMT*16);
			break;
		case BLT_LIGHTNING:
			w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			if(w!=65535)
			{
				GetGuy(w)->GetShot(me->shoot,GetGuy(w),0,0,2,map,world);
				LightningBolt(goodguy->x,goodguy->y-FIXAMT*20,GetGuy(w)->x,
					GetGuy(w)->y-FIXAMT*10-GetGuy(w)->z);
				MakeSound(SND_ZAP,GetGuy(w)->x,GetGuy(w)->y,SND_CUTOFF,1000);
			}
			else
			{
				// make sizzle around player if there was no target
				LightningBolt(
					goodguy->x-FIXAMT*32+Random(FIXAMT*64),
					goodguy->y-FIXAMT*52+Random(FIXAMT*64),
					goodguy->x-FIXAMT*32+Random(FIXAMT*64),
					goodguy->y-FIXAMT*52+Random(FIXAMT*64));
			}
			me->type=BLT_NONE;	// begone immediately
			break;
		case BLT_ORBITER:
			me->timer++;
			if(--me->anim==0)
			{
				me->timer--;
				me->anim=15;
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				if(me->target!=65535)
				{
					FireBullet(me->shoot,me->x,me->y,me->facing*32,BLT_GREEN,me->friendly);
				}
			}
			if(me->target!=65535)
			{
				BulletFaceGuy(me,GetGuy(me->target));
			}
			if(me->x<goodguy->x)
				me->dx+=FIXAMT;
			if(me->y<goodguy->y)
				me->dy+=FIXAMT;
			if(me->x>goodguy->x)
				me->dx-=FIXAMT;
			if(me->y>goodguy->y)
				me->dy-=FIXAMT;
			Clamp(&me->dx,FIXAMT*9);
			Clamp(&me->dy,FIXAMT*9);
			break;
		case BLT_ORBITER2:
			me->timer++;
			if(--me->anim==0)
			{
				me->timer--;
				me->anim=45;
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				if(me->target!=65535)
				{
					FireBullet(me->shoot,me->x,me->y,me->facing*32,BLT_BOMB,me->friendly);
				}
			}
			if(me->target!=65535)
			{
				BulletFaceGuy(me,GetGuy(me->target));
			}
			if(me->x<goodguy->x)
				me->dx+=FIXAMT;
			if(me->y<goodguy->y)
				me->dy+=FIXAMT;
			if(me->x>goodguy->x)
				me->dx-=FIXAMT;
			if(me->y>goodguy->y)
				me->dy-=FIXAMT;
			Clamp(&me->dx,FIXAMT*9);
			Clamp(&me->dy,FIXAMT*9);
			break;
		case BLT_ORBITER3:
			me->timer++;
			if(--me->anim==0)
			{
				me->timer--;
				me->anim=45;
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				if(me->target!=65535)
				{
					if (Random(2))
					FireBullet(me->shoot,me->x,me->y,me->facing*32,BLT_SHARK,me->friendly);
					else
					FireBullet(me->shoot,me->x,me->y,me->facing*32,BLT_FREEZE,me->friendly);
						
				}
			}
			if(me->target!=65535)
			{
				BulletFaceGuy(me,GetGuy(me->target));
			}
			if(me->x<goodguy->x)
				me->dx+=FIXAMT;
			if(me->y<goodguy->y)
				me->dy+=FIXAMT;
			if(me->x>goodguy->x)
				me->dx-=FIXAMT;
			if(me->y>goodguy->y)
				me->dy-=FIXAMT;
			Clamp(&me->dx,FIXAMT*9);
			Clamp(&me->dy,FIXAMT*9);
			break;
		case BLT_ORBITER4:
			me->timer++;
			if(--me->anim==0)
			{
				me->timer--;
				me->anim=45;
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				if(me->target!=65535)
				{
					if (Random(2))
					FireBullet(me->shoot,me->x,me->y,me->facing*32,BLT_SHARK,me->friendly);
					else
					FireBullet(me->shoot,me->x,me->y,me->facing*32,BLT_FREEZE,me->friendly);
						
				}
			}
			if(me->target!=65535)
			{
				BulletFaceGuy(me,GetGuy(me->target));
			}
			if(me->x<goodguy->x)
				me->dx+=FIXAMT;
			if(me->y<goodguy->y)
				me->dy+=FIXAMT;
			if(me->x>goodguy->x)
				me->dx-=FIXAMT;
			if(me->y>goodguy->y)
				me->dy-=FIXAMT;
			Clamp(&me->dx,FIXAMT*9);
			Clamp(&me->dy,FIXAMT*9);
			break;
		case BLT_BALLLIGHTNING:
			x=me->x;
			y=me->y-me->z;
			v=Random(256);
			v2=Random(16)+2;
			x2=(x+Cosine(v)*v2);
			y2=(y+Sine(v)*v2);
			v=Random(256);
			v2=Random(16)+2;
			x=(x+Cosine(v)*v2);
			y=(y+Sine(v)*v2);
			LightningBolt(x,y,x2,y2);
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,1);
			HitBadguys(me,map,world);
			break;
		case BLT_LIGHTNING2:
			w=LockOnEvil2(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			if(w!=65535)
			{
				GetGuy(w)->GetShot(me->shoot,GetGuy(w),0,0,5,map,world);
				LightningBolt(me->x,me->y-FIXAMT*20,GetGuy(w)->x,
					GetGuy(w)->y-FIXAMT*10-GetGuy(w)->z);
				MakeSound(SND_ZAP,GetGuy(w)->x,GetGuy(w)->y,SND_CUTOFF,1000);
			}
			else
			{
				// zap to nowhere
				LightningBolt(
					me->x,
					me->y,
					me->x-FIXAMT*64+Random(FIXAMT*128),
					me->y-FIXAMT*64+Random(FIXAMT*128));
			}
			me->type=BLT_NONE;	// begone immediately
			break;
		case BLT_MINDWIPE:
			if(!player.timeStop)
				me->anim++;
			if(me->anim>2)
				me->anim=0;

			HitBadguys(me,map,world);
			me->bright=(char)Random(16);
			break;
		case BLT_REFLECT:
			if(!player.timeStop)
				me->anim++;
			//map->BrightTorch(me->x/(TILE_WIDTH*FIXAMT),me->y/(TILE_HEIGHT*FIXAMT),me->timer,4);
			if(me->anim<3)
				HitBadguys(me,map,world);
			break;
		case BLT_SWAP:
			me->anim+=8;
			HitBadguys(me,map,world);
			break;
		case BLT_SCANNER:
			if(!player.timeStop)
				me->anim++;
			if(me->anim==3)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_SCANLOCK:
			if(me->target!=65535 && GetGuy(me->target)->type!=MONS_NONE)
			{
				me->x=GetGuy(me->target)->x;
				me->y=GetGuy(me->target)->y;
				me->z=GetGuy(me->target)->z;
				if(me->z==0)
					me->z=FIXAMT*20;
			}
			else
			{
				me->target=65535;
				if(me->timer>3)
					me->timer=3;	// go away if not on a target
			}
			me->dx=me->dy=me->dz=0;
			break;
		case BLT_SLIME:	// frame 259-270
			if(!player.timeStop)
				me->anim++;
			if(me->anim>1)
				HitBadguys(me,map,world);
			if(me->anim>12*3-1)
				me->type=0;
			break;
		case BLT_BEAM:
			if(!BulletCanGo(me->type,me->x,me->y,map,8,me->friendly))
			{
				me->type=BLT_BEAM2;
				me->timer=8;
				break;
			}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,16,4);
			HitBadguys(me,map,world);
			if(me->timer==LASER_TIME)
				FireBulletAfter(me->shoot,me->x+Cosine(me->facing*32)*24,me->y+Sine(me->facing*32)*24,me->facing,BLT_BEAM,me,me->friendly);
			me->anim=1-me->anim;
		case BLT_BEAM2:
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BURNT)
			{
				TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			}
			break;
		case BLT_REDBOOM:
			if(!player.timeStop)
				me->anim++;
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			HitBadguys(me,map,world);
			if(GetItem(map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT))->item)->trigger&ITR_BOMBED)
				{
					TriggerItem(NULL,map->GetTile((me->x>>FIXSHIFT)/TILE_WIDTH,((me->y>>FIXSHIFT)/TILE_HEIGHT)),(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
				}
			break;
		case BLT_YELWAVE:
		case BLT_PNKWAVE:
			HitBadguys(me,map,world);
			break;
	}
}

void RenderSmoke(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_SMOKE+frm),DISPLAY_DRAWME|DISPLAY_GHOST);
}

void RenderBubble(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_BUBBLE+frm),DISPLAY_DRAWME|DISPLAY_GLOW);
}

void RenderMindControl(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_SCANSHOT+frm),DISPLAY_DRAWME|DISPLAY_GLOW);
}

void RenderBoom(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_BOOM+frm),DISPLAY_DRAWME|DISPLAY_GLOW);
}

void RenderStinky(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_STINKY+frm),DISPLAY_DRAWME|DISPLAY_GHOST);
}

void RenderBullet(bullet_t *me)
{
	int v,x,y,z;
	sprite_t *curSpr;

	switch(me->type)
	{
	
		case BLT_ROCKET:
			v=me->facing+SPR_ROCKET;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_BOOMERANG:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_BOOMERANG);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_FART:
			curSpr=bulletSpr->GetSprite(SPR_FART+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_MEGABOOM:
			curSpr=bulletSpr->GetSprite(SPR_MEGABOOM+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_EVILFACE:
			curSpr=bulletSpr->GetSprite(me->anim/16+SPR_EVILFACE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GHOST);
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
			v=me->facing*16+(me->anim)+SPR_HAMMER;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_WAND:
			v=me->facing*5+(me->anim)+SPR_WAND;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_WAND2:
			v=me->facing*5+(me->anim)+SPR_WAND;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_WITCH:
			v=me->facing*5+(me->anim)+SPR_WAND;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,5,6,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_CHEESEHAMMER:
			v=(((me->facing/32)+me->anim)&7)*16+SPR_HAMMER;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,4,5,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_EVILHAMMER:
			v=me->facing*16+(me->anim)+SPR_HAMMER;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,0,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_ACID:
			v=me->facing*7+me->anim+SPR_ACID;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_SHARK:
		case BLT_SHARKGOOD:
			v=me->facing*7+me->anim+SPR_ACID;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,1,3,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_FREEZE:
		case BLT_FREEZE2:
			v=me->facing*7+me->anim+SPR_ACID;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,1,7,me->bright+6,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_MISSILE:
			v=me->facing+SPR_MISSILE;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_WIND:
			break;
		case BLT_TORPEDO:
			v=(me->facing)+SPR_MISSILE;
			curSpr=bulletSpr->GetSprite(v);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,0,4,me->bright+3,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_FLAME3:
		case BLT_FLAME4:
		case BLT_FLAME5:
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_HOTPANTS:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_FLAME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,0,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_FLAMEWALL:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_FLAMEWALL);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,0,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_LASER:
			curSpr=bulletSpr->GetSprite(me->facing+SPR_LASER);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_CLAWS:
			curSpr=bulletSpr->GetSprite(((me->facing)/16)+SPR_CLAWS);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_LASER2:
			curSpr=bulletSpr->GetSprite(((me->facing+8)/16)+SPR_LASER);
			for(v=0;v<3;v++)
			{
				z=me->z;//+Random(FIXAMT*8);
				x=me->x-FIXAMT*10+Random(FIXAMT*20+1);
				y=me->y-FIXAMT*10+Random(FIXAMT*20+1);

				SprDrawOff(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,4,me->bright+4,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			}
			break;
		case BLT_PAPER:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_PAPER);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_SKULL:
			v=me->facing+SPR_SKULL;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BUBBLE:
			curSpr=bulletSpr->GetSprite(me->anim/4+SPR_BUBBLE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BUBBLEPOP:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_BUBBLE+5);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BOMB_HOMING:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_BOMB);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,0,6,8,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BOMB:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_BOMB);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BOOM:
			curSpr=bulletSpr->GetSprite(7-me->timer+SPR_BOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ENERGY:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_ENERGY);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ENERGY_WAVE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_ENERGY);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,4,5,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ENERGY_BOUNCE:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_ENERGY);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,4,6,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_MEGABEAM:
			curSpr=bulletSpr->GetSprite((me->anim>>2)+SPR_MEGABEAM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-8,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_MEGABEAM1:
			curSpr=bulletSpr->GetSprite(6+SPR_MEGABEAM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-8,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_MEGABEAM2:
			curSpr=bulletSpr->GetSprite(7+SPR_MEGABEAM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-8,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SPORE:
			curSpr=bulletSpr->GetSprite(me->anim/4+SPR_SPORE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_SPOREGOOD:
			curSpr=bulletSpr->GetSprite(me->anim/4+SPR_SPORE);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,5,1,me->bright+2,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			break;
		case BLT_SHROOM:
			curSpr=bulletSpr->GetSprite(me->facing+SPR_SHROOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_PUMPKIN:
			curSpr=bulletSpr->GetSprite(me->facing+SPR_PUMPKIN);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_EGG:
			curSpr=bulletSpr->GetSprite(SPR_EGG);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_PIE:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_PIE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_GRENADE:
		case BLT_BIGYELLOW:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_GRENADE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_HOLESHOT:
			curSpr=bulletSpr->GetSprite(me->anim+533);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_YELBOOM:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_YELBOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SHOCKWAVE:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_SHOCKWAVE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SHOCKWAVE2:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_SHOCKWAVE);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,6,4,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			break;
		case BLT_LILBOOM:
		case BLT_LILBOOM2:
			curSpr=bulletSpr->GetSprite(4-(me->timer/2)+SPR_LILBOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SNOWBALL:
			curSpr=bulletSpr->GetSprite(SPR_SNOWBALL);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_BIGSNOW:
			curSpr=bulletSpr->GetSprite(SPR_BIGSNOW+(me->anim/2));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_BIGSNOW2:
			curSpr=bulletSpr->GetSprite(SPR_BIGSNOW+(me->anim/2));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,0,7,me->bright+4,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			break;
		case BLT_ICESPIKE:
			curSpr=bulletSpr->GetSprite(SPR_ICESPIKE+(me->anim));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_DIRTSPIKE:
			curSpr=bulletSpr->GetSprite(SPR_ICESPIKE+(me->anim));
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,0,2,me->bright-12,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			break;
		case BLT_ROCK:
			if((me->timer<8) && (me->timer&1))
				return;	// flicker when almost gone
			curSpr=bulletSpr->GetSprite(SPR_ROCK+(me->anim));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_ROCKBIG:
			if((me->timer<8) && (me->timer&1))
				return;	// flicker when almost gone
			curSpr=bulletSpr->GetSprite(SPR_ROCKBIG+(me->anim));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_SPINE:
			curSpr=bulletSpr->GetSprite(SPR_SPINE+(me->facing));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_ICESHARD:
			curSpr=bulletSpr->GetSprite(SPR_SPINE+(me->facing));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,5,0,me->bright+8,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			break;
		case BLT_BIGSHELL:
			curSpr=bulletSpr->GetSprite(SPR_BOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BIGAXE:
			curSpr=bulletSpr->GetSprite(SPR_BIGAXE+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_AXE:
			curSpr=bulletSpr->GetSprite(SPR_AXE+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_HARPOON:
			curSpr=bulletSpr->GetSprite(SPR_SPEAR+me->facing);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_SLASH:
			if(me->anim>0)
			{
				curSpr=bulletSpr->GetSprite(SPR_SLASH+me->facing*3+me->anim-1);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GHOST);
			}
			break;
		case BLT_MINE:
			curSpr=bulletSpr->GetSprite(SPR_MINE+me->anim/4);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			if(curMap->flags&MAP_UNDERWATER)	// underwater, it has a shadow, since it floats
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ORBITER:
			curSpr=bulletSpr->GetSprite(SPR_ORBITER+me->facing);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ORBITER2:
			curSpr=bulletSpr->GetSprite(SPR_ORBITER+me->facing);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,1,4,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ORBITER3:
			curSpr=bulletSpr->GetSprite(SPR_ORBITER+me->facing);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,1,7,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ORBITER4:
			curSpr=bulletSpr->GetSprite(SPR_ORBITER+me->facing);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,1,5,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_GREEN:
			curSpr=bulletSpr->GetSprite(SPR_GREEN);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BADGREEN:
			curSpr=bulletSpr->GetSprite(SPR_GREEN);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-4,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_SCANSHOT:
		case BLT_SCANNER:
			curSpr=bulletSpr->GetSprite(SPR_SCANSHOT+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SCANLOCK:
			if(me->timer>10 || (me->timer&1))
			{
				curSpr=bulletSpr->GetSprite(SPR_SCANLOCK);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
						DISPLAY_DRAWME|DISPLAY_GLOW);
			}
			break;
		case BLT_BALLLIGHTNING:
			break;
		case BLT_MINDWIPE:
			curSpr=bulletSpr->GetSprite(SPR_BOOM+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_REFLECT:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_YELBOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SWAP:
			int x,y;

			curSpr=bulletSpr->GetSprite(SPR_GREEN);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			for(v=0;v<4;v++)
			{
				x=me->x+Cosine((me->anim+v*64)&255)*6;
				y=me->y+Sine((me->anim+v*64)&255)*6;
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			}
			break;
		case BLT_SLIME:
			curSpr=bulletSpr->GetSprite(SPR_SLIME+(me->anim/3));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-4,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_BEAM:
			curSpr=bulletSpr->GetSprite(SPR_BEAM+1+me->facing%4);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_BEAM2:
			curSpr=bulletSpr->GetSprite(SPR_BEAM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_REDNADE:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_REDNADE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_REDBOOM:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_REDBOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_YELWAVE:
			curSpr=bulletSpr->GetSprite(SPR_WAVE+(me->facing));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_PNKWAVE:
			curSpr=bulletSpr->GetSprite(SPR_WAVE+(me->facing));
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,5,6,me->bright+8,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
	}
}

void UpdateBullets(Map *map,world_t *world)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(bullet[i].type)
			UpdateBullet(&bullet[i],map,world);
}

void RenderBullets(void)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(bullet[i].type)
			RenderBullet(&bullet[i]);
}

void FireMe(bullet_t *me,int x,int y,byte facing,byte type,byte friendly)
{
	int f;

	me->friendly=friendly;
	me->type=type;
	me->x=x;
	me->y=y;
	me->facing=facing;
	me->bright=0;
	me->ax=0;
	me->ay=0;

	switch(me->type)
	{
		case BLT_HOLESHOT:
			me->z=FIXAMT*20;
			me->dx=Cosine(facing)*8;
			me->dy=Sine(facing)*8;
			me->dz=0;
			me->anim=0;
			me->timer=60;
			break;
		case BLT_SCANSHOT:
			me->facing=Random(256);
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->dz=0;
			me->anim=0;
			me->timer=60;
			break;
		case BLT_SCANNER:
			me->dx=Cosine(facing*32)*16;
			me->dy=Sine(facing*32)*16;
			me->dz=0;
			me->anim=0;
			me->timer=60;
			me->facing=facing*32;
			break;
		case BLT_SWAP:
			me->dx=Cosine(facing*32)*10;
			me->dy=Sine(facing*32)*10;
			me->dz=0;
			me->anim=0;
			me->timer=120;
			me->facing=facing*32;
			break;
		case BLT_REFLECT:
			me->dx=0;
			me->dy=0;
			me->z=0;
			me->anim=0;
			me->timer=9;
			break;
		case BLT_BOOM:
			me->dx=0;
			me->dy=0;
			me->z=0;
			me->anim=0;
			me->timer=7;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1800);
			break;
		case BLT_BALLLIGHTNING:
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*11;
			me->dy=Sine(me->facing*32)*11;
			me->dz=0;
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=FIXAMT*10;
			break;
		case BLT_WAND:
		case BLT_WAND2:
		case BLT_WITCH:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=0;
			break;
		case BLT_LUNA:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=0;
			break;
		case BLT_LUNAX1:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*16;
			me->dy=Sine(me->facing*32)*16;
			me->dz=0;
			break;
		case BLT_LUNAX2:
		case BLT_CHEESEHAMMER:
			me->anim=0;
			me->timer=50;
			me->z=FIXAMT*15;
			me->dx=Cosine(me->facing*32)*14;
			me->dy=Sine(me->facing*32)*14;
			me->dz=0;
			break;
		case BLT_EVILHAMMER:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=FIXAMT*10;
			break;
		case BLT_BOMB_HOMING:
			me->anim=0;
			me->timer=90;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*6;
			me->dy=Sine(me->facing)*6;
			me->dz=FIXAMT*6;
			break;
		case BLT_BOMB:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=FIXAMT*5;
			break;
		case BLT_ROCKET:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=0;
			break;
		case BLT_BOOMERANG:
			me->anim=0;
			me->timer=180;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*14;
			me->dy=Sine(me->facing*32)*14;
			me->dz=0;
			break;
		case BLT_FART:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->dz=0;
			break;
		case BLT_WIND:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=0;
			me->facing=facing*2;
			me->target=65535;
			f=me->facing;
			f+=Random(5)-2;
			if(f<0)
				f+=16;
			me->facing=(byte)(f&15);
			me->x+=((Random(17)-8)<<FIXSHIFT);
			me->y+=((Random(17)-8)<<FIXSHIFT);
			me->dx=Cosine(me->facing*16)*4;
			me->dy=Sine(me->facing*16)*4;
			break;
			break;
		case BLT_MISSILE:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=0;
			me->facing=facing*2;
			me->target=65535;
			f=me->facing;
			f+=Random(5)-2;
			if(f<0)
				f+=16;
			me->facing=(byte)(f&15);
			me->x+=((Random(17)-8)<<FIXSHIFT);
			me->y+=((Random(17)-8)<<FIXSHIFT);
			me->dx=Cosine(me->facing*16)*4;
			me->dy=Sine(me->facing*16)*4;
			MakeSound(SND_MISSILELAUNCH,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_SKULL:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=0;
			me->facing=facing*2;
			me->target=65535;
			f=me->facing;
			f+=Random(5)-2;
			if(f<0)
				f+=16;
			me->facing=(byte)(f&15);
			me->x+=((Random(17)-8)<<FIXSHIFT);
			me->y+=((Random(17)-8)<<FIXSHIFT);
			me->dx=Cosine(me->facing*16)*4;
			me->dy=Sine(me->facing*16)*4;
			break;
		case BLT_TORPEDO:
			me->anim=Random(8);
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=-6+Random(13);
			f=facing*32*16;
			f+=Random(33)-16;
			if(f<0)
				f+=256*16;
			me->target=f%(256*16);
			me->facing=me->target/256;
			me->x+=((Random(17)-8)<<FIXSHIFT);
			me->y+=((Random(17)-8)<<FIXSHIFT);
			me->dx=0;
			me->dy=0;
			break;
		case BLT_ACID:
		case BLT_SHARK:
		case BLT_SHARKGOOD:
		case BLT_FREEZE2:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*10;
			me->dz=FIXAMT*4;
			me->dx=Cosine(me->facing)*10;
			me->dy=Sine(me->facing)*10;
			me->facing=((me->facing+16)&255)/32;
			break;
		case BLT_FREEZE:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*10;
			me->dz=0;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->facing=((me->facing+16)&255)/32;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			me->anim=0;
			me->timer=24-Random(4);
			me->z=FIXAMT*20;
			me->x+=((Random(3)-1)<<FIXSHIFT)+Cosine(me->facing*32)*5;
			me->y+=((Random(3)-1)<<FIXSHIFT)+Sine(me->facing*32)*5;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			me->dz=-FIXAMT/2;
			if(Random(5)==0)
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_FLAMEWALL:
			me->anim=0;
			me->timer=12;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			if(Random(5)==0)
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_FLAME5:
			me->anim=0;
			me->timer=24-Random(4);
			me->z=FIXAMT*20;
			me->x+=((Random(3)-1)<<FIXSHIFT)+Cosine(me->facing*32)*5;
			me->y+=((Random(3)-1)<<FIXSHIFT)+Sine(me->facing*32)*5;
			me->dx=Cosine(me->facing)*10;
			me->dy=Sine(me->facing)*10;
			me->dz=-FIXAMT/2;
			if(Random(5)==0)
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_FLAME3:
			me->anim=4;
			me->timer=24-Random(4);
			me->z=FIXAMT*20;
			me->facing=Random(256);
			me->x+=((Random(3)-1)<<FIXSHIFT)+Cosine(me->facing)*5;
			me->y+=((Random(3)-1)<<FIXSHIFT)+Sine(me->facing)*5;
			me->dx=Cosine(me->facing);
			me->dy=Sine(me->facing);
			me->dz=FIXAMT/2;
			break;
		case BLT_SITFLAME:
		case BLT_BADSITFLAME:
		case BLT_FLAME4:
			me->anim=0;
			me->timer=30+Random(15*15);
			me->z=FIXAMT*20;
			me->facing=Random(256);
			me->x+=((Random(3)-1)<<FIXSHIFT)+Cosine(me->facing)*5;
			me->y+=((Random(3)-1)<<FIXSHIFT)+Sine(me->facing)*5;
			f=Random(FIXAMT*6)+FIXAMT;
			me->dx=Cosine(me->facing)*f/FIXAMT;
			me->dy=Sine(me->facing)*f/FIXAMT;
			me->dz=Random(FIXAMT*4)+FIXAMT;
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_HOTPANTS:
			me->anim=0;
			me->timer=36;
			me->z=0;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_LASER:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20-Random(FIXAMT);
			me->x+=((Random(3)-1)<<FIXSHIFT);
			me->y+=((Random(3)-1)<<FIXSHIFT);
			me->x+=FIXAMT/2-Random(FIXAMT);
			me->y+=FIXAMT/2-Random(FIXAMT);
			me->facing=me->facing*32+4-Random(9);
			me->dx=Cosine(me->facing)*24;
			me->dy=Sine(me->facing)*24;
			me->facing/=16;
			me->dz=0;
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1050);
			break;
		case BLT_LASER2:
			me->anim=0;
			me->timer=30*10;
			me->facing=me->facing*32;
			me->dx=Cosine(me->facing)*32;
			me->dy=Sine(me->facing)*24;
			me->x+=Cosine(me->facing)*(Random(10));
			me->y+=Sine(me->facing)*(Random(8));
			me->dz=0;
			me->target=255+255*256;
			break;
		case BLT_ENERGY:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			// this thing receives full 0-255 facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			//MakeSound(SND_ENERGYFIRE,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_ENERGY_BOUNCE:
		case BLT_ENERGY_CIRCLS1:
		case BLT_ENERGY_CIRCLS2:
		case BLT_ENERGY_CIRCLF1:
		case BLT_ENERGY_CIRCLF2:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			// this thing receives full 0-255 facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			//MakeSound(SND_ENERGYFIRE,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_ENERGY_WAVE:
			me->anim=0;
			me->timer=120;
			me->z=FIXAMT*20;
			// this thing receives full 0-255 facings
			me->dx=Cosine(me->facing)*6;
			me->dy=Sine(me->facing)*6;
			me->dz=0;
			if(me->facing%64!=32){
				if(me->facing%128==0)
					me->ay=me->dx*1.25;
				else
					me->ax=me->dy*1.25;
			}
			break;
		case BLT_MEGABEAM:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*32;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			MakeSound(SND_MEGABEAM,me->x,me->y,SND_CUTOFF,5000);
			break;
		case BLT_MEGABEAM1:
			me->anim=0;
			me->timer=40;
			me->z=FIXAMT*32;
			me->dx=0;
			me->dy=32*FIXAMT;	// always 32 pixels to make a continuous not-too-overlapping beam
			me->dz=0;
			break;
		case BLT_SPORE:
		case BLT_SPOREGOOD:
			me->anim=0;
			me->timer=16;
			me->z=FIXAMT*16;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->dz=0;
			break;
		case BLT_SHROOM:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*32;
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->dz=0;
			break;
		case BLT_PUMPKIN:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*32;
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->dz=FIXAMT*10;
			break;
		case BLT_EGG:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*32;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=FIXAMT*10;
			break;
		case BLT_PIE:
			me->anim=0;
			me->timer=100;
			me->z=FIXAMT*32;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=FIXAMT*10;
			break;
		case BLT_GRENADE:
			me->anim=0;
			me->timer=255;
			me->z=FIXAMT*80;
			f=Random(12)+1;
			me->dx=Cosine(me->facing)*f;
			me->dy=Sine(me->facing)*f;
			me->dz=FIXAMT*20;
			break;
		case BLT_BIGYELLOW:
			me->anim=0;
			me->timer=30*4;
			me->z=FIXAMT*30;
			me->dz=0;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			break;
		case BLT_SHOCKWAVE:
		case BLT_SHOCKWAVE2:
			me->anim=0;
			me->timer=7;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_SNOWBALL:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			// this thing receives full 0-255 facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=FIXAMT*4;
			//MakeSound(SND_ENERGYFIRE,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_BIGSNOW:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			// this thing receives full 0-255 facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=FIXAMT*4;
			//MakeSound(SND_ENERGYFIRE,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_BIGSNOW2:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			// this thing receives full 0-255 facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=FIXAMT*4;
			//MakeSound(SND_ENERGYFIRE,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_ICESPIKE:
		case BLT_DIRTSPIKE:
			me->anim=0;
			me->timer=9;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_ROCK:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			me->dz=FIXAMT*8;
			break;
		case BLT_ROCKBIG:
			me->anim=0;
			me->timer=100;
			me->z=FIXAMT*24;
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			me->dz=FIXAMT*10;
			break;
		case BLT_SPINE:
		case BLT_ICESHARD:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			// this takes 0-255 directional facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->facing/=16;
			me->dz=0;
			break;
		case BLT_CLAWS:
			me->anim=0;
			me->timer=30*5;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->dz=0;
			break;
		case BLT_BIGSHELL:
			me->timer=30;
			me->z=FIXAMT*50;
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->dz=-FIXAMT*3;
			break;
		case BLT_BIGAXE:
			me->anim=0;
			me->timer=90;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*11;
			me->dy=Sine(me->facing*32)*11;
			me->dz=0;
			break;
		case BLT_LIGHTNING:
		case BLT_LIGHTNING2:
			me->timer=2;
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*9;
			me->dy=Sine(me->facing*32)*9;
			me->dz=FIXAMT*6;
			break;
		case BLT_HARPOON:
			me->timer=90;
			me->z=FIXAMT*10;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=FIXAMT;
			break;
		case BLT_SLASH:
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->timer=10;
			me->anim=0;
			me->z=FIXAMT*20;
			break;
		case BLT_MINE:
			me->anim=0;
			me->facing=2;
			me->timer=50;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			if(curMap->flags&MAP_UNDERWATER)
				me->z=FIXAMT*10;
			break;
		case BLT_BADGREEN:
		case BLT_GREEN:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			break;
		case BLT_ORBITER:
		case BLT_ORBITER2:
		case BLT_ORBITER3:
		case BLT_ORBITER4:
			me->anim=15;
			me->timer=40;
			if(me->type==BLT_ORBITER2)
				me->timer=10;
			if(me->type==BLT_ORBITER3)
				me->timer=20;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*8+Sine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*8+Cosine(me->facing*32)*4;
			me->dz=0;
			me->target=65535;
			break;
		case BLT_MINDWIPE:
			me->anim=0;
			me->timer=40;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			me->dz=0;
			break;
		case BLT_PAPER:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*9;
			me->dy=Sine(me->facing)*9;
			me->dz=0;
			break;
		case BLT_BUBBLE:
			me->anim=0;
			me->timer=40+Random(30);
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*5;
			me->dy=Sine(me->facing)*5;
			me->dz=FIXAMT*4;
			break;
		case BLT_EVILFACE:
			me->anim=0;
			me->dx=-FIXAMT+Random(FIXAMT*2);
			me->dy=-FIXAMT-Random(FIXAMT*2);
			me->dz=0;
			me->z=40*FIXAMT;
			me->timer=30*10;
			break;
		case BLT_LILBOOM:
		case BLT_LILBOOM2:
			// Fix the little booms to show proper sprites when fired manually
			me->timer=18;
			break;
		case BLT_MEGABOOM:
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			me->anim=0;
			me->timer=10;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_BUBBLEPOP:
			// Likewise with bubble pops
			me->timer=10;
			break;
		case BLT_SLIME:
			me->anim=0;
			me->timer=12*16;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_BEAM:
			me->anim=0;
			me->timer=LASER_TIME+LASER_DELAY;
			me->z=FIXAMT*30;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_BEAM2:
			me->anim=0;
			me->timer=8;
			me->z=FIXAMT*40;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_YELWAVE:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*2;
			me->dz=0;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			break;
		case BLT_PNKWAVE:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*2;
			me->dz=0;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			break;
		case BLT_REDNADE:
			me->anim=0;
			me->timer=255;
			me->z=FIXAMT*80;
			f=MGL_random(12)+1;
			me->dx=Cosine(me->facing)*f;
			me->dy=Sine(me->facing)*f;
			me->dz=FIXAMT*20;
			break;
	}
}

void QuadMissile(Guy *shoot, int x,int y,byte facing,byte friendly)
{
	int i;
	byte f;
	bullet_t *me;

	f=(facing*2-2)&15;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			me=&bullet[i];
			me->type=BLT_MISSILE;
			me->shoot=shoot;
			me->friendly=friendly;
			me->x=x;
			me->y=y;
			me->facing=f;
			me->bright=0;
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=0;
			me->target=65535;
			me->x+=Cosine(me->facing*16)*10;
			me->y+=Sine(me->facing*16)*10;
			me->dx=Cosine(me->facing*16)*4;
			me->dy=Sine(me->facing*16)*4;
			f=(f+1)&15;
			if(f==facing*2)
				f=(f+1)&15;
			if(f==((facing*2+3)&15))
				break;
		}
	MakeSound(SND_MISSILELAUNCH,x,y,SND_CUTOFF,1100);
}


void LaunchMegabeam(Guy *me,int x,int y,word owner)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,0,BLT_MEGABEAM,GetGuy(owner)->friendly);
			bullet[i].target=owner;
			bullet[i].shoot=me;
			break;
		}
}

void SpitAcid(Guy *me,int x,int y,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			bullet[i].friendly=friendly;
			bullet[i].type=type;
			bullet[i].x=x;
			bullet[i].y=y;
			bullet[i].facing=facing;
			bullet[i].bright=0;
			bullet[i].anim=0;
			bullet[i].timer=30;
			bullet[i].z=FIXAMT*30;
			bullet[i].dz=FIXAMT*3;
			bullet[i].dx=Cosine(bullet[i].facing)*10;
			bullet[i].dy=Sine(bullet[i].facing)*10;
			bullet[i].facing=((bullet[i].facing+16)&255)/32;
			bullet[i].shoot=me;
			break;
		}
}

void FireBullet(Guy *me,int x,int y,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type,friendly);
			bullet[i].shoot = me;
			break;
		}
}

void FireScanShots(Guy *me,Guy *victim)
{
	int i;
	byte count=0;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],goodguy->x,goodguy->y,(byte)Random(256),BLT_SCANSHOT,goodguy->friendly);
			bullet[i].target=victim->ID;
			bullet[i].shoot = me;
			count++;
			if(count==8)
				break;
		}
}

void FireBulletZ(Guy *me,int x,int y,int z,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type,friendly);
			bullet[i].z=z;
			bullet[i].shoot = me;
			break;
		}
}

// this only fires if there is room in the bullet list PAST a specific point
// this is used for the Megabeam to ensure that all the laser bits stay lined up nicely
void FireBulletAfter(Guy *me,int x,int y,byte facing,byte type,bullet_t *thisone,byte friendly)
{
	int i,j,start;

	start=-1;

	for(j=0;j<config.numBullets;j++)
		if(&bullet[j]==thisone)
		{
			start=j+1;
			break;
		}

	if(start==-1)
		return;

	for(i=start;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type,friendly);
			bullet[i].shoot = me;
			break;
		}
}

void FireExactBullet(Guy* me, int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<config.numBullets;i++)
		if(!bullet[i].type)
		{
			bullet[i].friendly=friendly;
			bullet[i].x=x;
			bullet[i].y=y;
			bullet[i].z=z;
			bullet[i].bright=0;
			bullet[i].dx=dx;
			bullet[i].dy=dy;
			bullet[i].dz=dz;
			bullet[i].anim=anim;
			bullet[i].timer=timer;
			bullet[i].facing=facing;
			bullet[i].type=type;
			bullet[i].target=65535;
			bullet[i].shoot = me;
			break;
		}
}

void HammerLaunch(Guy *me,int x,int y,byte facing,byte count,byte flags)
{
	byte angle,newfacing;
	byte type,spd,face,timer;
	int height,dz,dx,dy,f,target,anim;

	if(player.cheesePower)
	{
		type=BLT_CHEESEHAMMER;
		face=facing*32;
		spd=14;
		height=FIXAMT*15;
		dz=0;
		timer=50;
	}
	else if(flags&HMR_REFLECT)
	{
		type=BLT_HAMMER2;
		face=facing;
		spd=12;
		height=FIXAMT*20;
		dz=FIXAMT*6;
		timer=30;
	}
	else
	{
		type=BLT_HAMMER;
		face=facing;
		spd=12;
		height=FIXAMT*20;
		dz=FIXAMT*6;
		timer=30;
	}
	if(player.playAs==PLAY_LUNACHIK)
	{
		if(type==BLT_HAMMER)
			type=BLT_LUNA;
		else if(type==BLT_HAMMER2)
			type=BLT_LUNA2;
		else if(type==BLT_CHEESEHAMMER)
			type=BLT_LUNAX2;
		MakeSound(SND_LUNASHOOT,x,y,SND_CUTOFF,1200);
	}
	else if(player.playAs==PLAY_MYSTIC)
	{
		if(type==BLT_HAMMER)
			type=BLT_WAND;
		else if(type==BLT_HAMMER2)
			type=BLT_WAND2;
		MakeSound(SND_ZAP,x,y,SND_CUTOFF,1200);
		height=FIXAMT*40;
		dz=FIXAMT*-1;
	}
	else if(player.playAs==PLAY_WIZ)
	{
		if(type==BLT_HAMMER)
			type=BLT_BIGSHELL;
		else if(type==BLT_HAMMER2)
			type=BLT_BIGSHELL;//bigshell bouncy
		else if(type==BLT_CHEESEHAMMER)
			type=BLT_BIGSHELL;//super big shell
		MakeSound(SND_LUNASHOOT,x,y,SND_CUTOFF,1200);
		height=FIXAMT*40;
		dz=FIXAMT*-1;
	}
	else
		MakeSound(SND_HAMMERTOSS,x,y,SND_CUTOFF,1200);
	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		angle=facing*32;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		angle=facing*32-8;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		angle=facing*32+8;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		angle=facing*32-32;
		newfacing=((byte)(facing-1))%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		angle=facing*32+32;
		newfacing=(facing+1)%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		angle=facing*32-56;
		newfacing=((byte)(facing-2))%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		angle=facing*32+56;
		newfacing=(facing+2)%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
	}
	if(flags&HMR_REVERSE)
	{
		newfacing=((byte)(facing-4))%8;
		HammerLaunch(me,x,y,newfacing,count,flags&(~HMR_REVERSE));
	}
}

void WolfSpew(Guy *me,int x,int y,byte facing,byte count,byte flags)
{
	byte angle,newfacing;
	byte type,spd,face,timer;
	int height,dz,a;

	//haha its time
	MakeSound(SND_SLASH,x,y,SND_CUTOFF,1200);
	
	if(player.cheesePower)
	{
		type=BLT_BIGSHELL;
		face=facing*32;
		spd=16;
		height=FIXAMT*15;
		dz=0;
		timer=20;
	}
	else if(flags&HMR_REFLECT)
	{
		type=BLT_ENERGY_BOUNCE;
		face=facing;
		spd=12;
		height=FIXAMT*20;
		dz=FIXAMT*0;
		timer=30;
	}
	else
	{
		type=BLT_ENERGY;
		face=facing;
		spd=12;
		height=FIXAMT*20;
		dz=FIXAMT*0;
		timer=30;
	}
	
	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		angle=facing*32;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		angle=facing*32-8;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
		angle=facing*32+8;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		angle=facing*32-32;
		newfacing=((byte)(facing-1))%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
		angle=facing*32+32;
		newfacing=(facing+1)%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		angle=facing*32-56;
		newfacing=((byte)(facing-2))%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
		angle=facing*32+56;
		newfacing=(facing+2)%8;
		if(player.cheesePower)
			face=newfacing*32;
		else
			face=newfacing;
		FireExactBullet(me,x,y,height,Cosine(angle)*spd,Sine(angle)*spd,dz,0,timer,face,type,1);
		FireBullet(me,x+Cosine(angle)*32, y+Sine(angle)*32, facing,BLT_SLASH,1);
	}
	if(flags&HMR_REVERSE)
	{
		newfacing=((byte)(facing-4))%8;
		WolfSpew(me,x,y,newfacing,count,flags&(~HMR_REVERSE));
	}
}

void HappyFire(Guy *me,int x,int y,byte facing)
{
	byte happyList[]={BLT_HAMMER,BLT_HAMMER2,BLT_MISSILE,BLT_FLAME,BLT_LASER,BLT_BOMB,
					  BLT_BALLLIGHTNING,BLT_BIGSHELL,BLT_BIGAXE,BLT_LIGHTNING,BLT_SPEAR,BLT_SLASH,
					  BLT_GREEN,BLT_TORPEDO,BLT_BUBBLE,BLT_LUNA};
	byte b;

	b=happyList[Random(16)];
	if(b==BLT_BOMB || b==BLT_GREEN || b==BLT_BIGSHELL || b==BLT_BUBBLE)
		facing*=32;
	FireBullet(me,x,y,facing,b,1);
}

void HappyLaunch(Guy *me,int x,int y,byte facing,byte count,byte flags)
{
	MakeSound(SND_HAMMERTOSS,x,y,SND_CUTOFF,1200);
	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		HappyFire(me,x,y,facing);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		HappyFire(me,x,y,facing);
		if(Random(2)==0)
			HappyFire(me,x,y,(facing-1)&7);
		else
			HappyFire(me,x,y,(facing+1)&7);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		HappyFire(me,x,y,(facing-1)&7);
		HappyFire(me,x,y,(facing+1)&7);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		HappyFire(me,x,y,(facing-2)&7);
		HappyFire(me,x,y,(facing+2)&7);
	}
	if(flags&HMR_REVERSE)
	{
		HappyLaunch(me,x,y,(facing+4)&7,count,(flags&(~HMR_REVERSE)));
	}
}

void SkullFire(Guy *me,int x,int y,byte facing)
{
	byte b;
	b=BLT_SKULL;
	FireBullet(me,x,y,facing,b,1);
}

void SkullLaunch(Guy *me,int x,int y,byte facing,byte count,byte flags)
{
	MakeSound(SND_HAMMERTOSS,x,y,SND_CUTOFF,1200);
	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		SkullFire(me,x,y,facing);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		SkullFire(me,x,y,facing);
		if(Random(2)==0)
			SkullFire(me,x,y,(facing-1)&7);
		else
			SkullFire(me,x,y,(facing+1)&7);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		SkullFire(me,x,y,(facing-1)&7);
		SkullFire(me,x,y,(facing+1)&7);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		SkullFire(me,x,y,(facing-2)&7);
		SkullFire(me,x,y,(facing+2)&7);
	}
	if(flags&HMR_REVERSE)
	{
		SkullLaunch(me,x,y,(facing+4)&7,count,(flags&(~HMR_REVERSE)));
	}
}

void ShroomFire(Guy *me,int x,int y,byte facing)
{
	byte a;
	int i;

	for(i=0;i<3;i++)
	{
		a=(facing*32+16-Random(33))&255;
		FireExactBullet(me,x,y,FIXAMT*16,Cosine(a)*6,Sine(a)*6,0,0,20,a,BLT_SPORE,1);
	}
}

void ShroomSpew(Guy *me,int x,int y,byte facing,byte count,byte flags)
{
	MakeSound(SND_MUSHSPORES,x,y,SND_CUTOFF,1200);

	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		ShroomFire(me,x,y,facing);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		ShroomFire(me,x,y,facing);
		if(Random(2)==0)
			ShroomFire(me,x,y,(facing-1)&7);
		else
			ShroomFire(me,x,y,(facing+1)&7);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		ShroomFire(me,x,y,(facing-1)&7);
		ShroomFire(me,x,y,(facing+1)&7);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		ShroomFire(me,x,y,(facing-2)&7);
		ShroomFire(me,x,y,(facing+2)&7);
	}
	if(flags&HMR_REVERSE)
	{
		ShroomSpew(me,x,y,(facing+4)&7,count,(flags&(~HMR_REVERSE)));
	}
}

void ReflectShot(void)
{
	reflect=1;
}

void BulletSwap(int sx,int sy,int width,int height,int dx,int dy)
{
	int i;

	sx*=(TILE_WIDTH*FIXAMT);
	sy*=(TILE_HEIGHT*FIXAMT);
	dx*=(TILE_WIDTH*FIXAMT);
	dy*=(TILE_HEIGHT*FIXAMT);
	width*=(TILE_WIDTH*FIXAMT);
	height*=(TILE_HEIGHT*FIXAMT);

	for(i=0;i<config.numBullets;i++)
	{
		if(bullet[i].type)
		{
			if(bullet[i].x>=sx && bullet[i].y>=sy && bullet[i].x<=(sx+width) && bullet[i].y<=(sy+height))
			{
				// in target area, swap
				bullet[i].x+=(-sx+dx);
				bullet[i].y+=(-sy+dy);
			}
			else if(bullet[i].x>=dx && bullet[i].y>=dy && bullet[i].x<=(dx+width) && bullet[i].y<=(dy+height))
			{
				// in other target area, swap
				bullet[i].x+=(-dx+sx);
				bullet[i].y+=(-dy+sy);
			}
		}
	}
}

void ReflectBullets(Guy *me,int x,int y,byte size,byte friendly)
{
	int i;
	int rx,ry,rx2,ry2;

	rx=x-size*FIXAMT;
	rx2=x+size*FIXAMT;
	ry=y-size*FIXAMT;
	ry2=y+size*FIXAMT;

	for(i=0;i<config.numBullets;i++)
	{
		if(bullet[i].type && bullet[i].friendly!=friendly && bullet[i].x>rx &&
			bullet[i].y>ry && bullet[i].x<rx2 && bullet[i].y<ry2)
		{
			bullet[i].friendly=friendly;
			bullet[i].dx=-bullet[i].dx;
			bullet[i].dy=-bullet[i].dy;
			bullet[i].shoot = me;
			if(bullet[i].type==BLT_ICESPIKE || bullet[i].type==BLT_DIRTSPIKE)
				bullet[i].facing=(bullet[i].facing+4)&7;
		}
	}
}

void RemoveOrbiters(int n,byte f,byte t)
{
	int i;

	if(n==0)
		return;

	for(i=0;i<config.numBullets;i++)
	{
		if(bullet[i].type==BLT_ORBITER && bullet[i].friendly==f && bullet[i].type==t)
		{
			MakeSound(SND_BOMBBOOM,bullet[i].x,bullet[i].y,SND_CUTOFF,2000);
			bullet[i].dx=0;
			bullet[i].dy=0;
			bullet[i].dz=0;
			bullet[i].type=BLT_BOOM;
			bullet[i].timer=7;
			bullet[i].anim=0;
			n--;
			if(n==0)
				return;
		}
	}

}

void MakeRadar(int rx,int ry,byte w)
{
	if(rx==255)
		return;

	rx=(rx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	ry=(ry*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;

	TrackParticle(w,goodguy->x,goodguy->y,rx,ry);
/*	int camX,camY;

	if(rx==255)
		return;

	GetCamera(&camX,&camY);
	rx=rx*TILE_WIDTH-(camX-320);
	ry=ry*TILE_HEIGHT-(camY-240);

	if(rx<TILE_WIDTH/2)
		rx=TILE_WIDTH/2;

	if(ry<TILE_HEIGHT/2)
		ry=TILE_HEIGHT/2;

	if(rx>640-TILE_WIDTH/2)
		rx=640-TILE_WIDTH/2;

	if(ry>480-TILE_HEIGHT/2)
		ry=480-TILE_HEIGHT/2;

	rx+=(camX-320)+TILE_WIDTH/2;
	ry+=(camY-240)+TILE_HEIGHT/2;
	FireExactBullet(rx*FIXAMT,ry*FIXAMT,FIXAMT,0,0,0,0,20,0,BLT_SCANLOCK,0);
	*/
}

int CountBullets(byte type)
{
	int count = 0;

	for(int i=0;i<config.numBullets;i++)
		if(bullet[i].type == type)
			++count;

	return count;
}

int CountBulletsInRect(byte type,int x,int y,int x2,int y2)
{
	int count = 0;

	for(int i=0;i<config.numBullets;i++) {
		int tx = bullet[i].x / TILE_WIDTH / FIXAMT;
		int ty = bullet[i].y / TILE_HEIGHT / FIXAMT;
		if(bullet[i].type==type && tx>=x && ty>=y && tx<=x2 && ty<=y2)
			++count;
	}

	return count;
}

void ChangeBullet(byte fx,int x,int y,int type,int newtype)
{
	for(int i=0;i<config.numBullets;i++)
		if(bullet[i].type != newtype && ((type != 0 && bullet[i].type == type) || (type == 0 && bullet[i].type != 0)))
			if (x == 255 || ((bullet[i].x >> FIXSHIFT)/TILE_WIDTH == x && (bullet[i].y >> FIXSHIFT)/TILE_HEIGHT == y))
			{
				int dx = bullet[i].dx, dy = bullet[i].dy, f = bullet[i].facing;

				if (type == BLT_LASER)
					f /= 2;

				if (BulletFacingType(newtype) == 0)
					f = 0;
				else if (BulletFacingType(newtype) == 7 && BulletFacingType(type) == 255)
					f /= 32;
				else if (BulletFacingType(newtype) == 255 && BulletFacingType(type) == 7)
					f *= 32;

				FireMe(&bullet[i],bullet[i].x,bullet[i].y,f,newtype,bullet[i].friendly);
				bullet[i].dx = dx;
				bullet[i].dy = dy;
				if (fx)
					BlowSmoke(bullet[i].x, bullet[i].y, 0, FIXAMT/8);
			}

}

// TORPEDO, LASER
static const byte bulletFacingType[] = {
	0,  	// BLT_NONE    0
	7,  	// BLT_HAMMER  1
	7,  	// BLT_HAMMER2 2	// this is a hammer with reflection
	7,  	// BLT_MISSILE 3
	7,  	// BLT_FLAME   4
	7,  	// BLT_LASER	5
	255,  	// BLT_ACID	6
	255,  	// BLT_BOMB	7
	0,  	// BLT_BOOM	8
	255,  	// BLT_ENERGY  9
	0,  	// BLT_MEGABEAM 10		// this is a ball of energy that launches off a megabeam1
	0,  	// BLT_MEGABEAM1 11	// this is a huge laser beam (downward)
	0,  	// BLT_MEGABEAM2 12	// this is the laser hitting an object (just a visual effect)
	7,  	// BLT_FLAME2	13	// just like flame, except it is anti-Bouapha
	255,  	// BLT_SPORE	14
	7,  	// BLT_SHROOM  15
	255,  	// BLT_GRENADE 16	// energy grenade, an enemy weapon
	0,  	// BLT_YELBOOM 17	// yellow explosion made by energy grenade
	0,  	// BLT_SHOCKWAVE 18	// purple shockwave, for Super Zombie stomp
	0,  	// BLT_LILBOOM 19	// explosion made by missile
	255,  	// BLT_SNOWBALL 20
	255,  	// BLT_BIGSNOW  21
	7,  	// BLT_ICESPIKE 22	// spike juts out of the ground
	7,  	// BLT_ROCK	23
	255,  	// BLT_SPINE	24 // cactus spine
	7,  	// BLT_EVILHAMMER 25 // a grey hammer that is anti-bouapha
	255,  	// BLT_BIGSHELL 26	// Bouapha's power armor shoots these
	7,  	// BLT_BIGAXE	27	// Bouapha weapon
	0,  	// BLT_LIGHTNING 28 // Bouapha weapon
	7,  	// BLT_SPEAR	29	// Bouapha's version of the pygmy spear
	0,  	// BLT_SLASH	30	// Bouapha's machete slash
	0,  	// BLT_MINE	31	// Bouapha's mines
	7,  	// BLT_BADSPEAR 32	// pygmy-thrown spear
	7,  	// BLT_ORBITER  33	// guy that flies around Bouapha shooting
	255,  	// BLT_GREEN	34	// friendly green bullets
	7,  	// BLT_BALLLIGHTNING 35
	0,  	// BLT_LIGHTNING2 36
	7,  	// BLT_MINDWIPE	37
	0,  	// BLT_REFLECT	38
	7,  	// BLT_SWAP	39
	255,  	// BLT_SHARK	40
	7,  	// BLT_ORBITER2 41	// orbit bomber
	7,  	// BLT_HARPOON	42	// a spear, underwater - only difference is it sinks much more slowly
	7,  	// BLT_SCANNER	43	// what the scanner scans with
	255,  	// BLT_SCANSHOT 44	// the shots the scanner fires after scanning
	7,  	// BLT_TORPEDO	45	// Bouapha's minisub shoots them
	7,  	// BLT_DIRTSPIKE 46	// ice spike in brown
	255,  	// BLT_PAPER	47
	0,  	// BLT_SCANLOCK 48	// the scanner locked onto a guy
	255,  	// BLT_BUBBLE	49
	255,  	// BLT_FREEZE	50
	0,  	// BLT_BUBBLEPOP 51	// a bubble that is popping, just visual effect
	0,  	// BLT_LILBOOM2 52		// a harmless lilboom
	7,  	// BLT_CHEESEHAMMER 53	// enhanced hammers
	255,  	// BLT_FREEZE2	54		// a freeze bullet that drops like acid bullets and splats
	7,  	// BLT_LUNA	55		// lunachick's bullets
	7,  	// BLT_LUNA2	56		// lunachick's bullets with wall-bounce power
	7,  	// BLT_FLAME3	57
	7,  	// BLT_SITFLAME	58
	7,  	// BLT_BADSITFLAME	59
	7,  	// BLT_LASER2	60
	255,  	// BLT_BADGREEN
	7,  	// BLT_ORBITER3
	255,  	// BLT_GOODSHARK
	7,  	// orbiter 4
	7,  	// BLT_WIND		65
	0,  	// BLT_EVILFACE
	7,  	// evil lunachick fire 1
	7,  	// evil lunachick fire 2
	7,		// wave
	7,		// beam			70
	0,		// beam when it hits something
	0,		// slime... from the snail
	255,	// red grenade
	0,		// red grenade boom
	255,  	// big yellow	75
	0,  	// megaboom for julie
	7,  	// bouncy energy bois
	7,  	// rocket
	255,	// skull
	7,		// wand		80
	255,	// boomerang
	7,		// fart
	7,		// pumpkin
	7,		// flame4
	255,	// flame5 - flames in 255 directions!
	7,		// wand 2
	255,	// black hole shot
	0,		// black hole
	255,	// egg
	255,	// pie
	255,	// wave energy
	255,	// CW energy
	255,	// CCW energy
	255,	// CW energy 2
	255,	// CCW energy 2
	255,	// homing red bomb
	255,	// flame wall
	255,	// claws
	255,	// ice shard
	255,	// hot pants
	7,		// witch
	7,		// pink wave
	255,  	// good spore
	255,  	// rock big
	0,  	// shockwave 2
	255,  	// snowball 2
	0,
	7,
};

byte BulletFacingType(byte type)
{
	return bulletFacingType[type];
}

byte GetBulletAttackType(void)
{
	return attackType;
}

void BurnHay(int x,int y)
{
	int i,j,k;

	x/=(TILE_WIDTH*FIXAMT);
	y/=(TILE_HEIGHT*FIXAMT);

	for(i=x-1;i<=x+1;i++)
		for(j=y-1;j<=y+1;j++)
		{
			if(i<0 || j<0 || i>=curMap->width || j>=curMap->height)
				return;

			if(curMap->map[i+j*curMap->width].item==ITM_HAY || curMap->map[i+j*curMap->width].item==85)	// hay or minecart blocks
			{
				//if(curMap->map[i+j*curMap->width].item==85)
				{
					FireBullet(0,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_BADSITFLAME,0);
				}
				curMap->map[i+j*curMap->width].item=0;
				for(k=0;k<4;k++)
					BlowSmoke((i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,5,FIXAMT);
				MakeSound(SND_FLAMEGO,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,500);
			}
		}
}
