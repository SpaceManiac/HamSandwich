#include "winpch.h"
#include "particle.h"
#include <algorithm>
#include <memory>
#include "bullet.h"
#include "monster.h"
#include "progress.h"
#include "shop.h"

static std::unique_ptr<Particle[]> particleList;
static int maxParticles;

static int snowCount = 0;

Particle::Particle(void)
{
	this->life=0;
}

Particle::~Particle(void)
{
}

void Particle::SpurtGo(byte type,int x,int y,int z,byte angle,byte force)
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

void Particle::Go(byte type,int x,int y,int z,byte angle,byte force)
{
	byte fforce;

	if(force==0)
		return;

	this->type=type;
	size=2;
	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
		size=20;

	fforce=force/4;

	if(fforce==0)
		fforce=1;
	this->x=x+Random(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->y=y+Random(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->z=z;
	this->dx=Cosine(angle)*Random(fforce);
	this->dy=Sine(angle)*Random(fforce);
	this->dz=Random(fforce*2)<<FIXSHIFT;
	this->life=Random(force)+10;
}

void Particle::GoExact(byte type,int x,int y,int z,byte angle,byte force)
{
	byte fforce;

	if(force==0)
		return;

	this->type=type;
	size=6;
	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
		size=20;

	fforce=force/4;

	if(fforce==0)
		fforce=1;
	this->x=x;
	this->y=y;
	this->z=z;
	this->dx=Cosine(angle)*force;
	this->dy=Sine(angle)*force;
	this->dz=force*FIXAMT*2;
	this->life=25;
	this->color=(Random(7)+1)*32+16;
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
	this->life=4;
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
	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
		size=20;

	// Might be rendered before being updated, so don't leave color uninitialized.
	if (type == PART_WATER)
	{
		color = 96 + std::clamp(life, 8, 31);
	}
}

void Particle::GoRandomColor(byte color,int x,int y,int z,byte force)
{
	if(force==0)
		return;

	this->x=x+Random(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->y=y+Random(32<<FIXSHIFT)-(16<<FIXSHIFT);
	this->z=z;
	this->dx=(Random(force)-force/2)<<FIXSHIFT;
	this->dy=(Random(force)-force/2)<<FIXSHIFT;
	this->dz=Random(force*2)<<FIXSHIFT;
	this->life=Random(force)+20;
	this->size=this->life/2;
	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
		size=20;
	this->color=color*32+16;
	this->type=PART_COLOR;
}

void Particle::Update(Map *map)
{
	byte mapx,mapy;
	byte v;

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
			case PART_RADAR:
				dz+=FIXAMT; // no gravity
				size=life/3;
				break;
			case PART_COUNTESS:
				dz+=FIXAMT; // no gravity
				break;
			case PART_SMOKE:
				dz+=FIXAMT;	// no gravity
				z+=FIXAMT;
				size=(6-life/8);
				dx+=Random(FIXAMT)-FIXAMT/2;
				dy+=Random(FIXAMT)-FIXAMT/2;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				break;
			case PART_BUBBLE:
				dz+=FIXAMT;	// no gravity
				z+=FIXAMT;
				size=(3-life/8);
				dx+=Random(FIXAMT*2)-FIXAMT;
				dy+=Random(FIXAMT*2)-FIXAMT;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				if(life==0)
					MakeSound(SND_BUBBLEPOP,x,y,SND_CUTOFF,1);
				break;
			case PART_MINDCONTROL:
				dz+=FIXAMT;	// no gravity
				z+=FIXAMT;
				size++;
				if(size==3)
					size=0;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				break;
			case PART_STINKY:
				dz+=FIXAMT;	// no gravity
				z+=FIXAMT+FIXAMT/2;
				size=((life/2)&3);
				if(size==3)
					size=1;
				dx+=Random(FIXAMT)-FIXAMT/2;
				dy+=Random(FIXAMT)-FIXAMT/2;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				break;
			case PART_BOOM:
				dz+=FIXAMT;
				z+=FIXAMT;
				size=7-life;
				break;
			case PART_HAMMER:
				color=128+24;
				if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
					size=life/4;
				else
					size=life/8;
				break;
			case PART_COLOR:
				if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
					size=life/4;
				else
					size=life/8;
				break;
			case PART_LUNA:
				dz+=FIXAMT;	// no gravity
				size=life/2;
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
				if(v>31-8)
					v=31-8;
				color=64+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
					size=life/4;
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
				color=96+v;
				if(life>20)
					size=2;
				else if(life<10)
					size=0;
				else
					size=1;
				if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
					size=life/4;
				break;
			case PART_RAIN:
				snowCount++;
				if(z<=0)
				{
					life=0;
					ExplodeParticles2(PART_WATER,x,y,z,1+Random(5),3);
				}
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
				if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
					size=life/4;
				break;
			case PART_SNOW:
				dx+=Random(FIXAMT)-FIXAMT/2;
				dy+=Random(FIXAMT)-FIXAMT/2;
				Dampen(&dx,FIXAMT/8);
				Dampen(&dy,FIXAMT/8);
				dz+=FIXAMT-1;	// not as much gravity as other things
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
			case PART_FX:
				dz+=FIXAMT+FIXAMT/16;
				size=life/6;
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
			case PART_LIGHTNING:
				// get dimmer with each frame
				color/=2;
				break;
		}


		if(x<0 || y<0 || x>=((map->width*TILE_WIDTH)<<FIXSHIFT) || y>=((map->height*TILE_HEIGHT)<<FIXSHIFT))
		{
			life=0;
			return;
		}

		if(type==PART_SMOKE || type==PART_BOOM || type==PART_STINKY || type==PART_BUBBLE || type==PART_MINDCONTROL)
		{
			mapx=(x/TILE_WIDTH)>>FIXSHIFT;
			mapy=(y/TILE_HEIGHT)>>FIXSHIFT;
			color=64+map->GetTile(mapx,mapy)->templight;
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
	particleList = std::make_unique<Particle[]>(max);
	maxParticles = max;
}

void ExitParticles(void)
{
	maxParticles = 0;
	particleList.reset();
}

void UpdateParticles(Map *map)
{
	int i;

	snowCount=0;
	for(i=0;i<maxParticles;i++)
		particleList[i].Update(map);
}

void RenderParticle(int x,int y,byte *scrn,byte color,byte size)
{
	byte c1,c2;
	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

	if(x<0 || x>=wid || y<0 || y>=hei)
		return;

	switch(size)
	{
		case 2:	// big particle
			if(x<2 || x>=wid-2 || y<2 || y>=hei-2)
				return;

			if((color&31)>1)
				c1=color-2;	// only do this if subtracting 2 keeps it in the same color group
			else
				c1=color;
			if((c1&31)>1)
				c2=c1-2;	// only do this if subtracting 4 keeps it in the same color group
			else
				c2=c1;

			scrn+=(x+(y-2)*wid);
			*scrn=c2;
			scrn+=wid-1;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			scrn+=wid-3;
			*scrn++=c2;
			*scrn++=c1;
			*scrn++=color;
			*scrn++=c1;
			*scrn=c2;
			scrn+=wid-3;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			*(scrn+wid-1)=c2;
			break;
		case 1:	// normal particle
			if(x<1 || x>wid-2 || y<1 || y>hei-2)
				return;
			if(color&31)
				c1=color-1;	// only do this if subtracting 1 keeps it in the same color group
			else
				c1=color;
			scrn+=(x+(y-1)*wid);
			*scrn=c1;
			scrn+=wid-1;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			scrn+=wid-1;
			*scrn=c1;
			break;
		case 0:	// tiny particle (1 pixel)
			scrn[x+y*wid]=color;
			break;
	}
}

void HLine(int x,int x2,int y,byte c,byte *scrn)
{
	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

	if(x<0)
		x=0;
	if(x2>=wid)
		x2=wid-1;
	if(x2<0)
		return;
	if(x>=wid)
		return;
	if(y<0)
		return;
	if(y>=hei)
		return;

	scrn+=(x+y*wid);
	memset(scrn,c,x2-x+1);
}

void HLine2(int x,int x2,int y,byte c,byte *scrn)
{
	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

	int i;
	byte b,b3;
	byte c1,c2;

	if(x<0)
		x=0;
	if(x2>=wid)
		x2=wid-1;
	if(x2<0)
		return;
	if(x>=wid)
		return;
	if(y<0)
		return;
	if(y>=hei)
		return;

	c1=(c&31);
	c2=(c&(~31));
	scrn+=(x+y*wid);
	for(i=x;i<=x2;i++)
	{
		b=*scrn;
		b3=(b&31)+c1;
		if(b3>31)
			b3=31;
		*scrn=(b3+c2);
		scrn++;
	}
}

void HLine3(int x,int x2,int y,byte c,byte less,byte *scrn)
{
	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

	int i;
	byte b,b3;
	byte colRange;
	int centerCol;
	int curCol;
	int dCol;
	byte realCol;

	if(x<0)
		x=0;
	if(x2>=wid)
		x2=wid-1;
	if(x2<0)
		return;
	if(x>=wid)
		return;
	if(y<0)
		return;
	if(y>=hei)
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
	scrn+=(x+y*wid);
	for(i=x;i<=x2;i++)
	{

		realCol=(curCol/FIXAMT);
		b=*scrn;
		b3=(b&31)+realCol;
		if(b3>31)
			b3=31;
		if(b3<12)
			b3=12;
		*scrn=(b3+colRange);
		scrn++;

		curCol+=dCol;
		if(curCol>=centerCol)
		{
			dCol=-dCol;
			curCol=centerCol;
		}
	}
}

void RenderCircleParticle(int x,int y,int radius,byte c,byte *scrn)
{
	int d,curX,curY;

	d=3-(2*radius);
	curX=0;
	curY=radius;

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

void RenderLightningParticle(int x1,int y1,int x2,int y2,int range,byte bright,byte *scrn)
{
	static const byte ctab[]={
		8,3,2,3,8,
		3,2,1,2,3,
		2,1,0,1,2,
		3,2,1,2,3,
		8,3,2,3,8,
	};

	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

	int midx,midy;
	const byte *ctptr;
	byte b,brt;

	// base case: draw the (x1,y1) pixel
	if((x1-x2<2 && x1-x2>-2) && (y1-y2<2 && y1-y2>-2))
	{
		if(x1>=0 && x1<wid-5 && y1>=0 && y1<hei-5)
		{
			scrn+=(x1+y1*wid);
			ctptr=&ctab[0];
			for(midy=y1;midy<y1+5;midy++)
			{
				for(midx=x1;midx<x1+5;midx++)
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
				scrn+=wid-5;
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
		if(particleList[i].Alive())
		{
			if(particleList[i].type==PART_SMOKE)
				RenderSmoke(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
							 particleList[i].z>>FIXSHIFT,(char)(particleList[i].color),
							 particleList[i].size);
			else if(particleList[i].type==PART_BUBBLE)
				RenderBubble(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
							 particleList[i].z>>FIXSHIFT,(char)(particleList[i].color),
							 particleList[i].size);
			else if(particleList[i].type==PART_MINDCONTROL)
				RenderMindControl(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
							 particleList[i].z>>FIXSHIFT,(char)(particleList[i].color),
							 particleList[i].size);
			else if(particleList[i].type==PART_BOOM)
				RenderBoom(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
							 particleList[i].z>>FIXSHIFT,(char)(particleList[i].color),
							 particleList[i].size);
			else if(particleList[i].type==PART_LIGHTNING)
				LightningDraw(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
								particleList[i].dx>>FIXSHIFT,particleList[i].dy>>FIXSHIFT,
								particleList[i].color,(char)particleList[i].size);
			else if(particleList[i].type==PART_STINKY)
				RenderStinky(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
							 particleList[i].z>>FIXSHIFT,(char)(particleList[i].color),
							 particleList[i].size);
			else if(particleList[i].type==PART_COUNTESS)
				SprDraw(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
						particleList[i].z>>FIXSHIFT,
						255,particleList[i].life*4-8,
						GetMonsterSprite(MONS_COUNTESS,ANIM_IDLE,0,0),DISPLAY_DRAWME|DISPLAY_GLOW);
			else
				ParticleDraw(particleList[i].x>>FIXSHIFT,particleList[i].y>>FIXSHIFT,
							 particleList[i].z>>FIXSHIFT,particleList[i].color,particleList[i].size,
							 DISPLAY_DRAWME|DISPLAY_PARTICLE);
		}
	}
}

int BlowSmoke(int x,int y,int z,int dz)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=z;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=dz;
			particleList[i].life=6*4-Random(8);
			particleList[i].size=6;
			particleList[i].color=64;
			particleList[i].type=PART_SMOKE;
			return i;
		}
	}
	return -1;
}

void SmokeTile(int x,int y)
{
	int i,j;

	for(i=0;i<3;i++)
	{
		j=BlowSmoke((x*TILE_WIDTH+Random(TILE_WIDTH))*FIXAMT,(y*TILE_HEIGHT+Random(TILE_HEIGHT))*FIXAMT,0,FIXAMT/8);
		if(j!=-1)
		{
			particleList[j].dx=-FIXAMT*2+Random(FIXAMT*4+1);
			particleList[j].dy=-FIXAMT*2+Random(FIXAMT*4+1);
		}
	}
}

void BlowBubble(int x,int y,int z,int dz)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=z;
			particleList[i].dx=-FIXAMT+Random(FIXAMT*2);
			particleList[i].dy=-FIXAMT+Random(FIXAMT*2);
			particleList[i].dz=dz;
			particleList[i].life=3*8+7-Random(8);
			particleList[i].size=0;
			particleList[i].color=64;
			particleList[i].type=PART_BUBBLE;
			break;
		}
	}
}

