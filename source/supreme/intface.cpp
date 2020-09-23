#include "winpch.h"
#include "intface.h"
#include "player.h"
#include "shop.h"

#define SPR_LIFEMETER	0
#define	SPR_RAGEGAUGE	1
#define SPR_SCORE		2
#define SPR_WEAPONBOX	3
#define SPR_BRAINOMETER	4
#define SPR_HAMMERBOX	5
#define SPR_KEYBOX		6
#define SPR_POWERUP		7
#define SPR_OXYGAUGE	8
#define SPR_ENEMYLIFE	11
#define SPR_WPNNAME		22
#define SPR_CANDLE		SPR_WPNNAME+MAX_WEAPONS+8
#define SPR_LOONYKEY	SPR_CANDLE-1
#define SPR_KEYCH		SPR_CANDLE+1
#define SPR_BRAIN		SPR_KEYCH+4
#define SPR_RAGE		SPR_BRAIN+1
#define SPR_COINBOX		SPR_RAGE+5
#define SPR_VARBAR		SPR_COINBOX+2

#define SPR_KEYS		SPR_WPNNAME+MAX_WEAPONS-1
#define SPR_HAMMER		SPR_KEYS+4
#define SPR_KEYS2		SPR_VARBAR+1
#define SPR_CANDLOMETER	SPR_KEYS2+2
#define SPR_TIMER		SPR_CANDLOMETER+1
#define SPR_VARNUM		SPR_TIMER+1

#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		12
#define SPR_NUMBERS_NEW	SPR_TIMER+2
#define SPR_NUMERALS	SPR_NUMBERS_NEW+10
#define SPR_BOSSMETER	SPR_NUMERALS+5
#define SPR_WEAPONS		13
#define SPR_KEYRING		29

#define KEYRINGX		40
#define KEYRINGY		38

sprite_set_t *intfaceSpr;
static byte curLife=0;
static byte curBrains=0;
static byte monsAlive=0;
static int curMonsLife,curBossLife;
static int comboY,curCombo;

// these are for the enemy life meter
char monsName[32];
char bossName[32];
int monsHP, bossHP;
word monsTimer;
Guy *monster;

#define INTF_POWERUP 0
#define INTF_KEYS	1
#define INTF_VARBAR 2
#define INTF_RAGE	3
#define INTF_OXYGEN 4
#define INTF_LIFE	5
#define INTF_BRAINS 6
#define INTF_CANDLES 7
#define INTF_WEAPON 8
#define INTF_SCORE	9
#define INTF_HAMMERS 10
#define INTF_ENEMY	11
#define INTF_COINS	12
#define INTF_TIMER 	13
#define INTF_VARNUM 14
#define INTF_BOSS	15
#define NUM_INTF	16

#define IV_NONE			0
#define IV_BIGMETER		1
#define IV_SMALLMETER	2
#define IV_NUMBER		3
#define IV_VERTMETER	4
#define IV_ICONS		5
#define IV_KEYS			6
#define IV_DIAL			7	// the oxygen dial
#define IV_EVILMETER 	8
#define IV_BIGNUMBER 	9

typedef struct intface_t
{
	int  x,y,tx,ty;
	byte spr;
	byte valueType;
	byte valueLength;
	int  vOffX,vOffY;
	int  value,vDesired;
	byte otherVal;
} intface_t;

intface_t defaultSetup[NUM_INTF]={
	{0,-100,0,36,	// power ups
	 SPR_POWERUP,
	 IV_VERTMETER,64,
	 2,66,
	 0,0,
	 0},
	{304,-50,304,0,	// keys
	 SPR_KEYBOX,
	 IV_KEYS,0,
	 0,1,
	 0,0,
	 0},
	{120,-50,120,25,	// varbar
	 SPR_VARBAR,
	 IV_SMALLMETER,89,
	 29,3,
	 0,0,
	 0},
	{0,-50,0,25,	// rage meter
	 SPR_RAGEGAUGE,
	 IV_SMALLMETER,89,
	 29,3,
	 0,100,
	 0},
	{2,-50,2,48,	// oxygen
	 SPR_OXYGAUGE,
	 IV_DIAL,192,
	 14,12,
	 0,128,
	 0},
	{0,0,0,0,		// life meter
	 SPR_LIFEMETER,
	 IV_BIGMETER,198,
	 40,3,
	 0,198,
	 0},
	{587,-102,587,24,	// brains
	 SPR_BRAINOMETER,
	 IV_VERTMETER,29,
	 4,2,
	 0,50,
	 0},
	{626,-102,626,25+4,	// candle
	 SPR_CANDLOMETER,
	 IV_VERTMETER,29,
	 4,2,
	 0,50,
	 0},
	{387,-70,387,-1,	// weapon
	 SPR_WEAPONBOX,
	 IV_SMALLMETER,125,
	 2,15,
	 0,10,
	 1},
	{640,-50,640,0,	// score
	 SPR_SCORE,
	 IV_BIGNUMBER,6,
	 -108,2,
	 0,1000,
	 0},
	{242,-37,242,0,		// hammers
	 SPR_HAMMERBOX,
	 IV_ICONS,0,
	 0,0,
	 0,0,
	 0},
	{0,520,0,479,		// enemy life
	 SPR_ENEMYLIFE,
	 IV_EVILMETER,196,
	 2,-16,
	 0,196,
	 0},
	{639,520,639,479,	// coins
	 SPR_COINBOX,
	 IV_NUMBER,2,
	 -18,-14,
	 0,0,
	 0},
	{320,510,320,480,	// timer
	 SPR_TIMER,
	 IV_BIGNUMBER,3,
	 -25,-23,
	 0,0,
	 0},
	{640,510,320,480,	// var 0
	 SPR_VARNUM,
	 IV_BIGNUMBER,4,
	 -73,-22,
	 0,0,
	 0},
	{0,481,0,0,		// boss meter
	 SPR_BOSSMETER,
	 IV_BIGMETER,198,
	 40,3,
	 0,198,
	 0},
};

