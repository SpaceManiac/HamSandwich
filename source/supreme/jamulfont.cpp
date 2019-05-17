#include "jamulfont.h"
#include <stdio.h>


MGLDraw *fontmgl;
// this is a sort of palette translation table for the font
byte fontPal[256];

void FontInit(MGLDraw *mgl)
{
	int i;

	fontmgl=mgl;
	// default translation is none for the font palette
	for(i=0;i<256;i++)
		fontPal[i]=(byte)i;
}

void FontExit(void)
{
}

void FontFree(mfont_t *font)
{
	if(font->data)
		free(font->data);
}

int FontLoad(char *fname,mfont_t *font)
{
	FILE *f;
	int i;

	f=fopen(fname,"rb");
	if(!f)
		return FONT_FILENOTFOUND;

	if(fread(font,sizeof(mfont_t),1,f)!=1)
		return FONT_INVALIDFILE;

	font->data=(byte *)malloc(font->dataSize);
	if(!font->data)
		return FONT_CANTALLOC;

	if(fread(font->data,font->dataSize,1,f)!=1)
		return FONT_INVALIDFILE;

	fclose(f);
	font->chars[0]=font->data;
	for(i=1;i<font->numChars;i++)
		font->chars[i]=font->chars[i-1]+1+((*font->chars[i-1])*font->height);

	return FONT_OK;
}

int FontSave(char *fname,mfont_t *font)
{
	FILE *f;

	f=fopen(fname,"wb");
	if(!f)
		return FONT_FILENOTFOUND;

	if(fwrite(font,sizeof(mfont_t),1,f)!=1)
		return FONT_INVALIDFILE;

	if(fwrite(font->data,font->dataSize,1,f)!=1)
		return FONT_INVALIDFILE;

	fclose(f);
	return FONT_OK;
}

void FontPrintChar(int x, int y, char c, mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c-=(char)font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>=0) && (x<scrWidth) && (y>=0) && (y<scrHeight))
				*dst=fontPal[*src];
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

void FontPrintCharColor(int x, int y, char c,byte color,mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c-=(char)font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	color*=32;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>=0) && (x<scrWidth) && (y>=0) && (y<scrHeight))
			{
				if((*src>=64 && *src<64+32) || (*src>=128 && *src<128+32))
					*dst=((*src)&31)+color;
				else
					*dst=*src;
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


void FontPrintCharBright(int x, int y, char c,char bright,mfont_t *font)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c-=(char)font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;

	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>=0) && (x<scrWidth) && (y>=0) && (y<scrHeight))
			{
				*dst=*src+bright;
				if(*dst>(*src&(~31))+31)
					*dst=(*src&(~31))+31;
				else if(*dst<(*src&(~31)))
					*dst=*src&(~31);
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

void FontPrintCharSolid(int x, int y, char c, mfont_t *font, byte color)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c-=(char)font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if(*src && (x>=0) && (x<scrWidth) && (y>=0) && (y<scrHeight))
				*dst=color;
			dst++;
			src++;
			x++;
		}
		y++;
		x-=chrWidth;
		dst+=(scrWidth-chrWidth);
	}
}

