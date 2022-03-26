#include "log.h"
#include "appdata.h"
#include <stdio.h>
#include <stdarg.h>

#include <SDL_platform.h>
#include <SDL_log.h>

#ifdef __ANDROID__
	#include <android/log.h>
#endif

static SDL_LogOutputFunction original;
static void* originalUserdata;
static bool errorLogAttempted = false;
static FILE* errorLog = nullptr;

void HamLogOutput(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	original(originalUserdata, category, priority, message);
	if (errorLog && priority >= SDL_LOG_PRIORITY_WARN)
	{
		fprintf(errorLog, "%s\n", message);
		fflush(errorLog);
		AppdataSync();
	}
}

static void LogInit()
{
	if (!errorLogAttempted && AppdataIsInit())
	{
		errorLogAttempted = true;
		errorLog = AppdataOpen_Write("error.log");

#ifndef NDEBUG
		SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
#endif

		SDL_LogGetOutputFunction(&original, &originalUserdata);
		SDL_LogSetOutputFunction(HamLogOutput, nullptr);
	}
}

void LogDebug(const char* fmt, ...)
{
	LogInit();

	va_list args;
	va_start(args, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
	va_end(args);
}

void LogError(const char* fmt, ...)
{
	LogInit();

	va_list args;
	va_start(args, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);
	va_end(args);
}
