#include "tally.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "progress.h"
#include "editor.h"
#include "hiscore.h"
#include "goal.h"

#define CATEGORY_X	20
#define VALUE_X		280
#define RECORD_X	450
#define POINTS_X	620

static byte *backgd;
static byte curLine;
static int comboBonus;
static byte lineNum,skip;
static int baseScore,showBaseScore,finalScore,showFinalScore,points,totalCoins,showTotalCoins,bonusCoins;
static char levelName[32];
static int tallyWait;
static float destructBonus,destructPct,perfectBonus,diffBonus;
static score_t topTimes[3], topScores[3];

void InitTally(MGLDraw *mgl)
{
	int i;

	curLine=0;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	baseScore=player.score;
	showBaseScore=0;
	showFinalScore=0;
	finalScore=0;
	points=player.score;
	lineNum=0;
	tallyWait=0;
	skip=0;
	totalCoins=profile.progress.totalCoins-profile.progress.coinsSpent;
	showTotalCoins=totalCoins;

	if(player.totalEnemies==0)
	{
		destructPct=1.0f;
	}
	else
	{
		if(player.enemiesSlain>player.totalEnemies)
			player.enemiesSlain=player.totalEnemies;
		destructPct=((float)player.enemiesSlain/(float)player.totalEnemies);
		if(destructPct>1.0f)
			destructPct=1.0f;
	}
	destructBonus=0.5f+destructPct;
	destructPct*=100.0f;

	if(player.perfect)
		perfectBonus=1.5f;
	else
		perfectBonus=1.0f;

	if (profile.difficulty == DIFFICULTY_NORMAL)
		diffBonus = 0.75f;
	else if (profile.difficulty == DIFFICULTY_HARD)
		diffBonus = 1.0f;
	else if (profile.difficulty == DIFFICULTY_LUNATIC)
		diffBonus = 1.25f;
	static_assert(MAX_DIFFICULTY == 3, "Must handle new difficulty here");

	mgl->LastKeyPressed();

	if(player.bestCombo>=2)
		comboBonus=10*player.bestCombo;
	else
		comboBonus=0;

	topTimes[0].score=9*60*60*30+99*30*60+59*30;
	GetTopTimes(topTimes,curWorld.map[player.levelNum]);
	topScores[0].score=0;
	GetTopScores(topScores, curWorld.map[player.levelNum]);

	if(profile.progress.bestCombo<player.bestCombo)
		profile.progress.bestCombo=player.bestCombo;

	if(!player.cheated)
	{
		if(player.bestCombo>player.levelProg->recordCombo)
		{
			player.gotRecords|=RECORD_COMBO;
			player.levelProg->recordCombo=player.bestCombo;
		}
		if(destructPct>player.levelProg->recordDestroy)
		{
			player.gotRecords|=RECORD_DESTROY;
			player.levelProg->recordDestroy=destructPct;
		}
		if(player.score>player.levelProg->recordBaseScore)
		{
			player.gotRecords|=RECORD_BASE;
			player.levelProg->recordBaseScore=player.score;
		}
	}

	if(profile.musicMode==MUSIC_OFFICIAL)
		StopSong();
	MakeNormalSound(SND_TALLY);
}

void ExitTally(void)
{
	if(!editing && verified)
		profile.progress.totalCoins=totalCoins+profile.progress.coinsSpent;
	free(backgd);
	if(!player.cheated)
		GoalTallyPage(finalScore,player.perfect,(player.enemiesSlain==player.totalEnemies),player.clock);
}

