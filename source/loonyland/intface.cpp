#include "intface.h"
#include "player.h"
#include "options.h"
#include "bowling.h"
#include "ch_witch.h"
#include "ch_summon.h"
#include "ch_girl.h"

#define SPR_LIFEMETER	0
#define SPR_IFHAMMER	1
#define SPR_MINIGAUGE	2
#define SPR_NUMBERS		3
#define SPR_WEAPONS		13
#define SPR_KEYRING		4

#define KEYRINGX		40
#define KEYRINGY		38

sprite_set_t *intfaceSpr;
static byte curHearts=0;
static byte monsAlive=0;
static int curMonsLife;

static byte activeHeart;
static byte heartClock;
static int curMoney;
static int comboX;

static byte flip=0;

static byte *radar;
static byte radarAngle;

// these are for the enemy life meter
char monsName[32];
int monsHP;
word monsTimer;
Guy *monster;
// and the boss meter
int bossHP;

char areaNoName[32]="Unused";

char areaName[][32]={
	"Luniton Village",
	"Slurpy Swamp",
	"Rocky Cliffs",
	"Terror Glade",
	"Wicked Woods",
	"Mystery Lake",
	"Vampy Land",
	"A Cemetery",
	"Zombiton",
	"Pointless Plains",
	"A Farm",
	"Ghastly Glen"
};

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("graphics/intface.jsp");
	radar=(byte *)malloc(128*128);
}

void ExitInterface(void)
{
	delete intfaceSpr;
	free(radar);
}

void ResetInterface(void)
{
	int i;

	heartClock=0;
	activeHeart=255;
	curHearts=player.hearts;
	monsTimer=0;
	curMoney=player.money;
	comboX=200;
	bossHP=0;

	for(i=0;i<128*128;i++)
		radar[i]=0;
}

void RenderRadar(int x,int y,MGLDraw *mgl)
{
	int i,j;
	int rpos;
	byte *scrn;

	scrn=(byte *)(mgl->GetScreen()+x+y*mgl->GetWidth());
	rpos=0;

	for(j=0;j<128;j++)
	{
		for(i=0;i<128;i++)
		{
			if(radar[rpos])
				*scrn=radar[rpos];

			scrn++;
			rpos++;
		}
		scrn=scrn+mgl->GetWidth()-128;
	}
}

void AddRadarBlip(int x,int y,byte c,Map *map)
{
	int rx,ry;

	rx=(x*128)/map->width;
	ry=(y*128)/map->height;

	if(rx<0 || rx>127 || ry<0 || ry>127)
		return;

	if(flip&4)
	{
		radar[rx+ry*128]=c;

		if(rx>0)
			radar[rx-1+ry*128]=c;
		if(rx<127)
			radar[rx+1+ry*128]=c;
		if(ry>0)
			radar[rx+(ry-1)*128]=c;
		if(ry<127)
			radar[rx+(ry+1)*128]=c;

		if(flip&8)
		{
			if(rx>2)
				radar[rx-3+ry*128]=c;
			if(rx<127-2)
				radar[rx+3+ry*128]=c;
			if(ry>2)
				radar[rx+(ry-3)*128]=c;
			if(ry<127-2)
				radar[rx+(ry+3)*128]=c;
		}
	}
	else
	{
		radar[rx+ry*128]=c;

		if(rx>0 && radar[rx-1+ry*128]==0)
			radar[rx-1+ry*128]=0;
		if(rx<127 && radar[rx+1+ry*128]==0)
			radar[rx+1+ry*128]=0;
		if(ry>0 && radar[rx+(ry-1)*128]==0)
			radar[rx+(ry-1)*128]=0;
		if(ry<127 && radar[rx+(ry+1)*128]==0)
			radar[rx+(ry+1)*128]=0;
	}
}

void RadarSweep(void)
{
	int x,y,xx,yy;
	byte b,b2;

	x=63*FIXAMT+FIXAMT/2;
	y=63*FIXAMT+FIXAMT/2;

	while(1)
	{
		xx=x/FIXAMT;
		yy=y/FIXAMT;
		if(xx>=0 && yy>=0 && xx<128 && yy<128)
		{
			b=radar[xx+yy*128]&31;
			b2=radar[xx+yy*128]&(~31);
			if(b<31-8)
				b+=8;
			else
				b=31;
			if(radar[xx+yy*128]==0)
				radar[xx+yy*128]=7*32+b;
			else
				radar[xx+yy*128]=b2|b;
			x+=Cosine(radarAngle);
			y+=Sine(radarAngle);
		}
		else
			return;	// done drawing
	}
}

