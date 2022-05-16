#include "intface.h"
#include "player.h"

#define SPR_LIFEMETER	0
#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		3
#define SPR_WEAPONS		13
#define SPR_KEYRING		18
#define SPR_BRAINOMETER 29

#define KEYRINGX		40
#define KEYRINGY		38

sprite_set_t *intfaceSpr;
static byte curLife=0;
static byte curBrains=0;
static byte monsAlive=0;
static int curMonsLife;

// these are for the enemy life meter
char monsName[32];
int monsHP;
word monsTimer;
Guy *monster;

static byte flip=0;

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("graphics/intface.jsp");
}

void ExitInterface(void)
{
	delete intfaceSpr;
}

void ResetInterface(void)
{
	curLife=0;
	monsTimer=0;
	curBrains=0;
}

void DrawLifeMeter(int x,int y,byte amt)
{
	byte c;

	if(amt==0)
		return;
	amt--;

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
	monsTimer=60;	// 2 seconds
	monsAlive=alive;
}

void UpdateInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed)
{
	flip++;
	int i;

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

		if(brains>curBrains)
			curBrains++;
		else if(brains<curBrains)
			curBrains--;
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
	}
}

void RenderInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed,MGLDraw *mgl)
{
	int i;
	char s[6];

	intfaceSpr->GetSprite(SPR_LIFEMETER)->Draw(5,5,mgl);
	DrawLifeMeter(7,8,curLife);

	// hammer speed gauge
	intfaceSpr->GetSprite(SPR_MINIGAUGE)->Draw(138,5,mgl);
	DrawHammerSpeed(140,8,hamSpeed*2);

	// hammer reverse indicator
	intfaceSpr->GetSprite(SPR_MINIGAUGE)->Draw(147,5,mgl);
	if(hmrFlags&1)
		DrawLitGauge(149,8,112);

	// hammer reflect indicator
	intfaceSpr->GetSprite(SPR_MINIGAUGE)->Draw(156,5,mgl);
	if(hmrFlags&2)
		DrawLitGauge(158,8,175);

	// number of hammers
	for(i=0;i<hammers;i++)
		intfaceSpr->GetSprite(SPR_IFHAMMER)->Draw(166+i*19,3,mgl);

	DrawScore(430,5,score,mgl);

	// Enemy life gauge
	if(monsTimer)
	{
		intfaceSpr->GetSprite(SPR_LIFEMETER)->Draw(5,455,mgl);
		DrawLifeMeter(7,458,curMonsLife);
		// if the monster is dead, the name blinks
		Print(10,461,monsName,1,1);
		if(monsAlive || (flip&2)==0)
			Print(9,460,monsName,0,1);
	}

	// secondary weapons
	if(wpn)
	{
		intfaceSpr->GetSprite(SPR_WEAPONS-1+wpn)->Draw(595,30,mgl);
		sprintf(s,"%02d",ammo);
		Print(621,61,s,1,1);
		Print(620,60,s,0,1);
	}

	// the almighty keyring
	intfaceSpr->GetSprite(SPR_KEYRING)->Draw(KEYRINGX,KEYRINGY,mgl);
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

	if(PlayerKeyChain(0))
		intfaceSpr->GetSprite(SPR_KEYRING+7)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(PlayerKeyChain(1))
		intfaceSpr->GetSprite(SPR_KEYRING+8)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(PlayerKeyChain(2))
		intfaceSpr->GetSprite(SPR_KEYRING+9)->Draw(KEYRINGX,KEYRINGY,mgl);
	if(PlayerKeyChain(3))
		intfaceSpr->GetSprite(SPR_KEYRING+10)->Draw(KEYRINGX,KEYRINGY,mgl);

	// the brainometer
	intfaceSpr->GetSprite(SPR_BRAINOMETER)->Draw(616,345,mgl);
	if(curBrains)
		DrawFillBox(619,348+127-(curBrains-1),634,348+127,96+13+(curBrains/8));
	intfaceSpr->GetSprite(SPR_BRAINOMETER+1)->Draw(616,345,mgl);
}
