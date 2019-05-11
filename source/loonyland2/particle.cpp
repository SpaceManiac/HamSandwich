#include "particle.h"
#include "bullet.h"
#include "monster.h"
#include "options.h"
#include "player.h"
#include "leveldef.h"
#include "skill.h"

Particle **particleList;
int		maxParticles;
static snowCount=0;

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
	this->x=x+Random(4<<FIXSHIFT)-(2<<FIXSHIFT);
	this->y=y+Random(4<<FIXSHIFT)-(2<<FIXSHIFT);
	this->z=z;
	this->dx=Cosine(angle)*Random(fforce);
	this->dy=Sine(angle)*Random(fforce);
	this->dz=Random(fforce*2)<<FIXSHIFT;
	this->life=Random(force)+10;
}

void Particle::GoExact(byte type,int x,int y,int z,byte angle,byte force)
{
	if(force==0)
		return;

	this->type=type;
	size=6;

	this->x=x;
	this->y=y;
	this->z=z;
	this->dx=Cosine(angle)*force;
	this->dy=Sine(angle)*force;
	this->dz=force*FIXAMT*2;
	this->life=25;
	this->color=(byte)(Random(7)+1)*32+16;
	if(this->color==32*2+16)
		this->color=32*7+16;
}

void Particle::GoLightning(int x,int y,int x2,int y2)
{
	this->type=PART_LIGHTNING;
	this->x=x;
	this->y=y;
	this->dx=x2;
	this->dy=y2;
	this->life=2;
	this->color=64;
	this->size=(abs((x2-x)>>FIXSHIFT)+abs((y2-y)>>FIXSHIFT))/8;
}

