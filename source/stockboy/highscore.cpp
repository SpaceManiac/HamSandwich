#include "highscore.h"
#include "display.h"
#include "player.h"
#include "options.h"
#include "appdata.h"

#define HI_GUY_SPR 10

static int goldScore[3*MAX_MAPS];
static highScore_t addOnScore[3*MAX_MAPS];
static char addOnScoreName[64];

static int baseHiScore[NUM_GAMES][MAX_SCORES][3]=
{
	// training
	{
		{0,0,0},	// not used
	},
	// blowout
	{
		{500,1000,2000},	// normalia
		{125,250,500},	// chessboard
		{125,250,500},	// quadraphonic
		{90,175,350},	// tiny franchise
		{125,250,500},	// islands
		{250,500,1000},	// customers
		{250,500,1000},	// pushy customers
		{50,100,200},	// scarytown
	},
	// stockroom
	{
		{600,800,1000},	// january week 1
		{1000,2000,2500},	// january week 2
		{200,500,700},	// january week 3
		{1050,1200,1500},	// january week 4
		{150,250,300},	// february week 1
		{200,400,600},	// february week 2
		{120,160,200},	// february week 3
		{500,1000,1500},	// february week 4
		{1200,1400,1600},	// march week 1
		{140,200,260},	// march week 2
		{1500,1800,2200},	// march week 3
		{750,1500,3000},	// march week 4
		{2000,3000,4000},	// april week 1
		{100,200,300},	// april week 2
		{1000,1050,1100},	// april week 3
		{300,500,700},	// april week 4
		{200,500,700},	// may week 1
		{1100,1250,1500},	// may week 2
		{1400,1600,1800},	// may week 3
		{1100,1200,1300},	// may week 4
		{500,700,750},	// june week 1
		{600,800,1000},	// june week 2
		{500,650,750},	// june week 3
		{400,600,700},	// june week 4
		{1200,1600,1800},	// july week 1
		{10000,11000,12000},	// july week 2
		{300,400,500},	// july week 3
		{250,300,350},	// july week 4
		{800,1000,1200},	// august week 1
		{150,200,300},	// august week 2
		{2000,2500,3000},	// august week 3
		{3200,3500,4000},	// august week 4
		{1700,1800,1850},	// september week 1
		{2200,2400,2600},	// september week 2
		{2000,3000,4000},	// september week 3
		{1200,1300,1400},	// september week 4
		{1300,1400,1500},	// october week 1
		{200,400,800},	// october week 2
		{300,600,900},	// october week 3
		{700,800,850},	// october week 4
		{10500,11000,12000},	// november week 1
		{100,200,360},	// november week 2
		{200,300,600},	// november week 3
		{100,150,250},	// november week 4
		{2000,3000,3500},	// december week 1
		{1100,1500,2000},	// december week 2
		{600,800,1000},	// december week 3
		{350,450,550},	// december week 4
		{700,800,900},	// smarch week 1
		{1100,1200,1250},	// smarch week 2
		{800,1100,1300},	// smarch week 3
		{2000,3000,3500},	// smarch week 4
	},
	// clearance
	{
		{1000,2500,5000},	// only one type
	},
	// pest control
	{
		{500,1000,2000},	// traffic
		{100,200,400},	// blob maze
		{100,200,400},	// pigpen
		{50,100,200},	// spiral
		{50,100,200},	// surrounded
		{25,50,100},	// laser tag
		{2500,5000,10000},	// king blobby
		{2500,5000,10000},	// pork zone
		{25,50,100},	// hotel of doom
	},
	// add-ons
	{
		{0,0,0},	// not used
	},
	// parallel
	{
		{1500,2000,2700},	// january week 1
		{1500,2000,2500},	// january week 2
		{400,600,700},	// january week 3
		{300,400,500},	// january week 4
		{180,210,240},	// february week 1
		{400,500,650},	// february week 2
		{400,500,600},	// february week 3
		{400,600,800},	// february week 4
		{700,900,1000},	// march week 1
		{600,800,1000},	// march week 2
		{2000,2500,3000},	// march week 3
		{600,800,1200},	// march week 4
		{3000,5000,6000},	// april week 1
		{300,380,420},	// april week 2
		{300,350,400},	// april week 3
		{1200,1500,1700},	// april week 4
		{400,600,800},	// may week 1
		{400,600,800},	// may week 2
		{1500,2000,2600},	// may week 3
		{200,300,400},	// may week 4
		{500,650,700},	// june week 1
		{1100,1200,1400},	// june week 2
		{800,1000,1200},	// june week 3
		{300,500,700},	// june week 4
		{1000,1400,1700},	// july week 1
		{1200,1600,2000},	// july week 2
		{400,500,550},	// july week 3
		{300,350,400},	// july week 4
		{1200,1500,1800},	// august week 1
		{250,300,350},	// august week 2
		{1000,1500,2000},	// august week 3
		{2000,2500,3000},	// august week 4
		{600,800,1000},	// september week 1
		{1000,1600,1900},	// september week 2
		{800,1100,1300},	// september week 3
		{1500,2000,2500},	// september week 4
		{400,500,600},	// october week 1
		{400,600,800},	// october week 2
		{1200,1600,1800},	// october week 3
		{500,400,300},	// october week 4
		{1000,1600,2000},	// november week 1
		{200,400,450},	// november week 2
		{800,1000,1100},	// november week 3
		{400,800,1000},	// november week 4
		{1200,1700,2000},	// december week 1
		{800,1000,1200},	// december week 2
		{800,1000,1100},	// december week 3
		{200,400,450},	// december week 4
		{200,500,700},	// smarch week 1
		{100,250,300},	// smarch week 2
		{500,800,1000},	// smarch week 3
		{1000,2000,2500},	// smarch week 4
	},
};

