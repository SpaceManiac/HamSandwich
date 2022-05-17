#include "log.h"
#include <stdio.h>
#include <time.h>

#ifdef BETA
static FILE *logF;
#endif

void Log_Begin(const char *fname)
{
#ifdef BETA
	struct tm *newtime;
	time_t clock;

	logF=fopen(fname,"wt");

	time(&clock);
	newtime=localtime(&clock);

	fprintf(logF,"Stockboy download test log\n");
	fprintf(logF,"Log opened %s.\n\n",asctime(newtime));
#endif
}

void Log_End(void)
{
#ifdef BETA
	struct tm *newtime;
	time_t clock;

	time(&clock);
	newtime=localtime(&clock);

	fprintf(logF,"Log closed %s.",asctime(newtime));
	fclose(logF);
#endif
}

void Log_Print(const char *txt)
{
#ifdef BETA
	fprintf(logF,"%s\n",txt);
#endif
}

void Log_Printnum(const char *txt,int num)
{
#ifdef BETA
	fprintf(logF,"%s %d\n",txt,num);
#endif
}
