#include "mempackf.h"


int MemoryPackfile::pf_fclose(void *userdata)
{
	return 0;
	/* nothing */
}

int MemoryPackfile::pf_getc(void *userdata)
{
	MemoryPackfile * m = (MemoryPackfile*) userdata;
	if (m->position < m->stream + m->length)
	{
		unsigned char x = *m->position;
		m->position += 1;
		return x;
	}
	return EOF;
}

int MemoryPackfile::pf_ungetc(int c, void *userdata)
{
	MemoryPackfile * m = (MemoryPackfile*) userdata;
	if (m->position > m->stream)
	{
		m->position -= 1;
		return c;
	} else
	{
		return EOF;
	}
}

long MemoryPackfile::pf_fread(void *p, long n, void *userdata)
{
	MemoryPackfile *m = (MemoryPackfile*) userdata;
	unsigned char *cp = (unsigned char *) p;
	long i;
	int c;

	for (i = 0; i < n; i++)
	{
		if ((c = pf_getc(m)) == EOF)
			break;

		*(cp++) = c;
	}

	return i;
}

int MemoryPackfile::pf_putc(int c, void *userdata)
{
	MemoryPackfile * m = (MemoryPackfile*) userdata;
	if (m->position < m->stream + m->length)
	{
		*m->position = c;
		m->position += 1;
		return c;
	}
	return EOF;

}

long MemoryPackfile::pf_fwrite(const void *p, long n, void *userdata)
{
	MemoryPackfile *m = (MemoryPackfile*) userdata;
	const unsigned char * cp = (const unsigned char *) p;
	long i;
	int c;

	/* probably should replace this with memcpy */

	for (i = 0; i < n; i++)
	{
		if ((c = pf_putc(cp[i], userdata)) == EOF)
			break;
	}

	return i;
}

int MemoryPackfile::pf_fseek(void *userdata, int offset)
{
	MemoryPackfile * m = (MemoryPackfile*) userdata;
	if (offset >= 0 && m->position + offset < m->stream + m->length)
	{
		m->position += offset;
		return 0;
	} else
	{
		return -1;
	}
}

int MemoryPackfile::pf_feof(void *userdata)
{
	MemoryPackfile * m = (MemoryPackfile*) userdata;
	return m->position >= m->stream + m->length;
}

int MemoryPackfile::pf_ferror(void *userdata)
{
	MemoryPackfile * m = (MemoryPackfile*) userdata;
	return m->position < m->stream || m->position >= m->stream + m->length;
}
