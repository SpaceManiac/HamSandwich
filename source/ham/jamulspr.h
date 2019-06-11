#ifndef JAMULSPR_H
#define JAMULSPR_H

#include "jamultypes.h"
#include <stdio.h>

typedef struct SDL_RWops SDL_RWops;

class MGLDraw;

class sprite_t
{
public:
	sprite_t();
	sprite_t(byte *info);

	~sprite_t();

	bool LoadData(SDL_RWops *f);
	bool SaveData(FILE *f);
	void GetHeader(byte *buffer);
	void Draw(int x, int y, MGLDraw *mgl);

	void DrawBright(int x, int y, MGLDraw *mgl, char bright);
	void DrawColored(int x, int y, MGLDraw *mgl, byte color, char bright);
	void DrawOffColor(int x, int y, MGLDraw *mgl, byte fromColor, byte toColor, char bright);
	void DrawGhost(int x, int y, MGLDraw *mgl, char bright);
	void DrawGlow(int x, int y, MGLDraw *mgl, char bright);
	void DrawC(int x, int y, MGLDraw *mgl);

	// this makes half-height tilted black shadows (they darken by 4)
	void DrawShadow(int x, int y, MGLDraw *mgl);
	void GetCoords(int x, int y, int *rx, int *ry, int *rx2, int *ry2);

	word width;
	word height;
	short ofsx;
	short ofsy;
protected:
	dword size;
	byte *data;
};

class sprite_set_t
{
public:
	sprite_set_t();
	sprite_set_t(const char *fname);

	~sprite_set_t();

	bool Save(const char *fname);
	bool Load(const char *fname);
	sprite_t *GetSprite(int which);
	word GetCount();
protected:
	void Free();

	word count;
	sprite_t **spr;
};

void NewComputerSpriteFix(const char *fname);
void SetSpriteConstraints(int x, int y, int x2, int y2);

#endif
