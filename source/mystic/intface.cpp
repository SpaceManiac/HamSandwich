#include "intface.h"
#include "player.h"
#include "spell.h"

#define SPR_LIFEMETER	0
#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		3
#define SPR_WEAPONS		13
#define SPR_KEYRING		1
#define SPR_BRAINOMETER 8

#define KEYRINGX		86
#define KEYRINGY		-17

#define MTRHGT		8

sprite_set_t *intfaceSpr;
static byte curLife=0;
static byte curMana=0;
static byte curBrains=0;
static byte monsAlive=0;
static int curMonsLife;
static byte bookSlide[10];
static byte flip=0;

// these are for the enemy life meter
char monsName[32];
int monsHP;
word monsTimer;
int monsY;
int orbX;
Guy *monster;

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("graphics\\intface.jsp");
}

void ExitInterface(void)
{
	delete intfaceSpr;
}

void ResetInterface(void)
{
	int i;

	curLife=0;
	curMana=0;
	monsTimer=0;
	curBrains=0;
	orbX=680;
	for(i=0;i<10;i++)
		bookSlide[i]=0;
}

void DrawLifeMeter(int x,int y,byte amt,byte max,byte mode)
{
	byte c;

	if(amt<max && max<128)
		DrawFillBox(x+max-1,y+1,x+max-1,y+MTRHGT-1,16);

	if(amt==0)
		return;
	amt--;

	if(mode==0)
	{
		if(amt>(max*2/3))
			c=44;		// in the green
		else if(amt>(max/3))
			c=174;		// in the yellow
		else
			c=140;		// in the red!!
	}
	else
	{
		if(EnoughMana())
			c=111;
		else
			c=207;
	}

	DrawFillBox(x,y+1,x,y+MTRHGT-1,c-6);
	DrawFillBox(x+amt,y+1,x+amt,y+MTRHGT-1,c-6);
	if(amt>1)
	{
		DrawFillBox(x+1,y+1,x+1,y+MTRHGT-1,c-1);
		DrawFillBox(x+amt-1,y+1,x+amt-1,y+MTRHGT-1,c-1);
		if(amt>3)
		{
			// here's the short parts on top and bottom
			DrawFillBox(x+2,y,x+(amt-2),y,c-3);
			DrawFillBox(x+2,y+MTRHGT,x+(amt-2),y+MTRHGT,c-3);
			// and here's the normal part
			DrawFillBox(x+2,y+1,x+amt-2,y+1,c);
			DrawFillBox(x+2,y+MTRHGT-1,x+amt-2,y+MTRHGT-1,c);
			DrawFillBox(x+2,y+2,x+amt-2,y+2,c+8);
			DrawFillBox(x+2,y+3,x+amt-2,y+MTRHGT-2,c+4);
		}
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
	if(monsTimer==0)
		monsY=500;
	monsTimer=60;	// 2 seconds
	monsAlive=alive;
}

byte MonsterShown(void)
{
	return ((monsTimer>0 && monsY<500) && (strcmp(monsName,"Grumpy Golem")));
}

void DrawExperience(void)
{
	int i;
	int amt;
	int xp,need;
	byte c;

	xp=player.experience;
	need=player.needExp;

	if(xp>need)
		xp=need;

	if(need>0)
		amt=(need-xp)*19/need;
	else
		amt=19;

	for(i=0;i<19;i++)
	{
		if(i<amt)
			c=0;
		else
			c=128+24-(i-amt);
		DrawFillBox(2,33+i,2+34,33+i,c);
	}
}

void UpdateInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed)
{
	int i;

	flip++;

	for(i=0;i<2;i++)
	{
		if(life>curLife)
			curLife++;
		else if(life<curLife)
			curLife--;
		if(life>curLife)
			curLife++;
		else if(life<curLife)
			curLife--;

		if(player.mana>curMana)
			curMana++;
		else if(player.mana<curMana)
			curMana--;
		if(player.mana>curMana)
			curMana++;
		else if(player.mana<curMana)
			curMana--;

		if(brains>curBrains)
			curBrains++;
		else if(brains<curBrains)
			curBrains--;
	}

	// the orbometer
	if(curBrains>0)
	{
		if(orbX>640)
			orbX--;
	}
	else
	{
		if(orbX<680)
			orbX++;
	}

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

		if(monsTimer>10)
		{
			monsY-=4;
			if(monsY<479-17)
				monsY=479-17;
		}
		else
		{
			monsY+=4;
		}
	}

	for(i=9;i>=0;i--)
	{
		if(player.spell[i])
		{
			if(player.curSpell==i && bookSlide[i]<10)
				bookSlide[i]++;
			if(player.curSpell!=i && bookSlide[i]>0)
				bookSlide[i]--;
		}
	}

}

void RenderInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed,MGLDraw *mgl)
{
	int i;
	char s[32];

	// draw the base console
	intfaceSpr->GetSprite(0)->Draw(0,-1,mgl);
	// life meter
	DrawLifeMeter(2,3,curLife,player.maxLife,0);
	// mana meter
	DrawLifeMeter(2,19,curMana,player.maxMana,1);

	// hammer speed gauge
	if(hamSpeed<HAMMER_MIN_SPEED)
		intfaceSpr->GetSprite(31+(((HAMMER_MIN_SPEED-hamSpeed)*8)/HAMMER_MIN_SPEED))->DrawGlow(244,14,mgl,-4);

	// hammer reverse indicator
	if(hmrFlags&1)
		intfaceSpr->GetSprite(40)->DrawGlow(244+11,14,mgl,-4);

	// hammer reflect indicator
	if(hmrFlags&2)
		intfaceSpr->GetSprite(41)->DrawGlow(244+22,14,mgl,-6);

	// number of hammers
	for(i=0;i<hammers;i++)
		intfaceSpr->GetSprite(1)->Draw(i*21,-1,mgl);

	// Enemy life gauge
	if(monsTimer)
	{
		intfaceSpr->GetSprite(30)->Draw(0,monsY,mgl);
		DrawLifeMeter(2,monsY+4,curMonsLife,128,0);
		// if the monster is dead, the name blinks
		Print(10,monsY-20,monsName,1,2);
		if(monsAlive || (flip&2)==0)
			Print(9,monsY-21,monsName,0,2);
	}

	// the keyring
	if(PlayerKeys(3))
		intfaceSpr->GetSprite(SPR_KEYRING+1)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(PlayerKeys(1))
		intfaceSpr->GetSprite(SPR_KEYRING+2)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(PlayerKeys(2))
		intfaceSpr->GetSprite(SPR_KEYRING+3)->Draw(KEYRINGX,KEYRINGY,mgl);
	i=PlayerKeys(0);
	if(i)
		intfaceSpr->GetSprite(SPR_KEYRING+4)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(i>1)
		intfaceSpr->GetSprite(SPR_KEYRING+5)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(i>2)
		intfaceSpr->GetSprite(SPR_KEYRING+6)->Draw(KEYRINGX,KEYRINGY,mgl);


	// player level
	DrawExperience();
	intfaceSpr->GetSprite(43+player.level/10)->Draw(0,31,mgl);
	intfaceSpr->GetSprite(53+player.level-((player.level/10)*10))->Draw(0,31,mgl);

	// books
	intfaceSpr->GetSprite(42)->Draw(443,-1,mgl);
	for(i=9;i>=0;i--)
	{
		if(player.spell[i])
		{
			intfaceSpr->GetSprite(11+i*2+(player.spell[i]==2))->
				Draw(620-9*12+bookSlide[i]+i*12,22+bookSlide[i],mgl);
			if(player.curSpell==i)
			{
				RightPrintGlow(638,-1,spellName[i*2+player.spell[i]-1],2);
			}
		}
	}

	// cash
	sprintf(s,"$%05d",player.money);
	PrintGlow(320,-1,s,2);

	intfaceSpr->GetSprite(SPR_BRAINOMETER)->Draw(orbX,479,mgl);
	if(curBrains)
		DrawFillBox(orbX-19,349+127-(curBrains-1),orbX-4,349+127,96+13+(curBrains/8));
	intfaceSpr->GetSprite(SPR_BRAINOMETER+1)->Draw(orbX,479,mgl);

	if(player.levelNum==19 && player.worldNum==2)
		WhackazoidDisplay();
}

sprite_t *GetIntfaceSpr(byte spr)
{
	return intfaceSpr->GetSprite(spr);
}
