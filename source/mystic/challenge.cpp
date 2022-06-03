#include "challenge.h"
#include "shop.h"
#include "display.h"
#include "game.h"
#include "fairy.h"
#include "shop.h"
#include "guy.h"
#include "appdata.h"

#define ASK_RESETCHAR	1
#define ASK_RESETSTAR	2
#define ASK_PLAY		3
#define ASK_BUY			4
#define ASK_TRIVIA		5

challenge_t chal[]={
	{255,1,0,"Item Shop",0,
		{{GOAL_FINISH,0},
		{GOAL_FINISH,0},
		{GOAL_FINISH,0},
		{GOAL_FINISH,0},
		{GOAL_FINISH,0}},
	},
	{255,2,0,"Fairy Box",0,
		{{GOAL_FINISH,0},
		{GOAL_FINISH,0},
		{GOAL_FINISH,0},
		{GOAL_FINISH,0},
		{GOAL_FINISH,0}},
	},

	// chapter 1
	{0,3,0,"Grassy Knoll",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,300},
		{GOAL_MYSTIC,30*5},
		{GOAL_NOHIT,0},
		{GOAL_FINISH2,30*60+30*30}},
	},

	{0,2,0,"Mushy Hollow",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,1000},
		{GOAL_CRYSTALS,0},
		{GOAL_NOSHOOT,0},
		{GOAL_COMBO,10}},
	},

	{0,4,0,"Just A Puddle",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,300},
		{GOAL_NOHIT,0},
		{GOAL_COMBO,5},
		{GOAL_CRYSTALS,0}},
	},

	{0,15,0,"The Docks",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,400},
		{GOAL_MYSTIC,30*4},
		{GOAL_FINISH2,30*60+30*30},
		{GOAL_NOHIT,0}},
	},

	{0,16,0,"Plants Gone Wild",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2500},
		{GOAL_MYSTIC,30*3},
		{GOAL_KILL,0},
		{GOAL_COMBO,20}},
	},

	{0,17,0,"Trick Shot Chlg",30*60*4,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,450},
		{GOAL_CRYSTALS,0},
		{GOAL_FINISH2,30*60*3},
		{GOAL_NOSHOOT,0}},
	},

	{0,5,5,"The Grasslands",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,1000},
		{GOAL_KILLSOME,MONS_EGGSAC},
		{GOAL_CRYSTALS,0},
		{GOAL_FINISH2,30*60*2}},
	},

	{0,14,10,"The Bridge",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2500},
		{GOAL_KILLSOME,MONS_OCTOBOSS},
		{GOAL_FINISH2,30*60*2+30*30},
		{GOAL_COMBO,15}},
	},

	//--------------------------------
	{11,1,20,"Chapter 2",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	{1,2,0,"Creepy Creek",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_MYSTIC,30*3+15},
		{GOAL_KILLSOME,MONS_SERPENT},
		{GOAL_CRYSTALS,0}},
	},

	{1,3,0,"Mush-Aid",30*60*6,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,10000},
		{GOAL_CRYSTALS,0},
		{GOAL_MYSTIC,30*8},
		{GOAL_COMBO,40}},
	},

	{1,5,0,"Abandoned Village",30*60*4,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_FINISH2,30*2*60+30*30},
		{GOAL_MYSTIC,30*1+15},
		{GOAL_COMBO,10}},
	},

	{1,6,0,"Spider's Lair",30*60*6,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,10000},
		{GOAL_MYSTIC,30*6},
		{GOAL_FINISH2,30*60*5},
		{GOAL_NOSHOOT,0}},
	},

	{1,11,0,"Among The Hedges",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_FINISH2,30*60*4},
		{GOAL_KILL,0},
		{GOAL_NOSHOOT,0}},
	},

	{1,13,0,"Hidden Temple",30*60*4,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,1000},
		{GOAL_COMBO,20},
		{GOAL_NOSPELL,0},
		{GOAL_SCORE,3000}},
	},

	{1,14,5,"Lost In The Woods",30*60*10,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_MYSTIC,30*30},
		{GOAL_CRYSTALS,0},
		{GOAL_SCORE,8000}},
	},

	{1,15,5,"River Rafting",30*60*1,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,300},
		{GOAL_MYSTIC,30*7},
		{GOAL_KILLSOME,MONS_SERPENT},
		{GOAL_CRYSTALS,0}},
	},

	{1,16,5,"Temple Of Kings",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_CRYSTALS,0},
		{GOAL_NOKILL,MONS_INCABOT},
		{GOAL_NOSHOOT,0}},
	},

	{1,4,10,"Hidden Clearing",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,3000},
		{GOAL_FINISH2,30*60*2+30*55},
		{GOAL_MYSTIC,30*1+15},
		{GOAL_COMBO,30}},
	},

	{1,12,10,"Spooky Pass",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,500},
		{GOAL_COMBO,3},
		{GOAL_CRYSTALS,0},
		{GOAL_FINISH2,30*60*2}},
	},

	//--------------------------------
	{12,1,20,"Chapter 3",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	{2,6,0,"The Gatehouse",30*60*4,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,5000},
		{GOAL_COMBO,15},
		{GOAL_NOHIT,0},
		{GOAL_NOKILL,MONS_PEEPER}},
	},

	{2,9,0,"The Library",30*60*6,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,12000},
		{GOAL_MYSTIC,30*10},
		{GOAL_CRYSTALS,0},
		{GOAL_KILL,0}},
	},

	{2,11,0,"The Ballroom",30*60*6,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,50000},
		{GOAL_KILL,0},
		{GOAL_COMBO,100},
		{GOAL_FINISH2,30*60*5+30*30}},
	},

	{2,13,0,"Guest Chambers",30*60*10,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,5000},
		{GOAL_MYSTIC,25},
		{GOAL_CRYSTALS,0},
		{GOAL_NOSPELL,0}},
	},

	{2,16,0,"The Vault",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,500},
		{GOAL_FINISH2,30*60*2+30*45},
		{GOAL_SCORE,1000},
		{GOAL_MYSTIC,30*2}},
	},

	{2,18,0,"Violent Eyes",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,700},
		{GOAL_MYSTIC,30*3},
		{GOAL_CRYSTALS,0},
		{GOAL_NOSPELL,0}},
	},

	{2,17,5,"Twisty Passages",30*60*15,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,3000},
		{GOAL_CRYSTALS,0},
		{GOAL_COMBO,5},
		{GOAL_KILL,0}},
	},

	{2,21,5,"Guardian Battle",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_COMBO,2},
		{GOAL_NOHIT,0},
		{GOAL_CRYSTALS,0}},
	},

	{2,19,5,"Whack-A-Zoid",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,20000},
		{GOAL_CRYSTALS,0},
		{GOAL_COMBO,40},
		{GOAL_NOSHOOT,0}},
	},

	{2,20,5,"Castle Sewers",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,10000},
		{GOAL_COMBO,25},
		{GOAL_MYSTIC,30*10},
		{GOAL_KILL,0}},
	},

	{2,7,10,"Off The Deep End",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,5000},
		{GOAL_COMBO,10},
		{GOAL_CRYSTALS,0},
		{GOAL_NOHIT,0}},
	},

	{2,15,10,"The Keep",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,15000},
		{GOAL_COMBO,30},
		{GOAL_NOSHOOT,0},
		{GOAL_NOSPELL,0}},
	},

	//--------------------------------
	{13,1,20,"Chapter 4",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	{3,6,0,"Toasty Caverns",30*60*10,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,10000},
		{GOAL_NOSPELL,0},
		{GOAL_CRYSTALS,0},
		{GOAL_KILL,0}},
	},

	{3,7,0,"The Mines",30*60*8,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,200000},
		{GOAL_COMBO,200},
		{GOAL_MYSTIC,30*2},
		{GOAL_KILLSOME,MONS_MOSS}},
	},

	{3,15,0,"Gastro-Pods",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,2000},
		{GOAL_CRYSTALS,0},
		{GOAL_KILL,0},
		{GOAL_FINISH2,30*60*3+30*30}},
	},

	{3,9,0,"Obnoxiousness",30*60*6,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,10000},
		{GOAL_COMBO,30},
		{GOAL_FINISH2,30*60*5},
		{GOAL_MYSTIC,30*4}},
	},

	{3,10,0,"Bats! Bats! Bats!",30*60*10,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,30000},
		{GOAL_COMBO,50},
		{GOAL_NOSPELL,0},
		{GOAL_KILLSOME,MONS_BIGBAT}},
	},

	{3,12,5,"Red River Rafting",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,1200},
		{GOAL_FINISH2,30*60*4},
		{GOAL_CRYSTALS,0},
		{GOAL_KILL,0}},
	},

	{3,13,5,"Smashball",30*60*10,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,100},
		{GOAL_KILL,0},
		{GOAL_FINISH2,30*60*9+30*30},
		{GOAL_MYSTIC,30}},
	},

	{3,14,5,"Puzzling Caverns",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,3000},
		{GOAL_KILLSOME,MONS_SNAIL},
		{GOAL_MYSTIC,30},
		{GOAL_CRYSTALS,0}},
	},

	{3,8,10,"Armageddon Shrine",30*60*2,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,1500},
		{GOAL_FINISH2,30*60*1+30*53},
		{GOAL_NOHIT,0},
		{GOAL_NOSPELL,0}},
	},

	{3,11,15,"Bobby Khan",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,5000},
		{GOAL_FINISH2,30*60},
		{GOAL_NOSHOOT,0},
		{GOAL_CRYSTALS,0}},
	},

	// bonus levels --------------------------------------------------------------------------
	{14,1,15,"Bonus Levels",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	{4,19,0,"Ker-Splat!",30*60*3,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,10000},
		{GOAL_KILL,0},
		{GOAL_NOSPELL,0},
		{GOAL_GETALL,0}},
	},

	{4,18,0,"Stick Stream",30*60*1,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,500},
		{GOAL_KILL,0},
		{GOAL_FINISH2,30*52},
		{GOAL_COMBO,4}},
	},

	{5,18,0,"Forging A Head",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,5000000},
		{GOAL_COMBO,999},
		{GOAL_NOKILL,MONS_INCABOSS},
		{GOAL_GETALL,0}},
	},

	{5,17,0,"Sticky Webs",30*60*1,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,700},
		{GOAL_NOSHOOT,0},
		{GOAL_FINISH2,30*50},
		{GOAL_COMBO,5}},
	},

	{6,23,0,"Jeepers Peepers",30*60*1,
		{{GOAL_FINISH,0},
		{GOAL_NOHIT,0},
		{GOAL_MYSTIC,30*10},
		{GOAL_CRYSTALS,0},
		{GOAL_GETALL,0}},
	},

	{6,22,0,"Stick Graveyard",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,5000},
		{GOAL_CRYSTALS,0},
		{GOAL_NOSPELL,0},
		{GOAL_FINISH2,30*60}},
	},

	{7,17,0,"Snailbait",30*60*5,
		{{GOAL_FINISH,0},
		{GOAL_NOHIT,0},
		{GOAL_NOKILL,255},
		{GOAL_MYSTIC,30*4},
		{GOAL_GETALL,0}},
	},

	{7,16,0,"Subterranean Sticks",30*60*7,
		{{GOAL_FINISH,0},
		{GOAL_SCORE,6000},
		{GOAL_MYSTIC,30*8},
		{GOAL_NOSHOOT,0},
		{GOAL_FINISH2,30*60*6}},
	},

	{15,1,40,"Super Bonus!!",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	//--------------------------------
	{255,3,0,"Reset Character",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	{255,4,0,"Reset Goals",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},

	{255,0,0,"Exit",0,
		{{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0},
		{GOAL_NONE,0}},
	},
};

static byte oldc,chalCursor,numChals,fairyOn,challenging=0;
chalData_t chalData;
attempt_t attempt;
static sprite_set_t *chalSpr;
static byte *backgd;
static int offY;
static byte qCursor,asking,qLines;
static char question[10][48];
static byte percent;
static byte flipper=0;
static byte forbidden[8];
static byte nextChapter;
static byte goalsDoneThisTime;

void PickForbidden(void)
{
	int i,j;

	nextChapter=11;
	for(i=1;i<9;i++)
	{
		forbidden[i-1]=i;

		for(j=0;j<numChals;j++)
			if((chal[j].chapter==10+i || (chal[j].chapter==14 && i>4)) && chalData.bought[j])	// if you have bought the chapter notice for this chapter
			{
				nextChapter=chal[j].chapter+1;
				forbidden[i-1]=20;
			}
	}

	for(i=0;i<numChals;i++)
		if(chal[i].chapter==15)	// super bonus
		{
			if(chalData.bought[i]==0)
				strcpy(chal[i].name,"Super Bonus!!");
			else
				strcpy(chal[i].name,"Trivial Fun!");
		}
}

char *GoalText(byte goal,dword n)
{
	static char s[64];

	switch(goal)
	{
		case GOAL_NONE:
			s[0]='\0';
			break;
		case GOAL_FINISH:
			sprintf(s,"Complete the level!");
			break;
		case GOAL_FINISH2:
			sprintf(s,"Finish with %d:%02d remaining!",(n/(30*60)),(n/30)%60);
			break;
		case GOAL_KILL:
			sprintf(s,"Beat all badguys!");
			break;
		case GOAL_KILLSOME:
			if(n==MONS_MOSS)
				sprintf(s,"Defeat all %s!",MonsterName((byte)n));
			else
				sprintf(s,"Defeat all %ss!",MonsterName((byte)n));
			break;
		case GOAL_KILLBUT:
			sprintf(s,"Defeat all but %s monsters!",MonsterName((byte)n));
			break;
		case GOAL_SCORE:
			sprintf(s,"Earn %d points!",n);
			break;
		case GOAL_MYSTIC:
			sprintf(s,"Spell M-Y-S-T-I-C!");
			break;
		case GOAL_NOSHOOT:
			sprintf(s,"Finish without using fireballs!");
			break;
		case GOAL_NOSPELL:
			sprintf(s,"Finish without using spells!");
			break;
		case GOAL_NOHIT:
			sprintf(s,"Finish without getting hit!");
			break;
		case GOAL_COMBO:
			sprintf(s,"Achieve a %d combo!",n);
			break;
		case GOAL_CRYSTALS:
			sprintf(s,"Get all Challenge Crystals!");
			break;
		case GOAL_NOKILL:
			if(n==255)
			{
				sprintf(s,"Don't hurt anybody!");
			}
			else
			{
				if(n==MONS_INCABOSS)
					sprintf(s,"Don't hurt %s!",MonsterName((byte)n));
				else if(s[strlen(s)-1]=='s')
					sprintf(s,"Don't hurt any %s!",MonsterName((byte)n));
				else
					sprintf(s,"Don't hurt any %ss!",MonsterName((byte)n));
			}
			break;
		case GOAL_GETALL:
			sprintf(s,"All goals in one run!");
			break;
	}
	return s;
}

void ResetChallengeCharacter(void)
{
	InitPlayer(INIT_GAME,0,0);
	player.gotSpell[0]=1;
	player.spell[0]=1;
	player.haveFairy=FAIRY_GRABBY;
	player.templePuzzleDone=1;
}

void CalcChallengePercent(void)
{
	int i,n;

	n=0;
	for(i=0;i<numChals;i++)
		if(chal[i].chapter>=0 && chal[i].chapter<=7)
			n++;

	percent=(byte)(chalData.totalStars*100/(n*5));
}

void ResetChallengeStats(void)
{
	int i,j;

	for(i=0;i<64;i++)
	{
		chalData.topScore[i]=0;
		chalData.topTime[i]=0;
		chalData.topCombo[i]=0;
		if(i<numChals && chal[i].starCost==0 && (chal[i].chapter==0 || chal[i].chapter==255))
			chalData.bought[i]=1;
		else
			chalData.bought[i]=0;
		for(j=0;j<5;j++)
			chalData.goal[i][j]=0;
	}
	chalData.stars=0;
	chalData.totalStars=0;
	CalcChallengePercent();
	PickForbidden();
}

void RenderQuestionBox(MGLDraw *mgl)
{
	int i;

	chalSpr->GetSprite(4)->Draw(110,150,mgl);

	for(i=0;i<qLines;i++)
	{
		if(question[i][0]=='^')
		{
			CenterPrintGlow(152+i*20,&question[i][1],2);
			mgl->FillBox(320-200,152+i*20+10,320+200,152+i*20+10,31);
		}
		else
			CenterPrintGlow(152+i*20,question[i],2);
	}

	PrintGlow(180,360,"OKAY",2);
	PrintGlow(400,360,"CANCEL",2);
	if(qCursor==0)
	{
		chalSpr->GetSprite(2)->DrawColored(165,376,mgl,1+4*flipper,0);
		chalSpr->GetSprite(2)->DrawColored(255,376,mgl,1+4*flipper,0);
	}
	else
	{
		chalSpr->GetSprite(2)->DrawColored(385,376,mgl,1+4*flipper,0);
		chalSpr->GetSprite(2)->DrawColored(490,376,mgl,1+4*flipper,0);
	}
}

byte MoveCursor(char dir,byte curs)
{
	byte i,c;

	if(curs==0 && dir==-1)
		curs=numChals-1;
	else if(curs==numChals-1 && dir==1)
		curs=0;
	else
		curs+=dir;

	while(1)
	{
		c=0;
		for(i=0;i<8;i++)
			if(chal[curs].chapter==forbidden[i])
				c++;	// skip over levels from forbidden chapters

		if(chal[curs].chapter>10 && chal[curs].chapter<20 && chal[curs].chapter!=15 && (chalData.bought[curs] || chal[curs].chapter>nextChapter))
			c++;	// also skip over chapter headers for chapters you've bought or are too advanced

		if(chal[curs].chapter==15)	// skip the Super Bonus unless you've bought EVERYTHING else
		{
			for(i=0;i<curs;i++)
				if(!chalData.bought[i])
				{
					c++;
					break;
				}
		}

		if(!c)
			break;	// if you didn't hit any forbidden ones, this is okay
		if(curs==0 && dir==-1)
			curs=numChals-1;
		else if(curs==numChals-1 && dir==1)
			curs=0;
		else
			curs+=dir;
	}

	return curs;
}

void ChallengeMenuRender(MGLDraw *mgl)
{
	int i,j,k,x,y;
	char s[32];

	if(fairyOn==2)
	{
		RenderShop();
	}
	else
	{
		memcpy(mgl->GetScreen(),backgd,640*480);
		RenderPlayerGear(450,0);

		sprintf(s,"%d",chalData.stars);
		chalSpr->GetSprite(2)->Draw(620,20,mgl);
		RightPrintGlow(600,2,s,2);
		sprintf(s,"%d%% Complete",percent);
		PrintGlow(365,2,s,2);

		sprintf(s,"Level %d",player.level);
		PrintGlow(365,22,s,2);

		y=30+offY;
		j=MoveCursor(-1,chalCursor);
		j=MoveCursor(-1,j);
		j=MoveCursor(-1,j);
		j=MoveCursor(-1,j);

		for(i=chalCursor-4;i<=chalCursor+4;i++)
		{
			x=35-(abs(y-240)*abs(y-240))/128;
			if(j==chalCursor)
			{
				//chalSpr->GetSprite(1)->Draw(x,15+y,mgl);

				chalSpr->GetSprite(1)->Draw(x-4,11+y,mgl);
				chalSpr->GetSprite(1)->Draw(x+4,19+y,mgl);

			}
			//else
			{
				if(chal[j].chapter==255)
					chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,5,-20+(chalCursor==j)*15);
				else if(!chalData.bought[j])
					chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,4,-20+(chalCursor==j)*15);
				else
				{
					if((chal[j].chapter%10)==0)	// blue for ch 1
						chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,3,-20+(chalCursor==j)*15);
					else if((chal[j].chapter%10)==1)	// green for ch 2
						chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,1,-20+(chalCursor==j)*15);
					else if((chal[j].chapter%10)==2)	// grey for ch 3
						chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,0,-20+(chalCursor==j)*15);
					else if((chal[j].chapter%10)==3)	// brown for ch 4
						chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,2,-20+(chalCursor==j)*15);
					else	// bonus levels are purple
						chalSpr->GetSprite(1)->DrawColored(x,15+y,mgl,6,-20+(chalCursor==j)*15);
				}
			}

			Print(x-15-1,3+y-1,chal[j].name,-32,2);
			Print(x-15+1,3+y+1,chal[j].name,-32,2);
			PrintGlow(x-15,3+y,chal[j].name,2);
			if(chal[j].chapter!=255 && (chal[j].chapter!=15 || chalData.bought[j]==0))
			{
				if(chalData.bought[j])
				{
					for(k=0;k<5;k++)
					{
						chalSpr->GetSprite(2+(chalData.goal[j][k]==0))->Draw(x+200+k*25,y+20,mgl);
					}
				}
				else
				{
					chalSpr->GetSprite(2)->DrawColored(x+200,y+20,mgl,4,0);
					PrintGlow(x+230,y-12,"Buy for",2);
					sprintf(s,"%d Stars",chal[j].starCost);
					PrintGlow(x+230,y+8,s,2);
				}
			}
			else if(chal[j].chapter==15)
			{
				// show a star indicating if Trivia mode is on
				chalSpr->GetSprite(2+(chalData.bought[63]==0))->Draw(x+200+3*25,y+20,mgl);
			}

			y+=50;
			j=MoveCursor(1,j);
		}
		chalSpr->GetSprite(0)->Draw(0,0,mgl);
		chalSpr->GetSprite(0)->Draw(0,387,mgl);

		Print(62,2,"Challenge",0,0);
		Print(112,40,"Mode",0,0);

		chalSpr->GetSprite(2)->Draw(40,27,mgl);
		chalSpr->GetSprite(2)->Draw(93,65,mgl);

		chalSpr->GetSprite(2)->Draw(308,27,mgl);
		chalSpr->GetSprite(2)->Draw(255,65,mgl);

		// display data about this challenge
		if(chal[chalCursor].chapter!=255)
		{
			if(chalData.bought[chalCursor])
			{
				PrintBrightGlow(5,380,chal[chalCursor].name,-4,2);

				sprintf(s,"Top Score: %05d",chalData.topScore[chalCursor]);
				PrintBrightGlow(5,400,s,-4,2);
				i=chal[chalCursor].time-chalData.topTime[chalCursor];
				sprintf(s,"Top Time: %d:%02d",(i/(60*30)),(i/30)%60);
				PrintBrightGlow(5,420,s,-4,2);
				sprintf(s,"Top Combo: %d",chalData.topCombo[chalCursor]);
				PrintBrightGlow(5,440,s,-4,2);
			}
			else
			{
				if(chal[chalCursor].chapter<10)
				{
					PrintBrightGlow(5,380,chal[chalCursor].name,-4,2);

					PrintBrightGlow(5,400,"You can buy access to this",-4,2);
					sprintf(s,"level for %d stars.",chal[chalCursor].starCost);
					PrintBrightGlow(5,420,s,-4,2);
				}
				else
				{
					PrintBrightGlow(5,380,chal[chalCursor].name,-4,2);
					PrintBrightGlow(5,400,"You can buy access to this",-4,2);
					sprintf(s,"chapter for %d stars.",chal[chalCursor].starCost);
					PrintBrightGlow(5,420,s,-4,2);
				}
			}
		}
		else
		{
			if(chal[chalCursor].level==0)	// exit
			{
				PrintBrightGlow(5,380,"Exit",-4,2);
				PrintBrightGlow(5,400,"Get back to the main menu!",-4,2);
			}
			else if(chal[chalCursor].level==1)	// item shop
			{
				PrintBrightGlow(5,380,"Item Shop",-4,2);
				PrintBrightGlow(5,400,"Buy stuff to get stronger!",-4,2);
			}
			else if(chal[chalCursor].level==2)	// fairy box
			{
				PrintBrightGlow(5,380,"Fairy Box",-4,2);
				PrintBrightGlow(5,400,"Select a fairy to assist you!",-4,2);
			}
			else if(chal[chalCursor].level==3)	// reset char
			{
				PrintBrightGlow(5,380,"Reset Character",-4,2);
				PrintBrightGlow(5,400,"Put your character back to",-4,2);
				PrintBrightGlow(5,420,"level 1 with no gear.",-4,2);
			}
			else if(chal[chalCursor].level==4)	// reset goals
			{
				PrintBrightGlow(5,380,"Reset Goals",-4,2);
				PrintBrightGlow(5,400,"Clear all completed goals,",-4,2);
				PrintBrightGlow(5,420,"stars, and purchased levels.",-4,2);
			}

		}

		if(fairyOn==1)
			RenderFairyBox();
		if(fairyOn==3)	// question box
			RenderQuestionBox(mgl);
	}
}

