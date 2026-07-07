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
	uint32_t dataSize;		// the size in bytes of the data of the characters themselves
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

HAM_API void FontInit(MGLDraw *mgl);
HAM_API void FontExit(void);

HAM_API void FontFree(mfont_t *font);

struct SDL_IOStream;
HAM_API FontError FontLoad(const char *fname, mfont_t *font);
HAM_API FontError FontLoad(SDL_IOStream* rw, mfont_t *font);
HAM_API FontError FontSave(const char *fname, const mfont_t *font);

HAM_API void FontPrintString(int x, int y, std::string_view s, const mfont_t *font);
HAM_API void FontPrintStringSolid(int x, int y, std::string_view s, const mfont_t *font, byte color);
HAM_API void FontPrintStringLimit(int x, int y, int maxX, std::string_view s, const mfont_t *font);
HAM_API void FontPrintStringSolidLimit(int x, int y, int maxX, std::string_view s, const mfont_t *font, byte color);
HAM_API void FontPrintStringDropShadow(int x, int y, std::string_view s, const mfont_t *font, byte shadowColor, byte shadowOffset);
HAM_API void FontPrintStringColor(int x, int y, std::string_view s, const mfont_t *font, byte color, char bright = 0);
HAM_API void FontPrintStringBright(int x, int y, std::string_view s, const mfont_t *font, char bright);
HAM_API void FontPrintStringItalicsBright(int x, int y, std::string_view s, const mfont_t* font, char bright);
HAM_API void FontPrintStringBrightLimit(int x, int y, int maxX, std::string_view s, const mfont_t *font, char bright);
HAM_API void FontPrintStringRect(int x, int y, int x2, int y2, std::string_view s, int height, const mfont_t *font);
HAM_API void FontPrintStringGlowRect(int x, int y, int x2, int y2, std::string_view s, int height, char bright, const mfont_t *font);
HAM_API void FontPrintStringGlow(int x, int y, std::string_view s, const mfont_t *font, char bright = 0);
HAM_API void FontPrintStringProgressiveGlow(int x, int y, std::string_view s, const mfont_t *font, int bright);
HAM_API void FontPrintStringGlowLimited(int x, int y, int maxX, std::string_view s, const mfont_t *font, char bright);
HAM_API void FontPrintStringUnGlowLimited(int x, int y, int maxX, std::string_view s, const mfont_t *font);
HAM_API void FontPrintStringUnGlow(int x, int y, std::string_view s, const mfont_t *font);
HAM_API void FontPrintStringUnGlowRect(int x, int y, int x2, int y2, std::string_view s, int height, const mfont_t *font);
HAM_API void FontPrintStringBrightGlow(int x, int y, std::string_view s, char brt, const mfont_t *font);
HAM_API void FontPrintStringDark(int x, int y, std::string_view s, const mfont_t *font);
HAM_API void FontPrintStringDarkAdj(int x, int y, std::string_view s, int dark, const mfont_t *font);
HAM_API void FontPrintStringCursorLit(int x,int y,byte pos,byte blink,std::string_view s,const mfont_t *font,char bright);
HAM_API void FontPrintStringAngleUnGlow(int x,int y,std::string_view s,const mfont_t *font);
HAM_API void FontPrintStringAngle2(int x,int y,std::string_view s,const mfont_t *font);
HAM_API void FontPrintStringUnGlowSideways(int x,int y,int minY,std::string_view s,const mfont_t *font);
HAM_API void FontPrintStringMultiline(int x, int y, std::string_view s, const mfont_t *font);

HAM_API void FontPrintRectBlack(int x, int y, int x2, int y2, std::string_view s, int height, int bright, const mfont_t *font);
HAM_API void FontPrintRectBlack2(int x, int y, int x2, int y2, std::string_view s, int height, const mfont_t *font);

HAM_API void FontPrintCharItalicsBright(int x, int y, dword c, char bright, const mfont_t* font);
HAM_API void FontPrintCharBright(int x, int y, dword c, char bright, const mfont_t* font);
HAM_API void FontPrintCharSolid(int x, int y, byte c, const mfont_t* font, byte color);
HAM_API byte CharWidth(dword c, const mfont_t* font);

HAM_API int FontStrLen(std::string_view s, const mfont_t *font);
HAM_API void FontSetColors(byte first, byte count, byte *data);
HAM_API TASK(bool) FontInputText(std::string_view prompt, char *buffer, int len, void (*renderScrn)(const mfont_t *), const mfont_t *font);

HAM_API byte RightBraceHack(mfont_t *font);

#endif
