#include "bullet.h"
#include "guy.h"
#include "player.h"
#include "options.h"
#include "items.h"
#include "floor.h"
#include "pushing.h"
#include "score.h"
#include "clear2.h"

bullet_t bullet[MAX_BULLETS];
sprite_set_t *bulletSpr;
byte ballSoundClock;
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
	if(ItemFlags(map->map[x+y*map->width].item)&IF_TALL)
	{
		return 0;
	}

	return 1;
}

byte GoodBullet(byte type)
{
	return 0;
}


// acid splash in clearance mode
byte AcidSplash2(Map *map,byte color,int x,int y)
{
	if(x<0 || x>=map->width || y<0 || y>=map->width)
		return 0;
	if(map->map[x+y*map->width].item!=ITM_CRATE)
		return 0;
	if(map->map[x+y*map->width].itemInfo%8!=color)
		return 0;

	player.blownUp[color]++;
	map->map[x+y*map->width].item=ITM_NONE;
	Score_BlowUp(SCORE_OBJECT,color);
	BlowSmoke(x,y,color);
	AcidSplash2(map,color,x-1,y);
	AcidSplash2(map,color,x+1,y);
	AcidSplash2(map,color,x,y-1);
	AcidSplash2(map,color,x,y+1);
	Clear2Wait();
	return 1;
}

