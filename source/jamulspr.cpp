
#include "jamulspr.h"

// the sprites are 12 bytes, not including the data itself
#define SPRITE_INFO_SIZE 16

/*
Jamul Sprite - JSP

header:
count		1 word	how many frames in this sprite
data:
count structures:
	width	1 word		width of sprite in pixels
	height	1 word		height of sprite in pixels
	ofsX	1 short		x-coord of hotspot relative to left
	ofsY	1 short		y-coord of hotspot relative to top
	size	1 dword		how big the sprite data is in bytes
	data	size bytes	transparency RLE'd sprite data

	The RLE format is as follows:

	count	1 byte	if count is positive, this is considered
			a run of data, negative is a run of
			transparency.  If the run is data, it is
			followed by count bytes of data.  If
			it is transparent, the next RLE tag
			simply follows it.
			Runs do not cross line boundaries.
*/

// -------------------------------------------------------------------------
// ****************************** SPRITE_T *********************************
// -------------------------------------------------------------------------

// CONSTRUCTORS & DESTRUCTORS
sprite_t::sprite_t(void)
{
	width=0;
	height=0;
	ofsx=0;
	ofsy=0;
	size=0;
	data=NULL;
}

sprite_t::sprite_t(byte *info)
{
	memcpy(&width,&info[0],2);
	memcpy(&height,&info[2],2);
	memcpy(&ofsx,&info[4],2);
	memcpy(&ofsy,&info[6],2);
	memcpy(&size,&info[8],4);
}

sprite_t::~sprite_t(void)
{
	if(data)
		free(data);
}

// REGULAR MEMBER FUNCTIONS
bool sprite_t::LoadData(FILE *f)
{
	if(size==0)
		return TRUE;

	data=(byte *)malloc(size);
	if(!data)
		return FALSE;

	if(fread(data,1,size,f)!=size)
	{
		return FALSE;
	}
	return TRUE;
}

bool sprite_t::SaveData(FILE *f)
{
	if(size==0)
		return TRUE;

	if(!data)
		return TRUE;

	if(fwrite(data,1,size,f)!=size)
	{
		return FALSE;
	}
	return TRUE;
}

void sprite_t::GetHeader(byte *buffer)
{
	memcpy(&buffer[0],&width,2);
	memcpy(&buffer[2],&height,2);
	memcpy(&buffer[4],&ofsx,2);
	memcpy(&buffer[6],&ofsy,2);
	memcpy(&buffer[8],&size,4);
}

void sprite_t::GetCoords(int x,int y,int *rx,int *ry,int *rx2,int *ry2)
{
	*rx=x-ofsx;
	*ry=y-ofsy;
	*rx2=*rx+width;
	*ry2=*ry+height;
}

void sprite_t::Draw(int x,int y,MGLDraw *mgl)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;

	x-=ofsx;
	y-=ofsy;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth;

	spr=data;
	sprHeight=height;
	sprWidth=width;

	if(y>mgl->GetHeight()-height)
		sprHeight=mgl->GetHeight()-y;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		xor eax,eax			// zero out eax because we want it to work as a byte
loop1:
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		rep movsb			// move the data
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		rep movsb			// move the data
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		rep movsb			// move the data (ecx was already set up)
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again
linedone:
		dec sprHeight
		jz alldone			// All done!!!
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		add edi,ecx			// add one line's height
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)
void sprite_t::DrawBright(int x,int y,MGLDraw *mgl,char bright)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;

	if(bright==0)
	{
		// don't waste time!
		Draw(x,y,mgl);
		return;
	}

	x-=ofsx;
	y-=ofsy;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth;

	spr=data;
	sprHeight=height;
	sprWidth=width;

	if(y>mgl->GetHeight()-height)
		sprHeight=mgl->GetHeight()-y;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		xor eax,eax			// zero out eax because we want it to work as a byte
loop1:
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		call movedata
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		call movedata
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe	dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		call movedata
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again

movedata:					// here's the core of it.  when this is jumped to,
							// ecx = count to transfer
							// ebx = unusable (sprite width is contained there)
							// edx = unusable (draw counter is stored there)
							// esi,edi = src, dst ptrs
		push ebx
		mov  bh,bright
		xor eax,eax
movedataloop:
		mov al,es:[esi]
		cmp bh,0
		je  okay
		jg  brighten
		mov bl,al
		and bl,(~31)		// bl is al's color range
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay1
		mov al,bl			// yes, put it to it's min value
		jmp okay
