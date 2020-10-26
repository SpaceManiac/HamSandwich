#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "nsis.h"

const char* spam = "                ";

void show(int depth, const nsis::Directory& directory) {
	for (const auto& entry : directory.files) {
		printf("%s%s --> %zu\n", &spam[16 - depth], entry.first.c_str(), *(size_t*)&entry.second);
	}
	for (const auto& entry : directory.directories) {
		printf("%s%s:\n", &spam[16 - depth], entry.first.c_str());
		show(depth + 4, entry.second);
	}
}

int main(int argc, const char* argv[])
{
	(void) argc;

	FILE* fp = fopen(argv[1], "rb");
	if (!fp)
	{
		printf("fopen(%s): %s\n", argv[1], strerror(errno));
		exit(1);
	}
	nsis::Archive arc { fp };

	if (arc.is_ok()) {
		show(0, arc.root());
	} else {
		printf("nsis extract failure\n");
	}
}
