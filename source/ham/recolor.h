#ifndef RECOLOR_H
#define RECOLOR_H

#include "jamultypes.h"

byte SprGetColor(byte color);
byte SprModifyColor(byte color, byte hue);
byte SprModifyLight(byte color, int8_t bright);
byte SprModifyGlow(byte src, byte dst, int8_t bright);
// Ghostly parts are NOT affected by brightness.
// Default in Spooky Castle, Lunatic, Supreme, Eddie, Mystic, Sleepless.
byte SprModifyGhost(byte src, byte dst, int8_t bright);
// Ghostly parts ARE affected by brightness.
// Default in Stockboy, Loonyland, Loonyland 2.
byte SprModifyGhostBright(byte src, byte dst, int8_t bright);

#include "recolor.inl"

// Fully-general recolor map. Each input hue has an independent output hue and draw mode.
class Recolor
{
	static constexpr byte HUE_BITS = 3;
	static constexpr byte HUE_MASK = (1 << HUE_BITS) - 1;

	// Top 5 bits mode, bottom 3 bits hue.
	byte hueAndMode[8];
	// Only values from -31 to 31 are sensible as anything else will saturate.
	int8_t bright[8];

public:
	enum Hue : byte
	{
		Gray = 0,
		Green = 1,
		Brown = 2,
		Blue = 3,
		Red = 4,
		Yellow = 5,
		Purple = 6,
		Aqua = 7,
	};

	enum class Mode : byte
	{
		Normal = 0,
		Ghost = 1,
		Glow = 2,
		Loony = 3,
	};

	Recolor()
		: hueAndMode{0, 1, 2, 3, 4, 5, 6, 7}
		, bright{0, 0, 0, 0, 0, 0, 0, 0}
	{
	}

	static Recolor Bright(int8_t bright);
	static Recolor Colored(byte color, int8_t bright = 0);
	static Recolor OffColor(byte fromColor, byte toColor, int8_t bright = 0);
	static Recolor Ghost(int8_t bright = 0);
	static Recolor GhostBright(int8_t bright = 0);
	static Recolor Glow(int8_t bright = 0);
	static Recolor Loony(int8_t bright = 0);

	Hue GetHue(Hue fromHue);
	void SetHue(Hue fromHue, Hue toHue);

	Mode GetMode(Hue hue);
	void SetMode(Hue hue, Mode mode);

	int8_t GetBright(Hue hue) { return bright[hue]; }
	void SetBright(Hue hue, int8_t newBright) { bright[hue] = newBright; }

	byte Apply(byte color, byte underlying);
};

#endif // RECOLOR_H
