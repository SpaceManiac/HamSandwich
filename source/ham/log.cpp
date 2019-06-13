#include "log.h"
#include "appdata.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef SDL_UNPREFIXED
	#include <SDL_platform.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_platform.h>
#endif  // SDL_UNPREFIXED

#ifdef __ANDROID__
	#include <android/log.h>
#endif

static FILE* errorLog = nullptr;

void LogDebug(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
#ifdef __ANDROID__
	__android_log_vprint(ANDROID_LOG_DEBUG, "HamSandwich", fmt, args);
#else
	vprintf(fmt, args);
	printf("\n");
#endif
	va_end(args);
}

void LogError(const char* fmt, ...) {
	if (!errorLog) {
		errorLog = AppdataOpen("error.log", "wt");
		if (!errorLog) {
			return;
		}
	}

	va_list args;
	va_start(args, fmt);
#ifdef __ANDROID__
	__android_log_vprint(ANDROID_LOG_ERROR, "HamSandwich", fmt, args);
#else
	va_list dup;
	va_copy(dup, args);
	vfprintf(errorLog, fmt, args);
	fprintf(errorLog, "\n");
	vprintf(fmt, dup);
	printf("\n");
	va_end(dup);
#endif
	va_end(args);
}
