#include "particle.h"
#include "random.h"
#include "display.h"

typedef struct particle_t
{
	byte type;
	int x,y,z;
	int dx,dy,dz;
	byte size;
	byte timer;
	byte color;
} particle_t;

particle_t particle[MAX_PARTICLES];
byte gravity;

void InitParticles(void)
{
	ClearParticles();
	gravity=1;
}

void ExitParticles(void)
{
	// nothing to do
}

void GravitySwitch(byte on)
{
	gravity=on;
}

void ClearParticles(void)
{
	int i;

	for(i=0;i<MAX_PARTICLES;i++)
		particle[i].type=PART_NONE;
}

void UpdateParticle(particle_t *me)
{
	byte v;

	me->x+=me->dx;
	me->y+=me->dy;
	me->z+=me->dz;

	if(gravity)
		me->dz-=FIXAMT;	// gravity
	else
		me->dz=0;

	// bounce if you hit the ground
	if(me->z<0)
	{
		me->z=0;
		me->dz=-me->dz/2;
	}

	if(me->timer)
		me->timer--;
	else
	{
		me->type=PART_NONE;
		return;
	}

	switch(me->type)
	{
		case PART_PURPLE:
			v=me->timer;
			if(v>31-8)
				v=31-8;
			me->color=192+v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_RED:
			v=me->timer;
			if(v>31-8)
				v=31-8;
			me->color=128+v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_YELLOW:
			v=me->timer;
			if(v>31)
				v=31;
			me->color=160+v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_BLUE:
			v=me->timer;
			if(v>31)
				v=31;
			me->color=96+v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_GREEN:
			v=me->timer;
			if(v>31-8)
				v=31-8;
			me->color=32+v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_GREY:
			v=me->timer;
			if(v>31-13)
				v=31-13;
			me->color=v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_ENGINEGLOW:
			v=me->timer/2;
			if(v>31)
				v=31;
			me->color=v;
			if(me->timer>20)
				me->size=3;
			else if(me->timer>12)
				me->size=2;
			else if(me->timer<6)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_SNOW:
			v=me->timer;
			if(v>31)
				v=31;
			if(v<20)
				v=20;
			me->color=v;
			if(me->timer>10)
				me->size=2;
			else if(me->timer<3)
				me->size=0;
			else
				me->size=1;
			if(gravity)
				me->dz+=FIXAMT*3/4;	// less gravity
			me->dx+=FIXAMT/8-GetRand(FIXAMT/4);	// drift around
			me->dy+=FIXAMT/8-GetRand(FIXAMT/4);
			if(me->z==0)
				me->type=PART_NONE;
			break;
		case PART_SPARKLY:
			v=me->timer;
			if(v>31)
				v=31;
			if(v<8)
				v=8;
			me->color=GetRand(8)*32+v;
			if(me->timer>20)
				me->size=2;
			else if(me->timer<10)
				me->size=0;
			else
				me->size=1;
			break;
		case PART_SHOCKWAVE:
		case PART_RIPPLE:
		case PART_SMOKE:
		case PART_LILBOOM:
		case PART_BIGBOOM:
		case PART_SPUTTER:
		case PART_SANDWORM:
			me->z=0;
			me->dz=0;
			break;
		case PART_SMOKE2:
			if(gravity)
				me->dz+=FIXAMT;	// no gravity
			me->dx+=FIXAMT/8-GetRand(FIXAMT/4);
			me->dy+=FIXAMT/8-GetRand(FIXAMT/4);
			break;
	}
}

void UpdateParticles(void)
{
	int i;

	for(i=0;i<MAX_PARTICLES;i++)
		if(particle[i].type)
			UpdateParticle(&particle[i]);
}

