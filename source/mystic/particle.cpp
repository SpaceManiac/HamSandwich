#include "particle.h"
#include "bullet.h"
#include "monster.h"
#include "player.h"

Particle **particleList;
int		maxParticles;
static int snowCount=0;

Particle::Particle(void)
{
	this->life=0;
}

Particle::~Particle(void)
{
}

void Particle::Go(byte type,int x,int y,int z,byte angle,byte force)
{
	byte fforce;

	if(force==0)
		return;

	this->type=type;
	size=2;
	fforce=force/4;

	if(fforce==0)
		fforce=1;
	this->x=x+MGL_randoml(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->y=y+MGL_randoml(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->z=z;
	this->dx=Cosine(angle)*MGL_random(fforce);
	this->dy=Sine(angle)*MGL_random(fforce);
	this->dz=MGL_random(fforce*2)<<FIXSHIFT;
	this->life=MGL_random(force)+10;
}

void Particle::GoRandom(byte type,int x,int y,int z,byte force)
{
	this->type=type;
	size=2;
	if(force==0)
		return;

	this->x=x+MGL_randoml(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->y=y+MGL_randoml(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->z=z;
	this->dx=(MGL_random(force)-force/2)<<FIXSHIFT;
	this->dy=(MGL_random(force)-force/2)<<FIXSHIFT;
	this->dz=MGL_random(force*2)<<FIXSHIFT;
	this->life=MGL_random(force)+20;
}

void Particle::Update(Map *map)
{
	byte mapx,mapy;
	byte v;
	byte c1;
	char brt;

	if(life>0)
	{
		dz-=FIXAMT;
		x+=dx;
		y+=dy;
		z+=dz;
		if(z<0)
		{
			z=0;
			dz=-dz/2;
		}
		life--;
		switch(type)
		{
			case PART_FLOATER:
				x-=dx;
				y-=dy;	// it doesn't move that way
				dz+=FIXAMT+MGL_random(FIXAMT/4);	// no gravity and going up
				x-=Cosine(size)*dx;
				y-=Sine(size)*dx;
				size+=MGL_random(12);
				x+=Cosine(size)*dx;
				y+=Sine(size)*dx;
				dx+=dy;
				break;
			case PART_GHOST:
				dz+=FIXAMT;	// no gravity
				break;
			case PART_COMPASS:
				dz+=FIXAMT;	// no gravity
				v=life;
				if(v>31-8)
					v=31-8;
				color=32+4+v;
				if(life>10)
					size=2;
				else if(life<5)
					size=0;
				else
					size=1;
				break;
			case PART_SMOKE:
				dz+=FIXAMT;	// no gravity
				z+=FIXAMT;
				size=(6-life/8);
				dx+=MGL_random(65535)-FIXAMT/2;
				dy+=MGL_random(65535)-FIXAMT/2;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				break;
			case PART_BOOM:
				dz+=FIXAMT;
				z+=FIXAMT;
				size=7-life;
				break;
			case PART_HAMMER:
				v=life;
				if(v>31-8)
					v=31-8;
				color=128+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_MANA:
				dz+=FIXAMT+FIXAMT;
				v=life*2+8;
				if(v>31)
					v=31;
				color=96+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_LVLUP:
				dz+=FIXAMT+FIXAMT/8;
				break;
			case PART_YELLOW:
				v=life;
				if(v>31-4)
					v=31-4;
				color=160+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_SHORTYELLOW:
				v=life*4;
				if(v>31)
					v=31;
				color=160+v;
				if(life>5)
					size=2;
				else if(life<2)
					size=0;
				else
					size=1;
				break;
			case PART_DIRT:
				v=life;
				if(v>31-8)
					v=31-8;
				color=64+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_SNOW2:
				v=life*2;
				if(v>31)
					v=31;
				if(v<31-16)
					v=31-16;
				color=v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_WATER:
				v=life;
				if(v>31)
					v=31;
				if(v<8)
					v=8;
				if(player.worldNum==2 && player.levelNum==20)
					color=32+v;
				else if(player.worldNum==2 && player.levelNum==22)
					color=32*6+v;
				else
					color=96+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_SLIME:
				v=life;
				if(v>31-8)
					v=31-8;
				color=32+4+v;
				if(life>10)
					size=2;
				else if(life<5)
					size=0;
				else
					size=1;
				break;
			case PART_SNOW:
				dx+=MGL_random(65535)-FIXAMT/2;
				dy+=MGL_random(65535)-FIXAMT/2;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				dz+=FIXAMT-256;	// not as much gravity as other things
				color=31;
				if(z==0)
				{
					dx=0;
					dy=0;
					dz=0;
					if(life<50)
						size=1;
					if(life<25)
						size=0;
					color=life/4;
					if(color<20)
						color=20;
					if(color>31)
						color=31;
					snowCount++;
				}
				else
					life++;	// can't die while airborne
				break;
		}


		if(x<0 || y<0 || x>=((map->width*TILE_WIDTH)<<FIXSHIFT) || y>=((map->height*TILE_HEIGHT)<<FIXSHIFT))
		{
			life=0;
			return;
		}

		if(type==PART_GHOST)
		{
		}
		else if(type==PART_SMOKE || type==PART_BOOM)
		{
			mapx=(x/TILE_WIDTH)>>FIXSHIFT;
			mapy=(y/TILE_HEIGHT)>>FIXSHIFT;
			color=64+map->map[mapx+mapy*map->width].templight;
		}
		else
		{
			mapx=(x/TILE_WIDTH)>>FIXSHIFT;
			mapy=(y/TILE_HEIGHT)>>FIXSHIFT;

			// brighten it appropriately
			brt=map->map[mapx+mapy*map->width].templight;
			c1=(color&(~31));	// c1 is the color range
			color+=brt;
			if(color>c1+31 || color<c1)
			{
				if(brt>0)
					color=c1+31;
				else
					color=c1;
			}
		}
	}
}

bool Particle::Alive(void)
{
	return(life>0);
}

//--------------------------------------------------------------------------

void InitParticles(int max)
{
	int i;

	maxParticles=max;

	particleList=(Particle **)malloc(sizeof(Particle *)*maxParticles);
	for(i=0;i<maxParticles;i++)
		particleList[i]=new Particle();
}

void ExitParticles(void)
{
	int i;

	for(i=0;i<maxParticles;i++)
		delete particleList[i];

	free(particleList);
}

void UpdateParticles(Map *map)
{
	int i;

	snowCount=0;
	for(i=0;i<maxParticles;i++)
		particleList[i]->Update(map);
}

void RenderParticle(int x,int y,byte *scrn,byte color,byte size)
{
	byte c1,c2;

	if(x<0 || x>639 || y<0 || y>479)
		return;

	switch(size)
	{
		case 2:	// big particle
			if(x<2 || x>637 || y<2 || y>477)
				return;

			if((color&31)>1)
				c1=color-2;	// only do this if subtracting 2 keeps it in the same color group
			else
				c1=color;
			if((c1&31)>1)
				c2=c1-2;	// only do this if subtracting 4 keeps it in the same color group
			else
				c2=c1;

			scrn+=(x+(y-2)*640);
			*scrn=c2;
			scrn+=639;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			scrn+=637;
			*scrn++=c2;
			*scrn++=c1;
			*scrn++=color;
			*scrn++=c1;
			*scrn=c2;
			scrn+=637;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			*(scrn+639)=c2;
			break;
		case 1:	// normal particle
			if(x<1 || x>638 || y<1 || y>478)
				return;
			if(color&31)
				c1=color-1;	// only do this if subtracting 1 keeps it in the same color group
			else
				c1=color;
			scrn+=(x+(y-1)*640);
			*scrn=c1;
			scrn+=639;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			scrn+=639;
			*scrn=c1;
			break;
		case 0:	// tiny particle (1 pixel)
			scrn[x+y*640]=color;
			break;
	}
}

void RenderParticles(void)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(particleList[i]->Alive())
		{
			if(particleList[i]->type==PART_SMOKE)
				RenderSmoke(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,(char)(particleList[i]->color),
							 particleList[i]->size);
			else if(particleList[i]->type==PART_BOOM)
				RenderBoom(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,(char)(particleList[i]->color),
							 particleList[i]->size);
			else if(particleList[i]->type==PART_LVLUP)
			{
				if(particleList[i]->life>20 || (particleList[i]->life&1))
					RenderLvlUp(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
								 particleList[i]->z>>FIXSHIFT);
			}
			else if(particleList[i]->type==PART_GHOST)
				SprDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
						particleList[i]->z>>FIXSHIFT,255,0,GetMonsterSprite2(MONS_BOUAPHA,particleList[i]->size),
						DISPLAY_DRAWME|DISPLAY_GLOW);
			else if(particleList[i]->type==PART_FLOATER)
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,(particleList[i]->life/10),
							 DISPLAY_DRAWME|DISPLAY_PARTICLE);
			else
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,particleList[i]->size,
							 DISPLAY_DRAWME|DISPLAY_PARTICLE);
		}
	}
}

