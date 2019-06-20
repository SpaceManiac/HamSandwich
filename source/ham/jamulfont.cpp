#include "jamulfont.h"
#include "mgldraw.h"
#include "appdata.h"
#include <stdio.h>

MGLDraw *fontmgl;
// this is a sort of palette translation table for the font
byte fontPal[256];

void FontInit(MGLDraw *mgl)
{
	int i;

	fontmgl = mgl;
	// default translation is none for the font palette
	for (i = 0; i < 256; i++)
		fontPal[i] = (byte) i;
}

void FontExit(void)
{
}

void FontFree(mfont_t *font)
{
	if (font->data)
		free(font->data);
}

int FontLoad(const char *fname, mfont_t *font)
{
	SDL_RWops* f = AssetOpen_SDL(fname, "rb");
	if (!f)
		return FONT_FILENOTFOUND;

	if (SDL_RWread(f, font, sizeof(mfont_t), 1) != 1) {
		SDL_RWclose(f);
		return FONT_INVALIDFILE;
	}

	font->data = (byte *) malloc(font->dataSize);
	if (!font->data) {
		SDL_RWclose(f);
		return FONT_CANTALLOC;
	}

	if (SDL_RWread(f, font->data, font->dataSize, 1) != 1) {
		SDL_RWclose(f);
		return FONT_INVALIDFILE;
	}

	SDL_RWclose(f);
	font->chars[0] = font->data;
	for (int i = 1; i < font->numChars; i++)
		font->chars[i] = font->chars[i - 1] + 1 + ((*font->chars[i - 1]) * font->height);

	return FONT_OK;
}

int FontSave(char *fname, mfont_t *font)
{
	FILE *f;

	f = AssetOpen(fname, "wb");
	if (!f)
		return FONT_FILENOTFOUND;

	if (fwrite(font, sizeof (mfont_t), 1, f) != 1)
		return FONT_INVALIDFILE;

	if (fwrite(font->data, font->dataSize, 1, f) != 1)
		return FONT_INVALIDFILE;

	fclose(f);
	AppdataSync();
	return FONT_OK;
}

