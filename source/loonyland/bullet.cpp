#include "bullet.h"
#include "guy.h"
#include "player.h"
#include "quest.h"
#include "options.h"
#include "loonyball.h"
#include "bowling.h"
#include "ch_witch.h"
#include "badge.h"

bullet_t bullet[MAX_BULLETS];
sprite_set_t *bulletSpr;
byte reflect=0;
byte ballSoundClock,ballSteerClock;
byte bulControl;

byte bulletHittingType;

void InitBullets(void)
{
	bulletSpr=new sprite_set_t("graphics/bullets.jsp");

	memset(bullet,0,MAX_BULLETS*sizeof(bullet_t));
}

void ExitBullets(void)
{
	delete bulletSpr;
}

byte Bulletable(bullet_t *me,Map *map,int x,int y)
{
	// the IF_NOBULLET thing is cheesy - it is used only for autodoors, don't use for
	// anything else or it will mess up
	if(map->map[x+y*map->width].wall ||
		(ItemFlags(map->map[x+y*map->width].item)&IF_TALL) ||
		(curWorld.terrain[map->map[x+y*map->width].floor].flags&TF_NOBULLET) ||
		((ItemFlags(map->map[x+y*map->width].item)&IF_NOBULLET) && map->map[x+y*map->width].itemInfo<4))
	{
		if(me->type==BLT_BOMB && map->map[x+y*map->width].item==ITM_BOOMROCK)
		{
			BombRanOut(me);
		}

		return 0;
	}

	return 1;
}

void CheckBoomRock(Map *map,int x,int y)
{
	if(x>=0 && y>=0 && x<map->width && y<map->height)
	{
		if(map->map[x+y*map->width].item==ITM_BOOMROCK)
		{
			map->map[x+y*map->width].item=0;
			ExplodeParticles2(PART_DIRT,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,60,20);
			BadgeCheck(BE_DESTROY,ITM_BOOMROCK,map);
		}
	}
}

void CheckBoomRocks(Map *map,int xx,int yy,int size)
{
	int i,j;
	int mapx,mapy,mapx1,mapx2,mapy1,mapy2;

	xx>>=FIXSHIFT;
	yy>>=FIXSHIFT;

	mapx=xx/TILE_WIDTH;
	mapy=yy/TILE_HEIGHT;
	mapx1=(xx-size)/TILE_WIDTH;
	mapy1=(yy-size)/TILE_HEIGHT;
	mapx2=(xx+size)/TILE_WIDTH;
	mapy2=(yy+size)/TILE_HEIGHT;

	for(i=mapx1;i<=mapx2;i++)
		for(j=mapy1;j<=mapy2;j++)
		{
			CheckBoomRock(map,i,j);
		}
}

byte GoodBullet(byte type)
{
	return (type==BLT_BALLLIGHTNING || type==BLT_WHOOPEE || type==BLT_WHOOPEE2 ||
		 type==BLT_WHOOPEE3 || type==BLT_CACTUS || type==BLT_BOOMERANG ||
		 type==BLT_ICE || type==BLT_BOMB || type==BLT_GOODBOOM || type==BLT_GOODBOOM2
		 || type==BLT_HOTPANTS || type==BLT_ICE2 || type==BLT_BATSHOT || type==BLT_PSDSHOT ||
		 type==BLT_LOONYBALL || type==BLT_BOWLINGBALL || type==BLT_BOWLBOOM ||
		 (type>=BLT_WITCHBLAST && type<=BLT_WITCHKABOOM) || type==BLT_WOLFSHOT || type==BLT_CACTUS2 ||
		 (type>=BLT_SUMBOOM && type<=BLT_SUMHEAL) || type==BLT_DEVILDOLL || type==BLT_BATGAS || type==BLT_AURA ||
		 type==BLT_THFKNIFE || type==BLT_KNIFESHRP || type==BLT_THFFIRE);
}

byte BulletCanGo(bullet_t *me,int xx,int yy,Map *map,byte size)
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

	result=(xx>=0 && yy>=0 && mapx2<map->width && mapy2<map->height &&
		(Bulletable(me,map,mapx,mapy1)) &&
		(Bulletable(me,map,mapx,mapy2)) &&
		(Bulletable(me,map,mapx1,mapy)) &&
		(Bulletable(me,map,mapx2,mapy)) &&
		(Bulletable(me,map,mapx,mapy)) &&
		(Bulletable(me,map,mapx1,mapy1)) &&
		(Bulletable(me,map,mapx2,mapy1)) &&
		(Bulletable(me,map,mapx2,mapy2)) &&
		(Bulletable(me,map,mapx1,mapy2)));

	if((!result) && GoodBullet(me->type))
		SpecialShootCheck(map,mapx,mapy);

	return result;
}

void BulletHitWallX(bullet_t *me,Map *map,world_t *world)
{
	if(player.worldNum!=WORLD_BOWLING)
		TileHitCheck(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,map);

	switch(me->type)
	{
		case BLT_BALLLIGHTNING:
		case BLT_PSDSHOT:
		case BLT_WITCHBLAST:
		case BLT_DEVILDOLL:
		case BLT_THFKNIFE:
			if(me->type==BLT_THFKNIFE)
			{
				if(player.spellXP[ITM_WCACTUS-ITM_WBOMB])
				{
					int i;
					for(i=0;i<2;i++)
						FireBullet(me->x-me->dx,me->y-me->dy,(byte)Random(256),BLT_KNIFESHRP);
				}
				me->target=0;
			}
			if(player.fireFlags&FF_REFLECT)
			{
				me->x-=me->dx;
				me->facing=(byte)(84+Random(89));
				if(me->dx<0)
					me->facing=(me->facing+128)&255;
				me->dx=Cosine(me->facing)*11;
				me->dy=Sine(me->facing)*11;
			}
			else
				BulletRanOut(me,map,world);
			break;
		case BLT_LOONYBALL:
			me->x-=me->dx;
			me->dx=-me->dx;
			me->dy+=-FIXAMT/8+Random(FIXAMT/4+1);
			break;
		case BLT_BOWLINGBALL:
		case BLT_BOOMERANG:
		case BLT_SUMHEAL:
			me->x-=me->dx;
			me->dx=-me->dx;
			break;
		case BLT_ENERGY:
		case BLT_BIGSHOT:
		case BLT_FLAMEWALL:
		case BLT_FLAMESHOT:
		case BLT_BATSHOT:
		case BLT_WOLFSHOT:
		case BLT_SUMFLAME:
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_CLAW:
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,7);
			me->type=0;
			break;
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
			ExplodeParticles(PART_DIRT,me->x,me->y,me->z,8);
			me->type=0;
			break;
		case BLT_GASBLAST:
			me->x-=me->dx;
			me->dx=0;
			if(me->timer>6)
				me->timer=6;
			break;
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
		case BLT_WITCHGAS:
		case BLT_HOTPANTS:
		case BLT_ORBGRENADE:
		case BLT_WATER:
		case BLT_FOUNTAIN:
		case BLT_SUMGAS:
		case BLT_BATGAS:
		case BLT_THFFIRE:
			me->x-=me->dx;
			me->dx=0;
			break;
		case BLT_MEGABOOM:
		case BLT_GOODBOOM:
		case BLT_GOODBOOM2:
		case BLT_BOWLBOOM:
		case BLT_SUMBOOM:
			break;
		case BLT_ITEM:
			me->x-=me->dx;
			me->dx=-me->dx-FIXAMT*2+Random(FIXAMT*4);
			Clamp(&me->dx,FIXAMT*6);
			break;
		case BLT_CACTUS:
		case BLT_CACTUS2:
		case BLT_KNIFESHRP:
			me->target=0;
			me->x-=me->dx;
			me->facing=(byte)(84+Random(89));
			if(me->dx<0)
				me->facing=(me->facing+128)&255;
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			break;
		case BLT_WITCHBUZZSAW:
			BulletRanOut(me,map,world);
			break;
		case BLT_ICE:
		case BLT_ICE2:
		case BLT_ICESHARD:
		case BLT_MISSILE:
		case BLT_WIND:
		case BLT_WITCHICE:
		case BLT_SUMFROST:
			BulletRanOut(me,map,world);
			break;
		case BLT_BOMB:
		case BLT_WITCHPLAGUE:
			me->x-=me->dx;
			me->dx=-me->dx;
			break;
		case BLT_EVILFACE:
			// can go through anything
			break;
	}
}

void BulletHitWallY(bullet_t *me,Map *map,world_t *world)
{
	if(player.worldNum!=WORLD_BOWLING)
		TileHitCheck(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,map);

	switch(me->type)
	{
		case BLT_BALLLIGHTNING:
		case BLT_PSDSHOT:
		case BLT_WITCHBLAST:
		case BLT_DEVILDOLL:
		case BLT_THFKNIFE:
			if(me->type==BLT_THFKNIFE)
			{
				if(player.spellXP[ITM_WCACTUS-ITM_WBOMB])
				{
					int i;
					for(i=0;i<3;i++)
						FireBullet(me->x-me->dx,me->y-me->dy,(byte)Random(256),BLT_KNIFESHRP);
				}
				me->target=0;
			}
			if(player.fireFlags&FF_REFLECT)
			{
				me->y-=me->dy;
				me->facing=(byte)(20+Random(89));
				if(me->dy>0)
					me->facing+=128;
				me->dx=Cosine(me->facing)*11;
				me->dy=Sine(me->facing)*11;
			}
			else
				BulletRanOut(me,map,world);
			break;
		case BLT_LOONYBALL:
			me->y-=me->dy;
			me->dy=-me->dy;
			me->dx+=-FIXAMT/8+Random(FIXAMT/4+1);
			break;
		case BLT_BOWLINGBALL:
		case BLT_BOOMERANG:
		case BLT_SUMHEAL:
			me->y-=me->dy;
			me->dy=-me->dy;
			break;
		case BLT_ENERGY:
		case BLT_BIGSHOT:
		case BLT_FLAMEWALL:
		case BLT_FLAMESHOT:
		case BLT_BATSHOT:
		case BLT_WOLFSHOT:
		case BLT_SUMFLAME:
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_CLAW:
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,7);
			me->type=0;
			break;
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
			ExplodeParticles(PART_DIRT,me->x,me->y,me->z,8);
			me->type=0;
			break;
		case BLT_GASBLAST:
			me->y-=me->dy;
			me->dy=0;
			if(me->timer>6)
				me->timer=6;
			break;
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
		case BLT_WITCHGAS:
		case BLT_HOTPANTS:
		case BLT_ORBGRENADE:
		case BLT_WATER:
		case BLT_FOUNTAIN:
		case BLT_SUMGAS:
		case BLT_BATGAS:
		case BLT_THFFIRE:
			me->y-=me->dy;
			me->dy=0;
			break;
		case BLT_MEGABOOM:
		case BLT_GOODBOOM:
		case BLT_GOODBOOM2:
		case BLT_BOWLBOOM:
		case BLT_SUMBOOM:
			break;
		case BLT_ITEM:
			me->y-=me->dy;
			me->dy=-me->dy-FIXAMT*2+Random(FIXAMT*4);
			Clamp(&me->dy,FIXAMT*6);
			break;
		case BLT_CACTUS:
		case BLT_CACTUS2:
		case BLT_KNIFESHRP:
			me->target=0;
			me->y-=me->dy;
			me->facing=(byte)(20+Random(89));
			if(me->dy>0)
				me->facing+=128;
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			break;
		case BLT_WITCHBUZZSAW:
			BulletRanOut(me,map,world);
			break;
		case BLT_ICE:
		case BLT_ICE2:
		case BLT_MISSILE:
		case BLT_WIND:
		case BLT_ICESHARD:
		case BLT_WITCHICE:
		case BLT_SUMFROST:
			BulletRanOut(me,map,world);
			break;
		case BLT_BOMB:
		case BLT_WITCHPLAGUE:
			me->y-=me->dy;
			me->dy=-me->dy;
			break;
		case BLT_EVILFACE:
			// can go through anything
			break;
	}
}

