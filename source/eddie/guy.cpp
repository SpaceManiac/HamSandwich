#include "guy.h"
#include "player.h"
#include "tile.h"
#include "display.h"
#include "particle.h"
#include "random.h"
#include "sound.h"
#include "level.h"
#include "game.h"
#include "mission.h"
#include "status.h"
#include "options.h"

#define MAX_GUYS 256
#define MAX_SHROOMS 128

// guy flags
#define	GF_GOINGUP	1	// if true, the centipede is heading up, not down
#define GF_GOINGLEFT 2	// if true, the centipede is heading left, not right
#define GF_OFFSCREEN 4	// if true, this monster may move offscreen
#define GF_VICTORY	8	// if a frame goes by in which there are no living monsters with this
						// flag, the player wins (ergo, all centipede parts need this flag)
#define GF_HALFSCRN 16	// this creature is confined the lower part of the screen, like the player
#define GF_SHROOMEAT 32	// this creature eats shrooms it steps on
#define GF_GROWBLOCK 64	// shrooms can't grow over this creature
#define GF_GHOST	128	// don't worry about bumping into this
#define GF_EVIL		256	// affected by good attacks
#define GF_GOOD		512 // affected by evil attacks
#define GF_EVILMEAN 1024 // kills good guys on impact
#define GF_GOODMEAN 2048 // kills evil guys on impact
#define GF_FIXSHROOM 4096 // repairs any shroom stepped on
#define GF_FLYER	8192 // moves through other badguys, but not the player
#define GF_FIXTURRET 16384 // repairs any turret stepped on
#define GF_SANDWORM	32768	// only triggers a bump on things that are GOOD or EVIL (not GOODMEAN or EVILMEAN)

#define LOG_LENGTH 8

typedef struct move_t
{
	int dx,dy;
	int x,y;
	byte facing;
} move_t;

typedef struct guy_t
{
	byte num;
	byte type;
	word flags;

	byte anim;
	byte facing;
	byte hp;
	int reload;
	byte reload2;
	int speed;
	int x,y;
	int tx,ty;	// tile coordinates
	int dx,dy;
	int destx,desty;	// in tiles
	int rectx,recty,rectx2,recty2;
	move_t movelog[LOG_LENGTH];
	byte parent;
} guy_t;

guy_t guy[MAX_GUYS];
guy_t *goodguy;
static byte radioActive;
static byte flip=0;
static byte shroomAnim=0;
static byte numShrooms;
static byte wasHeadBonk;

void InitGuys(void)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
		guy[i].type=GUY_NONE;
	radioActive=0;	// if this is true, a radio is currently descending, so don't send any more
	numShrooms=0;
}

void InitGuysLeaveShrooms(byte shroomtype)
{
	int i,done;

	numShrooms=0;

	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type==shroomtype)
		{
			numShrooms++;
			guy[i].hp=4;
		}
		else
			guy[i].type=GUY_NONE;

	// now the shrooms need to be packed together so that when centipedes are added,
	// they aren't scattered, causing following to not work
	done=0;
	while(!done)
	{
		done=1;
		for(i=0;i<MAX_GUYS-1;i++)
			if(guy[i].type==GUY_NONE)
			{
				if(guy[i+1].type!=GUY_NONE)
					done=0;
				guy[i]=guy[i+1];
				guy[i+1].type=GUY_NONE;
			}
	}
	radioActive=0;	// if this is true, a radio is currently descending, so don't send any more
}

void ExitGuys(void)
{
}

void Dampen(int *value,int amt)
{
	if(*value>0)
	{
		*value-=amt;
		if(*value<0)
			*value=0;
	}
	if(*value<0)
	{
		*value+=amt;
		if(*value>0)
			*value=0;
	}
}

void Clamp(int *value,int amt)
{
	if(*value>amt)
		*value=amt;
	if(*value<-amt)
		*value=-amt;
}

byte GetLogLength(int speed)
{
	switch(speed)
	{
		case FIXAMT*2:
			return 6;
		case FIXAMT*4:
			return 4;
		case FIXAMT*6:
			return 3;
		case FIXAMT*8:
			return 2;
		case FIXAMT*10:
			return 2;
		case FIXAMT*12:
			return 2;
		case FIXAMT*16:
			return 1;
		default:
			return 3;
	}
}

