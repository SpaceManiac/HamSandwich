#include "intface.h"
#include "player.h"
#include "options.h"
#include "radar.h"
#include "alchemy.h"
#include "leveldef.h"
#include "arena.h"

#define SPR_LIFEMETER	0
#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		3
#define SPR_WEAPONS		13
#define SPR_KEYRING		4

#define KEYRINGX		40
#define KEYRINGY		38

sprite_set_t *intfaceSpr;
static word curHearts,curMagic,curStamina;
static byte monsAlive=0;
static int curMonsLife;

static int curMoney;
static byte moneyClock,moneyY,areaY,monsMeterY;

static byte flip=0;

// these are for the enemy life meter
char monsName[32];
int monsHP;
word monsTimer;
Guy *monster;
// and the boss meter
int bossHP;

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("graphics\\intface.jsp");
	Radar_Init();
}

void ExitInterface(void)
{
	delete intfaceSpr;
	Radar_Exit();
}

void ResetInterface(void)
{
	curHearts=player.hearts;
	curMagic=player.magic;
	curStamina=player.stamina;
	monsTimer=0;
	curMoney=player.money;
	moneyClock=0;
	bossHP=0;
	Radar_Init();
	moneyY=0;
	areaY=0;
	monsMeterY=20;
}

void RenderIntfaceSprite(int x,int y,int spr,char bright,MGLDraw *mgl)
{
	intfaceSpr->GetSprite(spr)->DrawBright(x,y,mgl,bright);
}

sprite_t *GetIntfaceSprite(int spr)
{
	return intfaceSpr->GetSprite(spr);
}

void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive)
{
	curMonsLife=formerLife;
	monsHP=life;
	strcpy(monsName,name);
	monsTimer=90;	// 3 seconds
	monsAlive=alive;
	if(!alive)
		monsTimer=30;
}

void UpdateInterface(void)
{
	flip++;

	if(moneyClock)
	{
		moneyClock--;
		if(areaY>0)
			areaY-=2;
		if(moneyY<30)
			moneyY+=2;
	}
	else
	{
		if(areaY<30)
			areaY+=2;
		if(moneyY>0)
			moneyY-=2;
	}

	Radar_Update();
	if(curHearts>player.hearts)
	{
		curHearts--;
		if(curHearts>=player.hearts+5)
			curHearts-=5;
	}
	else if(curHearts<player.hearts)
	{
		curHearts++;
		if(curHearts<=player.hearts-5)
			curHearts+=5;
	}
	if(curHearts>player.maxHearts)
		curHearts=player.maxHearts;

	if(curMagic>player.magic)
	{
		curMagic--;
		if(curMagic>=player.magic+5)
			curMagic-=5;
	}
	else if(curMagic<player.magic)
	{
		curMagic++;
		if(curMagic<=player.magic-5)
			curMagic+=5;
	}
	if(curMagic>player.maxMagic)
		curMagic=player.maxMagic;

	if(curStamina>player.stamina)
	{
		curStamina--;
		if(curStamina>=player.stamina+5)
			curStamina-=5;
	}
	else if(curStamina<player.stamina)
	{
		curStamina++;
		if(curStamina<=player.stamina-5)
			curStamina+=5;
	}
	if(curStamina>player.maxStamina)
		curStamina=player.maxStamina;

	if(curMoney>player.money)
	{
		moneyClock=120;
		curMoney--;
		if(curMoney>player.money+10)
			curMoney-=10;
		if(curMoney>player.money+100)
			curMoney-=100;
		if(curMoney>player.money+1000)
			curMoney-=1000;
	}
	else if(curMoney<player.money)
	{
		curMoney++;
		if(curMoney<player.money-10)
			curMoney+=10;
		if(curMoney<player.money-100)
			curMoney+=100;
		if(curMoney<player.money-1000)
			curMoney+=1000;
		moneyClock=120;
	}

	// Enemy life gauge
	if(monsTimer)
	{
		monsTimer--;
		if(curMonsLife<monsHP)
		{
			curMonsLife+=4;
			if(curMonsLife<monsHP-5)
				curMonsLife+=5;
			if(curMonsLife>monsHP)
				curMonsLife=monsHP;
		}
		if(curMonsLife>monsHP)
		{
			curMonsLife-=4;
			if(curMonsLife>monsHP+5)
				curMonsLife-=5;
			if(curMonsLife<monsHP)
				curMonsLife=monsHP;
		}
		if(monsMeterY>1)
			monsMeterY-=2;
		else if(monsMeterY==1)
			monsMeterY--;
	}
	else
		if(monsMeterY<20)
			monsMeterY++;
}