byte AcidSplash(Map *map,byte color,int x,int y)
{
	mapTile_t *m;
	Guy *g;
	byte c;

	if(x<0 || x>=map->width || y<0 || y>=map->width)
		return 0;

	if(GameType()==GAME_CLEARANCE)
		return AcidSplash2(map,map->map[x+y*map->width].itemInfo%8,x,y);

	m=&map->map[x+y*map->width];
	if((((m->itemInfo%8)==color) || (color==0)) &&
		(m->item==ITM_BARREL || m->item==ITM_BALL || m->item==ITM_CRATE ||
		 m->item==ITM_ICEBERG) && (m->itemInfo%8)!=0)
	{
		player.blownUp[m->itemInfo%8]++;
		m->item=ITM_NONE;
		Score_BlowUp(SCORE_OBJECT,m->itemInfo%8);
		BlowSmoke(x,y,color);
		return 1;
	}
	else if(m->item==ITM_MONS1 || m->item==ITM_MONS2 || m->item==ITM_MONS3 ||
		m->item==ITM_MONS4)
	{
		if(MonsterExplode(x,y,map,0))
		{
			BlowSmoke(x,y,color);
			return 1;
		}
	}
	else if((((m->itemInfo%8)==color) || (color==0)) &&
		(m->item==ITM_PENCIL) && (m->itemInfo%8)!=0)
	{
		player.deadPencils[m->itemInfo%8]++;
		BlowSmoke(x,y,color);
		m->item=ITM_NONE;
		return 1;
	}
	else if((((m->itemInfo%8)==color) || (color==0)) &&
		(m->item==ITM_KEY) && (m->itemInfo%8)!=0)
	{
		BlowSmoke(x,y,color);
		m->item=ITM_NONE;
		return 1;
	}
	else if(m->item==ITM_PLAYER)
	{
		PlayerExplode(map);
		return 1;
	}
	else if(m->item==ITM_MOVINGBALL)
	{
		// find which ball is on this space
		g=FindBall(x,y);
		if(g && g->color!=0 && (g->color==color || color==0))
		{
			// and remove it!
			c=g->color;
			player.blownUp[c]++;
			g->type=MONS_NONE;
			BlowSmoke(x,y,color);
			Score_BlowUp(SCORE_OBJECT,c);
			g->color=c;
			m->item=ITM_NONE;
			return 1;
		}
	}
	else if(m->item==ITM_ACID)
		return 0;
	else if((((m->itemInfo%8)==color) || (color==0)) &&
		(m->item==ITM_LASER) && (m->itemInfo%8)!=0)
	{
		m->item=ITM_NONE;
		BlowSmoke(x,y,color);
		return 1;
	}
	else if((((m->itemInfo%8)==color) || (color==0)) &&
		(m->item==ITM_DETONATE) && (m->itemInfo%8)!=0)
	{
		m->item=ITM_NONE;
		BlowSmoke(x,y,color);
		return 1;
	}
	else if((((m->itemInfo%8)==color) || (color==0)) &&
		(m->item==ITM_BOMB) && (m->itemInfo%8)!=0)
	{
		m->item=ITM_NONE;
		BlowSmoke(x,y,color);
		return 1;
	}
	else if((m->item==ITM_ACTIVEBOMB) && (color==0 || color==m->itemInfo%8))
	{
		g=FindBomb(x,y);
		if(g && g->type==MONS_BOOM)
			return 0;	//can't melt an explosion

		if(g)
		{
			g->type=MONS_NONE;
			BlowSmoke(x,y,color);
		}


		m->item=ITM_BOMB;
		return 1;
	}

	return 0;
}
byte BulletCanGo(bullet_t *me,int xx,int yy,Map *map,byte size)
{
	char dx[]={1,0,-1,0};
	char dy[]={0,1,0,-1};

	mapTile_t *m;
	Guy *g;

	xx=xx/(TILE_WIDTH*FIXAMT);
	yy=yy/(TILE_HEIGHT*FIXAMT);

	if(me->type==BLT_SMOKE)
		return 1;
	else
	{
		if(xx<0 || xx>=map->width || yy<0 || yy>=map->height)
			return 0;

		m=&map->map[xx+yy*map->width];
		if(m->item==ITM_ACTIVEBOMB || m->item==ITM_BOMB)
		{
			// detonate the bomb!
			DetonateBomb(xx,yy,map);
			return 0;
		}
		if(m->item==ITM_LASER)
		{
			// fire the laser, leaving it intact
			FireLaser(xx+dx[me->facing/2],yy+dy[me->facing/2],me->facing,m->itemInfo%8,map);
			return 0;
		}
		if(m->item==ITM_DETONATE)
		{
			DetonateAllBombs(m->itemInfo%8,map);
			MakeNormalSound(SND_DETONATOR);
			if((m->itemInfo%8)!=0)	// only vaporize it if it's not grey
			{
				BlowSmoke(xx,yy,m->itemInfo%8);
				m->item=ITM_NONE;
			}
			return 0;
		}
		if(m->item==ITM_PENCIL && (m->itemInfo%8)!=0)
		{
			player.deadPencils[m->itemInfo%8]++;
			BlowSmoke(xx,yy,m->itemInfo%8);
			m->item=ITM_NONE;
			return 0;
		}
		if(m->item==ITM_KEY && (m->itemInfo%8)!=0)
		{
			BlowSmoke(xx,yy,m->itemInfo%8);
			m->item=ITM_NONE;
			return 0;
		}
		if(m->item==ITM_CRANK)
		{
			TurnCrank(map,m);
			return 0;
		}
		if(m->item==ITM_ACID && (m->itemInfo%8)!=0)
		{
			BlowSmoke(xx,yy,m->itemInfo%8);
			AcidSplash(map,m->itemInfo%8,xx-1,yy-1);
			AcidSplash(map,m->itemInfo%8,xx,yy-1);
			AcidSplash(map,m->itemInfo%8,xx+1,yy-1);
			AcidSplash(map,m->itemInfo%8,xx-1,yy+1);
			AcidSplash(map,m->itemInfo%8,xx,yy+1);
			AcidSplash(map,m->itemInfo%8,xx+1,yy+1);
			AcidSplash(map,m->itemInfo%8,xx-1,yy);
			AcidSplash(map,m->itemInfo%8,xx+1,yy);
			MakeNormalSound(SND_ACID);
			m->item=ITM_NONE;
			return 0;
		}
		if(m->item>=ITM_MONS1 && m->item<=ITM_MONS4)
		{
			g=FindMonster(xx,yy);
			if(g==me->owner)
				return 1;
			else
				return 0;
		}

		if(m->item && m->item!=ITM_PLAYER)
			return 0;

		if(FloorIsSolid(m->floor))
			return 0;

		return 1;
	}
}

void BulletHitWallX(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_AQUA:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletHitWallY(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_AQUA:
			BulletRanOut(me,map,world);
			break;
	}
}

void BulletHitFloor(bullet_t *me,Map *map,world_t *world)
{
	me->z=0;
	me->dz=-me->dz;
}

