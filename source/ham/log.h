#ifndef LOG_H
#define LOG_H

// Conditionals for warnings/analysis based on SDL_stdinc.h
#ifdef __GNUC__
#define PRINTF_FUNC( fmtargnumber ) __attribute__ (( format( __printf__, fmtargnumber, fmtargnumber+1 )))
#else  // __GNUC__
#define PRINTF_FUNC( fmtargnumber )
#endif  // __GNUC__

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#include <sal.h>
#define PRINTF_FMT _Printf_format_string_
#else  // _MSC_VER
#define PRINTF_FMT
#endif  // _MSC_VER

// Debug and error logging functions
void LogDebug(PRINTF_FMT const char* fmt, ...) PRINTF_FUNC(1);
void LogError(PRINTF_FMT const char* fmt, ...) PRINTF_FUNC(1);

#undef PRINTF_FUNC
#undef PRINTF_FMT

#ifdef LOG
#define DBG(X) LogDebug("%s", X)
#define PrintToLog(A, B) LogDebug("%s : %d", A, B)
#else  // LOG
#define DBG(X)
#define PrintToLog(A, B)
#endif  // LOG

#endif  // LOG_H