// table of which color to modify in each pic
byte hiFromTab[]=
	{0,	// star
	 7,	// stockboy
	 6,	// gothgirl
	 5,	// ro-bot
	 7,	// happy stick
	 4,	// santa
	 5,	// cherub
	 5,	// shroom
	 1,	// pig
	 4,	// freak
	 5,	// hippie
};

static sprite_set_t *hiSpr=NULL;

void LoadHiScoreSprites(void)
{
	if(!hiSpr)
	{
		hiSpr=new sprite_set_t("graphics/hiscore.jsp");
	}
}

void FreeHiScoreSprites(void)
{
	if(hiSpr)
		delete hiSpr;
	hiSpr=NULL;
}

void ResetHighScore(byte mode,byte level)
{
	if(mode==GAME_ADDON)
	{
		strcpy(addOnScore[level*3+0].name,"Bronze Star");
		addOnScore[level*3+0].pic=0;
		addOnScore[level*3+0].color=2;
		addOnScore[level*3+0].bright=0;
		addOnScore[level*3+0].score=goldScore[level*3+2];
		strcpy(addOnScore[level*3+1].name,"Silver Star");
		addOnScore[level*3+1].pic=0;
		addOnScore[level*3+1].color=0;
		addOnScore[level*3+1].bright=0;
		addOnScore[level*3+1].score=goldScore[level*3+1];
		strcpy(addOnScore[level*3+2].name,"Gold Star");
		addOnScore[level*3+2].pic=0;
		addOnScore[level*3+2].color=5;
		addOnScore[level*3+2].bright=0;
		addOnScore[level*3+2].score=goldScore[level*3+0];
	}
	else
	{
		strcpy(opt.score[mode][level][0].name,"Bronze Star");
		opt.score[mode][level][0].pic=0;
		opt.score[mode][level][0].color=2;
		opt.score[mode][level][0].bright=0;
		opt.score[mode][level][0].score=baseHiScore[mode][level][0];
		strcpy(opt.score[mode][level][1].name,"Silver Star");
		opt.score[mode][level][1].pic=0;
		opt.score[mode][level][1].color=0;
		opt.score[mode][level][1].bright=0;
		opt.score[mode][level][1].score=baseHiScore[mode][level][1];
		strcpy(opt.score[mode][level][2].name,"Gold Star");
		opt.score[mode][level][2].pic=0;
		opt.score[mode][level][2].color=5;
		opt.score[mode][level][2].bright=0;
		opt.score[mode][level][2].score=baseHiScore[mode][level][2];
	}
}