void Particle::GoRandom(byte type,int x,int y,int z,byte force)
{
	this->type=type;
	size=2;
	if(force==0)
		return;

	this->x=x+Random(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->y=y+Random(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->z=z;
	this->dx=(Random(force)-force/2)<<FIXSHIFT;
	this->dy=(Random(force)-force/2)<<FIXSHIFT;
	this->dz=Random(force*2)<<FIXSHIFT;
	this->life=Random(force)+20;
}

void Particle::Update(Map *map)
{
	byte mapx,mapy;
	byte v;
	byte c1;
	char brt;
	int camx,camy;

	// kill ones that go too far afield
	GetCamera(&camx,&camy);
	if(((camx-x/FIXAMT)>330 ||
		(camy-y/FIXAMT)>250) && type!=PART_LIGHTNING)
	{
		life=0;
		return;
	}

	if(life>0)
	{
		if(type!=PART_LIGHTNING)
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
		}

		life--;
		switch(type)
		{
			case PART_CLOUD:
				dz=FIXAMT;
				if(life>30 && size<30)
					size++;
				if(life<30 && size>1)
					size--;
				color=31;
				dx+=Random(FIXAMT/2+1)-FIXAMT/4;
				dy+=Random(FIXAMT/2+1)-FIXAMT/4;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				break;
			case PART_SMOKE:
				dz+=FIXAMT+FIXAMT/8;
				dx+=-FIXAMT/16+rand()%(FIXAMT/8+1);
				dy+=-FIXAMT/16+rand()%(FIXAMT/8+1);
				color=life/4;
				break;
			case PART_GHOST:
				color=life/2;
				dz+=FIXAMT;
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
			case PART_GLASS:
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				break;
			case PART_DIRT:
				v=life;
				if(v>31-16)
					v=31-16;
				color=64+v;
				if(player.levelNum>=LVL_GEYSER && player.levelNum<=LVL_TITANS)
					color=32*6+v;
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
			case PART_FLOATY:
				v=life;
				if(v>31-8)
					v=31-8;
				color=32*5+4+v;
				size=life/4+1;
				dz+=FIXAMT+FIXAMT/4;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				dx=dx-FIXAMT/8+Random(FIXAMT/4);
				dy=dy-FIXAMT/8+Random(FIXAMT/4);
				break;
			case PART_WATER:
				v=life;
				if(v>31)
					v=31;
				if(v<8)
					v=8;
				color=96+v;
				size=life/3+1;
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
			case PART_YELLOW:
				v=life;
				if(v>31-8)
					v=31-8;
				color=32*5+4+v;
				if(life>10)
					size=2;
				else if(life<5)
					size=0;
				else
					size=1;
				break;
			case PART_SNOW:
				dx+=Random(FIXAMT)-FIXAMT/2;
				dy+=Random(FIXAMT)-FIXAMT/2;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				dz+=FIXAMT*15/16;	// not as much gravity as other things
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
					color=life/2;
					if(color<26)
						color=26;
					if(color>31)
						color=31;
					snowCount++;
				}
				else
					life++;	// can't die while airborne
				break;
			case PART_LIGHTNING:
				// get dimmer with each frame
				color/=2;
				break;
			case PART_CIRCLE:
				dz+=FIXAMT;
				if(size>1)
				{
					size-=2;
					if(size<0)
					{
						size=0;
						life=0;
					}
				}
				if(color>228)
					color-=2;
				break;
			case PART_RING:
				size+=8;
				life-=7;
				if(color>228)
					color-=2;
				break;
			case PART_FIRE:
				dz+=FIXAMT+Random(FIXAMT/4);
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				dx=dx-FIXAMT/8+Random(FIXAMT/4);
				dy=dy-FIXAMT/8+Random(FIXAMT/4);
				if(size>0)
					size--;
				else
					life=0;
				if(life>40)
					color=191;
				else if(life>20)
					color=160+(life-20);
				else
					color=128+life;
				break;
			case PART_COLDFIRE:
				dz+=FIXAMT+Random(FIXAMT/4);
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				dx=dx-FIXAMT/8+Random(FIXAMT/4);
				dy=dy-FIXAMT/8+Random(FIXAMT/4);
				if(size>0)
					size--;
				else
					life=0;
				if(color>0)
					color--;
				else
					life=0;
				break;
			case PART_FX:
				dz+=FIXAMT+FIXAMT/16;
				size=life/2;
				if(x<tx)
				{
					dx+=FIXAMT/4;
					dy-=FIXAMT/16;
				}
				if(x>tx)
				{
					dx-=FIXAMT/4;
					dy+=FIXAMT/16;
				}
				if(y>ty)
				{
					dx+=FIXAMT/16;
					dy-=FIXAMT/4;
				}
				if(y<ty)
				{
					dx-=FIXAMT/16;
					dy+=FIXAMT/4;
				}
				break;
			case PART_NUM:
			case PART_XP:
			case PART_BAN:
				dz+=FIXAMT;
				break;
		}


		if(x<0 || y<0 || x>=((map->width*TILE_WIDTH)<<FIXSHIFT) || y>=((map->height*TILE_HEIGHT)<<FIXSHIFT))
		{
			life=0;
			return;
		}

		if(type==PART_LIGHTNING || type==PART_GLASS || type==PART_FIRE || type==PART_COLDFIRE || type==PART_NUM ||
			type==PART_XP || type==PART_FX || type==PART_BAN)
		{
			// nothing to do
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

void HLine(int x,int x2,int y,byte c,byte *scrn)
{
	if(x<0)
		x=0;
	if(x2>639)
		x2=639;
	if(x2<0)
		return;
	if(x>639)
		return;
	if(y<0)
		return;
	if(y>479)
		return;

	scrn+=(x+y*640);
	memset(scrn,c,x2-x+1);
}

void HLine2(int x,int x2,int y,byte c,byte *scrn)
{
	int i;
	byte b,b2,b3;
	byte c1,c2;

	if(x<0)
		x=0;
	if(x2>639)
		x2=639;
	if(x2<0)
		return;
	if(x>639)
		return;
	if(y<0)
		return;
	if(y>479)
		return;

	c1=(c&31);
	c2=(c&(~31));
	scrn+=(x+y*640);
	for(i=x;i<=x2;i++)
	{
		b=*scrn;
		b2=b&(~31);
		b3=(b&31)+c1;
		if(b3>31)
			b3=31;
		*scrn=(b3+c2);
		scrn++;
	}
}

void HLine3(int x,int x2,int y,byte c,byte less,byte *scrn)
{
	int i;
	byte b,b2,b3;
	byte colRange;
	int centerCol;
	int curCol;
	int dCol;
	byte realCol;

	if(x<0)
		x=0;
	if(x2>639)
		x2=639;
	if(x2<0)
		return;
	if(x>639)
		return;
	if(y<0)
		return;
	if(y>479)
		return;

	if((c&31)<less)
		return;
	else
		c-=less;

	if(x2-x<2)
		return;	// too skinny to draw

	centerCol=(c&31)*FIXAMT;
	curCol=0;
	dCol=(centerCol/((x2-x)/2));

	colRange=(c&(~31));
	scrn+=(x+y*640);
	for(i=x;i<=x2;i++)
	{
		realCol=(curCol/FIXAMT);
		b=*scrn;
		b2=b&(~31);
		b3=(b&31)+realCol;
		if(b3>31)
			b3=31;
		if(b3<6)
			b3=6;
		*scrn=(b3+colRange);
		scrn++;

		curCol+=dCol;
		if(curCol>=centerCol)
		{
			dCol=-dCol;
			curCol=centerCol;
		}
		if(curCol<0)
			curCol=0;
	}
}

void RenderCircleParticle(int x,int y,int radius,byte c,byte *scrn)
{
	int d,curX,curY;

	d=3-(2*radius);
	curX=0;
	curY=radius;

	if((c&31)==0)
		return;

	while(curX<=curY)
	{
		HLine3(x-curY,x+curY,y+curX,c,curX,scrn);
		if(curX!=0)
			HLine3(x-curY,x+curY,y-curX,c,curX,scrn);

		if(d<=0)
			d=d+(curX*4)+6;
		else
		{
			d=d+4*(curX-curY)+10;
			if(curY!=curX)
			{
				HLine3(x-curX,x+curX,y+curY,c,curY,scrn);
				HLine3(x-curX,x+curX,y-curY,c,curY,scrn);
			}
			curY--;
		}
		curX++;
	}
}

void RenderRingParticle(int x,int y,int radius,byte c,byte *scrn)
{
	int d,curX,curY,pitch;

	d=3-(2*radius);
	curX=0;
	curY=radius;

	pitch=GetDisplayMGL()->GetWidth();

	while(curX<=curY)
	{
		PlotPixel(x-curX,y+curX,c,scrn,pitch);
		PlotPixel(x+curX,y+curX,c,scrn,pitch);

		if(curX!=0)
		{
			PlotPixel(x-curY,y-curX,c,scrn,pitch);
			PlotPixel(x+curY,y-curX,c,scrn,pitch);
		}
		if(d<=0)
			d=d+(curX*4)+6;
		else
		{
			d=d+4*(curX-curY)+10;
			if(curY!=curX)
			{
				PlotPixel(x-curX,y+curY,c,scrn,pitch);
				PlotPixel(x+curX,y+curY,c,scrn,pitch);
				PlotPixel(x-curX,y-curY,c,scrn,pitch);
				PlotPixel(x+curX,y-curY,c,scrn,pitch);
			}
			curY--;
		}
		curX++;
	}
}

void RenderGlowCircleParticle(int x,int y,int radius,byte c,byte *scrn)
{
	int i;
	int y1,y2,y3;
	int wid;

	wid=0;
	y1=y-(radius*3/4);
	y2=y-(radius/2);
	y3=y-(radius/4);
	for(i=y-radius+1;i<=y;i++)
	{
		HLine2(x-wid,x+wid,i,c,scrn);
		if(i>=y3)
			wid--;
		else if(i>=y2)
			wid++;
		else if(i>=y1)
			wid=1;
	}
}

// this was going to be local to renderlightningparticle, but that would've seriously
// chomped up the stack, since that function's recursive.
byte ctab[]={4,2,4,
			 2,1,2,
			 4,2,4};

void RenderLightningParticle(int x1,int y1,int x2,int y2,int range,byte bright,byte *scrn)
{
	int midx,midy;
	byte *ctptr;
	byte b,brt;

	if((x1<0 && x2<0) || (y1<0 && y2<0) || (x1>639 && x2>639) ||
		(y1>479 && y2>479))
		return;	// quit if both ends are off-screen

	// base case: draw the (x1,y1) pixel
	if((x1-x2<2 && x1-x2>-2) && (y1-y2<2 && y1-y2>-2))
	{
		if(x1>=0 && x1<635 && y1>=0 && y1<475)
		{
			scrn+=(x1+y1*640);
			ctptr=&ctab[0];
			for(midy=y1;midy<y1+3;midy++)
			{
				for(midx=x1;midx<x1+3;midx++)
				{
					b=*scrn;
					brt=bright>>(*ctptr);
					if(((b+brt)&(~31))!=(b&(~31)))
						*scrn=(b&(~31))+31;
					else
						*scrn=b+brt;
					ctptr++;
					scrn++;
				}
				scrn+=640-3;
			}
		}
	}
	else	// recursive case, find a (midx,midy) between the other two points
	{
		if(range<1)
			range=1;
		if(x1<x2)
			midx=x1+(x2-x1)/2;
		else
			midx=x2+(x1-x2)/2;
		midx+=Random(range)-range/2;
		if(y1<y2)
			midy=y1+(y2-y1)/2;
		else
			midy=y2+(y1-y2)/2;
		midy+=Random(range)-range/2;
		RenderLightningParticle(x1,y1,midx,midy,range*3/4,bright,scrn);
		RenderLightningParticle(midx,midy,x2,y2,range*3/4,bright,scrn);
	}
}

void RenderParticles(void)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(particleList[i]->Alive())
		{
			if(particleList[i]->type==PART_LIGHTNING)
				LightningDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
								particleList[i]->dx>>FIXSHIFT,particleList[i]->dy>>FIXSHIFT,
								particleList[i]->color,(char)particleList[i]->size);
			else if(particleList[i]->type==PART_NUM || particleList[i]->type==PART_XP)
			{
				if(particleList[i]->life>10 || (particleList[i]->life&1))
					NumberDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,particleList[i]->z>>FIXSHIFT,particleList[i]->type,particleList[i]->color,
							   particleList[i]->num,DISPLAY_DRAWME|DISPLAY_NUMBER);
			}
			else if(particleList[i]->type==PART_BAN)
			{
				if(particleList[i]->life>10 || (particleList[i]->life&1))
					NumberDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,particleList[i]->z>>FIXSHIFT,particleList[i]->type,particleList[i]->color,
							   particleList[i]->num,DISPLAY_DRAWME|DISPLAY_NUMBER);
			}
			else if(particleList[i]->type==PART_CIRCLE || particleList[i]->type==PART_FLOATY)
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,(byte)Random(particleList[i]->size+1),
							 DISPLAY_DRAWME|DISPLAY_CIRCLEPART);
			else if(particleList[i]->type==PART_FX || particleList[i]->type==PART_WATER)
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,particleList[i]->size,
							 DISPLAY_DRAWME|DISPLAY_CIRCLEPART);
			else if(particleList[i]->type==PART_CLOUD)
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,particleList[i]->size,
							 DISPLAY_DRAWME|DISPLAY_CIRCLEPART);
			else if(particleList[i]->type==PART_RING)
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,particleList[i]->size,
							 DISPLAY_DRAWME|DISPLAY_RINGPART);
			else if(particleList[i]->type==PART_FIRE || particleList[i]->type==PART_COLDFIRE)
			{
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
						 particleList[i]->z>>FIXSHIFT,particleList[i]->color,particleList[i]->size/4,
						 DISPLAY_DRAWME|DISPLAY_CIRCLEPART|DISPLAY_GLOW);
			}
			else if(particleList[i]->type==PART_GHOST)
			{
				bullet_t b;
				b.anim=0;
				b.x=particleList[i]->x;
				b.y=particleList[i]->y;
				b.z=particleList[i]->z;
				b.bright=(char)particleList[i]->color-30;
				b.type=BLT_DRAIN;
				b.facing=0;
				b.frame=243;
				b.timer=1;
				RenderBullet(&b);
			}
			else if(particleList[i]->type==PART_SMOKE)
			{
				bullet_t b;
				b.anim=0;
				b.x=particleList[i]->x;
				b.y=particleList[i]->y;
				b.z=particleList[i]->z;
				b.bright=0;
				b.type=BLT_SMOKE;
				b.facing=0;
				b.frame=321-(particleList[i]->color);
				if(b.frame<314)
					b.frame=314;
				b.timer=1;
				RenderBullet(&b);
			}
			else
				ParticleDraw(particleList[i]->x>>FIXSHIFT,particleList[i]->y>>FIXSHIFT,
							 particleList[i]->z>>FIXSHIFT,particleList[i]->color,particleList[i]->size,
							 DISPLAY_DRAWME|DISPLAY_PARTICLE);
		}
	}
}

