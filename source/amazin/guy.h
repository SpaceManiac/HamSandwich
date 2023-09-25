#ifndef GUY_H
#define GUY_H

#include "jamultypes.h"

class sprite_t;

void GuysInit();
void GuysExit();
void GuysInitGfx();
void GuysExitGfx();

void TitleUpdateGuys();
void TitleInitGuys();

void GuysRender();
const sprite_t* GetGuySprite(byte type, byte frame);

#endif  // GUY_H
