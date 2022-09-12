#ifndef RW_FUNCTIONS_H
#define RW_FUNCTIONS_H

#include <SDL_rwops.h>

// Some versions of SDL2 use a macro for SDL_RWclose, which we can't overload,
// so undef it and make it a normal function so we can overload it later.
#ifdef SDL_RWclose
#undef SDL_RWsize
#undef SDL_RWseek
#undef SDL_RWtell
#undef SDL_RWread
#undef SDL_RWwrite
#undef SDL_RWclose
inline Sint64 SDL_RWsize(SDL_RWops *context)
{
	return context->size(context);
}
inline Sint64 SDL_RWseek(SDL_RWops *context, Sint64 offset, int whence)
{
	return context->seek(context, offset, whence);
}
inline Sint64 SDL_RWtell(SDL_RWops *context)
{
	return context->seek(context, 0, RW_SEEK_CUR);
}
inline size_t SDL_RWread(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
{
	return context->read(context, ptr, size, maxnum);
}
inline size_t SDL_RWwrite(SDL_RWops *context, const void *ptr, size_t size, size_t num)
{
	return context->write(context, ptr, size, num);
}
inline int SDL_RWclose(SDL_RWops *context)
{
	return context->close(context);
}
#endif  // SDL_RWclose

#endif  // RW_FUNCTIONS_H