void MakeCircleParticle(int x,int y, int z,byte size)
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
			particleList[i]->dz=0;
			particleList[i]->life=size/2;
			particleList[i]->size=size;
			particleList[i]->color=255;
			particleList[i]->type=PART_CIRCLE;
			break;
		}
	}
}

void MakeCircleParticle2(int x,int y, int z,byte size)
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
			particleList[i]->dz=0;
			particleList[i]->life=size/4+2;
			particleList[i]->size=size;
			particleList[i]->color=255;
			particleList[i]->type=PART_CIRCLE;
			break;
		}
	}
}

void MakeRingParticle(int x,int y, int z,byte size,byte cnt)
{
	int i;
	byte ang;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			ang=(byte)Random(256);
			particleList[i]->x=x+Cosine(ang)*size;
			particleList[i]->y=y+Sine(ang)*size;
			particleList[i]->z=z;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=Random(FIXAMT*2);
			particleList[i]->color=191;
			particleList[i]->life=10+(byte)Random(30);
			particleList[i]->size=8*4+(byte)Random(4*4);
			particleList[i]->type=PART_FIRE;
			cnt--;
			if(!cnt)
				break;
		}
	}
}

void MakeColdRingParticle(int x,int y, int z,byte size)
{
	int i;
	byte ang;

	ang=0;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x+Cosine(ang)*size;
			particleList[i]->y=y+Sine(ang)*size;
			particleList[i]->z=z;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=Random(FIXAMT*2);
			particleList[i]->color=31;
			particleList[i]->life=10+Random(30);
			particleList[i]->size=8*4+(byte)Random(4*4);
			particleList[i]->type=PART_COLDFIRE;
			ang+=4;
			if(!ang)
				break;
		}
	}
}

