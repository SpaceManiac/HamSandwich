#include "tally.h"
#include "player.h"
#include "floor.h"
#include "sound.h"
#include "highscore.h"
#include "options.h"
#include "stars.h"
#include "music.h"

byte tallyMode;
int pScore;
byte showTally[7];
byte tallyWait;
byte tallyShouldWait;

static int deliverBonus;
static int timeBonus;
static int rescueBonus;
static int escapeBonus;
static int destructionBonus;
static int dumpBonus;
static int collectBonus;
static int baseScore;
static byte *backScr=NULL;
static byte starGot;
static byte starNew,pStarAdd;
static byte ignoreKeys;
static byte bright,tallyPic;
static char tallyMsg[48];
static byte noUpdateYet;

void SetTallyMsg(char *msg,byte winpic)
{
	strcpy(tallyMsg,msg);
	tallyPic=winpic;
}

void SetupTally(Map *map)
{
	int i,j;

	if(tallyPic==0)
		GetDisplayMGL()->LoadBMP("graphics\\losegame.bmp");
	else switch(profile.character)
	{
		case CHAR_SURF:
			GetDisplayMGL()->LoadBMP("graphics\\win_surfboy.bmp");
			break;
		case CHAR_GOTH:
			GetDisplayMGL()->LoadBMP("graphics\\win_goth.bmp");
			break;
		case CHAR_ROBOT:
			GetDisplayMGL()->LoadBMP("graphics\\win_robot.bmp");
			break;
		case CHAR_HAPPY:
			GetDisplayMGL()->LoadBMP("graphics\\win_happystick.bmp");
			break;
		case CHAR_SANTA:
			GetDisplayMGL()->LoadBMP("graphics\\win_santa.bmp");
			break;
		case CHAR_CHERUB:
			GetDisplayMGL()->LoadBMP("graphics\\win_cherub.bmp");
			break;
		case CHAR_SHROOM:
			GetDisplayMGL()->LoadBMP("graphics\\win_shroom.bmp");
			break;
		case CHAR_PIG:
			GetDisplayMGL()->LoadBMP("graphics\\win_warpig.bmp");
			break;
		case CHAR_FREAK:
			GetDisplayMGL()->LoadBMP("graphics\\win_freak.bmp");
			break;
		case CHAR_HIPPIE:
			GetDisplayMGL()->LoadBMP("graphics\\win_hippie.bmp");
			break;
	}

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		GetDisplayMGL()->FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);

	player.deliverBonus=0;
	for(i=0;i<map->width*map->height;i++)
	{
		if((map->map[i].item==ITM_BARREL ||
		   map->map[i].item==ITM_BALL || map->map[i].item==ITM_MOVINGBALL) && FloorOnlyColor(map->map[i].floor)==map->map[i].itemInfo%8)
			player.deliverBonus++;
	}

	if(map->timer>0)
		player.timeBonus=(map->timer-player.clock);

	player.rescueBonus=0;
	for(i=0;i<8;i++)
		for(j=0;j<4;j++)
			player.rescueBonus+=player.rescueMons[i][j];

	player.pencilBonus=0;
	for(i=0;i<8;i++)
		player.pencilBonus+=player.pencils[i];
	
	pScore=0;
	baseScore=player.score;
	for(i=0;i<7;i++)
		showTally[i]=0;

	tallyMode=255;
	tallyWait=30;
	tallyShouldWait=1;

	destructionBonus=0;
	deliverBonus=0;
	timeBonus=0;
	rescueBonus=0;
	escapeBonus=0;
	dumpBonus=0;
	collectBonus=0;
	bright=0;

	GetTaps();
	GetArrowTaps();
	GetDisplayMGL()->LastKeyPressed();
	MakeNormalSound(SND_WINLEVEL);
	InitStars(0,profile.starsLeft);
	ignoreKeys=30;
	Music_Stop();
	noUpdateYet=1;
}

void ExitTally(void)
{
	if(backScr)
	{
		free(backScr);
		backScr=NULL;
	}
}

byte MakeUpLevelNum(void)
{
	switch(GameType())
	{
		case GAME_BLOWOUT:
			return player.levelNum;
		case GAME_CLEARANCE:
			return 0;
		case GAME_PESTCONTROL:
			return player.levelNum;
		case GAME_TRAINING:
			return 0;
		case GAME_STOCKROOM:
			return player.levelNum+player.worldNum*4;
		case GAME_PARALLEL:
			return player.levelNum+player.worldNum*4;
		case GAME_ADDON:
			return 0;
	}
	return 0;
}