void BulletRanOut(bullet_t *me,Map *map,world_t *world)
{
	switch(me->type)
	{
		case BLT_AQUA:
			ExplodeParticles2(PART_WATER,me->x,me->y,me->z,12,6);
			MakeSound(SND_BULLETPOP,me->x,me->y,SND_CUTOFF,50);
			me->type=BLT_NONE;
			break;
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int mx,my;

	mx=me->x/(TILE_WIDTH*FIXAMT);
	my=me->y/(TILE_HEIGHT*FIXAMT);

	switch(me->type)
	{
		case BLT_AQUA:
			if(map->map[mx+my*map->width].item==ITM_PLAYER)
			{
				BulletRanOut(me,map,world);
				PlayerExplode(map);
			}
			break;
	}
}

void UpdateBullet(bullet_t *me,Map *map,world_t *world)
{
	if(me->timer)
		me->timer--;

	switch(me->type)
	{
		case BLT_SMOKE:
			me->x+=me->dx;
			me->y+=me->dy;
			me->z+=me->dz;
			me->x+=-FIXAMT/8+MGL_random(FIXAMT/4);
			me->y+=-FIXAMT/8+MGL_random(FIXAMT/4);
			me->dz+=FIXAMT/4;
			if(me->timer==0)
				me->type=BLT_NONE;
			if(me->timer>10)
				me->anim=0;
			else if(me->timer>5)
				me->anim=1;
			else if(me->timer>2)
				me->anim=2;
			else
				me->anim=3;
			break;
		case BLT_AQUA:
			me->x+=me->dx;
			me->y+=me->dy;

			me->mapx=me->x/(TILE_WIDTH*FIXAMT);
			me->mapy=me->y/(TILE_HEIGHT*FIXAMT);

			if(me->mapx!=me->oldmapx || me->mapy!=me->oldmapy)
			{
				if(!BulletCanGo(me,me->x,me->y,map,8))
				{
					BulletRanOut(me,map,world);
				}
				if(!BulletCanGo(me,me->x-me->dx,me->y-me->dy,map,8))
				{
					BulletRanOut(me,map,world);
				}
			}
			HitBadguys(me,map,world);

			me->anim++;
			if(me->anim>=5*2)
			{
				me->anim=0;
				me->owner=NULL;
			}
			break;
	}
	me->oldmapx=me->mapx;
	me->oldmapy=me->mapy;
}

void RenderBullet(bullet_t *me)
{
	switch(me->type)
	{
		case BLT_SMOKE:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,0,GetItmSprite(113+me->anim),DISPLAY_DRAWME|DISPLAY_GHOST);
			//ParticleDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,me->target*32+me->timer,me->anim*2+1,DISPLAY_DRAWME|DISPLAY_CIRCLEPART);
			break;
		case BLT_AQUA:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,GetItmSprite(117+me->anim/2),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,0,GetItmSprite(117+me->anim/2),DISPLAY_DRAWME);
			//SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,GetItmSprite(117+me->anim/2),DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
	}
}

void UpdateBullets(Map *map,world_t *world)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].type)
		{
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
	me->type=type;
	me->x=x;
	me->y=y;
	me->facing=facing;

	switch(me->type)
	{
		case BLT_SMOKE:
			me->z=MGL_random(FIXAMT*16);
			me->dx=0;
			me->dy=0;
			me->dz=0;
			me->x+=-FIXAMT*8+MGL_random(FIXAMT*16);
			me->y+=-FIXAMT*8+MGL_random(FIXAMT*16);
			me->anim=0;
			me->timer=10+MGL_random(10);
			break;
		case BLT_AQUA:
			me->z=FIXAMT*8;
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*4;
			me->dz=0;
			me->anim=0;
			me->timer=120;
			break;
	}
	me->mapx=me->x/(TILE_WIDTH*FIXAMT);
	me->mapy=me->y/(TILE_HEIGHT*FIXAMT);
	me->oldmapx=-1;
	me->oldmapy=-1;
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

byte Intersect(int rx,int ry,int rx2,int ry2,int r2x,int r2y,int r2x2,int r2y2)
{
	return (rx<r2x2 && rx2>r2x && ry<r2y2 && ry2>r2y);
}

byte BulletExists(byte type)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==type)
		{
			return 1;
		}
	}
	return 0;
}

void BlowSmoke(int x,int y,byte color)
{
	int i;
	bullet_t *b;

	x=(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	y=(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;

	player.destructionBonus++;	// for frying something
	for(i=0;i<10;i++)
	{
		b=FireBullet(x,y,0,BLT_SMOKE);
		if(b)
			b->target=color;
	}
}
