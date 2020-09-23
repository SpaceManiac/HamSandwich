#ifndef EDITHELP_H
#define EDITHELP_H

#include <stdio.h>
#include "display.h"

#define HELP_BASIC		0
#define HELP_FLOORTOOL	1
#define HELP_WALLTOOL	2
#define HELP_ITEMTOOL	3
#define HELP_BADGUYTOOL	4
#define HELP_LIGHTTOOL	5
#define HELP_SPECIALTOOL 6
#define HELP_SELECTTOOL 7
#define HELP_ERASERTOOL 8
#define HELP_ITEMEDIT	9
#define HELP_ITEMPICK	10
#define HELP_MONSPICK	11
#define HELP_SOUNDEDIT	12
#define HELP_SOUNDPICK	13
#define HELP_SPECIALEDIT 14
#define HELP_TERRAINEDIT 15
#define HELP_TERRAINPICK 16
#define HELP_WORLDSTITCH	17
#define HELP_WORLDLOAD	18
#define NUM_HELPS		19

void InitEditHelp(byte mode);
void ExitEditHelp(void);
void RenderEditHelp(int msx,int msy,MGLDraw *mgl);
byte EditHelpKey(char key);
byte EditHelpClick(int msx,int msy);

#endif
