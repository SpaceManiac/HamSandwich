// Usage: mini_xxd <bin_fname> <cpp_fname> <symbol_name>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		fprintf(stderr, "usage: mini_xxd <bin_fname> <cpp_fname> <symbol_name>\n");
		return 1;
	}
	const char *bin_fname = argv[1], *cpp_fname = argv[2], *symbol_name = argv[3];

	FILE* bin = fopen(bin_fname, "rb");
	if (!bin) return 1;

	FILE* cpp = fopen(cpp_fname, "wb");
	if (!cpp) return 1;

	fprintf(cpp, "\
#include <stddef.h>\n\
extern const unsigned char %s[] = {\n\
", symbol_name);

	unsigned char buf[16 * 1024];
	while (int read = fread(buf, 1, sizeof(buf), bin))
	{
		for (int i = 0; i < read; ++i)
		{
			fprintf(cpp, "%d,", buf[i]);
		}
	}

	fprintf(cpp, "\
};\n\
extern const size_t %s_size = sizeof(%s);\n\
", symbol_name, symbol_name);
}
