#include "message.h"
#include "sound.h"

message_t bigMessage;
message_t message;

void InitMessage(void)
{
	bigMessage.msg[0]='\0';
	message.msg[0]='\0';
}

void NewBigMessage(const char *txt,int time)
{
	strncpy(bigMessage.msg,txt,32);
	bigMessage.x=320-GetStrLength(bigMessage.msg,2)/2;
	bigMessage.y=-100;
	bigMessage.dy=0;
	bigMessage.timer=time;
	bigMessage.bright=0;
	bigMessage.brightDir=2;
}

void NewMessage(const char *txt,int time)
{
	strncpy(message.msg,txt,32);
	message.x=2;
	message.y=484;
	message.dy=-13;
	message.timer=time;
	message.bright=0;
	message.brightDir=2;
}

void NoRepeatNewMessage(const char *txt,int time)
{
	if(!strncmp(message.msg,txt,32))
		return;	// don't reset if showing the same message
	NewMessage(txt,time);
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
			bigMessage.dy=-bigMessage.dy/2;
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
		if(bigMessage.y>480)
		{
			bigMessage.msg[0]='\0';
			bigMessage.y=0;
			bigMessage.dy=0;
		}
	}
}

void UpdateMessage(void)
{
	UpdateBigMessage();

	message.y+=message.dy;
	message.dy+=1;
	message.bright+=message.brightDir;

	if(message.timer)
		message.timer--;
	else
		message.brightDir=-2;

	// while time still remains, don't start falling offscreen
	if(message.timer)
	{
		if(message.dy>0)
			message.dy=0;

		if(message.bright>=32)
			message.brightDir=-2;
		if(message.brightDir<0 && message.bright<0)
		{
			message.brightDir=0;
			message.bright=0;
		}
	}
	else	// go ahead and fall
	{
		if(message.y>480)
		{
			message.msg[0]='\0';
			message.y=0;
			message.dy=0;
		}
	}
}

void RenderMessage(void)
{
	char b;

	b=message.bright/2;
	if(b<0)
	{
		PrintColor(message.x,message.y,message.msg,4,b,2);
	}
	else
		Print(message.x,message.y,message.msg,b,2);

	b=bigMessage.bright/2;
	if(b<0)
	{
		PrintColor(bigMessage.x,bigMessage.y,bigMessage.msg,4,b,2);
	}
	else
		Print(bigMessage.x,bigMessage.y,bigMessage.msg,b,2);
}
