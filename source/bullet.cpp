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
#define SPR_BIGAXE	 310
#define SPR_SPEAR	 318
#define SPR_SLASH	 326
#define SPR_MINE	 350
#define SPR_STINKY	 355
#define SPR_GREEN	 358
#define SPR_ORBITER  359

bullet_t bullet[MAX_BULLETS];
sprite_set_t *bulletSpr;
byte reflect=0;

void InitBullets(void)
{
	bulletSpr=new sprite_set_t("graphics\\bullets.jsp");

	memset(bullet,0,MAX_BULLETS*sizeof(bullet_t));
}

void ExitBullets(void)
{
	delete bulletSpr;
}

byte Bulletable(Map *map,int x,int y)
{
	if(map->map[x+y*map->width].wall ||
		(map->map[x+y*map->width].item>=MAX_SHOOTABLE_ITMS &&
		 map->map[x+y*map->width].item<NEW_PICKUP_ITMS))
		return 0;

	return 1;
}

void OffScreenBulletDie(bullet_t *me,Map *map)
{
	if(me->x<0 || me->y<0 || me->x>=(map->width*TILE_WIDTH*FIXAMT) ||
		me->y>=(map->height*TILE_HEIGHT*FIXAMT))
		me->type=BLT_NONE;
}

byte BulletCanGo(int xx,int yy,Map *map,byte size)
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
		case BLT_BIGSHELL:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_ORBITER:
		case BLT_SWAP:
			me->x-=me->dx;
			me->dx=-me->dx;
			break;
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:	// reflects off walls
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_BALLLIGHTNING:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->x-=me->dx;
				me->facing=84+MGL_random(89);
				if(me->dx<0)
					me->facing=(me->facing+128)&255;
				me->dx=Cosine(me->facing)*11;
				me->dy=Sine(me->facing)*11;
			}
			else
				BulletRanOut(me,map,world);
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
		case BLT_SLASH:
			MakeSound(SND_CLANG,me->x,me->y,SND_CUTOFF,950);
			// supposedly sparks
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,4,8);
			me->type=BLT_NONE;
			break;
		case BLT_MISSILE:
		case BLT_SPINE:
		case BLT_BIGAXE:
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_MINDWIPE:
		case BLT_REFLECT:
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
		case BLT_SHARK:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_ENERGY:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_GREEN:
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
		case BLT_BIGSHELL:
			me->type=BLT_NONE;
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_ORBITER:
		case BLT_SWAP:
			me->y-=me->dy;
			me->dy=-me->dy;
			break;
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:	// reflects off walls
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_BALLLIGHTNING:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->y-=me->dy;
				me->facing=20+MGL_random(89);
				if(me->dy>0)
					me->facing+=128;
				me->dx=Cosine(me->facing)*11;
				me->dy=Sine(me->facing)*11;
			}
			else
				BulletRanOut(me,map,world);
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
		case BLT_SLASH:
			MakeSound(SND_CLANG,me->x,me->y,SND_CUTOFF,950);
			// supposedly sparks
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,4,8);
			me->type=BLT_NONE;
			break;
		case BLT_MISSILE:
		case BLT_SPINE:
		case BLT_BIGAXE:
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_MINDWIPE:
		case BLT_REFLECT:
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
		case BLT_SHARK:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_ENERGY:
			MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_GREEN:
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
		case BLT_BIGSHELL:
			BulletRanOut(me,map,world);
			break;
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
		case BLT_MINDWIPE:
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
		case BLT_BIGAXE:
		case BLT_SLASH:
		case BLT_MINE:
		case BLT_ORBITER:
		case BLT_BALLLIGHTNING:
		case BLT_REFLECT:
		case BLT_SWAP:
			me->z=0;
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
			BulletRanOut(me,map,world);
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
		case BLT_GREEN:
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
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
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
	Guy *g;

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
		case BLT_SLASH:
		case BLT_GREEN:
		case BLT_REFLECT:
			me->type=0;
			break;
		case BLT_SPEAR:
		case BLT_BADSPEAR:
		case BLT_MINDWIPE:
			ExplodeParticles(PART_HAMMER,me->x,me->y,0,8);
			me->type=0;
			break;
		case BLT_BIGSNOW:
			// poof into snowballs
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			me->type=0;
			for(i=0;i<8;i++)
				FireBullet(me->x,me->y,(byte)i*32,BLT_SNOWBALL,me->friendly);
			break;
		case BLT_SNOWBALL:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,6,12);
			break;
		case BLT_BIGAXE:
			me->type=0;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,10,8);
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
		case BLT_SWAP:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_SHARK:
			MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			me->type=0;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			me->type=0;
			BlowSmoke(me->x,me->y,me->z,FIXAMT);
			break;
		case BLT_BOMB:
		case BLT_MINE:
		case BLT_ORBITER:
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
				FireExactBullet(me->x,me->y,me->z,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE,me->friendly);
				FireExactBullet(me->x,me->y,me->z,Cosine(i)*6,Sine(i)*6,0,0,16,i,BLT_SPORE,me->friendly);
			}
			g=AddGuy(me->x-me->dx*2,me->y-me->dy*2,me->z,MONS_SHROOM);	// become a living shroom
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
				g->type=MONS_NONE;
			else if(g)
				g->friendly=me->friendly;
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
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int i;

	switch(me->type)
	{
		case BLT_LASER:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,1,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_BULLETHIT,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_GREEN:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,3,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SLIME,me->x,me->y,me->z,8);
				MakeSound(SND_BULLETHIT,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_BALLLIGHTNING:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
				MakeSound(SND_LIGHTNING,me->x,me->y,SND_CUTOFF,900);
				for(i=0;i<player.hammers-1;i++)
					FireBullet(me->x,me->y,0,BLT_LIGHTNING2,me->friendly);
				me->type=BLT_NONE;
			}
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_BIGSHELL:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,5,map,world,me->friendly))
			{
				if(!reflect)
				{
					me->type=BLT_NONE;
					ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
					MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
				}
				else
				{
					me->type=BLT_EVILHAMMER;
					me->facing=(me->facing+4)&7;
					me->dx=Cosine(me->facing*32)*12;
					me->dy=Sine(me->facing*32)*12;
					me->timer+=10;
				}
			}
			break;
		case BLT_SPEAR:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,5,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_BADSPEAR:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,3,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_MISSILE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,8,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_LILBOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,2,map,world,me->friendly))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_SLASH:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,Cosine(me->facing*32)*4,
				Sine(me->facing*32)*4,6,map,world,me->friendly))
			{
				// nothing needs to be done
			}
			break;
		case BLT_FLAME:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let them scream
			}
			break;
		case BLT_FLAME2:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,1,map,world,me->friendly))
			{
				// no noise, just let him scream
			}
			break;
		case BLT_SPORE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/2,me->dy/2,1,map,world,me->friendly))
			{

				PoisonVictim(GetLastGuyHit(),30);
				me->type=BLT_NONE;	// go away
			}
			break;
		case BLT_SPINE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/4,me->dy/4,4,map,world,me->friendly))
			{
				me->type=BLT_NONE;
			}
			break;
		case BLT_SHROOM:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate, not to mention the 10 damage you already did
			}
			break;
		case BLT_ICESPIKE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,
				6,map,world,me->friendly))
			{
				// nothing special happens
			}
			break;
		case BLT_BOMB:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,0,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_MINE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,0,map,world,me->friendly))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_BOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,64,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,4,map,world,me->friendly))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_BIGAXE:
			if(FindVictims2(me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,(4-MGL_random(9))<<FIXSHIFT,
				(4-MGL_random(9))<<FIXSHIFT,5,map,world,me->friendly))
			{
				ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,10,8);
			}
			break;
		case BLT_YELBOOM:
			i=20*(5-(me->timer/2));	// size expands as boom expands
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,2,map,world,me->friendly))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_SHOCKWAVE:
			i=30*(3-(me->timer/2))+30;	// size expands as wave expands
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,0,0,4,map,world,me->friendly))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_ACID:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,1000);
				ExplodeParticles(PART_SLIME,me->x,me->y,me->z,6);
			}
			break;
		case BLT_SHARK:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,1000);
				ExplodeParticles(PART_WATER,me->x,me->y,me->z,6);
			}
			break;
		case BLT_MEGABEAM1:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,40,0,12*FIXAMT,5,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_MEGABEAMHIT,me->x,me->y,SND_CUTOFF,1000);
				me->type=BLT_MEGABEAM2;
				me->dy=0;
				me->timer=2;
			}
			break;
		case BLT_ENERGY:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,3,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ENERGYBONK,me->x,me->y,SND_CUTOFF,950);
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,6);
			}
			break;
		case BLT_EVILHAMMER:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,10,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,950);
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			}
			break;
		case BLT_ROCK:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,8,map,world,me->friendly))
			{
				me->type=BLT_NONE;
				MakeSound(SND_ROCKBOUNCE,me->x,me->y,SND_CUTOFF,600);
				ExplodeParticles2(PART_DIRT,me->x,me->y,me->z,12,6);
			}
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
			if(me->z<FIXAMT*40)
			{
				if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,6,map,world,me->friendly))
				{
					BulletRanOut(me,map,world);
				}
			}
			break;
		case BLT_MINDWIPE:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,0,map,world,me->friendly))
			{
				if(ControlMind(GetLastGuyHit()))
				{
					MakeSound(SND_ROBOBOUAPHAON,me->x,me->y,SND_CUTOFF,1400);
				}
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_REFLECT:
			ReflectBullets(me->x,me->y,128,me->friendly);
			break;
		case BLT_SWAP:
			if(SwapMe(me->x,me->y,10,map))
				BulletRanOut(me,map,world);
			break;
	}
}

