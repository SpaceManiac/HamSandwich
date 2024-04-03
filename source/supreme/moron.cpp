#include "moron.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "playlist.h"
#include "nameentry.h"
#include "yesnodialog.h"
#include "goal.h"
#include "appdata.h"
#include <assert.h>

void PickQuestion(void);

typedef struct question_t
{
	char question[128];
	byte right,wrong;
	char rightAns[16][64];
	char wrongAns[16][64];
} question_t;

byte numQs[10];
question_t *questions[10];	// 10 arrays, one for each level of question
int levelValue[10]={1,5,10,25,45,75,120,200,320,500};

static const char introAns[4][8]={"Yes!","Sure!","Indeed!","No."};
static const char playedAns[4][16]={"Drat!","Darn!","Shoot!","Foiled again!"};

static byte *backgd;
static sprite_set_t *plSpr;
static char msBright,msDBright;
static int msx,msy,oldMsx,oldMsy;
static char buf[384];
static const char *ans[4];
static byte whichIsRight,level;
static byte cursor,lastQuestion;
static int winnings;
static byte havePlayed;

static word ansX[]={15,330,15,330};
static word ansY[]={344,344,409,409};

#define ANS_WID (295)
#define ANS_HEI	(56)

byte GetQuestion(question_t *q,FILE *f)
{
	q->right=0;
	q->wrong=0;

	while(fgets(buf,127,f))
	{
		if(buf[0]=='Q')
		{
			strcpy(q->question,&buf[2]);
			q->question[(strlen(q->question))-1]='\0';
		}
		else if(buf[0]=='A' && q->right<16)
		{
			strcpy(q->rightAns[q->right],&buf[2]);
			q->rightAns[q->right][strlen(q->rightAns[q->right])-1]='\0';
			q->right++;
		}
		else if(buf[0]=='W' && q->wrong<16)
		{
			strcpy(q->wrongAns[q->wrong],&buf[2]);
			q->wrongAns[q->wrong][strlen(q->wrongAns[q->wrong])-1]='\0';
			q->wrong++;
		}
		else if(buf[0] == '\r' || buf[0] == '\n')	// dead end, bucko!
			return 1;
	}
	return 0;	// if you got here, the file ended
}

byte GetQuestions(void)
{
	int i;
	FILE *f;

	f=AssetOpen("gallery/mrqs.bmp");
	if(!f)
		return 0;	// can't play without questions!

	for(i=0;i<10;i++)
	{
		numQs[i]=0;
		questions[i]=NULL;
	}

	while(fgets(buf,127,f))
	{
		if(buf[0]=='L')
		{
			// a new question is here!
			i=buf[2]-'1';
			if(i==0 && buf[3]=='0')
				i=9;	// "10" is two digits!
			numQs[i]++;
			if(questions[i]==NULL)
			{
				questions[i]=(question_t *)malloc(sizeof(question_t));
			}
			else
			{
				questions[i]=(question_t *)realloc(questions[i],sizeof(question_t)*numQs[i]);
			}
			if(!GetQuestion((question_t *)(&questions[i][(numQs[i]-1)]),f))
				break;	// that question was last
		}
	}
	fclose(f);
	return 1;
}

byte InitMoron(MGLDraw *mgl)
{
	int i;

	msBright=0;
	msDBright=1;
	level=0;

	lastQuestion=255;
	if(!GetQuestions())
		return 0;

	mgl->LoadBMP("graphics/moron.bmp");
	backgd=(byte *)malloc(640*480);
	plSpr=new sprite_set_t("graphics/pause.jsp");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
	mgl->LastKeyPressed();
	mgl->MouseTap();
	GetTaps();
	GetArrowTaps();

	if(!havePlayed)
	{
		strcpy(buf,"Welcome to \"Who Doesn't Want To Be A Moron?\"!  For a small fee of 50 coins, you can give it a try! "
				   "Each question raises the stakes, up to 500 coins!  You can walk away with your winnings at any time "
				   "by clicking QUIT... but if you ever get a question wrong, you lose it ALL!  Are you ready?");

		level=255;
		ans[0]=introAns[0];
		ans[1]=introAns[1];
		ans[2]=introAns[2];
		ans[3]=introAns[3];
		whichIsRight=3;
		cursor=0;
		winnings=0;
	}
	else
	{
		strcpy(buf,"Sorry, pal!  We're not filming another episode of the show until tomorrow.  You can give it another "
				   "shot tomorrow!");

		level=255;
		ans[0]=playedAns[0];
		ans[1]=playedAns[1];
		ans[2]=playedAns[2];
		ans[3]=playedAns[3];
		whichIsRight=3;
		cursor=0;
		winnings=0;
	}

	mgl->GetMouse(&msx,&msy);
	oldMsx=msx;
	oldMsy=msy;

	return 1;
}

