#ifndef RECOLOR_H
#define RECOLOR_H

#include "jamultypes.h"

byte SprModifyColor(byte color, byte hue);
byte SprGetColor(byte color);
byte SprModifyLight(byte color, int8_t bright);
byte SprModifyGhost(byte src, byte dst, int8_t bright);
byte SprModifyGhostBright(byte src, byte dst, int8_t bright);
byte SprModifyGlow(byte src, byte dst, int8_t bright);

#endif // RECOLOR_H
