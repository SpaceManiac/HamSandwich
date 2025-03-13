#include "bullet.h"
#include "guy.h"
#include "player.h"
#include "fairy.h"
#include "spell.h"
#include "challenge.h"
#include "skills.h"

#define SPR_FLAME   0
#define SPR_LASER   5
#define SPR_HAMMER  21
#define SPR_MISSILE 21
#define SPR_SMOKE	37
#define SPR_ACID	44
#define SPR_BOMB	100
#define SPR_ENERGY	108
#define SPR_BOOM	110
#define SPR_MEGABEAM 118
#define SPR_SPORE	126
#define SPR_SHROOM  130
#define SPR_GRENADE 138
#define SPR_YELBOOM 140
#define SPR_SHOCKWAVE 145
#define SPR_LILBOOM 149
#define SPR_SNOWBALL 154
#define SPR_BIGSNOW  155
#define SPR_ICESPIKE 158
#define SPR_ROCK	 162
#define SPR_SPINE	 166
#define SPR_FIREBALL 182
#define SPR_WAVE	 223
#define SPR_LIQUIFY  231
#define SPR_ICEBEAM  238
#define SPR_SKULL	 243
#define SPR_COMET	 271
#define SPR_COMETBOOM 279

bullet_t bullet[MAX_BULLETS];
sprite_set_t *bulletSpr;
byte bulletHittingType;
byte flameDmgTick;

void InitBullets(void)
{
	bulletSpr=new sprite_set_t("graphics/bullets.jsp");

	flameDmgTick = 0;

	memset(bullet,0,MAX_BULLETS*sizeof(bullet_t));
}

void ExitBullets(void)
{
	delete bulletSpr;
}

byte Bulletable(Map *map,int x,int y)
{
	if(x<0 || y<0 || x>=map->width || y>=map->height || map->map[x+y*map->width].wall ||
		map->map[x+y*map->width].item>=MAX_SHOOTABLE_ITMS)
		return 0;

	return 1;
}
void BurnTreesInArea(Map *map,int x, int y, int x2, int y2)
{
	x /= FIXAMT;
	y /= FIXAMT;
	x2 /= FIXAMT;
	y2 /= FIXAMT;
	int i = x, j = y;
	// we loop through each tile in the box, making sure we grab the end in case that's a new tile
	while (1)
	{
		i = x;
		while (1)
		{
			int tx, ty;
			tx = i / TILE_WIDTH;
			ty = j / TILE_HEIGHT;
			mapTile_t* tile = map->GetTile(tx,ty);
			if (tile && tile->item == ITM_TREE)
			{
				tile->item = ITM_BURNEDTREE;
				for (int i = 0; i < 6; i++)
					FireBullet((tx*TILE_WIDTH+TILE_WIDTH/2 - 24 + Random(48))*FIXAMT, (ty*TILE_HEIGHT+TILE_HEIGHT/2 - 16 + Random(32))*FIXAMT, 0, BLT_FAKELIQUIFY);
			}

			if (i == x2)
				break;
			i += TILE_WIDTH;
			if (i > x2)
				i = x2;
		}
		if (j == y2)
			break;
		j += TILE_HEIGHT;
		if (j > y2)
			j = y2;
	}
	
}

byte BulletCanGo(int xx,int yy,Map *map,byte size)
{
	bool result;
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

	return (byte)result;
}