inline void BulletFaceGuy(bullet_t *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

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
	int x,y,x2,y2,v,v2;
	byte b;
	word w;

	OffScreenBulletDie(me,map);
	if(!me->type)
		return;

	reflect=0;

	mapx=(me->x/TILE_WIDTH)>>FIXSHIFT;
	mapy=(me->y/TILE_HEIGHT)>>FIXSHIFT;
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
		|| me->type==BLT_ROCK || me->type==BLT_EVILHAMMER || me->type==BLT_SPEAR || me->type==BLT_BADSPEAR)
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
		case BLT_SPEAR:
		case BLT_BADSPEAR:
			me->dz+=FIXAMT/2;
			HitBadguys(me,map,world);
			break;
		case BLT_SLASH:
			me->anim++;
			if(me->anim==4)
				BulletRanOut(me,map,world);
			else if(me->anim==2)
			{
				HitBadguys(me,map,world);
				if(map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item==
					ITM_WALLGRASS)
				{
					map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
					ExplodeParticles2(PART_SLIME,me->x,me->y,me->z,30,10);
				}
			}
			break;
		case BLT_BIGAXE:
			me->anim++;
			if(me->anim>7)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_BIGSHELL:
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
				FireBullet(mapx,mapy,me->facing,BLT_ICESPIKE,me->friendly);
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
		case BLT_GREEN:
			HitBadguys(me,map,world);
			break;
		case BLT_LILBOOM:
			HitBadguys(me,map,world);
			break;
		case BLT_ACID:
		case BLT_SHARK:
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
				FireBulletAfter(me->x,me->y,0,BLT_MEGABEAM1,me,me->friendly);

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
					if(me->facing>8)
						me->facing++;
					else
						me->facing--;
				if(me->dx<0)
					if(me->facing>8)
						me->facing--;
					else
						me->facing++;
				if(me->dy>0)
					if(me->facing>11 || me->facing<4)
						me->facing++;
					else
						me->facing--;
				if(me->dy<0)
					if(me->facing>11 || me->facing<4)
						me->facing--;
					else
						me->facing++;

				if(me->facing>200)
					me->facing+=16;
				if(me->facing>15)
						me->facing-=16;
			}
			break;
		case BLT_LIGHTNING:
			w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			if(w!=65535)
			{
				GetGuy(w)->GetShot(0,0,2,map,world);
				LightningBolt(goodguy->x,goodguy->y-FIXAMT*20,GetGuy(w)->x,
					GetGuy(w)->y-FIXAMT*10-GetGuy(w)->z);
				MakeSound(SND_ZAP,GetGuy(w)->x,GetGuy(w)->y,SND_CUTOFF,1000);
			}
			else
			{
				// make sizzle around player if there was no target
				LightningBolt(
					goodguy->x-FIXAMT*32+MGL_randoml(FIXAMT*64),
					goodguy->y-FIXAMT*52+MGL_randoml(FIXAMT*64),
					goodguy->x-FIXAMT*32+MGL_randoml(FIXAMT*64),
					goodguy->y-FIXAMT*52+MGL_randoml(FIXAMT*64));
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
					FireBullet(me->x,me->y,me->facing*32,BLT_GREEN,me->friendly);
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
			v=MGL_random(256);
			v2=MGL_random(16)+2;
			x2=(x+Cosine(v)*v2);
			y2=(y+Sine(v)*v2);
			v=MGL_random(256);
			v2=MGL_random(16)+2;
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
				GetGuy(w)->GetShot(0,0,5,map,world);
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
					me->x-FIXAMT*64+MGL_randoml(FIXAMT*128),
					me->y-FIXAMT*64+MGL_randoml(FIXAMT*128));
			}
			me->type=BLT_NONE;	// begone immediately
			break;
		case BLT_MINDWIPE:
			me->anim++;
			if(me->anim>2)
				me->anim=0;

			HitBadguys(me,map,world);
			me->bright=(char)MGL_random(16);
			break;
		case BLT_REFLECT:
			me->anim++;
			//map->BrightTorch(me->x/(TILE_WIDTH*FIXAMT),me->y/(TILE_HEIGHT*FIXAMT),me->timer,4);
			if(me->anim<3)
				HitBadguys(me,map,world);
			break;
		case BLT_SWAP:
			me->anim+=8;
			HitBadguys(me,map,world);
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

