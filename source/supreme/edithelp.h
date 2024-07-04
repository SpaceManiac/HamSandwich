#ifndef EDITHELP_H
#define EDITHELP_H

#include <stdio.h>
#include "display.h"

enum : byte
{
	HELP_BASIC,
	HELP_FLOORTOOL,
	HELP_WALLTOOL,
	HELP_ITEMTOOL,
	HELP_BADGUYTOOL,
	HELP_LIGHTTOOL,
	HELP_SPECIALTOOL,
	HELP_SELECTTOOL,
	HELP_ERASERTOOL,
	HELP_ITEMEDIT,
	HELP_ITEMPICK,
	HELP_MONSPICK,
	HELP_SOUNDEDIT,
	HELP_SOUNDPICK,
	HELP_SPECIALEDIT,
	HELP_TERRAINEDIT,
	HELP_TERRAINPICK,
	HELP_WORLDSTITCH,
	HELP_ITEMJSP,

	NUM_HELPS
};

void InitEditHelp(byte mode);
void ExitEditHelp(void);
void RenderEditHelp(int msx,int msy,MGLDraw *mgl);
byte EditHelpKey(char key);
byte EditHelpClick(int msx,int msy);

#endif