okay1:
		add bl,31			// it didn't, let's see if it wrapped instead
		cmp al,bl
		jbe okay
		and bl,(~31)		// al gets darkened to 0.  This checks for wrap problems
		mov al,bl
		jmp okay
brighten:
		mov bl,al			// bl is al's color range
		and bl,(~31)
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay2
		mov al,bl
		add al,31			// make al max brightness.  This is a wrap issue
		jmp okay
okay2:
		add bl,31			// now see if it went over
		cmp al,bl
		jbe okay
		mov al,bl			// it did, so lock it to max
okay:
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		xor eax,eax
		pop ebx
		ret
linedone:
		dec sprHeight
		jz alldone			// All done!!!
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		add edi,ecx			// add one line's height
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

//	 color:  which hue (0-7) to use for the entire thing, ignoring its real hue
//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)
void sprite_t::DrawColored(int x,int y,MGLDraw *mgl,byte color,char bright)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;

	x-=ofsx;
	y-=ofsy;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth;

	spr=data;
	sprHeight=height;
	sprWidth=width;

	if(y>mgl->GetHeight()-height)
		sprHeight=mgl->GetHeight()-y;

	color*=32;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		xor eax,eax			// zero out eax because we want it to work as a byte
loop1:
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		call movedata
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		call movedata
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe	dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		call movedata
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again

movedata:					// here's the core of it.  when this is jumped to,
							// ecx = count to transfer
							// ebx = unusable (sprite width is contained there)
							// edx = unusable (draw counter is stored there)
							// esi,edi = src, dst ptrs
		push ebx
		push edx
		mov  dl,color		// hold the hue to be used
		mov  bh,bright
		xor eax,eax
movedataloop:
		mov al,es:[esi]
		and al,31			// strip off the hue of al
		or	al,dl			// and stick on the new hue

		cmp bh,0
		je  okay
		jg  brighten
		mov bl,al
		and bl,(~31)		// bl is al's color range
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay1
		mov al,bl			// yes, put it to it's min value
		jmp okay
okay1:
		add bl,31			// it didn't, let's see if it wrapped instead
		cmp al,bl
		jbe okay
		and bl,(~31)		// al gets darkened to 0.  This checks for wrap problems
		mov al,bl
		jmp okay
brighten:
		mov bl,al			// bl is al's color range
		and bl,(~31)
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay2
		add bl,31
		mov al,bl			// make al max brightness.  This is a wrap issue
		jmp okay
okay2:
		add bl,31			// now see if it went over
		cmp al,bl
		jbe okay
		mov al,bl			// it did, so lock it to max
okay:
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		xor eax,eax
		pop edx
		pop ebx
		ret
linedone:
		dec sprHeight
		jz alldone			// All done!!!
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		add edi,ecx			// add one line's height
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

void sprite_t::DrawOffColor(int x,int y,MGLDraw *mgl,byte fromColor,byte toColor,char bright)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;

	x-=ofsx;
	y-=ofsy;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth;

	spr=data;
	sprHeight=height;
	sprWidth=width;

	if(y>mgl->GetHeight()-height)
		sprHeight=mgl->GetHeight()-y;

	toColor*=32;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		xor eax,eax			// zero out eax because we want it to work as a byte
loop1:
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		call movedata
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		call movedata
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe	dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		call movedata
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again

movedata:					// here's the core of it.  when this is jumped to,
							// ecx = count to transfer
							// ebx = unusable (sprite width is contained there)
							// edx = unusable (draw counter is stored there)
							// esi,edi = src, dst ptrs
		push ebx
		push edx
		mov  dl,toColor		// hold the hue to be used
		mov  bh,bright
		mov  dh,fromColor
		xor  eax,eax
movedataloop:
		mov al,es:[esi]
		mov	ah,al
		shr	ah,5
		cmp ah,dh
		jne normalColor

		and al,31			// strip off the hue of al
		or	al,dl			// and stick on the new hue
normalColor:
		cmp bh,0
		je  okay
		jg  brighten
		mov bl,al
		and bl,(~31)		// bl is al's color range
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay1
		mov al,bl			// yes, put it to it's min value
		jmp okay
okay1:
		add bl,31			// it didn't, let's see if it wrapped instead
		cmp al,bl
		jbe okay
		and bl,(~31)		// al gets darkened to 0.  This checks for wrap problems
		mov al,bl
		jmp okay
