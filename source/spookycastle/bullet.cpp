#include "bullet.h"
#include "guy.h"
#include "player.h"

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

bullet_t bullet[MAX_BULLETS];
sprite_set_t *bulletSpr;

void InitBullets(void)
{
	bulletSpr=new sprite_set_t("graphics/bullets.jsp");

	memset(bullet,0,MAX_BULLETS*sizeof(bullet_t));
}

void ExitBullets(void)
{
	delete bulletSpr;
}

byte Bulletable(Map *map,int x,int y)
{
	if(map->map[x+y*map->width].wall ||
		map->map[x+y*map->width].item>=MAX_SHOOTABLE_ITMS)
		return 0;

	return 1;
}

byte BulletCanGo(int xx,int yy,Map *map,byte size)
{
	byte result;
	int mapx,mapy,mapx1,mapx2,mapy1,mapy2;

	xx>>=FIXSHIFT;
	yy>>=FIXSHIFT;

	mapx=xx/TILE_WIDTH;
	mapy=yy/TILE_HEIGHT;
	mapx1=(xx-size)/TILE_WIDTH;
	mapy1=(yy-size)/TILE_HEIGHT;
	mapx2=(xx+size)/TILE_WIDTH;
	mapy2=(yy+size)/TILE_HEIGHT;

	result=(mapx1>=0 && mapy1>=0 && mapx2<map->width && mapy2<map->height &&
		(Bulletable(map,mapx,mapy1)) &&
		(Bulletable(map,mapx,mapy2)) &&
		(Bulletable(map,mapx1,mapy)) &&
		(Bulletable(map,mapx2,mapy)) &&
		(Bulletable(map,mapx,mapy)) &&
		(Bulletable(map,mapx1,mapy1)) &&
		(Bulletable(map,mapx2,mapy1)) &&
		(Bulletable(map,mapx2,mapy2)) &&
		(Bulletable(map,mapx1,mapy2)));

	if(!result)
		SpecialShootCheck(map,mapx,mapy);

	return result;
}

