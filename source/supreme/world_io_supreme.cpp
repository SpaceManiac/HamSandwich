#include "world_io_supreme.h"
#include <SDL_rwops.h>

bool Supreme_GetWorldName(SDL_RWops *f, StringDestination name, StringDestination author)
{
	char authbuffer[32], namebuffer[32];

	if (SDL_RWseek(f, 8, RW_SEEK_SET) < 0)
		return false;
	if (SDL_RWread(f, authbuffer, 32, 1) < 1)
		return false;
	if (SDL_RWread(f, namebuffer, 32, 1) < 1)
		return false;

	authbuffer[31] = 0;
	author.assign(authbuffer);
	namebuffer[31] = 0;
	name.assign(namebuffer);

	return true;
}