void CleanRadar(void)
{
	int i;
	byte r;

	for(i=0;i<128*128;i++)
	{
		r=radar[i]&31;
		if(r<4)
			r=0;
		else r-=4;
		if(r>0)
			radar[i]=(radar[i]&(~31))+r;
		else
			radar[i]=0;
	}
}

void RenderIntfaceSprite(int x,int y,int spr,char bright,MGLDraw *mgl)
{
	intfaceSpr->GetSprite(spr)->DrawBright(x,y,mgl,bright);
}

sprite_t *GetIntfaceSprite(int spr)
{
	return intfaceSpr->GetSprite(spr);
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

void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive)
{
	curMonsLife=formerLife;
	monsHP=life;
	strcpy(monsName,name);
	monsTimer=60;	// 2 seconds
	monsAlive=alive;
}

void SetBossHP(int hp,int max)
{
	char s[64];
	static int prevHP=1;

	player.clockRunning=1;

	bossHP=hp*256/max;
	if(bossHP>255)
		bossHP=255;

	if(hp==0 && prevHP!=0)
	{
		if(player.worldNum!=WORLD_BOSSBASH)
		{
			sprintf(s,"Victory in %d:%02d:%02d",player.timeLimit/(30*60),(player.timeLimit/(30))%60,((player.timeLimit*100)/30)%100);
			NewMessage(s,90);
		}
		player.clockRunning=0;
	}

	prevHP=hp;
}

void UpdateInterface(byte life)
{
	flip++;

	if(opt.cheats[CH_RADAR])
	{
		radarAngle+=2;
		RadarSweep();
	}

	if(heartClock)
	{
		heartClock--;
		if(heartClock==0)
			activeHeart=255;
	}
	else
	{
		if(curHearts!=life)
		{
			if(curHearts>life)
			{
				curHearts--;
				activeHeart=curHearts+1;
			}
			else
			{
				MakeNormalSound(SND_HEAL);
				curHearts++;
				activeHeart=curHearts;
			}
			heartClock=4;
		}
	}

	if(curMoney>player.money)
	{
		curMoney--;
	}
	else if(curMoney<player.money)
	{
		curMoney++;
	}

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
	}

	if(player.comboClock && player.combo>1)
	{
		if(comboX)
			comboX-=10;
	}
	else
		if(comboX<200)
			comboX+=10;
}

char *GetAreaName(byte a)
{
	if(a==255)
		return areaNoName;
	return areaName[a];
}

