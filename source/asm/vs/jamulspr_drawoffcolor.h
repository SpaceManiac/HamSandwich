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
