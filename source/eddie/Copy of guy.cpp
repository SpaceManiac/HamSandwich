#include "guy.h"
#include "player.h"
#include "tile.h"
#include "display.h"
#include "particle.h"
#include "random.h"
#include "sound.h"
#include "level.h"

#define MAX_GUYS 256

// guy flags
#define	GF_GOINGUP	1	// if true, the centipede is heading up, not down
#define GF_GOINGLEFT 2	// if true, the centipede is heading left, not right
#define GF_OFFSCREEN 4	// if true, this monster may move offscreen
#define GF_VICTORY	8	// if a frame goes by in which there are no living monsters with this
						// flag, the player wins (ergo, all centipede parts need this flag)
#define GF_HALFSCRN 16	// this creature is confined the lower part of the screen, like the player

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
	byte flags;

	byte anim;
	byte facing;
	byte hp;
	byte reload;
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

void InitGuys(void)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
		guy[i].type=GUY_NONE;
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
		case FIXAMT*4:
			return 3;
		case FIXAMT*8:
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
			SpriteRect(SPR_ROCKET,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->rectx+=8;
			me->rectx2-=8;
			me->recty+=4;
			me->recty2-=2;
			break;
		case GUY_BULLET:
			SpriteRect(SPR_BULLET,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_MISSILE:
			SpriteRect(SPR_BULLET,1,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_SPIDER:
			SpriteRect(SPR_MONSTER,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
		case GUY_CENTIHEAD:
			//SpriteRect(SPR_PEDE1,2,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->rectx=mx-7;
			me->recty=my-7;
			me->rectx2=mx+7;
			me->recty2=my+7;
			break;
		case GUY_CENTIBODY:
			//SpriteRect(SPR_PEDE1,2,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			me->rectx=mx-7;
			me->recty=my-7;
			me->rectx2=mx+7;
			me->recty2=my+7;
			break;
		case GUY_SHROOM:
			SpriteRect(SPR_SHROOM,0,mx,my,&me->rectx,&me->recty,&me->rectx2,&me->recty2);
			break;
	}
}

void GetHit(guy_t *me,int i)
{
	int sx,sy;

	switch(me->type)
	{
		case GUY_CENTIHEAD:
			PlayerGetPoints(100);
		case GUY_CENTIBODY:
			PlayerGetPoints(50);	// so a centihead is worth 150
			MakeSound(SND_POW,1200);
			if(guy[i+1].type==GUY_CENTIBODY)
			{
				guy[i+1].type=GUY_CENTIHEAD;
				guy[i+1].anim=0;
				guy[i+1].parent=255;
				guy[i+1].flags=guy[i].flags;
				guy[i+1].tx=(guy[i+1].x/TILE_WIDTH)>>FIXSHIFT;
				guy[i+1].ty=(guy[i+1].y/TILE_HEIGHT)>>FIXSHIFT;
				if(guy[i+1].flags&GF_GOINGLEFT)
				{
					guy[i+1].destx=guy[i+1].tx-1;
				}
				else
				{
					guy[i+1].destx=guy[i+1].tx+1;
				}
				guy[i+1].desty=guy[i+1].ty;
			}
			sx=(me->x>>FIXSHIFT)/TILE_WIDTH;
			sy=(me->y>>FIXSHIFT)/TILE_HEIGHT;

			if(me->flags&GF_GOINGLEFT)
				sx--;
			else
				sx++;

			TellLevelAboutIt(EVT_MONSDIE,me->type);
			me->type=GUY_NONE;
			PlantShroom(sx,sy);
			break;
		case GUY_SHROOM:
			MakeSound(SND_PLUNK,1000);
			me->hp--;
			if(me->hp==0)
			{
				PlayerGetPoints(5);
				me->type=GUY_NONE;
			}
			break;
		case GUY_SPIDER:
			TellLevelAboutIt(EVT_MONSDIE,GUY_SPIDER);
			PlayerGetPoints(100);
			MakeSound(SND_POW,1200);
			me->type=GUY_NONE;
			break;
	}
}

byte CheckEnemyHit(guy_t *me)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && guy[i].type!=GUY_BULLET && guy[i].type!=GUY_PLAYER &&
				guy[i].type!=GUY_MISSILE)
		{
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				// hit!
				GetHit(&guy[i],i);
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
		if((guy[i].type) && guy[i].type!=GUY_BULLET && guy[i].type!=GUY_PLAYER && guy[i].type!=GUY_MISSILE)
		{
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				// check here to see if the guy you bumped was somebody who hurts you
				return 1;
			}
		}
	}
	return 0;
}

