#ifndef BACKGD_H
#define BACKGD_H

#include "jamultypes.h"

void InitBackgd(void);
void ExitBackgd(void);
void LoadBackgd(char *name);
void LoadStatus(void);
void RenderBackgd(byte *scr);
void RenderStatusBar(byte *scr);
void UpdateBackgd(void);
void SpecialRenderBackgd(byte *scr,int y);

void SetBackScroll(byte on);

#endif