brighten:
		mov bl,al			// bl is al's color range
		and bl,(~31)
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay2
		add bl,31
		mov al,bl			// make al max brightness.  This is a wrap issue
		jmp okay
okay2:
		add bl,31			// now see if it went over
		cmp al,bl
		jbe okay
		mov al,bl			// it did, so lock it to max
okay:
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		xor eax,eax
		pop edx
		pop ebx
		ret
linedone:
		dec sprHeight
		jz alldone			// All done!!!
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		add edi,ecx			// add one line's height
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

// a ghost sprite is rather special.  It is drawn normally (except lightened
// or darkened according to the brightness parameter), except where it is grey
// (color 1-31).  Wherever those colors occur, they are instead used as the
// degree to which the background should be brightened instead of drawn over.
//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)
void sprite_t::DrawGhost(int x,int y,MGLDraw *mgl,char bright)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;

	x-=ofsx;
	y-=ofsy;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth;

	spr=data;
	sprHeight=height;
	sprWidth=width;

	if(y>mgl->GetHeight()-height)
		sprHeight=mgl->GetHeight()-y;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		xor eax,eax			// zero out eax because we want it to work as a byte
loop1:
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		call movedata
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		call movedata
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe	dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		call movedata
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again

movedata:					// here's the core of it.  when this is jumped to,
							// ecx = count to transfer
							// ebx = unusable (sprite width is contained there)
							// edx = unusable (draw counter is stored there)
							// esi,edi = src, dst ptrs
		push ebx
		push edx
		mov  bh,bright
		xor eax,eax
movedataloop:
		mov al,es:[esi]
		cmp al,32
		jae	drawnormal		// go ahead and draw normally
		mov ah,es:[edi]		// get the destination color
		mov bl,ah
		and bl,(~31)		// bl is the color range for ah (destination color)
		add ah,al			// add the brightness factor to the color
		cmp ah,bl
		jb  minnedout
		add bl,31			// now bl is the brightest in its range
		cmp ah,bl
		ja  maxxedout		// overshot the brightest
		mov al,ah
		jmp okay			// don't lighten/darken if this is a transparent- it should already
							// be bright or dark due to the stuff you can see through it
minnedout:
		add bl,31
		mov al,bl
		jmp okay
maxxedout:
		mov al,bl
		jmp okay
drawnormal:
		cmp bh,0
		je  okay
		jg  brighten
		mov bl,al
		and bl,(~31)		// bl is al's color range
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay1
		mov al,bl			// yes, put it to it's min value
		jmp okay
okay1:
		add bl,31			// it didn't, let's see if it wrapped instead
		cmp al,bl
		jbe okay
		mov al,bl
		and al,(~31)		// al gets darkened to 0.  This checks for wrap problems
		jmp okay
brighten:
		mov bl,al			// bl is al's color range
		and bl,(~31)
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay2
		add bl,31
		mov al,bl			// make al max brightness.  This is a wrap issue
		jmp okay
okay2:
		add bl,31			// now see if it went over
		cmp al,bl
		jbe okay
		mov al,bl			// it did, so lock it to max
okay:
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		xor eax,eax
		pop edx
		pop ebx
		ret
linedone:
		dec sprHeight
		jz alldone			// All done!!!
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		add edi,ecx			// add one line's height
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

void sprite_t::DrawGlow(int x,int y,MGLDraw *mgl,char bright)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;

	x-=ofsx;
	y-=ofsy;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth;

	spr=data;
	sprHeight=height;
	sprWidth=width;

	if(y>mgl->GetHeight()-height)
		sprHeight=mgl->GetHeight()-y;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		xor eax,eax			// zero out eax because we want it to work as a byte
loop1:
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		call movedata
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		call movedata
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe	dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		call movedata
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again

movedata:					// here's the core of it.  when this is jumped to,
							// ecx = count to transfer
							// ebx = unusable (sprite width is contained there)
							// edx = unusable (draw counter is stored there)
							// esi,edi = src, dst ptrs
		push ebx
		push edx
		xor eax,eax
movedataloop:
		mov  bh,bright
		mov ah,es:[esi]
		mov al,es:[edi]		// get the destination color
		and al,31			// strip off the hue
		mov bl,ah
		and bl,31			// bl contains the brightness of the source color
		add bh,bl

		and ah,(~31)
		or  al,ah			// now al has ah's hue but its own brightness

		cmp bh,0
		je  okay
		jg  brighten
		mov bl,al
		and bl,(~31)		// bl is al's color range
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay1
		mov al,bl			// yes, put it to it's min value
		jmp okay
