#ifndef RECOLOR_H
#define RECOLOR_H

#include "jamultypes.h"

byte SprModifyColor(byte color, byte hue);
byte SprGetColor(byte color);
byte SprModifyLight(byte color, char bright);
byte SprModifyGhost(byte src, byte dst, char bright);
byte SprModifyGlow(byte src, byte dst, char bright);

#endif // RECOLOR_H