void UpdateGuyRect(guy_t *me)
{
	int mx,my;

	mx=me->x>>FIXSHIFT;
	my=me->y>>FIXSHIFT;
	switch(me->type)
	{
		case GUY_PLAYER:
			if(IsBonusStage() || GetWorld()==WORLD_ASTEROID)
			{
				SpriteRect(SPR_ROCKET,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
				me->rectx+=8;
				me->rectx2-=8;
				me->recty+=4;
				me->recty2-=2;
			}
			else
			{
				me->rectx=mx-8;
				me->rectx2=mx+8;
				me->recty=my-16;
				me->recty2=my+4;
			}
			break;
		case GUY_BULLET:
			SpriteRect(SPR_BULLET,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_CHAINBULLET:
			SpriteRect(SPR_BULLET,20,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SHOTBULLET:
			SpriteRect(SPR_BULLET,19,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_LASER:
			SpriteRect(SPR_BULLET,22,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_NUKE:
			SpriteRect(SPR_BULLET,24,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FLY:
			SpriteRect(SPR_MONSTER,31,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FLYSPIT:
			SpriteRect(SPR_BULLET,49,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FROG:
			SpriteRect(SPR_MONSTER,65,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_NUKEBOOM:
			me->rectx=mx-100;
			me->recty=my-100;
			me->rectx2=mx+100;
			me->recty2=my+100;
			break;
		case GUY_MISSILE:
			SpriteRect(SPR_BULLET,1,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_YELLOWBULLET:
			SpriteRect(SPR_BULLET,54,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_STRIDER:
			SpriteRect(SPR_MONSTER,41,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SPIDER:
		case GUY_ICESPIDER:
			SpriteRect(SPR_MONSTER,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_PRESENT:
			SpriteRect(SPR_BULLET,48,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_ROCK:
			SpriteRect(SPR_BULLET,55,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_POWERUP:
			SpriteRect(SPR_BULLET,73,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_CENTIHEAD:
		case GUY_CENTIBODY:
		case GUY_SNOWMAN:
		case GUY_AQUAHEAD:
		case GUY_AQUABODY:
		case GUY_AQUATAIL:
		case GUY_ROBOHEAD:
		case GUY_ROBOBODY:
		case GUY_REPAIR:
		case GUY_SPARKY:
		case GUY_FLAME:
		case GUY_LOBTAIL:
			me->rectx=mx-7;
			me->recty=my-7;
			me->rectx2=mx+7;
			me->recty2=my+7;
			break;
		case GUY_TURRET:
		case GUY_VOLCANO:
		case GUY_ALIENHEAD:
		case GUY_ALIENBODY:
			me->rectx=mx-10;
			me->recty=my-10;
			me->rectx2=mx+10;
			me->recty2=my+10;
			break;
		case GUY_SHROOM:
			me->rectx=mx-7;
			me->recty=my-7;
			me->rectx2=mx+7;
			me->recty2=my+7;
			break;
		case GUY_WINGMAN:
			SpriteRect(SPR_BULLET,47,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_RADIO:
			SpriteRect(SPR_BULLET,17,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SPIDERBOT:
			SpriteRect(SPR_MONSTER,92,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FIREANT:
			me->rectx=mx-14;
			me->recty=my-14;
			me->rectx2=mx+14;
			me->recty2=my+14;
			break;
		case GUY_BIGROID:
			SpriteRect(SPR_MONSTER,166+32,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_MEDROID:
			SpriteRect(SPR_MONSTER,166,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SMLROID:
			SpriteRect(SPR_MONSTER,166+16,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_LOBSTER:
			SpriteRect(SPR_MONSTER,214,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_LLOBCLAW:
			SpriteRect(SPR_MONSTER,215,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->rectx2-=(me->rectx2-me->rectx)/2;	// only the left half counts
			break;
		case GUY_RLOBCLAW:
			SpriteRect(SPR_MONSTER,220,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->rectx+=(me->rectx2-me->rectx)/2;	// only the right half counts
			break;
		case GUY_FIREBALL:
			SpriteRect(SPR_BULLET,59,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_GREENLASER:
			SpriteRect(SPR_BULLET,60,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_GREENLASER2:
			SpriteRect(SPR_BULLET,64+me->anim/2,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SPINSHIP:
			SpriteRect(SPR_MONSTER,229,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_CLAWSHIP:
			SpriteRect(SPR_MONSTER,249,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SMALLSHIP:
			SpriteRect(SPR_MONSTER,250,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SQUIDSHIP:
			SpriteRect(SPR_MONSTER,257,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_PWRUPUFO:
			SpriteRect(SPR_MONSTER,262,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FBALLSHIP:
			SpriteRect(SPR_MONSTER,266,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_DOUBLESHIP:
			SpriteRect(SPR_MONSTER,276,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_TRICERATOPS:
			if(me->facing==0)
				SpriteRect(SPR_MONSTER,277,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			else
				SpriteRect(SPR_MONSTER,286,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->recty+=30;
			break;
		case GUY_PTERODACTYL:
			SpriteRect(SPR_MONSTER,305,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_EYEBALL1:
			me->rectx=mx-45;
			me->recty=my-25;
			me->rectx2=mx+45;
			me->recty2=my+20;
			break;
		case GUY_EYEBALL2:
			me->rectx=mx-25;
			me->recty=my-15;
			me->rectx2=mx+25;
			me->recty2=my+15;
			break;
		case GUY_EYEBALL3:
			me->rectx=mx-20;
			me->recty=my-10;
			me->rectx2=mx+20;
			me->recty2=my+10;
			break;
		case GUY_EYEBALL4:
			me->rectx=mx-15;
			me->recty=my-10;
			me->rectx2=mx+15;
			me->recty2=my+10;
			break;
		case GUY_EYEGUY:
			SpriteRect(SPR_MONSTER,306,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_TUMBLEWEED:
			SpriteRect(SPR_MONSTER,326,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SCORPION:
			SpriteRect(SPR_MONSTER,320,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SANDWORM:
			SpriteRect(SPR_MONSTER,332,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_BABYALIEN:
			SpriteRect(SPR_MONSTER,345,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FLYALIEN:
			SpriteRect(SPR_MONSTER,401,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_REDBULLET:
			SpriteRect(SPR_BULLET,90,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_QUEEN:
			SpriteRect(SPR_QUEEN,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->recty2-=20;
			break;
		case GUY_QUEENHEAD:
			SpriteRect(SPR_QUEEN,4,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_QUEENTAIL:
			SpriteRect(SPR_QUEEN,16,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_ACIDGLOB:
			SpriteRect(SPR_BULLET,91,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_ACIDSPIT:
			SpriteRect(SPR_BULLET,92,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_FLYINGEGG:
			SpriteRect(SPR_BULLET,94,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_BLUEBULLET:
			SpriteRect(SPR_BULLET,99,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SATELLITE:
			SpriteRect(SPR_MONSTER,406,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
	}
}

void BulletExplode(guy_t *me)
{
	switch(me->type)
	{
		case GUY_BULLET:
			me->type=GUY_NONE;
			ExplodeParticles(PART_PURPLE,me->x,me->y,4,4);
			break;
		case GUY_MISSILE:
			me->type=GUY_NONE;
			MakeSound(SND_MISSILEHIT,1000);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			break;
		case GUY_CHAINBULLET:
			me->type=GUY_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,4,2);
			break;
		case GUY_SHOTBULLET:
		case GUY_REDBULLET:
			me->type=GUY_NONE;
			ExplodeParticles(PART_RED,me->x,me->y,4,4);
			break;
		case GUY_BLUEBULLET:
			me->type=GUY_NONE;
			ExplodeParticles(PART_BLUE,me->x,me->y,4,4);
			break;
		case GUY_LASER:
			// doesn't go away
			ExplodeParticles(PART_BLUE,me->x,me->y,4,2);
			break;
		case GUY_NUKE:
			MakeSound(SND_NUKEBOOM,1500);
			me->type=GUY_NUKEBOOM;
			me->anim=0;
			me->dx=0;
			me->dy=0;
			AddParticle(PART_SHOCKWAVE,me->x,me->y,1,6);
			break;
		case GUY_NUKEBOOM:
			// nothing happens
			break;
		case GUY_FLYSPIT:
			ExplodeParticles(PART_GREEN,me->x,me->y,4,4);
			me->type=GUY_NONE;
			break;
		case GUY_YELLOWBULLET:
			me->type=GUY_NONE;
			ExplodeParticles(PART_YELLOW,me->x,me->y,4,6);
			break;
		case GUY_ROCK:
			me->type=GUY_NONE;
			ExplodeParticles(PART_RED,me->x,me->y,2,6);
			ExplodeParticles(PART_RED,me->x,me->y,5,6);
			break;
		case GUY_FLAME:
			// doesn't go away
			ExplodeParticles(PART_RED,me->x,me->y,2,2);
			ExplodeParticles(PART_YELLOW,me->x,me->y,2,2);
			break;
		case GUY_FIREBALL:
			ExplodeParticles(PART_RED,me->x,me->y,2,2);
			ExplodeParticles(PART_RED,me->x,me->y,4,2);
			ExplodeParticles(PART_RED,me->x,me->y,6,2);
			ExplodeParticles(PART_RED,me->x,me->y,8,2);
			me->type=GUY_NONE;
			break;
		case GUY_GREENLASER2:
			// doesn't go away
			ExplodeParticles(PART_GREEN,me->x,me->y,2,5);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,5);
			ExplodeParticles(PART_GREEN,me->x,me->y,6,5);
			break;
		case GUY_POWERUP:
			ExplodeParticles(PART_SPARKLY,me->x,me->y,2,5);
			ExplodeParticles(PART_SPARKLY,me->x,me->y,4,5);
			me->type=GUY_NONE;
			break;
		case GUY_ACIDGLOB:
			ExplodeParticles(PART_GREEN,me->x,me->y,2,5);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,5);
			ExplodeParticles(PART_GREEN,me->x,me->y,6,5);
			me->type=GUY_NONE;
			break;
		case GUY_ACIDSPIT:
			ExplodeParticles(PART_GREEN,me->x,me->y,2,5);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,5);
			me->type=GUY_NONE;
			break;
		case GUY_FLYINGEGG:
			ExplodeParticles(PART_GREY,me->x,me->y,2,4);
			me->type=GUY_NONE;
			PlantShroom((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT);
			break;	
	}
}

void GetHit(guy_t *me)
{
	int sx,sy;

	switch(me->type)
	{
		case GUY_CENTIHEAD:
			PlayerGetPoints(50);
		case GUY_CENTIBODY:
			PlayerGetPoints(50);	// so a centihead is worth 100
			MakeSound(SND_POW,1200);
			if(guy[me->num+1].type==GUY_CENTIBODY)
			{
				guy[me->num+1].type=GUY_CENTIHEAD;
				guy[me->num+1].anim=0;
				guy[me->num+1].parent=255;
				guy[me->num+1].flags=me->flags;
				guy[me->num+1].tx=(guy[me->num+1].x/TILE_WIDTH)>>FIXSHIFT;
				guy[me->num+1].ty=(guy[me->num+1].y/TILE_HEIGHT)>>FIXSHIFT;
				if(guy[me->num+1].flags&GF_GOINGLEFT)
				{
					guy[me->num+1].dx=-guy[me->num+1].speed;
				}
				else
				{
					guy[me->num+1].dx=guy[me->num+1].speed;
				}
				guy[me->num+1].dy=0;
				guy[me->num+1].reload=0;
			}
			sx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			sy=(me->y>>FIXSHIFT)/TILE_HEIGHT;

			TellLevelAboutIt(EVT_MONSDIE,me->type);
			me->type=GUY_NONE;
			PlantShroom(sx,sy);
			break;
		case GUY_AQUAHEAD:
			PlayerGetPoints(50);
		case GUY_AQUABODY:
		case GUY_AQUATAIL:
			PlayerGetPoints(50);	// so an aquahead is worth 100
			MakeSound(SND_POW,1200);
			if(guy[me->num+1].type==GUY_AQUABODY || guy[me->num+1].type==GUY_AQUATAIL)
			{
				guy[me->num+1].type=GUY_AQUAHEAD;
				guy[me->num+1].anim=0;
				guy[me->num+1].parent=255;
				guy[me->num+1].flags=me->flags;
				guy[me->num+1].tx=(guy[me->num+1].x/TILE_WIDTH)>>FIXSHIFT;
				guy[me->num+1].ty=(guy[me->num+1].y/TILE_HEIGHT)>>FIXSHIFT;
				if(guy[me->num+1].flags&GF_GOINGLEFT)
				{
					guy[me->num+1].dx=-guy[me->num+1].speed;
				}
				else
				{
					guy[me->num+1].dx=guy[me->num+1].speed;
				}
				guy[me->num+1].dy=0;
				guy[me->num+1].reload=0;
			}
			sx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			sy=(me->y>>FIXSHIFT)/TILE_HEIGHT;

			TellLevelAboutIt(EVT_MONSDIE,me->type);
			me->type=GUY_NONE;
			PlantShroom(sx,sy);
			break;
		case GUY_ROBOHEAD:
			me->hp--;
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			if(me->hp==0)
			{
				PlayerGetPoints(50);
			}
			else
			{
				MakeSound(SND_PLUNK,1000);
				break;
			}
		case GUY_ROBOBODY:
			if(me->hp>0)
			{
				AddParticle(PART_LILBOOM,me->x,me->y,1,9);
				me->hp--;
				if(me->hp>0)
				{
					MakeSound(SND_PLUNK,1000);
					break;
				}
			}
			PlayerGetPoints(50);	// so a centihead is worth 100
			MakeSound(SND_POW,1200);
			if(guy[me->num+1].type==GUY_ROBOBODY)
			{
				guy[me->num+1].type=GUY_ROBOHEAD;
				guy[me->num+1].anim=GetRand(6);
				guy[me->num+1].parent=255;
				guy[me->num+1].flags=me->flags;
				guy[me->num+1].tx=(guy[me->num+1].x/TILE_WIDTH)>>FIXSHIFT;
				guy[me->num+1].ty=(guy[me->num+1].y/TILE_HEIGHT)>>FIXSHIFT;
				if(guy[me->num+1].flags&GF_GOINGLEFT)
				{
					guy[me->num+1].dx=-guy[me->num+1].speed;
				}
				else
				{
					guy[me->num+1].dx=guy[me->num+1].speed;
				}
				guy[me->num+1].dy=0;
				guy[me->num+1].reload=0;
			}
			sx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			sy=(me->y>>FIXSHIFT)/TILE_HEIGHT;

			TellLevelAboutIt(EVT_MONSDIE,me->type);
			me->type=GUY_NONE;
			PlantShroom(sx,sy);
			break;
		case GUY_ALIENHEAD:
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			PlayerGetPoints(50);
			ExplodeParticles(PART_GREEN,me->x,me->y,2,2);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,2);
		case GUY_ALIENBODY:
			PlayerGetPoints(50);	// so an alienhead is worth 100
			MakeSound(SND_POW,1200);
			ExplodeParticles(PART_GREEN,me->x,me->y,2,2);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,2);
			if(guy[me->num+1].type==GUY_ALIENBODY)
			{
				guy[me->num+1].type=GUY_ALIENHEAD;
				guy[me->num+1].anim=GetRand(20);
				guy[me->num+1].parent=255;
				guy[me->num+1].flags=me->flags;
				guy[me->num+1].tx=(guy[me->num+1].x/TILE_WIDTH)>>FIXSHIFT;
				guy[me->num+1].ty=(guy[me->num+1].y/TILE_HEIGHT)>>FIXSHIFT;
				if(guy[me->num+1].flags&GF_GOINGLEFT)
				{
					guy[me->num+1].dx=-guy[me->num+1].speed;
				}
				else
				{
					guy[me->num+1].dx=guy[me->num+1].speed;
				}
				guy[me->num+1].dy=0;
				guy[me->num+1].reload=0;
			}
			sx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			sy=(me->y>>FIXSHIFT)/TILE_HEIGHT;

			TellLevelAboutIt(EVT_MONSDIE,me->type);
			me->type=GUY_NONE;
			PlantShroom(sx,sy);
			break;
		case GUY_SHROOM:
			MakeSound(SND_PLUNK,1000);
			me->hp--;
			if(me->hp==0)
			{
				TellLevelAboutIt(EVT_MONSDIE,GUY_SHROOM);
				PlayerGetPoints(5);
				me->type=GUY_NONE;
				if(NoMission() && (!radioActive) && GetRand(4)==0)
				{
					// unleash a mission radio
					radioActive=1;
					me->hp=1;
					me->type=GUY_RADIO;
					me->dx=0;
					me->dy=FIXAMT;
					me->anim=0;
					ExplodeParticles(PART_SPARKLY,me->x,me->y,5,5);
					me->flags=GF_GHOST;
				}
				if(GetWorld()==WORLD_FINAL)	// eggs hatch baby aliens
				{
					AddGuyExact(GUY_BABYALIEN,me->x,me->y,0);
				}
				numShrooms--;
			}
			break;
		case GUY_SPIDER:
		case GUY_ICESPIDER:
		case GUY_SPIDERBOT:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			PlayerGetPoints(100);
			MakeSound(SND_POW,1200);
			me->type=GUY_NONE;
			break;
		case GUY_REPAIR:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_YELLOW,me->x,me->y,3,8);
			PlayerGetPoints(100);
			MakeSound(SND_POW,1200);
			me->type=GUY_NONE;
			break;
		case GUY_SNOWMAN:
			me->hp--;
			if(me->hp==0)
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(100);
				MakeSound(SND_POW,1200);
				ExplodeParticles(PART_SNOW,me->x,me->y,2,10);
				me->type=GUY_NONE;
			}
			else
			{
				me->y-=FIXAMT*16;	// knocked back
				ExplodeParticles(PART_SNOW,me->x,me->y,2,3);
				MakeSound(SND_PLUNK,900);
			}
			break;
		case GUY_FLY:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(20);
			MakeSound(SND_POW,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			me->type=GUY_NONE;
			break;
		case GUY_STRIDER:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(100);
			MakeSound(SND_POW,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_BLUE,me->x,me->y,2,5);
			me->type=GUY_NONE;
			break;
		case GUY_FROG:
			me->hp--;
			if(me->hp==0)
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(100);
				MakeSound(SND_POW,1200);
				ExplodeParticles(PART_GREEN,me->x,me->y,3,5);
				me->type=GUY_NONE;
			}
			else
			{
				me->y-=FIXAMT*16;	// knocked back
				ExplodeParticles(PART_GREEN,me->x,me->y,2,3);
				MakeSound(SND_PLUNK,900);
			}
			break;
		case GUY_TURRET:
			if(me->hp>0)
			{
				me->hp--;
				if(me->hp==0)
				{
					TellLevelAboutIt(EVT_MONSDIE,me->type);
					PlayerGetPoints(75);
					MakeSound(SND_BOOM,1200);
					ExplodeParticles(PART_YELLOW,me->x,me->y,4,12);
					me->flags|=GF_GHOST;
				}
				else
				{
					MakeSound(SND_PLUNK,900);
					ExplodeParticles(PART_YELLOW,me->x,me->y,2,6);
				}
			}
			break;
		case GUY_VOLCANO:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,900);
				ExplodeParticles(PART_RED,me->x,me->y,2,6);
			}
			else
			{
				MakeSound(SND_BOOM2,1200);
				AddParticle(PART_BIGBOOM,me->x,me->y,1,19);
				ExplodeParticles(PART_RED,me->x,me->y,2,12);
				ExplodeParticles(PART_YELLOW,me->x,me->y,4,12);
				ExplodeParticles(PART_RED,me->x,me->y,8,12);
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				me->type=GUY_NONE;
			}
			break;
		case GUY_SPARKY:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			MakeSound(SND_POW,1200);
			ExplodeParticles(PART_RED,me->x,me->y,2,4);
			ExplodeParticles(PART_YELLOW,me->x,me->y,4,4);
			ExplodeParticles(PART_RED,me->x,me->y,4,4);
			me->type=GUY_NONE;
			break;
		case GUY_FIREANT:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(100);
			MakeSound(SND_POW,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_RED,me->x,me->y,2,5);
			me->type=GUY_NONE;
			break;
		case GUY_BIGROID:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(50);
			MakeSound(SND_BOOM,1200);
			AddParticle(PART_BIGBOOM,me->x,me->y,1,19);
			me->type=GUY_NONE;
			PlantMonster(GUY_MEDROID,(me->x>>FIXSHIFT)/TILE_WIDTH-1,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			PlantMonster(GUY_MEDROID,(me->x>>FIXSHIFT)/TILE_WIDTH+1,(me->y>>FIXSHIFT)/TILE_HEIGHT);
			break;
		case GUY_MEDROID:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(25);
			MakeSound(SND_BOOM3,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			me->type=GUY_NONE;
			PlantMonster(GUY_SMLROID,(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT-1);
			PlantMonster(GUY_SMLROID,(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT+1);
			break;
		case GUY_SMLROID:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(10);
			MakeSound(SND_BOOM2,1200);
			ExplodeParticles(PART_GREY,me->x,me->y,2,4);
			ExplodeParticles(PART_GREY,me->x,me->y,4,4);
			me->type=GUY_NONE;
			break;
		case GUY_LOBSTER:
			me->hp--;
			if(me->hp==0)
			{
				PlayerGetPoints(500);
				MakeSound(SND_NUKEBOOM,2000);
				AddParticle(PART_SHOCKWAVE,me->x,me->y,1,6);
				for(sx=0;sx<10;sx++)
					AddParticle(PART_BIGBOOM,(me->rectx+GetRand(me->rectx2-me->rectx))<<FIXSHIFT,
								(me->recty+GetRand(me->recty2-me->recty))<<FIXSHIFT,1,19);
				ExplodeParticles(PART_GREY,me->x,me->y,2,7);
				ExplodeParticles(PART_GREY,me->x,me->y,4,7);
				ExplodeParticles(PART_GREY,me->x,me->y,6,7);
				ExplodeParticles(PART_GREY,me->x,me->y,8,7);
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				me->type=GUY_NONE;
				for(sy=0;sy<MAX_GUYS;sy++)
					if(guy[sy].parent==me->num)
						guy[sy].type=GUY_NONE;	// destroy all kids
			}
			else
			{
				if(me->reload2>4)
					me->reload2-=4;
				else
					me->reload2=1;
				MakeSound(SND_POW,1200);
				ExplodeParticles(PART_GREY,me->x,me->y,2,7);
				ExplodeParticles(PART_GREY,me->x,me->y,4,7);
			}
			break;
		case GUY_LLOBCLAW:
		case GUY_RLOBCLAW:
			me->hp--;
			if(me->hp==0)
			{
				for(sx=0;sx<3;sx++)
					AddParticle(PART_BIGBOOM,(me->rectx+GetRand(me->rectx2-me->rectx))<<FIXSHIFT,
								(me->recty+GetRand(me->recty2-me->recty))<<FIXSHIFT,1,19);
				MakeSound(SND_BOOM,1200);
				MakeSound(SND_BOOM2,1200);
				MakeSound(SND_BOOM3,1200);

				if(me->type==GUY_LLOBCLAW)
					sx=me->x-50*FIXAMT;
				else
					sx=me->x+50*FIXAMT;
				ExplodeParticles(PART_GREY,sx,me->y+50*FIXAMT,2,7);
				ExplodeParticles(PART_GREY,sx,me->y+50*FIXAMT,4,7);
				
				me->type=GUY_NONE;
			}
			else
			{
				MakeSound(SND_PLUNK,1200);
				if(me->type==GUY_LLOBCLAW)
					sx=me->x-50*FIXAMT;
				else
					sx=me->x+50*FIXAMT;
				ExplodeParticles(PART_GREY,sx,me->y+50*FIXAMT,2,3);
				ExplodeParticles(PART_GREY,sx,me->y+50*FIXAMT,4,3);
			}
			break;
		case GUY_LOBTAIL:
			// these shouldn't get hit
			break;
		case GUY_SPINSHIP:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(100);
			MakeSound(SND_BOOM2,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_GREY,me->x,me->y,2,5);
			me->type=GUY_NONE;
			break;
		case GUY_CLAWSHIP:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
				ExplodeParticles(PART_GREY,me->x,me->y,2,3);
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(500);
				MakeSound(SND_BOOM,1200);
				MakeSound(SND_BOOM3,1200);
				AddParticle(PART_BIGBOOM,me->x,me->y,1,19);
				ExplodeParticles(PART_GREY,me->x,me->y,2,5);
				ExplodeParticles(PART_YELLOW,me->x,me->y,4,5);
				ExplodeParticles(PART_GREY,me->x,me->y,6,5);
				me->type=GUY_NONE;
			}
			break;
		case GUY_SMALLSHIP:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(100);
			MakeSound(SND_BOOM2,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_GREY,me->x,me->y,2,5);
			me->type=GUY_NONE;
			break;
		case GUY_SQUIDSHIP:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(100);
			MakeSound(SND_BOOM2,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_GREY,me->x,me->y,2,5);
			me->type=GUY_NONE;
			break;
		case GUY_PWRUPUFO:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(50);
			MakeSound(SND_BOOM3,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_GREY,me->x,me->y,2,5);
			AddGuy(GUY_POWERUP,(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT,0);
			me->type=GUY_NONE;
			break;
		case GUY_DOUBLESHIP:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(100);
			MakeSound(SND_BOOM2,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_GREY,me->x,me->y,2,5);
			me->type=GUY_NONE;
			break;
		case GUY_FBALLSHIP:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
				me->facing=1;	// spin in pain
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(100);
				MakeSound(SND_BOOM,1200);
				AddParticle(PART_LILBOOM,me->x,me->y,1,9);
				ExplodeParticles(PART_GREY,me->x,me->y,2,5);
				ExplodeParticles(PART_BLUE,me->x,me->y,2,5);
				me->type=GUY_NONE;
			}
			break;
		case GUY_TRICERATOPS:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(250);
				MakeSound(SND_POW,1200);
				ExplodeParticles(PART_GREY,me->x,me->y,2,10);
				ExplodeParticles(PART_GREY,me->x,me->y,4,10);
				ExplodeParticles(PART_GREY,me->x,me->y,6,5);
				me->type=GUY_NONE;
			}
			break;
		case GUY_PTERODACTYL:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(50);
			MakeSound(SND_POW,1200);
			AddParticle(PART_LILBOOM,me->x,me->y-me->destx,1,9);
			me->type=GUY_NONE;
			break;
		case GUY_EYEBALL1:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
				ExplodeParticles(PART_RED,me->x,me->y,2,10);
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(250);
				MakeSound(SND_SLURP,1200);
				ExplodeParticles(PART_RED,me->x,me->y,2,10);
				ExplodeParticles(PART_RED,me->x,me->y,4,10);
				ExplodeParticles(PART_RED,me->x,me->y,6,5);
				me->type=GUY_NONE;
				AddGuy(GUY_EYEBALL2,(me->x>>FIXSHIFT)/TILE_WIDTH-1,(me->y>>FIXSHIFT)/TILE_HEIGHT-2,1);
				AddGuy(GUY_EYEBALL2,(me->x>>FIXSHIFT)/TILE_WIDTH+1,(me->y>>FIXSHIFT)/TILE_HEIGHT+2,0);
			}
			break;
		case GUY_EYEBALL2:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
				ExplodeParticles(PART_RED,me->x,me->y,2,10);
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(100);
				MakeSound(SND_SLURP,1200);
				ExplodeParticles(PART_RED,me->x,me->y,2,10);
				ExplodeParticles(PART_RED,me->x,me->y,4,10);
				ExplodeParticles(PART_RED,me->x,me->y,6,5);
				me->type=GUY_NONE;
				AddGuy(GUY_EYEBALL3,(me->x>>FIXSHIFT)/TILE_WIDTH-1,(me->y>>FIXSHIFT)/TILE_HEIGHT-2,1);
				AddGuy(GUY_EYEBALL3,(me->x>>FIXSHIFT)/TILE_WIDTH+1,(me->y>>FIXSHIFT)/TILE_HEIGHT+2,0);
			}
			break;
		case GUY_EYEBALL3:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(50);
			MakeSound(SND_SLURP,1200);
			ExplodeParticles(PART_RED,me->x,me->y,2,10);
			ExplodeParticles(PART_RED,me->x,me->y,4,5);
			me->type=GUY_NONE;
			AddGuy(GUY_EYEBALL4,(me->x>>FIXSHIFT)/TILE_WIDTH-1,(me->y>>FIXSHIFT)/TILE_HEIGHT-1,1);
			AddGuy(GUY_EYEBALL4,(me->x>>FIXSHIFT)/TILE_WIDTH+1,(me->y>>FIXSHIFT)/TILE_HEIGHT+1,0);
			break;
		case GUY_EYEBALL4:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(25);
			MakeSound(SND_POW,1200);
			ExplodeParticles(PART_RED,me->x,me->y,2,8);
			me->type=GUY_NONE;
			sx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			sy=(me->y>>FIXSHIFT)/TILE_HEIGHT;
			PlantShroom(sx,sy);
			break;
		case GUY_EYEGUY:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
				ExplodeParticles(PART_YELLOW,me->x,me->y,2,10);
				me->speed+=FIXAMT;
				if(me->dx>0)
					me->dx=-me->speed;
				else
					me->dx=me->speed;	// speed up and switch direction
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(250);
				MakeSound(SND_POW,1200);
				ExplodeParticles(PART_YELLOW,me->x,me->y,2,10);
				ExplodeParticles(PART_YELLOW,me->x,me->y,4,10);
				ExplodeParticles(PART_YELLOW,me->x,me->y,6,5);
				me->type=GUY_NONE;
			}
			break;
		case GUY_SCORPION:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,1200);
				ExplodeParticles(PART_YELLOW,me->x,me->y,2,3);
			}
			else
			{
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				PlayerGetPoints(50);
				MakeSound(SND_POW,1200);
				AddParticle(PART_LILBOOM,me->x,me->y,1,9);
				ExplodeParticles(PART_YELLOW,me->x,me->y,2,5);
				ExplodeParticles(PART_YELLOW,me->x,me->y,4,3);
				me->type=GUY_NONE;
			}
			break;
		case GUY_TUMBLEWEED:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(50);
			MakeSound(SND_POW,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			me->type=GUY_NONE;
			break;
		case GUY_SANDWORM:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(150);
			MakeSound(SND_POW,1200);
			AddParticle(PART_LILBOOM,me->x,me->y,1,9);
			ExplodeParticles(PART_GREY,me->x,me->y,2,10);
			ExplodeParticles(PART_GREY,me->x,me->y,4,10);
			me->type=GUY_NONE;
			break;
		case GUY_BABYALIEN:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(10);
			MakeSound(SND_BABYDIE,1200);
			ExplodeParticles(PART_GREEN,me->x,me->y,2,3);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,3);
			me->type=GUY_NONE;
			break;
		case GUY_FLYALIEN:
			TellLevelAboutIt(EVT_MONSDIE,me->type);
			PlayerGetPoints(50);
			MakeSound(SND_BABYDIE,1200);
			ExplodeParticles(PART_GREEN,me->x,me->y,2,6);
			ExplodeParticles(PART_GREEN,me->x,me->y,4,5);
			me->type=GUY_NONE;
			break;
		case GUY_QUEEN:
			if(me->desty==0)	// still has a head, can't be harmed
			{
				MakeSound(SND_CLINK,1000);
				break;
			}
			me->hp--;
			if(me->hp>0)
			{
				if(me->speed>0)
					me->speed=(51-me->hp)/2;
				else
					me->speed=-(51-me->hp)/2;
				MakeSound(SND_QUEENROAR,1200);
				ExplodeParticles(PART_GREEN,me->x,me->y+FIXAMT*30,2,6);
				ExplodeParticles(PART_GREEN,me->x,me->y+FIXAMT*30,4,5);
			}
			else	// the queen is dead!!!
			{
				PlayerGetPoints(5000);
				MakeSound(SND_QUEENDIE,5000);
				me->reload=120;
				me->flags|=GF_GHOST;
				for(sy=0;sy<MAX_GUYS;sy++)
				{
					if(guy[sy].parent==me->num)
						guy[sy].type=GUY_NONE;	// destroy all kids
				}
			}
			break;
		case GUY_QUEENHEAD:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_QUEENROAR,1200);
				ExplodeParticles(PART_GREEN,me->x,me->y,2,6);
				ExplodeParticles(PART_GREEN,me->x,me->y,4,5);
			}
			else	// the queen's head is dead!!!
			{
				PlayerGetPoints(500);
				MakeSound(SND_QUEENHEADDIE,5000);
				ExplodeParticles(PART_GREEN,me->x,me->y,2,20);
				ExplodeParticles(PART_GREEN,me->x,me->y,4,20);
				ExplodeParticles(PART_GREEN,me->x,me->y,6,20);
				for(sx=0;sx<4;sx++)
					AddParticle(PART_BIGBOOM,(me->rectx+GetRand(me->rectx2-me->rectx))<<FIXSHIFT,
								(me->recty+GetRand(me->recty2-me->recty))<<FIXSHIFT,1,19);
				ExplodeParticles(PART_GREY,me->x,me->y,2,7);
				ExplodeParticles(PART_GREY,me->x,me->y,4,7);
				TellLevelAboutIt(EVT_MONSDIE,me->type);
				me->type=GUY_NONE;
				if(me->parent!=-1)
					guy[me->parent].desty=1;	// tell it it no longer has a head
			}
			break;
		case GUY_QUEENTAIL:
			// should never get hit
			break;
		case GUY_SATELLITE:
			me->hp--;
			if(me->hp>0)
			{
				MakeSound(SND_PLUNK,800);
				ExplodeParticles(PART_GREY,me->x,me->y,2,3);
			}
			else
			{
				MakeSound(SND_BOOM2,800);
				ExplodeParticles(PART_GREY,me->x,me->y,2,3);
				ExplodeParticles(PART_GREY,me->x,me->y,4,5);
				ExplodeParticles(PART_GREY,me->x,me->y,6,3);
				AddParticle(PART_LILBOOM,me->x,me->y,1,9);
				me->type=GUY_NONE;
				if(!NaughtyTime())
					AddGuy(GUY_POWERUP,(me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT,0);
			}
			break;
		// now the goodguys
		case GUY_WINGMAN:
			MakeSound(SND_BOOM2,1200);
			AddParticle(PART_LILBOOM,me->x,me->y-8*FIXAMT,1,9);
			ExplodeParticles(PART_RED,me->x,me->y-8*FIXAMT,5,10);
			me->type=GUY_NONE;
			SetWingmen(GetWingmen()-1);
			break;
		case GUY_PLAYER:
			if(!GetShield())
			{
				MakeSound(SND_DIE,1400);
				AddParticle(PART_BIGBOOM,me->x,me->y-8*FIXAMT,1,19);
				ExplodeParticles(PART_RED,me->x,me->y-8*FIXAMT,10,25);
				ExplodeParticles(PART_RED,me->x,me->y-8*FIXAMT,3,25);
				PlayerDeathReset();
				me->flags|=GF_GHOST;
				if(AddLife(-1))
				{
					GameOver();
				}
			}
			else
			{
				// shield protected me
				MakeSound(SND_SHIELDHIT,1400);
				SetShield(0);
				SetInvincible(30);
				me->flags|=GF_GHOST;
			}
			break;
		// enemy bullets:
		case GUY_FLYSPIT:
		case GUY_ROCK:
		case GUY_YELLOWBULLET:
		case GUY_FLAME:
		case GUY_FIREBALL:
		case GUY_GREENLASER2:
		case GUY_REDBULLET:
		case GUY_ACIDGLOB:
		case GUY_ACIDSPIT:
		case GUY_FLYINGEGG:
		case GUY_BLUEBULLET:
			BulletExplode(me);
			break;
	}
}

byte CheckEnemyHit(guy_t *me)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && (i!=me->num) && (guy[i].flags&GF_EVIL))
		{
			if(guy[i].flags&GF_GHOST)
				continue;
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				// hit!
				GetHit(&guy[i]);
				return 1;
			}
		}
	}
	return 0;
}

byte NukeHit(guy_t *me)
{
	int i,retval;

	retval=0;

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && (i!=me->num) && (guy[i].flags&GF_EVIL))
		{
			if(guy[i].flags&GF_GHOST)
				continue;
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				// hit!
				GetHit(&guy[i]);
				retval=1;
			}
		}
	}
	return retval;
}

byte CheckGoodguyHit(guy_t *me)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && (guy[i].flags&GF_GOOD))
		{
			if(guy[i].flags&GF_GHOST)
				continue;
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				// hit!
				GetHit(&guy[i]);
				return 1;
			}
		}
	}
	return 0;
}

byte PlayerCheckMovement(guy_t *me)
{
	int i;

	if(me->rectx<0 || me->rectx2>SCR_WIDTH || me->recty2>SCR_HEIGHT || me->recty<SCR_HEIGHT-(SCR_HEIGHT/4))
		return 1;	// can't go offscreen

	if(GetInvincible())
		return 0;	// can't bump into things when invincible

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && (guy[i].flags&(GF_EVIL|GF_EVILMEAN)))
		{
			if(guy[i].flags&GF_GHOST)
				continue;
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				// check here to see if the guy you bumped was somebody who hurts you
				if(guy[i].flags&GF_EVILMEAN)
				{
					GetHit(&guy[i]);
					GetHit(me);
				}
				return 1;
			}
		}
	}
	return 0;
}

byte MonsterCheckWallsOnly(guy_t *me)
{
	int topy;

	topy=0+(SCR_HEIGHT-(SCR_HEIGHT/4))*((me->flags&GF_HALFSCRN)>0);
	if(!(me->flags&GF_OFFSCREEN))
		if(me->rectx<0 || me->rectx2>=SCR_WIDTH || me->recty2>=SCR_HEIGHT || me->recty<=topy)
			return 1;	// can't go offscreen

	return 0;
}

byte MonsterCheckMovement(guy_t *me)
{
	int i,topy,j;

	wasHeadBonk=0;

	topy=0+(SCR_HEIGHT-(SCR_HEIGHT/4))*((me->flags&GF_HALFSCRN)>0);
	if(!(me->flags&GF_OFFSCREEN))
		if(me->rectx<0 || me->rectx2>=SCR_WIDTH || me->recty2>=SCR_HEIGHT || me->recty<=topy)
			return 1;	// can't go offscreen

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && (&guy[i]!=me))
		{
			if((me->flags&GF_FIXTURRET) && (guy[i].type==GUY_TURRET) && (guy[i].hp==0))
			{
				if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
					me->recty2>=guy[i].recty)
				{
					guy[i].hp=2;
					guy[i].flags&=(~GF_GHOST);
					MakeSound(SND_TURRETFIXED,1500);
					ExplodeParticles(PART_YELLOW,guy[i].x,guy[i].y,5,10);
				}
				continue;
			}

			if(guy[i].flags&GF_GHOST)
				continue;
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				if(guy[i].parent!=255)
				{
					// if this guy turns out to be my child, then no collision
					j=guy[i].parent;
					topy=0;
					while(j!=255)
					{
						if(j==me->num || j==guy[i].num)
						{
							topy=1;
							break;
						}
						j=guy[j].parent;
					}
					if(topy)
						continue;	// it was my child
				}
				if(guy[i].flags&GF_FLYER)
					continue;	// can't bump a flyer

				// check here to see if the guy you bumped was somebody who hurts you
				if((guy[i].flags&GF_GOODMEAN) && (!(me->flags&GF_SANDWORM)))
				{
					BulletExplode(&guy[i]);
					GetHit(me);
				}
				// now check to see if it was some whiny loser, easily slain
				if((me->flags&GF_EVILMEAN) && (guy[i].flags&GF_GOOD))
				{
					GetHit(&guy[i]);
					GetHit(me);
				}
				// eat the shrooms you step on
				if((me->flags&GF_SHROOMEAT) && (guy[i].type==GUY_SHROOM))
				{
					guy[i].type=GUY_NONE;
					numShrooms--;
				}
				// fix the shrooms you step on
				if((me->flags&GF_FIXSHROOM) && (guy[i].type==GUY_SHROOM))
				{
					guy[i].hp=4;
				}
				if(me->flags&GF_SANDWORM)
				{
					if(!(guy[i].flags&(GF_GOOD|GF_EVIL)))
						return 0;	// doesn't bump bullets
				}
				if((me->type==GUY_CENTIHEAD || me->type==GUY_AQUAHEAD ||
					me->type==GUY_ROBOHEAD || me->type==GUY_ALIENHEAD ||
					me->type==GUY_EYEBALL1 || me->type==GUY_EYEBALL2 ||
					me->type==GUY_EYEBALL3 || me->type==GUY_EYEBALL4) &&
				    (guy[i].type==GUY_CENTIHEAD || guy[i].type==GUY_AQUAHEAD ||
					guy[i].type==GUY_ROBOHEAD || guy[i].type==GUY_ALIENHEAD ||
					guy[i].type==GUY_EYEBALL1 || guy[i].type==GUY_EYEBALL2 ||
					guy[i].type==GUY_EYEBALL3 || guy[i].type==GUY_EYEBALL4 ||
					guy[i].type==GUY_CENTIBODY || guy[i].type==GUY_AQUABODY ||
					guy[i].type==GUY_ROBOBODY || guy[i].type==GUY_ALIENBODY))
				{
					wasHeadBonk=1;
					// if both centipedes, basically
					if((guy[i].flags&GF_GOINGLEFT)==(me->flags&GF_GOINGLEFT))
					{
						if(me->rectx-(me->dx>>FIXSHIFT)>0 && me->rectx2-(me->dx>>FIXSHIFT)<SCR_WIDTH)
							me->x-=me->dx;
					}
				}
				return 1;
			}
		}
	}
	return 0;
}

void FireWeapon(guy_t *me)
{
	if(me->type==GUY_PLAYER)
	{
		switch(GetWeapon())
		{
			case WPN_NONE:
				break;
			case WPN_MISSILE:
				AddGuyExact(GUY_MISSILE,me->x,me->y-FIXAMT*20,0);
				AddGuyExact(GUY_MISSILE,me->x,me->y-FIXAMT*20,0);
				AddGuyExact(GUY_MISSILE,me->x,me->y-FIXAMT*20,0);
				AddGuyExact(GUY_MISSILE,me->x,me->y-FIXAMT*20,0);
				AddGuyExact(GUY_MISSILE,me->x,me->y-FIXAMT*20,0);
				MakeSound(SND_MISSILE,1000);
				me->reload2=15;
				UseAmmo(5);
				break;
			case WPN_CHAINGUN:
				MakeSound(SND_CHAINGUN,800);
				AddGuyExact(GUY_CHAINBULLET,me->x-GetRand(FIXAMT*8)+FIXAMT*4,
					me->y-GetRand(FIXAMT*8)-FIXAMT*12,0);
				me->reload2=0;
				UseAmmo(1);
				break;
			case WPN_LASER:
				MakeSound(SND_LASER,1000);
				AddGuyExact(GUY_LASER,me->x,me->y-32*FIXAMT,0);
				me->reload2=10;
				UseAmmo(1);
				break;
			case WPN_NUKE:
				MakeSound(SND_NUKELAUNCH,1000);
				AddGuyExact(GUY_NUKE,me->x,me->y-32*FIXAMT,0);
				me->reload2=30;
				UseAmmo(1);
				break;
			case WPN_SHOTGUN:
				MakeSound(SND_SHOTGUN,1000);
				AddGuyExact(GUY_SHOTBULLET,me->x,me->y-FIXAMT*20,0);
				AddGuyExact(GUY_SHOTBULLET,me->x,me->y-FIXAMT*20,1);
				AddGuyExact(GUY_SHOTBULLET,me->x,me->y-FIXAMT*20,2);
				AddGuyExact(GUY_SHOTBULLET,me->x,me->y-FIXAMT*20,3);
				AddGuyExact(GUY_SHOTBULLET,me->x,me->y-FIXAMT*20,4);
				me->reload2=15;
				UseAmmo(1);
				break;
		}
	}
}
// here are the update functions for the different GUY types
//--------------------------------------------------------------

void UpdatePlayer(guy_t *me)
{
	int msx,msy;
	byte keys;
	static byte ticker=0;

	if(ticker)
		ticker--;
	else
		ticker=2;

	if(me->reload)
		me->reload--;
	if(me->reload2)
		me->reload2--;

	GetMouse(&msx,&msy);
	keys=GetControls();

	if(keys&CONTROL_UP)
		msy=-20;
	if(keys&CONTROL_DN)
		msy=20;
	if(keys&CONTROL_LF)
		msx=-20;
	if(keys&CONTROL_RT)
		msx=20;

	if(GetInvincible()>0)
		me->flags|=GF_GHOST;
	else
		me->flags&=(~GF_GHOST);

	me->dx=(msx<<FIXSHIFT)/2;
	me->dy=(msy<<FIXSHIFT)/2;
	
	Clamp(&me->dx,FIXAMT*16);
	Clamp(&me->dy,FIXAMT*12);


	if(IsBonusStage() || GetWorld()==WORLD_ASTEROID)
	{
		// using rocket graphics
		if(me->dx<0 && me->facing>15)
		{
			me->facing-=4;
			if(me->facing>128)
				me->facing-=12;	
		}
		if(me->dx>0 && me->facing<255-15)
		{
			me->facing+=4;
			if(me->facing<128)
				me->facing+=12;	
		}
		if(me->dx==0)
		{
			if(me->facing>128)
				me->facing-=8;
			if(me->facing<128)
				me->facing+=8;
		}
		if(me->facing<128-7)
			me->anim=8-(me->facing/32);
		else if(me->facing>128+7)
			me->anim=(me->facing/32)-4;
		else
			me->anim=0;

		// spew flame particles
		AddParticleExact(PART_ENGINEGLOW,me->x-8*FIXAMT+GetRand(16*FIXAMT),
						 me->y+4*FIXAMT,0,0/*-FIXAMT+GetRand(FIXAMT*2)*/,FIXAMT/2+GetRand(FIXAMT/2),0,30);
		AddParticleExact(PART_ENGINEGLOW,me->x-8*FIXAMT+GetRand(16*FIXAMT),
						 me->y+4*FIXAMT,0,0/*-FIXAMT+GetRand(FIXAMT*2)*/,FIXAMT/2+GetRand(FIXAMT/2),0,30);
	}
	else
	{
		// using hover-eddie graphics
		if(me->dx<0 && me->facing>=8)
		{
			me->facing-=8;
			if(me->facing>128)
				me->facing-=12;	
		}
		if(me->dx>0 && me->facing<=255-8)
		{
			me->facing+=8;
			if(me->facing<128)
				me->facing+=12;	
		}
		if(me->dx==0)
		{
			if(me->facing>128)
				me->facing-=4;
			if(me->facing<128)
				me->facing+=4;
		}
		me->anim=13+((me->facing-128)/32);
		if(ticker==0)
			AddParticleExact(PART_SPUTTER,me->x,me->y-18*FIXAMT,0,0,FIXAMT*6,0,5);
	}

	me->x+=me->dx;
	UpdateGuyRect(me);
	while(me->dx && PlayerCheckMovement(me))
	{
		me->x-=me->dx;
		Dampen(&me->dx,FIXAMT);
		me->x+=me->dx;
		UpdateGuyRect(me);
	}
	
	me->y+=me->dy;
	UpdateGuyRect(me);
	while(me->dy && PlayerCheckMovement(me))
	{
		me->y-=me->dy;
		Dampen(&me->dy,FIXAMT);
		me->y+=me->dy;
		UpdateGuyRect(me);
	}
	
	if((MouseHeld() || (keys&CONTROL_B1)) && !me->reload)
	{
		if(IsBonusStage() || GetWorld()==WORLD_ASTEROID)
			AddGuyExact(GUY_BULLET,me->x,me->y-FIXAMT*8,0);
		else
			AddGuyExact(GUY_BULLET,me->x+8*FIXAMT,me->y-FIXAMT*8,0);

		MakeSound(SND_SHOOT,1000);
		TellLevelAboutIt(EVT_FIRE,0);
		me->reload=10-3*GetPower();	// more firepower=less reload time
	}
	if((RMouseHeld() || (keys&CONTROL_B2)) && !me->reload2)
	{
		FireWeapon(me);
		TellLevelAboutIt(EVT_FIRE,0);
	}
}

void UpdateBullet(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	if(MonsterCheckWallsOnly(me))
		me->type=GUY_NONE;
	if(CheckEnemyHit(me))
	{
		BulletExplode(me);
	}
}

void UpdateNukeBoom(guy_t *me)
{
	int i;
	me->anim++;
	if(me->anim>27)
		me->type=GUY_NONE;

	if(me->anim==3)
	{
		for(i=0;i<30;i++)
			if(NukeHit(me))
				BulletExplode(me);
	}
}

void UpdateLaser(guy_t *me)
{
	if(me->y<0)
		me->type=GUY_NONE;

	me->reload--;
	if(!me->reload)
	{
		me->anim++;
		if(me->anim==1)
		{
			if(CheckEnemyHit(me))
			{
				BulletExplode(me);
			}
			AddGuyExact(GUY_LASER,me->x,me->y-32*FIXAMT,0);
			me->reload=8;
		}
		else if(me->anim==2)
			me->reload=1;
		else
			me->type=GUY_NONE;
	}
}

void UpdateMissile(guy_t *me)
{
	me->y+=me->dy;
	me->x+=me->dx;
	
	if(me->dy>-FIXAMT*10)
		me->dy-=FIXAMT/2;

	if(me->y<0)
		me->type=GUY_NONE;

	if(me->x<goodguy->x)
		me->dx+=GetRand(FIXAMT*2);
	if(me->x>goodguy->x)
		me->dx-=GetRand(FIXAMT*2);
	
	Clamp(&me->dx,FIXAMT*8);

	AddParticle(PART_SMOKE,me->x-me->dx,me->y-me->dy,FIXAMT/4,8*3+7);
	if(me->dy<-FIXAMT*4)
		AddParticle(PART_SMOKE,me->x-me->dx-me->dx/2,me->y-me->dy-me->dy/2,FIXAMT/4,8*3+7);

	if(CheckEnemyHit(me))
	{
		BulletExplode(me);
	}
}

void UpdateCentihead(guy_t *me)
{
	int i,olddx;
	
	for(i=LOG_LENGTH-1;i>0;i--)
		me->movelog[i]=me->movelog[i-1];

	i=0;
	olddx=me->dx;

	if(me->type==GUY_AQUAHEAD || me->type==GUY_ROBOHEAD || me->type==GUY_ALIENHEAD)
	{
		me->anim++;
	}

	if(me->type==GUY_ALIENHEAD && me->reload2>0)
	{
		me->reload2--;
		if(me->reload2==0)
			me->flags&=(~GF_GHOST);
	}

	if(me->dy==0)
	{
		me->x+=me->dx;
		UpdateGuyRect(me);
		if(me->type!=GUY_ALIENHEAD || me->reload2==0)
			while(me->dx && MonsterCheckMovement(me))
			{
				i=1;
				me->x-=me->dx;
				Dampen(&me->dx,FIXAMT);
				me->x+=me->dx;
				UpdateGuyRect(me);
			}
	}
	else	// if moving vertically, you use a more lax collision checker
	{
		me->x+=me->dx;
		UpdateGuyRect(me);
		while(me->dx && MonsterCheckWallsOnly(me))
		{
			i=1;
			me->x-=me->dx;
			Dampen(&me->dx,FIXAMT);
			me->x+=me->dx;
			UpdateGuyRect(me);
		}
	}
	me->dx=olddx;
	// no collision detection vertically
	me->y+=me->dy;
	UpdateGuyRect(me);

	me->movelog[0].dx=me->dx;
	me->movelog[0].dy=me->dy;
	me->movelog[0].x=me->x;
	me->movelog[0].y=me->y;
	if(me->dx>0)
	{
		if(me->dy<0)
			me->movelog[0].facing=8;
		else if(me->dy>0)
			me->movelog[0].facing=2;
		else
			me->movelog[0].facing=1;
	}
	else if(me->dx<0)
	{
		if(me->dy<0)
			me->movelog[0].facing=6;
		else if(me->dy>0)
			me->movelog[0].facing=4;
		else
			me->movelog[0].facing=5;
	}
	else
	{
		if(me->dy<0)
			me->movelog[0].facing=7;
		else if(me->dy>0)
			me->movelog[0].facing=3;
		else
			me->movelog[0].facing=0;
	}
	if(me->movelog[0].facing>0)
		me->facing=me->movelog[0].facing-1;
	else me->facing=2;

	if(me->facing!=0 && me->facing!=4)	// not moving sideways
	{
		me->reload--;
		if(!me->reload)
		{
			me->dy=0;
			if(me->flags&GF_GOINGLEFT)
				me->dx=-me->speed;
			else
				me->dx=me->speed;
		}
		return;		// can't hit obstacles moving vertically
	}
	if(i==1)	// hit an obstacle
	{
		if(wasHeadBonk && GetRand(5)==0)	// 20% chance that you won't go down on a head bonk, so
											// as to limit head-lock problems
		{
			if(me->flags&GF_GOINGLEFT)
				me->dx=me->speed;
			else
				me->dx=-me->speed;
			me->reload=GetLogLength(me->speed);
			if(me->flags&GF_GOINGLEFT)
				me->flags&=(~GF_GOINGLEFT);
			else
				me->flags|=GF_GOINGLEFT;
			me->dy=0;
		}
		else
		{
			if(me->flags&GF_GOINGLEFT)
				me->dx=me->speed/2;
			else
				me->dx=-me->speed/2;
			me->x+=me->dx;
			if(me->flags&GF_GOINGUP)
			{
				me->dy=-me->speed;
			}
			else
			{
				me->dy=me->speed;
			}
			me->reload=GetLogLength(me->speed);
			if(me->flags&GF_GOINGLEFT)
				me->flags&=(~GF_GOINGLEFT);
			else
				me->flags|=GF_GOINGLEFT;

			if((me->y>>FIXSHIFT)<TILE_HEIGHT*3)
				me->flags&=(~GF_GOINGUP);
			if((me->y>>FIXSHIFT)>SCR_HEIGHT-TILE_HEIGHT*3)
				me->flags|=GF_GOINGUP;

			// centipedes loop at 1/3 up from the bottom, to make them more dangerous than
			// if they went all the way back to the top
			if((me->y>>FIXSHIFT)<SCR_HEIGHT-SCR_HEIGHT/3 && me->flags&GF_GOINGUP)
				me->flags&=(~GF_GOINGUP);
		}
	}
}

void UpdateCentibody(guy_t *me)
{
	int i;

	me->flags=guy[me->parent].flags;

	for(i=LOG_LENGTH-1;i>0;i--)
		me->movelog[i]=me->movelog[i-1];

	if(me->type!=GUY_AQUABODY)
		me->movelog[0]=guy[me->parent].movelog[GetLogLength(me->speed)];
	else
		me->movelog[0]=guy[me->parent].movelog[GetLogLength(me->speed)+1];

	me->x=me->movelog[i].x;
	me->y=me->movelog[i].y;
	
	if(me->movelog[0].facing>0)
		me->facing=me->movelog[0].facing-1;
	else me->facing=2;

	me->anim++;

	if(me->type==GUY_ALIENBODY)
	{
		if(me->anim>11)
			me->anim=0;
	}

	if(me->type==GUY_AQUABODY)
	{
		i=guy[me->num+1].type;
		if(i!=GUY_AQUABODY && i!=GUY_AQUATAIL)
			me->type=GUY_AQUATAIL;
	}
}

void UpdateSpider(guy_t *me)
{
	byte v;

	if(me->flags&GF_OFFSCREEN)
	{
		if(me->rectx>=0 && me->recty>=0 && me->rectx2<SCR_WIDTH && me->recty2<SCR_HEIGHT)
			me->flags&=(~GF_OFFSCREEN);	// now that you're on, you can't be off anymore
	}

	me->anim++;
	if(me->anim>=12)
	{
		MakeSound(SND_SPIDER,300);
		me->anim=0;
	}

	v=0;

	me->x+=me->dx;
	UpdateGuyRect(me);
	while(me->dx && MonsterCheckMovement(me))
	{
		v=1;
		me->x-=me->dx;
		Dampen(&me->dx,FIXAMT);
		me->x+=me->dx;
		UpdateGuyRect(me);
	}

	me->y+=me->dy;
	UpdateGuyRect(me);
	while(me->dy && MonsterCheckMovement(me))
	{
		v=1;
		me->y-=me->dy;
		Dampen(&me->dy,FIXAMT);
		me->y+=me->dy;
		UpdateGuyRect(me);
	}

	if(v && (!(me->flags&GF_OFFSCREEN)))
		me->reload=0;	// if you hit an obstacle, then definitely new direction time

	if(me->reload)
		me->reload--;
	else	// head in a new direction
	{
		if(!(me->flags&GF_OFFSCREEN))
		{
			v=GetRand(8);
			me->dx=0;
			me->dy=0;
			if(v==0 || v==1 || v==7)
				me->dx=me->speed;
			if(v==1 || v==2 || v==3)
				me->dy=me->speed;
			if(v==3 || v==4 || v==5)
				me->dx=-me->speed;
			if(v==5 || v==6 || v==7)
				me->dy=-me->speed;
			me->reload=10+GetRand(20);
		}
		else
		{
			// force him to head onto the screen if he's off
			if((me->x>>FIXSHIFT)>SCR_WIDTH/2)
				me->dx=-me->speed;
			else
				me->dx=me->speed;
			if((me->y>>FIXSHIFT)>SCR_HEIGHT-(SCR_HEIGHT/8))
				me->dy=-me->speed;
			else
				me->dy=me->speed;
			me->reload=2;
		}
	}
}

void UpdateRadio(guy_t *me)
{
	me->y+=me->dy;
	me->anim++;
	if(me->anim>3)
		me->anim=0;
	if((me->y>>FIXSHIFT)>SCR_HEIGHT)
	{
		me->type=GUY_NONE;
		radioActive=0;
	}

	if(me->rectx<goodguy->rectx2 && me->recty<goodguy->recty2 && me->rectx2>goodguy->rectx &&
		me->recty2>goodguy->recty)
	{
		AssignMission();	// player picked me up!
		ExplodeParticles(PART_SPARKLY,me->x,me->y,5,5);
		me->type=GUY_NONE;
		radioActive=0;
	}
}

void UpdatePowerup(guy_t *me)
{
	me->y+=me->dy;
	me->x+=me->dx;
	if(me->x>me->destx)
		me->dx-=FIXAMT/2;
	if(me->x<me->destx)
		me->dx+=FIXAMT/2;

	me->anim++;
	if(me->anim>15)
		me->anim=0;
	if((me->y>>FIXSHIFT)>SCR_HEIGHT)
	{
		me->type=GUY_NONE;
	}

	if(me->rectx<goodguy->rectx2 && me->recty<goodguy->recty2 && me->rectx2>goodguy->rectx &&
		me->recty2>goodguy->recty)
	{
		// give a power up
		MonitorSwitch(1);
		MissionWin(2);
		BulletExplode(me);
	}
}

void UpdatePresent(guy_t *me)
{
	if(NoMission())
		me->type=GUY_NONE;	// go away when the mission ends

	if(me->rectx<goodguy->rectx2 && me->recty<goodguy->recty2 && me->rectx2>goodguy->rectx &&
		me->recty2>goodguy->recty)
	{
		ExplodeParticles(PART_SPARKLY,me->x,me->y,5,5);
		me->type=GUY_NONE;
		TellLevelAboutIt(EVT_GET,GUY_PRESENT);
	}
}

void UpdateWingMan(guy_t *me)
{
	if(me->reload)
		me->reload--;

	me->x+=me->dx;
	me->y+=me->dy;

	me->destx=goodguy->x;
	me->desty=goodguy->y;

	if(me->reload2)	// I belong on the right
	{
		if(GetWingmen()==1)
			me->reload2=0;	// if I'm the only one left, switch sides in case a new one is obtained
		me->destx+=FIXAMT*32;
	}
	else
		me->destx-=FIXAMT*32;

	if(me->x<me->destx)
		me->dx+=FIXAMT;
	if(me->x>me->destx)
		me->dx-=FIXAMT;
	if(me->y<me->desty)
		me->dy+=FIXAMT;
	if(me->y>me->desty)
		me->dy-=FIXAMT;

	if(me->x<me->destx && me->dx<0)
		me->dx+=FIXAMT;
	if(me->x>me->destx && me->dx>0)
		me->dx-=FIXAMT;
	if(me->y<me->desty && me->dy<0)
		me->dy+=FIXAMT;
	if(me->y>me->desty && me->dy>0)
		me->dy-=FIXAMT;

	Dampen(&me->dx,FIXAMT/2);
	Dampen(&me->dy,FIXAMT/2);

	Clamp(&me->dx,FIXAMT*8);
	Clamp(&me->dy,FIXAMT*8);

	if(goodguy->reload==3 && me->reload==0)	// ready to fire
	{
		AddGuyExact(GUY_BULLET,me->x,me->y-10*FIXAMT,0);
		MakeSound(SND_SHOOT,1000);
		me->reload=10;
	}
}

void UpdateSnowman(guy_t *me)
{
	me->y+=me->dy;
	MonsterCheckMovement(me);	// just to make him hurt enemies and get shot, not to impede his
								// movement
	if((me->y>>FIXSHIFT)>SCR_HEIGHT)
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
		return;
	}
	if(me->y>=me->desty)
	{
		me->desty+=FIXAMT*16;
		if(GetRand(6)==0)
			PlantShroom((me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_WIDTH-1);
	}
	me->anim++;
	if(me->anim>11)
		me->anim=0;
}

void UpdateFly(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;

	if(me->reload)
		me->reload--;
	else
	{
		// shoot
		MakeSound(SND_FLYSPIT,1000);
		AddGuyExact(GUY_FLYSPIT,me->x,me->y+FIXAMT*10,0);
		me->reload=30+GetRand(60);
		// pick a new destination
		me->destx=GetRand(SCR_WIDTH)<<FIXSHIFT;
		me->desty=GetRand(SCR_HEIGHT)<<FIXSHIFT;
	}
	if(me->x<me->destx)
		me->dx+=FIXAMT;
	if(me->y<me->desty)
		me->dy+=FIXAMT;
	if(me->x>me->destx)
		me->dx-=FIXAMT;
	if(me->y>me->desty)
		me->dy-=FIXAMT;

	Clamp(&me->dx,FIXAMT*6);
	Clamp(&me->dy,FIXAMT*6);

	me->anim+=me->reload2-1;
	if(me->anim==2)
		me->reload2=0;
	if(me->anim==0)
		me->reload2=2;
}

void UpdateEnemyBullet(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;

	if(me->type==GUY_ACIDSPIT)
	{
		me->anim++;
		AddParticleExact(PART_GREEN,me->x,me->y,8*FIXAMT,FIXAMT-GetRand(FIXAMT*2),FIXAMT-GetRand(FIXAMT*2),
			0,30);
	}
	
	if(CheckGoodguyHit(me))
		BulletExplode(me);
	if(me->x<0 || me->y<0 || me->x>SCR_WIDTH*FIXAMT || me->y>SCR_HEIGHT*FIXAMT)
		me->type=GUY_NONE;
}

void UpdateStrider(guy_t *me)
{
	if(me->flags&GF_OFFSCREEN)
	{
		if(me->rectx>=0 && me->recty>=0 && me->rectx2<SCR_WIDTH && me->recty2<SCR_HEIGHT)
			me->flags&=(~GF_OFFSCREEN);	// now that you're on, you can't be off anymore
	}

	me->x+=me->dx;
	UpdateGuyRect(me);
	while(me->dx && MonsterCheckMovement(me))
	{
		me->x-=me->dx;
		Dampen(&me->dx,FIXAMT);
		me->x+=me->dx;
		UpdateGuyRect(me);
	}

	me->y+=me->dy;
	UpdateGuyRect(me);
	while(me->dy && MonsterCheckMovement(me))
	{
		me->y-=me->dy;
		Dampen(&me->dy,FIXAMT);
		me->y+=me->dy;
		UpdateGuyRect(me);
	}

	if(me->anim>0)
		me->anim--;

	if(me->reload)
		me->reload--;
	else
	{
		if(me->flags&GF_OFFSCREEN)
		{
			// force him to head onto the screen if he's off
			if((me->x>>FIXSHIFT)>SCR_WIDTH/2)
				me->facing=4*8;
			else
				me->facing=0;
		}
		switch(me->facing/8)
		{
			case 0:
				me->dx=me->speed;
				me->dy=0;
				break;
			case 1:
				me->dx=me->speed;
				me->dy=me->speed;
				break;
			case 2:
				me->dx=0;
				me->dy=me->speed;
				break;
			case 3:
				me->dx=-me->speed;
				me->dy=me->speed;
				break;
			case 4:
				me->dx=-me->speed;
				me->dy=0;
				break;
			case 5:
				me->dx=-me->speed;
				me->dy=-me->speed;
				break;
			case 6:
				me->dx=0;
				me->dy=-me->speed;
				break;
			case 7:
				me->dx=me->speed;
				me->dy=-me->speed;
				break;
		}
		me->reload=40;
		me->anim=31;
		// reload2 tells it which way to spin when it's sitting still
		if(GetRand(2)==0)
			me->reload2=0;
		else
			me->reload2=2;
		return;
	}

	if(me->dx==0 && me->dy==0)
	{
		// turn around until time to go again
		me->facing+=(me->reload2-1)*GetRand(3);
		me->facing&=63;
	}
	else
	{
		Dampen(&me->dx,FIXAMT/4);
		Dampen(&me->dy,FIXAMT/4);
		if((me->reload&3)==0)
			AddParticle(PART_RIPPLE,me->x,me->y,1,15);
	}
}

void UpdateFrog(guy_t *me)
{
	me->y+=me->dy;
	me->destx+=me->dx;	// dx is used as dz, and destx is used as z
	me->dx-=FIXAMT/2;

	MonsterCheckMovement(me);	// don't stop, just check for fixing shrooms, getting hit, etc.

	me->anim++;
	if(me->anim>17)
		me->anim=17;

	if(me->destx<=0)	// hit the ground
	{
		AddParticle(PART_RIPPLE,me->x,me->y,1,15);
		PlantShroom((me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
		me->destx=0;
		me->dx=FIXAMT*4;
		me->anim=0;
	}
	if(me->y>FIXAMT*SCR_HEIGHT)
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
	}
}

void UpdateTurret(guy_t *me)
{
	int i;
	// animate sweeping back and forth
	me->anim+=me->reload2-1;
	if(me->anim==0)
		me->reload2=2;
	if(me->anim==8*4+3)
		me->reload2=0;

	if(me->hp>0)
	{
		me->reload--;
		if(!me->reload)
		{
			// fire
			i=AddGuyExact(GUY_YELLOWBULLET,me->x,me->y,0);
			if(i!=-1)
			{
				MakeSound(SND_TURRETFIRE,1200);
				guy[i].dx=(goodguy->x-me->x)/32;
				guy[i].dy=(goodguy->y-me->y)/32;
			}
			me->reload=40+GetRand(160);
		}
	}
}

void UpdateRepairDroid(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	MonsterCheckMovement(me);	// just to make him hurt enemies and get shot, not to impede his
								// movement
	if(me->x<0)
	{
		me->x=0;
		me->reload=1;
	}
	if(me->x>SCR_WIDTH*FIXAMT)
	{
		me->x=SCR_WIDTH*FIXAMT;
		me->reload=1;
	}
	if((me->y>>FIXSHIFT)>SCR_HEIGHT)
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
		return;
	}

	if(GetRand(12)==0)
			PlantShroom((me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_WIDTH);
	
	me->anim++;
	if(me->anim>15)
	{
		MakeSound(SND_REPAIRBOT,700);
		me->anim=0;
	}

	me->reload--;
	if(me->reload==0)
	{
		if(me->reload2!=0)	// reload2=0 means going down, 1 means going left, 2 means right
		{
			// if was not going down, time to go down
			me->reload2=0;
			me->dx=0;
			me->dy=me->speed;
		}
		else
		{
			me->reload2=GetRand(2)+1;
			if(me->reload2==1)
				me->dx=-me->speed;
			else
				me->dx=me->speed;
			me->dy=0;
		}
		me->reload=GetRand(30)+4;
	}
}

void UpdateVolcano(guy_t *me)
{
	if(me->reload)
	{
		me->reload--;
		if(GetRand((me->reload/4)+1)==0)
			AddParticleExact(PART_SMOKE2,me->x,me->y,FIXAMT*28,0,0,FIXAMT*2+GetRand(2*FIXAMT),8*3+7);
		if(me->reload==55)
			MakeSound(SND_VOLCANOFIRE,1200);
	}
	else
	{
		me->anim++;
		if(me->anim==9)
		{
			// Erupt!
			AddGuyExact(GUY_ROCK,me->x,me->y,0);
			ExplodeParticlesHigh(PART_RED,me->x,me->y,2,5);
			ExplodeParticlesHigh(PART_YELLOW,me->x,me->y,2,5);
			ExplodeParticlesHigh(PART_RED,me->x,me->y,10,5);
			ExplodeParticlesHigh(PART_YELLOW,me->x,me->y,10,5);
			me->anim=0;
			me->reload=GetRand(240)+120;
		}
	}
}

void UpdateRock(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	me->destx+=me->desty;		// destx=z, desty=dz

	me->desty-=FIXAMT/2;
	
	if(me->desty<-FIXAMT*12)
		me->desty=-FIXAMT*12;

	me->anim++;

	
	if(me->destx<20*FIXAMT)
	{
		me->flags&=(~GF_GHOST);	// not a ghost anymore
		MonsterCheckMovement(me);
		
		if(me->destx<=0)
		{
			me->type=GUY_NONE;
			if(GetWorld()==WORLD_TARTARUS)
				PlantShroom((me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_WIDTH);
			ExplodeParticles(PART_RED,me->x,me->y,3,6);
		}
	}
	else
		if(MonsterCheckWallsOnly(me))
			me->type=GUY_NONE;
}

void UpdateSparky(guy_t *me)
{
	me->x+=me->dx;

	if(me->x>(SCR_WIDTH+16)*FIXAMT)
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
		return;
	}
	// reload2 contains which direction to animate in
	me->anim+=me->reload2-1;

	if(me->anim==5)
		me->reload2=0;
	if(me->anim==0)
		me->reload2=2;

	me->reload--;
	if(!me->reload)
	{
		// shoot flame
		MakeSound(SND_SPARKYFIRE,1200);
		AddGuyExact(GUY_FLAME,me->x,me->y+FIXAMT*8,0);
		me->reload=10+GetRand(20);
	}
}

void UpdateFlame(guy_t *me)
{
	me->anim+=me->reload2-1;

	if(me->anim>=11)
	{
		me->reload2=0;
		me->anim=11;
	}
	if(me->anim==0)
		me->type=GUY_NONE;	// all done
	if(me->anim==6 && me->reload2>0)	// on the way up, make children
		AddGuyExact(GUY_FLAME,me->x,me->y+FIXAMT*16,0);
}

void UpdateFireAnt(guy_t *me)
{
	byte v;

	if(me->flags&GF_OFFSCREEN)
	{
		if(me->rectx>=0 && me->recty>=0 && me->rectx2<SCR_WIDTH && me->recty2<SCR_HEIGHT)
			me->flags&=(~GF_OFFSCREEN);	// now that you're on, you can't be off anymore
	}

	me->anim++;
	if(me->anim>=12)
	{
		MakeSound(SND_SPIDER,300);
		me->anim=0;
	}

	v=0;

	me->x+=me->dx;
	UpdateGuyRect(me);
	while(me->dx && MonsterCheckMovement(me))
	{
		v=1;
		me->x-=me->dx;
		Dampen(&me->dx,FIXAMT);
		me->x+=me->dx;
		UpdateGuyRect(me);
	}

	me->y+=me->dy;
	UpdateGuyRect(me);
	while(me->dy && MonsterCheckMovement(me))
	{
		v=1;
		me->y-=me->dy;
		Dampen(&me->dy,FIXAMT);
		me->y+=me->dy;
		UpdateGuyRect(me);
	}

	if(v && (!(me->flags&GF_OFFSCREEN)))
		me->reload=0;	// if you hit an obstacle, then definitely new direction time

	if(me->reload)
		me->reload--;
	else	// head in a new direction
	{
		if(!(me->flags&GF_OFFSCREEN))
		{
			me->facing=(me->facing+1-GetRand(3))&7;
			v=me->facing;
			me->dx=0;
			me->dy=0;
			if(v==0 || v==1 || v==7)
				me->dx=me->speed;
			if(v==1 || v==2 || v==3)
				me->dy=me->speed;
			if(v==3 || v==4 || v==5)
				me->dx=-me->speed;
			if(v==5 || v==6 || v==7)
				me->dy=-me->speed;
			me->facing=v;
			me->reload=10+GetRand(20);
		}
		else
		{
			// force him to head onto the screen if he's off
			if((me->x>>FIXSHIFT)>SCR_WIDTH/2)
			{
				me->dx=-me->speed;
				if((me->y>>FIXSHIFT)>SCR_HEIGHT-(SCR_HEIGHT/8))
				{
					me->dy=-me->speed;
					me->facing=5;
				}
				else
				{
					me->dy=me->speed;
					me->facing=3;
				}
			}
			else
			{
				me->dx=me->speed;
				if((me->y>>FIXSHIFT)>SCR_HEIGHT-(SCR_HEIGHT/8))
				{
					me->dy=-me->speed;
					me->facing=7;
				}
				else
				{
					me->dy=me->speed;
					me->facing=1;
				}
			}
			me->reload=2;
		}
	}
}

void UpdateAsteroid(guy_t *me)
{
	me->x+=me->dx;
	UpdateGuyRect(me);
	if(MonsterCheckMovement(me))
	{
		me->x-=me->dx;
		// bounce off with some randomness
		me->dx=-me->dx+FIXAMT/2-GetRand(FIXAMT);
		me->dy+=FIXAMT/2-GetRand(FIXAMT);
	}
	me->y+=me->dy;
	if(me->reload2==1)
		me->y+=2*FIXAMT;	// all rocks always move down in space, due to scrolling

	UpdateGuyRect(me);
	if(MonsterCheckMovement(me))
	{
		me->y-=me->dy;
		// bounce off with some randomness
		me->dx+=FIXAMT/2-GetRand(FIXAMT);
		me->dy=-me->dy+FIXAMT/2-GetRand(FIXAMT);
	}

	switch(me->type)
	{
		case GUY_BIGROID:
			Clamp(&me->dx,FIXAMT*2);
			Clamp(&me->dy,FIXAMT*2);
			break;
		case GUY_MEDROID:
			Clamp(&me->dx,FIXAMT*4);
			Clamp(&me->dy,FIXAMT*4);
			break;
		case GUY_SMLROID:
			Clamp(&me->dx,FIXAMT*6);
			Clamp(&me->dy,FIXAMT*6);
			break;
	}
	if(me->reload2==1)	// this is a bonus stage, hence check for going away offscreen
	{
		if(me->x<-FIXAMT*32 || me->x>FIXAMT*(SCR_WIDTH+32) ||
		   me->y<-FIXAMT*64 || me->y>FIXAMT*(SCR_HEIGHT+32))
		   me->type=GUY_NONE;	// gone
	}
	me->anim++;
}

void UpdateLobsterTail(guy_t *me)
{
	int parenty;

	if(guy[me->parent].type==GUY_NONE)
	{
		me->type=GUY_NONE;
		return;
	}

	me->x+=me->dx;
	me->y+=me->dy;

	if(me->x<guy[me->parent].x-FIXAMT)
		me->dx+=FIXAMT;
	else if(me->x>guy[me->parent].x+FIXAMT)
		me->dx-=FIXAMT;
	else
	{
		Dampen(&me->dx,FIXAMT*2);
	}

	parenty=guy[me->parent].y;
	if(guy[me->parent].type==GUY_LOBSTER)
		parenty-=FIXAMT*40;
	else
		parenty-=FIXAMT*25;

	if(me->y<parenty)
		me->dy+=FIXAMT;
	else if(me->y>=parenty)
	{
		me->y=parenty;
		me->dy=0;
	}

	Clamp(&me->dx,FIXAMT*4);
	Clamp(&me->dy,FIXAMT*4);
}

void UpdateLobster(guy_t *me)
{
	if(me->reload==0)
	{
		MakeSound(SND_FIREBALL,1200);
		AddGuyExact(GUY_FIREBALL,me->x,me->y+FIXAMT*30,0);
		me->reload=20+GetRand(62-60+me->hp*3);
	}
	else
	{
		me->reload--;
	}
	me->reload2--;
	if(!me->reload2)
	{
		MakeSound(SND_LOBSTER,800);
		me->reload2=10+me->hp*4;
	}

	if(me->desty>0)
		me->desty--;
	else if(me->dy==0)	// don't launch satellites until in position, please
	{
		switch(GetLIndex())
		{
			case 0:
				// no satellites
				break;
			case 1:
				MakeSound(SND_MISSILE,1200);
				if(GetRand(2)==0)
					AddGuyExact(GUY_SATELLITE,me->x-30*FIXAMT,me->y-10*FIXAMT,0);
				else
					AddGuyExact(GUY_SATELLITE,me->x+30*FIXAMT,me->y-10*FIXAMT,1);
				break;
			case 2:
			case 3:
				MakeSound(SND_MISSILE,1200);
				AddGuyExact(GUY_SATELLITE,me->x-30*FIXAMT,me->y-10*FIXAMT,0);
				AddGuyExact(GUY_SATELLITE,me->x+30*FIXAMT,me->y-10*FIXAMT,1);
				break;
		}
		me->desty=120+GetRand(120)+10*(2-GetDifficulty())-30*(GetLIndex()==3);
	}

	me->x+=me->dx;
	me->y+=me->dy;

	if(me->y<FIXAMT*TILE_HEIGHT*10)
		me->dy+=FIXAMT/2;
	else
		me->dy=0;

	if(me->x<me->destx)
		me->dx+=FIXAMT;
	if(me->x>me->destx)
		me->dx-=FIXAMT;

	if(me->x<FIXAMT*TILE_WIDTH*6)
		me->destx=(MAP_WIDTH-6)*FIXAMT*TILE_WIDTH;
	if(me->x>(MAP_WIDTH-6)*FIXAMT*TILE_WIDTH)
		me->destx=6*FIXAMT*TILE_WIDTH;

	Dampen(&me->dx,FIXAMT/4);
	Clamp(&me->dx,FIXAMT*2);
	Dampen(&me->dy,FIXAMT/4);
	Clamp(&me->dy,FIXAMT*4);
}

void UpdateLobsterClaw(guy_t *me)
{
	int kid;

	if(guy[me->parent].type==GUY_NONE)
	{
		me->type=GUY_NONE;
		return;
	}

	me->dx=guy[me->parent].x-me->x;
	me->dy=guy[me->parent].y-me->y;

	me->x=guy[me->parent].x;
	me->y=guy[me->parent].y;

	if(me->reload)
		me->reload--;
	else
	{
		me->anim+=me->reload2-1;
		if(me->anim==1 && me->reload2==2)
			MakeSound(SND_LOBCHARGE,1500);
		if(me->anim==8*4 && me->reload2==2)
		{
			MakeSound(SND_EVILLASER,1200);
			if(me->type==GUY_LLOBCLAW)
				kid=AddGuyExact(GUY_GREENLASER,me->x-55*FIXAMT,me->y+FIXAMT*60,0);
			else
				kid=AddGuyExact(GUY_GREENLASER,me->x+55*FIXAMT,me->y+FIXAMT*60,0);
			if(kid!=-1)
				guy[kid].parent=me->num;
		}
		if(me->anim==8*5)
		{
			me->anim=8*5-1;
			me->reload2=0;
		}
		if(me->anim==0)
		{
			me->reload=90;
			me->reload2=2;
		}
	}
	// put kids in their place
	for(kid=0;kid<MAX_GUYS;kid++)
	{
		if(guy[kid].parent==me->num)
		{
			if(me->type==GUY_LLOBCLAW)
				guy[kid].x=me->x-57*FIXAMT;
			else
				guy[kid].x=me->x+60*FIXAMT;
		}
	}
}

void UpdateGreenLaser(guy_t *me)
{
	int kid;

	if(me->reload==16)
	{
		kid=AddGuyExact(GUY_GREENLASER2,me->x,me->y,0);
		if(kid!=-1)
			guy[kid].parent=me->parent;
	}
	//me->x+=guy[me->parent].dx;

	if(me->reload>9)
		me->anim--;
	if(me->reload<8)
		me->anim++;
	me->reload--;
	if(!me->reload)
		me->type=GUY_NONE;
}

void UpdateGreenLaser2(guy_t *me)
{
	int kid;

	CheckGoodguyHit(me);
	if((me->y<SCR_HEIGHT*FIXAMT) && (me->reload==16))
	{
		kid=AddGuyExact(GUY_GREENLASER2,me->x,me->y+37*FIXAMT,0);
		if(kid!=-1)
			guy[kid].parent=me->parent;
	}
	//me->x+=guy[me->parent].dx;

	if(me->reload>9)
		me->anim--;
	if(me->reload<8)
		me->anim++;
	me->reload--;
	if(!me->reload)
		me->type=GUY_NONE;
}

void UpdateSpinnyShip(guy_t *me)
{
	if(me->y<me->desty)
	{
		me->anim=0;
		me->x+=me->dx;
		me->y+=me->dy;
		if(me->y>=me->desty)
		{
			me->dx*=4;
			me->dy*=2;
		}
	}
	else	// in spinning dive
	{
		if(me->dx<0)
		{
			me->anim++;
			if(me->anim>15)
				me->anim=0;
		}
		else
		{
			me->anim--;
			if(me->anim>15)
				me->anim=15;
		}
		me->x+=me->dx;
		me->y+=me->dy;
		if(me->y>(SCR_HEIGHT+30)*FIXAMT)
			me->type=GUY_NONE;
	}
}

void UpdateClawShip(guy_t *me)
{
	int kid;

	// slowly descend
	me->y+=FIXAMT*2;
	
	if(me->y>(SCR_HEIGHT+50)*FIXAMT)
	{
		me->type=GUY_NONE;
		return;
	}

	// fire green laser as desired
	if(me->reload)
		me->reload--;
	else if(me->y<(SCR_HEIGHT-10)*FIXAMT)	// don't fire if near the bottom
	{
		me->anim+=me->reload2-1;
		if(me->anim==1 && me->reload2==2)
			MakeSound(SND_LOBCHARGE,1500);
		if(me->anim==8*4 && me->reload2==2)
		{
			MakeSound(SND_EVILLASER,1200);
			kid=AddGuyExact(GUY_GREENLASER,me->x,me->y+FIXAMT*20,0);
			if(kid!=-1)
				guy[kid].parent=me->num;
		}
		if(me->anim==8*5)
		{
			me->anim=8*5-1;
			me->reload2=0;
		}
		if(me->anim==0)
		{
			me->reload=60;
			me->reload2=2;
		}
	}
}

void UpdateSmallShip(guy_t *me)
{
	int i,oldx;

	oldx=me->x;

	me->y+=me->dy;
	me->x+=me->dx;

	if(me->y>(SCR_WIDTH+30)*FIXAMT)
	{
		me->type=GUY_NONE;
		return;
	}

	if(me->x<me->destx)
	{
		if(oldx>me->destx)
			me->destx=25*FIXAMT*TILE_WIDTH;
		me->dx+=FIXAMT/2;
	}
	else
	{
		if(oldx<me->destx)
			me->destx=5*FIXAMT*TILE_WIDTH;
		me->dx-=FIXAMT/2;
	}
	
	Clamp(&me->dx,FIXAMT*6);

	me->anim=3;
	i=me->dx/(FIXAMT*2);
	me->anim+=i;

	if(me->reload>0)
		me->reload--;
	else
	{
		MakeSound(SND_SHOOT2,1200);
		i=AddGuyExact(GUY_BLUEBULLET,me->x-FIXAMT*8,me->y+FIXAMT*10,0);
		if(i!=-1)
		{
			guy[i].dx=0;
			guy[i].dy=FIXAMT*8;
		}
		i=AddGuyExact(GUY_BLUEBULLET,me->x+FIXAMT*8,me->y+FIXAMT*10,0);
		if(i!=-1)
		{
			guy[i].dx=0;
			guy[i].dy=FIXAMT*8;
		}
		me->reload=30;
	}
}

void UpdateSquidShip(guy_t *me)
{
	// animate
	me->anim+=me->facing-1;
	if(me->anim>250)
	{
		me->anim=0;
		me->facing=2;
	}
	if(me->anim>9)
	{
		me->anim=9;
		me->facing=0;
	}
	
	// move
	me->x+=me->dx;

	if(me->x>me->destx && me->dx>0)
	{
		SetHordeDX(-FIXAMT*2,FIXAMT*32,me->num);
	}
	else if(me->x<me->destx && me->dx<0)
	{
		SetHordeDX(FIXAMT*2,SCR_WIDTH*FIXAMT-FIXAMT*32,me->num);
	}
}

void UpdatePowerUpUFO(guy_t *me)
{
	me->anim++;
	if(me->anim>3)
		me->anim=0;

	me->x+=me->dx;
	if((me->dx>0 && me->x>FIXAMT*SCR_WIDTH+FIXAMT*40) ||
		(me->dx<0 && me->x<-FIXAMT*40))
	{
		me->type=GUY_NONE;
		TellLevelAboutIt(EVT_MONSGONE,me->type);
	}
	if(IsBonusStage()==BNS_HOSTILE)
		me->y+=FIXAMT;	// drift down
}

void UpdateFireballShip(guy_t *me)
{
	// move in a horde-like fashion
	me->x+=me->dx;

	if(me->x>me->destx && me->dx>0)
	{
		SetHordeDX(-FIXAMT*2,FIXAMT*32,me->num);
	}
	else if(me->x<me->destx && me->dx<0)
	{
		SetHordeDX(FIXAMT*2,SCR_WIDTH*FIXAMT-FIXAMT*32,me->num);
	}

	if(me->facing==1)	// got hit, must spin
	{
		if(me->anim<5)
			me->anim=5;
		else
		{
			me->anim++;
			if(me->anim==10)
			{
				me->anim=0;
				me->facing=0;	// done spinning
			}
		}
	}
	else	// not spinning, must reload
	{
		if(me->reload)
		{
			me->reload--;
			if(me->reload>85)
				me->anim=90-me->reload;
			else if(me->reload>65)
				me->anim=3;
			else if(me->reload>45)
				me->anim=2;
			else if(me->reload>25)
				me->anim=1;
			else
				me->anim=0;
		}
		else	// reloaded, fire!
		{
			MakeSound(SND_FIREBALL,1200);
			AddGuyExact(GUY_FIREBALL,me->x,me->y+FIXAMT*20,0);
			me->reload=90;
		}
	}
}

void UpdateDoubleBarrelShip(guy_t *me)
{
	int i;
	// move in a horde-like fashion
	me->x+=me->dx;

	if(me->x>me->destx && me->dx>0)
	{
		SetHordeDX(-FIXAMT*2,FIXAMT*32,me->num);
	}
	else if(me->x<me->destx && me->dx<0)
	{
		SetHordeDX(FIXAMT*2,SCR_WIDTH*FIXAMT-FIXAMT*32,me->num);
	}

	if(me->reload)
		me->reload--;
	else
	{
		MakeSound(SND_SHOOT1,1200);
		i=AddGuyExact(GUY_REDBULLET,me->x-FIXAMT*8,me->y+FIXAMT*15,0);
		if(i!=-1)
		{
			guy[i].dx=0;
			guy[i].dy=FIXAMT*8;
		}
		i=AddGuyExact(GUY_REDBULLET,me->x+FIXAMT*8,me->y+FIXAMT*15,0);
		if(i!=-1)
		{
			guy[i].dx=0;
			guy[i].dy=FIXAMT*8;
		}
		me->reload=30+GetRand(30);
	}
}

void UpdateTriceratops(guy_t *me)
{
	me->x+=me->dx;
	MonsterCheckMovement(me);

	if(me->x>(SCR_WIDTH+50)*FIXAMT || me->x<(-50*FIXAMT))
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
	}

	me->anim++;
	if(me->anim>15)
		me->anim=0;
}

void UpdatePterodactyl(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	
	// using destx as a Z coordinate
	if(me->facing==0)	// heading up
		me->destx+=FIXAMT;
	else if(me->destx>10*FIXAMT)	// diving
		me->destx-=FIXAMT*4;

	MonsterCheckMovement(me);

	if(me->facing==0)	// heading up
	{
		me->anim+=me->reload-1;
		if(me->anim==5)
		{
			me->reload=0;
			MakeSound(SND_PTEROFLAP,10);
		}
		if(me->anim==0)
			me->reload=2;
		
		if(me->y>FIXAMT*(SCR_HEIGHT/2))
		{
			MakeSound(SND_PTEROSCREECH,1200);
			me->facing=1;
			me->dy=0;
			me->anim=6;
		}
	}
	else	// diving
	{
		if(me->anim<10)
			me->anim++;

		if(me->dy<FIXAMT*10)
			me->dy+=FIXAMT;

		if(me->y>FIXAMT*(SCR_HEIGHT+30))
		{
			TellLevelAboutIt(EVT_MONSGONE,me->type);
			me->type=GUY_NONE;
		}
	}
}

void UpdateEyeball(guy_t *me)
{
	int i;
	
	i=0;

	me->anim+=(me->type-GUY_EYEBALL1+1);

	if(me->anim>23)
		me->anim-=24;
	
	me->x+=me->dx;
	me->y+=me->dy;
	UpdateGuyRect(me);
	if((me->dy==0) && MonsterCheckMovement(me))
	{
		i=1;
	}

	if(me->dx>0)
	{
		if(me->dy<0)
			me->facing=8;
		else if(me->dy>0)
			me->facing=2;
		else
			me->facing=1;
	}
	else if(me->dx<0)
	{
		if(me->dy<0)
			me->facing=6;
		else if(me->dy>0)
			me->facing=4;
		else
			me->facing=5;
	}
	else
	{
		if(me->dy<0)
			me->facing=7;
		else if(me->dy>0)
			me->facing=3;
		else
			me->facing=0;
	}
	if(me->facing>0)
		me->facing--;
	else me->facing=2;

	if(me->reload)
	{
		me->reload--;
		if(me->reload==0)
		{
			me->dy=0;
			if(me->flags&GF_GOINGLEFT)
				me->dx=-me->speed;
			else
				me->dx=me->speed;
		}
	}
	
	if(i==1)	// hit an obstacle
	{
		me->dx=-me->dx/2;
		if((me->y>>FIXSHIFT)<TILE_HEIGHT*3)
			me->flags&=(~GF_GOINGUP);
		if((me->y>>FIXSHIFT)>SCR_HEIGHT-TILE_HEIGHT*3)
			me->flags|=GF_GOINGUP;

		// eyeballs loop at the middle of the screen
		if((me->y>>FIXSHIFT)<SCR_HEIGHT/2 && me->flags&GF_GOINGUP)
			me->flags&=(~GF_GOINGUP);

		if(me->flags&GF_GOINGLEFT)
		{
			me->flags&=(~GF_GOINGLEFT);
			me->reload=GetLogLength(me->speed);
		}
		else
		{
			me->flags|=GF_GOINGLEFT;
			me->reload=GetLogLength(me->speed);
		}

		if(me->flags&GF_GOINGUP)
		{
			me->dy=-me->speed;
		}
		else
		{
			me->dy=me->speed;
		}
	}
}

void UpdateEyeGuy(guy_t *me)
{
	byte animTable[]={0,1,2,3,2,1,0,4,5,6,5,4};
	byte fireAnimTable[]={7,8,8,9,10,10,11,11,11,12,12,12};

	me->reload2++;
	if(me->reload2>11)
	{
		me->reload2=0;
		if(me->reload==0)	// was firing
			me->reload=(me->hp*20)+10+GetRand(30);
	}

	if(me->y<FIXAMT*100)
	{
		me->y+=FIXAMT*2;
		MonsterCheckMovement(me);
		me->anim=animTable[me->reload2];
		return;	// can't do anything but advance
	}
	else
	{
		if(me->reload)
		{
			me->reload--;
			if(!me->reload)	// fire a flame
			{
				me->reload2=0;
				me->anim=fireAnimTable[0];
				return;
			}
			me->x+=me->dx;
			if(me->x<FIXAMT*30)
				me->dx=me->speed;
			if(me->x>(SCR_WIDTH-30)*FIXAMT)
				me->dx=-me->speed;
			MonsterCheckMovement(me);
			me->anim=animTable[me->reload2];
		}
		else
		{
			if(me->reload2==7)
			{
				MakeSound(SND_EYEGUYFIRE,1200);
				AddGuyExact(GUY_FLAME,me->x,me->y+FIXAMT*30,0);
			}
			me->anim=fireAnimTable[me->reload2];
		}
		
	}
}

void UpdateScorpion(guy_t *me)
{
	me->anim++;
	if(me->anim>11)
	{
		me->anim=0;
		MakeSound(SND_SCORPION,20);
	}

	me->x+=me->dx;
	me->y+=me->dy;
	UpdateGuyRect(me);
	MonsterCheckMovement(me);

	if(me->y>(SCR_HEIGHT+10)*FIXAMT)
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
	}

	if(me->reload)
		me->reload--;
	else
	{
		me->dx=-me->dx;	// start scrambling the other way
		MakeSound(SND_SCORPSHOOT,1200);
		AddGuyExact(GUY_FLYSPIT,me->x,me->y,0);
		me->reload=5+GetRand(40);
	}
}

void UpdateTumbleweed(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	me->destx+=me->desty;	// destx=Z, desty=DZ
	UpdateGuyRect(me);
	MonsterCheckMovement(me);
	if(me->y>(SCR_HEIGHT+40)*FIXAMT)
	{
		TellLevelAboutIt(EVT_MONSGONE,me->type);
		me->type=GUY_NONE;
	}
	me->desty-=FIXAMT/2;	// gravity
	if(me->destx<=0)
	{
		me->destx=0;
		me->desty=GetRand(FIXAMT*4)+FIXAMT;
		if(GetRand(3)==0)
			PlantShroom((me->x>>FIXSHIFT)/TILE_WIDTH,(me->y>>FIXSHIFT)/TILE_HEIGHT);
	}
	me->anim++;
	if(me->anim>5)
		me->anim=0;
}

void UpdateSandworm(guy_t *me)
{
	me->flags&=(~GF_GHOST);
	if(me->destx==0)	// in underground mode
	{
		me->x+=me->dx;
		me->y+=me->dy;

		if(me->x<0)
		{
			me->x=0;
			me->dx=0;
		}
		if(me->y<(SCR_HEIGHT-SCR_HEIGHT/3)*FIXAMT)
		{
			me->dy=0;
			me->y=(SCR_HEIGHT-SCR_HEIGHT/3)*FIXAMT;
		}
		if(me->x>SCR_WIDTH*FIXAMT)
		{
			me->x=SCR_WIDTH*FIXAMT;
			me->dx=0;
		}
		if(me->y>(SCR_HEIGHT-8)*FIXAMT)
		{
			me->y=(SCR_HEIGHT-8)*FIXAMT;
			me->dy=0;
		}

		AddParticleExact(PART_SANDWORM,me->x,me->y,0,0,0,0,5);
		me->dx+=FIXAMT-GetRand(FIXAMT*2);
		me->dy+=FIXAMT-GetRand(FIXAMT*2);
		Clamp(&me->dx,FIXAMT*4);
		Clamp(&me->dy,FIXAMT*4);
		UpdateGuyRect(me);
		if(MonsterCheckMovement(me))
		{
			// bumped into something, bite it
			me->destx=1;
			me->anim=0;
			me->flags|=GF_EVIL;	// can be shot now
			me->flags&=(~GF_SANDWORM);
			MakeSound(SND_SANDWORM,1200);
		}
	}
	else
	{
		MonsterCheckMovement(me);
		me->anim++;
		if(me->anim==7)
		{
			me->flags|=GF_EVILMEAN|GF_SHROOMEAT|GF_GOODMEAN;
			MonsterCheckMovement(me);
			CheckEnemyHit(me);
			me->flags&=(~(GF_EVILMEAN|GF_SHROOMEAT|GF_GOODMEAN));
		}
		if(me->anim==9)
		{
			me->flags&=(~GF_EVIL);	// can't be shot anymore
			me->flags|=GF_SANDWORM;
			me->destx=0;
		}
	}
	me->flags|=GF_GHOST;
}

void UpdateBabyAlien(guy_t *me)
{
	int cx,cy;
	byte walkTable[]={0,1,2,3,2,1};
	byte leapTable[]={4,5,6,6,5,4};

	me->x+=me->dx;
	me->y+=me->dy;
	UpdateGuyRect(me);
	if(MonsterCheckMovement(me))
	{
		me->x-=me->dx;
		me->y-=me->dy;
		if(me->reload2==0)	// not leaping
			me->reload=0;	// so turn, this direction is no good
		UpdateGuyRect(me);
	}

	me->destx++;
	if(me->destx>5)
		me->destx=0;

	if(me->reload2==1)	// leaping
	{
		if(me->destx==0)
		{
			me->reload2=0;	// stop leaping
			me->dx/=2;
			me->dy/=2;
		}
	}
	else
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->reload=5+GetRand(15);
			if(abs(goodguy->x-me->x)+abs(goodguy->y-me->y)<FIXAMT*40)
			{
				// leap at him
				MakeSound(SND_BABYLEAP,1200);
				me->anim=0;
				me->reload2=1;
				cx=(goodguy->x-me->x)/(FIXAMT*4);
				cy=(goodguy->y-me->y)/(FIXAMT*4);
				if(cx>0)
				{
					if(cy<0)
						me->facing=8;
					else if(cy>0)
						me->facing=2;
					else
						me->facing=1;
				}
				else if(cx<0)
				{
					if(cy<0)
						me->facing=6;
					else if(cy>0)
						me->facing=4;
					else
						me->facing=5;
				}
				else
				{
					if(cy<0)
						me->facing=7;
					else if(cy>0)
						me->facing=3;
					else
						me->facing=0;
				}
				if(me->facing!=0)
					me->facing--;
				else
					me->facing=2;
			}
			else	// don't need to leap, just turn
			{
				me->facing=(me->facing+1-GetRand(3))&7;
			}
			// whether leaping or not, need to calculate speed
			if(me->facing==0 || me->facing==7 || me->facing==1)
				me->dx=FIXAMT*6;
			else if(me->facing==3 || me->facing==4 || me->facing==5)
				me->dx=-FIXAMT*6;
			else
				me->dx=0;
			if(me->facing==1 || me->facing==2 || me->facing==3)
				me->dy=FIXAMT*6;
			else if(me->facing==5 || me->facing==6 || me->facing==7)
				me->dy=-FIXAMT*6;
			else
				me->dy=0;

			if(me->reload2)
			{
				// leaps at double speed
				me->dx*=2;
				me->dy*=2;
			}
		}
	}
	if(me->reload2)
		me->anim=leapTable[me->destx];
	else
		me->anim=walkTable[me->destx];
}

void UpdateFlyAlien(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;

	if(me->reload)
		me->reload--;
	else
	{
		// shoot
		MakeSound(SND_FLYALIENSPIT,1000);
		AddGuyExact(GUY_FLYSPIT,me->x,me->y+FIXAMT*10,0);
		me->reload=30+GetRand(60);
		// pick a new destination
		me->destx=GetRand(SCR_WIDTH)<<FIXSHIFT;
		me->desty=GetRand(SCR_HEIGHT)<<FIXSHIFT;
	}
	if(me->x<me->destx)
		me->dx+=FIXAMT;
	if(me->y<me->desty)
		me->dy+=FIXAMT;
	if(me->x>me->destx)
		me->dx-=FIXAMT;
	if(me->y>me->desty)
		me->dy-=FIXAMT;

	Clamp(&me->dx,FIXAMT*6);
	Clamp(&me->dy,FIXAMT*6);

	me->anim+=me->reload2-1;
	if(me->anim==4)
	{
		me->reload2=0;
		MakeSound(SND_PTEROFLAP,20);
	}
	if(me->anim==0)
		me->reload2=2;
}

void UpdateQueen(guy_t *me)
{
	int i;

	if(me->hp==0)	// dead, act like it
	{
		for(i=0;i<MAX_GUYS;i++)
			if(guy[i].type==GUY_SHROOM || guy[i].type==GUY_ALIENHEAD || guy[i].type==GUY_BABYALIEN ||
				guy[i].type==GUY_FLYALIEN)
				GetHit(&guy[i]);

		ExplodeParticles(PART_GREEN,me->x,me->y,2,2);
		ExplodeParticles(PART_GREEN,me->x,me->y,4,2);
		ExplodeParticles(PART_GREEN,me->x,me->y,6,2);
		ExplodeParticles(PART_GREEN,me->x,me->y,8,2);
		ExplodeParticles(PART_GREEN,me->x,me->y,12,2);
		AddParticle(PART_SHOCKWAVE,(me->rectx+GetRand(me->rectx2-me->rectx))<<FIXSHIFT,
					(me->recty+GetRand(me->recty2-me->recty))<<FIXSHIFT,1,6);
		AddParticle(PART_BIGBOOM,(me->rectx+GetRand(me->rectx2-me->rectx))<<FIXSHIFT,
					(me->recty+GetRand(me->recty2-me->recty))<<FIXSHIFT,1,19);
		ExplodeParticles(PART_GREY,me->x,me->y,2,1);
		ExplodeParticles(PART_GREY,me->x,me->y,4,1);
		ExplodeParticles(PART_GREY,me->x,me->y,6,1);
		ExplodeParticles(PART_GREY,me->x,me->y,8,1);

		me->reload--;
		if(me->reload==0)
			me->type=GUY_NONE;
		return;
	}

	me->anim+=me->speed;
	if(me->anim>63)
	{
		if(me->speed>0)
		{
			me->anim=63;
			me->speed=-(51-me->hp)/4;
		}
		else
		{
			me->anim=0;
			me->speed=(51-me->hp)/4;
		}
	}

	if(me->destx>0)
		me->destx--;
	else
	{
		MakeSound(SND_QUEENLOB,1200);
		AddGuyExact(GUY_ACIDGLOB,me->x-FIXAMT*40,me->y-FIXAMT*10,0);
		AddGuyExact(GUY_ACIDGLOB,me->x+FIXAMT*40,me->y-FIXAMT*10,0);
		me->destx=GetRand(120)+120;
	}
	if(me->desty==1)	// no head, may use green laser
	{
		if(me->reload>0)
		{
			me->reload--;
			if(me->reload==15)
				MakeSound(SND_LOBCHARGE,1200);
		}
		else
		{
			// fire green laser
			MakeSound(SND_EVILLASER,1500);
			AddGuyExact(GUY_GREENLASER,me->x,me->y+FIXAMT*30,0);
			me->reload=120;
		}
	}
	if(me->reload2>0)
		me->reload2--;
	else
	{
		// give birth to an alienpede, using new function to force it to move horizontally
		// and to begin ghostly and to be able to choose left or right facing
		MakeSound(SND_ALIENBIRTH,1200);
		if(GetRand(2)==0)
			GiveBirthToAlien(me->x-50*FIXAMT,me->y,1);
		else
			GiveBirthToAlien(me->x+50*FIXAMT,me->y,0);

		me->reload2=100+GetRand(150);
	}
}

void UpdateQueenHead(guy_t *me)
{
	if(goodguy->x<me->x)
		me->dx-=FIXAMT/4;
	else
		me->dx+=FIXAMT/4;

	Clamp(&me->dx,FIXAMT*2);
	me->x+=me->dx;
	if(me->x>(SCR_WIDTH/2+20)*FIXAMT)
	{
		me->x=(SCR_WIDTH/2+20)*FIXAMT;
	}
	if(me->x<(SCR_WIDTH/2-20)*FIXAMT)
	{
		me->x=(SCR_WIDTH/2-20)*FIXAMT;
	}

	me->y=guy[me->parent].y+20*FIXAMT-abs(me->x-(SCR_WIDTH/2)*FIXAMT);

	if(me->reload>0)
	{
		me->reload--;
		if(me->anim>0)
			me->anim--;
	}
	else
	{
		if(me->anim<9)
			me->anim++;
		else
		{
			// spit out some acid
			AddGuyExact(GUY_ACIDSPIT,me->x,me->y+FIXAMT*30,0);
			me->reload=20+GetRand(40);
		}
	}
}

void UpdateQueenTail(guy_t *me)
{
	if(me->reload>0)
		me->reload--;
	else
	{
		if(me->anim<25)
			me->anim++;
		else
		{
			me->anim=0;
			me->reload=30+GetRand(60);
		}
		if(me->anim==14)
		{
			// lob an egg
			AddGuyExact(GUY_FLYINGEGG,me->x,me->y-40*FIXAMT,0);
			if(GetDifficulty()>0)
				AddGuyExact(GUY_FLYINGEGG,me->x,me->y-40*FIXAMT,0);
				// or two
			if(GetDifficulty()>1)
				AddGuyExact(GUY_FLYINGEGG,me->x,me->y-40*FIXAMT,0);
				// or three
		}
	}
}

void UpdateAcidGlob(guy_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	me->destx+=me->desty;		// destx=z, desty=dz

	me->desty-=FIXAMT/2;
	
	if(me->desty<-FIXAMT*12)
		me->desty=-FIXAMT*12;

	me->anim++;
	if(me->anim>9)
		me->anim=0;

	if(me->type==GUY_ACIDGLOB)
		AddParticleExact(PART_GREEN,me->x,me->y,me->destx,FIXAMT-GetRand(FIXAMT*2),FIXAMT-GetRand(FIXAMT*2),
			0,20);
	
	if(me->destx<20*FIXAMT)
	{
		me->flags&=(~GF_GHOST);	// not a ghost anymore
		MonsterCheckMovement(me);
		
		if(me->destx<=0)
		{
			BulletExplode(me);
		}
	}
	else
		if(MonsterCheckWallsOnly(me))
			me->type=GUY_NONE;
}

void UpdateSatellite(guy_t *me)
{
	int i;

	if(me->facing==0)	// not yet blossomed
	{
		me->x+=me->dx;
		me->y+=me->dy;
		if(me->y<FIXAMT*40)
		{
			me->y=FIXAMT*40;
			me->dx=0;
			me->dy=0;
			me->facing=1;	// open up
			me->reload=36;

			// check to be sure there is room here
			for(i=0;i<MAX_GUYS;i++)
			{
				if(guy[i].type==GUY_SATELLITE && (i!=me->num) &&
					guy[i].rectx<me->rectx2-4 &&
					guy[i].rectx2>=me->rectx+4 &&
					guy[i].recty<me->recty2 && 
					guy[i].recty2>=me->recty)
				{
					me->type=GUY_NONE;
					return;	// can't go here, must vanish
				}
			}
		}
		if(me->x<0 || me->x>SCR_WIDTH*FIXAMT)
		{
			me->type=GUY_NONE;
			return;
		}
	}
	else
	{
		if(me->reload)
		{
			if(me->anim<6*4)
				me->anim++;
			me->reload--;
			if(me->reload==0)
			{
				me->destx=0;
			}
		}
		else
		{
			if(me->destx==0)	// yet to fire
			{
				me->anim+=2;
				if(me->anim>=8*4+3)
				{
					me->anim=8*4+3;
					me->destx=1;
					MakeSound(SND_SHOOT2,1200);
					AddGuyExact(GUY_REDBULLET,me->x,me->y+FIXAMT*10,0);
				}
			}
			else	// already fired
			{
				me->anim-=2;
				if(me->anim<=6*4)
				{
					me->anim=6*4;
					me->reload=60+GetRand(120);
					if(NaughtyTime())
						me->reload=GetRand(5)+1;	// fire like mad
				}
			}
		}
	}
}

// end of individual unique GUY type update functions
//-------------------------------------------------------------------

void UpdateGuy(guy_t *me,int n)
{
	UpdateGuyRect(me);
	switch(me->type)
	{
		case GUY_PLAYER:
			UpdatePlayer(me);
			break;
		case GUY_BULLET:
		case GUY_CHAINBULLET:
		case GUY_SHOTBULLET:
		case GUY_NUKE:
			UpdateBullet(me);
			break;
		case GUY_LASER:
			UpdateLaser(me);
			break;
		case GUY_MISSILE:
			UpdateMissile(me);
			break;
		case GUY_CENTIHEAD:
		case GUY_AQUAHEAD:
		case GUY_ROBOHEAD:
		case GUY_ALIENHEAD:
			UpdateCentihead(me);
			break;
		case GUY_CENTIBODY:
		case GUY_AQUABODY:
		case GUY_AQUATAIL:
		case GUY_ROBOBODY:
		case GUY_ALIENBODY:
			UpdateCentibody(me);
			break;
		case GUY_SPIDER:
		case GUY_ICESPIDER:
		case GUY_SPIDERBOT:
			UpdateSpider(me);
			break;
		case GUY_SHROOM:
			break;
		case GUY_RADIO:
			UpdateRadio(me);
			break;
		case GUY_PRESENT:
			UpdatePresent(me);
			break;
		case GUY_NUKEBOOM:
			UpdateNukeBoom(me);
			break;
		case GUY_WINGMAN:
			UpdateWingMan(me);
			break;
		case GUY_SNOWMAN:
			UpdateSnowman(me);
			break;
		case GUY_FLY:
			UpdateFly(me);
			break;
		case GUY_FLYSPIT:
		case GUY_YELLOWBULLET:
		case GUY_FIREBALL:
		case GUY_REDBULLET:
		case GUY_ACIDSPIT:
		case GUY_BLUEBULLET:
			UpdateEnemyBullet(me);
			break;
		case GUY_STRIDER:
			UpdateStrider(me);
			break;
		case GUY_FROG:
			UpdateFrog(me);
			break;
		case GUY_TURRET:
			UpdateTurret(me);
			break;
		case GUY_REPAIR:
			UpdateRepairDroid(me);
			break;
		case GUY_VOLCANO:
			UpdateVolcano(me);
			break;
		case GUY_ROCK:
			UpdateRock(me);
			break;
		case GUY_SPARKY:
			UpdateSparky(me);
			break;
		case GUY_FLAME:
			UpdateFlame(me);
			break;
		case GUY_FIREANT:
			UpdateFireAnt(me);
			break;
		case GUY_BIGROID:
		case GUY_MEDROID:
		case GUY_SMLROID:
			UpdateAsteroid(me);
			break;
		case GUY_LOBSTER:
			UpdateLobster(me);
			break;
		case GUY_LOBTAIL:
			UpdateLobsterTail(me);
			break;
		case GUY_LLOBCLAW:
		case GUY_RLOBCLAW:
			UpdateLobsterClaw(me);
			break;
		case GUY_GREENLASER:
			UpdateGreenLaser(me);
			break;
		case GUY_GREENLASER2:
			UpdateGreenLaser2(me);
			break;
		case GUY_POWERUP:
			UpdatePowerup(me);
			break;
		case GUY_SPINSHIP:
			UpdateSpinnyShip(me);
			break;
		case GUY_CLAWSHIP:
			UpdateClawShip(me);
			break;
		case GUY_SMALLSHIP:
			UpdateSmallShip(me);
			break;
		case GUY_SQUIDSHIP:
			UpdateSquidShip(me);
			break;
		case GUY_PWRUPUFO:
			UpdatePowerUpUFO(me);
			break;
		case GUY_FBALLSHIP:
			UpdateFireballShip(me);
			break;
		case GUY_DOUBLESHIP:
			UpdateDoubleBarrelShip(me);
			break;
		case GUY_TRICERATOPS:
			UpdateTriceratops(me);
			break;
		case GUY_PTERODACTYL:
			UpdatePterodactyl(me);
			break;
		case GUY_EYEBALL1:
		case GUY_EYEBALL2:
		case GUY_EYEBALL3:
		case GUY_EYEBALL4:
			UpdateEyeball(me);
			break;
		case GUY_EYEGUY:
			UpdateEyeGuy(me);
			break;
		case GUY_SCORPION:
			UpdateScorpion(me);
			break;
		case GUY_TUMBLEWEED:
			UpdateTumbleweed(me);
			break;
		case GUY_SANDWORM:
			UpdateSandworm(me);
			break;
		case GUY_BABYALIEN:
			UpdateBabyAlien(me);
			break;
		case GUY_FLYALIEN:
			UpdateFlyAlien(me);
			break;
		case GUY_QUEEN:
			UpdateQueen(me);
			break;
		case GUY_QUEENHEAD:
			UpdateQueenHead(me);
			break;
		case GUY_QUEENTAIL:
			UpdateQueenTail(me);
			break;
		case GUY_ACIDGLOB:
		case GUY_FLYINGEGG:
			UpdateAcidGlob(me);
			break;
		case GUY_SATELLITE:
			UpdateSatellite(me);
			break;
	}
}

byte UpdateGuys(void)
{
	int i;
	byte v;
	static byte flip=0;

	flip++;

	v=0;
	shroomAnim++;

	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type)
		{
			if(guy[i].flags&GF_VICTORY)
				v=1;
			if((flip&3) || GetDifficulty()>0 || guy[i].type==GUY_PLAYER ||
				guy[i].type==GUY_BULLET || guy[i].type==GUY_CHAINBULLET || guy[i].type==GUY_SHOTBULLET ||
				guy[i].type==GUY_NUKE || guy[i].type==GUY_LASER || guy[i].type==GUY_MISSILE || guy[i].type==GUY_POWERUP ||
				guy[i].type==GUY_RADIO || guy[i].type==GUY_NUKEBOOM || guy[i].type==GUY_WINGMAN || guy[i].type==GUY_PRESENT)
				UpdateGuy(&guy[i],i);	
		}
	// returns whether or not there are any existing VICTORY creatures (once they are all
	// dead, the level is won)
	return v;
}

byte AnimConvert(byte type,byte anim)
{
	byte table1[]={0,1,2,1,0,3,4,3};
	byte table2[]={0,1,2,3,2,1,0,4,5,6,5,4};
	byte table3[]={0,1,2,3,3,2,1,0};
	byte table4[]={0,1,2,3,4,3,2,1,0,5,6,7,8,7,6,5};

	switch(type)
	{
		case GUY_ALIENHEAD:
			return table3[(anim/2)&7];
			break;
		case GUY_ALIENBODY:
			return table2[anim];
			break;
		case GUY_CENTIBODY:
			if(GetWorld()==1 || GetWorld()==7)
			{
				anim/=2;
				anim&=7;
				return table1[anim];
			}
			else if(GetWorld()==2 || GetWorld()==5)
			{
				return ((anim/2)&7);
			}
			else if(GetWorld()==8)
				return table3[anim&7];
			break;
		case GUY_SPIDERBOT:
		case GUY_FIREANT:
			return table2[anim];
			break;
		case GUY_TRICERATOPS:
			return table4[anim];
			break;
		case GUY_EYEBALL1:
		case GUY_EYEBALL2:
		case GUY_EYEBALL3:
		case GUY_EYEBALL4:
			return table2[anim/2];
			break;
		case GUY_SCORPION:
			return table2[anim];
			break;
	}
	return 0;
}

int FigureShroomFrame(byte hp,byte world)
{
	byte table[]={0,0,1,2,3,3,2,1,0,0,4,5,6,6,5,4};
	byte table2[]={0,1,2,3,4,3,2,1};
	byte table3[]={0,1,2,1};
	byte table4[]={0,1,2,3,3,2,1,0};
	switch(world)
	{
		case WORLD_EARTH:
			return (table[(shroomAnim/4)&15])+(4-hp)*7;
			break;
		case WORLD_RIGEL:
			return 28+(4-hp);
			break;
		case WORLD_AGUA:
			return 32+(4-hp);
			break;
		case WORLD_TARTARUS:
			return 52+(4-hp);
			break;
		case WORLD_PROMETHEUS:
			return 36+(4*(4-hp))+((shroomAnim/4)&3);
			break;
		case WORLD_VARAKKIS:
			return (table[(shroomAnim/4)&15])+(4-hp)*7+56;
			break;
		case WORLD_JURASSIC:
			return (table[(shroomAnim/4)&7])+(4-hp)*5+84;
			break;
		case WORLD_GLOBULUS:
			return table3[(shroomAnim/4)&3]+(4-hp)*3+104;
			break;
		case WORLD_FINAL:
			return (116+4*(4-hp)+table4[((shroomAnim/4)&7)]);
			break;
		default:
			return 28+4-hp;
			break;
	}
}

void RenderGuy(guy_t *me)
{
	int i;

	switch(me->type)
	{
		case GUY_PLAYER:
			if(GetInvincible()&1)
				return;	// don't get drawn 50% of the time if you are invincible
			
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_ROCKET,
				me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_ROCKET,
				me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			
			if(GetShield())
			{
				if(IsBonusStage() || GetWorld()==WORLD_ASTEROID)
					SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,9,255,GetRand(3),SPR_BULLET,
					46,DISPLAY_DRAWME|DISPLAY_GLOW);
				else
					SprDraw(me->x>>FIXSHIFT,(me->y>>FIXSHIFT),19,255,GetRand(3),SPR_BULLET,
					89,DISPLAY_DRAWME|DISPLAY_GLOW);
			}
			break;
		case GUY_BULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,12,255,0,SPR_BULLET,
				0,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				0,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SHOTBULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				19,DISPLAY_DRAWME);
			break;
		case GUY_CHAINBULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				20,DISPLAY_DRAWME);
			break;
		case GUY_NUKE:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				24,DISPLAY_DRAWME);
			break;
		case GUY_NUKEBOOM:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				25+(me->anim/2),DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_LASER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				21+me->anim,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_PRESENT:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				48,DISPLAY_DRAWME);
			break;
		case GUY_MISSILE:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				1,DISPLAY_DRAWME);
			// also draw engine flame
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				6,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_SPIDER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_ICESPIDER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim+12,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim+12,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SPIDERBOT:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,AnimConvert(GUY_SPIDERBOT,me->anim)+92,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,AnimConvert(GUY_SPIDERBOT,me->anim)+92,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SNOWMAN:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim/2+24,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim/2+24,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_CENTIHEAD:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_PEDE1+GetWorld()-1,me->facing,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE1+GetWorld()-1,me->facing,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_CENTIBODY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE1+GetWorld()-1,
				8+me->facing*me->reload2+AnimConvert(GUY_CENTIBODY,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE1+GetWorld()-1,
				8+me->facing*me->reload2+AnimConvert(GUY_CENTIBODY,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_AQUAHEAD:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE3,me->facing*4+((me->anim/2)&3),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE3,me->facing*4+((me->anim/2)&3),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_AQUABODY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE3,4*8+me->facing*4+((me->anim/2)&3),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE3,4*8+me->facing*4+((me->anim/2)&3),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_AQUATAIL:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE3,4*16+me->facing*4+((me->anim/2)&3),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE3,4*16+me->facing*4+((me->anim/2)&3),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_ROBOHEAD:
			while(me->anim>5)
				me->anim-=6;
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_PEDE4,me->facing*12+6*(me->hp==1)+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE4,me->facing*12+6*(me->hp==1)+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_ROBOBODY:
			while(me->anim>5)
				me->anim-=6;
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_PEDE4,96+me->facing*12+6*(me->hp==1)+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE4,96+me->facing*12+6*(me->hp==1)+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SHROOM:
			i=FigureShroomFrame(me->hp,GetWorld());
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_SHROOM,
				i,DISPLAY_DRAWME);
			/*
			if(GetWorld()!=WORLD_AGUA)	// no shadows for lilypads
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_SHROOM,
					i,DISPLAY_DRAWME|DISPLAY_SHADOW);
					*/
			break;
		case GUY_RADIO:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_BULLET,
				17+me->anim/2,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				17+me->anim/2,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_WINGMAN:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_BULLET,
				47,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				47,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_FLY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,255,0,SPR_MONSTER,
				me->anim+30,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				me->anim+30,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_FLYSPIT:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,255,0,SPR_BULLET,
				49,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				49,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_YELLOWBULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_BULLET,
				54,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				54,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_STRIDER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				33+(me->facing/8)*4+(me->anim/8),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				33+(me->facing/8)*4+(me->anim/8),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_FROG:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->destx>>FIXSHIFT,255,0,SPR_MONSTER,
				65+(me->anim/2),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				65+me->anim/2,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_REPAIR:
			if(me->anim<8)
			{
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_MONSTER,
					84+me->anim,DISPLAY_DRAWME);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					84+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			}
			else
			{
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_MONSTER,
					84+15-me->anim,DISPLAY_DRAWME);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					84+15-me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			}
			break;
		case GUY_TURRET:
			if(me->hp>0)
			{
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					74+(me->anim/4),DISPLAY_DRAWME);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					74+(me->anim/4),DISPLAY_DRAWME|DISPLAY_SHADOW);
			}
			else
			{
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					83,DISPLAY_DRAWME);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					83,DISPLAY_DRAWME|DISPLAY_SHADOW);
			}
			break;
		case GUY_VOLCANO:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				99+(me->anim/2),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				99+me->anim/2,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_ROCK:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->destx>>FIXSHIFT,255,0,SPR_BULLET,
				55+((me->anim/2)&3),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				55+((me->anim/2)&3),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SPARKY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				104+(me->anim/2),DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_FLAME:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				109-(me->anim/4),DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_FIREANT:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				110+me->facing*7+AnimConvert(GUY_FIREANT,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				110+me->facing*7+AnimConvert(GUY_FIREANT,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_BIGROID:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				166+32+((me->anim/2)&15),DISPLAY_DRAWME);
			break;
		case GUY_MEDROID:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				166+((me->anim)&15),DISPLAY_DRAWME);
			break;
		case GUY_SMLROID:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				166+16+((me->anim)&15),DISPLAY_DRAWME);
			break;
		case GUY_LOBSTER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				214,DISPLAY_DRAWME);
			break;
		case GUY_LLOBCLAW:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,1,255,0,SPR_MONSTER,
				215+(me->anim/8),DISPLAY_DRAWME);
			break;
		case GUY_RLOBCLAW:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,1,255,0,SPR_MONSTER,
				220+(me->anim/8),DISPLAY_DRAWME);
			break;
		case GUY_LOBTAIL:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				225+me->reload2,DISPLAY_DRAWME);
			break;
		case GUY_FIREBALL:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				59,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_GREENLASER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				60+me->anim/2,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_GREENLASER2:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				64+me->anim/2,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_POWERUP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				73+me->anim,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_SQUIDSHIP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				257+(me->anim/2),DISPLAY_DRAWME);
			break;
		case GUY_PWRUPUFO:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				262+me->anim,DISPLAY_DRAWME);
			break;
		case GUY_CLAWSHIP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				245+(me->anim/8),DISPLAY_DRAWME);
			break;
		case GUY_SPINSHIP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				229+me->anim,DISPLAY_DRAWME);
			break;
		case GUY_SMALLSHIP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				250+me->anim,DISPLAY_DRAWME);
			break;
		case GUY_DOUBLESHIP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				276,DISPLAY_DRAWME);
			break;
		case GUY_FBALLSHIP:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				266+me->anim,DISPLAY_DRAWME);
			break;
		case GUY_TRICERATOPS:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				277+9*(me->facing==1)+AnimConvert(GUY_TRICERATOPS,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				277+9*(me->facing==1)+AnimConvert(GUY_TRICERATOPS,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_PTERODACTYL:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->destx>>FIXSHIFT,255,0,SPR_MONSTER,
				295+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				295+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_EYEBALL1:
		case GUY_EYEBALL2:
		case GUY_EYEBALL3:
		case GUY_EYEBALL4:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE9,
				(me->type-GUY_EYEBALL1)*56+me->facing*7+AnimConvert(GUY_EYEBALL1,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE9,
				(me->type-GUY_EYEBALL1)*56+me->facing*7+AnimConvert(GUY_EYEBALL1,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_EYEGUY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				306+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				306+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SCORPION:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				319+AnimConvert(GUY_SCORPION,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				319+AnimConvert(GUY_SCORPION,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_TUMBLEWEED:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->destx>>FIXSHIFT,255,0,SPR_MONSTER,
				326+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				326+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SANDWORM:
			if(me->destx==1)	// in bite mode (it's invisible otherwise)
			{
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					335+me->anim,DISPLAY_DRAWME);
				SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
					335+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			}
			break;
		case GUY_ALIENHEAD:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,3,255,0,SPR_PEDE6,
				me->facing*4+AnimConvert(GUY_ALIENHEAD,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE6,
				me->facing*4+AnimConvert(GUY_ALIENHEAD,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_ALIENBODY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE6,
				32+me->facing*7+AnimConvert(GUY_ALIENBODY,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE6,
				32+me->facing*7+AnimConvert(GUY_ALIENBODY,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_BABYALIEN:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				345+me->facing*7+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				345+me->facing*7+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_FLYALIEN:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,16,255,0,SPR_MONSTER,
				401+me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,
				401+me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_REDBULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_BULLET,
				90,DISPLAY_DRAWME);
			break;
		case GUY_QUEEN:
			SprDraw(me->x>>FIXSHIFT,(me->y>>FIXSHIFT),0,255,0,SPR_QUEEN,
				me->anim/16,DISPLAY_DRAWME);
			break;
		case GUY_QUEENHEAD:
			SprDraw(me->x>>FIXSHIFT,(me->y>>FIXSHIFT)+1,0,255,0,SPR_QUEEN,
				4+me->anim/2,DISPLAY_DRAWME);
			break;
		case GUY_QUEENTAIL:
			SprDraw(me->x>>FIXSHIFT,(me->y>>FIXSHIFT)-1,0,255,0,SPR_QUEEN,
				9+me->anim/2,DISPLAY_DRAWME);
			break;
		case GUY_ACIDGLOB:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->destx>>FIXSHIFT,255,0,SPR_BULLET,
				91,DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				91,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_FLYINGEGG:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->destx>>FIXSHIFT,255,0,SPR_BULLET,
				94+me->anim/2,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				94+me->anim/2,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_ACIDSPIT:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_BULLET,
				92+(me->anim&1),DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_BULLET,
				92+(me->anim&1),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_BLUEBULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_BULLET,
				99,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_SATELLITE:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,8,255,0,SPR_MONSTER,
				406+(me->anim/4),DISPLAY_DRAWME);
			break;
	}
}

void RenderGuys(void)
{
	int i;

	flip=1-flip;
	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type)
			RenderGuy(&guy[i]);
}

int FindTarget(byte type,int *x,int *y)
{
	int i,best,bestxd,bestyd,xd,yd;

	best=-1;
	
	for(i=0;i<MAX_GUYS;i++)
	{
		if(guy[i].type==type)
		{
			if(best==-1)
			{
				best=i;
				bestxd=abs(guy[best].x-goodguy->x);
				bestyd=abs(guy[best].y-goodguy->y);
			}
			else
			{
				xd=abs(guy[i].x-goodguy->x);
				if(xd<bestxd)
				{
					best=i;
					bestxd=abs(guy[best].x-goodguy->x);
					bestyd=abs(guy[best].y-goodguy->y);
				}
				else if(xd==bestxd)
				{
					yd=abs(guy[i].y-goodguy->y);
					if(yd<bestyd)
					{
						best=i;
						bestxd=abs(guy[best].x-goodguy->x);
						bestyd=abs(guy[best].y-goodguy->y);
					}
				}
			}
		}
	}
	if(best!=-1)
	{
		*x=(guy[best].x)>>FIXSHIFT;
		*y=(guy[best].y)>>FIXSHIFT;
		if(guy[best].type==GUY_PTERODACTYL)	// adjust for altitude
			*y-=(guy[best].destx>>FIXSHIFT);
	}
	else
	{
		*x=0;
		*y=0;
	}
	return best;
}

void SetHordeDX(int dx,int destx,int guynum)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
	{
		if(guy[i].type==GUY_SQUIDSHIP || guy[i].type==GUY_DOUBLESHIP || guy[i].type==GUY_FBALLSHIP)
			// if it is a horde ship
		{
			if(i>guynum)
				guy[i].x+=guy[i].dx*2;
			guy[i].dx=dx;
			guy[i].destx=destx;
		}
	}
}

void PlantShroom(int x,int y)
{
	int i;

	if(x<0 || x>=MAP_WIDTH || y<0 || y>=MAP_HEIGHT-2)
		return;	// can't add offscreen, or in the bottom two rows

	if(GetWorld()==WORLD_ASTEROID)
		return;	// no shrooms

	// first check to be sure nothing else is in this square
	for(i=0;i<MAX_GUYS;i++)
	{
		if(guy[i].type && (guy[i].flags&GF_GROWBLOCK) &&
			guy[i].rectx<(x*TILE_WIDTH)+TILE_WIDTH && 
			guy[i].rectx2>=(x*TILE_WIDTH) &&
			guy[i].recty<(y*TILE_HEIGHT)+TILE_HEIGHT && 
			guy[i].recty2>=(y*TILE_HEIGHT))
			return;
	}
	// okay, plant me
	AddGuy(GUY_SHROOM,x,y,0);
}

void PlantMonster(byte type,int x,int y)
{
	int i;

	if(x<0 || x>=MAP_WIDTH || y<0 || y>=MAP_HEIGHT-2)
		return;	// can't add offscreen, or in the bottom two rows

	// first check to be sure nothing else is in this square
	for(i=0;i<MAX_GUYS;i++)
	{
		if(guy[i].type && (guy[i].flags&GF_GROWBLOCK) &&
			guy[i].rectx<(x*TILE_WIDTH)+TILE_WIDTH && 
			guy[i].rectx2>=(x*TILE_WIDTH) &&
			guy[i].recty<(y*TILE_HEIGHT)+TILE_HEIGHT && 
			guy[i].recty2>=(y*TILE_HEIGHT))
			return;
	}
	// okay, plant me
	AddGuy(type,x,y,0);
}

void PlantMonster2(byte type,int x,int y)
{
	int i;

	if(x<0 || x>=MAP_WIDTH || y<0 || y>=MAP_HEIGHT-2)
		return;	// can't add offscreen, or in the bottom two rows

	// first check to be sure nothing else is in this square
	for(i=0;i<MAX_GUYS;i++)
	{
		if(guy[i].type && (guy[i].flags&GF_GROWBLOCK) &&
			guy[i].rectx<(x*TILE_WIDTH)+TILE_WIDTH && 
			guy[i].rectx2>=(x*TILE_WIDTH) &&
			guy[i].recty<(y*TILE_HEIGHT)+TILE_HEIGHT && 
			guy[i].recty2>=(y*TILE_HEIGHT))
		{
			if(guy[i].type==GUY_SHROOM)
			{
				numShrooms--;
				guy[i].type=GUY_NONE;
			}
		}
	}
	// okay, plant me
	AddGuy(type,x,y,0);
}

void PutPresents(int num)
{
	int i,x,y,tries,noadd;

	tries=0;
	while(num>0 && tries<10000)
	{
		x=GetRand(MAP_WIDTH);
		y=GetRand(MAP_HEIGHT/4)+MAP_HEIGHT-MAP_HEIGHT/4;
	
		noadd=0;
		// first check to be sure nothing else is in this square
		for(i=0;i<MAX_GUYS;i++)
		{
			if(guy[i].type && (guy[i].flags&GF_GROWBLOCK) &&
				guy[i].rectx<(x*TILE_WIDTH)+TILE_WIDTH && 
				guy[i].rectx2>=(x*TILE_WIDTH) &&
				guy[i].recty<(y*TILE_HEIGHT)+TILE_HEIGHT && 
				guy[i].recty2>=(y*TILE_HEIGHT))
			{
				tries++;
				noadd=1;
				i=MAX_GUYS;
			}
		}
		// okay, put me
		if(!noadd)
		{
			if(!AddGuy(GUY_PRESENT,x,y,0))
			{
				tries++;
				continue;
			}
			num--;
		}
	}
}

void MakeCentipede(int x,int y,int value,byte length)
{
	int i;

	switch(GetWorld())
	{
		case WORLD_EARTH:
		case WORLD_RIGEL:
		case WORLD_TARTARUS:
		case WORLD_JURASSIC:
		case WORLD_VARAKKIS:
			AddGuy(GUY_CENTIHEAD,x,y,value);
			for(i=0;i<length;i++)
				AddGuy(GUY_CENTIBODY,x,y,value);
			break;
		case WORLD_AGUA:
			AddGuy(GUY_AQUAHEAD,x,y,value);
			for(i=0;i<length;i++)
				AddGuy(GUY_AQUABODY,x,y,value);
			break;
		case WORLD_FINAL:
			AddGuy(GUY_ALIENHEAD,x,y,value);
			for(i=0;i<length;i++)
				AddGuy(GUY_ALIENBODY,x,y,value);
			break;
		case WORLD_PROMETHEUS:
			AddGuy(GUY_ROBOHEAD,x,y,value);
			for(i=0;i<length;i++)
				AddGuy(GUY_ROBOBODY,x,y,value);
			break;
	}
}

void MakeLobster(int x,int y)
{
	int i,j,core;

	core=AddGuyExact(GUY_LOBSTER,x,y,0);
	i=AddGuyExact(GUY_LOBTAIL,x,y,0);
	guy[i].parent=core;
	j=AddGuyExact(GUY_LOBTAIL,x,y,1);
	guy[j].parent=i;
	i=AddGuyExact(GUY_LOBTAIL,x,y,2);
	guy[i].parent=j;
	j=AddGuyExact(GUY_LOBTAIL,x,y,3);
	guy[j].parent=i;
	i=AddGuyExact(GUY_LLOBCLAW,x,y,0);
	guy[i].parent=core;
	i=AddGuyExact(GUY_RLOBCLAW,x,y,0);
	guy[i].parent=core;
}

void MakeQueen(void)
{
	int i,core;

	core=AddGuyExact(GUY_QUEEN,(SCR_WIDTH/2*FIXAMT),(SCR_HEIGHT/3*FIXAMT),0);
	i=AddGuyExact(GUY_QUEENHEAD,(SCR_WIDTH/2*FIXAMT),(SCR_HEIGHT/3*FIXAMT),0);
	guy[i].parent=core;
	i=AddGuyExact(GUY_QUEENTAIL,(SCR_WIDTH/2*FIXAMT),(SCR_HEIGHT/3*FIXAMT),0);
	guy[i].parent=core;
}

int AddGuyExact(byte type,int x,int y,byte val)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
		if(!guy[i].type)
		{
			guy[i].type=type;
			guy[i].x=x;
			guy[i].y=y;
			guy[i].parent=255;
			guy[i].flags=0;
			guy[i].num=i;
			switch(guy[i].type)
			{
				case GUY_BULLET:
					guy[i].flags=GF_GOODMEAN;
					guy[i].dx=0;
					guy[i].dy=-FIXAMT*12;
					break;
				case GUY_MISSILE:
					guy[i].flags=GF_GOODMEAN;
					guy[i].dx=-FIXAMT*8+GetRand(FIXAMT*16);
					guy[i].dy=-FIXAMT+GetRand(FIXAMT*4);
					guy[i].anim=0;
					break;
				case GUY_CHAINBULLET:
					guy[i].flags=GF_GOODMEAN;
					guy[i].dx=-FIXAMT*2+GetRand(FIXAMT*4);
					guy[i].dy=-FIXAMT*14;
					guy[i].anim=0;
					break;
				case GUY_SHOTBULLET:
					guy[i].flags=GF_GOODMEAN;
					guy[i].dx=-FIXAMT*4+FIXAMT*val*2;
					guy[i].dy=-FIXAMT*10+((abs(2-val))<<FIXSHIFT);
					guy[i].anim=0;
					break;
				case GUY_LASER:
					guy[i].flags=GF_GOODMEAN;
					guy[i].anim=0;
					guy[i].reload=2;
					guy[i].dx=0;
					guy[i].dy=0;
					break;
				case GUY_NUKE:
					guy[i].flags=GF_GOODMEAN;
					guy[i].anim=0;
					guy[i].dx=0;
					guy[i].dy=-FIXAMT*8;
					break;
				case GUY_WINGMAN:
					guy[i].reload2=val;	// reload2 tells the wingman whether to be left or right
										// of the player
					guy[i].flags=GF_GOOD;
					guy[i].anim=0;
					guy[i].dx=0;
					guy[i].dy=0;
					break;
				case GUY_FLYSPIT:
				case GUY_REDBULLET:
				case GUY_YELLOWBULLET:
				case GUY_BLUEBULLET:
					guy[i].flags=GF_EVILMEAN|GF_FLYER;
					guy[i].dx=0;
					guy[i].dy=FIXAMT*8;
					break;
				case GUY_ROCK:
					guy[i].flags=GF_EVILMEAN|GF_GHOST;
					guy[i].dx=-FIXAMT*4+GetRand(FIXAMT*9);
					guy[i].dy=-FIXAMT*4+GetRand(FIXAMT*9);
					guy[i].destx=FIXAMT*20;
					guy[i].desty=FIXAMT*4+GetRand(FIXAMT*16);
					break;
				case GUY_FLAME:
					guy[i].flags=GF_EVILMEAN|GF_FLYER;
					guy[i].anim=0;
					guy[i].reload2=3;
					break;
				case GUY_LOBSTER:
					guy[i].flags=GF_EVIL|GF_FLYER|GF_VICTORY;
					guy[i].hp=20+10*(GetLevel()-26);
					guy[i].reload=120;
					guy[i].reload2=1;
					guy[i].destx=6*TILE_WIDTH*FIXAMT;
					guy[i].desty=120;
					break;
				case GUY_LOBTAIL:
					guy[i].flags=GF_GHOST|GF_FLYER;
					guy[i].reload2=val;
					break;
				case GUY_RLOBCLAW:
				case GUY_LLOBCLAW:
					guy[i].hp=10+5*(GetLevel()-26);
					guy[i].flags=GF_EVIL|GF_FLYER;
					guy[i].reload=120+40*(type==GUY_RLOBCLAW);
					guy[i].reload2=2;
					guy[i].anim=0;
					break;
				case GUY_FIREBALL:
					guy[i].flags=GF_EVILMEAN|GF_FLYER;
					guy[i].dy=FIXAMT*8;
					guy[i].dx=0;
					break;
				case GUY_GREENLASER:
					guy[i].flags=GF_EVILMEAN|GF_FLYER;
					guy[i].reload=16;
					guy[i].anim=7;
					break;
				case GUY_GREENLASER2:
					guy[i].flags=GF_EVILMEAN|GF_FLYER;
					guy[i].reload=16;
					guy[i].anim=7;
					break;
				case GUY_BABYALIEN:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_GROWBLOCK;
					guy[i].reload2=1;
					guy[i].reload=3;
					guy[i].anim=0;
					guy[i].destx=0;
					guy[i].facing=2;
					guy[i].dx=0;
					guy[i].dy=FIXAMT*12;
					MakeSound(SND_BABYLEAP,1200);
					break;
				case GUY_QUEEN:
					guy[i].flags=GF_EVIL|GF_GROWBLOCK|GF_VICTORY;
					guy[i].desty=0;
					guy[i].anim=0;
					guy[i].hp=50;
					guy[i].reload=120;
					guy[i].reload2=60;
					guy[i].speed=1;
					break;
				case GUY_QUEENHEAD:
					guy[i].flags=GF_EVIL|GF_GROWBLOCK|GF_FLYER;
					guy[i].hp=30;
					guy[i].reload=30;
					guy[i].anim=0;
					guy[i].dx=0;
					break;
				case GUY_QUEENTAIL:
					guy[i].flags=GF_GHOST;
					guy[i].reload=30;
					guy[i].anim=0;
					break;
				case GUY_ACIDGLOB:
					guy[i].flags=GF_EVILMEAN|GF_GHOST;
					guy[i].dx=-FIXAMT*4+GetRand(FIXAMT*9);
					guy[i].dy=FIXAMT*3+GetRand(FIXAMT*4);
					guy[i].destx=FIXAMT*20;
					guy[i].desty=FIXAMT*12+GetRand(FIXAMT*3);
					break;
				case GUY_FLYINGEGG:
					guy[i].flags=GF_EVILMEAN|GF_GHOST;
					guy[i].dx=-FIXAMT*3+GetRand(FIXAMT*6);
					guy[i].dy=FIXAMT*3+GetRand(FIXAMT*3);
					guy[i].destx=FIXAMT*20;
					guy[i].desty=FIXAMT*4+GetRand(FIXAMT*4);
					break;
				case GUY_ACIDSPIT:
					guy[i].flags=GF_EVILMEAN|GF_FLYER;
					guy[i].dy=FIXAMT*8;
					guy[i].dx=0;
					guy[i].anim=0;
					break;
				case GUY_SATELLITE:
					guy[i].flags=GF_EVIL|GF_FLYER;
					guy[i].dy=-FIXAMT*3;
					guy[i].dx=GetRand(4*FIXAMT)+FIXAMT;
					guy[i].facing=0;
					guy[i].hp=2;
					guy[i].anim=0;
					if(val==0)
						guy[i].dx=-guy[i].dx;
					AddParticle(PART_SMOKE,guy[i].x,guy[i].y,FIXAMT,5*3+7);
					AddParticle(PART_SMOKE,guy[i].x,guy[i].y,FIXAMT,5*3+7);
					AddParticle(PART_SMOKE,guy[i].x,guy[i].y,FIXAMT,5*3+7);
					break;
			}
			UpdateGuyRect(&guy[i]);
			return i;
		}
	return -1;
}

void GiveWingman(void)
{
	AddGuyExact(GUY_WINGMAN,goodguy->x,goodguy->y,GetWingmen());
	SetWingmen(GetWingmen()+1);
}

void KillWingmen(void)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type==GUY_WINGMAN)
			GetHit(&guy[i]);
	SetWingmen(0);
}