void FontPrintCharGlow(int x, int y, char c, mfont_t *font,int bright)
{
	byte *dst,*src;
	int scrWidth,scrHeight,chrWidth;
	int i,j;
	int v;

	scrWidth=fontmgl->GetWidth();
	scrHeight=fontmgl->GetHeight();
	dst=fontmgl->GetScreen()+x+y*scrWidth;

	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return; // unprintable

	c-=(char)font->firstChar;

	chrWidth=*(font->chars[c]);
	src=font->chars[c]+1;
	for(j=0;j<font->height;j++)
	{
		for(i=0;i<(*font->chars[c]);i++)
		{
			if((*src>0) && (x>=0) && (x<scrWidth) && (y>=0) && (y<scrHeight))
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

void FontPrintCharUnGlow(int x, int y, char c, mfont_t *font)
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

	c-=(char)font->firstChar;

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

void FontPrintCharUnGlowLimited(int x, int y, int maxX, char c, mfont_t *font)
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

	c-=(char)font->firstChar;

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

void FontPrintCharGlowLimited(int x, int y, int maxX, char c, mfont_t *font,int bright)
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

	c-=(char)font->firstChar;

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

byte CharWidth(char c, mfont_t *font)
{
	if(c<font->firstChar || c>=(font->firstChar+font->numChars))
		return font->spaceSize; // unprintable

	c-=(char)font->firstChar;

	return *(font->chars[c]);
}

void FontPrintString(int x,int y,char *s,mfont_t *font)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintChar(x,y,s[i],font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringGlow(int x,int y,char *s,mfont_t *font,char bright)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharGlow(x,y,s[i],font,bright);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringGlowLimited(int x,int y,int maxX,char *s,mfont_t *font,char bright)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharGlowLimited(x,y,maxX,s[i],font,bright);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringProgressiveGlow(int x,int y,char *s,mfont_t *font,int bright)
{
	int i;
	int br;

	br=bright+strlen(s)*4;
	for(i=0;i<(int)strlen(s);i++)
	{
		if(br<0)
			FontPrintCharGlow(x,y,s[i],font,br);
		else if(br<20)
			FontPrintCharGlow(x,y,s[i],font,br);
		else if(br<40)
			FontPrintCharGlow(x,y,s[i],font,40-br);
		else
			FontPrintCharGlow(x,y,s[i],font,0);
		x+=CharWidth(s[i],font)+font->gapSize;
		br-=4;
	}
}

void FontPrintStringUnGlow(int x,int y,char *s,mfont_t *font)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharUnGlow(x,y,s[i],font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringRect(int x,int y,int x2,int y2,char *s,int height,mfont_t *font)
{
	int i,tx,ty;
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
			for(i=0;i<(int)strlen(tok);i++)
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

void FontPrintStringGlowRect(int x,int y,int x2,int y2,char *s,int height,char bright,mfont_t *font)
{
	int i,tx,ty;
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
			for(i=0;i<(int)strlen(tok);i++)
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

void FontPrintStringUnGlowRect(int x,int y,int x2,int y2,char *s,int height,mfont_t *font)
{
	int i,tx,ty;
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
			for(i=0;i<(int)strlen(tok);i++)
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

void FontPrintStringLimit(int x,int y,int maxX,char *s,mfont_t *font)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
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

void FontPrintStringColor(int x,int y,char *s,mfont_t *font,byte color)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharColor(x,y,s[i],color,font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringBright(int x,int y,char *s,mfont_t *font,char bright)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharBright(x,y,s[i],bright,font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringBrightLimit(int x,int y,int maxX,char *s,mfont_t *font,char bright)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
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

void FontPrintStringSolid(int x,int y,char *s,mfont_t *font,byte color)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharSolid(x,y,s[i],font,color);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontPrintStringSolidLimit(int x,int y,int maxX,char *s,mfont_t *font,byte color)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		if(x+CharWidth(s[i],font)<maxX)
		{
			FontPrintCharSolid(x,y,s[i],font,color);
			x+=CharWidth(s[i],font)+font->gapSize;
		}
		else break;
	}
}

void FontPrintStringDropShadow(int x,int y,char *s,mfont_t *font,byte shadowColor,byte shadowOffset)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
	{
		FontPrintCharSolid(x+shadowOffset,y+shadowOffset,s[i],font,shadowColor);
		FontPrintChar(x,y,s[i],font);
		x+=CharWidth(s[i],font)+font->gapSize;
	}
}

void FontSetColors(byte first,byte count,byte *data)
{
	memcpy(&fontPal[first],data,count);
}

int FontStrLen(char *s,mfont_t *font)
{
	int i,len=0;

	for(i=0;i<(int)strlen(s);i++)
	{
		len+=CharWidth(s[i],font)+font->gapSize;
	}
	return len;
}

bool FontInputText(char *prompt,char *buffer,int len,void (*renderScrn)(mfont_t *),mfont_t *font)
{
	int pos=0;
	bool done=0;
	char c;

	while(buffer[pos] && pos<len)
		pos++;
	while(!done)
	{
		renderScrn(font);
		fontmgl->FillBox(0,200,639,250,0);
		fontmgl->Box(0,200,639,250,255);
		FontPrintString(2,202,prompt,font);
		buffer[pos]='_';
		buffer[pos+1]='\0';
		FontPrintString(2,202+font->height+2,buffer,font);
		buffer[pos]='\0';
		fontmgl->Flip();
		if(!fontmgl->Process())
			return false;
		if((c=fontmgl->LastKeyPressed()))
		{
			if(c==8) // backspace
			{
				if(pos>0)
				{
					pos--;
					buffer[pos]='\0';
				}
			}
			else if(c==27)
			{
				done=true;
				buffer[0]='\0';
			}
			else if(c==13)
			{
				done=true;
				buffer[pos]='\0';
			}
			else if(pos<len)
			{
				buffer[pos++]=c;
				buffer[pos]='\0';
			}
		}
	}
	return true;
}