void MindControlSparks(int x,int y,int z,int dz)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=z;
			particleList[i].dx=-FIXAMT*3+Random(FIXAMT*6);
			particleList[i].dy=-FIXAMT*3+Random(FIXAMT*6);
			particleList[i].dz=dz;
			particleList[i].life=20+Random(20);
			particleList[i].size=0;
			particleList[i].color=64;
			particleList[i].type=PART_MINDCONTROL;
			break;
		}
	}
}
void StinkySteam(int x,int y,int z,int dz)
{
	int i;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=z;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=dz;
			particleList[i].life=6*4-Random(8);
			particleList[i].size=0;
			particleList[i].color=64;
			particleList[i].type=PART_STINKY;
			break;
		}
	}
}

void CountessGlow(int x,int y)
{
	int i;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=0;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=0;
			particleList[i].life=4;
			particleList[i].size=0;
			particleList[i].color=64;
			particleList[i].type=PART_COUNTESS;
			break;
		}
	}
}

void BlowUpGuy(int x,int y,int x2,int y2,int z,byte amt)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=(x+Random(x2-x))<<FIXSHIFT;
			particleList[i].y=(y+Random(y2-y))<<FIXSHIFT;
			particleList[i].z=z;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=0;
			particleList[i].life=7;
			particleList[i].size=0;
			particleList[i].color=64;
			particleList[i].type=PART_BOOM;
			MakeSound(SND_BOMBBOOM,particleList[i].x,particleList[i].y,SND_CUTOFF,1800);
			if(!(--amt))
				break;
		}
	}
}