void BulletHitFloor(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_BALLLIGHTNING:
		case BLT_BOMB:
			me->z=0;
			me->dz=-me->dz;
			break;
		case BLT_LOONYBALL:
			if(me->dz<-FIXAMT)
			{
				MakeSound(SND_BALLBOUNCE,me->x,me->y,SND_CUTOFF,300);
			}
			me->z=0;
			me->dz=-me->dz*15/16;
			// friction!
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);
			break;
		case BLT_DEVILDOLL:
			me->z=0;
			me->dz=-me->dz*11/16;
			// friction!
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);
			break;
		case BLT_ENERGY:
		case BLT_BIGSHOT:
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
			me->type=0;
			break;
		case BLT_BOWLINGBALL:
			me->z=0;
			me->dz=-me->dz/2;
			Dampen(&me->dx,FIXAMT/32);
			Dampen(&me->dy,FIXAMT/32);
			break;
		case BLT_FLAMEWALL:
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
		case BLT_GASBLAST:
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
		case BLT_MEGABOOM:
		case BLT_GOODBOOM:
		case BLT_GOODBOOM2:
		case BLT_CACTUS:
		case BLT_CACTUS2:
		case BLT_ICE:
		case BLT_ICE2:
		case BLT_BOOMERANG:
		case BLT_HOTPANTS:
		case BLT_MISSILE:
		case BLT_BATSHOT:
		case BLT_WIND:
		case BLT_ICESHARD:
		case BLT_FOUNTAIN:
		case BLT_FLAMESHOT:
		case BLT_PSDSHOT:
		case BLT_BOWLBOOM:
		case BLT_WITCHBLAST:
		case BLT_WITCHBUZZSAW:
		case BLT_WITCHICE:
		case BLT_WITCHGAS:
		case BLT_WITCHPLAGUE:
		case BLT_WOLFSHOT:
		case BLT_CLAW:
		case BLT_SUMBOOM:
		case BLT_SUMGAS:
		case BLT_SUMFLAME:
		case BLT_SUMHEAL:
		case BLT_SUMFROST:
		case BLT_BATGAS:
		case BLT_THFKNIFE:
		case BLT_KNIFESHRP:
		case BLT_THFFIRE:
			// don't care about the floor
			break;
		case BLT_ORBGRENADE:
		case BLT_WATER:
			BulletRanOut(me,map,world);
			break;
		case BLT_ITEM:
			me->z=0;
			me->dz=-me->dz;
			break;
		case BLT_EVILFACE:
			// can go through anything
			break;
	}
}

void BombRanOut(bullet_t *me)
{
	if(player.wpnLevel==0)
	{
		me->type=BLT_GOODBOOM;
		me->timer=7;
		MakeSound(SND_BOOM,me->x,me->y,SND_CUTOFF,1200);
	}
	else if(player.wpnLevel==1)
	{
		me->type=BLT_GOODBOOM;
		me->timer=7;
		MakeSound(SND_BOOM,me->x,me->y,SND_CUTOFF,1200);

		// and launch two more
		FireBullet(me->x-64*FIXAMT+Random(128*FIXAMT),
			me->y-48*FIXAMT+Random(96*FIXAMT),0,BLT_GOODBOOM);
		FireBullet(me->x-64*FIXAMT+Random(128*FIXAMT),
			me->y-48*FIXAMT+Random(96*FIXAMT),0,BLT_GOODBOOM);
	}
	else
	{
		me->type=BLT_GOODBOOM2;
		me->timer=9;
		MakeSound(SND_BIGBOOM,me->x,me->y,SND_CUTOFF,1200);
	}
	me->anim=0;
	me->dx=0;
	me->dy=0;
	me->dz=0;
}

