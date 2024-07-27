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

	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Stockboy download test log", asctime(newtime));
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Log opened %s.", asctime(newtime));
#endif
}

void Log_End(void)
{
#ifdef BETA
	struct tm *newtime;
	time_t clock;

	time(&clock);
	newtime=localtime(&clock);

	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Log closed %s.", asctime(newtime));
#endif
}

void Log_Print(const char *txt)
{
#ifdef BETA
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", txt);
#endif
}

void Log_Printnum(const char *txt,int num)
{
#ifdef BETA
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s %d", txt, num);
#endif
}
