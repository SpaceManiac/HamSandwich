#include "intface.h"
#include "player.h"
#include "spell.h"
#include "achieves.h"

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
	intfaceSpr=new sprite_set_t("graphics/intface.jsp");
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
	orbX=SCRWID+40;
	for(i=0;i<10;i++)
		bookSlide[i]=0;
}

void DrawLifeMeter(int x,int y,byte amt,byte max,byte mode,byte barrier)
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
		if(EnoughMana() || VeryClassicMode())
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
	if (barrier > 0)
	{
		if(barrier>1)
			DrawFillBox(x+1, y + MTRHGT - 4, x + barrier-1, y + MTRHGT-4, 12);
		DrawFillBox(x, y + MTRHGT - 3, x + barrier, y + MTRHGT-3, 24);
		DrawFillBox(x, y + MTRHGT - 2, x + barrier, y + MTRHGT-1, 16);
		if(barrier>1)
			DrawFillBox(x+1, y + MTRHGT, x + barrier-1, y + MTRHGT, 12);
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

void ResetInterface(bool fillLifeAndMana)
{
	PlayerUpdateLife();
	if (fillLifeAndMana)
	{
		player.life = player.maxLife;
		player.mana = player.maxMana;
	}

	curLife = player.life;
	curMana = player.mana;
}

void UpdateInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed)
{
	int i;

	flip++;
	if(!GameIsPaused())
		UpdateAchieves();
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
		if(orbX>SCRWID)
			orbX--;
	}
	else
	{
		if(orbX<SCRWID+40)
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

	RenderNumberParticles();
	if (player.levelNum == 19 && player.worldNum == 3 && player.puzzleVar[0] != HORK_NONE)	// a horkbox is active!
	{
		Guy* g = GetGuy((word)player.puzzleVar[1]);
		if (g)
		{
			int maxHork = 60 * 30 - 3 * 30 * 7 + player.puzzleVar[2] * 30 * 3;	// first horkbox is only 39s long, going up to 60s at max
			int dist = g->hp*(SCRWID / 2) / maxHork;
			DrawFillBox(SCRWID/2-dist, SCRHEI - 10, SCRWID/2+dist, SCRHEI, 32 * 4 + 16);
			switch (player.puzzleVar[0])
			{
				case HORK_LIFEDRAIN:
					strcpy(s, "Life Drain");
					break;
				case HORK_MANADRAIN:
					strcpy(s, "Mana Drain");
					break;
				case HORK_NOFIREBALLS:
					strcpy(s, "Fireproof");
					break;
				case HORK_BOMBS:
					strcpy(s, "Boomstorm");
					break;
				case HORK_FLAMES:
					strcpy(s, "Fricassee");
					break;
				case HORK_HEAL:
					strcpy(s, "Monstrous Mending");
					break;
				case HORK_TRAP:
					strcpy(s, "Stonegrasp");
					break;
			}
			CenterPrint(HALFWID+2,SCRHEI - 20+2, s,-31, 2);
			CenterPrintGlow(SCRHEI - 20, s, 2);
		}
	}
	// draw the base console
	intfaceSpr->GetSprite(0)->Draw(0,-1,mgl);
	// life meter
	DrawLifeMeter(2,3,curLife,player.maxLife,0,player.barrier);
	// mana meter
	DrawLifeMeter(2,19,curMana,player.maxMana,1,0);

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
		DrawLifeMeter(2,monsY+4,curMonsLife,128,0,0);
		// if the monster is dead, the name blinks
		Print(10,monsY-20,monsName,1,2);
		if(monsAlive || (flip&2)==0)
			Print(9,monsY-21,monsName,0,2);
	}

	if (!ClassicMode() && player.skillPts > 0 && !player.disableSkillWarning)
	{
		if(player.skillPts==1)
			sprintf(s, "%d Skill Point!", player.skillPts);
		else
			sprintf(s, "%d Skill Points!", player.skillPts);
		PrintBright(44 + 1, 37 + 1, s, -31, 2);
		if ((flip % 32) > 4)
			PrintBrightGlow(44, 37, s, 0, 2);
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
	intfaceSpr->GetSprite(42)->Draw(SCRWID-197,-1,mgl);
	byte mySpell = player.curSpell;
	for(i=9;i>=0;i--)
	{
		if(player.spell[i])
		{
			player.curSpell = i;
			intfaceSpr->GetSprite(11+i*2+(player.spell[i]==2 && !player.downgradeSpell[i]) && !VeryClassicMode())->
				DrawBright(SCRWID-20-9*12+bookSlide[i]+i*12,22+bookSlide[i],mgl,-10+10*EnoughMana());
			if(mySpell==i)
			{
				byte b = player.spell[i] - 1;
				if (player.downgradeSpell[i])
					b = 0;
				if(EnoughMana())
					RightPrintGlow(SCRWID-2,-1,spellName[i*2+b],2);
				else
					PrintBrightGlow(SCRWID - 2-GetStrLength(spellName[i*2+b],2), -1, spellName[i * 2 + b], -22, 2);
			}
		}
	}
	player.curSpell = mySpell;
	// cash
	sprintf(s,"$%05d",TotalMoney());
	PrintGlow(HALFWID,-1,s,2);

	intfaceSpr->GetSprite(SPR_BRAINOMETER)->Draw(orbX,SCRHEI-1,mgl);
	if(curBrains)
		DrawFillBox(orbX-19,SCRHEI-131+127-(curBrains-1),orbX-4,SCRHEI-131+127,96+13+(curBrains/8));
	intfaceSpr->GetSprite(SPR_BRAINOMETER+1)->Draw(orbX,SCRHEI-1,mgl);

	if(player.levelNum==19 && player.worldNum==2)
		WhackazoidDisplay();

	if (!ClassicMode())
	{
		byte mana = GetStoredHealMana();
		int y = 90;
		if (mana > 0)
		{
			RenderSkillBox(3, y, 25, y+20, 32 * 5 + 16, 32 * 5+10);
			intfaceSpr->GetSprite(11 + 8 * 2 + (player.spell[8] == 2 && !player.downgradeSpell[8]))->Draw(3, y-10, mgl);
			DrawFillBox(20, y+2, 23, y+18, 0);
			byte fill = (byte)((int)GetStoredHealMana() * 16 / (int)HealCostIgnoringStoredMana());
			if (fill > 16) fill = 16;
			DrawFillBox(20, y+2+(16-fill), 23, y+18, 32*1+16);
			y += 40;
		}
		mana = GetStoredSeekerMana();
		if (mana > 0)
		{
			RenderSkillBox(3, y, 25, y+20, 32 * 5 + 16, 32 * 5 + 10);
			intfaceSpr->GetSprite(11 + SPL_SEEKER * 2 + (player.spell[SPL_SEEKER] == 2 && !player.downgradeSpell[SPL_SEEKER]))->Draw(3, y-10, mgl);
			DrawFillBox(20, y+2, 23, y+18, 0);
			byte fill = (byte)((int)GetStoredSeekerMana() * 16 / (int)SpellCost(SPL_SEEKER,true));
			if (fill > 16) fill = 16;
			DrawFillBox(20, y+2+ (16 - fill), 23, y+18, 32 * 1 + 16);
		}
		if (player.ectoplasm > 0)
		{
			for(int i=-1;i<=1;i++)
				for(int j=-1;j<=1;j++)
					bulletSpr->GetSprite(262)->DrawBright(SCRWID - 20+i, HALFHEI+j, mgl, -31);
			bulletSpr->GetSprite(262)->DrawGlow(SCRWID - 20, HALFHEI, mgl, 0);
			sprintf(s, "%d", player.ectoplasm);
			RightPrint(SCRWID - 3+2, HALFHEI +2, s, -31, 2);
			RightPrint(SCRWID - 3, HALFHEI, s, 0, 2);
		}
	}
	if (player.puzzleVar[0] > 0 || player.puzzleVar[1] > 0 || player.puzzleVar[2] > 0 || (player.worldNum==2 && player.levelNum==24))
	{
		if (player.worldNum == 2 && player.levelNum == 24)	// swamp witch ingredients again, but with a stew heat timer
		{
			byte state, temp, stews;
			GetOrderUpStats(&state, &stews, &temp);
			if (state == 1)	// ingredients when finding ingredients
			{
				int y = HALFHEI - 50;
				RenderSkillBox(SCRWID - 60, y, SCRWID + 4, y + 114, 32 * 5 + 16, 32 * 5 + 4);
				// grimbleweed
				GetItemSprite(1)->Draw(SCRWID - 60 + 5 + 16, y + 110 - 34 * 3 + 21, mgl);
				sprintf(s, "%d", player.puzzleVar[0]);
				Print(SCRWID - 60 + 3 + 32 - 5, y + 110 - 34 * 3 + 5, s, 0, 2);
				// octon juice
				bulletSpr->GetSprite(262)->DrawColored(SCRWID - 60 + 5 + 16 - 4, y + 110 - 34 * 2 + 18, mgl, 6, 0);
				sprintf(s, "%d", player.puzzleVar[1]);
				Print(SCRWID - 60 + 3 + 32 - 5, y + 110 - 34 * 2 + 5, s, 0, 2);
				// toadstools
				GetItemSprite(21 + 3)->Draw(SCRWID - 60 + 5 + 16, y + 110 - 34 + 21, mgl);
				sprintf(s, "%d", player.puzzleVar[2]);
				Print(SCRWID - 60 + 3 + 32 - 5, y + 110 - 34 * 1 + 5, s, 0, 2);
			}
			else if (state == 2)	// delivering stew
			{
				int x = SCRWID - 30;
				int y = HALFHEI - 40;
				RenderSkillBox(x, y - 1, x + 20, y + 121, 31, 0);
				byte c[] = { 4,5,1,6,3,7 };	// colors for the flames
				int hgt = 120 / 6;
				byte tempY = temp * 120 / (6 * 40);
				for (int i = 0; i < 6; i++)
				{
					int yy = y + 120 - hgt * i;
					if (temp >= i * 40 && temp < (i + 1) * 40)
						DrawFillBox(x + 1, yy - hgt, x + 19, yy, c[i] * 32 + 20);
					else
						DrawFillBox(x + 1, yy - hgt, x + 19, yy, c[i] * 32 + 8);
				}
				DrawFillBox(x - 2, y + 120 - tempY, x + 22, y + 120 - tempY, 31);
				GetItemSprite(5)->Draw(x, y - 10, mgl);
				sprintf(s, "%d", stews);
				RightPrint(x + 30+2, y - 20+2, s, -31, 2);
				RightPrint(x + 30, y - 20, s, 0, 2);
			}
		}
		if (player.worldNum == 0 && player.levelNum == 20)	// swamp witch ingredients
		{
			byte state;
			byte temp;
			word time;
			byte score;
			GetSwampStats(&state,&temp, &time,&score);

			if (state == 0)	// ingredients
			{
				int y = HALFHEI - 50;
				RenderSkillBox(SCRWID - 60, y, SCRWID + 4, y+114, 32 * 5 + 16, 32 * 5 + 4);
				// grimbleweed
				GetItemSprite(1)->Draw(SCRWID - 60 + 5 + 16, y+110 - 34 * 3 + 21, mgl);
				sprintf(s, "%d", player.puzzleVar[0]);
				Print(SCRWID - 60 + 3 + 32 - 5, y+110 - 34 * 3 + 5, s, 0, 2);
				// octon juice
				bulletSpr->GetSprite(262)->DrawColored(SCRWID - 60 + 5 + 16 - 4, y+110 - 34 * 2 + 18, mgl, 6, 0);
				sprintf(s, "%d", player.puzzleVar[1]);
				Print(SCRWID - 60 + 3 + 32 - 5, y+110 - 34 * 2 + 5, s, 0, 2);
				// toadstools
				GetItemSprite(21 + 3)->Draw(SCRWID - 60 + 5 + 16, y+110 - 34 + 21, mgl);
				sprintf(s, "%d", player.puzzleVar[2]);
				Print(SCRWID - 60 + 3 + 32 - 5, y+110 - 34 * 1 + 5, s, 0, 2);
			}
			else if (state == 2)	// cooking
			{
				int x, y;
				GetCamera(&x, &y);
				x = (65*TILE_WIDTH+TILE_WIDTH/2)  - x + HALFWID;
				y = (62*TILE_HEIGHT+TILE_HEIGHT/2) - y + HALFHEI;
				x -= 70;
				y -= 60;
				// time meter
				RenderSkillBox(x, y-1, x+20, y+121, 31, 0);
				DrawFillBox(x+1, y+120- (time * 120 / (30 * 10)), x+19, y+120, 24);	// cook timer
				DrawFillBox(x-2, y+120 - (time * 120 / (30 * 10)), x+22, y+120- (time * 120 / (30 * 10)), 31);	// bar at the top of cook timer

				// cook meter
				x += 120;
				RenderSkillBox(x, y-1, x+20, y+121, 31, 0);
				byte c[] = { 4,5,1,6,3,7 };	// colors for the flames
				int hgt = 120 / 6;
				byte tempY = temp * 120 / (6*40);
				byte boil, simmer;
				GetSwampTemps(&boil, &simmer);
				for (int i = 0; i < 6; i++)
				{
					int yy = y+120-hgt*i;
					if (temp >= i * 40 && temp < (i + 1) * 40)
						DrawFillBox(x + 1, yy - hgt, x + 19, yy, c[i] * 32 + 20);
					else
						DrawFillBox(x + 1, yy - hgt, x + 19, yy, c[i] * 32 + 8);
					if (boil == i)
						PrintGlow(x + 25, yy - hgt, "B", 2);
					if (simmer == i)
						PrintGlow(x + 25, yy - hgt, "S", 2);
				}
				DrawFillBox(x - 2, y + 120 - tempY, x + 22, y + 120 - tempY, 31);

			}
			else if (state == 3)	// finished!
			{
				int y = HALFHEI - 50;
				RenderSkillBox(SCRWID - 60, y, SCRWID + 4, y+114, 32 * 5 + 16, 32 * 5 + 4);
				CenterPrint(SCRWID - 30, y+5, "Quality", 0, 1);
				if (score < 60)
					sprintf(s, "F");
				else if (score < 70)
					sprintf(s, "D");
				else if (score < 80)
					sprintf(s, "C");
				else if (score < 90)
					sprintf(s, "B");
				else if (score < 100)
					sprintf(s, "A");
				else
					sprintf(s, "S!");
				CenterPrint(SCRWID - 30+2, y+35+2, s, -31, 0);
				CenterPrint(SCRWID - 30, y+35, s, 0, 0);

				sprintf(s, "%02d%%", score);
				CenterPrint(SCRWID - 30+2, y+75+2, s, -31, 2);
				CenterPrint(SCRWID - 30, y+75, s, 0, 2);
			}
			// state=1 means no display
		}
	}
	RenderAchieves();
}

sprite_t *GetIntfaceSpr(byte spr)
{
	return intfaceSpr->GetSprite(spr);
}
