#ifndef MESSAGE_H
#define MESSAGE_H

#include "winpch.h"
#include "display.h"

typedef struct message_t
{
	int x,y;
	int dy;
	int timer;
	int bright;
	char brightDir;
	char msg[64];
} message_t;

void InitMessage(void);
void NewMessage(char *txt,int time);
void NewBigMessage(char *txt,int time);
void UpdateMessage(void);
void RenderMessage(void);
void NoRepeatNewMessage(char *txt,int time);

#endif