static void FontPrintChar(int x, int y, dword c, mfont_t *font)
{
	byte *dst, *src;
	int scrWidth, scrHeight, chrWidth;
	int i, j;

	scrWidth = fontmgl->GetWidth();
	scrHeight = fontmgl->GetHeight();
	dst = fontmgl->GetScreen() + x + y*scrWidth;

	if (c < font->firstChar || c >= (font->firstChar + font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth = *(font->chars[c]);
	src = font->chars[c] + 1;
	for (j = 0; j < font->height; j++)
	{
		for (i = 0; i < (*font->chars[c]); i++)
		{
			if (*src && (x >= 0) && (x < scrWidth) && (y >= 0) && (y < scrHeight))
				*dst = fontPal[*src];
			dst++;
			src++;
			x++;
		}
		y++;
		x -= chrWidth;
		dst += (scrWidth - chrWidth);
	}
}

static void FontPrintCharDark(int x, int y, dword c, mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;
	int b1,b2;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>0) && (x<scrWidth) && (y>0) && (y<scrHeight))
			{
				b2=(fontPal[*src]&31);
				if(b2>0)
				{
					b1=((*dst)&31);
					if(b1>b2)
						b1-=b2;
					else
						b1=0;
					*dst=((*dst)&(~31))+b1;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static void FontPrintCharDark2(int x, int y, dword c, byte howdark, mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;
	int b1,b2;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>0) && (x<scrWidth) && (y>0) && (y<scrHeight))
			{
				b2=(fontPal[*src]&31);
				if(b2>howdark)
					b2-=howdark;
				else
					b2=0;
				if(b2>0)
				{
					b1=((*dst)&31);
					if(b1>b2)
						b1-=b2;
					else
						b1=0;
					*dst=((*dst)&(~31))+b1;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static void FontPrintCharColor(int x, int y, dword c, byte color, char bright, mfont_t *font)
{
	byte *dst, *src;
	int scrWidth, scrHeight, chrWidth;
	int i, j;

	scrWidth = fontmgl->GetWidth();
	scrHeight = fontmgl->GetHeight();
	dst = fontmgl->GetScreen() + x + y*scrWidth;

	if (c < font->firstChar || c >= (font->firstChar + font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth = *(font->chars[c]);
	src = font->chars[c] + 1;
	color *= 32;
	for (j = 0; j < font->height; j++)
	{
		for (i = 0; i < (*font->chars[c]); i++)
		{
			if (*src && (x >= 0) && (x < scrWidth) && (y >= 0) && (y < scrHeight))
			{
				byte b = (*src + bright);
				if((b&(~31))!=((*src)&(~31)))
				{
					if(bright>0)
						b=31;
					else
						b=0;
				}
				*dst=(b&31)+color;
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x -= chrWidth;
		dst += (scrWidth - chrWidth);
	}
}

static void FontPrintCharBright(int x, int y, dword c, char bright, mfont_t *font)
{
	byte *dst, *src;
	int scrWidth, scrHeight, chrWidth;
	int i, j;

	scrWidth = fontmgl->GetWidth();
	scrHeight = fontmgl->GetHeight();
	dst = fontmgl->GetScreen() + x + y*scrWidth;

	if (c < font->firstChar || c >= (font->firstChar + font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth = *(font->chars[c]);
	src = font->chars[c] + 1;

	for (j = 0; j < font->height; j++)
	{
		for (i = 0; i < (*font->chars[c]); i++)
		{
			if (*src && (x >= 0) && (x < scrWidth) && (y >= 0) && (y < scrHeight))
			{
				*dst = *src + bright;
				if ((*dst & (~31)) != (*src & (~31)))
				{
					if (bright > 0)
						*dst = *src | 31;
					else
						*dst = *src & (~31);
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x -= chrWidth;
		dst += (scrWidth - chrWidth);
	}
}

static void FontPrintCharSolid(int x, int y, byte c, mfont_t *font, byte color)
{
	byte *dst, *src;
	int scrWidth, scrHeight, chrWidth;
	int i, j;

	scrWidth = fontmgl->GetWidth();
	scrHeight = fontmgl->GetHeight();
	dst = fontmgl->GetScreen() + x + y*scrWidth;

	if (c < font->firstChar || c >= (font->firstChar + font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth = *(font->chars[c]);
	src = font->chars[c] + 1;
	for (j = 0; j < font->height; j++)
	{
		for (i = 0; i < (*font->chars[c]); i++)
		{
			if (*src && (x >= 0) && (x < scrWidth) && (y >= 0) && (y < scrHeight))
				*dst = color;
			dst++;
			src++;
			x++;
		}
		y++;
		x -= chrWidth;
		dst += (scrWidth - chrWidth);
	}
}

static void FontPrintCharGlow(int x, int y, dword c, int bright, mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>0) && (x<scrWidth) && (y>0) && (y<scrHeight))
			{
				int b2=(fontPal[*src]&31)+bright;
				if(b2>0)
				{
					int b1=((*dst)&31)+b2;
					if(b1>31)
						b1=31;
					*dst=((*dst)&(~31))+b1;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static void FontPrintCharBrightGlow(int x, int y, dword c, char brt, mfont_t *font)
{
	byte *dst,*src,b;
	int scrWidth,scrHeight,chrWidth;
	int i,j;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>0) && (x<scrWidth) && (y>0) && (y<scrHeight) &&
				(((*src)&31)+brt>0))
			{

				b=*dst+((*src)&31)+brt;
				if((b&(~31))!=((*dst)&(~31)))
					*dst=((*dst)&(~31))+31;
				else
				{
					if(b>*dst)	// don't plot it if it would darken
						*dst=b;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static void FontPrintCharUnGlow(int x, int y, dword c, mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;
	int b1,b2;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>=0) && (x<scrWidth) && (y>=0) && (y<scrHeight))
			{
				b2=(fontPal[*src]&31);
				if(b2>0)
				{
					b1=((*dst)&31)-b2;
					if(b1<0)
						b1=0;
					*dst=((*dst)&(~31))+b1;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static void FontPrintCharUnGlowLimited(int x, int y, int maxX, dword c, mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;
	int b1,b2;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>=0) && (x<maxX) && (y>=0) && (y<scrHeight))
			{
				b2=(fontPal[*src]&31);
				if(b2>0)
				{
					b1=((*dst)&31)-b2;
					if(b1<0)
						b1=0;
					*dst=((*dst)&(~31))+b1;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static void FontPrintCharGlowLimited(int x, int y, int maxX, dword c, mfont_t *font,int bright)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;
	int v;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(maxX>639)
		maxX=639;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c -= font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if((*src>0) && (x>=0) && (x<maxX) && (y>=0) && (y<scrHeight))
			{
				v=(*src)&31;
				v+=bright;
				if(v>0)
				{
					v+=(*dst)&31;
					if(v>31)
						*dst=((*dst)&(~31))+31;
					else
						*dst=((*dst)&(~31))+v;
				}
			}
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

static byte CharWidth(dword c, mfont_t *font)
{
	if (c < font->firstChar || c >= (font->firstChar + font->numChars))
		return font->spaceSize; // unprintable

	c -= font->firstChar;

	return *(font->chars[c]);
}

void FontPrintString(int x, int y, const char *s, mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintChar(x, y, s[i], font);
		x += CharWidth(s[i], font) + font->gapSize;
	}
}

void FontPrintStringColor(int x, int y, const char *s, mfont_t *font, byte color, char bright)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharColor(x, y, s[i], color, bright, font);
		x += CharWidth(s[i], font) + font->gapSize;
	}
}

void FontPrintStringDark(int x,int y,const char *s,mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharDark(x,y,s[i],font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringDarkAdj(int x,int y, const char *s,int dark,mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		int d = dark + (int) i;
		if (d < 0)
			d = 0;
		else if (d > 32)
			d = 32;
		FontPrintCharGlow(x-1,y-1,s[i],0,font);
		FontPrintCharGlow(x+1,y-1,s[i],0,font);
		FontPrintCharGlow(x-1,y+1,s[i],0,font);
		FontPrintCharGlow(x+1,y+1,s[i],0,font);
		FontPrintCharDark2(x,y,s[i],d,font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringBright(int x, int y, const char *s, mfont_t *font, char bright)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharBright(x, y, s[i], bright, font);
		x += CharWidth(s[i], font) + font->gapSize;
	}
}

void FontPrintStringSolid(int x, int y, const char *s, mfont_t *font, byte color)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharSolid(x, y, s[i], font, color);
		x += CharWidth(s[i], font) + font->gapSize;
	}
}

void FontPrintStringGlow(int x, int y, const char *s, mfont_t *font, char bright)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharGlow(x, y, s[i], bright, font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringBrightGlow(int x, int y, const char *s, char brt, mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharBrightGlow(x,y,s[i],brt,font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringDropShadow(int x, int y, const char *s, mfont_t *font, byte shadowColor, byte shadowOffset)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharSolid(x + shadowOffset, y + shadowOffset, s[i], font, shadowColor);
		FontPrintChar(x, y, s[i], font);
		x += CharWidth(s[i], font) + font->gapSize;
	}
}

void FontPrintRectBlack(int x,int y,int x2,int y2, const char *s,int height,int bright,mfont_t *font)
{
	int tx,ty,len;
	char *tmp;
	char *tok;

	tmp=(char *)malloc(strlen(s)+2);
	strcpy(tmp,s);

	tx=x+2;
	ty=y+2;
	tok=strtok(tmp," \n\t");
	while(tok)
	{
		if(tok[0]=='^')
		{	// carriage return
			tx=x+2;
			ty+=height;
			if(ty>y2-height-2)
			{
				free(tmp);
				return;	// no more room
			}
		}
		else
		{
			len=FontStrLen(tok,font);
			if(tx+len>x2-2)
			{
				tx=x+2;
				ty+=height;
				if(ty>y2-height-2)
				{
					FontPrintStringDarkAdj(tx,ty,"+",bright,font);
					free(tmp);
					return;	// no more room
				}
			}
			FontPrintStringDarkAdj(tx,ty,tok,bright,font);
			bright+=(strlen(tok)+1);
			tx+=len;
			tx+=font->spaceSize;
		}
		tok=strtok(NULL," \n\t");
		if(bright>32)
		{
			free(tmp);
			return;
		}
	}
	free(tmp);
}

void FontPrintRectBlack2(int x,int y,int x2,int y2, const char *s,int height,mfont_t *font)
{
	int tx,ty,len;
	char *tmp;
	char *tok;

	tmp=(char *)malloc(strlen(s)+2);
	strcpy(tmp,s);

	tx=x+2;
	ty=y+2;
	tok=strtok(tmp," \n\t");
	while(tok)
	{
		if(tok[0]=='^')
		{	// carriage return
			tx=x+2;
			ty+=height;
			if(ty>y2-height-2)
			{
				free(tmp);
				return;	// no more room
			}
		}
		else
		{
			len=FontStrLen(tok,font);
			if(tx+len>x2-2)
			{
				tx=x+2;
				ty+=height;
				if(ty>y2-height-2)
				{
					FontPrintStringDark(tx,ty,"+",font);
					free(tmp);
					return;	// no more room
				}
			}
			FontPrintStringDark(tx,ty,tok,font);
			tx+=len;
			tx+=font->spaceSize;
		}
		tok=strtok(NULL," \n\t");
	}
	free(tmp);
}

void FontSetColors(byte first, byte count, byte *data)
{
	memcpy(&fontPal[first], data, count);
}

int FontStrLen(const char *s, mfont_t *font)
{
	int width = 0;
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		width += CharWidth(s[i], font) + font->gapSize;
	}
	return width;
}

bool FontInputText(const char *prompt, char *buffer, int len, void (*renderScrn)(mfont_t *), mfont_t *font)
{
	int pos = 0;
	bool done = 0;
	char c;

	while (buffer[pos] && pos < len)
		pos++;
	while (!done)
	{
		renderScrn(font);
		fontmgl->FillBox(0, 200, 639, 250, 0);
		fontmgl->Box(0, 200, 639, 250, 255);
		FontPrintString(2, 202, prompt, font);
		buffer[pos] = '_';
		buffer[pos + 1] = '\0';
		FontPrintString(2, 202 + font->height + 2, buffer, font);
		buffer[pos] = '\0';
		fontmgl->Flip();
		if (!fontmgl->Process())
			return false;
		if ((c = fontmgl->LastKeyPressed())) // extra pair of parentheses for a warning about assignment in truth value
		{
			if (c == 8) // backspace
			{
				if (pos > 0)
				{
					pos--;
					buffer[pos] = '\0';
				}
			}
			else if (c == 27)
			{
				done = true;
				buffer[0] = '\0';
			}
			else if (c == 13)
			{
				done = true;
				buffer[pos] = '\0';
			}
			else if (pos < len)
			{
				buffer[pos++] = c;
				buffer[pos] = '\0';
			}
		}
	}
	return true;
}

void FontPrintStringLimit(int x,int y,int maxX,const char*s,mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		if(x+CharWidth(s[i],font)<maxX)
		{
			FontPrintChar(x,y,s[i],font);
			x+=CharWidth(s[i],font)+font->gapSize;
		}
		else
			break;
	}
}

void FontPrintStringSolidLimit(int x,int y,int maxX,const char*s,mfont_t *font,byte color)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		if(x+CharWidth(s[i],font)<maxX)
		{
			FontPrintCharSolid(x,y,s[i],font,color);
			x+=CharWidth(s[i],font)+font->gapSize;
		}
		else break;
	}
}

void FontPrintStringBrightLimit(int x,int y,int maxX,const char*s,mfont_t *font,char bright)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		if(x+CharWidth(s[i],font)<maxX)
		{
			FontPrintCharBright(x,y,s[i],bright,font);
			x+=CharWidth(s[i],font)+font->gapSize;
		}
		else
			break;
	}
}

void FontPrintStringRect(int x,int y,int x2,int y2,const char*s,int height,mfont_t *font)
{
	int tx,ty;
	char *tmp;
	char *tok;

	tmp=(char *)malloc(strlen(s)+2);
	strcpy(tmp,s);

	tx=x+2;
	ty=y+2;
	tok=strtok(tmp," \n\t");
	while(tok)
	{
		if(tok[0]=='^')
		{	// carriage return
			tx=x+2;
			ty+=height;
			if(ty>y2-height-2)
			{
				free(tmp);
				return;	// no more room
			}
		}
		else
		{
			if(tx+FontStrLen(tok,font)>x2-2)
			{
				tx=x+2;
				ty+=height;
				if(ty>y2-height-2)
				{
					free(tmp);
					return;	// no more room
				}
			}
			size_t len = strlen(tok);
			for(size_t i=0; i<len; i++)
			{
				FontPrintChar(tx,ty,tok[i],font);
				tx+=CharWidth(tok[i],font)+font->gapSize;
			}
			tx+=CharWidth(' ',font)+font->gapSize;
		}
		tok=strtok(NULL," \n\t");
	}
	free(tmp);
}

void FontPrintStringGlowRect(int x,int y,int x2,int y2,const char*s,int height,char bright,mfont_t *font)
{
	int tx,ty;
	char *tmp;
	char *tok;

	tmp=(char *)malloc(strlen(s)+2);
	strcpy(tmp,s);

	tx=x+2;
	ty=y+2;
	tok=strtok(tmp," \n\t");
	while(tok)
	{
		if(tok[0]=='^')
		{	// carriage return
			tx=x+2;
			ty+=height;
			if(ty>y2-height-2)
			{
				free(tmp);
				return;	// no more room
			}
		}
		else
		{
			if(tx+FontStrLen(tok,font)>x2-2)
			{
				tx=x+2;
				ty+=height;
				if(ty>y2-height-2)
				{
					free(tmp);
					return;	// no more room
				}
			}
			size_t len = strlen(tok);
			for (size_t i = 0; i < len; ++i)
			{
				FontPrintCharGlowLimited(tx,ty,x2,tok[i],font,bright);
				tx+=CharWidth(tok[i],font)+font->gapSize;
			}
			tx+=CharWidth(' ',font)+font->gapSize;
		}
		tok=strtok(NULL," \n\t");
	}
	free(tmp);
}

void FontPrintStringGlowLimited(int x,int y,int maxX,const char*s,mfont_t *font,char bright)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharGlowLimited(x,y,maxX,s[i],font,bright);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringProgressiveGlow(int x,int y,const char*s,mfont_t *font,int bright)
{
	size_t len = strlen(s);
	int br=bright+len*4;
	for (size_t i = 0; i < len; ++i)
	{
		if(br<0)
			FontPrintCharGlow(x,y,s[i],br,font);
		else if(br<20)
			FontPrintCharGlow(x,y,s[i],br,font);
		else if(br<40)
			FontPrintCharGlow(x,y,s[i],40-br,font);
		else
			FontPrintCharGlow(x,y,s[i],0,font);
		x+=CharWidth(s[i],font)+font->gapSize;
		br-=4;
	}
}

void FontPrintStringUnGlow(int x,int y,const char*s,mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharUnGlow(x,y,s[i],font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringUnGlowRect(int x,int y,int x2,int y2,const char *s,int height,mfont_t *font)
{
	int tx,ty;
	char *tmp;
	char *tok;

	tmp=(char *)malloc(strlen(s)+2);
	strcpy(tmp,s);

	tx=x+2;
	ty=y+2;
	tok=strtok(tmp," \n\t");
	while(tok)
	{
		if(tok[0]=='^')
		{	// carriage return
			tx=x+2;
			ty+=height;
			if(ty>y2-height-2)
			{
				free(tmp);
				return;	// no more room
			}
		}
		else
		{
			if(tx+FontStrLen(tok,font)>x2-2)
			{
				tx=x+2;
				ty+=height;
				if(ty>y2-height-2)
				{
					free(tmp);
					return;	// no more room
				}
			}
			size_t len = strlen(tok);
			for (size_t i = 0; i < len; ++i)
			{
				FontPrintCharUnGlowLimited(tx,ty,x2,tok[i],font);
				tx+=CharWidth(tok[i],font)+font->gapSize;
			}
			tx+=CharWidth(' ',font)+font->gapSize;
		}
		tok=strtok(NULL," \n\t");
	}
	free(tmp);
}

void FontPrintStringUnGlowLimited(int x,int y,int maxX,const char*s,mfont_t *font)
{
	size_t len = strlen(s);
	for (size_t i = 0; i < len; ++i)
	{
		FontPrintCharUnGlowLimited(x,y,maxX,s[i],font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

byte RightBraceHack(mfont_t *font)
{
	static byte RIGHT_BRACE[] = {
		5,
		31, 31, 0, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 0, 31, 31,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		0, 0, 31, 0, 0,
		31, 31, 0, 0, 0,
		0, 0, 0, 0, 0};

	byte result = font->firstChar + font->numChars;
	font->chars[font->numChars++] = font->chars['}' - font->firstChar];
	font->chars['}' - font->firstChar] = RIGHT_BRACE;
	return result;
}