byte DoQuestion(void)
{
	int i,j;

	if(chal[chalCursor].chapter==255)
	{
		// special questions
		if(chal[chalCursor].level==2)	// fairy box
		{
			EnterFairyBox();
			fairyOn=1;
			return 0;
		}
		else if(chal[chalCursor].level==1)	// shop
		{
			EnterShop();
			fairyOn=2;
			return 0;
		}
		else if(chal[chalCursor].level==0)	// exit
		{
			return 1;
		}
		else if(chal[chalCursor].level==3)	// reset character
		{
			fairyOn=3;
			oldc=255;
			strcpy(question[0],"Do you really want to");
			strcpy(question[1],"reset your Challenge Mode");
			strcpy(question[2],"character to level 1 with");
			strcpy(question[3],"no items or spells?");
			qLines=4;
			asking=ASK_RESETCHAR;
			qCursor=1;	// default to no
			return 0;
		}
		else if(chal[chalCursor].level==4)	// reset stats
		{
			fairyOn=3;
			oldc=255;
			strcpy(question[0],"Do you really want to");
			strcpy(question[1],"reset all Challenge Mode");
			strcpy(question[2],"stars and purchased");
			strcpy(question[3],"levels?");
			qLines=4;
			asking=ASK_RESETSTAR;
			qCursor=1;	// default to no
			return 0;
		}
	}
	else if(chal[chalCursor].chapter==15 && chalData.bought[chalCursor])
	{
		fairyOn=3;
		oldc=255;
		strcpy(question[0],"Hit Okay to turn on Trivia");
		strcpy(question[1],"mode, or Cancel to disable it.");
		strcpy(question[2],"Then play any level to see the");
		strcpy(question[3],"trivial fun!");
		qLines=4;
		asking=ASK_TRIVIA;
		qCursor=0;	// default to yes
		return 0;
	}
	else
	{
		if(chalData.bought[chalCursor])
		{
			byte goalCount;
			fairyOn=3;
			oldc=255;
			sprintf(question[0],"%s (%d:%02d)",chal[chalCursor].name,(chal[chalCursor].time/(60*30)),(chal[chalCursor].time/30)%60);
			strcpy(question[1],"");
			j=2;
			goalCount=0;
			for(i=0;i<5;i++)
				if(!chalData.goal[chalCursor][i])
				{
					goalCount++;
					sprintf(question[j++],"* %s",GoalText(chal[chalCursor].goal[i].type,chal[chalCursor].goal[i].n));
				}
				else
				{
					sprintf(question[j++],"^* %s",GoalText(chal[chalCursor].goal[i].type,chal[chalCursor].goal[i].n));
				}
			if(goalCount==0)
			{
				strcpy(question[j++],"");
				strcpy(question[j++],"You've completed all goals!");
			}
			else
			{
				strcpy(question[j++],"");
				if(goalCount>1)
					sprintf(question[j++],"You have %d goals to go!",goalCount);
				else
					sprintf(question[j++],"You have %d goal to go!",goalCount);
			}
			qLines=j;
			asking=ASK_PLAY;
			qCursor=0;	// default to yes
			return 0;
		}
		else if(chalData.stars>=chal[chalCursor].starCost)
		{
			if(chal[chalCursor].chapter<10)
			{
				fairyOn=3;
				oldc=255;
				strcpy(question[0],chal[chalCursor].name);
				strcpy(question[1],"");
				strcpy(question[2],"You can buy access to this");
				sprintf(question[3],"level for %d stars.",chal[chalCursor].starCost);
				qLines=4;
				asking=ASK_BUY;
				qCursor=0;	// default to yes
				return 0;
			}
			else if(chal[chalCursor].chapter<=nextChapter)	// this is a chapter header
			{
				fairyOn=3;
				oldc=255;
				strcpy(question[0],chal[chalCursor].name);
				strcpy(question[1],"");
				strcpy(question[2],"You can buy access to this");
				if(chal[chalCursor].chapter<14)
					sprintf(question[3],"chapter for %d stars.",chal[chalCursor].starCost);
				else
					sprintf(question[3],"bonus for %d stars.",chal[chalCursor].starCost);
				qLines=4;
				asking=ASK_BUY;
				qCursor=0;	// default to yes
				return 0;
			}
			else	// a chapter you can afford, but aren't allowed to buy yet
			{
				MakeNormalSound(SND_UNAVAILABLE);
				oldc=255;
				return 0;
			}
		}
		else
		{
			MakeNormalSound(SND_UNAVAILABLE);
			oldc=255;
			return 0;
		}
	}
	return 0;
}

