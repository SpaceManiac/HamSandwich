#include "debuggo.h"
#include <stdio.h>

void WriteDebug(char *s)
{
	FILE *f;

	f=fopen("debug.txt","a+t");
	if(f==NULL)
		printf("ppo");
	else
	{
		fprintf(f,"%s\n",s);
		fclose(f);
	}
}