byte MonsterCheckMovement(guy_t *me)
{
	int i,topy,j;

	topy=0+(SCR_HEIGHT-(SCR_HEIGHT/4))*((me->flags&GF_HALFSCRN)>0);
	if(!(me->flags&GF_OFFSCREEN))
		if(me->rectx<0 || me->rectx2>=SCR_WIDTH || me->recty2>=SCR_HEIGHT || me->recty<=topy)
			return 1;	// can't go offscreen

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && (&guy[i]!=me))
		{
			if(guy[i].type==GUY_PLAYER && GetInvincible()>0)
				continue;	// invincible players don't get in your way
			if(me->rectx<=guy[i].rectx2 && me->recty<=guy[i].recty2 && me->rectx2>=guy[i].rectx && 
				me->recty2>=guy[i].recty)
			{
				if(guy[i].parent!=255)
				{
					// if this guy turns out to be my child, then no collision
					j=guy[i].parent;
					while(j!=255)
					{
						if(j==me->num)
							continue;	// it's my child
						j=guy[j].parent;
					}
				}
				// check here to see if the guy you bumped was somebody who hurts you
				return 1;
			}
		}
	}
	return 0;
}

byte MonsterInPath(guy_t *me,int mynum)
{
	int i;
	int mex,mey,mex2,mey2;

	mex=me->rectx;
	mey=me->recty;
	mex2=me->rectx2;
	mey2=me->recty2;

	if(me->flags&GF_GOINGLEFT)
		mex-=TILE_WIDTH+TILE_WIDTH/4;
	else
		mex2+=TILE_WIDTH+TILE_WIDTH/4;

	mex+=TILE_WIDTH/4;
	mex2-=TILE_WIDTH/4;
	mey+=TILE_WIDTH/4;
	mey2-=TILE_WIDTH/4;

	for(i=0;i<MAX_GUYS;i++)
	{
		if((guy[i].type) && guy[i].type!=GUY_BULLET && i!=mynum)
		{
			if(guy[i].type==GUY_PLAYER && GetInvincible()>0)
				continue;	// invincible players don't get in your way
			if(mex<=guy[i].rectx2 && mey<=guy[i].recty2 && 
				mex2>=guy[i].rectx && mey2>=guy[i].recty)
			{
				// hit!
				if(guy[i].type==GUY_PLAYER)
				{
					SetInvincible(60);
					AddParticle(PART_BIGBOOM,guy[i].x,guy[i].y,1,19);
				}
				return 1;
			}
		}
	}
	return 0;
}

// here are the update functions for the different GUY types
//--------------------------------------------------------------

void UpdatePlayer(guy_t *me)
{
	int msx,msy;

	if(me->reload)
		me->reload--;

	GetMouse(&msx,&msy);

	me->dx=(msx<<FIXSHIFT)/2;
	me->dy=(msy<<FIXSHIFT)/2;
	
	Clamp(&me->dx,FIXAMT*16);
	Clamp(&me->dy,FIXAMT*12);

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
	
	if(MouseHeld() && !me->reload)
	{
		AddGuyExact(GUY_BULLET,me->x,me->y);
		MakeSound(SND_CLICK,1000);
		me->reload=10;
	}
	if(RMouseClick())
	{
		AddGuyExact(GUY_MISSILE,me->x,me->y);
		AddGuyExact(GUY_MISSILE,me->x,me->y);
		AddGuyExact(GUY_MISSILE,me->x,me->y);
		AddGuyExact(GUY_MISSILE,me->x,me->y);
		AddGuyExact(GUY_MISSILE,me->x,me->y);
		MakeSound(SND_MISSILE,1000);
	}
}

void UpdateBullet(guy_t *me)
{
	me->y+=me->dy;
	if(me->y<0)
		me->type=GUY_NONE;
	if(CheckEnemyHit(me))
	{
		me->type=GUY_NONE;
		ExplodeParticles(PART_PURPLE,me->x,me->y,4,6);
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
		me->type=GUY_NONE;
		AddParticle(PART_LILBOOM,me->x,me->y,1,9);
		//ExplodeParticles(PART_PURPLE,me->x,me->y,4,6);
	}
}