byte ChallengeMenuUpdate(MGLDraw *mgl,int *lastTime)
{
	byte c;
	static byte reptCounter=0;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(offY>0)
		{
			offY-=4;
			if(offY<0)
				offY=0;
		}
		if(offY<0)
		{
			offY+=4;
			if(offY>0)
				offY=0;
		}
		if(fairyOn==1)
		{
			if(UpdateFairyBox(mgl))
			{
				fairyOn=0;
				oldc=255;
				mgl->LastKeyPressed();
			}
		}
		else if(fairyOn==2)	// shop
		{
			if(UpdateShop(mgl))
			{
				fairyOn=0;
				oldc=255;
				mgl->LastKeyPressed();
			}
		}
		else if(fairyOn==3)	// question
		{
			c=GetControls();

			reptCounter++;
			if((!oldc) || (reptCounter>10))
				reptCounter=0;

			if((c&CONTROL_LF) && (!reptCounter))
			{
				qCursor=1-qCursor;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_RT) && (!reptCounter))
			{
				qCursor=1-qCursor;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))))
			{
				MakeNormalSound(SND_MENUSELECT);
				if(qCursor==1)
				{
					fairyOn=0;
					oldc=255;
					mgl->LastKeyPressed();
					if(asking==ASK_TRIVIA)
						chalData.bought[63]=0;
					return 0;
				}
				switch(asking)
				{
					case ASK_PLAY:
						fairyOn=0;
						oldc=255;
						return 1;
						break;
					case ASK_BUY:
						chalData.stars-=chal[chalCursor].starCost;
						chalData.bought[chalCursor]=1;
						if(chal[chalCursor].chapter>10)
						{
							PickForbidden();
							for(c=0;c<numChals;c++)
								if((chal[c].chapter==chal[chalCursor].chapter-10 ||
									(chal[chalCursor].chapter==14 && chal[c].chapter>3))	&& chal[c].starCost==0)
									chalData.bought[c]=1;

							chalCursor=MoveCursor(1,chalCursor);
						}
						fairyOn=0;
						MakeNormalSound(SND_PURCHASE);
						oldc=255;
						return 0;
						break;
					case ASK_RESETCHAR:
						ResetChallengeCharacter();
						fairyOn=0;
						oldc=255;
						return 0;
						break;
					case ASK_RESETSTAR:
						ResetChallengeStats();
						fairyOn=0;
						oldc=255;
						return 0;
						break;
					case ASK_TRIVIA:
						chalData.bought[63]=1;
						fairyOn=0;
						oldc=255;
						return 0;
						break;
				}
			}
			if(mgl->LastKeyPressed()==27)
			{
				MakeNormalSound(SND_MENUSELECT);
				fairyOn=0;
				oldc=255;
				return 0;
			}
			oldc=c;
		}
		else
		{
			c=GetControls();

			reptCounter++;
			if((!oldc) || (reptCounter>10))
				reptCounter=0;

			if((c&CONTROL_UP) && (!reptCounter))
			{
				chalCursor=MoveCursor(-1,chalCursor);
				offY=-40;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_DN) && (!reptCounter))
			{
				chalCursor=MoveCursor(1,chalCursor);
				offY=40;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_LF) && (!reptCounter))
			{
				for(c=0;c<5;c++)
					chalCursor=MoveCursor(-1,chalCursor);
				offY=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if((c&CONTROL_RT) && (!reptCounter))
			{
				for(c=0;c<5;c++)
					chalCursor=MoveCursor(1,chalCursor);
				offY=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
			{
				EnterFairyBox();
				fairyOn=1;
				oldc=255;
				return 0;
			}

			if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))))
			{
				MakeNormalSound(SND_MENUSELECT);
				return DoQuestion();
			}
			oldc=c;
		}
		flipper=1-flipper;

		*lastTime-=TIME_PER_FRAME;
	}

	if(mgl->LastKeyPressed()==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		if(fairyOn)
		{
			fairyOn=0;
		}
		else
		{
			chalCursor=numChals-1;
			return 1;
		}
	}

	JamulSoundUpdate();
	return 0;
}