void BulletHitWallX(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_HAMMER:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:	// reflects off walls
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_GRENADE:
			me->x-=me->dx;
			break;
		case BLT_LASER:	// reflects off walls
			MakeSound(SND_BULLETREFLECT,me->x,me->y,SND_CUTOFF,900);
			me->x-=me->dx;
			me->dx=-me->dx;
			me->dy+=-FIXAMT/4+MGL_random(FIXAMT/2);
			me->facing=((byte)(8-me->facing))&15;
			break;
		case BLT_BOMB:
			MakeSound(SND_BOMBREFLECT,me->x,me->y,SND_CUTOFF,600);
			me->x-=me->dx;
			me->dx=-me->dx;
			me->dy+=-FIXAMT/4+MGL_random(FIXAMT/2);
			me->facing=((byte)(8-me->facing))&15;
			break;
		case BLT_MISSILE:
		case BLT_SPINE:
			BulletRanOut(me,map,world);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			me->x-=me->dx;
			me->dy=((3-MGL_random(7))<<FIXSHIFT);
			me->dx=0;
			break;
		case BLT_ROCK:	// reflects off walls
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_ENERGY:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
			me->type=BLT_MEGABEAM2;
			me->dy=0;
			me->timer=2;
			break;
		case BLT_SPORE:
		case BLT_ICESPIKE:
			me->type=BLT_NONE;
			break;
		case BLT_SHROOM:
			// detonate
			BulletRanOut(me,map,world);
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletHitWallY(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_HAMMER:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:	// reflects off walls
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_GRENADE:
			me->y-=me->dy;
			break;
		case BLT_LASER:
			MakeSound(SND_BULLETREFLECT,me->x,me->y,SND_CUTOFF,900);
			me->y-=me->dy;
			me->dy=-me->dy;
			me->dx+=-FIXAMT/4+MGL_random(FIXAMT/2);
			me->facing=(16-me->facing)&15;
			break;
		case BLT_BOMB:
			MakeSound(SND_BOMBREFLECT,me->x,me->y,SND_CUTOFF,600);
			me->y-=me->dy;
			me->dy=-me->dy;
			me->dx+=-FIXAMT/4+MGL_random(FIXAMT/2);
			me->facing=(16-me->facing)&15;
			break;
		case BLT_MISSILE:
		case BLT_SPINE:
			BulletRanOut(me,map,world);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			me->y-=me->dy;
			me->dx=((3-MGL_random(7))<<FIXSHIFT);
			me->dy=0;
			break;
		case BLT_ROCK:	// reflects off walls
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_ENERGY:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
			me->type=BLT_MEGABEAM2;
			me->dy=0;
			me->timer=2;
			break;
		case BLT_SPORE:
		case BLT_ICESPIKE:
			me->type=BLT_NONE;
			break;
		case BLT_SHROOM:
			// detonate
			BulletRanOut(me,map,world);
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletHitFloor(bullet_t *me,Map *map,world_t *world)
{
	int x,y;

	switch(me->type)
	{
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,850);
			me->dz=-me->dz*3/4;
			me->z=0;
			x=(me->x>>FIXSHIFT)/TILE_WIDTH;
			y=(me->y>>FIXSHIFT)/TILE_HEIGHT;
			if(world->terrain[map->map[x+y*map->width].floor].flags&TF_WATER)
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
			else if(world->terrain[map->map[x+y*map->width].floor].flags&TF_LAVA)
				ExplodeParticles(PART_HAMMER,me->x,me->y,0,8);
			break;
		case BLT_BOMB:
			if(me->dz<-FIXAMT)	// don't make it on small bounces, because it'd be annoying
				MakeSound(SND_BOMBREFLECT,me->x,me->y,SND_CUTOFF,600);
			me->dz=-me->dz*3/4;
			me->z=0;
			break;
		case BLT_MISSILE: // this really really should never happen
		case BLT_FLAME:
		case BLT_FLAME2:
		case BLT_LASER:
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
		case BLT_MEGABEAM2:
		case BLT_SPORE:
		case BLT_YELBOOM:
		case BLT_LILBOOM:
		case BLT_SHOCKWAVE:
		case BLT_ICESPIKE:
		case BLT_SPINE:
			me->z=0;
			break;
		case BLT_ROCK:
			if(me->dz<-FIXAMT)
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
			me->dz=-me->dz*3/4;
			me->z=0;
			break;
		case BLT_ENERGY:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHROOM:
			// detonate
			BulletRanOut(me,map,world);
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
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletRanOut(bullet_t *me,Map *map,world_t *world)
{
	int i;

	switch(me->type)
	{
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_LILBOOM:
		case BLT_LASER:
		case BLT_BOOM:
		case BLT_ENERGY:
		case BLT_MEGABEAM:
		case BLT_MEGABEAM1:
		case BLT_MEGABEAM2:
		case BLT_SPORE:
		case BLT_GRENADE:
		case BLT_YELBOOM:
		case BLT_SHOCKWAVE:
		case BLT_ICESPIKE:
		case BLT_ROCK:
		case BLT_SPINE:
		case BLT_EVILHAMMER:
			me->type=0;
			break;
		case BLT_BIGSNOW:
			// poof into snowballs
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			me->type=0;
			for(i=0;i<8;i++)
				FireBullet(me->x,me->y,(byte)i*32,BLT_SNOWBALL);
			break;
		case BLT_SNOWBALL:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,6,12);
			break;
		case BLT_MISSILE:
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1500);
			me->type=BLT_LILBOOM;
			me->timer=9;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_ACID:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			me->type=0;
			BlowSmoke(me->x,me->y,me->z,FIXAMT);
			break;
		case BLT_BOMB:
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
				FireExactBullet(me->x,me->y,me->z,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE);
				FireExactBullet(me->x,me->y,me->z,Cosine(i)*6,Sine(i)*6,0,0,16,i,BLT_SPORE);
			}
			AddGuy(me->x-me->dx*2,me->y-me->dy*2,me->z,MONS_SHROOM);	// become a living shroom
			me->type=BLT_NONE;	// all gone
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1000);
			break;
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int i;

	switch(me->type)
	{
		case BLT_LASER:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,1,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_BULLETHIT,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,5,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_MISSILE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,8,map,world))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_LILBOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,2,map,world))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_FLAME:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world))
			{
				// no noise, just let them scream
			}
			break;
		case BLT_FLAME2:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world))
			{
				// no noise, just let him scream
			}
			break;
		case BLT_SPORE:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/2,me->dy/2,1,map,world))
			{
				PoisonPlayer(30);
				me->type=BLT_NONE;	// go away
			}
			break;
		case BLT_SPINE:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/4,me->dy/4,4,map,world))
			{
				me->type=BLT_NONE;
			}
			break;
		case BLT_SHROOM:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world))
			{
				BulletRanOut(me,map,world);	// detonate, not to mention the 10 damage you already did
			}
			break;
		case BLT_ICESPIKE:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,
				6,map,world))
			{
				// nothing special happens
			}
			break;
		case BLT_BOMB:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,0,map,world))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_BOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,64,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,4,map,world))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_YELBOOM:
			i=20*(5-(me->timer/2));	// size expands as boom expands
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,2,map,world))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_SHOCKWAVE:
			i=30*(3-(me->timer/2))+30;	// size expands as wave expands
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,0,0,4,map,world))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_ACID:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,5,map,world))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,1000);
				ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			}
			break;
		case BLT_MEGABEAM1:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,40,0,12*FIXAMT,5,map,world))
			{
				me->type=BLT_NONE;
				MakeSound(SND_MEGABEAMHIT,me->x,me->y,SND_CUTOFF,1000);
				me->type=BLT_MEGABEAM2;
				me->dy=0;
				me->timer=2;
			}
			break;
		case BLT_ENERGY:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,3,map,world))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,6);
			}
			break;
		case BLT_EVILHAMMER:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world))
			{
				me->type=BLT_NONE;
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,950);
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			}
			break;
		case BLT_ROCK:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,8,map,world))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
				ExplodeParticles2(PART_DIRT,me->x,me->y,me->z,12,6);
			}
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,6,map,world))
			{
				BulletRanOut(me,map,world);
			}
			break;
	}
}