void NewMeter(word length,word now,byte hgt,int x,int y,byte c)
{
	if(now>length)
		now=length;

	// 17 is the normal height

	DrawBox(x-2,y-1,x+length+1,y+hgt-1,0);
	DrawBox(x-1,y-2,x+length,y+hgt,0);

	if(now>0)
		DrawFillBox(x,y,x+now-1,y+hgt-2,c*32+16);
	if(now<length)
		DrawFillBox(x+now,y,x+length-1,y+hgt-2,c*32+4);
	if(now>0)
		DrawFillBox(x+now-1,y,x+now-1,y+hgt-2,c*32+24);
	if(now>0)
		DrawFillBox(x+now-2,y,x+now-2,y+hgt-2,c*32+20);
	if(now<length)
		DrawFillBox(x+now,y,x+now,y+hgt-2,c*32+16);

	if(now<length)
	{
		// highlight the empty part
		DrawFillBox(x+now,y,x+length-1,y,c*32+8);
		DrawFillBox(x+now,y+hgt-2,x+length-1,y+hgt-2,c*32+2);
		DrawFillBox(x+now,y+hgt-3,x+length-1,y+hgt-3,c*32+3);

		DrawFillBox(x+now,y+2,x+length-1,y+2,c*32+24);
		DrawFillBox(x+now,y+3,x+length-1,y+3,c*32+20);
	}
	if(now>0)
	{
		// highlight the full part
		DrawFillBox(x,y,x+now-1,y,c*32+24);
		DrawFillBox(x,y+hgt-2,x+now-1,y+hgt-2,c*32+10);
		DrawFillBox(x,y+hgt-3,x+now-1,y+hgt-3,c*32+13);

		DrawFillBox(x,y+2,x+now-1,y+2,c*32+31);
		DrawFillBox(x,y+3,x+now-1,y+3,c*32+24);
	}
}

void NewRvsMeter(word length,word now,byte hgt,int x,int y,byte c)
{
	if(now>length)
		now=length;

	// 17 is the normal height

	DrawBox(x-length-1,y-1,x+2,y+hgt-1,0);
	DrawBox(x-length,y-2,x+1,y+hgt,0);

	if(now>0)
		DrawFillBox(x-now+1,y,x,y+hgt-2,c*32+16);
	if(now<length)
		DrawFillBox(x-length+1,y,x-now,y+hgt-2,c*32+4);
	if(now>0)
		DrawFillBox(x-now+1,y,x-now+1,y+hgt-2,c*32+24);
	if(now>0)
		DrawFillBox(x-now+2,y,x-now+2,y+hgt-2,c*32+20);
	if(now<length)
		DrawFillBox(x-now,y,x-now,y+hgt-2,c*32+16);

	if(now<length)
	{
		// highlight the empty part
		DrawFillBox(x-length+1,y,x-now,y,c*32+8);
		DrawFillBox(x-length+1,y+hgt-2,x-now,y+hgt-2,c*32+2);
		DrawFillBox(x-length+1,y+hgt-3,x-now,y+hgt-3,c*32+3);

		DrawFillBox(x-length+1,y+2,x-now,y+2,c*32+24);
		DrawFillBox(x-length+1,y+3,x-now,y+3,c*32+20);
	}
	if(now>0)
	{
		// highlight the full part
		DrawFillBox(x-now+1,y,x,y,c*32+24);
		DrawFillBox(x-now+1,y+hgt-2,x,y+hgt-2,c*32+10);
		DrawFillBox(x-now+1,y+hgt-3,x,y+hgt-3,c*32+13);

		DrawFillBox(x-now+1,y+2,x,y+2,c*32+31);
		DrawFillBox(x-now+1,y+3,x,y+3,c*32+24);
	}
}