void RenderParticles(void)
{
	int i;
	int x,y;

	for(i=0;i<MAX_PARTICLES;i++)
		if(particle[i].type)
		{
			x=particle[i].x>>FIXSHIFT;
			y=(particle[i].y-particle[i].z)>>FIXSHIFT;
			switch(particle[i].type)
			{
				case PART_SMOKE:
				case PART_SMOKE2:
					GhostSprite(SPR_BULLET,5-particle[i].timer/8,x,y,0);
					break;
				case PART_LILBOOM:
					GlowSprite(SPR_BULLET,16-particle[i].timer/2,x,y,0);
					break;
				case PART_BIGBOOM:
					GlowSprite(SPR_BULLET,11-particle[i].timer/4,x,y,0);
					break;
				case PART_SPUTTER:
					GhostSprite(SPR_BULLET,72-particle[i].timer,x,y,0);
					break;
				case PART_ENGINEGLOW:
					RenderGlowParticle(x,y,particle[i].color,particle[i].size);
					break;
				default:
					RenderParticle(x,y,particle[i].color,particle[i].size);
					break;
				case PART_SHOCKWAVE:
				case PART_RIPPLE:
				case PART_SANDWORM:
					break;
			}
		}
}

void PreRenderParticles(void)
{
	int i;
	int x,y;

	for(i=0;i<MAX_PARTICLES;i++)
		if(particle[i].type)
		{
			x=particle[i].x>>FIXSHIFT;
			y=(particle[i].y-particle[i].z)>>FIXSHIFT;
			switch(particle[i].type)
			{
				case PART_SHOCKWAVE:
					GhostSprite(SPR_BULLET,45-particle[i].timer,x,y,0);
					break;
				case PART_RIPPLE:
					GhostSprite(SPR_BULLET,53-particle[i].timer/4,x,y,0);
					break;
				case PART_SANDWORM:
					GhostSprite(SPR_MONSTER,332+GetRand(3),x,y,0);
					break;
				default:
					// no other particles should be pre-drawn
					break;
			}
		}
}

void AddParticle(byte type,int x,int y,int force,byte life)
{
	int i;

	for(i=0;i<MAX_PARTICLES;i++)
	{
		if(!particle[i].type)
		{
			particle[i].type=type;
			particle[i].x=x;
			particle[i].y=y;
			particle[i].z=0;
			particle[i].dx=(-force+GetRand(force*2+1));
			particle[i].dy=(-force+GetRand(force*2+1));
			particle[i].dz=GetRand(force)+FIXAMT*4;
			particle[i].timer=life;
			UpdateParticle(&particle[i]);
			return;
		}
	}
}

void AddParticleExact(byte type,int x,int y,int z,int dx,int dy,int dz,byte life)
{
	int i;

	for(i=0;i<MAX_PARTICLES;i++)
	{
		if(!particle[i].type)
		{
			particle[i].type=type;
			particle[i].x=x;
			particle[i].y=y;
			particle[i].z=z;
			particle[i].dx=dx;
			particle[i].dy=dy;
			particle[i].dz=dz;
			particle[i].timer=life;
			UpdateParticle(&particle[i]);
			return;
		}
	}
}

void ExplodeParticles(byte type,int x,int y,byte force,byte count)
{
	int i;

	for(i=0;i<MAX_PARTICLES;i++)
	{
		if(!particle[i].type)
		{
			count--;
			particle[i].type=type;
			particle[i].x=x;
			particle[i].y=y;
			particle[i].z=0;
			particle[i].dx=(-(force<<FIXSHIFT)+GetRand((force*2+1)<<FIXSHIFT));
			particle[i].dy=(-(force<<FIXSHIFT)+GetRand((force*2+1)<<FIXSHIFT));
			particle[i].dz=GetRand(force<<FIXSHIFT)+FIXAMT*4;
			particle[i].timer=30+GetRand(12);
			UpdateParticle(&particle[i]);
			if(!count)
				return;
		}
	}
}

void ExplodeParticlesHigh(byte type,int x,int y,byte force,byte count)
{
	int i;

	for(i=0;i<MAX_PARTICLES;i++)
	{
		if(!particle[i].type)
		{
			count--;
			particle[i].type=type;
			particle[i].x=x;
			particle[i].y=y;
			particle[i].z=FIXAMT*20;
			particle[i].dx=(-(force<<FIXSHIFT)+GetRand((force*2+1)<<FIXSHIFT));
			particle[i].dy=(-(force<<FIXSHIFT)+GetRand((force*2+1)<<FIXSHIFT));
			particle[i].dz=GetRand(force<<FIXSHIFT)+FIXAMT*4;
			particle[i].timer=30+GetRand(12);
			UpdateParticle(&particle[i]);
			if(!count)
				return;
		}
	}
}