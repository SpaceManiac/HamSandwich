#include "bossbash.h"
#include "options.h"
#include "badge.h"
#include "plasma.h"
#include "log.h"

static byte oldc;
static byte cursor;
static byte power;
static byte pickMode;
static byte victim;

#define POWERX	80
#define VICTIMX 380

dword parTime[]={
	3*60*30,	// frankenjulie
	3*60*30,	// polterguy
	4*60*30,	// happy stick
	3*60*30,	// larry
	5*60*30,	// gusty
	5*60*30,	// stoney
	5*60*30,	// drippy
	5*60*30,	// toasty
	12*60*30,	// evilizer
	7*60*30,	// bonkula
	4*60*30,	// frankenfury
	5*60*30,	// haunted halls
	5*60*30,	// larry & friends
	4*60*30,	// pumpkins
	6*60*30,	// boss party
};

char levelName[][32]={
	"Frankenjulie",
	"Polterguy",
	"Happy Stick Witch",
	"Larry",
	"Gusty The Elder",
	"Stoney The Elder",
	"Drippy The Elder",
	"Toasty The Elder",
	"The Evilizer",
	"Count Bonkula",
	"Frankenfury",
	"Haunted Halls",
	"Larry & Friends",
	"Return to SPISPOPD",
	"Wild Boss Party 2001",
	"Exit",
};

byte bossReq[]={
	255,
	BOSS_FRANK,
	BOSS_GHOST,
	BOSS_WITCH,
	BOSS_LARRY,
	BOSS_ELDER1,
	BOSS_ELDER2,
	BOSS_ELDER3,
	BOSS_ELDER4,
	BOSS_EVILIZER,
	BOSS_BONKULA,
	BOSS_FRANK,
	BOSS_GHOST,
	BOSS_LARRY,
	255,
	BOSS_EVILIZER,
};

char pwrName[7][32]={"Puny","Weakling","Normal","Mighty","Vast","Jaw-Dropping","Exit"};

char *BashPowerName(byte lvl)
{
	return pwrName[lvl];
}

char *BashLevelName(byte lvl)
{
	return levelName[lvl];
}

void InitBossMenu(void)
{
	oldc=255;
	cursor=0;
	power=0;
	pickMode=0;
	victim=255;
	InitPlasma(6);
}

void ExitBossMenu(void)
{
	SaveOptions();
	ExitPlasma();
}

