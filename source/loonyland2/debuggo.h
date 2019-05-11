#ifndef DEBUGGO_H
#define DEBUGGO_H

#ifdef _DEBUG
#define DBG(x) WriteDebug(x);
#else
#define DBG(x) ;
#endif

void WriteDebug(char *s);

#endif
