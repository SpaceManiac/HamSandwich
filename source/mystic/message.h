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
	char msg[32];
} message_t;

void InitSpeechSystem(void);

void InitMessage(void);
void NewMessage(const char *txt,int time);
void NewBigMessage(const char *txt,int time);
void UpdateMessage(void);
void RenderMessage(void);
void NoRepeatNewMessage(const char *txt,int time);

void InitFarley(void);
byte UpdateFarley(MGLDraw *mgl);
void RenderFarley(void);
byte FarleyWorldChoice(void);
void OutlineFarleySprite(word spr, int x, int y, char bright);

void InitSpeech(byte spc);
byte UpdateSpeech(MGLDraw *mgl);
void RenderSpeech(void);

void RenderBasicControls(void);
void RenderSpellControls(void);

#endif