void LoadChallenge(void)
{
	FILE *f;

	f=AppdataOpen("challenge.sav");
	if(!f)
	{
		ResetChallengeStats();
		ResetChallengeCharacter();
	}
	else
	{
		fread(&chalData,1,sizeof(chalData_t),f);
		fclose(f);
		memcpy(&player,&chalData.player,sizeof(player_t));
	}
}

void SaveChallenge(void)
{
	FILE *f;

	f=AppdataOpen_Write("challenge.sav");
	if(f)
	{
		memcpy(&chalData.player,&player,sizeof(player_t));
		fwrite(&chalData,1,sizeof(chalData_t),f);
		fclose(f);
		AppdataSync();
	}
}

void InitChallengeMenu(MGLDraw *mgl)
{
	int i;

	GetDisplayMGL()->LoadBMP("graphics/shop.bmp");
	backgd=(byte *)malloc(640*480);
	if(!backgd)
		FatalError("Out of memory!!");
	memcpy(backgd,GetDisplayMGL()->GetScreen(),640*480);

	chalSpr=new sprite_set_t("graphics/chal.jsp");
	i=0;
	while((chal[i].chapter!=255 || chal[i].level!=0))
		i++;
	numChals=i+1;

	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;
	chalCursor=0;
	LoadChallenge();
	fairyOn=0;
	player.life=player.maxLife;
	player.mana=player.maxMana;
	challenging=1;
	offY=0;

	PickForbidden();
	CalcChallengePercent();
}

