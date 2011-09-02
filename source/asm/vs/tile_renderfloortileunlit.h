__asm
	{
		pusha
		push ds
		pop	 es
		mov  esi,src
		mov  edi,dst
		mov  edx,hgt
		mov  ecx,wid
loop1:
		rep  movsb
		mov  ecx,wid
		add  esi,TILE_WIDTH
		sub	 esi,wid
		add  edi,640
		sub  edi,wid
		dec  edx
		jnz  loop1
		popa
	}
