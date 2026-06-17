// Inline definitions so that these color routines can be fully optimized.
#include "jamultypes.h"

// ----------------------------------------------------------------------------
// Basic color manipulation

inline byte SprGetColor(byte color)
{
	return (color >> 5);
}

inline byte SprModifyColor(byte color, byte hue)
{
	return (hue << 5) | (color & 31);
}

inline byte SprModifyLight(byte color, int8_t bright)
{
	byte value = (color & 31);
	if (value <= -bright) value = 0;
	else if (31 - value <= bright) value = 31;
	else value += bright;
	return (color & ~31) | value;
}

inline byte SprModifyGlow(byte src, byte dst, int8_t bright)
{
	return SprModifyLight(src, (dst & 31) + bright);
}

inline byte SprModifyGhost(byte src, byte dst, int8_t bright)
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

inline byte SprModifyGhostBright(byte src, byte dst, int8_t bright)
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