void BulletHitWallX(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_ICEBEAM2:
		case BLT_DEATHBEAM2:
		case BLT_COMET:
		case BLT_COMETBOOM:
		case BLT_COMETBOOM2:
		case BLT_ICECOMET:
		case BLT_ICECOMETBOOM:
			break;
		case BLT_HAMMER:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_REDFBALL:
			me->type = BLT_NONE;
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			break;
		case BLT_SKULL:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->x-=me->dx;
				me->dx=-me->dx;
				me->lastHit=65535;
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
		case BLT_MINIFBALL:
		case BLT_PTEROSHOT:
		case BLT_YELWAVE:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,2);
			break;
		case BLT_ICESHARD:
			me->type = BLT_NONE;
			ExplodeParticles(PART_SNOW2, me->x, me->y, me->z, 2);
			break;
		case BLT_HAMMER2:
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,3);
		case BLT_EVILHAMMER:	// reflects off walls
			me->x-=me->dx;
			me->dx=-me->dx;
			me->lastHit=65535;
			me->facing=((byte)(4-me->facing))&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_COIN:	// reflects off walls
		case BLT_BIGCOIN:
		case BLT_BIGYELLOW:
		case BLT_RUNESTONE:
		case BLT_LIFEPOTION:
		case BLT_MANAPOTION:
			me->x-=me->dx;
			me->dx=-me->dx;
			me->facing=((byte)(4-me->facing))&7;
			//MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900); // ching!
			break;
		case BLT_GRENADE:
		case BLT_ICECLOUD:
		case BLT_MINE:
		case BLT_GLOB_BOMB:
			me->x-=me->dx;
			break;
		case BLT_LASER:	// reflects off walls
		case BLT_LASER2:
			me->x-=me->dx;
			me->dx=-me->dx;
			me->dy+=-FIXAMT/4+MGL_random(FIXAMT/2);
			me->facing=((byte)(8-me->facing))&15;
			break;
		case BLT_BOMB:
			me->x-=me->dx;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			BurnTreesInArea(map, me->x - 8*FIXAMT, me->y - 8*FIXAMT, me->x + 8*FIXAMT, me->y + 8*FIXAMT);
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
		case BLT_ICEBEAM:
			me->timer=8;
			me->type=BLT_ICEBEAM2;
			break;
		case BLT_DEATHBEAM:
			me->timer=8;
			if(MGL_random(3)==0)
				me->type=BLT_DEATHBEAM2;
			else
				me->type=BLT_NONE;
			break;
		case BLT_MISSILE:
			if(ClassicMode())
				BulletRanOut(me, map, world);
			else
			{
				int c = SkillValue(SKILL_SEEKBOUNCE);
				if (MGL_random(100) < c)
				{
					me->x -= me->dx;
					me->dx = -me->dx;
					me->facing = ((byte)(8 - me->facing)) & 15;
				}
				else
					BulletRanOut(me, map, world);
			}
			break;
		default:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletHitWallY(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_ICEBEAM2:
		case BLT_DEATHBEAM2:
		case BLT_COMET:
		case BLT_COMETBOOM:
		case BLT_COMETBOOM2:
		case BLT_ICECOMET:
		case BLT_ICECOMETBOOM:
			break;
		case BLT_HAMMER:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,8);
			MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_REDFBALL:
			me->type = BLT_NONE;
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			break;
		case BLT_SKULL:
			if(player.hammerFlags&HMR_REFLECT)
			{
				me->lastHit=65535;
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
		case BLT_MINIFBALL:
		case BLT_PTEROSHOT:
		case BLT_YELWAVE:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,2);
			break;
		case BLT_ICESHARD:
			me->type = BLT_NONE;
			ExplodeParticles(PART_SNOW2, me->x, me->y, me->z, 2);
			break;
		case BLT_HAMMER2:
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,3);
		case BLT_EVILHAMMER:	// reflects off walls
			me->lastHit=65535;
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900);
			break;
		case BLT_COIN:	// reflects off walls
		case BLT_BIGCOIN:
		case BLT_BIGYELLOW:
		case BLT_RUNESTONE:
		case BLT_MANAPOTION:
		case BLT_LIFEPOTION:
			me->y-=me->dy;
			me->dy=-me->dy;
			me->facing=(8-me->facing)&7;
			//MakeSound(SND_HAMMERREFLECT,me->x,me->y,SND_CUTOFF,900); ching!
			break;
		case BLT_GRENADE:
		case BLT_ICECLOUD:
		case BLT_MINE:
		case BLT_GLOB_BOMB:
			me->y-=me->dy;
			break;
		case BLT_LASER:
		case BLT_LASER2:
			me->y-=me->dy;
			me->dy=-me->dy;
			me->dx+=-FIXAMT/4+MGL_random(FIXAMT/2);
			me->facing=(16-me->facing)&15;
			break;
		case BLT_BOMB:
			me->y-=me->dy;
			break;
		case BLT_FLAME:
		case BLT_FLAME2:
			BurnTreesInArea(map, me->x - 8*FIXAMT, me->y - 8*FIXAMT, me->x + 8*FIXAMT, me->y + 8*FIXAMT);
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
		case BLT_ICEBEAM:
			me->timer=8;
			me->type=BLT_ICEBEAM2;
			break;
		case BLT_DEATHBEAM:
			me->timer=8;
			if(MGL_random(3)==0)
				me->type=BLT_DEATHBEAM2;
			else
				me->type=BLT_NONE;
			break;
		case BLT_MISSILE:
			if (ClassicMode())
				BulletRanOut(me, map, world);
			else
			{
				int c = SkillValue(SKILL_SEEKBOUNCE);
				if (MGL_random(100) < c)
				{
					me->y -= me->dy;
					me->dy = -me->dy;
					me->facing = (16 - me->facing) & 15;
				}
				else
					BulletRanOut(me, map, world);
			}
			break;
		default:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletHitFloor(bullet_t *me,Map *map,world_t *world)
{
	int x,y;

	switch(me->type)
	{
		case BLT_COMET:
			me->type=BLT_COMETBOOM;
			me->anim=0;
			me->timer=18;	// 9 frames of animation
			me->dz=0;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,200);
			break;
		case BLT_ICECOMET:
			me->type = BLT_ICECOMETBOOM;
			me->anim = 0;
			me->timer = 18;	// 9 frames of animation
			me->dz = 0;
			MakeSound(SND_BOMBBOOM, me->x, me->y, SND_CUTOFF, 200);
			break;
		case BLT_ICECOMETBOOM:
		case BLT_COMETBOOM:
		case BLT_COMETBOOM2:
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
		case BLT_COIN:
		case BLT_BIGCOIN:
		case BLT_RUNESTONE:
		case BLT_LIFEPOTION:
		case BLT_MANAPOTION:
			me->dz=-me->dz*7/8;
			me->z=0;
			x=(me->x>>FIXSHIFT)/TILE_WIDTH;
			y=(me->y>>FIXSHIFT)/TILE_HEIGHT;
			if(world->terrain[map->map[x+y*map->width].floor].flags&TF_WATER)
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,8);
				me->type=BLT_NONE;
			}
			else if(player.worldNum==3 && world->terrain[map->map[x+y*map->width].floor].flags&TF_LAVA)
			{
				BlowWigglySmoke(me->x,me->y,0,FIXAMT*4);
				BlowWigglySmoke(me->x,me->y,0,FIXAMT*2);
				BlowWigglySmoke(me->x,me->y,0,FIXAMT*6);
				me->type=BLT_NONE;
			}
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
		case BLT_GLOB_BOMB:
			MakeSound(SND_BOMBBOOM, me->x, me->y, SND_CUTOFF, 950);
			for(int i=0;i<8;i++)
				FireBullet(me->x - FIXAMT * 40 + MGL_randoml(FIXAMT * 81), me->y - FIXAMT * 30 + MGL_randoml(FIXAMT * 61), 0, BLT_SLIME);
			me->type = 0;
			break;
		case BLT_MINE:
			me->dx = 0;
			me->dy = 0;
			me->dz = FIXAMT*40/60;
			me->z = 0;
			me->anim = 1;
			me->timer = 30*30;
			MakeSound(SND_ACIDSPLAT, me->x, me->y, SND_CUTOFF, 950);
			break;
		case BLT_SNOWBALL:
		case BLT_BIGSNOW:
			BulletRanOut(me,map,world);
			break;
		default:
			me->z=0;
			break;
	}
}