static byte intfFlip;

intface_t intf[NUM_INTF];

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("expgraphics/intface.jsp");
	memcpy(intf,defaultSetup,sizeof(intface_t)*NUM_INTF);
}

void ExitInterface(void)
{
	delete intfaceSpr;
}

void ResetInterface(void)
{
	comboY=-22;
	curCombo=0;
	intfFlip=0;
	curLife=0;
	monsTimer=0;
	curBrains=0;
	memcpy(intf,defaultSetup,sizeof(intface_t)*NUM_INTF);
}

void DrawLifeMeter(int x,int y,byte amt)
{
	byte c;

	if(amt==0)
		return;
	amt--;
	if(amt>198)
		amt=198;

	if(amt>64+16)
		c=44;		// in the green
	else if(amt>32)
		c=174;		// in the yellow
	else
		c=140;		// in the red!!

	DrawFillBox(x,y+1,x,y+12,c-6);
	DrawFillBox(x+amt,y+1,x+amt,y+12,c-6);
	if(amt>1)
	{
		DrawFillBox(x+1,y+1,x+1,y+12,c-1);
		DrawFillBox(x+amt-1,y+1,x+amt-1,y+12,c-1);
		if(amt>3)
		{
			// here's the short parts on top and bottom
			DrawFillBox(x+2,y,x+(amt-2),y,c-3);
			DrawFillBox(x+2,y+13,x+(amt-2),y+13,c-3);
			// and here's the normal part
			DrawFillBox(x+2,y+1,x+amt-2,y+1,c);
			DrawFillBox(x+2,y+12,x+amt-2,y+12,c);
			DrawFillBox(x+2,y+2,x+amt-2,y+2,c+8);
			DrawFillBox(x+2,y+3,x+amt-2,y+11,c+4);
		}
	}
}

void DrawRageMeter(int x,int y,byte enuf,byte amt)
{
	byte c;
	static byte blink=0;

	blink=1-blink;

	if(amt==0)
		return;
	amt--;

	if(enuf)
	{
		if(blink)
			c=48;
		else
			c=44;
	}
	else
	{
		if(blink)
			c=140;
		else
			c=134;
	}

	if(amt>0)
	{
		DrawFillBox(x,y,x+amt,y+1,c);
	}
}

void DrawOxygenMeter(int x,int y,byte amt)
{
	byte c;

	if(amt==0)
		return;
	amt--;

	if(amt>64)
		c=32*1+16;
	else if(amt>32)
		c=32*5+16;
	else
		c=32*4+16;

	if(amt>0)
	{
		DrawFillBox(x,y,x+amt,y+1,c);
	}
}

void DrawScore(int x,int y,int score,MGLDraw *mgl)
{
	int i,j,n;

	if(score<0)		// I don't think you can have a negative score
		score=0;

	j=100000000;			// 100 million
	for(i=0;i<9;i++)
	{
		n=(score/j)%10;
		intfaceSpr->GetSprite(n+SPR_NUMBERS)->Draw(x,y,mgl);
		j/=10;
		x+=23;
	}
}

void DrawHammerSpeed(int x,int y,byte spd)
{
	byte b;

	if(spd>=32)
		return;

	b=(32-spd)*14/32;
	b--;

	DrawFillBox(x+1,y+13-b,x+1,y+13,143);
	DrawFillBox(x+2,y+13-b,x+2,y+13,141);
	if(b>2)
	{
		DrawFillBox(x,y+12-(b-2),x,y+12,138);
		DrawFillBox(x+3,y+12-(b-2),x+3,y+12,138);
	}
}