void GlassShatter(int x,int y,int x2,int y2,int z,byte amt)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=(x+Random(x2-x))<<FIXSHIFT;
			particleList[i].y=(y+Random(y2-y))<<FIXSHIFT;
			particleList[i].z=z;
			particleList[i].GoRandom(PART_GLASS,(x+Random(x2-x))<<FIXSHIFT,(y+Random(y2-y))<<FIXSHIFT,
					Random(10*FIXAMT),20);
			particleList[i].color=Random(8)*32+16;
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

	x+=Cosine(ang2)*12;
	y+=Sine(ang2)*12;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].SpurtGo(type,x,y,z,angle,force);
			if(!--amt)
				break;
		}
	}
}

void ExplodeParticles(byte type,int x,int y,int z,byte force)
{
	int i,amt;

	amt=force;

	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
	{
		amt*=4;
		force*=2;
	}
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoRandom(type,x,y,z,force);
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

	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
	{
		num*=4;
		force*=2;
	}

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoRandom(type,x,y,z,force);
			if(!--num)
				break;
		}
	}
}

void ExplodeParticlesColor(byte color,int x,int y,int z,byte num,byte force)
{
	int i;

	if(num==0)
		return;

	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
	{
		num*=4;
		force*=2;
	}

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoRandomColor(color,x,y,z,force);
			if(!--num)
				break;
		}
	}
}