okay1:
		add bl,31			// it didn't, let's see if it wrapped instead
		cmp al,bl
		jbe okay
		mov al,bl
		and al,(~31)		// al gets darkened to 0.  This checks for wrap problems
		jmp okay
brighten:
		mov bl,al			// bl is al's color range
		and bl,(~31)
		add al,bh
		cmp al,bl			// does al descend below it's range?
		jae okay2
		add bl,31
		mov al,bl			// make al max brightness.  This is a wrap issue
		jmp okay
okay2:
		add bl,31			// now see if it went over
		cmp al,bl
		jbe okay
		mov al,bl			// it did, so lock it to max
okay:
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		xor eax,eax
		pop edx
		pop ebx
		ret
linedone:
		dec sprHeight
		jz alldone			// All done!!!
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		add edi,ecx			// add one line's height
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

void sprite_t::DrawShadow(int x,int y,MGLDraw *mgl)
{
	dword scrWidth=mgl->GetWidth();
	dword dst;
	byte *scrn=mgl->GetScreen();
	byte *spr;
	dword doDraw;
	dword minX,maxX;
	dword sprHeight,sprWidth;
	byte alternate;

	//x-=ofsx+height/2;
	//y-=ofsy-height/2;	// start the sprite half of it's height down
	x-=ofsx+height/2;
	y-=ofsy/2;
	if(x>639 || y>479)
		return;	// whole sprite is offscreen

	dst=x+y*scrWidth;

	doDraw=0;
	if(y<0)
		doDraw=-y*2;

	minX=y*scrWidth+(dword)scrn;
	maxX=minX+scrWidth-1;

	spr=data;
	sprHeight=height/2;
	sprWidth=width;

	if(y>mgl->GetHeight()-(int)sprHeight)
		sprHeight=mgl->GetHeight()-y;

	if(sprHeight==0)
		return;

	__asm
	{
		// esi = address of sprite data
		// edi = screen address to draw at
		// edx = doDraw (counts down until you can draw if sprite is partially above screen)
		// ebx = sprite width counter
		// ecx = placeholder for moving data
		// eax = work register
		pusha
		push ds
		pop  es
		mov edi,scrn
		add edi,dst
		mov esi,spr
		mov edx,doDraw
		mov ebx,sprWidth
		mov alternate,1
loop1:
		xor eax,eax			// zero out eax because we want it to work as a byte
		cmp ebx,0			// check to see if this line is done (x has run out)
		jz  linedone
		mov al,ds:[esi]		// get next byte of source
		inc esi
		sal al,1
		jnc	drawpixels		// if the byte is positive, draw a run of pixels
							// otherwise we are skipping pixels
		shr al,1
		add edi,eax			// move destination forward by that much
		sub ebx,eax			// and decrease width count by that much
		jmp loop1
drawpixels:
		shr al,1
		sub ebx,eax
		cmp alternate,1		// first check: since every other line is skipped, check to see if
							// this one is skipped (alternate=1 when not skipping)
		jne	dontplot
		cmp edx,0			// check to be sure we're not too high up
		jnz dontplot
		cmp edi,maxX		// check to be sure we're not off the end
		jae dontplot
		cmp edi,minX		// check to be sure we're not too far left either
							// (in which case it may be partial)
		jbe dontplotallofit

		mov ecx,eax
		add ecx,edi
		cmp ecx,maxX		// check to see if this run will run off the end
		jae cropit
		mov ecx,eax
		call movedata
		jmp loop1
cropit:
		sub ecx,maxX
		push ecx			// store the amount it runs over
		sub eax,ecx			// chop off the amount it runs over by from the amount to draw
		mov ecx,eax
		call movedata
		pop ecx
		add esi,ecx			// move the source pointer up by the undrawn amount
		add edi,ecx
		jmp loop1
dontplotallofit:
		mov ecx,eax
		add ecx,edi
		cmp ecx,minX		// check to see if some of this run is onscreen
		jbe	dontplot		// nope, treat it as a complete ignore
		sub ecx,minX
		sub eax,ecx			// now eax contains how many to skip, and ecx contains how many to draw
		add edi,eax
		add esi,eax
		call movedata
		jmp loop1
dontplot:
		add edi,eax
		add esi,eax
		jmp loop1			// just move the pointers and loop again

movedata:					// here's the core of it.  when this is jumped to,
							// ecx = count to transfer
							// ebx = unusable (sprite width is contained there)
							// edx = unusable (draw counter is stored there)
							// esi,edi = src, dst ptrs
		push ebx
movedataloop:
		xor eax,eax
		mov al,es:[edi]
		mov bl,al
		and bl,31
		cmp bl,4
		jb	cantdarken
		sub al,4
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		pop ebx
		ret
cantdarken:
		and al,~31
		mov es:[edi],al		// store the byte
		inc esi
		inc edi
		dec ecx
		jnz movedataloop	// loop around
		pop ebx
		ret
linedone:
		neg alternate		// flip the draw/don't draw flag
		mov ecx,scrWidth	// just handy for speed
		mov ebx,sprWidth
		sub edi,ebx			// subtract width from the destination, and...
		cmp alternate,1
		je  linedone2
		dec sprHeight
		jz  alldone			// All done!!!
		add edi,ecx			// add one line's height
		inc edi
		add maxX,ecx		// move the xMax and xMin down one line
		add minX,ecx
linedone2:
		cmp edx,0
		jz	loop1
		dec edx				// if doDraw (edx) was nonzero, count it down
		jmp loop1
alldone:
		popa
	}
}