void MakePlagueRingParticle(int x,int y, int z,int size)
{
	int i;
	int ang;

	ang=-(rand()%6);
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x+Cosine(ang)*size;
			particleList[i]->y=y+Sine(ang)*size;
			particleList[i]->z=z;
			particleList[i]->dx=-FIXAMT*2+Random(FIXAMT*4+1);
			particleList[i]->dy=-FIXAMT*2+Random(FIXAMT*4+1);
			particleList[i]->dz=Random(FIXAMT*2);
			particleList[i]->color=31;
			particleList[i]->life=20+Random(10);
			particleList[i]->size=10;
			particleList[i]->type=PART_FLOATY;
			ang+=6;
			if(ang>255)
				break;
		}
	}
}

void GlassShatter(int x,int y,int x2,int y2,int z,byte amt)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=(x+Random(x2-x));
			particleList[i]->y=(y+Random(y2-y));
			particleList[i]->z=z;
			particleList[i]->GoRandom(PART_GLASS,(x+Random(x2-x)),(y+Random(y2-y)),
					Random(10*FIXAMT),20);
			particleList[i]->color=(byte)Random(8)*32+16;
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

	x+=Cosine(ang2)*5;
	y+=Sine(ang2)*5;

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

void MakeItSnow(Map *map)
{
	int i;
	int cx,cy;

	// only 25% of particles may be snowflakes
	if(Random(100)>30 || snowCount>maxParticles/4)
		return;

	GetCamera(&cx,&cy);
	cx-=320;
	cy-=240;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=(Random(640)+cx)<<FIXSHIFT;
			particleList[i]->y=(Random(480)+cy)<<FIXSHIFT;
			particleList[i]->z=(300+Random(300))<<FIXSHIFT;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->size=2;
			particleList[i]->life=50+Random(50);
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

			particleList[i]->x=x-FIXAMT*10+Random(FIXAMT*20);
			particleList[i]->y=y-FIXAMT*10+Random(FIXAMT*20);
			particleList[i]->z=(50+Random(20))<<FIXSHIFT;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->size=2;
			particleList[i]->life=20+Random(30);
			particleList[i]->type=PART_SNOW;
			break;
		}
	}
}