void UpdateBullet(bullet_t *me,Map *map,world_t *world)
{
	int mapx,mapy;
	byte b;

	mapx=(me->x/TILE_WIDTH)>>FIXSHIFT;
	mapy=(me->y/TILE_HEIGHT)>>FIXSHIFT;
	if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height)
		me->bright=map->map[mapx+mapy*map->width].templight;

	b=0;

	me->x+=me->dx;
	if(!BulletCanGo(me->x,me->y,map,8))
		BulletHitWallX(me,map,world);
	else
	{
		me->y+=me->dy;
		if(!BulletCanGo(me->x,me->y,map,8))
			BulletHitWallY(me,map,world);
	}

	me->z+=me->dz;

	if(me->z<0)
		BulletHitFloor(me,map,world);

	// all gravity-affected bullets, get gravitized
	if(me->type==BLT_HAMMER || me->type==BLT_HAMMER2 || me->type==BLT_BOMB || me->type==BLT_GRENADE
		|| me->type==BLT_ROCK || me->type==BLT_EVILHAMMER)
		me->dz-=FIXAMT;

	me->timer--;
	if(!me->timer)
		BulletRanOut(me,map,world);

	// special things like animation
	switch(me->type)
	{
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:
			me->anim++;
			if(me->anim>15)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_SPINE:
			HitBadguys(me,map,world);
			break;
		case BLT_ICESPIKE:
			if(me->timer>4)
				me->anim=(8-me->timer);
			else
				me->anim=me->timer-1;
			if(me->timer==5)
				HitBadguys(me,map,world);
			else if(me->timer==4)
			{
				b=(me->facing*32-32+MGL_random(65))&255;
				mapx=(me->x+Cosine(b)*32);
				mapy=(me->y+Sine(b)*32);
				FireBullet(mapx,mapy,me->facing,BLT_ICESPIKE);
			}
			break;
		case BLT_ROCK:
			me->anim++;
			if(me->anim>3)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_ENERGY:
			me->anim=1-me->anim;	// flip-flop animation
			HitBadguys(me,map,world);
			break;
		case BLT_LILBOOM:
			HitBadguys(me,map,world);
			break;
		case BLT_ACID:
			me->dz-=FIXAMT/2;	// less gravity than normal things
			me->anim++;
			if(me->anim>6)
				me->anim=0;
			// hit goodguy
			HitBadguys(me,map,world);
			break;
		case BLT_SPORE:
			me->anim++;
			if(me->anim>3*4+3)
				me->anim=3*4+3;
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			HitBadguys(me,map,world);
			break;
		case BLT_SNOWBALL:
			me->dz-=FIXAMT/2;	// less gravity than normal things
			HitBadguys(me,map,world);
			break;
		case BLT_BIGSNOW:
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
		case BLT_SHOCKWAVE:
			me->anim++;
			HitBadguys(me,map,world);
			break;
		case BLT_YELBOOM:
			me->anim++;
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			HitBadguys(me,map,world);
			break;
		case BLT_SHROOM:
			HitBadguys(me,map,world);
			break;
		case BLT_MEGABEAM:
			if(me->anim<(4*5))
				me->anim++;
			else	// FIRE!
				FireBulletAfter(me->x,me->y,0,BLT_MEGABEAM1,me);

			// keep this beam locked onto its launcher
			if(GetGuyPos(me->target,&me->x,&me->y))
				me->y+=FIXAMT*8;

			break;
		case BLT_MEGABEAM1:
			HitBadguys(me,map,world);
			break;
		case BLT_MEGABEAM2:
			// nothin
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			if(me->timer&1)	// every other frame
				HitBadguys(me,map,world);
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
			me->dz+=MGL_random(FIXAMT/8);		//anti gravity
			me->dx+=MGL_random(65535)-FIXAMT/2;
			me->dy+=MGL_random(65535)-FIXAMT/2;
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			Clamp(&me->dx,FIXAMT*10);
			Clamp(&me->dy,FIXAMT*10);
			me->anim=((32-me->timer)/8)+1;
			if(me->anim>4)
				me->anim=4;
			break;
		case BLT_LASER:
			HitBadguys(me,map,world);
			break;
		case BLT_BOMB:
			me->anim++;
			if(me->anim>15)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_BOOM:
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			HitBadguys(me,map,world);
			break;
		case BLT_MISSILE:
			if(me->timer==40)
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			me->anim++;
			if(((me->anim>0) && (me->target!=65535)) ||
				((me->anim>2) && (me->target==65535)))
			{
				BlowSmoke(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
				me->anim=0;
			}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);

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
	}
}

