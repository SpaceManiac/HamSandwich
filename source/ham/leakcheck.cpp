#include "leakcheck.h"

#if defined(WIN32) && !defined(NDEBUG)
#include <crtdbg.h>

void EnableLeakCheck()
{
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag); // Set flag to the new value
}
#else
void EnableLeakCheck();
#endif
