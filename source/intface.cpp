#include "intface.h"
#include "player.h"

#define SPR_LIFEMETER	0
#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		3
#define SPR_WEAPONS		13
/*
#define SPR_KEYRING		29
#define SPR_BRAINOMETER 40
#define SPR_RAGE		42
#define	SPR_RAGEGAUGE	47 */
// I guess Expando added more weapon graphics in the middle which aren't in a normal Dr. L install
#define SPR_KEYRING     24
#define SPR_BRAINOMETER 35
#define SPR_RAGE        37
#define SPR_RAGEGAUGE   42

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

void RenderRage(byte size,MGLDraw *mgl)
{
	intfaceSpr->GetSprite(SPR_RAGE+size)->Draw(320,240,mgl);
}

void RenderInterface(byte life,byte rage,byte hmrFlags,byte hammers,int brains,int score,byte wpn,int ammo,
					 byte hamSpeed,MGLDraw *mgl)
{
	int i;
	static byte flip=0;
	char s[6];

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

		if(brains>curBrains)
			curBrains++;
		else if(brains<curBrains)
			curBrains--;
	}

	intfaceSpr->GetSprite(SPR_LIFEMETER)->Draw(5,3,mgl);
	DrawLifeMeter(7,8,curLife);
	intfaceSpr->GetSprite(SPR_RAGEGAUGE)->Draw(5,3,mgl);
	DrawRageMeter(7,29,(rage>=life),rage);

	// hammer speed gauge
	intfaceSpr->GetSprite(SPR_MINIGAUGE)->Draw(139,3,mgl);
	DrawHammerSpeed(141,8,hamSpeed*2);

	// hammer reverse indicator
	intfaceSpr->GetSprite(SPR_MINIGAUGE)->Draw(148,3,mgl);
	if(hmrFlags&1)
		DrawLitGauge(150,8,112);

	// hammer reflect indicator
	intfaceSpr->GetSprite(SPR_MINIGAUGE)->Draw(157,3,mgl);
	if(hmrFlags&2)
		DrawLitGauge(159,8,175);

	// number of hammers
	for(i=0;i<hammers;i++)
		intfaceSpr->GetSprite(SPR_IFHAMMER)->Draw(167+i*19,3,mgl);

	DrawScore(432,2,score,mgl);

	// Enemy life gauge
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

		intfaceSpr->GetSprite(SPR_LIFEMETER)->Draw(6,453,mgl);
		DrawLifeMeter(8,458,curMonsLife);
		// if the monster is dead, the name blinks
		Print(11,461,monsName,1,1);
		if(monsAlive || (flip&2)==0)
			Print(10,460,monsName,0,1);
	}

	// secondary weapons
	if(wpn)
	{
		if(wpn!=WPN_PWRARMOR)
		{
			intfaceSpr->GetSprite(SPR_WEAPONS-1+wpn)->Draw(595,30,mgl);
			sprintf(s,"%02d",ammo);
			Print(621,61,s,1,1);
			Print(620,60,s,0,1);
		}
		else
		{
			intfaceSpr->GetSprite(SPR_WEAPONS-1+wpn)->Draw(595,30,mgl);
			sprintf(s,"%3d",ammo/10);
			Print(601,61,s,1,1);
			Print(600,60,s,0,1);
		}
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
	intfaceSpr->GetSprite(SPR_BRAINOMETER)->Draw(617,342,mgl);
	if(curBrains)
		DrawFillBox(620,347+127-(curBrains-1),635,347+127,96+13+(curBrains/8));
	intfaceSpr->GetSprite(SPR_BRAINOMETER+1)->Draw(617,342,mgl);
}