void UpdateCentihead(guy_t *me,int n)
{
	int rx,ry,i;
	byte there=0;

	rx=(me->destx*TILE_WIDTH+TILE_WIDTH/2)<<FIXSHIFT;
	ry=(me->desty*TILE_HEIGHT+TILE_HEIGHT/2)<<FIXSHIFT;

	for(i=LOG_LENGTH-1;i>0;i--)
		me->movelog[i]=me->movelog[i-1];

	me->dx=0;
	me->dy=0;
	if(rx>me->x)
	{
		me->dx=me->speed;
	}
	else if(rx<me->x)
	{
		me->dx=-me->speed;
	}
	if(ry>me->y)
	{
		me->dy=me->speed;
	}
	else if(ry<me->y)
	{
		me->dy=-me->speed;
	}	

	me->x+=me->dx;
	UpdateGuyRect(me);
	while(me->dx && MonsterCheckMovement(me))
	{
		i=1;
		me->x-=me->dx;
		Dampen(&me->dx,FIXAMT);
		me->x+=me->dx;
		UpdateGuyRect(me);
	}

	me->y+=me->dy;
	UpdateGuyRect(me);
	while(me->dy && MonsterCheckMovement(me))
	{
		i=1;
		me->y-=me->dy;
		Dampen(&me->dy,FIXAMT);
		me->y+=me->dy;
		UpdateGuyRect(me);
	}

	me->movelog[0].dx=me->dx;
	me->movelog[0].dy=me->dy;
	me->movelog[0].x=me->x;
	me->movelog[0].y=me->y;
	if(me->dx>0)
	{
		if(me->dy<0)
			me->movelog[0]->facing=8;
		else if(me->dy>0)
			me->movelog[0]->facing=2;
		else
			me->movelog[0]->facing=1;
	}
	else if(me->dx<0)
	{
		if(me->dy<0)
			me->movelog[0]->facing=6;
		else if(me->dy>0)
			me->movelog[0]->facing=4;
		else
			me->movelog[0]->facing=5;
	}
	else
	{
		if(me->dy<0)
			me->movelog[0]->facing=7;
		else if(me->dy>0)
			me->movelog[0]->facing=3;
		else
			me->movelog[0]->facing=0;
	}
	if(me->movelog[0]->facing>0)
		me->facing=me->movelog[0]->facing-1;
	else me->facing=2;

	if(me->x==rx && me->y==ry)
		there=1;

	if(there)	// got to destination
	{
		me->tx=me->destx;
		me->ty=me->desty;
		if(me->flags&GF_GOINGLEFT)
		{
			if(me->tx<1 || MonsterInPath(me,n) || me->ty<0)
			{
				me->flags&=(~GF_GOINGLEFT);	// time to go right
				if(me->flags&GF_GOINGUP)
				{
					me->desty--;
					if(me->desty<1)
						me->flags&=(~GF_GOINGUP);
				}
				else
				{
					me->desty++;
					if(me->desty>MAP_HEIGHT-2)
						me->flags|=GF_GOINGUP;
				}
			}
			else
			{
				me->destx--;
			}
		}
		else	// going right
		{
			if(me->tx>=MAP_WIDTH-1 || MonsterInPath(me,n) || me->ty<0)
			{
				me->flags|=GF_GOINGLEFT;	// time to go left
				if(me->flags&GF_GOINGUP)
				{
					me->desty--;
					if(me->desty<1)
						me->flags&=(~GF_GOINGUP);
				}
				else
				{
					me->desty++;
					if(me->desty>MAP_HEIGHT-2)
						me->flags|=GF_GOINGUP;
				}
			}
			else
			{
				me->destx++;
			}
		}
	}
}

void UpdateCentibody(guy_t *me)
{
	int i;

	me->flags=guy[me->parent].flags;

	for(i=LOG_LENGTH-1;i>0;i--)
		me->movelog[i]=me->movelog[i-1];

	me->movelog[0]=guy[me->parent].movelog[GetLogLength(me->speed)];

	if(me->movelog[0]==1 || me->movelog[0]==2 || me->movelog[0]==8)
		me->x+=me->speed;
	if(me->movelog[0]==2 || me->movelog[0]==3 || me->movelog[0]==4)
		me->y+=me->speed;
	if(me->movelog[0]==4 || me->movelog[0]==5 || me->movelog[0]==6)
		me->x-=me->speed;
	if(me->movelog[0]==6 || me->movelog[0]==7 || me->movelog[0]==8)
		me->y-=me->speed;

	if(me->movelog[0]>0)
		me->facing=me->movelog[0]-1;
	else me->facing=2;

	me->anim++;
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
		me->anim=0;

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
			UpdateBullet(me);
			break;
		case GUY_MISSILE:
			UpdateMissile(me);
			break;
		case GUY_CENTIHEAD:
			UpdateCentihead(me,n);
			break;
		case GUY_CENTIBODY:
			UpdateCentibody(me);
			break;
		case GUY_SPIDER:
			UpdateSpider(me);
			break;
		case GUY_SHROOM:
			break;
	}
}

byte UpdateGuys(void)
{
	int i;
	byte v;

	v=0;
	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type)
		{
			if(guy[i].flags&GF_VICTORY)
				v=1;
			UpdateGuy(&guy[i],i);
		}
	// returns whether or not there are any existing VICTORY creatures (once they are all
	// dead, the level is won)
	return v;
}