void DrawLitGauge(int x,int y,byte c)
{
	DrawFillBox(x+1,y,x+1,y+13,c+2);
	DrawFillBox(x+2,y,x+2,y+13,c);
	DrawFillBox(x,y+1,x,y+12,c-4);
	DrawFillBox(x+3,y+1,x+3,y+12,c-4);
}

void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive)
{
	curMonsLife=formerLife;
	monsHP=life;
	strcpy(monsName,name);
	monsTimer=90;	// 3 seconds
	monsAlive=alive;
	intf[INTF_ENEMY].ty=479;
}

void SetBossBar(char *name)
{
	strcpy(bossName,name);
}

void RenderRage(byte size,MGLDraw *mgl)
{
	intfaceSpr->GetSprite(SPR_RAGE+size)->Draw(320,240,mgl);
}

void DrawBigMeter(int x,int y,int value,int length,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	if(value>length)
		value=length;

	c=32;

	if(value<length*2/3)
		c=162;
	if(value<length/4)
		c=128;

	mgl->FillBox(x,y,x+value-1,y,c+10);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+10);
	mgl->FillBox(x,y+2,x+value-1,y+2,c+10);
	mgl->FillBox(x,y+3,x+value-1,y+3,c+12);
	mgl->FillBox(x,y+4,x+value-1,y+4,c+12);
	mgl->FillBox(x,y+5,x+value-1,y+5,c+12);
	mgl->FillBox(x,y+6,x+value-1,y+6,c+15);
	mgl->FillBox(x,y+7,x+value-1,y+7,c+15);
	mgl->FillBox(x,y+8,x+value-1,y+8,c+26);
	mgl->FillBox(x,y+9,x+value-1,y+9,c+26);
	mgl->FillBox(x,y+10,x+value-1,y+10,c+15);
	mgl->FillBox(x,y+11,x+value-1,y+11,c+15);
	mgl->FillBox(x,y+12,x+value-1,y+12,c+12);
	mgl->FillBox(x,y+13,x+value-1,y+13,c+12);
	mgl->FillBox(x,y+14,x+value-1,y+14,c+12);
	mgl->FillBox(x,y+15,x+value-1,y+15,c+10);
	mgl->FillBox(x,y+16,x+value-1,y+16,c+10);
	mgl->FillBox(x,y+17,x+value-1,y+17,c+10);
}

void DrawEvilMeter(int x,int y,int value,int length,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	if(value>length)
		value=length;

	c=32;

	if(value<length*2/3)
		c=162;
	if(value<length/4)
		c=128;

	mgl->FillBox(x,y,x+value-1,y,c+6);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+8);
	mgl->FillBox(x,y+2,x+value-1,y+4,c+11);
	mgl->FillBox(x,y+5,x+value-1,y+5,c+15);
	mgl->FillBox(x,y+6,x+value-1,y+11,c+11);
	mgl->FillBox(x,y+12,x+value-1,y+12,c+8);
	mgl->FillBox(x,y+13,x+value-1,y+13,c+6);
}

void DrawSmallMeter(int x,int y,int value,byte red,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	c=32;

	if(red==1)
		c=128;
	else if(red==2)
		c=36;
	else if(red==3)
		c=228;
	else if(red>=16)
		c=4+(red-16)*32;

	mgl->FillBox(x,y,x+value-1,y,c+8);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+11);
	mgl->FillBox(x,y+2,x+value-1,y+2,c+15);
	mgl->FillBox(x,y+3,x+value-1,y+3,c+18);
	mgl->FillBox(x,y+4,x+value-1,y+4,c+15);
	mgl->FillBox(x,y+5,x+value-1,y+5,c+11);
	mgl->FillBox(x,y+6,x+value-1,y+6,c+8);
}

void DrawVertMeter(int x,int y,int value,int height,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	c=32;

	mgl->FillBox(x,y+height-(value-1),x,y+height,c+7);
	mgl->FillBox(x+1,y+height-(value-1),x+1,y+height,c+12);
	mgl->FillBox(x+2,y+height-(value-1),x+2,y+height,c+15);
	mgl->FillBox(x+3,y+height-(value-1),x+3,y+height,c+12);
	mgl->FillBox(x+4,y+height-(value-1),x+4,y+height,c+7);
}

void DrawNumber(int x,int y,int value,byte length,MGLDraw *mgl)
{
	int i;
	char s[8];

	if(value<0)		// I don't think you can have a negative score
		value=0;
	if(value>999999)
		value=999999;

	sprintf(s,"%d",value);

	if(strlen(s)<length)
	{
		x+=9*(length-strlen(s));
		length=strlen(s);
	}
	for(i=0;i<length;i++)
	{
		intfaceSpr->GetSprite(s[i]-'0'+SPR_NUMBERS)->Draw(x,y,mgl);
		x+=9;
	}
}