void ExitChallengeMenu(void)
{
	challenging=0;
	free(backgd);
	delete chalSpr;
	SaveChallenge();
}

void InitAttempt(void)
{
	int i;

	attempt.score=0;
	attempt.kills=0;
	attempt.time=chal[chalCursor].time;
	attempt.comboClock=0;
	attempt.curCombo=0;
	attempt.curLetter=0;
	attempt.letterClock=30*5;
	attempt.bestCombo=0;
	for(i=0;i<5;i++)
	{
		attempt.goalOk[i]=1;
		if(chal[chalCursor].goal[i].type==GOAL_MYSTIC)
			attempt.letterMaxClock=chal[chalCursor].goal[i].n;
	}
	attempt.quit=0;
}

TASK(byte) ChallengeMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	InitChallengeMenu(mgl);
	StopSong();
	if(CurrentSong()!=SONG_SHOP)
		PlaySong(SONG_SHOP);
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=ChallengeMenuUpdate(mgl,&lastTime);
		ChallengeMenuRender(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN 0;
		EndClock();
		if(b==1)
		{
			if(chal[chalCursor].chapter==255)
			{
				if(chal[chalCursor].level==0)	// exit
					break;
			}
			else
			{
				InitAttempt();
				if(chal[chalCursor].chapter<4)
					AWAIT ChallengePlay(chal[chalCursor].chapter,chal[chalCursor].level);
				else
					AWAIT ChallengePlay(chal[chalCursor].chapter-4,chal[chalCursor].level);
				if(CurrentSong()!=SONG_SHOP)
					PlaySong(SONG_SHOP);

				player.life=player.maxLife;
				player.mana=player.maxMana;
				oldc=255;
				mgl->LastKeyPressed();
				b=0;
				if(attempt.quit)
					b=1;
				CalcChallengePercent();
			}
		}
	}
	ExitChallengeMenu();
	CO_RETURN 0;
}