byte UpdateTally(int *lastTime,MGLDraw *mgl)
{
	char c;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();

		if(!skip)
		{
			if(showTotalCoins<totalCoins)
				showTotalCoins++;
			if(showBaseScore<baseScore-1000)
				showBaseScore+=300+Random(100);
			if(showBaseScore<baseScore-100)
				showBaseScore+=30+Random(10);
			else if(showBaseScore<baseScore-10)
				showBaseScore+=1+Random(9);
			else if(showBaseScore<baseScore)
				showBaseScore++;
			else if(showBaseScore>baseScore+1000)
				showBaseScore-=(300+Random(100));
			else if(showBaseScore>baseScore+100)
				showBaseScore-=(30+Random(10));
			else if(showBaseScore>baseScore+10)
				showBaseScore-=(1+Random(9));
			else if(showBaseScore>baseScore)
				showBaseScore--;

			if(showFinalScore<finalScore-1000)
				showFinalScore+=300+Random(100);
			if(showFinalScore<finalScore-100)
				showFinalScore+=30+Random(10);
			else if(showFinalScore<finalScore-10)
				showFinalScore+=1+Random(9);
			else if(showFinalScore<finalScore)
				showFinalScore++;
			else if(showFinalScore>finalScore+100)
				showFinalScore-=(30+Random(10));
			else if(showFinalScore>finalScore+10)
				showFinalScore-=(1+Random(9));
			else if(showFinalScore>finalScore)
				showFinalScore--;

			tallyWait++;
		}
		else
		{
			tallyWait=30;
			baseScore=points+player.bestCombo*10;
			if(baseScore>(int)(player.clock/10))
				baseScore-=(player.clock/10);
			else
				baseScore=0;

			finalScore=(int)((float)baseScore*diffBonus);
			finalScore=(int)((float)finalScore*destructBonus);
			finalScore=(int)((float)finalScore*perfectBonus);

			totalCoins=profile.progress.totalCoins-profile.progress.coinsSpent;
			if(!player.cheated)
				totalCoins+=player.coins;
			if(!player.cheated)
			{
				bonusCoins=finalScore/100;
				if(bonusCoins>500)
					bonusCoins=500;
				totalCoins+=bonusCoins;
			}

			showBaseScore=baseScore;
			showFinalScore=finalScore;
			showTotalCoins=totalCoins;
		}

		if(lineNum<3)	// if in the base score section, keep final score equal to it
		{
			finalScore=baseScore;
			showFinalScore=showBaseScore;
		}
		switch(lineNum)
		{
			case 0:	// base points
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=1;
					baseScore+=player.bestCombo*10;
				}
				break;
			case 1:	// combo
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=2;
					if(baseScore>(int)(player.clock/10))
						baseScore-=(player.clock/10);
					else
						baseScore=0;
				}
				break;
			case 2:	// time
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=3;
					finalScore=(int)((float)finalScore*diffBonus);
				}
				break;
			case 3:	// difficulty bonus
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=4;
					finalScore=(int)((float)finalScore*destructBonus);
				}
				break;
			case 4:	// destruction bonus
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=5;
					finalScore=(int)((float)finalScore*perfectBonus);
				}
				break;
			case 5:	// perfect bonus
				if(player.perfect==0)
					tallyWait=30;
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=6;
					if(!player.cheated)
						totalCoins+=player.coins;
				}
				break;
			case 6:	// coins found
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=7;
					if(!player.cheated)
					{
						bonusCoins=finalScore/100;
						if(bonusCoins>500)
							bonusCoins=500;
						totalCoins+=bonusCoins;
					}
				}
				break;
			case 7:	// bonus coins
				if(tallyWait>=30)
				{
					tallyWait=0;
					lineNum=8;
				}
				break;
			case 8:	// all done, waiting for exit
				skip=1;
				break;
		}
		*lastTime-=TIME_PER_FRAME;
	}

	c=mgl->LastKeyPressed();
	if(c || GetTaps())
	{
		if(skip==0)
			skip=1;
		else
		{
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
	}
	return 0;
}

static void TallyLine(byte n,int y,const char *category,const char *value,const char *points,const char *record,byte gotRec)
{
	char bright;

	if(n==255)
		bright=0;
	else if(lineNum>n)
		bright=0;
	else if(lineNum==n)
		bright=(tallyWait*2-30);
	else
		bright=-32;

	PrintGlow(CATEGORY_X,y,category,bright,2);
	PrintGlow(VALUE_X-GetStrLength(value,2),y,value,bright,2);
	if(gotRec && bright==0)
		PrintGlow(RECORD_X-GetStrLength(record,2),y-Random(2),record,bright-16+Random(32),2);
	else
		PrintGlow(RECORD_X-GetStrLength(record,2),y,record,bright,2);
	PrintGlow(POINTS_X-GetStrLength(points,2),y,points,bright,2);
}

static void Tally2Line(byte n,int y,const char *category,const char *value,const char *mult)
{
	char bright;

	if(lineNum>n)
		bright=0;
	else if(lineNum==n)
		bright=(tallyWait*2-30);
	else
		bright=-32;

	PrintGlow(CATEGORY_X,y,category,bright,2);
	PrintGlow(VALUE_X-GetStrLength(value,2),y,value,bright,2);
	PrintGlow(POINTS_X-GetStrLength(mult,2),y,mult,bright,2);
}