// -------------------------------------------------------------------------
// ***************************** SPRITE_SET_T ******************************
// -------------------------------------------------------------------------

// CONSTRUCTORS & DESTRUCTORS
sprite_set_t::sprite_set_t(void)
{
	count=0;
	spr=NULL;
}

sprite_set_t::sprite_set_t(const char *fname)
{
	count=0;
	spr=NULL;
	Load(fname);
}

sprite_set_t::~sprite_set_t(void)
{
	Free();
}

// REGULAR MEMBER FUNCTIONS
bool sprite_set_t::Load(const char *fname)
{
	FILE *f;
	int i;
	byte *buffer;

	if(spr)
		Free();

	f=fopen(fname,"rb");
	if(!f)
		return FALSE;
	// read the count
	fread(&count,2,1,f);

	spr=(sprite_t **)malloc(sizeof(sprite_t *)*count);
	if(!spr)
	{
		fclose(f);
		return FALSE;
	}

	// allocate a buffer to load sprites into
	buffer=(byte *)malloc(SPRITE_INFO_SIZE*count);
	if(!buffer)
	{
		fclose(f);
		free(spr);
		return FALSE;
	}

	// read in the sprite headers
	if(fread(buffer,SPRITE_INFO_SIZE,count,f)!=count)
	{
		fclose(f);
		free(spr);
		free(buffer);
		return FALSE;
	}

	// allocate the sprites and read in the data for them
	for(i=0;i<count;i++)
	{
		spr[i]=new sprite_t(&buffer[i*SPRITE_INFO_SIZE]);
		if(!spr[i])
		{
			fclose(f);
			return FALSE;
		}
		if(!spr[i]->LoadData(f))
		{
			fclose(f);
			return FALSE;
		}
	}
	free(buffer);
	fclose(f);
	return TRUE;
}

bool sprite_set_t::Save(const char *fname)
{
	FILE *f;
	int i;
	byte *buffer;

	f=fopen(fname,"wb");
	if(!f)
		return FALSE;
	// write the count
	fwrite(&count,2,1,f);

	// allocate a buffer to copy sprites into
	buffer=(byte *)malloc(SPRITE_INFO_SIZE*count);
	if(!buffer)
	{
		fclose(f);
		return FALSE;
	}

	for(i=0;i<count;i++)
		spr[i]->GetHeader(&buffer[i*SPRITE_INFO_SIZE]);

	// write the sprites out
	if(fwrite(buffer,SPRITE_INFO_SIZE,count,f)!=count)
	{
		fclose(f);
		free(buffer);
		return FALSE;
	}

	// write the sprite data
	for(i=0;i<count;i++)
	{
		if(!spr[i]->SaveData(f))
		{
			fclose(f);
			return FALSE;
		}
	}
	fclose(f);
	return TRUE;

}

void sprite_set_t::Free(void)
{
	int i;
	for(i=0;i<count;i++)
		delete spr[i];
	free(spr);
}

sprite_t *sprite_set_t::GetSprite(int which)
{
	if(spr && which<=count && spr[which])
		return spr[which];
	return NULL;
}

word sprite_set_t::GetCount(void)
{
	return count;
}
