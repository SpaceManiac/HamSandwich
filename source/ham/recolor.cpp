#include "recolor.h"

// ----------------------------------------------------------------------------
// Basic color manipulation

byte SprModifyColor(byte color, byte hue)
{
	return (hue << 5) | (color & 31);
}

byte SprGetColor(byte color)
{
	return (color >> 5);
}

byte SprModifyLight(byte color, int8_t bright)
{
	byte value = (color & 31) + bright;
	if (value > 128) value = 0; // since byte is unsigned...
	else if (value > 31) value = 31;
	return (color & ~31) | value;
}

// Ghostly parts are NOT affected by brightness.
// Default in Spooky Castle, Lunatic, Supreme, Eddie, Mystic, Sleepless.
byte SprModifyGhost(byte src, byte dst, int8_t bright)
{
	if (src >> 5 == 0)
	{
		return SprModifyLight(dst, src);
	}
	else
	{
		return SprModifyLight(src, bright);
	}
}

// Ghostly parts ARE affected by brightness.
// Default in Stockboy, Loonyland, Loonyland 2.
byte SprModifyGhostBright(byte src, byte dst, int8_t bright)
{
	if (src >> 5 == 0)
	{
		return SprModifyLight(dst, (int8_t)src + bright);
	}
	else
	{
		return SprModifyLight(src, bright);
	}
}

byte SprModifyGlow(byte src, byte dst, int8_t bright)
{
	return SprModifyLight(src, (dst & 31) + bright);
}
