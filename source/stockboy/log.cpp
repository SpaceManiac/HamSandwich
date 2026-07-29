#include "log.h"
#include <stdio.h>
#include <time.h>
#include "appdata.h"

void Log_Begin(const char *fname)
{
#ifdef BETA
	struct tm *newtime;
	time_t clock;

	time(&clock);
	newtime=localtime(&clock);

	LogDebug("Stockboy download test log", asctime(newtime));
	LogDebug("Log opened %s.", asctime(newtime));
#endif
}

void Log_End(void)
{
#ifdef BETA
	struct tm *newtime;
	time_t clock;

	time(&clock);
	newtime=localtime(&clock);

	LogDebug("Log closed %s.", asctime(newtime));
#endif
}

void Log_Print(const char *txt)
{
#ifdef BETA
	LogDebug("%s", txt);
#endif
}

void Log_Printnum(const char *txt,int num)
{
#ifdef BETA
	LogDebug("%s %d", txt, num);
#endif
}