void DrawNumberL(int x,int y,int value,byte length,MGLDraw *mgl)
{
	int i;
	char s[8];

	if(value<0)		// I don't think you can have a negative score
		value=0;
	if(value>999999)
		value=999999;

	sprintf(s,"%d",value);

	if(strlen(s)<length)
	{
		x+=9*(length+strlen(s));
		length=strlen(s);
	}
	for(i=0;i<length;i++)
	{
		intfaceSpr->GetSprite(s[i]-'0'+SPR_NUMBERS)->Draw(x,y,mgl);
		x+=9;
	}
}

void DrawBigNumber(int x,int y,int value,byte length,MGLDraw *mgl)
{
	int i;
	char s[8];

	if(value<0)		// I don't think you can have a negative score
		value=0;
	if(value>999999)
		value=999999;

	sprintf(s,"%d",value);

	if(strlen(s)<length)
	{
		x+=18*(length-strlen(s));
		length=strlen(s);
	}
	for(i=0;i<length;i++)
	{
		intfaceSpr->GetSprite(s[i]-'0'+SPR_NUMBERS_NEW)->Draw(x,y,mgl);
		x+=18;
	}
}

void DrawDial(int x,int y,int value,byte length,MGLDraw *mgl)
{
	int ang;
	int tx,ty,i;

	ang=(64+256-value)&255;

	if(value==0)
		intfaceSpr->GetSprite(8)->DrawOffColor(x-14,y-12,mgl,1,4,intfFlip*8);
	if(value<192/4)
		intfaceSpr->GetSprite(10)->Draw(x-14,y-12,mgl);
	else if(value<192/2)
		intfaceSpr->GetSprite(9)->Draw(x-14,y-12,mgl);

	x*=FIXAMT;
	y*=FIXAMT;
	for(i=0;i<8;i++)
	{
		tx=(x+Cosine(ang)*i)/FIXAMT;
		ty=(y-Sine(ang)*i)/FIXAMT;
		mgl->FillBox(tx,ty,tx,ty,31);
	}
}

void DrawPULightning(int x,int y,int height,byte color,MGLDraw *mgl)
{
	byte *scrn;
	int pitch;
	int i,x2,b,db;

	pitch=mgl->GetWidth();
	scrn=mgl->GetScreen();
	scrn+=x+y*pitch;

	x2=Random(4);
	b=Random(8);
	if(Random(2))
		db=1;
	else
		db=-1;
	for(i=0;i<height;i++)
	{
		if(x+x2>=0 && x+x2<639 && y>=0 && y<480)
		{
			scrn[x2]=color+b;
			scrn[x2+1]=color+b;
		}
		b+=db;
		if(b>7)
		{
			b=7;
			db=-1;
		}
		if(b<0)
		{
			b=0;
			db=1;
		}
		switch(Random(5))
		{
			case 1:
				x2++;
				if(x2>5)
					x2=5;
				break;
			case 2:
				x2--;
				if(x2<0)
					x2=0;
				break;
		}
		scrn-=pitch;
		y--;
	}
}

void DrawPowerupBar(int x,int y,MGLDraw *mgl)
{
	byte height[11],th;
	byte color[11],tc;
	int i,swap;

	height[0]=player.shield*64/240;
	color[0]=32*3+16;	// blue for shield
	height[1]=player.speed*64/255;
	color[1]=32*7+20;	// light aqua for speed
	height[2]=player.invisibility*64/255;
	color[2]=32*6+14;	// purple for invis
	height[3]=player.ammoCrate*64/255;
	color[3]=32*(player.clock/16&7)+20;	// rainbow for ammo!
	if(goodguy)
		height[4]=goodguy->poison*64/255;
	else
		height[4]=0;
	color[4]=32*1+16;	// green for poison
	height[5]=player.cheesePower*64/255;
	color[5]=32*5+16;	// yellow for cheese
	height[6]=player.garlic*64/255;
	color[6]=32*0+20;	// light grey for garlic
	if(goodguy)
	{
		height[7]=goodguy->ignited*64/255;
		height[8]=goodguy->weak*64/255;
		height[9]=goodguy->strong*64/255;
		height[10]=goodguy->frozen*64/255;
	}
	else
	{
		height[7]=0;
		height[8]=0;
		height[9]=0;
		height[10]=0;
	}
	color[7]=32*4+12;	// dark red for burning
	color[8]=32*4+20;	// light red for weakness
	color[9]=32*0+12;	// dark grey for steelskin
	color[10]=32*7+16;	// aqua for frozen
	//dark red for weakness (lower defense)
	//dark grey for strength (higher defense)

	swap=1;
	while(swap)
	{
		swap=0;
		for(i=0;i<10;i++)
		{
			if(height[i]<height[i+1])
			{
				th=height[i];
				tc=color[i];
				height[i]=height[i+1];
				color[i]=color[i+1];
				height[i+1]=th;
				color[i+1]=tc;
				swap=1;
			}
		}
	}

	for(i=0;i<10;i++)
	{
		if(height[i]>0)
		{
			DrawPULightning(x,y,height[i],color[i],mgl);
		}
	}
}

