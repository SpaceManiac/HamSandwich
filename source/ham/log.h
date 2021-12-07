#ifndef LOG_H
#define LOG_H

#include <SDL_stdinc.h>

// Debug and error logging functions
void LogDebug(SDL_PRINTF_FORMAT_STRING const char* fmt, ...) SDL_PRINTF_VARARG_FUNC(1);
void LogError(SDL_PRINTF_FORMAT_STRING const char* fmt, ...) SDL_PRINTF_VARARG_FUNC(1);

#ifdef LOG
#	define DBG(X) LogDebug("%s", X)
#	define PrintToLog(A, B) LogDebug("%s : %d", A, B)
#else  // LOG
#	define DBG(X)
#	define PrintToLog(A, B)
#endif  // LOG

#endif  // LOG_H