void RenderStinky(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_STINKY+frm),DISPLAY_DRAWME|DISPLAY_GHOST);
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
		case BLT_SHARK:
			v=me->facing*7+me->anim+SPR_ACID;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDrawOff(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,1,3,me->bright,curSpr,
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
		case BLT_SPEAR:
		case BLT_BADSPEAR:
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
			break;
		case BLT_ORBITER:
			curSpr=bulletSpr->GetSprite(SPR_ORBITER+me->facing);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
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

void FireMe(bullet_t *me,int x,int y,byte facing,byte type,byte friendly)
{
	int f;

	me->friendly=friendly;
	me->type=type;
	me->x=x;
	me->y=y;
	me->facing=facing;
	me->bright=0;

	switch(me->type)
	{
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
		case BLT_SHARK:
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
			break;
		case BLT_GREEN:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			break;
		case BLT_ORBITER:
			me->anim=15;
			me->timer=40;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*8+Sine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*8+Cosine(me->facing*32)*4;
			me->dz=0;
			break;
		case BLT_MINDWIPE:
			me->anim=0;
			me->timer=40;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			me->dz=0;
			break;
	}
}

void QuadMissile(int x,int y,byte facing,byte friendly)
{
	int i;
	byte f;
	bullet_t *me;

	f=(facing*2-2)&15;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			me=&bullet[i];
			me->type=BLT_MISSILE;
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


void LaunchMegabeam(int x,int y,word owner)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,0,BLT_MEGABEAM,GetGuy(owner)->friendly);
			bullet[i].target=owner;
			break;
		}
}

