#ifndef MESSAGE_H
#define MESSAGE_H

#include "winpch.h"
#include "display.h"
#pragma pack(1)

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
void NewMessage(const char *txt,int time);
void NewBigMessage(const char *txt,int time,byte type);
void UpdateMessage(void);
void RenderMessage(void);
void NoRepeatNewMessage(const char *txt,int time);
void LogMessage(const char *s);
void RenderMessagesPause(int x,int y);
void MessageDisable(byte b);

#endif
