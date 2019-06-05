#ifndef YESNODIALOG_H
#define YESNODIALOG_H

#include <stdio.h>
#include "display.h"

#define YNM_YES		1
#define YNM_NO		2

void InitYesNoDialog(const char *text,const char *choice1,const char *choice2);
void ExitYesNoDialog(void);
void RenderYesNoDialog(int msx,int msy,MGLDraw *mgl);
void RenderYesNoDialog2(int msx,int msy,MGLDraw *mgl);
void RenderYesNoDialog3(int msx,int msy,MGLDraw *mgl);
byte YesNoDialogKey(char key);
byte YesNoDialogClick(int msx,int msy);

byte YesNoDialogCommand(void);

#endif