void ColorRing(byte color,int x,int y,int z,byte num,byte force)
{
	int i;
	byte a,aPlus;

	if(num==0)
		return;

	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
	{
		num*=4;
		force*=2;
	}

	a=0;
	aPlus=256/num;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoExact(PART_COLOR,x,y,z,a,force);
			a+=aPlus;
			if(!--num)
				break;
		}
	}
}

void HealRing(byte color,int x,int y,int z,byte num,byte force)
{
	int i;
	byte a,aPlus;

	if(num==0)
		return;

	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
	{
		num*=4;
		force*=2;
	}

	a=0;
	aPlus=256/num;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoExact(PART_FX,x,y,z,a,force);
			particleList[i].x+=particleList[i].dx*20;
			particleList[i].y+=particleList[i].dy*20;
			particleList[i].dx+=-(force/4)+Random(force/2+1);
			particleList[i].dy+=-(force/4)+Random(force/2+1);
			particleList[i].dz=FIXAMT*5-Random(FIXAMT*3);
			particleList[i].color=color*32+16;
			particleList[i].tx=x;
			particleList[i].life=50;
			particleList[i].ty=y;
			a+=aPlus;
			if(!--num)
				break;
		}
	}
}

void TeamChangeRing(byte color,int x,int y,int z,byte num,byte force)
{
	int i;
	byte a,aPlus;

	if(num==0)
		return;

	if(profile.progress.purchase[modeShopNum[MODE_SPLATTER]]&SIF_ACTIVE)
	{
		num*=4;
		force*=2;
	}

	a=0;
	aPlus=256/num;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoExact(PART_FX,x,y,z,a,force);
			particleList[i].x+=particleList[i].dx*5;
			particleList[i].y+=particleList[i].dy*5;
			particleList[i].dz=0;
			particleList[i].color=color*32+16;
			particleList[i].tx=x;
			particleList[i].life=40;
			particleList[i].ty=y;
			a+=aPlus;
			if(!--num)
				break;
		}
	}
}

