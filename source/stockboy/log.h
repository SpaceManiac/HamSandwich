#ifndef LOG_H
#define LOG_H

#include "mgldraw.h"

void Log_Begin(char *fname);
void Log_End(void);
void Log_Print(char *txt);
void Log_Printnum(char *txt,int num);

#endif