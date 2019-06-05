#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <stdio.h>
#include "display.h"

#define TM_OKAY		1
#define TM_CANCEL	2

void InitTextDialog(const char *title,const char *contents,byte maxLen);
void ExitTextDialog(void);
void RenderTextDialog(int msx,int msy,MGLDraw *mgl);
void RenderTextDialog2(int msx,int msy,MGLDraw *mgl);

byte TextDialogKey(char key);
byte TextDialogClick(int msx,int msy);

const char *GetText(void);
byte TextDialogCommand(void);

#endif