byte Challenging(void)
{
	return challenging;
}

byte GoalNumber(byte type)
{
	int i;

	for(i=0;i<5;i++)
		if(chal[chalCursor].goal[i].type==type)
			return (byte)i;
	return 255;
}

void ChallengeEvent(byte type,int n)
{
	byte g;

	if(!challenging)
		return;

	switch(type)
	{
		case CE_NONE:
			break;
		case CE_KILL:
			attempt.kills++;
			if(attempt.curCombo<999)
				attempt.curCombo++;
			if(attempt.curCombo>attempt.bestCombo)
				attempt.bestCombo=attempt.curCombo;
			attempt.comboClock=60;
			attempt.score+=attempt.curCombo*10;
			g=GoalNumber(GOAL_NOKILL);
			if(chal[chalCursor].goal[g].n==(dword)n || chal[chalCursor].goal[g].n==255)
				attempt.goalOk[g]=0;	// if you killed a forbidden guy, you are screwed
			if(GoalNumber(GOAL_KILL)!=255 && !MonsterExists(255))
			{
				FloaterParticles(GetGoodguy()->x,GetGoodguy()->y,7,320,-8,64);
				FloaterParticles(GetGoodguy()->x,GetGoodguy()->y,7,160,-4,64);
				FloaterParticles(GetGoodguy()->x,GetGoodguy()->y,7,80,-2,64);
				FloaterParticles(GetGoodguy()->x,GetGoodguy()->y,7,40,-1,64);
				NewMessage("Monsters cleared!",60);
			}
			break;
		case CE_SHOOT:
			attempt.score-=n;
			g=GoalNumber(GOAL_NOSHOOT);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get No Shoot goal complete
			break;
		case CE_SPELL:
			n/=2;
			if(n==0)
				n=1;
			attempt.score-=n;
			g=GoalNumber(GOAL_NOSPELL);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get No Spell goal complete
			break;
		case CE_OUCH:
			attempt.score-=n;
			g=GoalNumber(GOAL_NOHIT);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get No hit goal complete
			break;
		case CE_HIT:
			break;
		case CE_DIE:
			attempt.score-=500;
			g=GoalNumber(GOAL_FINISH);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get finish goal complete
			g=GoalNumber(GOAL_FINISH2);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get finish2 goal complete
			g=GoalNumber(GOAL_NOSHOOT);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get No shoot goal complete
			g=GoalNumber(GOAL_NOSPELL);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get No Spell goal complete
			g=GoalNumber(GOAL_NOHIT);
			if(g!=255)
				attempt.goalOk[g]=0;	// no longer possible to get No hit goal complete
			break;
		case CE_WIN:
			attempt.score+=500;
			break;
		case CE_GET:
			switch(n)
			{
				case ITM_COIN:
				case ITM_HAMMERUP:
				case ITM_PANTS:
				case ITM_REFLECT:
				case ITM_REVERSE:
					attempt.score++;
					break;
				case ITM_BIGCOIN:
					attempt.score+=10;
					break;
				case ITM_BRAIN:
				case ITM_CHLGCRYSTAL:
					attempt.score+=5;
					break;
				case ITM_LETTERM:
					attempt.curLetter++;
					attempt.letterClock=attempt.letterMaxClock;
					if(attempt.curLetter==6)
						MakeNormalSound(SND_FAIRYGET);
					break;
			}
			break;
		case CE_POINTS:
			attempt.score+=n;
			break;
		case CE_QUIT:
			attempt.quit=1;
			break;
	}
	if(attempt.score<0)
		attempt.score=0;
}

