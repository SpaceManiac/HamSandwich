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

byte SprModifyLight(byte color, char bright)
{
	byte value = (color & 31) + bright;
	if (value > 128) value = 0; // since byte is unsigned...
	else if (value > 31) value = 31;
	return (color & ~31) | value;
}

byte SprModifyGhost(byte src, byte dst, char bright)
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

byte SprModifyGlow(byte src, byte dst, char bright)
{
	return SprModifyLight(src, (dst & 31) + bright);
}