void ExitMoron(void)
{
	int i;

	free(backgd);
	delete plSpr;

	for(i=0;i<10;i++)
		if(questions[i]!=NULL)
			free(questions[i]);
}

void PickQuestion(void)
{
	byte l;
	byte q,a,w,prevW[2];
	const char *c;

	if(Random(5)==0)
		l=(level-1+Random(3));
	else
		l=level;	// usually pick from this level
	if(l==255)
		l=0;
	if(l>9)
		l=9;

	assert(numQs[l]!=0);

	q=lastQuestion;
	while(q==lastQuestion)
		q=Random(numQs[l]);
	lastQuestion=q;

	if(levelValue[level]==1)
		sprintf(buf,"For 1 coin... %s",questions[l][q].question);
	else
		sprintf(buf,"For %d coins... %s",levelValue[level],questions[l][q].question);

	assert(questions[l][q].right>0);

	a=Random(questions[l][q].right);
	ans[0]=questions[l][q].rightAns[a];
	whichIsRight=0;
	prevW[0]=255;
	prevW[1]=255;

	assert(questions[l][q].wrong>2);

	w=Random(questions[l][q].wrong);
	ans[1]=questions[l][q].wrongAns[w];
	prevW[0]=w;

	while(w==prevW[0])
		w=Random(questions[l][q].wrong);

	ans[2]=questions[l][q].wrongAns[w];
	prevW[1]=w;

	while(w==prevW[0] || w==prevW[1])
		w=Random(questions[l][q].wrong);

	ans[3]=questions[l][q].wrongAns[w];

	// okay, now we have right answer A, and 3 wrong answers BCD
	// not very challenging

	l=Random(4);	// so randomly decide which to swap the right answer with, since the wrongs are already random
	if(l==0)
		return;	// no swap needed

	c=ans[l];
	ans[l]=ans[0];
	ans[0]=c;
	whichIsRight=l;
}

byte UpdateMoron(int *lastTime,MGLDraw *mgl)
{
	char k;
	byte c;
	int i;

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	k=mgl->LastKeyPressed();

	if(k==27)
		return 1;

	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;

		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}
	if(oldMsx!=msx || oldMsy!=msy)
	{
		for(i=0;i<4;i++)
		{
			if(msx>=ansX[i] && msx<ansX[i]+ANS_WID && msy>=ansY[i] && msy<ansY[i]+ANS_WID)
				cursor=i;
		}
		if(msx>=487 && msx<=626 && msy>=6 && msy<=48)
			cursor=255;
	}
	c=GetTaps()|GetArrowTaps();
	if(c&CONTROL_UP)
	{
		if(cursor==255)
			cursor=3;
		else if(cursor<2)
			cursor=255;
		else
			cursor-=2;
	}
	if(c&CONTROL_DN)
	{
		if(cursor==255)
			cursor=1;
		else if(cursor<2)
			cursor+=2;
		else
			cursor=255;
	}
	if(c&CONTROL_LF)
	{
		if(cursor==255)
			cursor=0;
		else if(cursor==0 || cursor==2)
			cursor++;
		else
			cursor--;
	}
	if(c&CONTROL_RT)
	{
		if(cursor==255)
			cursor=1;
		else if(cursor==0 || cursor==2)
			cursor++;
		else
			cursor--;
	}
	if(mgl->MouseTap() || (c&CONTROL_B1))
	{
		if(level==255)
		{
			if(havePlayed || cursor==3 || cursor==255 || (profile.progress.totalCoins-profile.progress.coinsSpent<50))
				return 1;
			else
			{
				profile.progress.coinsSpent+=50;
				level=0;
				PickQuestion();
				MakeNormalSound(SND_MORONHELLO);
			}
		}
		else
		{
			if(cursor==255)
			{
				profile.progress.totalCoins+=winnings;
				return 1;
			}
			else
			{
				if(cursor!=whichIsRight)
				{
					MakeNormalSound(SND_MORONWRONG);
					return 1;	// you lose
				}
				else
				{

					winnings=levelValue[level];
					if(level==9)
					{
						profile.progress.totalCoins+=winnings;
						CompleteGoal(58);
						MakeNormalSound(SND_MORONWIN);
						return 1;
					}
					else
					{
						MakeNormalSound(SND_MORONRIGHT+Random(2));
						level++;
						PickQuestion();
					}
				}
			}
		}
	}

	oldMsx=msx;
	oldMsy=msy;
	return 0;
}