byte ChallengeUpdate(void)
{
	if(attempt.time==0)
	{
		return 0;
	}
	if(attempt.letterClock)
		attempt.letterClock--;
	if(!attempt.letterClock)
	{
		if(attempt.curLetter>0 && attempt.curLetter<6)
		{
			attempt.curLetter=0;
			MakeNormalSound(SND_UNAVAILABLE);
		}
		attempt.letterClock=attempt.letterMaxClock;
	}

	if(attempt.comboClock)
		attempt.comboClock--;
	if(!attempt.comboClock)
		attempt.curCombo=0;
	attempt.time--;
	if((attempt.time%30)==0 && attempt.score>0)
		attempt.score--;

	return 1;
}

void ChallengeRender(MGLDraw *mgl)
{
	char s[16];

	sprintf(s,"%d:%02d",(attempt.time/(30*60)),(attempt.time/30)%60);
	PrintGlow(45,32,s,2);
	sprintf(s,"%05d",attempt.score);
	PrintGlow(2,60,s,2);
	sprintf(s,"Combo x%d",attempt.curCombo);
	if(attempt.curCombo>1)
		PrintGlow(2,80,s,2);
}

void RenderAGoal(int x,int y,byte goal,dword n)
{
	Print(x,y,GoalText(goal,n),0,2);
}