void GiveBirthToAlien(int x,int y,byte lefty)
{
	int i,j;

	for(i=0;i<MAX_GUYS;i++)
		if(!guy[i].type)
		{
			guy[i].type=GUY_ALIENHEAD;
			guy[i].x=x;
			guy[i].y=y;
			guy[i].tx=(x/TILE_WIDTH)>>FIXSHIFT;
			guy[i].ty=(y/TILE_HEIGHT)>>FIXSHIFT;
			guy[i].parent=255;
			guy[i].hp=1;
			guy[i].num=i;
	
			guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN|GF_GHOST;
			guy[i].speed=FIXAMT*8;
			if(lefty==1)
			{
				guy[i].dx=-guy[i].speed;
				guy[i].flags|=GF_GOINGLEFT;
			}
			else
				guy[i].dx=guy[i].speed;
			guy[i].dy=0;
			guy[i].reload=GetLogLength(guy[i].speed)*2;
			guy[i].reload2=20;	// countdown until clear of mother, for bumping into things
			guy[i].anim=GetRand(6);
			guy[i].hp=2;
			guy[i].facing=byte(lefty==1)*4;

			for(j=0;j<LOG_LENGTH;j++)
			{
				guy[i].movelog[j].x=guy[i].x;
				guy[i].movelog[j].y=guy[i].y;
				guy[i].movelog[j].dx=0;
				guy[i].movelog[j].dy=0;
				guy[i].movelog[j].facing=0;
			}
			return;
		}
}