void BlowSmoke(int x,int y,int z,int dz)
{
	int i;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x;
			particleList[i]->y=y;
			particleList[i]->z=z;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=dz;
			particleList[i]->life=6*4-MGL_random(8);
			particleList[i]->size=16;
			particleList[i]->color=64;
			particleList[i]->type=PART_SMOKE;
			break;
		}
	}
}

void BlowWigglySmoke(int x,int y,int z,int dz)
{
	int i;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x+MGL_randoml(FIXAMT*4)-FIXAMT*2;
			particleList[i]->y=y+MGL_randoml(FIXAMT*4)-FIXAMT*2;
			particleList[i]->z=z;
			particleList[i]->dx=MGL_randoml(FIXAMT*2)-FIXAMT;
			particleList[i]->dy=MGL_randoml(FIXAMT*2)-FIXAMT;
			particleList[i]->dz=dz;
			particleList[i]->life=6*4-MGL_random(8);
			particleList[i]->size=16;
			particleList[i]->color=64;
			particleList[i]->type=PART_SMOKE;
			break;
		}
	}
}

void BlowUpGuy(int x,int y,int x2,int y2,int z,byte amt)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=(x+MGL_randoml(x2-x))<<FIXSHIFT;
			particleList[i]->y=(y+MGL_randoml(y2-y))<<FIXSHIFT;
			particleList[i]->z=z;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->life=7;
			particleList[i]->size=0;
			particleList[i]->color=64;
			particleList[i]->type=PART_BOOM;
			MakeSound(SND_BOMBBOOM,particleList[i]->x,particleList[i]->y,SND_CUTOFF,1800);
			if(!(--amt))
				break;
		}
	}
}

