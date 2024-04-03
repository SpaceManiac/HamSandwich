#ifndef LOG_H
#define LOG_H

#include "mgldraw.h"

void Log_Begin(const char *fname);
void Log_End(void);
void Log_Print(const char *txt);
void Log_Printnum(const char *txt,int num);

#endif