void LightningBolt(int x,int y,int x2,int y2)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			MakeSound(SND_SHOCK,x,y,SND_CUTOFF|SND_RANDOM,300);
			particleList[i]->GoLightning(x,y,x2,y2);
			break;
		}
	}
}

void Burn(int x,int y,int z)
{
	int i;
	byte num;

	num=(byte)Random(8)+1;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x-FIXAMT*2+Random(FIXAMT*4);
			particleList[i]->y=y;
			particleList[i]->z=z-FIXAMT*2+Random(FIXAMT*4);
			particleList[i]->dx=-FIXAMT*2+Random(FIXAMT*4);
			particleList[i]->dy=0;
			particleList[i]->dz=Random(FIXAMT*2);
			particleList[i]->color=191;
			particleList[i]->life=10+Random(30);
			particleList[i]->size=8*4+(byte)Random(4*4);
			particleList[i]->type=PART_FIRE;
			if(--num==0)
				break;
		}
	}
}

void Smoke(int x,int y,int z)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x-FIXAMT*2+Random(FIXAMT*4);
			particleList[i]->y=y;
			particleList[i]->z=z-FIXAMT*2+Random(FIXAMT*4);
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=0;
			particleList[i]->color=7;
			particleList[i]->life=7*4+3;
			particleList[i]->size=1;
			particleList[i]->type=PART_SMOKE;
			return;
		}
	}
}