void RenderInterface(MGLDraw *mgl)
{
	int i,mx,my;
	char s[32];

	if(player.worldNum==WORLD_BOWLING)
	{
		RenderBowlingIntface(mgl);
		return;
	}

	for(i=player.maxHearts-1;i>=curHearts;i--)
	{
		intfaceSpr->GetSprite(2)->Draw(2+i*5,2,mgl);
	}
	for(i=0;i<curHearts;i++)
	{
		intfaceSpr->GetSprite(1)->Draw(2+i*5,2,mgl);
	}
	if(activeHeart!=255)
		intfaceSpr->GetSprite(0)->Draw(2+activeHeart*5,2,mgl);

	// Enemy life gauge
	if(monsTimer)
	{
		DrawLifeMeter(8,458,curMonsLife);
		// if the monster is dead, the name blinks
		Print(11,461,monsName,1,1);
		if(monsAlive || (flip&2)==0)
			Print(10,460,monsName,0,1);
	}

	if(player.weapon && player.monsType!=MONS_PLYRWITCH && player.monsType!=MONS_LOONY2 &&
		player.monsType!=MONS_PLYRWOLF && player.monsType!=MONS_PLYRSUMMON && player.monsType!=MONS_PLYRTHIEF)
	{
		InstaRenderItem(17,42,player.weapon,0,mgl);
		if(player.wpnLevel==0)
			Print(10,42,"I",0,0);
		else if(player.wpnLevel==1)
			Print(5,42,"II",0,0);
		else
			Print(0,42,"III",0,0);
		sprintf(s,"%d",player.wpnCost);
		Print(25,62,s,0,0);
		InstaRenderItem(8,82,ITM_LITTLEGEM,0,mgl);
	}
	if(player.keys[0]>0)
	{
		if(player.monsType==MONS_PLYRWITCH || player.monsType==MONS_PLYRTHIEF)
		{
			InstaRenderItem(20,142,ITM_KEY,0,mgl);
			sprintf(s,"x%d",player.keys[0]);
			Print(38,107,s,0,2);
		}
		else
		{
			InstaRenderItem(50,52,ITM_KEY,0,mgl);
			sprintf(s,"x%d",player.keys[0]);
			Print(68,17,s,0,2);
		}
	}

	if(player.worldNum==WORLD_LOONYBALL)
	{
		if(player.levelNum<4)
			sprintf(s,"Score: %d/%d",curMoney,player.maxMoney);
		else
			sprintf(s,"Score: %d",curMoney);

		RightPrint(637,1,s,-32,0);
		RightPrint(639,3,s,-32,0);
		if(curMoney<player.maxMoney)
			RightPrintColor(638,2,s,4,0,0);
		else
			RightPrint(638,2,s,0,0);
	}
	else
	{
		sprintf(s,"%03d/%03d",curMoney,player.maxMoney);

		RightPrint(637,1,s,-32,0);
		RightPrint(639,3,s,-32,0);
		RightPrint(638,2,s,0,0);
		InstaRenderItem(530,22,ITM_LITTLEGEM,0,mgl);
	}

	if(player.worldNum!=WORLD_BOSSBASH)
	{
		if(player.timeLimit>0 && (player.clockRunning || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX)))
		{
			sprintf(s,"%d:%02d:%02d",player.timeLimit/(30*60),(player.timeLimit/(30))%60,((player.timeLimit*100)/30)%100);
			Print(400,1,s,-32,0);
			Print(402,3,s,-32,0);
			Print(401,2,s,0,0);
		}
	}
	else
	{
		sprintf(s,"%d:%02d:%02d",player.timeToFinish/(30*60),(player.timeToFinish/(30))%60,((player.timeToFinish*100)/30)%100);
		Print(400,1,s,-32,0);
		Print(402,3,s,-32,0);
		Print(401,2,s,0,0);
	}

	// display area name
	if(player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX|| player.worldNum==WORLD_RANDOMIZER)
	{
		if(opt.cheats[CH_CRYSTAL])
		{
			if(player.crystalBall==0)	// no more items
			{
				RightPrint(637,452,player.areaName,-32,0);
				RightPrint(639,454,player.areaName,-32,0);
				RightPrintColor(638,453,player.areaName,4,0,0);
			}
			else	// yes more items
			{
				RightPrint(637,452,player.areaName,-32+player.crystalBall,0);
				RightPrint(639,454,player.areaName,-32+player.crystalBall,0);
				RightPrintColor(638,453,player.areaName,7,-10+player.crystalBall,0);
			}
		}
		else
		{
			RightPrint(637,452,player.areaName,-32,0);
			RightPrint(639,454,player.areaName,-32,0);
			RightPrint(638,453,player.areaName,0,0);
		}
	}

	if(player.worldNum==WORLD_SLINGSHOT)
	{
		mgl->GetMouse(&mx, &my);
		RenderIntfaceSprite(mx,my,9,0,mgl);
	}

	// bat indicator
	if(player.fireFlags&FF_HELPERHERE)
	{
		DrawBox(620-5,55,620+5,55+65,128+16);
		DrawFillBox(620-4,56,620+4,55+64,0);
		if(player.batLevel>0)
			DrawFillBox(620-4,56,620+4,56+player.batLevel/4,128+8);
		if(player.batLevel==255)
			InstaRenderItem(620-4+Random(8),52-4+Random(8),ITM_BATDOLL,32,mgl);
		else
			InstaRenderItem(620,52,ITM_BATDOLL,(char)(-10+(player.batLevel/8)),mgl);
	}

	// boss hp
	if(bossHP>0)
	{
		if(bossHP>128+32)
			i=44;		// in the green
		else if(bossHP>64)
			i=174;		// in the yellow
		else
			i=140;		// in the red!!

		DrawBox(2,150,22,150+256+4,31);
		DrawFillBox(4,150+256+1-bossHP,20,150+256+2,i);
		Print(7,181,"B",-32,0);
		Print(7,231,"O",-32,0);
		Print(9,281,"S",-32,0);
		Print(9,331,"S",-32,0);
		Print(5,179,"B",-32,0);
		Print(5,229,"O",-32,0);
		Print(7,279,"S",-32,0);
		Print(7,329,"S",-32,0);

		Print(6,180,"B",0,0);
		Print(6,230,"O",0,0);
		Print(8,280,"S",0,0);
		Print(8,330,"S",0,0);
	}

	// witch mana meter
	if(player.monsType==MONS_PLYRWITCH)
	{
		DrawBox(2,25,2+3+(player.fireRange+1)*32,40,31);
		if(player.xtraVar/FIXAMT)
		{
			// make it red if you can't afford the spell
			if(player.xtraVar/FIXAMT<SpellCost(player.xtraByte))
				DrawFillBox(4,27,3+(player.xtraVar/FIXAMT),38,4*32+16);
			else
			{
				// make it blue with white for the part the spell will use up
				DrawFillBox(4,27,3+(player.xtraVar/FIXAMT)-SpellCost(player.xtraByte),38,7*32+16);
				DrawFillBox(4+(player.xtraVar/FIXAMT)-SpellCost(player.xtraByte),27,3+(player.xtraVar/FIXAMT),38,31);
			}
		}
		sprintf(s,"%s %d",SpellName(player.xtraByte),SpellLevel(player.xtraByte)+1);
		Print(2,42,s,0,0);
		//sprintf(s,"%d%%",SpellPct(player.xtraByte));
		//Print(2,62,s,0,0);
	}
	else if(player.monsType==MONS_PLYRSUMMON)
	{
		if(player.weapon==0)
			sprintf(s,"%s %d",SummonName(0),SummonCost(0));
		else
			sprintf(s,"%s %d",SummonName(player.weapon-ITM_WBOMB+1),SummonCost(player.weapon-ITM_WBOMB+1));
		RightPrint(608,30,s,0,0);
	}
	else if(player.monsType==MONS_PLYRTHIEF)
	{
		DrawBox(2,25,2+3+(player.fireRange+1)*32,40,31);
		if(player.xtraVar/FIXAMT)
		{
			// make it red if you are reloading
			if(player.xtraByte)
				DrawFillBox(4,27,3+(player.xtraVar/FIXAMT),38,4*32+16);
			else
			{
				// make it blue
				DrawFillBox(4,27,3+(player.xtraVar/FIXAMT),38,7*32+16);
			}
		}
		if(player.weapon>0)
		{
			sprintf(s,"%s",BombName(player.weapon-ITM_WBOMB+1));
			Print(2,42,s,0,0);
		}
		RenderKnifeUps(mgl);
	}
	// combo meter
	if(comboX<200 && player.combo>1)
	{
		sprintf(s,"Combo x%d",player.combo);
		//RightPrint(638+comboX,80,s,0,0);
		Print(2-comboX,80,s,0,0);
	}

	// radar
	if(opt.cheats[CH_RADAR])
	{
		RenderRadar(510,330,mgl);
		//DrawBox(510,330,510+127,330+127,31);
	}

	// summoned troops
	if(player.monsType==MONS_PLYRSUMMON)
		RenderSummonLife(mgl);

	// scribbles for vintage mode, on top of EVERYTHING
	if(opt.cheats[CH_VINTAGE])
	{
		if(Random(10)==0)
		{
			intfaceSpr->GetSprite(18+Random(6))->DrawGhost(Random(640),Random(480),
				mgl,-8+(byte)Random(17));
			if(Random(10)==0)
			{
				intfaceSpr->GetSprite(18+Random(6))->DrawGhost(Random(640),Random(480),
					mgl,-8+(byte)Random(17));
				if(Random(10)==0)
				{
					intfaceSpr->GetSprite(18+Random(6))->DrawGhost(Random(640),Random(480),
						mgl,-8+(byte)Random(17));
				}
			}
		}
	}
}