void PrintRectMoron(int x,int y,int x2,int y2,const char *s)
{
	PrintUnGlowRect(x-1,y,x2-1,y2,16,s,2);
	PrintUnGlowRect(x+1,y,x2+1,y2,16,s,2);
	PrintUnGlowRect(x,y-1,x2,y2-1,16,s,2);
	PrintUnGlowRect(x,y+1,x2,y2+1,16,s,2);
	PrintGlowRectBright(x,y,x2,y2,16,s,16,2);
}

void RenderMoron(MGLDraw *mgl)
{
	int i;
	char s[32];

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	InstaRenderItem(TILE_WIDTH/2,18,ITM_COIN,0,mgl);
	sprintf(s,"%u",profile.progress.totalCoins-profile.progress.coinsSpent);
	Print(32-1,4,s,-32,2);
	Print(32+1,4,s,-32,2);
	Print(32,4+1,s,-32,2);
	Print(32,4-1,s,-32,2);
	PrintGlow(32,4,s,8,2);

	sprintf(s,"Winnings: %d",winnings);
	Print(200-1,4,s,-32,2);
	Print(200+1,4,s,-32,2);
	Print(200,4+1,s,-32,2);
	Print(200,4-1,s,-32,2);
	PrintGlow(200,4,s,8,2);

	if(cursor==255)
	{
		DrawBox(487,6,626,48,31);
		DrawBox(487-1,6-1,626+1,48+1,31);
		DrawBox(487+1,6+1,626-1,48-1,31);
		DrawBox(487-3,6-3,626+3,48+3,31);
	}
	else
	{
		DrawBox(ansX[cursor],ansY[cursor],ansX[cursor]+ANS_WID,ansY[cursor]+ANS_HEI,31);
		DrawBox(ansX[cursor]-1,ansY[cursor]-1,ansX[cursor]+ANS_WID+1,ansY[cursor]+ANS_HEI+1,32*3+20);
		DrawBox(ansX[cursor]+1,ansY[cursor]+1,ansX[cursor]+ANS_WID-1,ansY[cursor]+ANS_HEI-1,32*3+20);
		DrawBox(ansX[cursor]-3,ansY[cursor]-3,ansX[cursor]+ANS_WID+3,ansY[cursor]+ANS_HEI+3,32*3+20);
	}

	PrintRectMoron(492,10,626,48,"QUIT!");

	PrintRectMoron(20,235,620,318,buf);

	PrintRectMoron(20,350,305,392,ans[0]);
	PrintRectMoron(333,350,621,392,ans[1]);
	PrintRectMoron(20,416,305,458,ans[2]);
	PrintRectMoron(333,416,621,458,ans[3]);

	// mouse cursor
	SetSpriteConstraints(13,13,627,467);
	if(msx<13)
		msx=13;
	if(msy<13)
		msy=13;
	if(msx>622)
		msx=622;
	if(msy>462)
		msy=462;
	plSpr->GetSprite(0)->DrawBright(msx,msy,mgl,msBright/2);
	ClearSpriteConstraints();
}

//----------------

TASK(void) Moron(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	if(!InitMoron(mgl))
		CO_RETURN;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateMoron(&lastTime,mgl);
		RenderMoron(mgl);

		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}
	ExitMoron();
	GoalPurchase();
	havePlayed=1;
}

void ResetMoron(void)
{
	havePlayed=0;
}