void RenderPlayerMeters(void)
{
	byte lfHgt;
	byte y;
	int life;

	if(player.var[VAR_MADCAP])
	{
		y=2;
		lfHgt=20-curHearts*12/player.maxHearts;
		if(curHearts<=player.maxHearts/4 && (player.timeToFinish%2))
			NewMeter(player.maxHearts/4,curHearts/4,lfHgt,2,y,5);	// red life meter in upper left blinks yellow
		else
			NewMeter(player.maxHearts/4,curHearts/4,lfHgt,2,y,4);	// red life meter in upper left
	}
	else
	{
		y=2;
		lfHgt=20-curHearts*12/player.maxHearts;
		if(curHearts<=player.maxHearts/4 && (player.timeToFinish%2))
			NewMeter(player.maxHearts*2,curHearts*2,lfHgt,2,y,5);	// red life meter in upper left blinks yellow
		else
			NewMeter(player.maxHearts*2,curHearts*2,lfHgt,2,y,4);	// red life meter in upper left
	}

	y+=lfHgt+1;
	lfHgt=20-curStamina*12/player.maxStamina;
	NewMeter(player.maxStamina*2,curStamina*2,lfHgt,2,y,0);	// grey stamina meter under life meter
	y+=lfHgt+1;
	lfHgt=20-curMagic*12/player.maxMagic;
	NewMeter(player.maxMagic*2,curMagic*2,lfHgt,2,y,3);	// blue magic meter under stamina
	y+=lfHgt+1;

	life=player.var[VAR_CLOCKLIFE]+player.var[VAR_CLOCKLIFE2]*256;
	if(life>0)
	{
		lfHgt=20-life*12/MonsterHP(MONS_YOURBOT,GOOD);
		if(player.var[VAR_MADCAP])
			NewMeter(MonsterHP(MONS_YOURBOT,GOOD)/4,life/4,lfHgt,2,y,2);
		else
			NewMeter(MonsterHP(MONS_YOURBOT,GOOD),life,lfHgt,2,y,2);
		y+=lfHgt+1;
	}
	if(player.potionClock && player.potionType!=0)
	{
		lfHgt=10;
		NewMeter(64,player.potionClock*64/PotionDuration(player.potionType,player.potionPower),lfHgt,2,y,6);
		//NewRvsMeter(64,player.potionClock*64/PotionDuration(player.potionType,player.potionPower),17,637,29,6);
	}
}

void RenderInterface(MGLDraw *mgl)
{
	char s[32];

	Radar_Render(mgl);

	RenderPlayerMeters();

	// Enemy life gauge
	NewRvsMeter(128,curMonsLife,17,637,462+monsMeterY,4);
	// if the monster is dead, the name blinks
	RightPrint(635,466+monsMeterY,monsName,1,1);
	if(monsAlive || (flip&2)==0)
		RightPrint(634,465+monsMeterY,monsName,0,1);

	sprintf(s,"$%04d",curMoney);

	RightPrintDark(637,-29-1+moneyY,s,0);
	RightPrintDark(639,-29+1+moneyY,s,0);
	RightPrint(638,-29+moneyY,s,0,0);

	if(player.timeLimit>0 && (player.clockRunning || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX)))
	{
		sprintf(s,"%d:%02d:%02d",player.timeLimit/(30*60),(player.timeLimit/(30))%60,((player.timeLimit*100)/30)%100);
		Print(400,1,s,-32,0);
		Print(402,3,s,-32,0);
		Print(401,2,s,0,0);
	}

	// display area name
	if(levelDef[player.levelNum].flags&LF_ARENA)
	{
		PrintArenaInfo(areaY);
	}
	else
	{
		RightPrintDark(637,-29-1+areaY,curWorld.map[player.levelNum]->name,0);
		RightPrintDark(639,-29+1+areaY,curWorld.map[player.levelNum]->name,0);
		RightPrintColor(638,-29+areaY,curWorld.map[player.levelNum]->name,(byte)((levelDef[player.levelNum].flags&LF_TOWN)>0),-2,0);
	}
}
