#ifndef JAMULFONT_H
#define JAMULFONT_H

#include <stddef.h>
#include <string_view>
#include "jamultypes.h"
#include "coro.h"

class MGLDraw;

const int FONT_MAX_CHARS = 128;

struct mfont_t
{
	byte numChars;		// # of characters in the font
	byte firstChar;		// the first character's ASCII value (they ascend from there)
	byte height;		// height in pixels of the font
	byte spaceSize;		// # of pixels wide to make spaces
	byte gapSize;		// # of pixels between adjacent letters
	byte gapHeight;		// # of pixels to descend for a carriage return
	int32_t dataSize;		// the size in bytes of the data of the characters themselves
	byte *data;			// pointer to the character data
	byte *chars[FONT_MAX_CHARS]; // pointers to each character's data (can't have more than FONT_MAX_CHARS)
};

// each character in the font is stored as:
// width    1 byte       width of the character in pixels
// data     width*height bytes of actual data

// error codes
enum FontError
{
	FONT_OK = 0,
	FONT_FILENOTFOUND,
	FONT_CANTALLOC,
	FONT_INVALIDFILE
};

void FontInit(MGLDraw *mgl);
void FontExit(void);

void FontFree(mfont_t *font);

struct SDL_RWops;
FontError FontLoad(const char *fname, mfont_t *font);
FontError FontLoad(SDL_RWops* rw, mfont_t *font);
FontError FontSave(const char *fname, const mfont_t *font);

void FontPrintString(int x, int y, std::string_view s, const mfont_t *font);
void FontPrintStringSolid(int x, int y, std::string_view s, const mfont_t *font, byte color);
void FontPrintStringLimit(int x, int y, int maxX, std::string_view s, const mfont_t *font);
void FontPrintStringSolidLimit(int x, int y, int maxX, std::string_view s, const mfont_t *font, byte color);
void FontPrintStringDropShadow(int x, int y, std::string_view s, const mfont_t *font, byte shadowColor, byte shadowOffset);
void FontPrintStringColor(int x, int y, std::string_view s, const mfont_t *font, byte color, char bright = 0);
void FontPrintStringBright(int x, int y, std::string_view s, const mfont_t *font, char bright);
void FontPrintStringBrightLimit(int x, int y, int maxX, std::string_view s, const mfont_t *font, char bright);
void FontPrintStringRect(int x, int y, int x2, int y2, std::string_view s, int height, const mfont_t *font);
void FontPrintStringGlowRect(int x, int y, int x2, int y2, std::string_view s, int height, char bright, const mfont_t *font);
void FontPrintStringGlow(int x, int y, std::string_view s, const mfont_t *font, char bright = 0);
void FontPrintStringProgressiveGlow(int x, int y, std::string_view s, const mfont_t *font, int bright);
void FontPrintStringGlowLimited(int x, int y, int maxX, std::string_view s, const mfont_t *font, char bright);
void FontPrintStringUnGlowLimited(int x, int y, int maxX, std::string_view s, const mfont_t *font);
void FontPrintStringUnGlow(int x, int y, std::string_view s, const mfont_t *font);
void FontPrintStringUnGlowRect(int x, int y, int x2, int y2, std::string_view s, int height, const mfont_t *font);
void FontPrintStringBrightGlow(int x, int y, std::string_view s, char brt, const mfont_t *font);
void FontPrintStringDark(int x, int y, std::string_view s, const mfont_t *font);
void FontPrintStringDarkAdj(int x, int y, std::string_view s, int dark, const mfont_t *font);
void FontPrintStringCursorLit(int x,int y,byte pos,byte blink,std::string_view s,const mfont_t *font,char bright);
void FontPrintStringAngleUnGlow(int x,int y,std::string_view s,const mfont_t *font);
void FontPrintStringAngle2(int x,int y,std::string_view s,const mfont_t *font);
void FontPrintStringUnGlowSideways(int x,int y,int minY,std::string_view s,const mfont_t *font);
void FontPrintStringMultiline(int x, int y, std::string_view s, const mfont_t *font);

void FontPrintRectBlack(int x, int y, int x2, int y2, std::string_view s, int height, int bright, const mfont_t *font);
void FontPrintRectBlack2(int x, int y, int x2, int y2, std::string_view s, int height, const mfont_t *font);

int FontStrLen(std::string_view s, const mfont_t *font);
void FontSetColors(byte first, byte count, byte *data);
TASK(bool) FontInputText(std::string_view prompt, char *buffer, int len, void (*renderScrn)(const mfont_t *), const mfont_t *font);

byte RightBraceHack(mfont_t *font);

#endif
