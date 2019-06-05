#include "message.h"
#include "sound.h"
#include "vars.h"
#include "progress.h"

message_t bigMessage;
message_t message;

const char *VariableMsg(const char *txt)
{
	static char out[256];
	int i,outpos,j;
	char num[16];

	outpos=0;
	for(i=0;i<(int)strlen(txt);i++)
	{
		out[outpos]='\0';
		if(txt[i]!='%')
		{
			out[outpos++]=txt[i];
			out[outpos]='\0';
		}
		else
		{
			if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && txt[i+2]=='N' && txt[i+3]=='M')
			{
				for(j=0;j<(int)strlen(profile.name);j++)
				{
					out[outpos++]=profile.name[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && txt[i+2]=='V')
			{
				sprintf(num,"%d",GetVar(VAR_LOCAL+txt[i+3]-'0'));
				for(j=0;j<(int)strlen(num);j++)
				{
					out[outpos++]=num[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && txt[i+2]=='G')
			{
				sprintf(num,"%d",GetVar(VAR_GLOBAL+txt[i+3]-'0'));
				for(j=0;j<(int)strlen(num);j++)
				{
					out[outpos++]=num[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && GetSpecialVarFunc(txt[i+2]))
			{
				sprintf(num,"%d",GetSpecialVarFunc(txt[i+2])(txt[i+3]));
				for(j=0;j<(int)strlen(num);j++)
				{
					out[outpos++]=num[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else	// must be a normal %
			{
				out[outpos++]='%';
			}
			out[outpos]='\0';
		}
		if(outpos>=255)
			break;
	}
	return out;
}

void InitMessage(void)
{
	message.msg[0]='\0';
}

void NewBigMessage(const char *txt,int time)
{
	strncpy(bigMessage.msg,VariableMsg(txt),32);
	bigMessage.x=320-GetStrLength(bigMessage.msg,0)/2;
	bigMessage.y=-100;
	bigMessage.dy=0;
	bigMessage.timer=time;
	bigMessage.bright=-32;
	bigMessage.brightDir=2;
}

void NewMessage(const char *txt,int time,byte priority)
{
	if(message.priority==1 && priority==0)
		return;	// can't override it
	strncpy(message.msg,VariableMsg(txt),32);
	message.x=2;
	message.y=484;
	message.dy=-13;
	message.timer=time;
	message.bright=-32;
	message.brightDir=2;
	message.priority=priority;
}

byte NoRepeatNewMessage(const char *txt,int time,byte priority)
{
	if(message.timer>priority && !strncmp(message.msg,txt,32))
		return 0;	// don't reset if showing the same message
	NewMessage(txt,time,1);
	return 1;
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
			message.priority=0;
		}
	}
}

void RenderMessage(void)
{
	char b;

	b=message.bright/2;
	Print(message.x,message.y,message.msg,b,0);
	b=bigMessage.bright/2;
	Print(bigMessage.x,bigMessage.y,bigMessage.msg,b,0);
}
