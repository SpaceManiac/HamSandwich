#ifndef JAMULSPR_H
#define JAMULSPR_H

#include <stdio.h>
#include <vector>
#include <memory>
#include "jamultypes.h"

typedef struct SDL_RWops SDL_RWops;

class MGLDraw;

class sprite_t
{
public:
	sprite_t();
	sprite_t(byte *info);

	bool LoadData(SDL_RWops *f);
	bool SaveData(FILE *f) const;
	void GetHeader(byte *buffer) const;

	void Draw(int x, int y, MGLDraw *mgl) const;
	void DrawBright(int x, int y, MGLDraw *mgl, char bright) const;
	void DrawColored(int x, int y, MGLDraw *mgl, byte color, char bright) const;
	void DrawOffColor(int x, int y, MGLDraw *mgl, byte fromColor, byte toColor, char bright) const;
	void DrawGhost(int x, int y, MGLDraw *mgl, char bright) const;
	void DrawGlow(int x, int y, MGLDraw *mgl, char bright) const;
	// this makes half-height tilted black shadows (they darken by 4)
	void DrawShadow(int x, int y, MGLDraw *mgl) const;

	void GetCoords(int x, int y, int *rx, int *ry, int *rx2, int *ry2) const;

	word width;
	word height;
	int16_t ofsx;
	int16_t ofsy;
protected:
	dword size;
	std::vector<byte> data;
};

class sprite_set_t
{
public:
	sprite_set_t();
	sprite_set_t(const char *fname);

	bool Save(const char *fname) const;
	bool Load(const char *fname);
	sprite_t* GetSprite(int which);
	const sprite_t* GetSprite(int which) const;
	word GetCount() const;
protected:
	std::vector<std::unique_ptr<sprite_t>> spr;
};

void NewComputerSpriteFix(const char *fname);
// x2 and y2 are inclusive.
void SetSpriteConstraints(int x, int y, int x2, int y2);
void ClearSpriteConstraints();

byte SprModifyColor(byte color, byte hue);
byte SprGetColor(byte color);
byte SprModifyLight(byte color, char bright);
byte SprModifyGhost(byte src, byte dst, char bright);
byte SprModifyGlow(byte src, byte dst, char bright);

#endif
