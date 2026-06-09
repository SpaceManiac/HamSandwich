#ifndef RW_FUNCTIONS_H
#define RW_FUNCTIONS_H

#include <SDL3/SDL_iostream.h>

// Some versions of SDL2 use a macro for SDL_CloseIO, which we can't overload,
// so undef it and make it a normal function so we can overload it later.
#ifdef SDL_CloseIO
#undef SDL_GetIOSize
#undef SDL_SeekIO
#undef SDL_TellIO
#undef SDL_ReadIO
#undef SDL_WriteIO
#undef SDL_CloseIO
inline Sint64 SDL_GetIOSize(SDL_IOStream *context)
{
	return context->size(context);
}
inline Sint64 SDL_SeekIO(SDL_IOStream *context, Sint64 offset, int whence)
{
	return context->seek(context, offset, whence);
}
inline Sint64 SDL_TellIO(SDL_IOStream *context)
{
	return context->seek(context, 0, SDL_IO_SEEK_CUR);
}
inline size_t SDL_ReadIO(SDL_IOStream *context, void *ptr, size_t size, size_t maxnum)
{
	return context->read(context, ptr, size, maxnum);
}
inline size_t SDL_WriteIO(SDL_IOStream *context, const void *ptr, size_t size, size_t num)
{
	return context->write(context, ptr, size, num);
}
inline int SDL_CloseIO(SDL_IOStream *context)
{
	return context->close(context);
}
#endif  // SDL_CloseIO

#endif  // RW_FUNCTIONS_H
