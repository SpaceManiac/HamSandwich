#ifndef VIEWDIALOG_H
#define VIEWDIALOG_H

#include "display.h"
#include "world.h"

void InitViewDialog(void);
void ExitViewDialog(void);
void RenderViewDialog(int msx,int msy,MGLDraw *mgl);
byte ViewDialogClick(int msx,int msy);
void ReturnToSong(void);

#endif