byte GoalIsDone(goal_t *g,byte w)
{
	switch(g->type)
	{
		case GOAL_NONE:
			return 0;
			break;
		case GOAL_FINISH:
			if(attempt.goalOk[w])
				return 1;
			break;
		case GOAL_FINISH2:
			if(attempt.goalOk[w])
			{
				if(attempt.time>=g->n)
					return 1;
			}
			break;
		case GOAL_KILL:
			if(!MonsterExists(255))
				return 1;
			break;
		case GOAL_KILLSOME:
			if(!MonsterExists((byte)g->n))
				return 1;
			break;
		case GOAL_KILLBUT:
			if(!MonsterExistsExcept((byte)g->n))
				return 1;
			break;
		case GOAL_SCORE:
			if(attempt.score>=(int)g->n)
				return 1;
			break;
		case GOAL_MYSTIC:
			if(attempt.curLetter==6)
				return 1;
			break;
		case GOAL_NOSHOOT:
			if(attempt.goalOk[w])
				return 1;
			break;
		case GOAL_NOSPELL:
			if(attempt.goalOk[w])
				return 1;
			break;
		case GOAL_NOHIT:
			if(attempt.goalOk[w])
				return 1;
			break;
		case GOAL_COMBO:
			if(attempt.bestCombo>=g->n)
				return 1;
			break;
		case GOAL_CRYSTALS:
			if(GetChallengeCrystals()==0)
				return 1;
			break;
		case GOAL_NOKILL:
			if(attempt.goalOk[w] && attempt.goalOk[0])
				return 1;
			break;
		case GOAL_GETALL:
			if(goalsDoneThisTime==4)
				return 1;
			break;

	}
	return 0;
}

void CompleteGoals(void)
{
	int i;

	goalsDoneThisTime=0;
	for(i=0;i<5;i++)
	{
		if(GoalIsDone(&chal[chalCursor].goal[i],(byte)i))
		{
			goalsDoneThisTime++;
			if(chalData.goal[chalCursor][i]==0)
			{
				chalData.stars++;
				chalData.totalStars++;
				player.money+=10;
				if(player.money>50000)
					player.money=50000;
				chalData.goal[chalCursor][i]=1;
			}
		}
	}
}

// tally page
//--------------------------------------------------------------------------
void ChallengeTallyRender(MGLDraw *mgl)
{
	int i;
	char s[32];

	mgl->ClearScreen();

	RenderPlayerGear(450,0);
	i=0;
	CenterPrint(320,2,"Challenge Results",0,0);

	sprintf(s,"Time Left: %d:%02d",(attempt.time/(30*60)),(attempt.time/30)%60);
	Print(5,50,s,0,2);

	sprintf(s,"Score: %05d",attempt.score);
	Print(5,70,s,0,2);

	sprintf(s,"Monsters Slain: %d",attempt.kills);
	Print(5,90,s,0,2);

	sprintf(s,"Best Combo: %d",attempt.bestCombo);
	Print(5,110,s,0,2);

	Print(5,150,"Goals:",0,2);

	for(i=0;i<5;i++)
	{
		RenderAGoal(30,180+i*30,chal[chalCursor].goal[i].type,chal[chalCursor].goal[i].n);
		if(chalData.goal[chalCursor][i])
			chalSpr->GetSprite(2)->DrawBright(8,198+i*30,mgl,(char)MGL_random(8));
		else
			chalSpr->GetSprite(3)->DrawBright(8,198+i*30,mgl,0);
	}

	PrintBrightGlow(30,450,"Press Fire To Continue",(char)MGL_random(8),2);
}

byte ChallengeTallyUpdate(MGLDraw *mgl,int *lastTime)
{
	byte c;
	static byte reptCounter=0;

	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateItems();
		c=GetControls();

		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
		oldc=c;
		*lastTime-=TIME_PER_FRAME;
	}

	if(mgl->LastKeyPressed()==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		return 1;
	}

	JamulSoundUpdate();
	return 0;
}

void InitChallengeTally(MGLDraw *mgl)
{
	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;
	if(attempt.bestCombo==1)
		attempt.bestCombo=0;
	CompleteGoals();
	if(attempt.score>(int)chalData.topScore[chalCursor])
		chalData.topScore[chalCursor]=attempt.score;
	if(attempt.time>chalData.topTime[chalCursor] && attempt.goalOk[0])
		chalData.topTime[chalCursor]=attempt.time;
	if(attempt.bestCombo>chalData.topCombo[chalCursor])
		chalData.topCombo[chalCursor]=attempt.bestCombo;
}

TASK(void) ChallengeTally(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	if(attempt.quit)
		CO_RETURN;

	InitChallengeTally(mgl);
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=ChallengeTallyUpdate(mgl,&lastTime);
		ChallengeTallyRender(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		EndClock();
	}
}

byte CurrentLetter(void)
{
	if(!challenging)
		return 0;

	return attempt.curLetter;
}

byte DoTrivia(void)
{
	return (challenging==1 && chalData.bought[63]==1);
}

void CheatChal(void)
{
	chalData.stars+=10;
}