void CheckForStars(void)
{
	int i;

	if(TestingLevel())
	{
		if(player.score>=GoldScore(player.levelNum,0))
			starGot=4;	// got gold
		else if(player.score>=GoldScore(player.levelNum,1))
			starGot=3;	// got silver
		else if(player.score>=GoldScore(player.levelNum,2))
			starGot=2;	// got bronze
		else
			starGot=0;
		starNew=0;
		pStarAdd=0;
		InitStars(starGot,profile.starsLeft);
		return;
	}
	if(GameType()==GAME_ADDON)
	{
		starNew=0;
		pStarAdd=0;
		starGot=CheckAddOnScore(player.levelNum,player.score);
		InitStars(starGot,profile.starsLeft);
		return;
	}

	i=MakeUpLevelNum();

	starGot=CheckHighScore(GameType(),i,player.score);
	if(profile.starGot[GameType()][i]>=starGot)
		starNew=0;
	else
		starNew=1;

	if(starNew)
	{
		i=profile.starGot[GameType()][MakeUpLevelNum()];
		if(starGot==2)
			pStarAdd=1;
		else if(starGot==3)
		{
			if(i==2)	// already had the bronze
				pStarAdd=2;
			else
				pStarAdd=3;
		}
		else
		{
			if(i==2)	// already had bronze
				pStarAdd=4;
			else if(i==3)	// already had silver
				pStarAdd=2;
			else
				pStarAdd=5;
		}
		profile.starGot[GameType()][MakeUpLevelNum()]=starGot;
	}
	else
	{
		pStarAdd=0;
		if(GameType()==GAME_STOCKROOM &&
			profile.starGot[GameType()][MakeUpLevelNum()]==0)
			profile.starGot[GameType()][MakeUpLevelNum()]=1;
		if(GameType()==GAME_PARALLEL &&
			profile.starGot[GameType()][MakeUpLevelNum()]==0)
			profile.starGot[GameType()][MakeUpLevelNum()]=1;
	}

	InitStars(starGot,profile.starsLeft);
	profile.starsLeft+=pStarAdd;
	profile.stars+=pStarAdd;
}

void BrightenBackgd(void)
{
	int i;
	byte b;

	if(tallyMode==255)
		return;
	if(bright==255)
		return;
	bright++;
	if((bright%2)>0)
		return;

	for(i=0;i<640*480;i++)
	{
		b=backScr[i];
		if(((b+1)%32)==0)
			b=31;
		else
			b++;
		backScr[i]=b;
	}
}