void BulletRanOut(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_WITCHSHOCK:
		case BLT_ENERGY:
		case BLT_BIGSHOT:
		case BLT_FLAMEWALL:
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
		case BLT_GASBLAST:
		case BLT_MEGABOOM:
		case BLT_GOODBOOM:
		case BLT_GOODBOOM2:
		case BLT_BOWLBOOM:
		case BLT_BADBOOM:
		case BLT_ITEM:
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
		case BLT_CACTUS:
		case BLT_CACTUS2:
		case BLT_SWAMPGAS:
		case BLT_SWAMPGAS2:
		case BLT_MISLSMOKE:
		case BLT_HOTPANTS:
		case BLT_BATSHOT:
		case BLT_FOUNTAIN:
		case BLT_FLAMESHOT:
		case BLT_EVILFACE:
		case BLT_WITCHKABOOM:
		case BLT_WITCHSPEED:
		case BLT_WITCHGAS:
		case BLT_WITCHPLAGUE:
		case BLT_WOLFSHOT:
		case BLT_CLAW:
		case BLT_SUMBOOM:
		case BLT_SUMGAS:
		case BLT_SUMFLAME:
		case BLT_SUMHEAL:
		case BLT_BATGAS:
		case BLT_AURA:
		case BLT_KNIFESHRP:
		case BLT_THFFIRE:
			me->type=0;
			break;
		case BLT_LOONYBALL:
			me->timer=100;
			break;
		case BLT_BOWLINGBALL:
			GutterBall();
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			me->type=0;
			break;
		case BLT_WIND:
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			me->type=BLT_NONE;
			break;
		case BLT_THFKNIFE:
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,3);
			me->type=BLT_NONE;
			if(player.spellXP[ITM_WBOMB-ITM_WBOMB])
				FireBullet(me->x,me->y,me->facing,BLT_GOODBOOM);
			if(player.spellXP[ITM_WWHOOPEE-ITM_WBOMB])
				FireBullet(me->x,me->y,me->facing,BLT_WHOOPEE);
			break;
		case BLT_BALLLIGHTNING:
		case BLT_PSDSHOT:
		case BLT_WITCHBLAST:
		case BLT_WITCHBUZZSAW:
		case BLT_DEVILDOLL:
			if(opt.cheats[CH_FROGWPN])
				ExplodeParticles(PART_SLIME,me->x,me->y,me->z,4);
			else
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,4);
			me->type=BLT_NONE;
			break;
		case BLT_BOOMERANG:
			ExplodeParticles(PART_DIRT,me->x,me->y,me->z,8);
			me->type=0;
			break;
		case BLT_ICE:
		case BLT_ICE2:
		case BLT_ICESHARD:
		case BLT_WITCHICE:
		case BLT_SUMFROST:
			ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
			me->type=0;
			break;
		case BLT_ORBGRENADE:
			FireBullet(me->x,me->y,0,BLT_MEGABOOM);
			me->type=0;
			break;
		case BLT_MISSILE:
			FireBullet(me->x,me->y,0,BLT_BADBOOM);
			me->type=0;
			break;
		case BLT_BOMB:
			BombRanOut(me);
			break;
		case BLT_WATER:
			if(Random(3)==0)
				MakeSound(SND_WATERSPLASH,me->x,me->y,SND_CUTOFF,200);
			ExplodeParticles(PART_WATER,me->x,me->y,me->z,8);
			me->type=BLT_NONE;
			break;
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int i;
	byte b;
	bullet_t *you;

	bulletHittingType=me->type;

	switch(me->type)
	{
		case BLT_WITCHICE:
			if(FreezeVictimWitch(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,65535,250,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,10);
			}
			break;
		case BLT_WITCHGAS:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,me->dx,me->dy,1+SpellLevel(SPL_PLAGUE)/3,map,world))
			{
				// nothing, just ouch
			}
			break;
		case BLT_WITCHBLAST:
		case BLT_DEVILDOLL:
			if(player.monsType==MONS_PLYRSUMMON)
			{
				i=5;
			}
			else
			{
				i=SpellLevel(SPL_BLAST)*2;
				if(opt.cheats[CH_HEAVYHIT])
					i*=3;
				i+=2;
			}

			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,i,map,world))
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
				me->type=BLT_NONE;
			}
			break;
		case BLT_WITCHBUZZSAW:
			if(FindNewVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,&me->target,12,me->dx,me->dy,2,map,world))
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,3);
			}
			break;
		case BLT_LOONYBALL:
			GetKicked(me);
			break;
		case BLT_BOWLINGBALL:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,10,me->dx,me->dy,10,map,world))
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
				BowlingBallIsDone();
				me->type=0;
			}
			break;
		case BLT_BALLLIGHTNING:
			i=player.firePower*2+2;
			if(opt.cheats[CH_HEAVYHIT])
			{
				i+=player.fireRange*2;
				i+=player.fireRate*2;
			}
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,i,map,world))
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
				me->type=BLT_NONE;
			}
			break;
		case BLT_PSDSHOT:
			i=2+player.firePower/2;
			if(opt.cheats[CH_HEAVYHIT])
			{
				i+=player.fireRange*2;
				i+=player.fireRate*2;
			}
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,i,map,world))
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,8);
				me->type=BLT_NONE;
			}
			break;
		case BLT_WOLFSHOT:
			i=(player.firePower/2)+2;
			if(opt.cheats[CH_HEAVYHIT])
			{
				i+=player.fireRate/2;
				i*=(player.fireRange/3)+1;
			}
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,i,map,world))
			{
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				me->type=BLT_NONE;
			}
			break;
		case BLT_BATSHOT:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,1,map,world))
			{
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
				me->type=BLT_NONE;
			}
			break;
		case BLT_THFKNIFE:
			if(FindNewVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,&me->target,8,me->dx,me->dy,2,map,world))
			{
				if(player.spellXP[ITM_WICE-ITM_WBOMB] && me->target)
				{
					if(GetGuy(me->target)->hp>0 && !(MonsterFlags(GetGuy(me->target)->type)&MF_INVINCIBLE))
						GetGuy(me->target)->ice+=20;
				}
				if(player.spellXP[ITM_WCACTUS-ITM_WBOMB])
				{
					for(i=0;i<3;i++)
					{
						you=FireBullet(me->x-me->dx,me->y-me->dy,(byte)Random(256),BLT_KNIFESHRP);
						if(you)
							you->target=me->target;
					}
				}

				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,3);
				BulletRanOut(me,map,world);
				if(player.spellXP[ITM_WLIGHTNING-ITM_WBOMB])
				{
					me->type=BLT_THFKNIFE;	// don't vanish it if you've got the lightning powerup
				}
			}
			break;
		case BLT_BIGSHOT:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,3,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,12);
			}
			break;
		case BLT_ENERGY:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,1,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,6);
			}
			break;
		case BLT_FLAMEWALL:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,1,map,world))
			{
				// make a noise
			}
			break;
		case BLT_SUMFLAME:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,1,map,world))
			{
				// ouch
			}
			FindSummonToHelp(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,1,map,world);
			break;
		case BLT_FLAMESHOT:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,3,map,world))
			{
				// make a noise
			}
			break;
		case BLT_EARTHSPIKE:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,3,map,world))
			{
			}
			break;
		case BLT_EARTHSPIKE2:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,1,map,world))
			{
			}
			break;
		case BLT_GASBLAST:
			if((b=FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,me->dx,me->dy,0,map,world)))
			{
				// 3 hearts/seconds of poison please
				if(b==2)
					PoisonPlayer(32*3);
			}
			break;
		case BLT_MEGABOOM:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,100,0,0,4,map,world))
			{
				// ouch
			}
			break;
		case BLT_ITEM:
			if(ItemHitPlayer(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,0,0,me->anim,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			}
			break;
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,me->dx,me->dy,1+(me->type==BLT_WHOOPEE3),map,world))
			{
				// nothing, just ouch
			}
			break;
		case BLT_SUMGAS:
		case BLT_BATGAS:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,me->dx,me->dy,1,map,world))
			{
				// nothing, just ouch
			}
			break;
		case BLT_CACTUS:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx,me->dy,player.wpnLevel+1,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,3);
			}
			break;
		case BLT_KNIFESHRP:
			if(FindNewVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,&me->target,4,me->dx,me->dy,1,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,3);
			}
			break;
		case BLT_CACTUS2:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx,me->dy,4,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,6);
			}
			break;
		case BLT_BOOMERANG:
			if(FindNewVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,&me->target,12,me->dx,me->dy,7,map,world))
			{
				ExplodeParticles(PART_DIRT,me->x,me->y,me->z,3);
			}
			if(me->timer<60 &&
				ItemHitPlayer(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,0,0,0,map,world))
			{
				// get 2 gems for catching it again
				player.money+=(1+player.wpnLevel);
				if(player.money>player.maxMoney)
					player.money=player.maxMoney;
				me->type=BLT_NONE;
			}
			break;
		case BLT_ICE:
		case BLT_ICE2:
			if((i=FreezeVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,me->target,128,map,world)))
			{
				me->target=i;
				if(player.wpnLevel>0 && me->type!=BLT_ICE2)
				{
					for(i=0;i<player.wpnLevel*8;i++)
					{
						you=FireBullet(me->x,me->y,(byte)Random(256),BLT_ICE2);
						if(you)
							you->target=me->target;
					}
				}
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,10);
			}
			break;
		case BLT_SUMFROST:
			if(FreezeVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,65535,100*(player.firePower+1),map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,10);
			}
			break;
		case BLT_BOMB:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,5,map,world))
			{
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_BADBOOM:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,48,0,0,3,map,world))
			{
				// ouch
			}
			break;
		case BLT_BOWLBOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,me->dx,me->dy,6,map,world))
			{
				// ouch
			}
			break;
		case BLT_GOODBOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,48,0,0,6-3*(player.monsType==MONS_PLYRTHIEF),map,world))
			{
				// ouch
			}
			CheckBoomRocks(map,me->x,me->y,48);
			break;
		case BLT_GOODBOOM2:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,128,0,0,20,map,world))
			{
				// ouch
			}
			CheckBoomRocks(map,me->x,me->y,128);
			break;
		case BLT_SUMBOOM:
			i=30*(player.firePower+1)/11;
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,128,0,0,i,map,world))
			{
				// ouch
			}
			CheckBoomRocks(map,me->x,me->y,128);
			break;
		case BLT_SWAMPGAS:
		case BLT_SWAMPGAS2:
			if((b=FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,10,me->dx,me->dy,0,map,world)))
			{
				// 1 hearts/seconds of poison please
				if(b==2)
					PoisonPlayer(32*1);
			}
			break;
		case BLT_HOTPANTS:
		case BLT_THFFIRE:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,1,map,world))
			{
				// ouch
			}
			break;
		case BLT_MISSILE:
		case BLT_WIND:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,3,map,world))
			{
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_SLINGPOW:
			if(FindSlingVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,3,player.firePower*2,map,world))
			{
				ExplodeParticles(PART_HAMMER,me->x,me->y,0,16);
			}
			else
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,0,4);
				MakeNormalSound(SND_MUMMYSHOOT);
			}
			me->type=BLT_NONE;
			break;
		case BLT_ICESHARD:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/2,me->dy/2,0,map,world))
			{
				player.invinc=0;	// do not blink
				player.stone+=15;
				goodguy->dx+=me->dx/4;
				goodguy->dy+=me->dy/4;
				MakeSound(SND_FREEZE,me->x,me->y,SND_CUTOFF,1200);
			}
			break;
		case BLT_WATER:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,1,map,world))
			{
				FireBullet(me->x-10*FIXAMT+Random(20*FIXAMT),me->y-10*FIXAMT+Random(20*FIXAMT),0,BLT_MISLSMOKE);
				MakeSound(SND_STEAM,me->x,me->y,SND_CUTOFF,600);
				me->type=BLT_NONE;
			}
			break;
		case BLT_CLAW:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,3,map,world))
			{
				ExplodeParticles(PART_SNOW2,me->x,me->y,0,10);
				me->type=BLT_NONE;
			}
			break;
		case BLT_SUMHEAL:
			if(FindSummonToHelp(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,2,map,world))
			{
				MakeSound(SND_HEAL,me->x,me->y,SND_CUTOFF,600);
				ExplodeParticles(PART_SLIME,me->x,me->y,0,10);
				me->type=BLT_NONE;
			}
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

inline void BulletFaceGuy2(bullet_t *me,Guy *goodguy)
{
	int f;
	int diff,dir,desired,amt;

	f=me->facing;
	me->facing/=32;
	BulletFaceGuy(me,goodguy);
	desired=me->facing*32;
	me->facing=f;

	if(desired>me->facing)
	{
		diff=desired-me->facing;
		if(diff>128)
		{
			dir=-1;
			diff=256-diff;
		}
		else
			dir=1;
		amt=4;
		if(amt>diff)
			amt=diff;
	}
	else
	{
		diff=me->facing-desired;
		if(diff>128)
		{
			dir=1;
			diff=256-diff;
		}
		else
			dir=-1;
		amt=4;
		if(amt>diff)
			amt=diff;
	}
	me->facing+=dir*amt;
}

void CalcBulletFacing(bullet_t *me)
{
	int dx,dy;

	dx=me->dx/(FIXAMT*4);
	dy=me->dy/(FIXAMT*4);

	if(dx==0)
	{
		if(dy<0)
			me->facing=192;
		else
			me->facing=64;
	}
	else if(dy==0)
	{
		if(dx<0)
			me->facing=128;
		else
			me->facing=0;
	}
	else
	{
		if(dx<0)
		{
			if(dy<0)
				me->facing=160;
			else
				me->facing=96;
		}
		else
		{
			if(dy<0)
				me->facing=224;
			else
				me->facing=32;
		}
	}
}

