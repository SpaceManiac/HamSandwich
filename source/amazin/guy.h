#ifndef GUY_H
#define GUY_H

#include "jamultypes.h"

enum class GuyType
{
	None,
	Bouapha,
	Bouaphetta,
	Smoove,
	Chuckles,
	Helga,
	Pete,
};

class sprite_t;

void GuysInit();
void GuysExit();
void GuysInitGfx();
void GuysExitGfx();

void GuyAdd(byte x, byte y, GuyType type);

void GuysUpdate();
void TitleUpdateGuys();
void TitleInitGuys();
void CongratsInitGuys();

void GuysRender();
const sprite_t* GetGuySprite(byte type, byte frame);

#endif  // GUY_H