void BulletRanOut(bullet_t *me,Map *map,world_t *world)
{
	int i;

	switch(me->type)
	{
		default:
			me->type=BLT_NONE;
			break;
		case BLT_MINIFBALL:
		case BLT_PTEROSHOT:
		case BLT_YELWAVE:
			me->type=BLT_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,2);
			break;
		case BLT_ICESHARD:
			me->type = BLT_NONE;
			ExplodeParticles(PART_SNOW2, me->x, me->y, me->z, 2);
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_BIGYELLOW:
		case BLT_SKULL:
			ExplodeParticles2(PART_YELLOW,me->x,me->y,me->z,6,3);
			me->type=0;
			break;
		case BLT_REDFBALL:
			ExplodeParticles2(PART_HAMMER, me->x, me->y, me->z, 4, 1);
			me->type = 0;
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
		case BLT_LIQUIFY:
		case BLT_LIQUIFY2:
		case BLT_LIQUIFY3:
		case BLT_FAKELIQUIFY:
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
			if (!ClassicMode() && Random(100)<RuneValue(Rune::INFERNO2))
				FireBullet(me->x, me->y, 0, BLT_MINE);
			break;
		case BLT_SHROOM:
			me->x-=me->dx*2;
			me->y-=me->dy*2;
			for(i=0;i<256;i+=8)
			{
				FireExactBullet(me->x,me->y,me->z,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE);
				FireExactBullet(me->x,me->y,me->z,Cosine(i)*6,Sine(i)*6,0,0,16,i,BLT_SPORE);
			}
			AddGuy(me->x,me->y,me->z,MONS_SHROOM);	// become a living shroom
			me->type=BLT_NONE;	// all gone
			MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1000);
			break;
		case BLT_MINE:
			me->type = 0;
			BlowSmoke(me->x, me->y, me->z, FIXAMT);
			break;
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int i,j;

	bulletHittingType=me->type;
	switch(me->type)
	{
		case BLT_COMETBOOM:
		case BLT_COMETBOOM2:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,64,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,10,map,world))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_ICECOMETBOOM:
			if (FindVictims(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 64, (8 - MGL_random(17)) << FIXSHIFT,
				(8 - MGL_random(16)) << FIXSHIFT, -1000, map, world))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_SLIME:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,0,0,2,map,world))
			{
				ExplodeParticles(PART_WATER,me->x,me->y,me->z,4);
			}
			break;
		case BLT_LASER:
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx/2,me->dy/2,1,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_SNOW2,me->x,me->y,me->z,4);
				MakeSound(SND_BULLETHIT,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_LASER2:
			j = (int)(RuneValue(Rune::ENERGY2) / 100.0f);
			if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 8, me->dx / 2, me->dy / 2, j, map, world))
			{
				me->type = BLT_NONE;
				ExplodeParticles(PART_SNOW2, me->x, me->y, me->z, 4);
				MakeSound(SND_BULLETHIT, me->x, me->y, SND_CUTOFF, 900);
			}
			break;
		case BLT_ICECLOUD:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,20,0,0,-1000,map,world))
			{
				// freezing is handled on the victim end
			}
			break;
		case BLT_ICEBEAM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,0,0,-1000,map,world))
			{
				// freeze FX are on the victim end
			}
			break;
		case BLT_DEATHBEAM:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,0,0,5,map,world))
			{
				ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,4);
				MakeSound(SND_MEGABEAMHIT,me->x,me->y,SND_CUTOFF,900);
				me->type=BLT_DEATHBEAM2;
				me->timer=8;
			}
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_SKULL:
		{
			j = player.damage;
			if (player.fairyOn == FAIRY_SMASHY && player.mana)
				j = j * 3 / 2;
			if (!ClassicMode() && RuneValue(Rune::SHOTGUN) > 0)
			{
				j = j / 3;	// less damage for shotgun shots
				if (j < 1) j = 1;
			}
			if (me->lastHit == 65535)
				i = FindVictimNot(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 16, me->dx, me->dy, j, me->lastHit, map, world);
			else
			{
				j /= 2;
				if (j < 1) j = 1;
				i = FindVictimNot(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 16, me->dx, me->dy, j / 2, me->lastHit, map, world);
			}
			if (i != 65535)
			{
				if (player.fairyOn == FAIRY_VAMPY)
				{
					if ((!player.berserk && (MGL_random(2) == 0)) ||
						(player.berserk && (MGL_random(4) == 0)))
						PlayerHeal(1);	// heal 1 pt vampirism, 50% chance normally, 25% chance if berserk
				}
				me->lastHit = i;
				if (player.gear & GEAR_BOUNCY)
				{
					me->facing = (byte)MGL_random(256);
					me->dx = Cosine(me->facing) * 10;
					me->dy = Sine(me->facing) * 10;
					if (me->type == BLT_SKULL)
					{
						me->facing = (me->facing + 8) & 255;
						me->facing /= 16;
					}
					else
					{
						me->facing = (me->facing + 16) & 255;
						me->facing /= 32;
					}
				}
				else
					me->type = BLT_NONE;
				ExplodeParticles(PART_YELLOW, me->x, me->y, me->z, 8);
				MakeSound(SND_HAMMERBONK, me->x, me->y, SND_CUTOFF, 900);
			}
		}
			break;
		case BLT_REDFBALL:
		{
			j = SkillValue(SKILL_FLAMEON)+1;
			i = FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 6, 0,0, j, map, world);
			if (i != 65535)
			{
				me->type = BLT_NONE;
				ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			}
		}
		break;
		case BLT_MINIFBALL:
			j = 3;
			if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 6, me->dx, me->dy, j, map, world))
			{
				me->type = BLT_NONE;
				ExplodeParticles(PART_YELLOW, me->x, me->y, me->z, 4);
				MakeSound(SND_HAMMERBONK, me->x, me->y, SND_CUTOFF, 900);
			}
			break;
		case BLT_ICESHARD:
			if (FindVictimNot(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 6, me->dx, me->dy, me->anim,(word)me->dz, map, world)!=65535)
			{
				me->type = BLT_NONE;
				ExplodeParticles(PART_SNOW2, me->x, me->y, me->z, 2);
				MakeSound(SND_HAMMERBONK, me->x, me->y, SND_CUTOFF, 900);
			}
			break;
		case BLT_PTEROSHOT:
			if (ClassicMode())
				j = 3;
			else
			{
				j = (SkillValue(SKILL_SUMMON) + 3) / 2;
				if (player.summonDmgBoost)
				{
					j = j * (100 + SkillValue(SKILL_HEALSUMMONS));
					if (Random(j % 100) >= 50)
						j += 100;
					j /= 100;
				}
			}
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,me->dx,me->dy,j,map,world))
			{
				me->type=BLT_NONE;
				ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,4);
				MakeSound(SND_HAMMERBONK,me->x,me->y,SND_CUTOFF,900);
			}
			break;
		case BLT_MISSILE:
			i = SpellLevel() / 5;
			if (!ClassicMode())
				i = SkillValue(SKILL_SEEKER)*2;
			if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,i,map,world))
			{
				BulletRanOut(me,map,world);	// detonate
			}
			break;
		case BLT_COIN:
		case BLT_BIGCOIN:
		case BLT_RUNESTONE:
		case BLT_LIFEPOTION:
		case BLT_MANAPOTION:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,0,0,0,map,world))
			{
				if(me->type==BLT_COIN)
				{
					FloaterParticles(me->x,me->y,5,24,0,4);
					GainMoney(1);
					MakeSound(SND_MONEY, me->x, me->y, SND_CUTOFF, 500);
					ChallengeEvent(CE_GET,ITM_COIN);
				}
				else if (me->type == BLT_RUNESTONE)
				{
					if (player.runeStones < MAX_RUNESTONES)
						player.runeStones++;
					MakeSound(SND_CHLGCRYSTAL, me->x, me->y, SND_CUTOFF, 500);
				}
				else if (me->type == BLT_LIFEPOTION)
				{
					PlayerGetItem(ITM_HEALTHPOT, me->x, me->y);
				}
				else if (me->type == BLT_MANAPOTION)
				{
					PlayerGetItem(ITM_MANAPOT, me->x, me->y);
				}
				else
				{
					FloaterParticles(me->x,me->y,5,24,0,4);
					FloaterParticles(me->x,me->y,5,32,-1,4);
					GainMoney(10);
					ChallengeEvent(CE_GET,ITM_BIGCOIN);
					// double noise
					MakeSound(SND_MONEY, me->x, me->y, SND_CUTOFF, 500);
					MakeSound(SND_MONEY,me->x,me->y,SND_CUTOFF,500);
				}
				me->type=BLT_NONE;
				
			}
			if(GetGoodguy() && me->type!=BLT_LIFEPOTION && me->type!=BLT_MANAPOTION)
			{
				if(player.gear&GEAR_MAGNET)
				{
					i=(16-(abs(me->x-GetGoodguy()->x)+abs(me->y-GetGoodguy()->y))/(FIXAMT*TILE_WIDTH));

					if(i>0)
					{
						if(GetGoodguy()->x>me->x)
							me->dx+=i*FIXAMT/128;
						else
							me->dx-=i*FIXAMT/128;
						if(GetGoodguy()->y>me->y)
							me->dy+=i*FIXAMT/128;
						else
							me->dy-=i*FIXAMT/128;
					}
				}
				if(player.fairyOn==FAIRY_GRABBY)
				{
					i=(16-(abs(me->x-GetGoodguy()->x)+abs(me->y-GetGoodguy()->y))/(FIXAMT*TILE_WIDTH));

					if(GetGoodguy()->x>me->x)
						me->dx+=i*FIXAMT/128;
					else
						me->dx-=i*FIXAMT/128;
					if(GetGoodguy()->y>me->y)
						me->dy+=i*FIXAMT/128;
					else
						me->dy-=i*FIXAMT/128;
				}
			}
			break;
		case BLT_LILBOOM:
			i = SpellLevel() / 10 + 1;
			if (!ClassicMode())
				i = SkillValue(SKILL_SEEKER) + 1;
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,i,map,world))
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
		case BLT_MINE:
			if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 40, me->dx, me->dy, 0, map, world))
			{
				me->type = BLT_NONE;
				FireExactBullet(me->x, me->y, 0, 0, 0, 0, 0, 7, 0, BLT_BOOM);	// detonate
				MakeSound(SND_BOMBBOOM, me->x, me->y, SND_CUTOFF, 100);
			}
			break;
			break;
		case BLT_LIQUIFY:
		case BLT_LIQUIFY2:
		case BLT_LIQUIFY3:
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
		case BLT_BIGYELLOW:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,me->dx,me->dy,20+(player.worldNum==3)*10-10*(player.worldNum==1),map,world))
			{
				// hit noise
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_YELWAVE:
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,me->dx,me->dy,8,map,world))
			{
				// hit noise
				BulletRanOut(me,map,world);
			}
			break;
		case BLT_SPORE:
			if((i=FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,4,me->dx/2,me->dy/2,1,map,world)))
			{
				if(i==2)	// only poison if the player himself is hit
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
		case BLT_BOOM:
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,64,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,2,map,world))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_SEEKBOOM:
			if (FindVictims(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 64, (8 - MGL_random(17)) << FIXSHIFT,
				(8 - MGL_random(16)) << FIXSHIFT, SkillValue(SKILL_SEEKBOOM), map, world))
			{
				// nothing much to do here, the victim will scream quite enough
			}
			break;
		case BLT_YELBOOM:
			i=20*(5-(me->timer/2));	// size expands as boom expands
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,(8-MGL_random(17))<<FIXSHIFT,
				(8-MGL_random(16))<<FIXSHIFT,5,map,world))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_SHOCKWAVE:
			i=30*(3-(me->timer/2))+30;	// size expands as wave expands
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,0,0,12-6*(player.worldNum==1),map,world))
			{
				// don't disappear because Bouapha needs to get multipounded
			}
			break;
		case BLT_GOODSHOCK:
			i=30*(3-(me->timer/2))+30;	// size expands as wave expands
			if (ClassicMode())
				j = 4;
			else
			{
				j = (SkillValue(SKILL_SUMMON) + 3) / 2;
				if (player.summonDmgBoost)
				{
					j = j * (100 + SkillValue(SKILL_HEALSUMMONS));
					if (Random(j % 100) >= 50)
						j += 100;
					j = j / 100;
				}
			}
			if(FindVictims(me->x>>FIXSHIFT,me->y>>FIXSHIFT,i,0,0,j,map,world))
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
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,4+(player.worldNum>1)*8,map,world))
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
			if(FindGoodVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,me->dx,me->dy,20,map,world))
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
	bulletHittingType=0;
}