void DrawKeys(int x,int y,MGLDraw *mgl)
{
	int i;

	for(i=0;i<player.keys[0];i++)
		intfaceSpr->GetSprite(SPR_KEYS)->Draw(x+i*11,y+16,mgl);

	if(player.keys[1])
		intfaceSpr->GetSprite(SPR_KEYS+1)->Draw(x,y,mgl);
	if(player.keys[2])
		intfaceSpr->GetSprite(SPR_KEYS+2)->Draw(x+11,y,mgl);
	if(player.keys[3])
		intfaceSpr->GetSprite(SPR_KEYS+3)->Draw(x+22,y,mgl);
	if(player.keys[4])
		intfaceSpr->GetSprite(SPR_KEYS2)->Draw(x+33,y,mgl);
	if(player.keys[5])
		intfaceSpr->GetSprite(SPR_KEYS2+1)->Draw(x+44,y,mgl);
}

void DrawHammers(int x,int y,MGLDraw *mgl)
{
	int i,p;

	if (player.hammers>0)
	{
		intfaceSpr->GetSprite(SPR_HAMMER)->Draw(x+2,y+2,mgl);
		intfaceSpr->GetSprite(SPR_NUMERALS+player.hammers-1)->Draw(x+20,y+3,mgl);
	}
	if (player.hamSpeed<20)
	{
		p=floor((20-player.hamSpeed)/4);
		intfaceSpr->GetSprite(SPR_HAMMER+1)->Draw(x+3,y+19,mgl);
		intfaceSpr->GetSprite(SPR_NUMERALS+p-1)->Draw(x+20,y+19,mgl);
	}


	if(player.hammerFlags&HMR_REFLECT)
		intfaceSpr->GetSprite(SPR_HAMMER+2)->Draw(x+39,y+20,mgl);
	if(player.hammerFlags&HMR_REVERSE)
		intfaceSpr->GetSprite(SPR_HAMMER+3)->Draw(x+39,y+2,mgl);
}

