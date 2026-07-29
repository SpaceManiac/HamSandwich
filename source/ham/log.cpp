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
static std::string buffer;

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

static const char *categoryPrefixes[] = {
	"",
	"(error) ",
	"(assert) ",
	"(system) ",
	"(audio) ",
	"(video) ",
	"(render) ",
	"(input) ",
	"(test) ",
	"(gpu) ",
	"(10) ",
	"(11) ",
	"(12) ",
	"(13) ",
	"(14) ",
	"(15) ",
	"(16) ",
	"(17) ",
	"(18) ",
};
static_assert(std::size(categoryPrefixes) == SDL_LOG_CATEGORY_CUSTOM);

static void HamLogOutput(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	(void)userdata;
	original(originalUserdata, category, priority, message);

	if ((size_t)category < std::size(categoryPrefixes))
	{
		buffer.append(categoryPrefixes[category]);
	}
	if (priority < std::size(priorityPrefixes))
	{
		buffer.append(priorityPrefixes[priority]);
	}
	buffer.append(message);
	buffer.append("\n");
	if (errorLog)
	{
		buffer.erase(0, SDL_WriteIO(errorLog, buffer.data(), buffer.size()));
		SDL_FlushIO(errorLog);
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
		if (errorLog && !buffer.empty())
		{
			buffer.erase(0, SDL_WriteIO(errorLog, buffer.data(), buffer.size()));
			SDL_FlushIO(errorLog);
			AppdataSync();
		}
	}
}

void LogDebug(const char* fmt, ...)
{
	LogInit();

	va_list args;
	va_start(args, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, fmt, args);
	va_end(args);
}

void LogInfo(const char* fmt, ...)
{
	LogInit();

	va_list args;
	va_start(args, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
	va_end(args);
}

void LogWarn(const char* fmt, ...)
{
	LogInit();

	va_list args;
	va_start(args, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, fmt, args);
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