void SpitAcid(int x,int y,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
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
			break;
		}
}

void FireBullet(int x,int y,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type,friendly);
			break;
		}
}

void FireBulletZ(int x,int y,int z,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type,friendly);
			bullet[i].z=z;
			break;
		}
}

// this only fires if there is room in the bullet list PAST a specific point
// this is used for the Megabeam to ensure that all the laser bits stay lined up nicely
void FireBulletAfter(int x,int y,byte facing,byte type,bullet_t *thisone,byte friendly)
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
			FireMe(&bullet[i],x,y,facing,type,friendly);
			break;
		}
}

void FireExactBullet(int x,int y,int z,int dx,int dy,int dz,byte anim,byte timer,byte facing,byte type,byte friendly)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
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
						0,30,facing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		angle=facing*32-8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,facing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
		angle=facing*32+8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,facing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		angle=facing*32-32;
		newfacing=((byte)(facing-1))%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
		angle=facing*32+32;
		newfacing=(facing+1)%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		angle=facing*32-56;
		newfacing=((byte)(facing-2))%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
		angle=facing*32+56;
		newfacing=(facing+2)%8;
		FireExactBullet(x,y,FIXAMT*20,
						Cosine(angle)*12,Sine(angle)*12,FIXAMT*6,
						0,30,newfacing,BLT_HAMMER+((flags&HMR_REFLECT)>0),1);
	}
	if(flags&HMR_REVERSE)
	{
		newfacing=((byte)(facing-4))%8;
		HammerLaunch(x,y,newfacing,count,flags&(~HMR_REVERSE));
	}
}

