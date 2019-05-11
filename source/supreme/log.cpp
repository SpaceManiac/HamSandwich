#include "log.h"

FILE *logF;
int logLine;

void InitLog(void)
{
#ifdef LOG
	logF=fopen("log.txt","wt");
	logLine=0;
	fprintf(logF,"Log is go for action!\n");
#endif
}

void ExitLog(void)
{
#ifdef LOG
	fclose(logF);
#endif
}

void PrintToLog(char *s,int n)
{
#ifdef LOG
	fprintf(logF,"%05d: %s : %d\n",logLine++,s,n);
	fflush(logF);
#endif
}
