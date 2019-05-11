#ifndef SPCLDIALOG_H
#define SPCLDIALOG_H

#include "winpch.h"
#include "display.h"
#include "world.h"
#pragma pack(1)

void InitSpclDialog(special_t *theSpcl,byte spnum,world_t *wrld,byte currentMap);
void ExitSpclDialog(void);
void RenderSpclDialog(int msx,int msy,MGLDraw *mgl);
byte SpclDialogKey(char key);
byte SpclDialogClick(int msx,int msy);

#endif