void ColorDrop(byte color,int x,int y,int z)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].type=PART_LUNA;
			particleList[i].color=color*32+16;
			particleList[i].size=50;
			particleList[i].life=15;
			particleList[i].dx=-64+Random(129);
			particleList[i].dy=-64+Random(129);
			particleList[i].dz=-64+Random(129);
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=z;
			break;
		}
	}
}

void MakeItSnow(Map *map)
{
	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

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
		if(!particleList[i].Alive())
		{
			particleList[i].x=(Random(wid)+cx)<<FIXSHIFT;
			particleList[i].y=(Random(hei)+cy)<<FIXSHIFT;
			particleList[i].z=(300+Random(300))<<FIXSHIFT;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=0;
			particleList[i].size=2;
			particleList[i].life=50+Random(50);
			particleList[i].type=PART_SNOW;
			particleList[i].color=31;
			break;
		}
	}
}

void MakeItRain(Map *map)
{
	int wid = GetDisplayMGL()->GetWidth(), hei = GetDisplayMGL()->GetHeight();

	int i;
	int cx,cy;

	// only 25% of particles may be rain
	if(Random(100)>30 || snowCount>maxParticles/4)
		return;

	GetCamera(&cx,&cy);
	cx-=320;
	cy-=240;
	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=(Random(wid)+cx)<<FIXSHIFT;
			particleList[i].y=(Random(hei)+cy)<<FIXSHIFT;
			particleList[i].z=(300+Random(300))<<FIXSHIFT;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=-FIXAMT*2;
			particleList[i].size=2;
			particleList[i].life=50+Random(50);
			particleList[i].type=PART_RAIN;
			particleList[i].color=3*32+16;
			break;
		}
	}
}