void ParticleBoom(byte type,int x,int y,int z)
{
	byte amt,ptype,force;
	int i;

	// different types of booms have different numbers they use and particle types to explode and whatnot
	switch(type)
	{
		case PART_GHOST:
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->y=y-FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->z=z;
					particleList[i]->dx=-FIXAMT/2+Random(FIXAMT+1);
					particleList[i]->dy=-FIXAMT/2+Random(FIXAMT+1);
					particleList[i]->dz=0;
					particleList[i]->color=31;
					particleList[i]->life=60;
					particleList[i]->size=1;
					particleList[i]->type=PART_GHOST;
					break;
				}
			}
			return;
			break;
		case PART_WHITEPOP:
			ptype=PART_SNOW2;
			force=4;
			amt=6;
			break;
		case PART_GREENPOP:
			ptype=PART_SLIME;
			force=4;
			amt=3;
			break;
		case PART_REDPOP:
			ptype=PART_HAMMER;
			force=4;
			amt=3;
			break;
		case PART_SPARKPOP:
			ptype=PART_YELLOW;
			force=4;
			amt=6;
			break;
		case PART_DIRT:
			ptype=PART_DIRT;
			force=4;
			amt=10;
			break;
		case PART_FIREPOP:
			ptype=PART_FIRE;
			force=5;
			amt=8;
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->y=y;
					particleList[i]->z=z-FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->dx=-FIXAMT*3+Random(FIXAMT*6+1);
					particleList[i]->dy=-FIXAMT*3+Random(FIXAMT*6+1);
					particleList[i]->dz=Random(FIXAMT*2);
					particleList[i]->color=191;
					particleList[i]->life=10+Random(30);
					particleList[i]->size=8*4+(byte)Random(4*4);
					particleList[i]->type=PART_FIRE;
					if(--amt==0)
						break;
				}
			}
			return;
			break;
		case PART_SNOW:
			ptype=PART_SNOW;
			force=2;
			amt=1;
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*1+Random(FIXAMT*2+1);
					particleList[i]->y=y;
					particleList[i]->z=z-FIXAMT*1+Random(FIXAMT*2+1);
					particleList[i]->dx=0;
					particleList[i]->dy=0;
					particleList[i]->dz=0;
					particleList[i]->size=2;
					particleList[i]->life=20+Random(20);
					particleList[i]->type=PART_SNOW;
					particleList[i]->color=31;
					if(--amt==0)
						break;
				}
			}
			return;
			break;
		case PART_CLOUD:
			ptype=PART_CLOUD;
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					amt=rand()%256;
					force=rand()%((byte)SpecificSkillVal(1,SKILL_ICECUBE));
					particleList[i]->x=x+Cosine(amt)*force;
					particleList[i]->y=y+Sine(amt)*force;
					particleList[i]->z=z;
					particleList[i]->dx=0;
					particleList[i]->dy=0;
					particleList[i]->dz=0;
					particleList[i]->size=1;
					particleList[i]->life=90;
					particleList[i]->type=PART_CLOUD;
					particleList[i]->color=31;
					break;
				}
			}
			return;
			break;
		case PART_FLOATYPOP:
			ptype=PART_FLOATY;
			force=5;
			amt=1;
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*10+Random(FIXAMT*20+1);
					particleList[i]->y=y-FIXAMT*10+Random(FIXAMT*20+1);
					particleList[i]->z=FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->dx=-FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->dy=-FIXAMT*2+Random(FIXAMT*4+1);
					particleList[i]->dz=Random(FIXAMT*2);
					particleList[i]->color=31;
					particleList[i]->life=20+Random(10);
					particleList[i]->size=10;
					particleList[i]->type=ptype;
					if(--amt==0)
						break;
				}
			}
			return;
			break;
		case PART_SMOKE:
		case PART_SMOKESINGLE:
			ptype=PART_SMOKE;
			amt=15-14*(type==PART_SMOKESINGLE);
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*10+Random(FIXAMT*20+1);
					particleList[i]->y=y-FIXAMT*10+Random(FIXAMT*20+1);
					particleList[i]->z=FIXAMT*2+Random(FIXAMT*15+1);
					particleList[i]->dx=-FIXAMT+Random(FIXAMT*2+1);
					particleList[i]->dy=-FIXAMT+Random(FIXAMT*2+1);
					particleList[i]->dz=Random(FIXAMT);
					particleList[i]->color=7;
					particleList[i]->life=7*4+3;
					particleList[i]->size=1;
					particleList[i]->type=ptype;
					if(--amt==0)
						break;
				}
			}
			return;
			break;
		case PART_STEAM:
			ptype=PART_SMOKE;
			amt=4;
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*10+Random(FIXAMT*20+1);
					particleList[i]->y=y-FIXAMT*10+Random(FIXAMT*20+1);
					particleList[i]->z=z-FIXAMT*4+Random(FIXAMT*8+1);
					particleList[i]->dx=-FIXAMT+Random(FIXAMT*2+1);
					particleList[i]->dy=-FIXAMT+Random(FIXAMT*2+1);
					particleList[i]->dz=Random(FIXAMT*2)+FIXAMT*5;
					particleList[i]->color=7;
					particleList[i]->life=7*4+3;
					particleList[i]->size=1;
					particleList[i]->type=ptype;
					if(--amt==0)
						break;
				}
			}
			return;
			break;
		case PART_FIRE:
			ptype=PART_FIRE;
			amt=4;
			force=1;
			for(i=0;i<maxParticles;i++)
			{
				if(!particleList[i]->Alive())
				{
					particleList[i]->x=x-FIXAMT*2+Random(FIXAMT*4);
					particleList[i]->y=y;
					particleList[i]->z=z-FIXAMT*2+Random(FIXAMT*4);
					particleList[i]->dx=-FIXAMT*2+Random(FIXAMT*4);
					particleList[i]->dy=0;
					particleList[i]->dz=Random(FIXAMT*2);
					particleList[i]->color=191;
					particleList[i]->life=10+Random(30);
					particleList[i]->size=8*4+(byte)Random(4*4);
					particleList[i]->type=PART_FIRE;
					if(--amt==0)
						break;
				}
			}
			return;
			break;
		default:
			return;
			break;
	}
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->GoRandom(ptype,x,y,z,force);
			if(!--amt)
				break;
		}
	}
}

