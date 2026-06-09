#include "recolor.h"
#include <string.h>
#include <algorithm>
#include <SDL3/SDL_timer.h>

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

// ----------------------------------------------------------------------------

Recolor Recolor::Bright(int8_t bright)
{
	Recolor result;
	memset(result.bright, bright, sizeof(result.bright));
	return result;
}

Recolor Recolor::Colored(byte color, int8_t bright)
{
	Recolor result;
	memset(result.hueAndMode, color, sizeof(result.hueAndMode));
	memset(result.bright, bright, sizeof(result.bright));
	return result;
}

Recolor Recolor::OffColor(byte fromColor, byte toColor, int8_t bright)
{
	Recolor result;
	result.hueAndMode[fromColor] = toColor;
	memset(result.bright, bright, sizeof(result.bright));
	return result;
}

Recolor Recolor::Ghost(int8_t bright)
{
	Recolor result;
	result.hueAndMode[Gray] = (byte)Mode::Ghost << HUE_BITS;
	memset(result.bright, bright, sizeof(result.bright));
	result.bright[Gray] = 0;
	return result;
}

Recolor Recolor::GhostBright(int8_t bright)
{
	Recolor result;
	result.hueAndMode[Gray] = (byte)Mode::Ghost << HUE_BITS;
	memset(result.bright, bright, sizeof(result.bright));
	// GhostBright is when you DON'T set result.bright[Gray] = 0;
	return result;
}

Recolor Recolor::Glow(int8_t bright)
{
	Recolor result;
	for (int i = 0; i < 7; ++i)
	{
		result.hueAndMode[i] |= (byte)Mode::Glow << HUE_BITS;
	}
	memset(result.bright, bright, sizeof(result.bright));
	return result;
}

Recolor Recolor::Loony(int8_t bright)
{
	Recolor result;
	memset(result.hueAndMode, (byte)Mode::Loony << HUE_BITS, sizeof(result.hueAndMode));
	memset(result.bright, bright, sizeof(result.bright));
	return result;
}

Recolor::Hue Recolor::GetHue(Hue fromHue)
{
	return (Hue)(hueAndMode[fromHue] & HUE_MASK);
}

void Recolor::SetHue(Hue fromHue, Hue toHue)
{
	hueAndMode[fromHue] = (hueAndMode[fromHue] & ~HUE_MASK) | (byte)toHue;
}

Recolor::Mode Recolor::GetMode(Hue hue)
{
	return (Mode)((hueAndMode[hue] & ~HUE_MASK) >> HUE_BITS);
}

void Recolor::SetMode(Hue hue, Mode mode)
{
	hueAndMode[hue] = (hueAndMode[hue] & HUE_MASK) | ((byte)mode << HUE_BITS);
}

byte Recolor::Apply(byte color, byte underlying)
{
	byte hue = hueAndMode[color >> 5] & HUE_MASK;
	Mode mode = Mode(hueAndMode[color >> 5] >> HUE_BITS);
	uint8_t brightChange = bright[color >> 5];

	switch (mode)
	{
		default:
		case Mode::Normal:
			return SprModifyLight(SprModifyColor(color, hue), brightChange);
		case Mode::Ghost:
			return SprModifyLight(underlying, (color & 31) + brightChange);
		case Mode::Glow:
			return SprModifyLight(color, (underlying & 31) + brightChange);
		case Mode::Loony:
		{
			// NOTE: differs from Keys of Lunacy in two ways:
			// 1. Keys do not animate while the game is paused.
			// 2. Keys animate at double speed in Manic Mode.
			// Fixing this is left as an exercise to whoever actually uses this feature.
			uint64_t timer = SDL_GetTicks() * 30 / 1000;
			brightChange += abs((int)(16 - (timer & 31)));
			return SprModifyLight(SprModifyColor(color, (timer & 255) / 32), brightChange);
		}
	}
}