void ResetHighScores(byte mode)
{
	int i;

	if(mode==GAME_ADDON)
	{
		for(i=0;i<MAX_MAPS;i++)
			ResetHighScore(mode,i);
		SaveAddOnScores();
	}
	else
	{
		for(i=0;i<MAX_SCORES;i++)
			ResetHighScore(mode,i);
	}
}

void RenderStar(byte color,byte spr,int x,int y,MGLDraw *mgl)
{
	byte colFromTab[]={5,5,0,0,0,
					   0,0,0,0,0,
					   0,0,0,0,0,
					   0,0,0,0,0,
					   0,0,0,0,0,
					   0,0,0,0,0,
			   		   5,0,0,0,0,
					   0,0,0,0,0};

	x/=FIXAMT;
	y/=FIXAMT;

	LoadHiScoreSprites();

	if(color!=2)
		hiSpr->GetSprite(spr)->DrawOffColor(x,y,mgl,colFromTab[spr-3],color,0);
	else
		hiSpr->GetSprite(spr)->DrawOffColor(x,y,mgl,colFromTab[spr-3],color,-4);
}

void RenderGlowStar(byte spr,int x,int y,MGLDraw *mgl)
{
	x/=FIXAMT;
	y/=FIXAMT;

	LoadHiScoreSprites();

	hiSpr->GetSprite(spr)->DrawGlow(x,y,mgl,-15);
}

void RenderGhostStar(byte spr,int x,int y,char bright,MGLDraw *mgl)
{
	x/=FIXAMT;
	y/=FIXAMT;

	LoadHiScoreSprites();

	//hiSpr->GetSprite(spr)->DrawGhost(x,y,mgl,bright);
	hiSpr->GetSprite(spr)->DrawGhost(x,y,mgl,bright);
}

void RenderHighScore(byte mode,byte level,byte score,int x,int y,MGLDraw *mgl)
{
	highScore_t *h;
	char num[10];

	LoadHiScoreSprites();

	if(mode!=GAME_ADDON)
		h=&opt.score[mode][level][score];
	else
		h=&addOnScore[level*3+score];

	hiSpr->GetSprite(score)->Draw(x,y,mgl);
	hiSpr->GetSprite(HI_GUY_SPR+h->pic)->DrawOffColor(x,y,mgl,hiFromTab[h->pic],h->color,h->bright);
	y-=4;
	PrintOutline(x+34,y,h->name,0,0);

	sprintf(num,"%07d",h->score);
	PrintOutline(x+34,y+20,num,0,0);
}

byte CheckHighScore(byte mode,byte level,int score)
{
	if(score>=opt.score[mode][level][2].score)
	{
		// #1 high score
		opt.score[mode][level][0]=opt.score[mode][level][1];
		opt.score[mode][level][1]=opt.score[mode][level][2];
		strcpy(opt.score[mode][level][2].name,profile.name);
		opt.score[mode][level][2].pic=1+profile.character;
		opt.score[mode][level][2].color=profile.color;
		opt.score[mode][level][2].bright=profile.bright;
		opt.score[mode][level][2].score=score;
		SaveOptions();
	}
	else if(score>=opt.score[mode][level][1].score)
	{
		// #2 high score
		opt.score[mode][level][0]=opt.score[mode][level][1];
		strcpy(opt.score[mode][level][1].name,profile.name);
		opt.score[mode][level][1].pic=1+profile.character;
		opt.score[mode][level][1].color=profile.color;
		opt.score[mode][level][1].bright=profile.bright;
		opt.score[mode][level][1].score=score;
		SaveOptions();
	}
	else if(score>=opt.score[mode][level][0].score)
	{
		// #3 high score
		strcpy(opt.score[mode][level][0].name,profile.name);
		opt.score[mode][level][0].pic=1+profile.character;
		opt.score[mode][level][0].color=profile.color;
		opt.score[mode][level][0].bright=profile.bright;
		opt.score[mode][level][0].score=score;
		SaveOptions();
	}
	if(score>=baseHiScore[mode][level][2])
		return 4;	// got gold
	if(score>=baseHiScore[mode][level][1])
		return 3;	// got silver
	if(score>=baseHiScore[mode][level][0])
		return 2;	// got bronze
	return 0;
}

