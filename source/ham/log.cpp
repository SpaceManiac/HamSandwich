#include "log.h"
#include "appdata.h"
#include <stdio.h>
#include <stdarg.h>

#include <SDL3/SDL_platform.h>
#include <SDL3/SDL_log.h>

#ifdef __ANDROID__
	#include <android/log.h>
#endif

static SDL_LogOutputFunction original = nullptr;
static void* originalUserdata = nullptr;
static bool errorLogAttempted = false;
static owned::SDL_IOStream errorLog = nullptr;

static const char *priorityPrefixes[] = {
    "",
	"TRACE: ",
    "VERBOSE: ",
    "DEBUG: ",
    "INFO: ",
    "WARN: ",
    "ERROR: ",
    "CRITICAL: ",
};
static_assert(std::size(priorityPrefixes) == SDL_LOG_PRIORITY_COUNT);

void HamLogOutput(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	(void)userdata;
	original(originalUserdata, category, priority, message);
	if (errorLog)
	{
		SDL_WriteIO(errorLog, priorityPrefixes[priority], 1, strlen(priorityPrefixes[priority]));
		SDL_WriteIO(errorLog, message, 1, strlen(message));
		SDL_WriteIO(errorLog, "\n", 1, 1);
		// We should flush here but SDL_RWflush doesn't exist in SDL2.
		AppdataSync();
	}
}

void LogInit()
{
	if (!original)
	{
#ifndef NDEBUG
		SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
#endif
		SDL_GetLogOutputFunction(&original, &originalUserdata);
		SDL_SetLogOutputFunction(HamLogOutput, nullptr);
	}

	if (!errorLogAttempted && AppdataIsInit())
	{
		errorLogAttempted = true;
		errorLog = AppdataOpen_Write("error.log");
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