void SpurtParticles(byte type,bool left,int x,int y,int z,byte angle,byte force)
{
	int i,amt;
	byte ang2;

	amt=force;

	x+=Cosine(angle)*10;
	y+=Sine(angle)*10;
	if(left)
		ang2=angle+64;
	else
		ang2=angle+128+64;

	x+=Cosine(ang2)*20;
	y+=Sine(ang2)*20;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->Go(type,x,y,z,angle,force);
			if(!--amt)
				break;
		}
	}
}

void ExplodeParticles(byte type,int x,int y,int z,byte force)
{
	int i,amt;

	amt=force;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->GoRandom(type,x,y,z,force);
			if(!--amt)
				break;
		}
	}
}

void ExplodeParticles2(byte type,int x,int y,int z,byte num,byte force)
{
	int i;

	if(num==0)
		return;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->GoRandom(type,x,y,z,force);
			if(!--num)
				break;
		}
	}
}

void ManaParticles(int x,int y)
{
	int i,num;

	num=30;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->GoRandom(PART_MANA,x,y,FIXAMT*10,2);
			particleList[i]->dz=-((int)MGL_randoml(FIXAMT));
			if(!--num)
				break;
		}
	}
}

void TrailMe(int x,int y,byte f)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->type=PART_GHOST;
			particleList[i]->x=x;
			particleList[i]->y=y;
			particleList[i]->z=0;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->life=5;
			particleList[i]->size=f;
			break;
		}
	}
}