void UpdateBullet(bullet_t *me,Map *map,world_t *world)
{
	int mapx,mapy,i;
	byte b;

	mapx=(me->x/TILE_WIDTH)>>FIXSHIFT;
	mapy=(me->y/TILE_HEIGHT)>>FIXSHIFT;
	if(me->x<0 || me->x>=map->width*TILE_WIDTH*FIXAMT || me->y<0 || me->y>=map->height*TILE_WIDTH*FIXAMT)
	{
		me->type=BLT_NONE;
		return;
	}

	me->bright=map->map[mapx+mapy*map->width].templight;

	// special things like animation
	switch(me->type)
	{
		case BLT_MINE:
			if (me->anim == 1)	// floating
			{
				if (me->z > 30*FIXAMT)
					me->dz = 0;
				if (me->timer < 30 * 30 - 60)	// 2s arming time
					HitBadguys(me, map, world);
			}
			break;
		case BLT_COMET:
		case BLT_ICECOMET:
			me->anim++;
			if(me->anim==8)
				me->anim=0;
			break;
		case BLT_COMETBOOM:
			me->anim++;
			HitBadguys(me,map,world);
			break;
		case BLT_COMETBOOM2:
		case BLT_ICECOMETBOOM:
			me->anim++;
			if(me->anim==1)
				HitBadguys(me, map, world);
			break;
		case BLT_SLIME:	// frame 259-270
			me->anim++;
			if(me->anim&1)
				HitBadguys(me,map,world);
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
		case BLT_EVILHAMMER:
			me->anim++;
			if(me->anim>4)
			{
				me->anim=0;
				AddParticle(me->x,me->y,me->z,-FIXAMT/2+MGL_randoml(FIXAMT),-FIXAMT/2+MGL_randoml(FIXAMT),0,
							10,PART_SHORTYELLOW,191);
			}
			HitBadguys(me,map,world);
			break;
		case BLT_REDFBALL:
			HitBadguys(me, map, world);
			break;
		case BLT_MINIFBALL:
		case BLT_PTEROSHOT:
		case BLT_ICESHARD:
			HitBadguys(me,map,world);
			break;
		case BLT_COIN:
		case BLT_BIGCOIN:
		case BLT_LIFEPOTION:
		case BLT_MANAPOTION:
		case BLT_RUNESTONE:
			Dampen(&me->dx,FIXAMT/64);
			Dampen(&me->dy,FIXAMT/64);
			me->anim++;
			if(me->anim>=8*2)
				me->anim=0;
			HitBadguys(me,map,world);
			break;
		case BLT_SPINE:
			HitBadguys(me,map,world);
			break;
		case BLT_FAKELIQUIFY:
			if(Random(2))
				me->anim++;
			if (me->anim == 14)
			{
				me->type = BLT_NONE;
				BlowSmoke(me->x, me->y, me->z, FIXAMT);
			}
			me->z += FIXAMT;
			break;
		case BLT_LIQUIFY:
		case BLT_LIQUIFY2:
		case BLT_LIQUIFY3:
			if(me->anim&1)
				HitBadguys(me,map,world);
			me->anim++;
			if(me->anim==14)
			{
				me->type=BLT_NONE;
				BlowSmoke(me->x,me->y,me->z,FIXAMT);
			}
			if(me->anim==4 && me->timer>13)
			{
				b=(me->facing-8+MGL_random(17))&255;
				if (!ClassicMode())
				{
					// ricochet off walls!
					int x, y;
					int map2x, map2y;
					int mapx, mapy;
					mapx = me->x / (TILE_WIDTH * FIXAMT);
					mapy = me->y / (TILE_HEIGHT * FIXAMT);
					x = me->x + Cosine(b) * 16;
					y = me->y + Sine(b) * 16;
					map2x = x / (TILE_WIDTH * FIXAMT);
					map2y = y / (TILE_HEIGHT * FIXAMT);

					if (!Bulletable(map, map2x, mapy))	// bounced moving horiz
					{
						BurnTreesInArea(map, me->x - 16*FIXAMT, me->y - 16*FIXAMT, me->x + 16*FIXAMT, me->y + 16*FIXAMT);

						b = 128 - b;
						x = me->x + Cosine(b) * 16;
						y = me->y + Sine(b) * 16;
						map2x = x / (TILE_WIDTH * FIXAMT);
						map2y = y / (TILE_HEIGHT * FIXAMT);
					}
					if (!Bulletable(map, mapx, map2y))	// bounced moving vert
					{
						BurnTreesInArea(map, me->x - 16*FIXAMT, me->y - 16*FIXAMT, me->x + 16*FIXAMT, me->y + 16*FIXAMT);

						b = 256 - b;
						x = me->x + Cosine(b) * 16;
						y = me->y + Sine(b) * 16;
					}
						
					FireExactBullet(x, y, 0, 0, 0, 0, 0, me->timer, b, me->type);
				}
				else
					FireExactBullet(me->x+Cosine(b)*16,me->y+Sine(b)*16,0,0,0,0,0,me->timer,b,me->type);
			}
			mapx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			mapy=(me->y>>FIXSHIFT)/TILE_HEIGHT;

			map->BrightTorch(mapx,mapy,2,4);
			if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_WATER)
				BulletRanOut(me,map,world);
			break;
		case BLT_ICEBEAM2:
		case BLT_DEATHBEAM2:
			break;
		case BLT_DEATHBEAM:
			if(!BulletCanGo(me->x,me->y,map,8))
			{
				me->type=BLT_DEATHBEAM2;
				me->timer=8;
				break;
			}
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,32,4);
			HitBadguys(me,map,world);
			if(me->timer==4)
				FireBulletAfter(me->x+Cosine(me->facing)*16,me->y+Sine(me->facing)*16,me->facing,BLT_DEATHBEAM,me);
			me->anim=1-me->anim;
			break;
		case BLT_ICEBEAM:
			if(!BulletCanGo(me->x,me->y,map,8))
			{
				me->type=BLT_ICEBEAM2;
				me->timer=8;
				break;
			}
			HitBadguys(me,map,world);
			if(me->timer==4)
				FireBulletAfter(me->x+Cosine(me->facing)*16,me->y+Sine(me->facing)*16,me->facing,BLT_ICEBEAM,me);
			me->anim=1-me->anim;
			if(me->anim==0)
				BlowWigglySmoke(me->x,me->y,me->z-MGL_randoml(FIXAMT*4),0);
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
			if(me->x<0 || me->y<0 || me->x>=(map->width+1)*TILE_WIDTH*FIXAMT ||
				me->y>=(map->height+1)*TILE_HEIGHT*FIXAMT)
				me->type=BLT_NONE;
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
		case BLT_GLOB_BOMB:
			me->anim=1-me->anim;
			break;
		case BLT_BIGYELLOW:
			me->anim=1-me->anim;
			HitBadguys(me,map,world);
			break;
		case BLT_SHOCKWAVE:
		case BLT_GOODSHOCK:
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
			if(player.worldNum==3)
			{
				if(GetGuyPos(me->target,&me->x,&me->y))
					me->y+=FIXAMT*8;
			}

			break;
		case BLT_MEGABEAM1:
		case BLT_YELWAVE:
			HitBadguys(me,map,world);
			break;
		case BLT_MEGABEAM2:
			// nothin
			break;
		case BLT_FLAME:
		{
			if (ClassicMode())
			{
				if (me->timer & 1)	// every other frame
					HitBadguys(me, map, world);
				map->BrightTorch((me->x / TILE_WIDTH) >> FIXSHIFT,
					(me->y / TILE_HEIGHT) >> FIXSHIFT, 8, 4);
				me->dz += MGL_random(FIXAMT / 8);		//anti gravity
				me->dx += MGL_random(65535) - FIXAMT / 2;
				me->dy += MGL_random(65535) - FIXAMT / 2;
				Dampen(&me->dx, FIXAMT / 4);
				Dampen(&me->dy, FIXAMT / 4);
				Clamp(&me->dx, FIXAMT * 10);
				Clamp(&me->dy, FIXAMT * 10);
				me->anim = ((32 - me->timer) / 8) + 1;
				if (me->anim > 4)
					me->anim = 4;
			}
			else
			{
				if(flameDmgTick==0)
				{
					HitBadguys(me, map, world);
				}
				map->BrightTorch((me->x / TILE_WIDTH) >> FIXSHIFT,
					(me->y / TILE_HEIGHT) >> FIXSHIFT, 8, 4);
				me->dz += MGL_random(FIXAMT / 8);		//anti gravity
				me->dx += MGL_random(65535) - FIXAMT / 2;
				me->dy += MGL_random(65535) - FIXAMT / 2;
				Dampen(&me->dx, FIXAMT / 4);
				Dampen(&me->dy, FIXAMT / 4);
				Clamp(&me->dx, FIXAMT * 10);
				Clamp(&me->dy, FIXAMT * 10);
				me->anim = ((32 - me->timer) / 8) + 1;
				byte maxFrame = (SkillValue(SKILL_FLAMEON)) + 1;
				if (maxFrame > 4) maxFrame = 4;
				if (me->anim > maxFrame)
					me->anim = maxFrame;
			}
		}
			break;
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
		case BLT_LASER2:
			HitBadguys(me,map,world);
			break;
		case BLT_BOMB:
			break;
		case BLT_BOOM:
		case BLT_SEEKBOOM:
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,12,8);
			HitBadguys(me,map,world);
			if(me->timer==1)
				BurnTreesInArea(map, me->x - 48*FIXAMT, me->y - 48*FIXAMT, me->x + 48*FIXAMT, me->y + 48*FIXAMT);
			break;
		case BLT_ICECLOUD:
			me->anim=1-me->anim;
			if(me->anim==0)
				BlowWigglySmoke(me->x,me->y,me->z-MGL_randoml(FIXAMT*4),0);
			HitBadguys(me,map,world);
			break;
		case BLT_MISSILE:
		case BLT_SKULL:
			me->anim++;
			if(me->type==BLT_MISSILE)
			{
				if(me->timer==40)
					me->target=LockOnEvil(map,me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				if(((me->anim>0) && (me->target!=65535)) ||
					((me->anim>2) && (me->target==65535)))
				{
					BlowSmoke(me->x-me->dx,me->y-me->dy,me->z,FIXAMT/16);
					me->anim=0;
				}
			}
			else
			{
				if(me->timer==50)
					me->target=LockOnEvil(map,me->x>>FIXSHIFT,me->y>>FIXSHIFT);
				map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
								 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,4);
			}

			HitBadguys(me,map,world);
			if(!GetGuyPos(me->target,&mapx,&mapy))
				me->target=65535;
			else
			{
				if (me->type == BLT_MISSILE)
				{
					if (ClassicMode())
						i = (FIXAMT / 8) * (SpellLevel() / 2) + FIXAMT / 4;
					else
						i = (FIXAMT / 8) * (SkillValue(SKILL_SEEKER)*4+5) + FIXAMT / 4;
				}
				else
					i=FIXAMT;

				if(me->x>mapx)
					me->dx-=i;
				else
					me->dx+=i;
				if(me->y>mapy)
					me->dy-=i;
				else
					me->dy+=i;

				if(me->type==BLT_MISSILE)
					i=SpellLevel()/3+4;
				else
					i=8;
				Clamp(&me->dx,i<<FIXSHIFT);
				Clamp(&me->dy,i<<FIXSHIFT);

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
	if(me->type==BLT_NONE)
		return;

	// if you're in a wall even before moving, explode (to stop infinite bounces)
	if((me->type==BLT_HAMMER || me->type==BLT_HAMMER2 || me->type==BLT_LASER || me->type==BLT_LASER2 || me->type==BLT_COIN || me->type==BLT_RUNESTONE || me->type==BLT_PTEROSHOT || me->type==BLT_ICESHARD ||
		me->type==BLT_MINIFBALL || me->type==BLT_BIGYELLOW || me->type==BLT_BIGCOIN || me->type==BLT_LIQUIFY || me->type==BLT_LIQUIFY2 || me->type==BLT_LIQUIFY3
		|| me->type==BLT_ICEBEAM || me->type==BLT_SKULL || me->type==BLT_DEATHBEAM || me->type==BLT_REDFBALL) &&
		(!BulletCanGo(me->x,me->y,map,1)))
	{
		if(me->type==BLT_HAMMER2 || me->type==BLT_HAMMER)
		{
			me->type=BLT_HAMMER;
			BulletHitWallX(me,map,world);
		}
		else if(me->type==BLT_ICEBEAM || me->type==BLT_DEATHBEAM)
		{
			BulletHitWallX(me,map,world);
		}
		else
		{
			BulletRanOut(me,map,world);
		}
		return;
	}

	b=0;

	me->x+=me->dx;
	if(!BulletCanGo(me->x,me->y,map,1))
		BulletHitWallX(me,map,world);
	else
	{
		me->y+=me->dy;
		if(!BulletCanGo(me->x,me->y,map,1))
			BulletHitWallY(me,map,world);
	}

	if(me->type!=BLT_ICESHARD)	// iceshard stores guy ID in DZ, don't use it
		me->z+=me->dz;

	if(me->z<0)
		BulletHitFloor(me,map,world);

	// all gravity-affected bullets, get gravitized
	if(me->type==BLT_BOMB || me->type==BLT_GRENADE || (me->type==BLT_MINE && me->anim==0) || me->type==BLT_GLOB_BOMB || me->type==BLT_LIFEPOTION || me->type==BLT_MANAPOTION
		|| me->type==BLT_ROCK || me->type==BLT_EVILHAMMER || me->type==BLT_COIN || me->type==BLT_RUNESTONE
		|| me->type==BLT_BIGCOIN)
		me->dz-=FIXAMT;

	me->timer--;
	if(!me->timer)
		BulletRanOut(me,map,world);
}

