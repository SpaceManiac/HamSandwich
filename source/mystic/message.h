#ifndef MESSAGE_H
#define MESSAGE_H

#include "winpch.h"
#include "display.h"

void InitMessage(void);
void NewMessage(const char *txt,int time);
void NewBigMessage(const char *txt,int time);
void UpdateMessage(void);
void RenderMessage(void);
void NoRepeatNewMessage(const char *txt,int time);


void InitSpeech(byte spc);
byte UpdateSpeech(MGLDraw *mgl);
void RenderSpeech(void);

#endif
