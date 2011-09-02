__asm
	{
		pusha
		push ds
		pop	 es
		mov  esi,src
		mov  edi,dst
		mov  edx,hgt
		mov  ecx,wid
		mov  bh,light
loop1:
		mov  al,[esi]
		mov  bl,al
		and  bl,~31
		add  al,bh
		cmp  al,bl
		jae	 okay1
		cmp  bh,0
		jl	 fine
		mov  al,bl
		add  al,31
		jmp okay2
fine:
		mov  al,bl
		jmp okay2
okay1:
		add  bl,31
		cmp  al,bl
		jb	 okay2
		cmp  bh,0
		jl   fine2
		mov  al,bl
		jmp  okay2
fine2:
		mov  al,bl
		and  al,(~31)
okay2:
		mov  [edi],al
		inc  esi
		inc  edi
		dec  ecx
		jnz	 loop1
		mov  ecx,wid
		add  esi,TILE_WIDTH
		sub	 esi,wid
		add  edi,640
		sub  edi,wid
		dec  edx
		jnz  loop1
		popa
	}