void RenderSmoke(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_SMOKE+frm),DISPLAY_DRAWME|DISPLAY_GHOST);
}

void RenderBoom(int x,int y,int z,char bright,byte frm)
{
	SprDraw(x,y,z,255,bright-64,bulletSpr->GetSprite(SPR_BOOM+frm),DISPLAY_DRAWME|DISPLAY_GLOW);
}

void RenderLvlUp(int x,int y,int z)
{
	SprDraw(x,y,z,255,0,bulletSpr->GetSprite(222),DISPLAY_DRAWME|DISPLAY_GLOW);
}

void RenderBullet(bullet_t *me)
{
	int v;
	sprite_t *curSpr;

	switch(me->type)
	{
		case BLT_COMET:
			curSpr=bulletSpr->GetSprite(SPR_COMET+me->anim);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,0,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ICECOMET:
			curSpr = bulletSpr->GetSprite(SPR_COMET + me->anim);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 7, 0, curSpr,
				DISPLAY_DRAWME);
			break;
		case BLT_COMETBOOM:
		case BLT_COMETBOOM2:
			curSpr=bulletSpr->GetSprite(SPR_COMETBOOM+me->anim/2);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,0,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ICECOMETBOOM:
			curSpr = bulletSpr->GetSprite(SPR_COMETBOOM + me->anim / 2);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 7, 0, curSpr,
				DISPLAY_DRAWME);
			break;
		case BLT_SLIME:
			curSpr=bulletSpr->GetSprite(259+(me->anim/16));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-4,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ICECLOUD:
			// invisible... it just gives off steam
			break;
		case BLT_ICEBEAM:
			curSpr=bulletSpr->GetSprite(((me->facing/32)&3)+239);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_ICEBEAM2:
			curSpr=bulletSpr->GetSprite(238);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_DEATHBEAM:
			curSpr=bulletSpr->GetSprite(((me->facing/32)&3)+239);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,4,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_DEATHBEAM2:
			curSpr=bulletSpr->GetSprite(238);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,4,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_LIQUIFY:
		case BLT_LIQUIFY2:
		case BLT_LIQUIFY3:
		case BLT_FAKELIQUIFY:
			curSpr=bulletSpr->GetSprite(231+me->anim/2);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_COIN:
		case BLT_BIGCOIN:
			if(me->timer<30 && (me->timer&1)==0)
				return;
			if(me->type==BLT_COIN)
				curSpr=GetItemSprite(147+me->anim/2);
			else
				curSpr=GetItemSprite(159+me->anim/2);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_LIFEPOTION:
			if (me->timer < 30 && (me->timer & 1) == 0)
				return;
			curSpr = GetItemSprite(156);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 255, me->bright+((me->timer&7)==0)*2, curSpr,
				DISPLAY_DRAWME);
			break;
		case BLT_MANAPOTION:
			if (me->timer < 30 && (me->timer & 1) == 0)
				return;
			curSpr = GetItemSprite(155);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 255, me->bright + ((me->timer & 7) == 0) * 2, curSpr,
				DISPLAY_DRAWME);
			break;
		case BLT_RUNESTONE:
			if (me->timer < 30 && (me->timer & 1) == 0)
				return;
			curSpr = GetItemSprite(280);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 255, me->bright, curSpr,
				DISPLAY_DRAWME);
			break;
		case BLT_HAMMER:
		case BLT_HAMMER2:
			v=me->facing*5+(me->anim)+SPR_FIREBALL;
			curSpr=bulletSpr->GetSprite(v);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_REDFBALL:
			curSpr = bulletSpr->GetSprite(SPR_SPINE + (me->facing));
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 4, me->bright, curSpr,
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
			curSpr=bulletSpr->GetSprite(SPR_MISSILE+me->facing);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-6,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SKULL:
			curSpr=bulletSpr->GetSprite(SPR_SKULL+me->facing);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright-2,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
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
		case BLT_LASER2:
			curSpr = bulletSpr->GetSprite(me->facing + SPR_LASER);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 6, me->bright, curSpr,
				DISPLAY_DRAWME);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case BLT_BOMB:
			// invisible
			break;
		case BLT_BOOM:
		case BLT_SEEKBOOM:
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
		case BLT_BIGYELLOW:
			curSpr=bulletSpr->GetSprite(me->anim+SPR_GRENADE);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case BLT_GLOB_BOMB:
			curSpr = bulletSpr->GetSprite(me->anim + SPR_GRENADE);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 7, me->bright, curSpr,
				DISPLAY_DRAWME);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 7, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case BLT_MINE:
			curSpr = bulletSpr->GetSprite(110+(me->anim==1)*Random(2));	// vibrate wildly when you are timing up
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_GLOW);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 255, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case BLT_YELBOOM:
			curSpr=bulletSpr->GetSprite(me->anim/2+SPR_YELBOOM);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case BLT_SHOCKWAVE:
		case BLT_GOODSHOCK:
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
		case BLT_MINIFBALL:
		case BLT_PTEROSHOT:
			curSpr=bulletSpr->GetSprite(SPR_SPINE+(me->facing));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME);
			break;
		case BLT_ICESHARD:
			curSpr = bulletSpr->GetSprite(SPR_SPINE + (me->facing));
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 0, 0, me->bright, curSpr,
				DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(me->x >> FIXSHIFT, me->y >> FIXSHIFT, me->z >> FIXSHIFT, 0, me->bright+6, curSpr,
				DISPLAY_DRAWME);
			break;
		case BLT_YELWAVE:
			curSpr=bulletSpr->GetSprite(SPR_WAVE+(me->facing));
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,me->bright,curSpr,
					DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
	}
}