void UpdateInterface(Map *map)
{
	int i,j;

	if(player.comboClock && player.combo>1 && comboY<2)
		comboY+=4;
	if(!player.comboClock && comboY>-36)
		comboY-=4;
	if(player.comboClock && player.combo>1)
		curCombo=player.combo;

	if(player.shield || player.garlic || player.speed || player.invisibility || player.ammoCrate || (goodguy && goodguy->poison) || (goodguy && goodguy->ignited) || (goodguy && goodguy->weak) || (goodguy && goodguy->strong) || player.cheesePower || (goodguy && goodguy->frozen))
	{
		intf[INTF_POWERUP].tx=0;
		if(player.rage>0)
			intf[INTF_POWERUP].ty=36;
		else
			intf[INTF_POWERUP].ty=25;
	}
	else
	{
		intf[INTF_POWERUP].tx=-10;
		intf[INTF_POWERUP].ty=25;
	}

	// set up the correct destinations for each piece
	if(player.keys[0]+player.keys[1]+player.keys[2]+player.keys[3]+player.keys[4]+player.keys[5])
	{
		intf[INTF_KEYS].tx=304;
		intf[INTF_KEYS].ty=0; // used to be 18, upped by 7 for varbar
	}
	else
	{
		intf[INTF_KEYS].tx=304;
		intf[INTF_KEYS].ty=-36;
	}
	if(player.varbarMax>0)
	{
		intf[INTF_VARBAR].tx=120;
		intf[INTF_VARBAR].ty=23;
	}
	else
	{
		intf[INTF_VARBAR].tx=120;
		intf[INTF_VARBAR].ty=0;
	}
	if(player.rage>0 && player.ability[ABIL_RAGE])
	{
		intf[INTF_RAGE].tx=0;
		intf[INTF_RAGE].ty=23;
	}
	else
	{
		intf[INTF_RAGE].tx=0;
		intf[INTF_RAGE].ty=0;
	}

	if(map->flags&(MAP_UNDERWATER|MAP_OXYGEN))
	{
		intf[INTF_OXYGEN].tx=2;
		if (player.rage>0 && player.ability[ABIL_RAGE])
			intf[INTF_OXYGEN].ty=36;
		else
			intf[INTF_OXYGEN].ty=24;
	}
	else
	{
		intf[INTF_OXYGEN].tx=2;
		intf[INTF_OXYGEN].ty=-25;
	}

	intf[INTF_LIFE].tx=0;
	intf[INTF_LIFE].ty=0;

	if(player.curBossHP>0)
	{
		intf[INTF_BOSS].tx=0;
		intf[INTF_BOSS].ty=480-25;
	}
	else
	{
		intf[INTF_BOSS].tx=0;
		intf[INTF_BOSS].ty=481;
		player.maxBossHP = 0;
	}

	if(player.hammers==0 && (player.hammerFlags&(HMR_REVERSE|HMR_REFLECT))==0 && player.hamSpeed==20)
	{
		intf[INTF_HAMMERS].tx=243;
		intf[INTF_HAMMERS].ty=-36;
	}
	else
	{
		intf[INTF_HAMMERS].tx=243;
		intf[INTF_HAMMERS].ty=0;
	}

	// now do the same stuff for the other side of the screen
	if(player.brains>=map->numBrains)
	{
		intf[INTF_BRAINS].tx=587;
		intf[INTF_BRAINS].ty=-36;
	}
	else
	{
		intf[INTF_BRAINS].tx=587;
		intf[INTF_BRAINS].ty=25;
	}
	if(player.candles>=map->numCandles)
	{
		intf[INTF_CANDLES].tx=626;
		intf[INTF_CANDLES].ty=-36;
	}
	else
	{
		intf[INTF_CANDLES].tx=626;
		intf[INTF_CANDLES].ty=25;
	}
	// now do the same stuff for the other side of the screen
	/*if (profile.candlometer)
	{	
		if(player.candles>=map->numCandles)
		{
			intf[INTF_CANDLES].tx=640+30;
			intf[INTF_CANDLES].ty=35;
		}
		else
		{
			intf[INTF_CANDLES].tx=639;
			intf[INTF_CANDLES].ty=35;
		}
	}
	else
	{
		intf[INTF_CANDLES].tx=640+30;
		intf[INTF_CANDLES].ty=35;
	}*/
	if(player.weapon)
	{
		intf[INTF_WEAPON].tx=387;
		intf[INTF_WEAPON].ty=0;
	}
	else
	{
		intf[INTF_WEAPON].tx=387;
		intf[INTF_WEAPON].ty=-25;
	}
	if(player.coins)
	{
		if(curMap && curMap->flags&MAP_SHOWV0&&(player.var[0]>0))
			intf[INTF_COINS].ty=479-25;
		else
			intf[INTF_COINS].ty=479;
		intf[INTF_COINS].tx=639;
	}
	else
	{
		intf[INTF_COINS].tx=639;
		intf[INTF_COINS].ty=480+20;
	}
	// timer time!
	if(curMap && curMap->flags&MAP_TIMER&&player.timer>0)
	{
		intf[INTF_TIMER].tx=320;
		intf[INTF_TIMER].ty=480;
	}
	else
	{
		intf[INTF_TIMER].tx=320;
		intf[INTF_TIMER].ty=535;
	}
	if(curMap && curMap->flags&MAP_SHOWV0&&(player.var[0]>0))
	{
		intf[INTF_VARNUM].tx=640;
		intf[INTF_VARNUM].ty=480;
	}
	else
	{
		intf[INTF_VARNUM].tx=639;
		intf[INTF_VARNUM].ty=535;
	}
	
	
	intfFlip=1-intfFlip;
	for(i=0;i<NUM_INTF;i++)
	{
		if (player.clock < 10)
			intf[i].value = intf[i].vDesired;
		switch(i)	// whatever is unique to each
		{
			case INTF_LIFE:
				intf[i].vDesired=player.life*198/goodguy->maxHP;
				if(intf[i].value<intf[i].vDesired-10)
					intf[i].value+=3;
				if(intf[i].value>intf[i].vDesired+10)
					intf[i].value-=3;
				if(intf[i].value>198)
					intf[i].value=198;
				break;
			case INTF_RAGE:
				intf[i].vDesired=player.rage*intf[i].valueLength/(127*256);
				break;
			case INTF_VARBAR:
				if (player.varbarMax>0)
					if (player.varbar>player.varbarMax)
						intf[i].vDesired=intf[i].valueLength;
					else
						intf[i].vDesired=player.varbar*intf[i].valueLength/player.varbarMax;
				else
					intf[i].vDesired=0;
				break;
			case INTF_OXYGEN:
				intf[i].vDesired=player.oxygen*intf[i].valueLength/(127*256);
				break;
			case INTF_SCORE:
				intf[i].vDesired=player.score;
				break;
			case INTF_COINS:
				intf[i].vDesired=player.coins;
				break;
			case INTF_TIMER: //ah its time for time
				intf[i].vDesired=player.timer;
				if(player.clock < 30)
					if (player.clock % 30 == 0)
					player.timer--;
				else if(player.timer < 0)
					player.timer = 0;
				else if(player.timer > 999)
					player.timer = 999;
				break;
			case INTF_VARNUM: //ah its time for SHOWING VARIABLE 0
				intf[i].vDesired=player.var[0];
				break;
			case INTF_WEAPON:
				if(player.weapon>0)
					intf[i].vDesired=player.ammo*intf[i].valueLength/WeaponMaxAmmo(player.weapon);
				else
					intf[i].vDesired=0;
				break;
			case INTF_BRAINS:
				int b;

				b=map->numBrains-player.brains;
				if(b<0)
					b=0;
				if(map->numBrains>0)
					intf[i].vDesired=b*intf[i].valueLength/map->numBrains;
				else
					intf[i].vDesired=0;
				break;
			case INTF_CANDLES:
				int c;

				c=map->numCandles-player.candles;
				if(c<0)
					c=0;
				if(map->numCandles>0)
					intf[i].vDesired=c*intf[i].valueLength/map->numCandles;
				else
					intf[i].vDesired=0;
				break;
			case INTF_ENEMY:
				if(monsTimer)
				{
					monsTimer--;
					if(curMonsLife<monsHP)
					{
						curMonsLife+=4;
						if(curMonsLife>monsHP)
							curMonsLife=monsHP;
					}
					if(curMonsLife>monsHP)
					{
						curMonsLife-=4;
						if(curMonsLife<monsHP)
							curMonsLife=monsHP;
					}
					intf[i].vDesired=curMonsLife*intf[i].valueLength/128;
					intf[i].value=curMonsLife*intf[i].valueLength/128;
					if(player.maxBossHP!=0)
					intf[i].ty=455;
					else
					intf[i].ty=479;
				}
				else
				{
					intf[i].vDesired=0;
					intf[i].ty=479+30;
				}
				break;
			case INTF_BOSS:
				if(player.maxBossHP!=0)
					intf[i].vDesired=player.curBossHP*intf[i].valueLength/player.maxBossHP;
				else
					intf[i].vDesired=0;
				if(intf[i].value<intf[i].vDesired-10)
					intf[i].value+=3;
				if(intf[i].value>intf[i].vDesired+10)
					intf[i].value-=3;
				if(intf[i].value>198)
					intf[i].value=198;
				break;
		}
		if(intf[i].value<intf[i].vDesired)
			intf[i].value++;
		if(intf[i].value<intf[i].vDesired-100)
			intf[i].value+=20;
		if(intf[i].value<intf[i].vDesired-1000)
			intf[i].value+=200;
		if(intf[i].value>intf[i].vDesired)
			intf[i].value--;
		if(intf[i].value>intf[i].vDesired+100)
			intf[i].value-=20;
		if(intf[i].value>intf[i].vDesired+1000)
			intf[i].value-=200;

		for(j=0;j<2;j++)
		{
			if(intf[i].x>intf[i].tx)
				intf[i].x--;
			if(intf[i].x<intf[i].tx)
				intf[i].x++;
			if(intf[i].y>intf[i].ty)
				intf[i].y--;
			if(intf[i].y<intf[i].ty)
				intf[i].y++;
		}
	}
}

