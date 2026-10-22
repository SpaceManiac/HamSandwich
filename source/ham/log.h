#ifndef LOG_H
#define LOG_H

#include "jamultypes.h"
#include <SDL3/SDL_log.h>

// Debug and error logging functions
HAM_API void LogInit();
#define LogTrace(...) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define LogVerbose(...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define LogDebug(...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define LogInfo SDL_Log
#define LogWarn(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define LogError(...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

#define IsLogPriority(PRIORITY) (SDL_GetLogPriority(SDL_LOG_CATEGORY_APPLICATION) <= (PRIORITY))

#endif  // LOG_H