byte UpdateBossMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		c=mgl->LastKeyPressed();
		c2=GetControls()|GetArrows();

		if(c==27)
		{
			victim=255;
			if(pickMode==0)
				return 1;
			else
				pickMode=0;
		}

		if((c2&CONTROL_UP) && (!(oldc&CONTROL_UP)))
		{
			if(pickMode==0)
			{
				power--;
				if(power>6)
					power=6;
			}
			else
			{
				victim--;
				if(victim==0)
					victim=curWorld.numMaps;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
		{
			if(pickMode==0)
			{
				power++;
				if(power>6)
					power=0;
			}
			else
			{
				victim++;
				if(victim>curWorld.numMaps)
					victim=1;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c2&(CONTROL_B1|CONTROL_B2|CONTROL_B3)) && (!(oldc&(CONTROL_B1|CONTROL_B2|CONTROL_B3))))
		{
			if(pickMode==0)
			{
				if(power==6)
					return 1;	// exit
				pickMode=1;
				victim=1;
			}
			else
			{
				if(victim==curWorld.numMaps)
					return 255;	// exit
				else
				{
					if(bossReq[victim]==255 || opt.bossDead[bossReq[victim]])
					{
						return victim;
					}
					else
					{
						MakeNormalSound(SND_PENALTYOPEN);
					}
				}
			}
			MakeNormalSound(SND_MENUSELECT);
		}
		UpdatePlasma();
		oldc=c2;

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

void RenderBossMenu(MGLDraw *mgl)
{
	int i;
	int wid;
	int pos;

	wid=mgl->GetWidth();
	pos=(int)mgl->GetScreen()+40*wid;
	for(i=40;i<480-40;i++)
	{
		memset((byte *)pos,6*32+2,640);
		pos+=wid;
	}

	RenderPlasma(mgl,1);
	RenderPlasma(mgl,0);

	CenterPrintGlow(320,-2,"Boss Bash",0,2);
	DrawBox(0,47,639,48,31);
	DrawBox(0,75,639,75,31);

	RenderLightningParticle(-5,77+power*25+15,POWERX+3,77+power*25+15,20,10,mgl->GetScreen());
	if(pickMode==1)
	{
		RenderLightningParticle(POWERX-5+GetStrLength(pwrName[power],0),77+power*25+15,VICTIMX+3,77-25+victim*25+15,25,10,mgl->GetScreen());
		RenderLightningParticle(VICTIMX-5+GetStrLength(levelName[victim-1],0),77-25+victim*25+15,645,77-25+victim*25+15,20,10,mgl->GetScreen());
	}

	if(pickMode==0)
	{
		PrintGlow(POWERX,49,"Select Power Level",0,0);
	}
	else
		PrintColor(POWERX,49,"Power Level",6,-10,0);
	for(i=0;i<7;i++)
	{
		if(power!=i)
			PrintColor(POWERX,77+i*25,pwrName[i],6,-10-10*(pickMode==1),0);
		else
		{
			PrintColor(POWERX-1,77+i*25-1,pwrName[i],6,-10,0);
			PrintColor(POWERX+1,77+i*25+1,pwrName[i],6,-10,0);
			PrintGlow(POWERX,77+i*25,pwrName[i],0,0);
		}
	}
	if(pickMode==1)
		PrintGlow(VICTIMX,49,"Select Victim",0,0);
	else
		PrintColor(VICTIMX,49,"Victims",6,-10,0);

	for(i=1;i<curWorld.numMaps;i++)
	{
		if(bossReq[i]==255 || opt.bossDead[bossReq[i]])
		{
			if(victim!=i)
				PrintColor(VICTIMX,77-25+i*25,curWorld.map[i]->name,6,-10,0);
			else
			{
				PrintColor(VICTIMX-1,77-25+i*25-1,curWorld.map[i]->name,6,-10,0);
				PrintColor(VICTIMX+1,77-25+i*25+1,curWorld.map[i]->name,6,-10,0);
				PrintGlow(VICTIMX,77-25+i*25,curWorld.map[i]->name,0,0);
			}
		}
		else
		{
			if(victim!=i)
				PrintColor(VICTIMX,77-25+i*25,"??????",6,-10,0);
			else
			{
				PrintColor(VICTIMX-1,77-25+i*25-1,"??????",6,-10,0);
				PrintColor(VICTIMX+1,77-25+i*25+1,"??????",6,-10,0);
				PrintGlow(VICTIMX,77-25+i*25,"??????",0,0);
			}
		}
	}
	if(victim!=i)
		PrintColor(VICTIMX,77-25+i*25,"Exit",6,-10,0);
	else
	{
		//PrintColor(320-1,77-25+i*25-1,"Exit",6,-10,0);
		//PrintColor(320+1,77-25+i*25+1,"Exit",6,-10,0);
		PrintGlow(VICTIMX,77-25+i*25,"Exit",0,0);
	}
}

byte BossMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	DBG("InitBossMenu");
	InitBossMenu();

	lastTime=1;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateBossMenu(&lastTime,mgl);
		RenderBossMenu(mgl);
		mgl->Flip();

		if(mgl->LastKeyPressed()==27)
		{
			victim=255;
			done=1;
		}

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	DBG("ExitBossMenu");
	ExitBossMenu();

	if(victim==255)
		return 0;

	return victim;
}

byte BashPower(void)
{
	return power;
}

void UpdateBash(void)
{
	if(!AnyMonsterExists())
	{
		DBG("SendWinLevel");
		player.numSaves=parTime[player.levelNum-1];
		SendMessageToGame(MSG_WINLEVEL,0);
	}
}
