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