byte AnimConvert(byte type,byte anim)
{
	byte table1[]={0,1,2,1,0,3,4,3};

	switch(type)
	{
		case GUY_CENTIBODY:
			anim/=2;
			anim&=7;
			return table1[anim];
			break;
	}
	return 0;
}

void RenderGuy(guy_t *me)
{
	switch(me->type)
	{
		case GUY_PLAYER:
			if(GetInvincible()&1)
				return;	// don't get drawn 50% of the time if you are invincible
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_ROCKET,
				me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_ROCKET,
				me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_BULLET:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_BULLET,
				0,DISPLAY_DRAWME);
			break;
		case GUY_MISSILE:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_BULLET,
				1,DISPLAY_DRAWME);
			// also draw engine flame
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_BULLET,
				6,DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case GUY_SPIDER:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_MONSTER,me->anim,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_CENTIHEAD:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,6,255,0,SPR_PEDE1,me->facing,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE1,me->facing,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_CENTIBODY:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE1,
				8+me->facing*5+AnimConvert(GUY_CENTIBODY,me->anim),DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_PEDE1,
				8+me->facing*5+AnimConvert(GUY_CENTIBODY,me->anim),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case GUY_SHROOM:
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_SHROOM,
				4-me->hp,DISPLAY_DRAWME);
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,0,SPR_SHROOM,
				4-me->hp,DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
	}
}

void RenderGuys(void)
{
	int i;

	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type)
			RenderGuy(&guy[i]);
}

void PlantShroom(int x,int y)
{
	int i;

	if(x<0 || x>=MAP_WIDTH || y<0 || y>=MAP_HEIGHT-2)
		return;	// can't add offscreen, or in the bottom two rows

	// first check to be sure nothing else is in this square
	for(i=0;i<MAX_GUYS;i++)
		if(guy[i].type && guy[i].x==(x*FIXAMT*TILE_WIDTH)+(TILE_WIDTH/2)*FIXAMT &&
			guy[i].y==(y*FIXAMT*TILE_HEIGHT)+(TILE_HEIGHT/2)*FIXAMT)
			return;

	// okay, plant me
	AddGuy(GUY_SHROOM,x,y,0);
}

void MakeCentipede(int x,int y,int value,byte length)
{
	int i;

	AddGuy(GUY_CENTIHEAD,x,y,value);

	for(i=0;i<length;i++)
		AddGuy(GUY_CENTIBODY,x,y,value);
}

void AddGuyExact(byte type,int x,int y)
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
				case GUY_PLAYER:
					guy[i].dx=0;
					guy[i].dy=0;
					guy[i].reload=0;
					break;
				case GUY_BULLET:
					guy[i].dx=0;
					guy[i].dy=-FIXAMT*12;
					break;
				case GUY_MISSILE:
					guy[i].dx=-FIXAMT*8+GetRand(FIXAMT*16);
					guy[i].dy=-FIXAMT+GetRand(FIXAMT*4);
					guy[i].anim=0;
					break;
			}
			UpdateGuyRect(&guy[i]);
			return;
		}
}

void AddGuy(byte type,int x,int y,int value)
{
	int i,j;

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
					guy[i].dx=0;
					guy[i].dy=0;
					guy[i].reload=0;
					guy[i].facing=128;
					goodguy=&guy[i];
					break;
				case GUY_BULLET:
					guy[i].dx=0;
					guy[i].dy=-FIXAMT*12;
					break;
				case GUY_CENTIHEAD:
					guy[i].flags=GF_VICTORY;
					guy[i].destx=guy[i].tx+1;
					guy[i].desty=guy[i].ty;
					guy[i].speed=value;
					for(j=0;j<LOG_LENGTH;j++)
						guy[i].movelog[j]=0;
					break;
				case GUY_CENTIBODY:
					guy[i].anim=0;
					guy[i].parent=i-1;
					guy[i].flags=GF_VICTORY;
					guy[i].speed=value;
					for(j=0;j<LOG_LENGTH;j++)
						guy[i].movelog[j]=0;
					break;
				case GUY_SPIDER:
					guy[i].speed=value;
					if(guy[i].x<0)
						guy[i].dx=guy[i].speed;
					else
						guy[i].dx=-guy[i].speed;

					guy[i].flags=GF_OFFSCREEN|GF_HALFSCRN;
					guy[i].dy=0;
					guy[i].reload=16;
					guy[i].anim=0;
					break;
				case GUY_SHROOM:
					guy[i].hp=4;
					break;
			}
			UpdateGuyRect(&guy[i]);
			return;
		}
}
