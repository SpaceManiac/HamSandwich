#include "intface.h"
#include "player.h"
#include "shop.h"

#define SPR_LIFEMETER	0
#define	SPR_RAGEGAUGE	1
#define SPR_SCORE	    2
#define SPR_WEAPONBOX	3
#define SPR_BRAINOMETER	4
#define SPR_HAMMERBOX	5
#define SPR_KEYBOX		6
#define SPR_POWERUP		7
#define SPR_OXYGAUGE	8
#define SPR_ENEMYLIFE	11
#define SPR_COINBOX		62
#define SPR_WPNNAME		22
#define SPR_LOONYKEY	50
#define SPR_CANDLE		51
#define SPR_KEYCH		52
#define SPR_BRAIN		56

#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		12
#define SPR_WEAPONS		13
#define SPR_KEYRING		29
#define SPR_RAGE		57

#define KEYRINGX		40
#define KEYRINGY		38

sprite_set_t *intfaceSpr;
static byte curLife=0;
static byte curBrains=0;
static byte monsAlive=0;
static int curMonsLife;
static int comboY,curCombo;

// these are for the enemy life meter
char monsName[32];
int monsHP;
word monsTimer;
Guy *monster;

#define INTF_AMMO	0
#define INTF_LIFE	1
#define INTF_ENEMY	2
#define NUM_INTF	3

#define IV_NONE		0
#define IV_BIGMETER	1
#define IV_SMALLMETER 2
#define IV_NUMBER	3
#define IV_VERTMETER 4
#define IV_ICONS	5
#define IV_KEYS		6
#define IV_DIAL		7	// the oxygen dial
#define IV_EVILMETER 8

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
	{0,-100,0,48,	// ammo
	 SPR_POWERUP,
	 IV_VERTMETER,64,
	 2,2,
	 0,0,
	 0},
	{0,-50,17,0,		// life meter
	 SPR_LIFEMETER,
	 IV_BIGMETER,128,
	 18,2,
	 0,128,
	 0},
	{0,520,0,479,		// enemy life
	 SPR_ENEMYLIFE,
	 IV_EVILMETER,196,
	 2,-16,
	 0,196,
	 0},
};

static byte intfFlip;

intface_t intf[NUM_INTF];

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("graphics/intface.jsp");
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
	if(amt>128)
		amt=128;

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

void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive)
{
	curMonsLife=formerLife;
	monsHP=life;
	strcpy(monsName,name);
	monsTimer=90;	// 3 seconds
	monsAlive=alive;
	intf[INTF_ENEMY].ty=479;
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

	if(PlayerMaxLife()<128)
		mgl->FillBox(x+PlayerMaxLife(),y,x+128,y+7,32+4);
	mgl->FillBox(x,y,x+value-1,y,c+10);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+12);
	mgl->FillBox(x,y+2,x+value-1,y+2,c+15);
	mgl->FillBox(x,y+3,x+value-1,y+3,c+26);
	mgl->FillBox(x,y+4,x+value-1,y+5,c+15);
	mgl->FillBox(x,y+6,x+value-1,y+6,c+12);
	mgl->FillBox(x,y+7,x+value-1,y+7,c+10);
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
	mgl->FillBox(x,y+5,x+value-1,y+5,c+22);
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

	mgl->FillBox(x,y,x+value-1,y,c+7);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+12);
	mgl->FillBox(x,y+2,x+value-1,y+2,c+15);
	mgl->FillBox(x,y+3,x+value-1,y+3,c+12);
	mgl->FillBox(x,y+4,x+value-1,y+4,c+7);
}

void DrawVertMeter(int x,int y,int value,int height,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	if(player.ammo<GetAmmoCost(player.weapon-1))
		c=32*4;
	else
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

void UpdateInterface(Map *map)
{
	int i,j;

	if(player.comboClock && player.combo>1 && comboY<2)
		comboY+=2;
	if(!player.comboClock && comboY>-22)
		comboY-=2;
	if(player.comboClock && player.combo>1)
		curCombo=player.combo;

	// set up the correct destinations for each piece
	intf[INTF_AMMO].tx=0;
	intf[INTF_AMMO].ty=10;

	intf[INTF_LIFE].tx=0;
	intf[INTF_LIFE].ty=0;

	intfFlip=1-intfFlip;
	for(i=0;i<NUM_INTF;i++)
	{
		switch(i)	// whatever is unique to each
		{
			case INTF_LIFE:
				intf[i].vDesired=player.life*128/128;//goodguy->maxHP;
				intf[i].valueLength=MonsterHP(MONS_BOUAPHA);
				if(intf[i].value<intf[i].vDesired-10)
					intf[i].value+=3;
				if(intf[i].value>intf[i].vDesired+10)
					intf[i].value-=3;
				if(intf[i].value>128)
					intf[i].value=128;
				break;
			case INTF_AMMO:
				intf[i].vDesired=player.ammo*intf[i].valueLength/(100);
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
					intf[i].ty=479;
				}
				else
				{
					intf[i].vDesired=0;
					intf[i].ty=479+30;
				}
				break;
		}
		if(intf[i].value<intf[i].vDesired)
			intf[i].value++;
		if(intf[i].value<intf[i].vDesired-100)
			intf[i].value+=10;
		if(intf[i].value<intf[i].vDesired-1000)
			intf[i].value+=100;
		if(intf[i].value>intf[i].vDesired)
			intf[i].value--;
		if(intf[i].value>intf[i].vDesired+100)
			intf[i].value-=10;
		if(intf[i].value>intf[i].vDesired+1000)
			intf[i].value-=100;

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
	word hc;

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
					PrintGlow(intf[i].x+intf[i].vOffX+2,intf[i].y+intf[i].vOffY+1,monsName,0,1);
				break;
			case IV_SMALLMETER:
				if(i==INTF_AMMO && intfFlip==0)
					DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,2,mgl);
				else
					DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,0,mgl);
				break;
			case IV_VERTMETER:
				DrawVertMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
		}
	}

	if(player.hammers>0)
	{
		hc=player.hammerCharge/100;
		if(hc<1)
			hc=1;
		if(hc>5)
			hc=5;
		for(i=0;i<(int)hc;i++)
			intfaceSpr->GetSprite(46)->Draw(12+i*10,12,mgl);
	}

	combo[0]=0;
	/*
#ifdef _DEBUG
	for(i=0;i<8;i++)
	{
		sprintf(combo,"V%d: %d",i,player.var[i]);
		Print(2+i*40,40,combo,0,1);
	}
	sprintf(combo,"%d,%d",player.enterX,player.enterY);
	Print(30,60,combo,0,1);
#endif
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
