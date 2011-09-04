/**
 * Base file for assembly stuff
 */

#ifndef ASM_BASE_H
#define	ASM_BASE_H

// ASM_VAR(type, name, value); - define a variable that should be visible to the assembler
// ASM_START() - begin an assembly code block
// _(instructions) - a line of assembly code
// ASM_END() - end an assembly code block

#ifdef __MINGW32__
// MinGW
#define ASM_VAR(type, prefix, name, value) static type name __asm__(#prefix "_" #name); name = value
#define ASM_START() asm (
#define _(instr...) #instr "\n"
#define ASM_END() );

#else
// Visual Studio
#define ASM_VAR(name) type ##name = value
#define ASM_START() asm {
#define _(instr...) instr
#define ASM_END() }

#define ASM_TILE_WIDTH 32

#endif

#endif	/* ASM_BASE_H */

