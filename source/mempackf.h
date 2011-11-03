#ifndef MEMPACKF_H
#define	MEMPACKF_H

#include <allegro.h>

class MemoryPackfile
{
	unsigned char * stream;		// points to start
	unsigned char * position;	// points to current position
	int length;
	PACKFILE_VTABLE table;

	static int pf_fclose(void*);
	static int pf_getc(void*);
	static int pf_ungetc(int, void*);
	static long pf_fread(void*, long, void*);
	static int pf_putc(int, void*);
	static long pf_fwrite(const void*, long, void*);
	static int pf_fseek(void*, int);
	static int pf_feof(void*);
	static int pf_ferror(void*);

public:
	MemoryPackfile(unsigned char * stream, int length) : stream(stream), position(stream), length(length) {
		table.pf_fclose = pf_fclose;
		table.pf_getc = pf_getc;
		table.pf_ungetc = pf_ungetc;
		table.pf_fread = pf_fread;
		table.pf_putc = pf_putc;
		table.pf_fwrite = pf_fwrite;
		table.pf_fseek = pf_fseek;
		table.pf_feof = pf_feof;
		table.pf_ferror = pf_ferror;
	}

	int getSize() const { return position - stream; }
	PACKFILE* open() { return pack_fopen_vtable(&table, this); }

};

#endif	/* MEMPACKF_H */
