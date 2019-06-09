#include "appdata.h"

#ifdef _WIN32
#include <io.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

FILE* AppdataOpen(const char* file, const char* mode)
{
	char buffer[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buffer);
	sprintf(buffer + strlen(buffer), "\\Hamumu");
	mkdir(buffer);
	sprintf(buffer + strlen(buffer), "\\DrLunatic");
	mkdir(buffer);
	sprintf(buffer + strlen(buffer), "\\%s", file);
	return fopen(buffer, mode);
}

#else

FILE* AppdataOpen(const char* file, const char* mode)
{
    return fopen(file, mode);
}

#endif
