#include "log.h"
#include "appdata.h"
#include <stdio.h>
#include <stdarg.h>

#include <SDL_platform.h>

#ifdef __ANDROID__
	#include <android/log.h>
#endif

static bool errorLogAttempted = false;
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
	if (!errorLogAttempted) {
		errorLogAttempted = true;
		errorLog = AppdataOpen("error.log", "wt");
	}

	va_list args;
	va_start(args, fmt);

	if (errorLog) {
		va_list dup;
		va_copy(dup, args);
		vfprintf(errorLog, fmt, dup);
		fprintf(errorLog, "\n");
		va_end(dup);
	}

#ifdef __ANDROID__
	__android_log_vprint(ANDROID_LOG_ERROR, "HamSandwich", fmt, args);
#else
	vprintf(fmt, args);
	printf("\n");
#endif
	va_end(args);

	if (errorLog) {
		fflush(errorLog);
		AppdataSync();
	}
}