void UpdateBullets(Map *map,world_t *world)
{
	int i;

	byte max = (6 - SkillValue(SKILL_FLAMEON));
	if (flameDmgTick > 0)
		flameDmgTick--;
	else
		flameDmgTick = max;

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

	me->lastHit=65535;
	me->type=type;
	me->x=x;
	me->y=y;
	me->facing=facing;
	me->bright=0;
	me->anim=0;

	switch(me->type)
	{
		case BLT_COMETBOOM2:
		case BLT_ICECOMETBOOM:
			me->anim = 0;
			me->timer = 18;
			me->dx = me->dy = me->dz = 0;
			me->z = 0;
			break;
		case BLT_COMET:
		case BLT_ICECOMET:
			me->anim=(byte)MGL_random(8);
			me->timer=255;
			me->z=400*FIXAMT+MGL_randoml(300*FIXAMT);
			me->dx=0;
			me->dy=0;
			me->dz=-FIXAMT*50;
			break;
		case BLT_SLIME:
			me->anim=0;
			me->timer=12*16;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_FAKELIQUIFY:
			me->anim = 0;
			me->timer = 40;
			me->z = 0;
			me->dx = 0;
			me->dy = 0;
			me->dz = 0;
			break;
		case BLT_LIQUIFY:
			me->anim=0;
			if (ClassicMode())
				me->timer = SpellLevel() * 2;
			else
				me->timer = 25+SkillValue(SKILL_FLAMEON) * 15;
			me->z=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_LIQUIFY2:
			me->anim = 0;
			if (ClassicMode())
				me->timer = SpellLevel() * 2;
			else
				me->timer = 25 + SkillValue(SKILL_INFERNO) * 15;
			me->z = 0;
			me->dx = 0;
			me->dy = 0;
			me->dz = 0;
			break;
		case BLT_LIQUIFY3:
			me->anim = 0;
			if (ClassicMode())
				me->timer = SpellLevel() * 2;
			else
			{
				byte dur = 20;
				if (RuneValue(Rune::BERSERK2) > 0)
					dur = (byte)(20.0f * RuneValue(Rune::BERSERK2) / 100.0f);
				me->timer = dur;
			}
			me->z = 0;
			me->dx = 0;
			me->dy = 0;
			me->dz = 0;
			break;
		case BLT_ICEBEAM:
			me->anim=0;
			me->timer=4;
			me->z=FIXAMT*20;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_DEATHBEAM:
			me->anim=0;
			me->timer=4;
			me->z=FIXAMT*45;
			if(me->facing==32*0 || me->facing==32*4)
				me->z=FIXAMT*50;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_DEATHBEAM2:
			me->anim=0;
			me->timer=8;
			me->z=FIXAMT*40;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
		case BLT_BIGYELLOW:
			me->anim=0;
			me->timer=30*10;
			me->z=FIXAMT*30;
			me->dz=0;
			me->dx=Cosine(me->facing)*4;
			me->dy=Sine(me->facing)*4;
			break;
		case BLT_YELWAVE:
			me->anim=0;
			me->timer=15;
			me->z=FIXAMT*2;
			me->dz=0;
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
			break;
		case BLT_COIN:
		case BLT_BIGCOIN:
		case BLT_RUNESTONE:
			me->facing=(byte)MGL_random(256);
			f=MGL_randoml(3)+1;
			me->dx=-FIXAMT*4+MGL_randoml(FIXAMT*8);
			me->dy=-FIXAMT*4+MGL_randoml(FIXAMT*8);
			me->dz=MGL_randoml(FIXAMT*6)+FIXAMT*4;
			me->z=FIXAMT/2;
			me->timer=30*9+MGL_random(30*4);
			break;
		case BLT_LIFEPOTION:
		case BLT_MANAPOTION:
			me->facing = (byte)MGL_random(256);
			f = MGL_randoml(3) + 1;
			me->dx = -FIXAMT * 2 + MGL_randoml(FIXAMT * 4);
			me->dy = -FIXAMT * 2 + MGL_randoml(FIXAMT * 4);
			me->dz = MGL_randoml(FIXAMT * 6) + FIXAMT * 4;
			me->z = FIXAMT / 2;
			me->timer = 30 * 20;
			break;
		case BLT_BOOM:
		case BLT_SEEKBOOM:
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
		case BLT_REDFBALL:
			me->anim = 0;
			me->timer = 30;
			me->z = FIXAMT * 20;
			me->dx = Cosine(me->facing) * 6;
			me->dy = Sine(me->facing) * 6;
			me->dz = 0;
			me->facing=(me->facing + 7) / 16;
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
			//me->x+=((MGL_random(17)-8)<<FIXSHIFT);
			//me->y+=((MGL_random(17)-8)<<FIXSHIFT);
			me->dx=Cosine(me->facing*16)*4;
			me->dy=Sine(me->facing*16)*4;
			MakeSound(SND_MISSILELAUNCH,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_SKULL:
			me->anim=0;
			me->timer=60;
			me->z=FIXAMT*20;
			me->dz=0;
			me->target=65535;
			me->dx=Cosine(me->facing)*6;
			me->dy=Sine(me->facing)*6;
			me->facing=facing/16;
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
			me->anim=0;
			me->lastHit = 0;
			if (ClassicMode())
				me->timer = (SpellLevel() / 2) + 10 - MGL_random(4);
			else
				me->timer = (SkillValue(SKILL_FLAMEON)*6) + 10 - MGL_random(4);
			me->z=FIXAMT*20;
			me->x+=((MGL_random(3)-1)<<FIXSHIFT)+Cosine(me->facing*32)*5;
			me->y+=((MGL_random(3)-1)<<FIXSHIFT)+Sine(me->facing*32)*5;
			if (!ClassicMode() && SkillValue(SKILL_FLAMEON)>0)	// spread increasing with points
			{
				byte range = SkillValue(SKILL_FLAMEON) * 4;
				me->dx = Cosine((byte)(me->facing * 32-range+Random(range*2))) * 10;
				me->dy = Sine((byte)(me->facing * 32-range+Random(range*2))) * 10;
			}
			else
			{
				me->dx = Cosine(me->facing * 32) * 10;
				me->dy = Sine(me->facing * 32) * 10;
			}
			me->dz=-FIXAMT/2;
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_FLAME2:
			me->anim=0;
			me->timer=24-MGL_random(4);
			me->z=FIXAMT*20;
			me->x+=((MGL_random(3)-1)<<FIXSHIFT)+Cosine(me->facing)*5;
			me->y+=((MGL_random(3)-1)<<FIXSHIFT)+Sine(me->facing)*5;
			me->dx=Cosine(me->facing)*10;
			me->dy=Sine(me->facing)*10;
			me->dz=-FIXAMT/2;
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
			break;
		case BLT_LASER:
		case BLT_LASER2:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20-MGL_random(65535);
			me->x+=((MGL_random(3)-1)<<FIXSHIFT);
			me->y+=((MGL_random(3)-1)<<FIXSHIFT);
			me->x+=FIXAMT/2-MGL_random(65535);
			me->y+=FIXAMT/2-MGL_random(65535);
			me->facing=me->facing*32+4-MGL_random(9);
			me->dx=Cosine(me->facing)*(12+(SpellLevel()/10));
			me->dy=Sine(me->facing)*(12+(SpellLevel()/10));
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
		case BLT_GLOB_BOMB:
			me->anim = 0;
			me->timer = 255;
			me->z = FIXAMT * 80;
			f = MGL_random(12) + 1;
			me->dx = Cosine(me->facing) * f;
			me->dy = Sine(me->facing) * f;
			me->dz = FIXAMT * 20;
			break;
		case BLT_MINE:
			me->anim = 0;
			me->timer = 255;
			me->z = 1;
			me->facing = Random(256);
			f = MGL_random(3) + 1;
			me->dx = Cosine(me->facing) * f;
			me->dy = Sine(me->facing) * f;
			me->dz = FIXAMT * 10;
			break;
		case BLT_SHOCKWAVE:
		case BLT_GOODSHOCK:
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
		case BLT_MINIFBALL:
		case BLT_PTEROSHOT:
			me->anim=0;
			me->timer=30;
			me->z=FIXAMT*20;
			// this takes 0-255 directional facings
			me->dx=Cosine(me->facing)*12;
			me->dy=Sine(me->facing)*12;
			me->facing/=16;
			me->dz=0;
			break;
		case BLT_ICECLOUD:
			me->anim=0;
			me->timer=SpellLevel()*2+10;
			me->z=FIXAMT*10;
			me->dx=Cosine(me->facing*32);
			me->dy=Sine(me->facing*32);
			me->dz=-FIXAMT/2;
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
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
			bullet[i].lastHit = 65535;
			bullet[i].target = 65535;
			break;
		}
}

void HammerLaunch(int x, int y, byte facing, byte count, byte flags,bool skulls)
{
	byte angle, newfacing;
	byte bulType;
	byte timer = 30;
	int speed = 12;
	byte facingDivide = 32;
	
	if (skulls)
	{
		facingDivide = 16;
		speed = 6;
		timer = 60;
		bulType = BLT_SKULL;
		MakeSound(SND_SKELSHOOT, x, y, SND_CUTOFF, 1200);
	}
	else
	{
		bulType = BLT_HAMMER + ((flags & HMR_REFLECT) > 0);
		MakeSound(SND_HAMMERTOSS, x, y, SND_CUTOFF, 1200);
	}
	if (!ClassicMode() && RuneValue(Rune::SHOTGUN) > 0)
	{
		count += RuneValue(Rune::SHOTGUN);
		for (int i = 0; i < count; i++)
		{
			byte a = facing * 32 - 24 + Random(48);
			FireExactBullet(x - FIXAMT * 10 + Random(FIXAMT * 20), y - FIXAMT * 10 + Random(FIXAMT * 20), FIXAMT * 20, Cosine(a) * (speed*4/3), Sine(a) * (speed * 4 / 3), 0, 0, timer / 3, a / facingDivide, bulType);
		}
		if (flags & HMR_REVERSE)
		{
			newfacing = ((byte)(facing - 4)) % 8;
			HammerLaunch(x, y, newfacing, count, flags & (~HMR_REVERSE),skulls);
		}
		return;
	}
	if (count == 1 || count == 3 || count == 5)	// 1,3,5 have direct forward fire
	{
		angle = facing * 32;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle/facingDivide, bulType);
	}
	if (count == 2 || count == 4)	// these have slight off-angle double forward fire
	{
		angle = facing * 32 - 8;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle / facingDivide, bulType);
		angle = facing * 32 + 8;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle / facingDivide, bulType);
	}
	if (count == 3 || count == 5)	// these have 45 degree angle fire
	{
		angle = facing * 32 - 32;
		newfacing = ((byte)(facing - 1)) % 8;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle / facingDivide, bulType);
		angle = facing * 32 + 32;
		newfacing = (facing + 1) % 8;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle / facingDivide, bulType);
	}
	if (count == 4 || count == 5)	// these add almost 90 degree off fire
	{
		angle = facing * 32 - 56;
		newfacing = ((byte)(facing - 2)) % 8;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle / facingDivide, bulType);
		angle = facing * 32 + 56;
		newfacing = (facing + 2) % 8;
		FireExactBullet(x, y, FIXAMT * 20,
			Cosine(angle) * speed, Sine(angle) * speed, 0,
			0, timer, angle / facingDivide, bulType);
	}
	if (flags & HMR_REVERSE)
	{
		newfacing = ((byte)(facing - 4)) % 8;
		HammerLaunch(x, y, newfacing, count, flags & (~HMR_REVERSE),skulls);
	}
}