void BustLightning(int s,int e,char b,Map *map)
{
	int x1,y1,x2,y2;

	int i;

	x1=0;
	y1=0;
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[x1+y1*map->width].tag==s)
			break;
		x1++;
		if(x1==map->width)
		{
			x1=0;
			y1++;
		}
	}

	x2=0;
	y2=0;
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[x2+y2*map->width].tag==e)
			break;
		x2++;
		if(x2==map->width)
		{
			x2=0;
			y2++;
		}
	}

	LightningBolt((x1*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				  (y1*TILE_HEIGHT)*FIXAMT,
				  (x2*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				  (y2*TILE_HEIGHT)*FIXAMT);
	map->BrightTorch(x1,y1,b,8);
	map->BrightTorch(x2,y2,b,8);
}

void UpdateBullet(bullet_t *me,Map *map,world_t *world)
{
	byte bustOrder[]={29,20,21,22,23,30,31,24,25,26,27,28,29};
	int mapx,mapy;
	byte b,b2,b3,b4;
	bullet_t *you;
	word w;

	if(!me->type)
		return;

	reflect=0;

	mapx=(me->x/TILE_WIDTH)>>FIXSHIFT;
	mapy=(me->y/TILE_HEIGHT)>>FIXSHIFT;
	if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height)
		me->bright=map->map[mapx+mapy*map->width].templight;
	else
		me->bright=-32;

	b=0;

	if(me->type!=BLT_EVILFACE)
	{
		me->x+=me->dx;
		if(!BulletCanGo(me,me->x,me->y,map,8))
			BulletHitWallX(me,map,world);
		else
		{
			me->y+=me->dy;
			if(!BulletCanGo(me,me->x,me->y,map,8))
				BulletHitWallY(me,map,world);
		}
	}
	else
	{
		me->x+=me->dx;
		me->y+=me->dy;
		if(me->x<0 || me->x>=(map->width-1)*TILE_WIDTH*FIXAMT ||
			me->y<0 || me->y>=(map->height-1)*TILE_HEIGHT*FIXAMT)
			me->type=BLT_NONE;
	}

	me->z+=me->dz;

	if(me->z<0)
		BulletHitFloor(me,map,world);

	// all gravity-affected bullets, get gravitized
	if(me->type==BLT_BALLLIGHTNING || me->type==BLT_ITEM || me->type==BLT_BOMB || me->type==BLT_ORBGRENADE ||
		me->type==BLT_LOONYBALL || me->type==BLT_BOWLINGBALL || me->type==BLT_DEVILDOLL)
		me->dz-=FIXAMT;

	me->timer--;
	if(!me->timer)
		BulletRanOut(me,map,world);

	// special things like animation
	switch(me->type)
	{
		case BLT_SUMHEAL:
			if(me->timer>100)
			{
			}
			else
			{
				if(me->target==65535 || !GetGuyPos(me->target,&mapx,&mapy) ||
					GetGuy(me->target)->hp==0)
				{
					me->target=LockOnSummon(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				}
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

					Clamp(&me->dx,FIXAMT*10);
					Clamp(&me->dy,FIXAMT*10);
					HitBadguys(me,map,world);
				}
			}
			me->anim++;
			if(me->anim>=16)
			{
				me->anim=0;
			}
			break;
		case BLT_WITCHPLAGUE:
			if(me->target==65535 || !GetGuyPos(me->target,&mapx,&mapy) ||
				GetGuy(me->target)->hp==0)
			{
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			}
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

				Clamp(&me->dx,FIXAMT*10);
				Clamp(&me->dy,FIXAMT*10);
			}
			me->anim++;
			b=15-SpellLevel(SPL_PLAGUE);
			if(me->anim>=b)
			{
				me->anim=0;
				FireBullet(me->x,me->y,(byte)Random(256),BLT_WITCHGAS);
			}
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,1);
			break;
		case BLT_WITCHGAS:
			if(me->timer>=10)
			{
				me->anim++;
				if(me->anim>5*2)
				{
					me->anim=3*2;
					HitBadguys(me,map,world);
				}
				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_WITCHICE:
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if(me->x>mapx)
					me->dx-=FIXAMT/2;
				else
					me->dx+=FIXAMT/2;
				if(me->y>mapy)
					me->dy-=FIXAMT/2;
				else
					me->dy+=FIXAMT/2;

				Clamp(&me->dx,FIXAMT*14);
				Clamp(&me->dy,FIXAMT*14);
				b=me->facing;
				BulletFaceGuy2(me,GetGuy(me->target));
			}
			me->anim=1-me->anim;
			if(me->anim)
				ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,1,1);
			HitBadguys(me,map,world);
			break;
		case BLT_WITCHSHOCK:
			me->x=goodguy->x;
			me->y=goodguy->y;
			me->anim++;
			if(me->anim>12-SpellLevel(SPL_SHOCK))
			{
				me->anim=0;
				w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				if(w!=65535)
				{
					bulletHittingType=BLT_WITCHSHOCK;
					GetGuy(w)->GetShot(0,0,1+(SpellLevel(SPL_SHOCK)>4),map,world);
					LightningBolt(goodguy->x,goodguy->y-FIXAMT*20,GetGuy(w)->x,
						GetGuy(w)->y-FIXAMT*10-GetGuy(w)->z);
					MakeSound(SND_ZAP,GetGuy(w)->x,GetGuy(w)->y,SND_CUTOFF,1000);
				}
			}
			// make sizzle around player constantly
			LightningBolt(
				goodguy->x-FIXAMT*32+Random(FIXAMT*64),
				goodguy->y-FIXAMT*52+Random(FIXAMT*64),
				goodguy->x-FIXAMT*32+Random(FIXAMT*64),
				goodguy->y-FIXAMT*52+Random(FIXAMT*64));
			break;
		case BLT_WITCHSPEED:
			me->x=goodguy->x;
			me->y=goodguy->y;
			Burn(me->x,me->y,Random(FIXAMT*40));
			if(player.speed<1)
			{
				player.speed=10;
				PowerupSpell(SPL_SPEED,SpellLevel(SPL_SPEED)+1);
			}
			break;
		case BLT_WITCHKABOOM:
			if(me->timer==59)
			{
				// one huge explosion
				FireBullet(me->x,me->y,0,BLT_GOODBOOM2);
			}
			b2=SpellLevel(SPL_KABOOM)*4;
			if(SpellLevel(SPL_KABOOM)>=4)
				b2+=(SpellLevel(SPL_KABOOM)-3)*4;

			b4=SpellLevel(SPL_KABOOM)*2;
			if(b2>b4)
				b3=b4;
			else
				b3=b2;
			if(me->timer==45 && b2>0)
			{
				// ring of lesser ones
				for(w=0;w<b3;w++)
				{
					b=256*w/b3;
					FireBullet(me->x+Cosine(b)*80,me->y+Sine(b)*80,0,BLT_GOODBOOM);
				}
			}
			b2-=b3;
			if(b2>b4)
				b3=b4;
			else
				b3=b2;
			if(me->timer==30 && b2>0)
			{
				// ring of lesser ones
				for(w=0;w<b3;w++)
				{
					b=256*w/b3;
					FireBullet(me->x+Cosine(b)*160,me->y+Sine(b)*160,0,BLT_GOODBOOM);
				}
			}
			b2-=b3;
			if(b2>b4)
				b3=b4;
			else
				b3=b2;
			if(me->timer==15 && b2>0)
			{
				// ring of lesser ones
				for(w=0;w<b3;w++)
				{
					b=256*w/b3;
					FireBullet(me->x+Cosine(b)*240,me->y+Sine(b)*240,0,BLT_GOODBOOM);
				}
			}
			b2-=b3;
			b3=b2;
			if(me->timer==1 && b2>0)
			{
				// ring of lesser ones
				for(w=0;w<b3;w++)
				{
					b=256*w/b3;
					FireBullet(me->x+Cosine(b)*320,me->y+Sine(b)*320,0,BLT_GOODBOOM);
				}
			}
			break;
		case BLT_WITCHBUZZSAW:
			me->facing+=SpellLevel(SPL_BUZZSAW)+2;
			if(me->facing<3)
				me->target=65535;
			me->x=goodguy->x+Cosine(me->facing)*48;
			me->y=goodguy->y+Sine(me->facing)*48;
			if(!opt.cheats[CH_FROGWPN])
				MakeCircleParticle(me->x,me->y,me->z,8);
			HitBadguys(me,map,world);
			break;
		case BLT_WITCHBLAST:
			if(player.fireFlags&FF_GUIDED)
			{
				if(bulControl&CONTROL_UP)
					me->dy-=FIXAMT;
				if(bulControl&CONTROL_DN)
					me->dy+=FIXAMT;
				if(bulControl&CONTROL_LF)
					me->dx-=FIXAMT;
				if(bulControl&CONTROL_RT)
					me->dx+=FIXAMT;
				Clamp(&me->dx,FIXAMT*16);
				Clamp(&me->dy,FIXAMT*16);
			}
			if(player.fireFlags&FF_HOMING)
			{
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

					Clamp(&me->dx,FIXAMT*14);
					Clamp(&me->dy,FIXAMT*14);
				}
			}

			if(!opt.cheats[CH_FROGWPN])
			{
				if(player.monsType==MONS_PLYRSUMMON)
				{
					b=8;
					map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,2,4);
				}
				else
				{
					if(!opt.cheats[CH_HEAVYHIT])
						b=SpellLevel(SPL_BLAST)*2+4;
					else
						b=SpellLevel(SPL_BLAST)*6+4;
					map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,SpellLevel(SPL_BLAST),4);
				}

				MakeCircleParticle2(me->x,me->y,me->z,b/2);
				MakeCircleParticle2(me->x+Cosine(me->facing)*b,me->y+Sine(me->facing)*b,me->z,b/2);
				MakeCircleParticle2(me->x+Cosine((me->facing+85)&255)*b,me->y+Sine((me->facing+85)&255)*b,me->z,b/2);
				MakeCircleParticle2(me->x+Cosine((me->facing+85*2)&255)*b,me->y+Sine((me->facing+85*2)&255)*b,me->z,b/2);
			}
			me->facing+=8;

			HitBadguys(me,map,world);
			break;
		case BLT_DEVILDOLL:
			if(player.fireFlags&FF_GUIDED)
			{
				if(bulControl&CONTROL_UP)
					me->dy-=FIXAMT;
				if(bulControl&CONTROL_DN)
					me->dy+=FIXAMT;
				if(bulControl&CONTROL_LF)
					me->dx-=FIXAMT;
				if(bulControl&CONTROL_RT)
					me->dx+=FIXAMT;
				Clamp(&me->dx,FIXAMT*16);
				Clamp(&me->dy,FIXAMT*16);
			}
			if(player.fireFlags&FF_HOMING)
			{
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

					Clamp(&me->dx,FIXAMT*14);
					Clamp(&me->dy,FIXAMT*14);
				}
			}

			HitBadguys(me,map,world);
			break;
		case BLT_LOONYBALL:
			if(me->target==65535)
			{
				HitBadguys(me,map,world);
				Dampen(&me->dx,FIXAMT/16);
				Dampen(&me->dy,FIXAMT/16);
				Clamp(&me->dx,FIXAMT*16);
				Clamp(&me->dy,FIXAMT*16);
				if(me->anim<100)
					me->anim++;

				if(opt.cheats[CH_BEND] && ballSteerClock)
				{
					ballSteerClock--;
					b=bulControl;
					if(b&CONTROL_LF)
						me->dx-=FIXAMT/2;
					if(b&CONTROL_RT)
						me->dx+=FIXAMT/2;
				}
			}
			else
			{
				me->anim=0;
				if(!GetGuyPos(me->target,&mapx,&mapy))
				{
					me->target=65535;
				}
				else
				{
					me->x=mapx;
					me->y=mapy+FIXAMT*16;
					me->z=FIXAMT*20+GetGuy(me->target)->z;
					me->dz=0;
				}
			}
			if(opt.cheats[CH_KICKCAT] && Random(600)==0)
			{
				MakeSound(SND_MEOW,me->x,me->y,SND_CUTOFF,500);
			}
			if(ballSoundClock)
				ballSoundClock--;
			break;
		case BLT_BOWLINGBALL:
			HitBadguys(me,map,world);
			if(me->y<FIXAMT*TILE_HEIGHT*3)
			{
				// went into the ball return deal
				GutterBall();
				me->type=0;
			}
			b=bulControl;
			if(opt.cheats[CH_BEND])
			{
				if(b&CONTROL_LF)
					me->dx-=FIXAMT/2;
				if(b&CONTROL_RT)
					me->dx+=FIXAMT/2;
				if(b&CONTROL_UP)
					me->dy-=FIXAMT/2;
				if(b&CONTROL_DN)
					me->dy+=FIXAMT/2;
				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);
			}
			else
			{
				if(b&CONTROL_LF)
					me->dx-=FIXAMT/8;
				if(b&CONTROL_RT)
					me->dx+=FIXAMT/8;
			}
			break;
		case BLT_ENERGY:
			me->anim=1-me->anim;	// flip-flop animation
			HitBadguys(me,map,world);
			break;
		case BLT_BIGSHOT:
			me->anim=1-me->anim;	// flip-flop animation
			me->facing+=16;			// spin
			HitBadguys(me,map,world);
			break;
		case BLT_BALLLIGHTNING:
			if(player.fireFlags&FF_GUIDED)
			{
				if(bulControl&CONTROL_UP)
					me->dy-=FIXAMT;
				if(bulControl&CONTROL_DN)
					me->dy+=FIXAMT;
				if(bulControl&CONTROL_LF)
					me->dx-=FIXAMT;
				if(bulControl&CONTROL_RT)
					me->dx+=FIXAMT;
				Clamp(&me->dx,FIXAMT*16);
				Clamp(&me->dy,FIXAMT*16);
			}
			if(player.fireFlags&FF_HOMING)
			{
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

					Clamp(&me->dx,FIXAMT*14);
					Clamp(&me->dy,FIXAMT*14);
				}
			}

			if(!opt.cheats[CH_FROGWPN])
			{
				if(!opt.cheats[CH_HEAVYHIT])
					MakeCircleParticle(me->x,me->y,me->z,player.firePower*2+4);
				else
					MakeCircleParticle(me->x,me->y,me->z,player.firePower*2+player.fireRate*2+player.fireRange*2+4);
			}

			HitBadguys(me,map,world);
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,player.firePower,4);
			break;
		case BLT_PSDSHOT:
			if(player.fireFlags&FF_GUIDED)
			{
				if(bulControl&CONTROL_UP)
					me->dy-=FIXAMT;
				if(bulControl&CONTROL_DN)
					me->dy+=FIXAMT;
				if(bulControl&CONTROL_LF)
					me->dx-=FIXAMT;
				if(bulControl&CONTROL_RT)
					me->dx+=FIXAMT;
				Clamp(&me->dx,FIXAMT*16);
				Clamp(&me->dy,FIXAMT*16);
			}
			if(player.fireFlags&FF_HOMING)
			{
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

					Clamp(&me->dx,FIXAMT*14);
					Clamp(&me->dy,FIXAMT*14);
				}
			}

			if(!opt.cheats[CH_FROGWPN])
			{
				if(!opt.cheats[CH_HEAVYHIT])
					MakeCircleParticle(me->x,me->y,me->z,4+player.firePower/2);
				else
					MakeCircleParticle(me->x,me->y,me->z,6+player.fireRate*2+player.fireRange*2);
			}

			HitBadguys(me,map,world);
			break;
		case BLT_FLAMEWALL:
			if(me->timer>6)
				me->anim=(me->timer-6);
			else
				me->anim=(6-me->timer);
			if(me->anim>3)
				HitBadguys(me,map,world);
			break;
		case BLT_FLAMESHOT:
			if(me->anim>1)
				me->anim--;
			else
				me->anim=1-me->anim;
			HitBadguys(me,map,world);
			break;
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
			if(me->timer>22 || me->timer<10)
			{
				me->anim++;
				if(me->anim==15)
					me->type=BLT_NONE;
			}
			if(me->anim==4)
			{
				FireBullet(me->x+Cosine(me->facing)*TILE_WIDTH,me->y+Sine(me->facing)*TILE_HEIGHT,
						((me->facing+(256-16))+(byte)Random(33))&255,BLT_EARTHSPIKE);
			}
			if(me->anim>4 && me->anim<10)
				HitBadguys(me,map,world);
			break;
		case BLT_GASBLAST:
			if(me->timer>=10 && me->timer<=50)
			{
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
		case BLT_MEGABOOM:
			me->anim++;
			if(me->anim<2)
				HitBadguys(me,map,world);
			break;
		case BLT_ITEM:
			HitBadguys(me,map,world);
			break;
		case BLT_WHOOPEE:
			if(me->timer>=10 && me->timer<=50)
			{
				me->anim++;
				if(me->anim>5*2)
				{
					me->anim=3*2;
					HitBadguys(me,map,world);
				}
				Dampen(&me->dx,FIXAMT);
				Dampen(&me->dy,FIXAMT);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_WHOOPEE2:
			if(me->timer>=10 && me->timer<=110)
			{
				me->anim++;
				if(me->anim>5*2)
				{
					me->anim=3*2;
					HitBadguys(me,map,world);
				}
				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_WHOOPEE3:
			if(me->timer>=10 && me->timer<=170)
			{
				me->anim++;
				if(me->anim>5*2)
				{
					me->anim=3*2;
					HitBadguys(me,map,world);
				}
				if(me->anim==4*2)	// trigger twice as often
					HitBadguys(me,map,world);

				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_SUMGAS:
			if(me->timer>=10 && me->timer<=(20+player.firePower*20))
			{
				me->anim++;
				if(me->anim>5*2)
					me->anim=3*2;
				if((me->timer%(11-player.firePower))==0)
					HitBadguys(me,map,world);

				Dampen(&me->dx,FIXAMT/32);
				Dampen(&me->dy,FIXAMT/32);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_BATGAS:
			if(me->timer>=10 && me->timer<=50)
			{
				me->anim++;
				if(me->anim>5*2)
					me->anim=3*2;
				if((me->timer&3)==0  || player.batLevel==255)
					HitBadguys(me,map,world);

				Dampen(&me->dx,FIXAMT/32);
				Dampen(&me->dy,FIXAMT/32);
			}
			else if(me->timer<6)
			{
				if(me->anim>0)
					me->anim--;
			}
			break;
		case BLT_CACTUS:
		case BLT_CACTUS2:
		case BLT_ICESHARD:
		case BLT_KNIFESHRP:
			HitBadguys(me,map,world);
			break;
		case BLT_BOOMERANG:
			me->anim++;
			if(me->anim>15)
				me->anim=0;
			if(me->timer<60)
			{
				// home in on player
				BulletFaceGuy(me,goodguy);
				me->dx+=Cosine(me->facing*32);
				me->dy+=Sine(me->facing*32);
				Clamp(&me->dx,FIXAMT*8);
				Clamp(&me->dy,FIXAMT*8);
			}
			HitBadguys(me,map,world);
			break;
		case BLT_ICE:
		case BLT_ICE2:
			HitBadguys(me,map,world);
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,1,1);
			break;
		case BLT_SUMFROST:
			HitBadguys(me,map,world);
			ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,1,1);
			break;
		case BLT_BATSHOT:
		case BLT_WOLFSHOT:
			HitBadguys(me,map,world);
			break;
		case BLT_BOMB:
			me->anim=me->timer/10;
			HitBadguys(me,map,world);
			break;
		case BLT_GOODBOOM:
		case BLT_BADBOOM:
			me->anim++;
			if(me->anim<2)
				HitBadguys(me,map,world);
			break;
		case BLT_BOWLBOOM:
			me->x-=me->dx;
			me->y-=me->dy;
			me->anim++;
			if(me->anim<2)
				HitBadguys(me,map,world);
			break;
		case BLT_GOODBOOM2:
		case BLT_SUMBOOM:
			me->anim++;
			if(me->anim<2)
				HitBadguys(me,map,world);
			break;
		case BLT_MISLSMOKE:
			me->anim++;
			break;
		case BLT_SWAMPGAS:
		case BLT_SWAMPGAS2:
			if(me->anim>0)
			{
				me->anim++;
				HitBadguys(me,map,world);
				return;	// don't let the timer reset!
			}
			if(me->timer==1)
			{
				me->timer=3;
				switch(map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
					((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item)
				{
					case ITM_GASPIPELR:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						if(me->facing==0)
							me->x+=TILE_WIDTH*FIXAMT;
						else
							me->x-=TILE_WIDTH*FIXAMT;
						break;
					case ITM_GASPIPEUD:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						if(me->facing==2)
							me->y+=TILE_HEIGHT*FIXAMT;
						else
							me->y-=TILE_HEIGHT*FIXAMT;
						break;
					case ITM_GASPIPEDR:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						if(me->facing==4)
						{
							me->y+=TILE_HEIGHT*FIXAMT;
							me->facing=2;
						}
						else
						{
							me->x+=TILE_WIDTH*FIXAMT;
							me->facing=0;
						}
						break;
					case ITM_GASPIPEDL:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						if(me->facing==0)
						{
							me->y+=TILE_HEIGHT*FIXAMT;
							me->facing=2;
						}
						else
						{
							me->x-=TILE_WIDTH*FIXAMT;
							me->facing=4;
						}
						break;
					case ITM_GASPIPEUR:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						if(me->facing==4)
						{
							me->y-=TILE_HEIGHT*FIXAMT;
							me->facing=6;
						}
						else
						{
							me->x+=TILE_WIDTH*FIXAMT;
							me->facing=0;
						}
						break;
					case ITM_GASPIPEUL:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						if(me->facing==0)
						{
							me->y-=TILE_HEIGHT*FIXAMT;
							me->facing=6;
						}
						else
						{
							me->x-=TILE_WIDTH*FIXAMT;
							me->facing=4;
						}
						break;
					case ITM_GASSPARK:
						if(me->type==BLT_SWAMPGAS2)
						{
							map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
								((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item=0;
							FireBullet(me->x,me->y,0,BLT_GOODBOOM);
						}
						map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
							((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].itemInfo=18;
						me->type=BLT_NONE;
						//me->anim=1;
						//me->timer=7;
						break;
					default:
						me->anim=1;
						me->timer=7;
						break;
				}
				if(!GasCanEnter(me->facing,(me->x>>FIXSHIFT)/TILE_WIDTH,
								(me->y>>FIXSHIFT)/TILE_HEIGHT,map))
				{
					// vaporize if you can't go here
					me->type=BLT_NONE;
				}
				else
				{
					// if you're going through a pipe, make it steam
					b=map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
						((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].item;
					if(b>=ITM_GASPIPELR && b<=ITM_GASPIPEUL)
						map->map[(me->x>>FIXSHIFT)/TILE_WIDTH+
						((me->y>>FIXSHIFT)/TILE_HEIGHT)*map->width].itemInfo=1;
					else if(me->type==BLT_SWAMPGAS2)
					{
						// make big boom if not going through a pipe
						FireBullet(me->x,me->y,0,BLT_GOODBOOM2);
						me->type=BLT_NONE;
						player.var[240]++;	// and point out that you have finished
					}
				}
			}
			break;
		case BLT_HOTPANTS:
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->anim>0)
				me->anim--;
			else
				me->anim=1;
			if(me->timer<6)
				me->anim=(6-me->timer)/2;

			if(me->target>0 && me->timer==16 && player.wpnLevel>0)
			{
				for(b=0;b<4;b++)
				{
					you=FireBullet(me->x,me->y,(byte)Random(256),BLT_HOTPANTS);
					if(you)
						you->target=me->target-1;
				}
			}
			if(me->anim==0)
				HitBadguys(me,map,world);
			break;
		case BLT_THFFIRE:
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			if(me->anim>0)
				me->anim--;
			else
				me->anim=1;
			if(me->timer<6)
				me->anim=(6-me->timer)/2;
			if(me->target)
				me->target--;
			else
			{
				HitBadguys(me,map,world);
				me->target=2;
			}
			break;
		case BLT_SUMFLAME:
			Dampen(&me->dx,FIXAMT/8);
			Dampen(&me->dy,FIXAMT/8);
			if(me->anim>0)
				me->anim--;
			else
				me->anim=1;
			if(me->timer<6)
				me->anim=(6-me->timer)/2;

			if(me->anim==0)
				HitBadguys(me,map,world);
			break;
		case BLT_ORBGRENADE:
			me->anim++;
			break;
		case BLT_MISSILE:
			BulletFaceGuy2(me,goodguy);
			me->dx+=Cosine(me->facing)*2;
			me->dy+=Sine(me->facing)*2;
			Dampen(&me->dx,FIXAMT/2);
			Dampen(&me->dy,FIXAMT/2);
			Clamp(&me->dx,FIXAMT*8);
			Clamp(&me->dy,FIXAMT*8);
			HitBadguys(me,map,world);
			me->anim=1-me->anim;
			if(me->anim)
				FireBullet(me->x,me->y-FIXAMT*20,0,BLT_MISLSMOKE);
			break;
		case BLT_WIND:
			BulletFaceGuy2(me,goodguy);
			me->dx+=Cosine(me->facing)*2;
			me->dy+=Sine(me->facing)*2;
			Dampen(&me->dx,FIXAMT/2);
			Dampen(&me->dy,FIXAMT/2);
			Clamp(&me->dx,FIXAMT*6);
			Clamp(&me->dy,FIXAMT*6);
			HitBadguys(me,map,world);
			FireBullet(me->x,me->y-FIXAMT*20,0,BLT_MISLSMOKE);
			break;
		case BLT_LIGHTNING:
			w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			if(w!=65535)
			{
				bulletHittingType=BLT_LIGHTNING;
				GetGuy(w)->GetShot(0,0,player.wpnLevel*2+1,map,world);
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
		case BLT_SLINGPOW:
			HitBadguys(me,map,world);
			break;
		case BLT_WATER:
			if(me->anim==0)
			{
				for(w=0;w<3;w++)
				{
					you=FireBullet(me->x-me->dx/2,me->y-me->dy/2,me->facing,BLT_WATER);
					if(you)
					{
						you->x+=-FIXAMT*8+Random(FIXAMT*16);
						you->y+=-FIXAMT*8+Random(FIXAMT*16);
						you->z=me->z-me->dz-FIXAMT*4+Random(FIXAMT*8);
						you->anim=1;
					}
				}
				me->anim=1;
			}
			HitBadguys(me,map,world);
			me->dz-=FIXAMT/2;
			break;
		case BLT_FOUNTAIN:
			FireBullet(me->x,me->y,me->facing,BLT_WATER);
			break;
		case BLT_PORTAL:
			switch(me->target)
			{
				case 0:
					SetNoSaving(true);
					ShakeScreen(10);
					// lighting up around the edges
					if(me->timer==20)
					{
						MakeNormalSound2(SND_ZOMBIESDEAD);
						MakeNormalSound2(SND_GRATE);
						me->timer=30;
						me->facing++;
						if(me->facing==12)
						{
							me->target=1;
						}
					}
					break;
				case 1:
					SetNoSaving(true);
					ShakeScreen(10);
					// shooting into the center
					if(me->timer==20)
					{
						MakeNormalSound2(SND_GRATE);
						me->timer=30;
						me->facing++;
						if(me->facing==24)
						{
							me->target=2;
							player.var[VAR_PORTALOPEN]=1;
							PlayerSetVar(VAR_QUESTDONE+QUEST_BUSTS,1);
						}
					}
					break;
				case 2:
					SetNoSaving(false);
					// removing the zappers
					if(me->timer==20)
					{
						me->timer=25;
						me->facing++;
						if(me->facing==36)
						{
							me->type=0;
						}
					}
					break;
			}
			if(me->anim==0)
			{
				if(me->facing<12)
				{
					for(w=0;w<=me->facing;w++)
					{
						BustLightning(bustOrder[w],bustOrder[w+1],me->facing*4,map);
					}
				}
				else
				{
					if(me->target<2)
					{
						// stop doing the circle lightning when the portal is forming
						for(w=0;w<=11;w++)
						{
							BustLightning(bustOrder[w],bustOrder[w+1],48,map);
						}
					}
					if(me->facing<24)
					{
						for(w=0;w<=me->facing-12;w++)
						{
							BustLightning(bustOrder[w],32,48,map);
						}
					}
					else
					{
						for(w=me->facing-24;w<=11;w++)
						{
							BustLightning(bustOrder[w],32,48,map);
						}
					}
				}
			}
			me->anim++;
			if(me->anim==2)
				me->anim=0;
			break;
		case BLT_EVILFACE:
			me->anim+=(byte)Random(3);
			if(me->anim>=6*16)
				me->anim=6*16-1;
			me->dx+=-FIXAMT/16+Random(FIXAMT/8);
			me->dy-=Random(FIXAMT/16);
			break;
		case BLT_WOLFSHOCK:
			w=LockOnEvil3(me->x>>FIXSHIFT,me->y>>FIXSHIFT,(player.fireRange+1)*TILE_HEIGHT);
			if(w!=65535)
			{
				bulletHittingType=BLT_WOLFSHOCK;
				GetGuy(w)->GetShot(0,0,(player.firePower+2)/3,map,world);
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
		case BLT_SUMSHOCK:
			w=LockOnEvil3(me->x>>FIXSHIFT,me->y>>FIXSHIFT,(player.firePower+1)*TILE_HEIGHT);
			if(w!=65535)
			{
				bulletHittingType=BLT_SUMSHOCK;
				GetGuy(w)->GetShot(0,0,1,map,world);
				LightningBolt(me->x,me->y-FIXAMT*20,GetGuy(w)->x,GetGuy(w)->y-FIXAMT*10-GetGuy(w)->z);
				MakeSound(SND_ZAP,GetGuy(w)->x,GetGuy(w)->y,SND_CUTOFF,1000);
			}
			else
			{
				// make sizzle around ghost if there was no target
				LightningBolt(
					me->x-FIXAMT*32+Random(FIXAMT*64),
					me->y-FIXAMT*52+Random(FIXAMT*64),
					me->x-FIXAMT*32+Random(FIXAMT*64),
					me->y-FIXAMT*52+Random(FIXAMT*64));
			}
			me->type=BLT_NONE;	// begone immediately
			break;
		case BLT_CLAW:
			HitBadguys(me,map,world);
			break;
		case BLT_THFKNIFE:
			if(player.fireFlags&FF_GUIDED)
			{
				if(bulControl&CONTROL_UP)
					me->dy-=FIXAMT;
				if(bulControl&CONTROL_DN)
					me->dy+=FIXAMT;
				if(bulControl&CONTROL_LF)
					me->dx-=FIXAMT;
				if(bulControl&CONTROL_RT)
					me->dx+=FIXAMT;
				Clamp(&me->dx,FIXAMT*16);
				Clamp(&me->dy,FIXAMT*16);
				CalcBulletFacing(me);
			}
			if(me->anim)
				me->anim--;
			else
			{
				me->anim=1;
				if((player.fireFlags&FF_HOMING) || player.spellXP[ITM_WBOOMERANG-ITM_WBOMB])
				{
					word w;

					w=LockOnEvilNotYou(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->target);
					if(GetGuyPos(w,&mapx,&mapy))
					{
						if(me->x>mapx)
							me->dx-=FIXAMT;
						else
							me->dx+=FIXAMT;
						if(me->y>mapy)
							me->dy-=FIXAMT;
						else
							me->dy+=FIXAMT;

						Clamp(&me->dx,FIXAMT*14);
						Clamp(&me->dy,FIXAMT*14);
					}
					CalcBulletFacing(me);
				}
				else
				{
					me->dx=Cosine(me->facing)*12;
					me->dy=Sine(me->facing)*12;
				}
				if(player.spellXP[ITM_WHOTPANTS-ITM_WBOMB] && Random(3)==0)
				{
					FireBullet(me->x,me->y,(byte)Random(256),BLT_THFFIRE);
				}
			}

			HitBadguys(me,map,world);
			break;
		case BLT_AURA:
			me->facing+=4;
			me->x=goodguy->x+Cosine(me->facing)*20;
			me->y=goodguy->y+Sine(me->facing)*20;
			break;
	}
}

void RenderBullet(bullet_t *me)
{
	sprite_t *curSpr;

	switch(me->type)
	{
		case BLT_SUMHEAL:
			RenderSumHeal(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,me->bright);
			break;
		case BLT_EVILFACE:
			curSpr=bulletSpr->GetSprite(me->anim/16+137);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GHOST);
			break;
		case BLT_LOONYBALL:
		case BLT_BOWLINGBALL:
			if(opt.cheats[CH_KICKCAT])
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,ITM_CAT,0,me->bright);
			else
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,ITM_MYSORB,0,me->bright);
			break;
		case BLT_ENERGY:
			curSpr=bulletSpr->GetSprite(me->anim+10);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BIGSHOT:
			curSpr=bulletSpr->GetSprite(me->anim+10);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			// extra deals
			SprDraw((me->x+Cosine(me->facing)*10)>>FIXSHIFT,
				(me->y+Sine(me->facing)*10)>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw((me->x+Cosine(me->facing)*10)>>FIXSHIFT,
				(me->y+Sine(me->facing)*10)>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw((me->x+Cosine(me->facing+86)*10)>>FIXSHIFT,
				(me->y+Sine(me->facing+86)*10)>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw((me->x+Cosine(me->facing+86)*10)>>FIXSHIFT,
				(me->y+Sine(me->facing+86)*10)>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw((me->x+Cosine(me->facing+172)*10)>>FIXSHIFT,
				(me->y+Sine(me->facing+172)*10)>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw((me->x+Cosine(me->facing+172)*10)>>FIXSHIFT,
				(me->y+Sine(me->facing+172)*10)>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BALLLIGHTNING:
		case BLT_PSDSHOT:
		case BLT_WITCHBLAST:
		case BLT_WITCHBUZZSAW:

			if(opt.cheats[CH_FROGWPN])
			{
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,ITM_FROGDOLL,0,me->bright);
			}
			break;
		case BLT_DEVILDOLL:
			RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,ITM_BATDOLL+me->anim,0,me->bright);
			break;
		case BLT_FLAMEWALL:
			curSpr=bulletSpr->GetSprite(me->anim/2);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_FLAMESHOT:
			curSpr=bulletSpr->GetSprite(me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
			curSpr=bulletSpr->GetSprite(19-me->anim/2);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_GASBLAST:
			curSpr=bulletSpr->GetSprite(9-me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_MEGABOOM:
		case BLT_GOODBOOM2:
		case BLT_SUMBOOM:
			curSpr=bulletSpr->GetSprite(20+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ITEM:
			if(me->timer<30 && (me->timer&1))
				return;
			if(opt.cheats[CH_FROGWPN])
			{
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,ITM_FROGDOLL,0,me->bright);
			}
			else
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,me->anim,0,me->bright);
			break;
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
		case BLT_SUMGAS:
		case BLT_BATGAS:
			curSpr=bulletSpr->GetSprite(164-(me->anim/2));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_WITCHGAS:
			curSpr=bulletSpr->GetSprite(164-(me->anim/2));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_CACTUS:
			curSpr=bulletSpr->GetSprite((me->facing/16)+30);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BOOMERANG:
			curSpr=bulletSpr->GetSprite(me->anim+46);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_ICE:
		case BLT_ICE2:
		case BLT_SUMFROST:
		case BLT_WITCHICE:
			curSpr=bulletSpr->GetSprite((me->facing/16)+62);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BATSHOT:
			curSpr=bulletSpr->GetSprite((me->facing/16)+62);
			OffSprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,0,4,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_WOLFSHOT:
			curSpr=bulletSpr->GetSprite((me->facing/16)+62);
			OffSprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,0,6,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_BOMB:
			curSpr=bulletSpr->GetSprite(95-me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_GOODBOOM:
		case BLT_BADBOOM:
			curSpr=bulletSpr->GetSprite(78+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_BOWLBOOM:
			break;
		case BLT_SWAMPGAS:
		case BLT_SWAMPGAS2:
		case BLT_MISLSMOKE:
			if(me->anim>0)
			{
				curSpr=bulletSpr->GetSprite(85+me->anim/2);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
						DISPLAY_DRAWME|DISPLAY_GLOW);
			}
			// else don't draw it at all
			break;
		case BLT_HOTPANTS:
		case BLT_SUMFLAME:
		case BLT_THFFIRE:
			curSpr=bulletSpr->GetSprite(me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ORBGRENADE:
			RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,ITM_MYSORB,0,me->bright);
			break;
		case BLT_MISSILE:
			curSpr=bulletSpr->GetSprite(me->facing/16+96);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_SLINGPOW:
		case BLT_WIND:
			// no image
			break;
		case BLT_ICESHARD:
			curSpr=bulletSpr->GetSprite((me->facing/16)+30);
			OffSprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,5,0,me->bright+6,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_KNIFESHRP:
			curSpr=bulletSpr->GetSprite((me->facing/16)+30);
			OffSprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,5,0,me->bright+14,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_WATER:
			curSpr=bulletSpr->GetSprite(136);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_CLAW:
		case BLT_THFKNIFE:
			curSpr=bulletSpr->GetSprite((me->facing/16)+143);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_CACTUS2:
			curSpr=bulletSpr->GetSprite((me->facing/16)+143);
			OffSprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,0,5,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_AURA:
			curSpr=bulletSpr->GetSprite((me->facing/16)+143);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-8,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
	}
}

void UpdateBullets(Map *map,world_t *world)
{
	int i;
	static byte updFlip=0,thornFlip=0;

	updFlip++;
	if(updFlip==4)
	{
		updFlip=0;
		thornFlip++;

		if(opt.cheats[CH_THORNS] && thornFlip==2)
		{
			thornFlip=0;
			FireBullet(goodguy->x,goodguy->y,0,BLT_AURA);
			FireBullet(goodguy->x,goodguy->y,128,BLT_AURA);
		}
	}

	if(opt.cheats[CH_GUIDED] || player.worldNum==WORLD_BOWLING || player.worldNum==WORLD_LOONYBALL)
		bulControl=GetControls();

	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].type)
		{
			if(opt.cheats[CH_SLOMO] && !GoodBullet(bullet[i].type) && (updFlip&1))
			{
				// don't update
			}
			else
			{
				if(GoodBullet(bullet[i].type))
					UpdateBullet(&bullet[i],map,world);
				else
				{
					if(player.difficulty!=DIFF_BEGINNER || updFlip!=0)
						UpdateBullet(&bullet[i],map,world);
					if(player.difficulty==DIFF_LOONY && updFlip==0)
						UpdateBullet(&bullet[i],map,world);
				}
			}

			// vintage = double speed
			if(opt.cheats[CH_VINTAGE] && bullet[i].type)
				UpdateBullet(&bullet[i],map,world);

			// terror = double speed for ENEMY bullets
			if((player.cheatsOn&PC_TERROR) && bullet[i].type && !GoodBullet(bullet[i].type))
				UpdateBullet(&bullet[i],map,world);

			// quick = double speed for FRIENDLY
			if((opt.cheats[CH_QUICK]) && bullet[i].type && GoodBullet(bullet[i].type))
				UpdateBullet(&bullet[i],map,world);
		}
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
	int i;

	me->type=type;
	me->x=x;
	me->y=y;
	me->facing=facing;
	me->bright=0;
	me->dx=0;
	me->dy=0;
	me->dz=0;

	switch(me->type)
	{
		case BLT_SUMHEAL:
			me->target=65535;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*6;
			me->dy=Sine(me->facing)*6;
			me->timer=120;
			me->anim=0;
			break;
		case BLT_WITCHPLAGUE:
			me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->timer=60+SpellLevel(SPL_PLAGUE)*15;
			me->anim=0;
			break;
		case BLT_WITCHGAS:
			me->anim=0;
			me->timer=80;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			break;
		case BLT_WITCHICE:
			me->anim=0;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->z=FIXAMT*20;
			me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			me->timer=60;
			break;
		case BLT_WITCHSHOCK:
			me->timer=(SpellLevel(SPL_SHOCK)+1)*90;
			break;
		case BLT_WITCHSPEED:
			me->timer=(SpellLevel(SPL_SPEED)+1)*60;
			break;
		case BLT_WITCHKABOOM:
			me->timer=60;
			break;
		case BLT_WITCHBLAST:
			if(player.monsType==MONS_PLYRSUMMON)
			{
				me->timer=6*3;
			}
			else
			{
				if(!opt.cheats[CH_HEAVYHIT])
					me->timer=(SpellLevel(SPL_BLAST)+3)*3;
				else
					me->timer=6*3;
			}

			if(player.fireFlags&FF_GUIDED)
				me->timer=30*3;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*11;
			me->dy=Sine(me->facing)*11;
			me->dz=0;
			if(player.fireFlags&FF_HOMING)
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			break;
		case BLT_DEVILDOLL:
			me->timer=6*3;
			if(player.fireFlags&FF_GUIDED)
				me->timer=30*3;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*11;
			me->dy=Sine(me->facing)*11;
			me->dz=-FIXAMT+Random(FIXAMT*6);
			if(opt.cheats[CH_FROGWPN])
				me->anim=2;
			else
				me->anim=(byte)Random(8);
			if(player.fireFlags&FF_HOMING)
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			break;
		case BLT_WITCHBUZZSAW:
			me->timer=SpellLevel(SPL_BUZZSAW)*30+60;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_EVILFACE:
			me->anim=0;
			me->dx=-FIXAMT+Random(FIXAMT*2);
			me->dy=-FIXAMT-Random(FIXAMT*2);
			me->dz=0;
			me->z=40*FIXAMT;
			me->timer=30*10;
			break;
		case BLT_LOONYBALL:
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->z=50*FIXAMT;
			me->timer=100;
			me->target=65535;
			ballSoundClock=0;
			break;
		case BLT_BOWLINGBALL:
			me->dx=Cosine(me->facing)*8+goodguy->dx;
			me->dy=Sine(me->facing)*8+goodguy->dy;
			me->dz=0;
			me->z=FIXAMT*20;
			me->timer=30*4;
			break;
		case BLT_BALLLIGHTNING:
			if(!opt.cheats[CH_HEAVYHIT])
				me->timer=(player.fireRange+3)*3;
			else
				me->timer=6*3;

			if(player.fireFlags&FF_GUIDED)
				me->timer=30*3;
			me->z=FIXAMT*20;
			// this uses only 8-way facings
			me->dx=Cosine(me->facing*32)*11;
			me->dy=Sine(me->facing*32)*11;
			me->dz=0;
			me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			break;
		case BLT_PSDSHOT:
			if(!opt.cheats[CH_HEAVYHIT])
				me->timer=(player.fireRange+3)*3;
			else
				me->timer=6*3;

			if(player.fireFlags&FF_GUIDED)
				me->timer=30*3;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*11;
			me->dy=Sine(me->facing)*11;
			me->dz=0;
			if(player.fireFlags&FF_HOMING)
				me->target=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
			break;
		case BLT_AURA:
			me->anim=0;
			me->timer=32;
			me->z=0;
			me->dx=0;
			me->dy=0;
			break;
		case BLT_ENERGY:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			break;
		case BLT_BIGSHOT:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*6;
			me->dy=Sine(me->facing)*6;
			me->dz=0;
			break;
		case BLT_FLAMEWALL:
			me->anim=0;
			me->timer=12;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			MakeSound(SND_FIREBURN,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_FLAMESHOT:
			me->anim=3;
			me->timer=60;
			me->z=0;
			me->dx=Cosine(me->facing)*7;
			me->dy=Sine(me->facing)*7;
			me->dz=0;
			MakeSound(SND_FIREBURN,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_EARTHSPIKE:
		case BLT_EARTHSPIKE2:
			me->anim=0;
			me->timer=30;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_GASBLAST:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->dz=0;
			break;
		case BLT_MEGABOOM:
			MakeSound(SND_BIGBOOM,me->x,me->y,SND_CUTOFF,1200);
			me->anim=0;
			me->timer=10;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_ITEM:
			me->anim=facing;
			me->timer=30*5;
			me->z=FIXAMT*20;
			me->facing=(byte)Random(256);
			me->bright=(byte)Random(5);
			me->dx=Cosine(me->facing)*me->bright;
			me->dy=Sine(me->facing)*me->bright;
			me->dz=Random(FIXAMT*4);
			if(me->anim>=ITM_BATDOLL && me->anim<=ITM_WOLFDOLL)
			{
				me->bright=(byte)Random(3);
				me->timer=30*20;
				me->dx=Cosine(me->facing)*me->bright;
				me->dy=Sine(me->facing)*me->bright;
			}
			me->bright=0;
			break;
		case BLT_WHOOPEE:
		case BLT_WHOOPEE2:
		case BLT_WHOOPEE3:
			me->anim=0;
			me->timer=60*(type-BLT_WHOOPEE+1);
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*2;
			me->dy=Sine(me->facing)*2;
			me->dz=0;
			break;
		case BLT_BATGAS:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->facing=(byte)Random(256);
			me->dx=Cosine(me->facing)*2;
			me->dy=Sine(me->facing)*2;
			me->dz=0;
			break;
		case BLT_SUMGAS:
			me->anim=0;
			me->timer=30+player.firePower*10;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->dz=0;
			break;
		case BLT_CACTUS:
		case BLT_CACTUS2:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			me->dz=0;
			break;
		case BLT_KNIFESHRP:
			me->target=0;
			me->anim=0;
			me->timer=20;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			me->dz=0;
			break;
		case BLT_ICESHARD:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->dz=0;
			break;
		case BLT_BOOMERANG:
			me->target=-1;
			me->anim=0;
			me->timer=100;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			break;
		case BLT_ICE:
		case BLT_ICE2:
			me->target=0;
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->dz=0;
			break;
		case BLT_SUMFROST:
			me->target=0;
			me->anim=0;
			me->timer=30+player.firePower*10;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*10;
			me->dy=Sine(me->facing)*10;
			me->dz=0;
			break;
		case BLT_BATSHOT:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->dz=0;
			break;
		case BLT_WOLFSHOT:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*16;
			me->dy=Sine(me->facing)*16;
			me->dz=0;
			break;
		case BLT_BOMB:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*30;
			me->dx=Cosine(me->facing)*6;
			me->dy=Sine(me->facing)*6;
			me->dz=0;
			break;
		case BLT_GOODBOOM:
		case BLT_BADBOOM:
			MakeSound(SND_BOOM,me->x,me->y,SND_CUTOFF,1200);
			me->anim=0;
			me->timer=6;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_BOWLBOOM:
			me->anim=0;
			me->timer=6;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_GOODBOOM2:
		case BLT_SUMBOOM:
			MakeSound(SND_BIGBOOM,me->x,me->y,SND_CUTOFF,1200);
			me->anim=0;
			me->timer=9;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_SWAMPGAS:
		case BLT_SWAMPGAS2:
			me->anim=0;
			me->timer=3;
			me->z=FIXAMT*2;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_MISLSMOKE:
			me->anim=0;
			me->timer=7;
			me->z=FIXAMT*2;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_HOTPANTS:
			me->anim=0;
			me->timer=20+player.wpnLevel*6;
			me->z=0;
			me->dx=Cosine(me->facing)*8;
			me->dy=Sine(me->facing)*8;
			me->dz=0;
			me->target=player.wpnLevel;
			MakeSound(SND_FIREBURN,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_THFFIRE:
			me->anim=0;
			me->timer=30;
			me->z=0;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			me->dz=0;
			me->target=0;
			MakeSound(SND_FIREBURN,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_SUMFLAME:
			me->anim=0;
			me->timer=20+player.firePower*6;
			me->z=0;
			me->dx=Cosine(me->facing)*(5+player.firePower/2);
			me->dy=Sine(me->facing)*(5+player.firePower/2);
			me->dz=0;
			MakeSound(SND_FIREBURN,me->x,me->y,SND_CUTOFF,600);
			break;
		case BLT_MISSILE:
		case BLT_WIND:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_ORBGRENADE:
			me->anim=0;
			me->timer=120;
			me->z=FIXAMT*60;
			i=Random(8)+1;
			me->dx=Cosine(me->facing)*i;
			me->dy=Sine(me->facing)*i;
			me->dz=20*FIXAMT;
			break;
		case BLT_LIGHTNING:
		case BLT_WOLFSHOCK:
		case BLT_SUMSHOCK:
			me->timer=5;
			break;
		case BLT_SLINGPOW:
			me->timer=3;
			break;
		case BLT_FOUNTAIN:
			MakeSound(SND_WATERSPURT,me->x,me->y,SND_CUTOFF,200);
			me->timer=15;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_WATER:
			me->z=0;
			me->timer=60;
			me->anim=0;
			switch(me->facing)
			{
				case 0:
					me->dx=FIXAMT*19;
					me->dy=0;
					me->dz=FIXAMT*8;
					break;
				case 1:
					me->dx=0;
					me->dy=FIXAMT*17;
					me->dz=FIXAMT*6;
					break;
				case 2:
					me->dx=-FIXAMT*19;
					me->dy=0;
					me->dz=FIXAMT*8;
					break;
				case 3:
					me->dx=0;
					me->dy=- FIXAMT*17;
					me->dz=FIXAMT*6;
					break;
			}
			break;
		case BLT_PORTAL:
			me->anim=0;
			me->timer=30;
			me->target=0;
			me->facing=0;
			break;
		case BLT_CLAW:
			me->anim=0;
			me->timer=30*5;
			me->z=FIXAMT*20;
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->dz=0;
			break;
		case BLT_THFKNIFE:
			me->target=65535;
			me->anim=0;
			me->timer=30+30*(player.spellXP[ITM_WBOOMERANG-ITM_WBOMB]>0);
			me->z=FIXAMT*20;
			if(player.spellXP[ITM_WBOOMERANG-ITM_WBOMB])
			{
				me->dx=Cosine(me->facing)*8;
				me->dy=Sine(me->facing)*8;
			}
			else
			{
				me->dx=Cosine(me->facing)*12;
				me->dy=Sine(me->facing)*12;
			}
			me->dz=0;
			break;
	}
}

bullet_t *FireBullet(int x,int y,byte facing,byte type)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type);
			return &bullet[i];
			break;
		}
	return NULL;
}

void FireBulletZ(int x,int y,int z,byte facing,byte type)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,facing,type);
			bullet[i].z=z;
			break;
		}
}

// this only fires if there is room in the bullet list PAST a specific point
// this is used for the Megabeam to ensure that all the laser bits stay lined up nicely
void FireBulletAfter(int x,int y,byte facing,byte type,bullet_t *thisone)
{
	int i,j,start=MAX_BULLETS;

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

byte Intersect(int rx,int ry,int rx2,int ry2,int r2x,int r2y,int r2x2,int r2y2)
{
	return (rx<r2x2 && rx2>r2x && ry<r2y2 && ry2>r2y);
}

void GetKicked(bullet_t *me)
{
	int myx,myy,myx2,myy2;
	int youx,youy,youx2,youy2;
	byte c;

	ScoreGoal(me,curMap);

	if(me->z>FIXAMT*40)
		return;	// too high to be kicked!

	if(me->anim<10)
		return;

	if(me->z==0 && me->dx==0 && me->dy==0)
	{
		if(curWorld.terrain[curMap->map[(me->x/TILE_WIDTH)/FIXAMT+((me->y/TILE_HEIGHT)/FIXAMT)*curMap->width].floor].flags&TF_WATER)
		{
			player.oob++;
			NewMessage("Out of bounds!",60);
			MakeNormalSound(SND_PENALTYOPEN);
			me->type=BLT_NONE;
			FireBullet(((curMap->width/2)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				((curMap->height/2)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_LOONYBALL);
			return;
		}
	}

	myx=(me->x>>FIXSHIFT)-8;
	myx2=(me->x>>FIXSHIFT)+8;
	myy=(me->y>>FIXSHIFT)-8;
	myy2=(me->y>>FIXSHIFT)+8;

	youx=goodguy->rectx+goodguy->x/FIXAMT;
	youx2=goodguy->rectx2+goodguy->x/FIXAMT;
	youy=goodguy->recty+goodguy->y/FIXAMT;
	youy2=goodguy->recty2+goodguy->y/FIXAMT;

	if(Intersect(myx,myy,myx2,myy2,youx,youy,youx2,youy2))
	{
		c=bulControl;

		me->dx/=2;
		me->dy/=2;
		if(c&CONTROL_B2)
		{
			if(opt.cheats[CH_KICKCAT])
				MakeSound(SND_CATKICK,me->x,me->y,SND_CUTOFF,300);
			else
				MakeSound(SND_BALLKICK,me->x,me->y,SND_CUTOFF,300);
			me->dx+=goodguy->dx*3/2;
			me->dy+=goodguy->dy*3/2;
			me->dz=FIXAMT*10;
			me->anim=0;
			ballSteerClock=60;
		}
		else
		{
			if(ballSoundClock==0)
			{
				if(opt.cheats[CH_KICKCAT])
					MakeSound(SND_CATDRIBBLE+Random(2),me->x,me->y,SND_CUTOFF,300);
				else
					MakeSound(SND_BALLDRIBBLE,me->x,me->y,SND_CUTOFF,300);
				ballSoundClock=5;
			}
			me->dx=goodguy->dx;
			me->dy=goodguy->dy;
		}
		me->x+=me->dx;
		if(!Bulletable(me,curMap,(me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT))
			BulletHitWallX(me,curMap,&curWorld);
		me->y+=me->dy;
		if(!Bulletable(me,curMap,(me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT))
			BulletHitWallY(me,curMap,&curWorld);
	}

	// bounce off other players
	c=BallHitCheck(me);
	if(c)
	{
		if(ballSoundClock==0)
		{
			if(opt.cheats[CH_KICKCAT])
				MakeSound(SND_CATDRIBBLE+Random(2),me->x,me->y,SND_CUTOFF,300);
			else
				MakeSound(SND_BALLDRIBBLE,me->x,me->y,SND_CUTOFF,300);
			ballSoundClock=5;
		}
		if(c==10)
		{
			// couldn't determine an angle
			me->facing=(byte)Random(256);
			me->dx/=2;
			me->dy/=2;
			me->dx+=Cosine(me->facing)*4;
			me->dy+=Sine(me->facing)*4;
		}
		else
		{
			c--;
			if(c!=2 && c!=6)
			{
				me->dx=-me->dx*3/4;
			}
			if(c!=0 && c!=4)
			{
				me->dy=-me->dy*3/4;
			}
		}
	}
}

void GetBallCoords(int *x,int *y,int *z)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==BLT_LOONYBALL)
		{
			*x=bullet[i].x;
			*y=bullet[i].y;
			*z=bullet[i].z;
			return;
		}
	}
}

void CatchBall(word id)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==BLT_LOONYBALL)
		{
			bullet[i].dx=0;
			bullet[i].dy=0;
			bullet[i].dz=0;
			bullet[i].target=id;
			ballSteerClock=0;
			return;
		}
	}
}

void ThrowBall(void)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==BLT_LOONYBALL)
		{
			if(opt.cheats[CH_KICKCAT])
				MakeSound(SND_CATKICK,bullet[i].x,bullet[i].y,SND_CUTOFF,300);
			else
				MakeSound(SND_BALLKICK,bullet[i].x,bullet[i].y,SND_CUTOFF,300);
			bullet[i].dz=FIXAMT*6;
			bullet[i].dy=FIXAMT*10;
			bullet[i].dx=-FIXAMT*4+Random(FIXAMT*8);
			bullet[i].target=65535;
			ballSteerClock=0;
			return;
		}
	}
}

bullet_t *FindItemBullet(int x,int y)
{
	int i,best,bestdist,d;

	best=-1;
	bestdist=9999999;
	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==BLT_ITEM)
		{
			d=abs(bullet[i].x-x)+abs(bullet[i].y-y);
			if(best==-1 || d<bestdist)
			{
				best=i;
				bestdist=abs(bullet[i].x-x)+abs(bullet[i].y-y);
			}

		}
	}
	if(best==-1)
		return NULL;
	else
		return &bullet[best];
}