void RenderSmoke(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_SMOKE+frm),DISPLAY_DRAWME|DISPLAY_GHOST);
}

void RenderBoom(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_BOOM+frm),DISPLAY_DRAWME|DISPLAY_GLOW);
}

void RenderBullet(bullet_t *me)
{
	int v;
	sprite_t *curSpr;

	switch(me->type)
	{
		case BLT_HAMMER:
		case BLT_HAMMER2:
			v=me->facing*16+(me->anim)+SPR_HAMMER;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
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
		case BLT_MISSILE:
			v=me->facing+SPR_MISSILE;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_FLAME);
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
		case BLT_SHROOM:
			curSpr=bulletSpr->GetSprite(me->facing+SPR_SHROOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_GRENADE:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_GRENADE);
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
		case BLT_LILBOOM:
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
		case BLT_ICESPIKE:
			curSpr=bulletSpr->GetSprite(SPR_ICESPIKE+(me->anim));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME);
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
		case BLT_SPINE:
			curSpr=bulletSpr->GetSprite(SPR_SPINE+(me->facing));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
	}
}

void UpdateBullets(Map *map,world_t *world)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].type)
			UpdateBullet(&bullet[i],map,world);
}

void RenderBullets(void)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].type)
			RenderBullet(&bullet[i]);
}