byte LoadAddOnScores(char *fname)
{
	FILE *f;
	int i,j;

	strcpy(addOnScoreName,fname);

	// replace the ".sbl" with ".hig"
	addOnScoreName[strlen(addOnScoreName)-3]='h';
	addOnScoreName[strlen(addOnScoreName)-2]='i';
	addOnScoreName[strlen(addOnScoreName)-1]='g';

	f=AppdataOpen(addOnScoreName);
	if(!f)
	{
		// set up empty scores
		for(i=0;i<MAX_MAPS;i++)
		{
			strcpy(addOnScore[i*3+0].name,"Bronze Star");
			addOnScore[i*3+0].color=2;
			strcpy(addOnScore[i*3+1].name,"Silver Star");
			addOnScore[i*3+1].color=0;
			strcpy(addOnScore[i*3+2].name,"Gold Star");
			addOnScore[i*3+2].color=5;
			for(j=0;j<3;j++)
			{
				goldScore[i*3+0]=0;
				addOnScore[i*3+0].bright=0;
				addOnScore[i*3+0].color=0;
				addOnScore[i*3+0].pic=0;
				addOnScore[i*3+0].score=0;
			}
		}
		return 0;
	}

	fread(goldScore,MAX_MAPS*3,sizeof(int),f);
	fread(addOnScore,MAX_MAPS*3,sizeof(highScore_t),f);
	fclose(f);

	return 1;
}

void SetAddOnScoreName(char *fname)
{
	strcpy(addOnScoreName,fname);

	// replace the ".sbl" with ".hig"
	addOnScoreName[strlen(addOnScoreName)-3]='h';
	addOnScoreName[strlen(addOnScoreName)-2]='i';
	addOnScoreName[strlen(addOnScoreName)-1]='g';
}

byte SaveAddOnScores(void)
{
	FILE *f;

	f=AppdataOpen_Write(addOnScoreName);
	if(!f)
		return 0;

	fwrite(goldScore,MAX_MAPS*3,sizeof(int),f);
	fwrite(addOnScore,MAX_MAPS*3,sizeof(highScore_t),f);
	fclose(f);

	return 1;
}

byte CheckAddOnScore(byte level,int score)
{
	if(score>=addOnScore[level*3+2].score)
	{
		// #1 high score
		addOnScore[level*3+0]=addOnScore[level*3+1];
		addOnScore[level*3+1]=addOnScore[level*3+2];
		strcpy(addOnScore[level*3+2].name,profile.name);
		addOnScore[level*3+2].pic=1+profile.character;
		addOnScore[level*3+2].color=profile.color;
		addOnScore[level*3+2].bright=profile.bright;
		addOnScore[level*3+2].score=score;
		SaveAddOnScores();
	}
	else if(score>=addOnScore[level*3+1].score)
	{
		// #2 high score
		addOnScore[level*3+0]=addOnScore[level*3+1];
		strcpy(addOnScore[level*3+1].name,profile.name);
		addOnScore[level*3+1].pic=1+profile.character;
		addOnScore[level*3+1].color=profile.color;
		addOnScore[level*3+1].bright=profile.bright;
		addOnScore[level*3+1].score=score;
		SaveAddOnScores();
	}
	else if(score>=addOnScore[level*3+0].score)
	{
		// #3 high score
		strcpy(addOnScore[level*3+0].name,profile.name);
		addOnScore[level*3+0].pic=1+profile.character;
		addOnScore[level*3+0].color=profile.color;
		addOnScore[level*3+0].bright=profile.bright;
		addOnScore[level*3+0].score=score;
		SaveAddOnScores();
	}
	if(score>=goldScore[level*3+0])
		return 4;	// got gold
	if(score>=goldScore[level*3+1])
		return 3;	// got silver
	if(score>=goldScore[level*3+2])
		return 2;	// got bronze
	return 0;
}

int GoldScore(int lvl,byte score)
{
	return goldScore[lvl*3+score];
}

void SetGoldScore(int lvl,byte which,int score)
{
	goldScore[lvl*3+which]=score;
}

void SwapGoldScores(int a,int b)
{
	int i;
	int s;

	for(i=0;i<3;i++)
	{
		s=goldScore[a*3+i];
		goldScore[a*3+i]=goldScore[b*3+i];
		goldScore[b*3+i]=s;
	}
}