void AddParticle(int x,int y,int z,int dx,int dy,int dz,byte life,byte type,byte color)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{

			particleList[i]->x=x;
			particleList[i]->y=y;
			particleList[i]->z=z;
			particleList[i]->dx=dx;
			particleList[i]->dy=dy;
			particleList[i]->dz=dz;
			particleList[i]->size=2;
			particleList[i]->life=life;
			particleList[i]->type=type;
			particleList[i]->color=color;
			break;
		}
	}
}

void MakeItSnow(Map *map)
{
	int i;
	int cx,cy;

	// only 25% of particles may be snowflakes
	if(MGL_random(100)>30 || snowCount>maxParticles/4)
		return;

	GetCamera(&cx,&cy);
	cx-=320;
	cy-=240;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{

			particleList[i]->x=(MGL_random(640)+cx)<<FIXSHIFT;
			particleList[i]->y=(MGL_random(480)+cy)<<FIXSHIFT;
			particleList[i]->z=(300+MGL_random(300))<<FIXSHIFT;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->size=2;
			particleList[i]->life=50+MGL_random(50);
			particleList[i]->type=PART_SNOW;
			particleList[i]->color=31;
			break;
		}
	}
}

void SpecialSnow(int x,int y)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{

			particleList[i]->x=x;
			particleList[i]->y=y;
			particleList[i]->z=(10+MGL_random(20))<<FIXSHIFT;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->size=2;
			particleList[i]->life=20+MGL_random(30);
			particleList[i]->type=PART_SNOW;
			break;
		}
	}
}

void CompassTrail(int sx,int sy,int ex,int ey)
{
	int i;
	int incx,incy,n;

	sx=(sx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	ex=(ex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	sy=(sy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	ey=(ey*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;

	incx=(ex-sx)/10;
	incy=(ey-sy)/10;

	n=MGL_randoml(FIXAMT);
	sx=sx+(incx/FIXAMT)*n;
	sy=sy+(incy/FIXAMT)*n;

	for(i=0;i<10;i++)
	{
		AddParticle(sx,sy,FIXAMT*20,0,0,0,20,PART_COMPASS,1);
		sx+=incx;
		sy+=incy;
		if(incx>0 && sx>ex)
			sx=ex;
		if(incx<0 && sx<ex)
			sx=ex;
		if(incy>0 && sy>ey)
			sy=ey;
		if(incy<0 && sy<ey)
			sy=ey;
	}
}

void FloaterParticles(int x,int y,byte color,int radius,int spread,byte count)
{
	int i;
	byte ang,numLeft;

	ang=(byte)MGL_random(256);
	numLeft=count;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{

			particleList[i]->x=x+Cosine(ang)*radius;
			particleList[i]->y=y+Sine(ang)*radius;
			particleList[i]->z=0;
			particleList[i]->dx=radius;	// dx=radius
			particleList[i]->dy=spread;	// dy=spread (how rapidly they move outward or inward)
			particleList[i]->dz=0;
			particleList[i]->size=ang;
			particleList[i]->life=29;
			particleList[i]->type=PART_FLOATER;
			particleList[i]->color=color*32+16;
			ang+=(256/count);
			numLeft--;
			if(!numLeft)
				break;
		}
	}
}