void SpecialSnow(int x,int y)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{

			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=(10+Random(20))<<FIXSHIFT;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=0;
			particleList[i].size=2;
			particleList[i].life=20+Random(30);
			particleList[i].type=PART_SNOW;
			break;
		}
	}
}

void LightningBolt(int x,int y,int x2,int y2)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].GoLightning(x,y,x2,y2);
			break;
		}
	}
}

void WeathermanWeather(int x,int y)
{
	int i;
	int a;

	// only 25% of particles may be snowflakes
	if(snowCount>maxParticles/4)
		return;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			a=Random(256);

			particleList[i].x=x+Cosine(a)*Random(FIXAMT*60)/FIXAMT;
			particleList[i].y=y+Sine(a)*Random(FIXAMT*50)/FIXAMT;
			particleList[i].z=(300+Random(300))<<FIXSHIFT;
			particleList[i].dx=0;
			particleList[i].dy=0;
			particleList[i].dz=0;
			particleList[i].size=2;
			particleList[i].life=50+Random(50);
			if(Random(2)==0)
			{
				particleList[i].type=PART_SNOW;
				particleList[i].color=31;
			}
			else
			{
				particleList[i].type=PART_RAIN;
				particleList[i].color=3*32+16;
			}
			break;
		}
	}
}

void JackFrostWeather(int x,int y)
{
	int i;
	int a;

	// only 25% of particles may be snowflakes
	if(snowCount>maxParticles/4)
		return;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			a=Random(256);

			particleList[i].x=x+Cosine(a)*Random(FIXAMT*60)/FIXAMT;
			particleList[i].y=y+Sine(a)*Random(FIXAMT*50)/FIXAMT;
			particleList[i].z=(10+Random(100))<<FIXSHIFT;
			particleList[i].dx=-FIXAMT+Random(FIXAMT*2+1);
			particleList[i].dy=-FIXAMT+Random(FIXAMT*2+1);
			particleList[i].dz=0;
			particleList[i].size=2;
			particleList[i].life=50+Random(50);
			particleList[i].type=PART_SNOW;
			particleList[i].color=31;
			break;
		}
	}
}

void TrackParticle(byte color,int x,int y,int tx,int ty)
{
	int i;

	for(i=0;i<maxParticles;i++)
	{
		if(!particleList[i].Alive())
		{
			particleList[i].x=x;
			particleList[i].y=y;
			particleList[i].z=10*FIXAMT;
			particleList[i].dx=(tx-x)/30;
			particleList[i].dy=(ty-y)/30;
			particleList[i].dz=0;
			particleList[i].size=10;
			particleList[i].life=30;
			particleList[i].type=PART_RADAR;
			particleList[i].color=color*32+16;
			break;
		}
	}
}

int CountParticles()
{
	int i, n = 0;
	for (i = 0; i < maxParticles; ++i)
		if (particleList[i].Alive())
			++n;
	return n;
}