void CoinLine(byte n,int y,const char *title,const char *num)
{
	char bright;

	if(n==255)
		bright=0;
	else if(lineNum>n)
		bright=0;
	else if(lineNum==n)
		bright=(tallyWait*2-30);
	else
		bright=-32;

	PrintGlow(CATEGORY_X,y,title,bright,2);
	PrintGlow(VALUE_X-GetStrLength(num,2),y,num,bright,2);
}

void MakeTime(char *s,dword clock)
{
	sprintf(s,"%u:%05.2f",clock/(30*60),(float)((clock%(30*60))/30.0f));
}

void RenderTally(MGLDraw *mgl)
{
	int i;
	char s[32],s2[32],s3[32];

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	if(player.cheated)
		PrintGlow(320-GetStrLength("You CHEATED THROUGH",2)/2,20,"You CHEATED THROUGH",0,2);
	else
		PrintGlow(320-GetStrLength("You completed",2)/2,20,"You completed",0,2);
	PrintGlow(320-GetStrLength(levelName,2)/2,40,levelName,0,2);
	mgl->FillBox(160,58,639-160,58,32*1+16);

	TallyLine(255,70,"Category","Value","Bonus","Record",0);
	mgl->FillBox(20,88,620,88,32*1+16);

	sprintf(s,"%d",points);
	sprintf(s2,"%u",player.levelProg->recordBaseScore);
	TallyLine(0,95,"Score",s,s,s2,(player.gotRecords&RECORD_BASE));

	sprintf(s,"%d",player.bestCombo);
	sprintf(s2,"%d",player.bestCombo*10);
	sprintf(s3,"%d",player.levelProg->recordCombo);
	TallyLine(1,120,"Combo",s,s2,s3,(player.gotRecords&RECORD_COMBO));

	MakeTime(s,player.clock);
	sprintf(s2,"%d",-(player.clock/10));
	MakeTime(s3,topTimes[0].score);
	TallyLine(2,145,"Time",s,s2,s3,((player.gotRecords&RECORD_TIME) && !(player.gotRecords&RECORD_TIME2)));

	mgl->FillBox(410,168,620,168,32*1+16);
	sprintf(s,"%d",showBaseScore);
	PrintGlow(POINTS_X-GetStrLength(s,2),170,s,0,2);
	PrintGlow(420,170,"Base Score:",0,2);

	sprintf(s,"x%1.2f",diffBonus);
	if(player.cheated)
		Tally2Line(3,210,"Difficulty","CHEATER!",s);
	else
		Tally2Line(3,210,"Difficulty",GetDifficultyName(profile.difficulty),s);

	sprintf(s,"%0.1f%%",destructPct);
	sprintf(s2,"x%1.1f",destructBonus);
	//Tally2Line(4,240,"Destruction",s,s2);
	sprintf(s3,"%0.1f%%",player.levelProg->recordDestroy);
	TallyLine(4,240,"Destruction",s,s2,s3,(player.gotRecords&RECORD_DESTROY));

	if(player.perfect)
	{
		Tally2Line(5,270,"Perfect!","","x1.5");
	}

	mgl->FillBox(410,308,620,308,32*1+16);
	if(player.cheated)
		strcpy(s,"ZILCH!");
	else
		sprintf(s,"%d",showFinalScore);
	if((player.gotRecords&RECORD_SCORE) && !(player.gotRecords&RECORD_SCORE2))
		PrintGlow(POINTS_X-GetStrLength(s,2),310-Random(2),s,-16+Random(32),2);
	else
		PrintGlow(POINTS_X-GetStrLength(s,2),310,s,0,2);
	PrintGlow(420,310,"Total Score:",0,2);

	sprintf(s,"%d",topScores[0].score);
	PrintGlow(420,330,"Record:",0,2);
	PrintGlow(POINTS_X-GetStrLength(s,2),330,s,0,2);

	sprintf(s,"%d",player.coins);
	CoinLine(6,330,"Coins Found",s);
	if(player.cheated)
		strcpy(s,"ZIP!");
	else
		sprintf(s,"%d",bonusCoins);
	CoinLine(7,350,"Bonus Coins",s);

	mgl->FillBox(20,378,VALUE_X,378,32*1+16);
	sprintf(s,"%d",showTotalCoins);
	CoinLine(255,380,"Total Coins",s);
}

//----------------

TASK(void) Tally(MGLDraw *mgl,const char *lvlName,byte countIt)
{
	byte done=0;
	int lastTime=1;

	strcpy(levelName,lvlName);

	InitTally(mgl);
	GetTaps();
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=UpdateTally(&lastTime,mgl);
		RenderTally(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitTally();
	SaveProfile();
}