void FireMe(bullet_t *me,int x,int y,byte facing,byte type)
{
	int f;

	me->type=type;
	me->x=x;
	me->y=y;
	me->facing=facing;
	me->bright=0;

	switch(me->type)
	{
		case BLT_BOOM:
			me->dx=0;
			me->dy=0;
			me->z=0;
			me->anim=0;
			me->timer=7;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1800);
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
		case BLT_EVILHAMMER:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			me->dz=FIXAMT*10;
			break;
		case BLT_BOMB:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->dz=FIXAMT*5;
			MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
			break;
		case BLT_MISSILE:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=0;
			me->facing=facing*2;
			me->target=65535;
			f=me->facing;
			f+=MGL_random(5)-2;
			if(f<0)
				f+=16;
			me->facing=(byte)(f&15);
			me->x+=((MGL_random(17)-8)<<FIXSHIFT);
			me->y+=((MGL_random(17)-8)<<FIXSHIFT);
			me->dx=Cosine(me->facing*16)*4;
			me->dy=Sine(me->facing*16)*4;
			MakeSound(SND_MISSILELAUNCH,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_ACID:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*10;
			me->dz=FIXAMT*4;
			me->dx=Cosine(me->facing)*10;
			me->dy=Sine(me->facing)*10;
			me->facing=((me->facing+16)&255)/32;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			me->anim=0;
			me->timer=24-MGL_random(4);
			me->z=FIXAMT*20;
			me->x+=((MGL_random(3)-1)<<FIXSHIFT)+Cosine(me->facing*32)*5;
			me->y+=((MGL_random(3)-1)<<FIXSHIFT)+Sine(me->facing*32)*5;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			me->dz=-FIXAMT/2;
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_LASER:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20-MGL_random(65535);
			me->x+=((MGL_random(3)-1)<<FIXSHIFT);
			me->y+=((MGL_random(3)-1)<<FIXSHIFT);
			me->x+=FIXAMT/2-MGL_random(65535);
			me->y+=FIXAMT/2-MGL_random(65535);
			me->facing=me->facing*32+4-MGL_random(9);
			me->dx=Cosine(me->facing)*24;
			me->dy=Sine(me->facing)*24;
			me->facing/=16;
			me->dz=0;
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1050);
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
		case BLT_GRENADE:
			me->anim=0;
			me->timer=255;
			me->z=FIXAMT*80;
			f=MGL_random(12)+1;
			me->dx=Cosine(me->facing)*f;
			me->dy=Sine(me->facing)*f;
			me->dz=FIXAMT*20;
			break;
		case BLT_SHOCKWAVE:
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
		case BLT_ICESPIKE:
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
			//MakeSound(SND_ENERGYFIRE,me->x,me->y,SND_CUTOFF,950);
			break;
		case BLT_SPINE:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			// this takes 0-255 directional facings
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->facing/=16;
			me->dz=0;
			break;
	}
}

void LaunchMegabeam(int x,int y,word owner)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,0,BLT_MEGABEAM);
			bullet[i].target=owner;
			break;
		}
}

void FireBullet(int x,int y,byte facing,byte type)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type);
			break;
		}
}

// this only fires if there is room in the bullet list PAST a specific point
// this is used for the Megabeam to ensure that all the laser bits stay lined up nicely
void FireBulletAfter(int x,int y,byte facing,byte type,bullet_t *thisone)
{
	int i,j,start;

	for(j=0;j<MAX_BULLETS;j++)
		if(&bullet[j]==thisone)
		{
			start=j+1;
			break;
		}
	for(i=start;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type);
			break;
		}
}

void FireExactBullet(int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
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
			break;
		}
}

void HammerLaunch(int x,int y,byte facing,byte count,byte flags)
{
	byte angle,newfacing;

	MakeSound(SND_HAMMERTOSS,x,y,SND_CUTOFF,1200);
	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		angle=facing*32;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,facing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		angle=facing*32-8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,facing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
		angle=facing*32+8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,facing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		angle=facing*32-32;
		newfacing=((byte)(facing-1))%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
		angle=facing*32+32;
		newfacing=(facing+1)%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		angle=facing*32-56;
		newfacing=((byte)(facing-2))%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
		angle=facing*32+56;
		newfacing=(facing+2)%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0));
	}
	if(flags&HMR_REVERSE)
	{
		newfacing=((byte)(facing-4))%8;
		HammerLaunch(x,y,newfacing,count,flags&(~HMR_REVERSE));
	}
}
