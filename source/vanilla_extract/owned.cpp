#include "vanilla_extract.h"
#include <SDL_rwops.h>
#include <SDL_log.h>
#include <errno.h>

void owned::_deleter::SDL_RWops::operator()(::SDL_RWops* ptr)
{
	if (ptr)
	{
		if (SDL_RWclose(ptr) != 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RWclose: %s", SDL_GetError());
		}
	}
}

void owned::_deleter::FILE::operator()(::FILE* ptr)
{
	if (ptr)
	{
		if (fclose(ptr) != 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "fclose: %s", strerror(errno));
		}
	}
}