void HappyFire(int x,int y,byte facing)
{
	byte happyList[]={BLT_HAMMER,BLT_HAMMER2,BLT_MISSILE,BLT_FLAME,BLT_LASER,BLT_BOMB,
					  BLT_BALLLIGHTNING,BLT_BIGSHELL,BLT_BIGAXE,BLT_LIGHTNING,BLT_SPEAR,BLT_SLASH,
					  BLT_GREEN};
	byte b;

	b=happyList[MGL_random(13)];

	FireBullet(x,y,facing,b,1);
}

void HappyLaunch(int x,int y,byte facing,byte count,byte flags)
{
	MakeSound(SND_HAMMERTOSS,x,y,SND_CUTOFF,1200);
	if(count==1 || count==3 || count==5)	// 1,3,5 have direct forward fire
	{
		HappyFire(x,y,facing);
	}
	if(count==2 || count==4)	// these have slight off-angle double forward fire
	{
		HappyFire(x,y,facing);
		if(MGL_random(2)==0)
			HappyFire(x,y,(facing-1)&7);
		else
			HappyFire(x,y,(facing+1)&7);
	}
	if(count==3 || count==5)	// these have 45 degree angle fire
	{
		HappyFire(x,y,(facing-1)&7);
		HappyFire(x,y,(facing+1)&7);
	}
	if(count==4 || count==5)	// these add almost 90 degree off fire
	{
		HappyFire(x,y,(facing-2)&7);
		HappyFire(x,y,(facing+2)&7);
	}
	if(flags&HMR_REVERSE)
	{
		HappyLaunch(x,y,(facing+4)&7,count,(flags&(~HMR_REVERSE)));
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

	for(i=0;i<MAX_BULLETS;i++)
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

void ReflectBullets(int x,int y,byte size,byte friendly)
{
	int i;
	int rx,ry,rx2,ry2;

	rx=x-size*FIXAMT;
	rx2=x+size*FIXAMT;
	ry=y-size*FIXAMT;
	ry2=y+size*FIXAMT;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type && bullet[i].friendly!=friendly && bullet[i].x>rx &&
			bullet[i].y>ry && bullet[i].x<rx2 && bullet[i].y<ry2)
		{
			bullet[i].friendly=friendly;
			bullet[i].dx=-bullet[i].dx;
			bullet[i].dy=-bullet[i].dy;
		}
	}
}

void BulletTally(void)
{
#ifdef LOG
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type)
		{
			fprintf(logFile,"BUL %d: Type %d",i,bullet[i].type);
		}
	}
	fflush(logFile);
#endif
}
