#include "nsis.h"

int main() {

	nsis::Archive arc { fopen("build/installers/supreme8_install.exe", "rb") };

	printf("%s\n", arc.populate_file_list() ? "ok" : "err");

}
