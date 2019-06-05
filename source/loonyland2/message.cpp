#include "message.h"
#include "sound.h"
#include "pause.h"
#include "player.h"

message_t bigMessage;
byte bigType;
static byte messageDisable=0;

void InitMessage(void)
{
	bigMessage.msg[0]='\0';
	bigMessage.y=-100;
	bigMessage.timer=0;
}

void NewBigMessage(const char *txt,int time,byte type)
{
	if(messageDisable)
		return;

	strncpy(bigMessage.msg,txt,64);
	bigMessage.x=320-GetStrLength(bigMessage.msg,0)/2;
	bigMessage.y=-100;
	bigMessage.dy=0;
	bigMessage.timer=time;
	bigMessage.bright=0;
	bigMessage.brightDir=2;
	bigType=type;
}

void NewMessage(const char *txt,int time)
{
	if(messageDisable)
		return;

	MakeNormalSound(SND_MESSAGE);
	LogMessage(txt);
}

void NoRepeatNewMessage(const char *txt,int time)
{
	if(messageDisable)
		return;

	if(!strcmp(player.log[MAX_LOG-1],txt) && player.logTime>0)
		return;

	MakeNormalSound(SND_MESSAGE);
	LogMessage(txt);
}

void UpdateBigMessage(void)
{
	bigMessage.y+=bigMessage.dy;
	bigMessage.dy+=2;
	bigMessage.bright+=bigMessage.brightDir;

	if(bigMessage.timer)
		bigMessage.timer--;
	else
		bigMessage.brightDir=-2;

	// while time still remains, don't start falling offscreen
	if(bigMessage.timer)
	{
		if(bigMessage.y>200)
		{
			bigMessage.y=200;
			bigMessage.dy=-bigMessage.dy/4;
			if(bigMessage.dy>-2)
				bigMessage.dy=0;
		}
		if(bigMessage.bright>=32)
			bigMessage.brightDir=-1;
		if(bigMessage.brightDir<0 && bigMessage.bright<0)
		{
			bigMessage.brightDir=0;
			bigMessage.bright=0;
		}
	}
	else	// go ahead and fall
	{
		bigMessage.dy-=4;
		if(bigMessage.y<-50)
		{
			bigMessage.msg[0]='\0';
			bigMessage.y=0;
			bigMessage.x=0;
			bigMessage.dy=0;
		}
	}
}

void UpdateMessage(void)
{
	UpdateBigMessage();

	if(player.logTime)
	{
		player.logTime--;
		if(player.logBright<32)
			player.logBright+=2;
		if(player.logBright>32)
			player.logBright=32;
	}
	else if(player.logBright)
		player.logBright--;
}

void RenderMessage(void)
{
	char b;

	b=bigMessage.bright/2;
	if(bigMessage.x>0)
	{
		GetIntfaceSprite(23+bigType)->DrawShadow(320,250,GetDisplayMGL());
		GetIntfaceSprite(23+bigType)->Draw(320,bigMessage.y+30,GetDisplayMGL());
		if(b<0)
			Print(bigMessage.x,bigMessage.y,bigMessage.msg,b,0);
		else
			Print(bigMessage.x,bigMessage.y,bigMessage.msg,b,0);
	}

	// render last 4 lines of log
	int i,wid;
	if(player.logBright==0)
		return;

	wid=0;
	for(i=0;i<4;i++)
		if(GetStrLength(player.log[MAX_LOG-i-1],1)>wid)
			wid=GetStrLength(player.log[MAX_LOG-i-1],1);

	wid+=4;
	GetDisplayMGL()->DarkBox(1,480-14*4-2,wid,480-2,(player.logBright/4));
	for(i=0;i<4;i++)
	{
		PrintGlow(2,480-14*(i+1),player.log[MAX_LOG-i-1],-32+player.logBright,1);
	}
}

void RenderMessagesPause(int x,int y)
{
	int i;

	PauseBox(x+202,y+5,x+638,y+428,234);
	for(i=0;i<MAX_LOG;i++)
	{
		PrintDark(x+208,y+429-(i+1)*14,player.log[MAX_LOG-1-i],1);
		Print(x+207,y+428-(i+1)*14,player.log[MAX_LOG-1-i],0,1);
	}
}

void LogMessage(const char *s)
{
	int i;

	player.logTime=180;
	for(i=0;i<MAX_LOG-1;i++)
	{
		memcpy(player.log[i],player.log[i+1],sizeof(char)*LOG_LEN);
	}
	strncpy(player.log[MAX_LOG-1],s,LOG_LEN);
	player.log[MAX_LOG-1][LOG_LEN-1]='\0';
}

void MessageDisable(byte b)
{
	messageDisable=b;
}