byte AddGuy(byte type,int x,int y,int value)
{
	int i,j;

	if(type==GUY_SHROOM)
	{
		if(numShrooms>=MAX_SHROOMS)
			return 0;		// can't add any more shrooms dammit!
		numShrooms++;
	}

	for(i=0;i<MAX_GUYS;i++)
		if(!guy[i].type)
		{
			guy[i].type=type;
			guy[i].x=(x*FIXAMT*TILE_WIDTH)+(TILE_WIDTH/2)*FIXAMT;
			guy[i].y=(y*FIXAMT*TILE_HEIGHT)+(TILE_HEIGHT/2)*FIXAMT;
			guy[i].tx=x;
			guy[i].ty=y;
			guy[i].parent=255;
			guy[i].hp=1;
			guy[i].flags=0;
			guy[i].num=i;
			switch(guy[i].type)
			{
				case GUY_PLAYER:
					guy[i].flags=GF_GROWBLOCK|GF_GOOD;
					guy[i].dx=0;
					guy[i].dy=0;
					guy[i].reload=0;
					guy[i].reload2=0;
					guy[i].facing=128;
					if(IsBonusStage() || GetWorld()==WORLD_ASTEROID)
						guy[i].anim=0;
					else
						guy[i].anim=13;
					goodguy=&guy[i];
					if(GetWingmen()>0)
						AddGuyExact(GUY_WINGMAN,guy[i].x,guy[i].y,0);
					if(GetWingmen()>1)
						AddGuyExact(GUY_WINGMAN,guy[i].x,guy[i].y,1);
					break;
				case GUY_CENTIHEAD:
					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					guy[i].speed=value;
					guy[i].dx=guy[i].speed/2;
					guy[i].dy=guy[i].speed;
					guy[i].reload=GetLogLength(guy[i].speed)*2;
					for(j=0;j<LOG_LENGTH;j++)
					{
						guy[i].movelog[j].x=guy[i].x;
						guy[i].movelog[j].y=guy[i].y;
						guy[i].movelog[j].dx=0;
						guy[i].movelog[j].dy=0;
						guy[i].movelog[j].facing=0;
					}
					break;
				case GUY_CENTIBODY:
					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					guy[i].anim=GetRand(256);
					guy[i].parent=i-1;
					switch(GetWorld())	// reload2 contains how many frames per facing he has
					{
						case 1:
						case 7:
							guy[i].reload2=5;
							break;
						case 2:
							guy[i].reload2=8;
							break;
						case 5:
							guy[i].reload2=8;
							break;
						case 8:
							guy[i].reload2=4;
							break;
						default:
							guy[i].reload2=1;
							break;
					}
					guy[i].speed=value;
					for(j=0;j<LOG_LENGTH;j++)
					{
						guy[i].movelog[j].x=guy[i].x;
						guy[i].movelog[j].y=guy[i].y;
						guy[i].movelog[j].dx=0;
						guy[i].movelog[j].dy=0;
						guy[i].movelog[j].facing=0;
					}
					break;
				case GUY_AQUAHEAD:
					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					guy[i].speed=value;
					guy[i].dx=guy[i].speed/2;
					guy[i].dy=guy[i].speed;
					guy[i].reload=GetLogLength(guy[i].speed)*2;
					guy[i].anim=0;
					for(j=0;j<LOG_LENGTH;j++)
					{
						guy[i].movelog[j].x=guy[i].x;
						guy[i].movelog[j].y=guy[i].y;
						guy[i].movelog[j].dx=0;
						guy[i].movelog[j].dy=0;
						guy[i].movelog[j].facing=0;
					}
					break;
				case GUY_AQUABODY:
					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					guy[i].anim=0;
					guy[i].parent=i-1;
					guy[i].speed=value;
					for(j=0;j<LOG_LENGTH;j++)
					{
						guy[i].movelog[j].x=guy[i].x;
						guy[i].movelog[j].y=guy[i].y;
						guy[i].movelog[j].dx=0;
						guy[i].movelog[j].dy=0;
						guy[i].movelog[j].facing=0;
					}
					break;
				case GUY_ROBOHEAD:
				case GUY_ALIENHEAD:
					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					guy[i].speed=value;
					guy[i].dx=guy[i].speed/2;
					guy[i].dy=guy[i].speed;
					guy[i].reload=GetLogLength(guy[i].speed)*2;
					guy[i].anim=GetRand(6);
					guy[i].hp=2;
					guy[i].reload2=0;
					guy[i].facing=2;
					for(j=0;j<LOG_LENGTH;j++)
					{
						guy[i].movelog[j].x=guy[i].x;
						guy[i].movelog[j].y=guy[i].y;
						guy[i].movelog[j].dx=0;
						guy[i].movelog[j].dy=0;
						guy[i].movelog[j].facing=0;
					}
					break;
				case GUY_ROBOBODY:
				case GUY_ALIENBODY:
					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					guy[i].anim=GetRand(6);
					guy[i].parent=i-1;
					guy[i].speed=value;
					guy[i].hp=2;
					for(j=0;j<LOG_LENGTH;j++)
					{
						guy[i].movelog[j].x=guy[i].x;
						guy[i].movelog[j].y=guy[i].y;
						guy[i].movelog[j].dx=0;
						guy[i].movelog[j].dy=0;
						guy[i].movelog[j].facing=0;
					}
					break;
				case GUY_SPIDER:
				case GUY_ICESPIDER:
				case GUY_SPIDERBOT:
				case GUY_FIREANT:
					guy[i].flags=GF_OFFSCREEN|GF_HALFSCRN|GF_GROWBLOCK|GF_SHROOMEAT|GF_EVIL|GF_EVILMEAN;
					guy[i].speed=value;
					if(guy[i].x<0)
					{
						guy[i].dx=guy[i].speed;
						guy[i].facing=0;
					}
					else
					{
						guy[i].dx=-guy[i].speed;
						guy[i].facing=4;
					}

					guy[i].dy=0;
					guy[i].reload=16;
					guy[i].anim=0;
					break;
				case GUY_SHROOM:
					guy[i].flags=GF_GROWBLOCK|GF_EVIL;
					guy[i].hp=4;
					break;
				case GUY_PRESENT:
					guy[i].flags=GF_GROWBLOCK|GF_GHOST;
					guy[i].hp=1;
					break;
				case GUY_SNOWMAN:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_FIXSHROOM;
					guy[i].desty=guy[i].y+8*FIXAMT;
					guy[i].dy=value;
					guy[i].anim=0;
					guy[i].hp=2;
					break;
				case GUY_FLY:
					MakeSound(SND_FLYARRIVE,1200);
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_FLYER;
					guy[i].destx=(SCR_WIDTH/2)<<FIXSHIFT;
					guy[i].desty=(SCR_HEIGHT/2)<<FIXSHIFT;
					guy[i].dx=0;
					guy[i].dy=0;
					guy[i].anim=0;
					guy[i].hp=1;
					guy[i].reload=30;
					guy[i].reload2=2;
					break;
				case GUY_STRIDER:
					guy[i].flags=GF_OFFSCREEN|GF_HALFSCRN|GF_GROWBLOCK|GF_SHROOMEAT|GF_EVIL|GF_EVILMEAN;
					guy[i].speed=value;
					if(guy[i].x<0)
					{
						guy[i].facing=0;
						guy[i].dx=guy[i].speed;
					}
					else
					{
						guy[i].dx=-guy[i].speed;
						guy[i].facing=8*4;
					}

					guy[i].dy=0;
					guy[i].reload=60;
					guy[i].anim=31;
					

					if(GetRand(2))
						guy[i].reload2=0;
					else
						guy[i].reload2=2;
					break;
				case GUY_FROG:
					MakeSound(SND_RIBBIT,1200);
					guy[i].flags=GF_OFFSCREEN|GF_FIXSHROOM|GF_EVIL|GF_EVILMEAN;
					guy[i].hp=2;
					guy[i].anim=0;
					guy[i].destx=0;
					guy[i].dx=FIXAMT*4;
					guy[i].dy=value;
					break;
				case GUY_TURRET:
					guy[i].flags=GF_GROWBLOCK|GF_EVIL;
					guy[i].hp=2;
					guy[i].anim=GetRand(8*4+3);
					guy[i].reload2=2;
					guy[i].reload=41+GetRand(60);
					break;
				case GUY_REPAIR:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_FIXSHROOM|GF_FIXTURRET;
					guy[i].dx=0;
					guy[i].dy=value;
					guy[i].speed=value;
					guy[i].reload2=0;
					guy[i].reload=GetRand(30)+2;
					guy[i].anim=0;
					guy[i].hp=1;
					break;
				case GUY_VOLCANO:
					guy[i].flags=GF_GROWBLOCK|GF_EVIL;
					guy[i].hp=10;
					guy[i].anim=0;
					guy[i].reload=120+GetRand(240);
					break;
				case GUY_SPARKY:
					guy[i].flags=GF_EVIL|GF_FLYER;
					guy[i].dx=FIXAMT*6;
					guy[i].anim=0;
					guy[i].reload2=2;
					guy[i].reload=GetRand(20)+1;
					break;
				case GUY_BIGROID:
					guy[i].flags=GF_EVIL|GF_EVILMEAN;
					if(IsBonusStage())
					{
						guy[i].reload2=1;
						guy[i].flags|=GF_OFFSCREEN;
					}
					else
						guy[i].reload2=0;
					guy[i].dx=-FIXAMT*2+GetRand(FIXAMT*4);
					guy[i].dy=-FIXAMT*2+GetRand(FIXAMT*4);
					if(IsBonusStage())
						guy[i].dy=FIXAMT/8+GetRand(FIXAMT*2);
					guy[i].anim=GetRand(256);
					break;
				case GUY_MEDROID:
					guy[i].flags=GF_EVIL|GF_EVILMEAN;
					if(IsBonusStage())
					{
						guy[i].reload2=1;
						guy[i].flags|=GF_OFFSCREEN;
					}
					else
						guy[i].reload2=0;
					guy[i].dx=-FIXAMT*4+GetRand(FIXAMT*8);
					guy[i].dy=-FIXAMT*4+GetRand(FIXAMT*8);
					if(IsBonusStage())
						guy[i].dy=FIXAMT/4+GetRand(FIXAMT*4);
					guy[i].anim=GetRand(256);
					break;
				case GUY_SMLROID:
					guy[i].flags=GF_EVIL|GF_EVILMEAN;
					if(IsBonusStage())
					{
						guy[i].reload2=1;
						guy[i].flags|=GF_OFFSCREEN;
					}
					else
						guy[i].reload2=0;
					guy[i].dx=-FIXAMT*6+GetRand(FIXAMT*12);
					guy[i].dy=-FIXAMT*6+GetRand(FIXAMT*12);
					if(IsBonusStage())
						guy[i].dy=FIXAMT/4+GetRand(FIXAMT*6);
					guy[i].anim=GetRand(256);
					break;
				case GUY_POWERUP:
					guy[i].flags=GF_GHOST|GF_VICTORY;
					guy[i].dx=FIXAMT*6;
					guy[i].dy=FIXAMT*2;
					guy[i].destx=guy[i].x;
					guy[i].anim=0;
					break;
				case GUY_SQUIDSHIP:
					guy[i].flags=GF_EVIL|GF_VICTORY;
					guy[i].dx=FIXAMT*2;
					guy[i].destx=SCR_WIDTH*FIXAMT-32*FIXAMT;
					guy[i].anim=0;
					guy[i].facing=2;
					break;
				case GUY_PWRUPUFO:
					guy[i].flags=GF_EVIL|GF_VICTORY;
					if(guy[i].x<0)
						guy[i].dx=value;
					else
						guy[i].dx=-value;
					guy[i].anim=0;
					break;
				case GUY_CLAWSHIP:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_VICTORY;
					guy[i].hp=10;
					guy[i].anim=0;
					guy[i].reload=value;
					guy[i].reload2=2;
					break;
				case GUY_SPINSHIP:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_VICTORY;
					guy[i].desty=FIXAMT*240;
					guy[i].dx=value;
					guy[i].dy=FIXAMT*3;
					break;
				case GUY_SMALLSHIP:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_VICTORY;
					guy[i].hp=2;
					if(value)
						guy[i].destx=25*FIXAMT*TILE_WIDTH;
					else
						guy[i].destx=5*FIXAMT*TILE_WIDTH;
					guy[i].dx=0;
					guy[i].dy=FIXAMT*3;
					guy[i].reload=20-(y*5);
					break;
				case GUY_FBALLSHIP:
					guy[i].hp=5;
					guy[i].flags=GF_EVIL|GF_VICTORY;
					guy[i].dx=FIXAMT*2;
					guy[i].destx=SCR_WIDTH*FIXAMT-32*FIXAMT;
					guy[i].anim=0;
					guy[i].facing=0;
					guy[i].reload=GetRand(80)+11;
					break;
				case GUY_DOUBLESHIP:
					guy[i].flags=GF_EVIL|GF_VICTORY;
					guy[i].dx=FIXAMT*2;
					guy[i].destx=SCR_WIDTH*FIXAMT-32*FIXAMT;
					guy[i].reload=GetRand(60);
					break;
				case GUY_TRICERATOPS:
					guy[i].flags=GF_OFFSCREEN|GF_HALFSCRN|GF_GROWBLOCK|GF_SHROOMEAT|GF_EVIL|GF_EVILMEAN;
					guy[i].hp=3;
					guy[i].dx=value;
					if(value>0)
						guy[i].facing=0;
					else
						guy[i].facing=1;
					MakeSound(SND_TRICERAROAR,1200);
					break;
				case GUY_PTERODACTYL:
					guy[i].flags=GF_OFFSCREEN|GF_EVIL|GF_EVILMEAN;
					if(guy[i].x<(SCR_WIDTH/2)*FIXAMT)
						guy[i].dx=GetRand(FIXAMT*3);
					else
						guy[i].dx=-GetRand(FIXAMT*3);
					guy[i].destx=10*FIXAMT;
					guy[i].dy=FIXAMT*3;
					guy[i].reload=2;
					guy[i].anim=0;
					guy[i].facing=0;
					break;
				case GUY_EYEBALL1:
				case GUY_EYEBALL2:
				case GUY_EYEBALL3:
				case GUY_EYEBALL4:
					if(guy[i].type==GUY_EYEBALL1)
						guy[i].hp=10;
					else if(guy[i].type==GUY_EYEBALL2)
						guy[i].hp=3;
					else
						guy[i].hp=1;

					guy[i].flags=GF_VICTORY|GF_EVIL|GF_EVILMEAN;
					if(value)
						guy[i].flags|=GF_GOINGLEFT;
					if(guy[i].type==GUY_EYEBALL1)
						guy[i].speed=FIXAMT*2;
					else if(guy[i].type==GUY_EYEBALL2)
						guy[i].speed=FIXAMT*4;
					else if(guy[i].type==GUY_EYEBALL3)
						guy[i].speed=FIXAMT*6;
					else
						guy[i].speed=FIXAMT*10;
					if(value)
					{
						guy[i].dx=-guy[i].speed;
						guy[i].facing=4;
					}
					else
					{
						guy[i].dx=guy[i].speed;
						guy[i].facing=0;
					}
					guy[i].dy=0;
					guy[i].anim=0;
					guy[i].reload=0;
					break;
				case GUY_EYEGUY:
					guy[i].speed=FIXAMT*2;
					guy[i].hp=5;
					guy[i].dx=FIXAMT*2;
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_SHROOMEAT|GF_GROWBLOCK;
					guy[i].reload=20;
					guy[i].anim=0;
					guy[i].reload2=0;
					break;
				case GUY_SCORPION:
					guy[i].hp=2;
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_GROWBLOCK;
					guy[i].dx=value;
					guy[i].dy=value;
					guy[i].reload=GetRand(20)+5;
					guy[i].anim=0;
					break;
				case GUY_TUMBLEWEED:
					guy[i].flags=GF_EVIL|GF_EVILMEAN;
					guy[i].dx=-FIXAMT+GetRand(FIXAMT*2);
					guy[i].dy=FIXAMT*3;
					// destx=Z coordinate, desty=DZ
					guy[i].destx=GetRand(FIXAMT*3)+1;
					guy[i].desty=FIXAMT*2;
					break;
				case GUY_SANDWORM:
					guy[i].flags=GF_OFFSCREEN|GF_SANDWORM;
					guy[i].dx=0;
					guy[i].dy=0;
					guy[i].destx=0;
					guy[i].hp=1;
					break;
				case GUY_FLYALIEN:
					guy[i].flags=GF_EVIL|GF_EVILMEAN|GF_FLYER;
					guy[i].destx=(SCR_WIDTH/2)<<FIXSHIFT;
					guy[i].desty=(SCR_HEIGHT/2)<<FIXSHIFT;
					guy[i].dx=0;
					guy[i].dy=0;
					guy[i].anim=0;
					guy[i].hp=1;
					guy[i].reload=30;
					guy[i].reload2=2;
					break;
			}
			UpdateGuyRect(&guy[i]);
			return 1;
		}
	return 0;
}