void RenderInterface(MGLDraw *mgl)
{
	int i;
	char combo[16];

	if(shopping)	// special mutant interface when shopping
	{
		InstaRenderItem(639-TILE_WIDTH/2,479-8,ITM_COIN,0,mgl);
		sprintf(combo,"%lu",profile.progress.totalCoins-profile.progress.coinsSpent);
		Print(639-TILE_WIDTH-GetStrLength(combo,2)-1,479-18,combo,-32,2);
		Print(639-TILE_WIDTH-GetStrLength(combo,2)+1,479-18,combo,-32,2);
		Print(639-TILE_WIDTH-GetStrLength(combo,2),479-18+1,combo,-32,2);
		Print(639-TILE_WIDTH-GetStrLength(combo,2),479-18-1,combo,-32,2);
		PrintGlow(639-TILE_WIDTH-GetStrLength(combo,2),479-18,combo,0,2);

		InstaRenderItem(639-TILE_WIDTH/2,479-38,ITM_LOONYKEY,0,mgl);
		sprintf(combo,"%lu",profile.progress.loonyKeys-profile.progress.loonyKeysUsed);
		Print(639-TILE_WIDTH-GetStrLength(combo,2)-1,479-38,combo,-32,2);
		Print(639-TILE_WIDTH-GetStrLength(combo,2)+1,479-38,combo,-32,2);
		Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38+1,combo,-32,2);
		Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38-1,combo,-32,2);
		PrintGlow(639-TILE_WIDTH-GetStrLength(combo,2),479-38,combo,0,2);
		return;
	}

	if(curMap && curMap->flags&MAP_STEALTH)
		intfaceSpr->GetSprite(SPR_COINBOX+1)->Draw(625,460,mgl);

	for(i=0;i<NUM_INTF;i++)
	{
		intfaceSpr->GetSprite(intf[i].spr)->Draw(intf[i].x,intf[i].y,mgl);

		switch(intf[i].valueType)
		{
			case IV_BIGMETER:
				DrawBigMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
			case IV_EVILMETER:
				DrawEvilMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				if(monsAlive || intfFlip)
					PrintGlow(intf[i].x+intf[i].vOffX+2,intf[i].y+intf[i].vOffY-1,monsName,0,2);
				break;
			case IV_SMALLMETER:
				if(i!=INTF_RAGE || player.rage/256>=player.life)
				{
					if(i==INTF_RAGE && intfFlip==0)
						DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,2,mgl);
					else if (i==INTF_VARBAR)
						DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,16+player.varbarColor,mgl);
					else
						DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,0,mgl);
				}
				else
					DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,1,mgl);
				break;
			case IV_NUMBER:
				DrawNumber(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
			case IV_BIGNUMBER:
				DrawBigNumber(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
			case IV_VERTMETER:
				if(i==INTF_POWERUP)
					DrawPowerupBar(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,mgl);
				else
					DrawVertMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
			case IV_KEYS:
				DrawKeys(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,mgl);
				break;
			case IV_ICONS:
				DrawHammers(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,mgl);
				break;
			case IV_DIAL:
				DrawDial(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
		}
		if(i==INTF_LIFE)
		{
			DrawNumber(intf[i].x+2,intf[i].y+9,player.life,4,mgl);
		}
		if(i==INTF_BOSS)
		{
			DrawNumber(intf[i].x+2,intf[i].y+9,player.curBossHP,4,mgl);
			if(bossName)
			{
				PrintGlow(intf[i].x+intf[i].vOffX+2,intf[i].y+intf[i].vOffY-1,bossName,0,2);
			}
		}
		if(i==INTF_WEAPON)
		{
			if(player.weapon)
				intfaceSpr->GetSprite(SPR_WPNNAME+player.weapon-1)->Draw(intf[i].x+131,intf[i].y+24,mgl);
		}
	}

	sprintf(combo,"Combo x%d",curCombo);
	PrintGlow(240,comboY,combo,0,2);
	
	sprintf(combo,"DEBUG: %d%",player.debugString);
	//PrintGlow(240,80,combo,0,2);
	
	/*
		char wand[32];
		sprintf(wand,"%lu",player.wand[0]);
		PrintGlow(32,120,wand,0,2);
		sprintf(wand,"%lu",player.wand[1]);
		PrintGlow(32,150,wand,0,2);
		sprintf(wand,"%lu",player.wand[2]);
		PrintGlow(32,180,wand,0,2);
		sprintf(wand,"%lu",player.wand[3]);
		PrintGlow(32,210,wand,0,2);
	*/
}

