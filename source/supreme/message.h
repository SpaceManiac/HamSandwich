#ifndef MESSAGE_H
#define MESSAGE_H

#include "display.h"

struct message_t
{
	int x,y;
	int dy;
	int timer;
	int bright;
	char brightDir;
	char msg[32];
	byte priority;
};

void InitMessage(void);
void NewMessage(const char *txt,int time,byte priority);
void NewBigMessage(const char *txt,int time);
void UpdateMessage(void);
void RenderMessage(void);
byte NoRepeatNewMessage(const char *txt,int time,byte priority);
const char *VariableMsg(const char *txt);

#endif