void NumberParticle(short num,int x,int y,byte color)
{
	int i;

	if(num>=9999 || num==4999)
		return;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x;
			particleList[i]->y=y;
			particleList[i]->z=FIXAMT*40;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=FIXAMT+FIXAMT;
			particleList[i]->size=2;
			particleList[i]->life=60;
			particleList[i]->type=PART_NUM;
			particleList[i]->num=num;
			particleList[i]->color=color;
			break;
		}
	}
}

void NumberParticle2(short num,int x,int y,byte color,byte type)
{
	int i;

	if(num==9999)
		return;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->x=x;
			particleList[i]->y=y;
			particleList[i]->z=FIXAMT*40;
			particleList[i]->dx=0;
			particleList[i]->dy=0;
			particleList[i]->dz=FIXAMT+FIXAMT;
			if(type==PART_XP)
				particleList[i]->dz=FIXAMT+FIXAMT*2;
			particleList[i]->size=2;
			particleList[i]->life=60;
			particleList[i]->type=type;
			particleList[i]->num=num;
			particleList[i]->color=color;
			break;
		}
	}
}

void FXRing(byte color,int x,int y,int z,byte num,byte force)
{
	int i;
	byte a,aPlus;

	if(num==0)
		return;

	a=rand()%256;
	aPlus=256/num;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i]->Alive())
		{
			particleList[i]->GoExact(PART_FX,x,y,z,a,force);
			particleList[i]->x+=particleList[i]->dx*20;
			particleList[i]->y+=particleList[i]->dy*20;
			particleList[i]->dx+=-(force/4)+Random(force/2+1);
			particleList[i]->dy+=-(force/4)+Random(force/2+1);
			particleList[i]->dz=FIXAMT*5-Random(FIXAMT*3);
			particleList[i]->color=color*32+16;
			particleList[i]->tx=x;
			particleList[i]->life=50;
			particleList[i]->ty=y;
			a+=aPlus;
			if(!--num)
				break;
		}
	}
}