void RenderCollectedStuff(int x,int y,MGLDraw *mgl)
{
	int p;

	if((player.worldProg->keychains&KC_LOONY))
		intfaceSpr->GetSprite(SPR_LOONYKEY)->Draw(x+183,y+57,mgl);
	if((player.levelProg->flags&LF_CANDLES))
		intfaceSpr->GetSprite(SPR_CANDLE)->Draw(x+181,y+20,mgl);

	if(PlayerBrains()>=curMap->numBrains)
		intfaceSpr->GetSprite(SPR_BRAIN)->Draw(x+144,y+1,mgl);

	if(player.worldProg->keychains&KC_KEYCH1)
		intfaceSpr->GetSprite(SPR_KEYCH+3)->Draw(x+171,y+29,mgl);
	if(player.worldProg->keychains&KC_KEYCH2)
		intfaceSpr->GetSprite(SPR_KEYCH)->Draw(x+150,y+42,mgl);
	if(player.worldProg->keychains&KC_KEYCH3)
		intfaceSpr->GetSprite(SPR_KEYCH+2)->Draw(x+161,y+29,mgl);
	if(player.worldProg->keychains&KC_KEYCH4)
		intfaceSpr->GetSprite(SPR_KEYCH+1)->Draw(x+160,y+42,mgl);

	p=(int)player.worldProg->percentage;
	if(p>100)
		p=100;
	if(p<0)
		p=0;

	DrawNumber(x+146,y+58,p,3,mgl);
}