byte UpdateTally(void)
{
	if(noUpdateYet)
	{
		noUpdateYet=0;
		baseScore=player.score;
	}

	BrightenBackgd();

	if(pScore>player.score)
	{
		if(pScore>player.score+100)
			pScore-=100;
		else if(pScore>player.score+10)
			pScore-=10;
		else
			pScore--;
	}
	else if(pScore<player.score)
	{
		if(pScore<player.score-1000)
			pScore+=1000;
		if(pScore<player.score-100)
			pScore+=100;
		else if(pScore<player.score-10)
			pScore+=10;
		else
			pScore++;
	}

	UpdateStars();
	if(ignoreKeys)
	{
		ignoreKeys--;
		if(ignoreKeys==0)
		{
			// clear those buffers
			GetTaps();
			GetArrowTaps();
			GetDisplayMGL()->LastKeyPressed();
		}
	}

	if(tallyShouldWait)
	{
		if(ignoreKeys==0 && (GetTaps()||GetArrowTaps()||GetDisplayMGL()->LastKeyPressed()))
			tallyShouldWait=0;

		tallyWait--;
		if(tallyWait)
			return 1;
		tallyWait=5;

	}
	switch(tallyMode)
	{
		case 255:
			tallyMode=0;
			break;
		case 0:
			showTally[0]=1;
			if(player.deliverBonus>0)
			{
				deliverBonus+=10;
				player.deliverBonus--;
				player.score+=10;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else
				tallyMode=1;
			break;
		case 1:
			showTally[1]=1;
			if(player.rescueBonus>0)
			{
				rescueBonus+=20;
				player.rescueBonus--;
				player.score+=20;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else
				tallyMode=2;
			break;
		case 2:
			showTally[2]=1;
			if(player.dumpBonus>0)
			{
				dumpBonus+=5;
				player.dumpBonus--;
				player.score+=5;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else
				tallyMode=3;
			break;
		case 3:
			showTally[3]=1;
			if(player.timeBonus>30)
			{
				timeBonus+=5*5;
				player.timeBonus-=5;
				player.score+=5*5;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else if(player.timeBonus>0)
			{
				timeBonus+=5;
				player.timeBonus--;
				player.score+=5;
				MakeNormalSound(SND_MENUCLICK);
			}
			else
				tallyMode=4;
			break;
		case 4:
			showTally[4]=1;
			if(player.escapeBonus)
			{
				escapeBonus+=1000;
				player.escapeBonus=0;
				player.score+=1000;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else
				tallyMode=5;
			break;
		case 5:
			showTally[5]=1;
			if(player.destructionBonus>150)
			{
				destructionBonus+=100;
				player.destructionBonus-=100;
				player.score+=100;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else if(player.destructionBonus>25)
			{
				destructionBonus+=10;
				player.destructionBonus-=10;
				player.score+=10;
				MakeNormalSound(SND_MENUCLICK);
			}
			else if(player.destructionBonus)
			{
				destructionBonus++;
				player.destructionBonus--;
				player.score++;
				MakeNormalSound(SND_MENUCLICK);
			}
			else
			{
				tallyMode=6;
			}
			break;
		case 6:	// collection bonus
			showTally[6]=1;
			if(player.pencilBonus>150)
			{
				collectBonus+=100;
				player.pencilBonus-=100;
				player.score+=100;
				
				MakeNormalSound(SND_MENUCLICK);
			}
			else if(player.pencilBonus>25)
			{
				collectBonus+=10;
				player.pencilBonus-=10;
				player.score+=10;
				MakeNormalSound(SND_MENUCLICK);
			}
			else if(player.pencilBonus)
			{
				collectBonus++;
				player.pencilBonus--;
				player.score++;
				MakeNormalSound(SND_MENUCLICK);
			}
			else
			{
				CheckForStars();
				tallyMode=7;
			}
			break;
		case 7:
			// star display
			if(pStarAdd)
			{
				pStarAdd--;
				LaunchStar(STARSRCX,STARSRCY);
			}
			else
			{
				if(!StarsLeft())
					tallyMode=8;
			}
			break;
		case 8:
			tallyShouldWait=0;
			if(pScore!=player.score)
				break;
			else
			{
				tallyMode=9;
				// clear those buffers
				GetTaps();
				GetArrowTaps();
				GetDisplayMGL()->LastKeyPressed();
			}
			break;
		case 9:
			if(GetTaps()||GetArrowTaps()||GetDisplayMGL()->LastKeyPressed())
			{
				SaveProfile(opt.curProfile);
				SaveOptions();
				return 0;
			}
			break;
	}

	return 1;
}

void PrintStat(int y,char *txt,int num)
{
	char s[16];

	sprintf(s,"%d",num);
	if(num==0)
	{
		PrintOutline(170,y,txt,0,0);
		RightPrintOutline(470,y,s,0,0);	
	}
	else
	{
		PrintUnGlow(170,y,txt,0);
		RightPrintUnGlow(470,y,s,0);
	}
}

void RenderTally(MGLDraw *mgl)
{
	char s[128];
	int i;

	if(!GM_doDraw)
		return;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	if(showTally[0])
		CenterPrintUnGlow(320,50,tallyMsg,0);

	if(showTally[0])
	{
		PrintStat(100,"Base Score",baseScore);
		PrintStat(140,"Delivery Bonus",deliverBonus);
	}
	if(showTally[1])
	{
		PrintStat(180,"Rescue Bonus",rescueBonus);
	}
	if(showTally[2])
	{
		PrintStat(220,"Disposal Bonus",dumpBonus);
	}
	if(showTally[3])
	{
		PrintStat(260,"Time Bonus",timeBonus);
	}
	if(showTally[4])
	{
		PrintStat(300,"Escape Bonus",escapeBonus);
	}
	if(showTally[5])
	{
		PrintStat(340,"Destruction Bonus",destructionBonus);
	}
	if(showTally[6])
	{
		PrintStat(380,"Collection Bonus",collectBonus);
	}

	if(showTally[0])
	{
		sprintf(s,"Total Score: %06d",pScore);
		CenterPrintUnGlow(320,420,s,0);

		RenderEndStar2(mgl);
		if(tallyMode>=7)
		{
			RenderStartStar(STARSRCX,STARSRCY,mgl);
			RenderStars(mgl);
		}
	}
}