void Armageddon(Map *map,int x,int y)
{
	x=x-400*FIXAMT+MGL_randoml(800*FIXAMT);
	y=y-300*FIXAMT+MGL_randoml(600*FIXAMT);

	if(x<0 || y<0 || x>=map->width*TILE_WIDTH*FIXAMT || y>=map->height*TILE_HEIGHT*FIXAMT)
		return;

	if (SkillValue(SKILL_MANAGETTIN) > 0)
	{
		if (Random(100) >= (int)SkillValue(SKILL_MANAGETTIN))
			return;	// chance of no comet
	}
	
	if(!ClassicMode() && SkillValue(SKILL_FIMBULWINTER)>0)
		FireBullet(x,y,0,BLT_ICECOMET);
	else
		FireBullet(x, y, 0, BLT_COMET);
	MakeSound(SND_FLAMEGO,x,y,SND_CUTOFF,100);
}

byte BulletHittingType(void)
{
	return bulletHittingType;
}

void BackdraftEffect(Guy *me, int radius)
{
	int j;
	int maxDist = radius * radius;
	int x, y;

	x = me->x >> FIXSHIFT;
	y = me->y >> FIXSHIFT;
	for (j = 0; j < MAX_BULLETS; j++)
	{
		bullet_t* b = &bullet[j];
		if (b->type == BLT_ACID || b->type == BLT_FLAME2 || b->type == BLT_SPORE || b->type == BLT_SHROOM || b->type == BLT_GRENADE || b->type==BLT_GLOB_BOMB ||
			b->type == BLT_ROCK || b->type == BLT_SPINE || b->type == BLT_YELWAVE || b->type == BLT_BIGYELLOW || b->type == BLT_SLIME)
		{
			int dist = ((b->x >> FIXSHIFT) - x) * ((b->x >> FIXSHIFT) - x) + ((b->y >> FIXSHIFT) - y) * ((b->y >> FIXSHIFT) - y);
			if (dist < maxDist)
			{
				b->type = BLT_NONE;
				ExplodeParticles2(PART_SMOKE, b->x>>FIXSHIFT, b->y>FIXSHIFT, b->z>>FIXSHIFT, 1, 0);	// turn to smoke
				//float a = atan2f((b->y>>FIXSHIFT)-y, (b->x>>FIXSHIFT)-x) * 128.0f / 3.14159f;
				//if (a < 0) a += 256;
				//if (a > 255) a -= 256;
			//	FireBullet(b->x, b->y, (byte)(a), BLT_REDFBALL);
			}
		}
	}
}
