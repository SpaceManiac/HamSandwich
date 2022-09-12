#ifndef OWNED_H
#define OWNED_H

#include <stdio.h>
#include <SDL_rwops.h>
#include <memory>
#include "rw_functions.h"

// ----------------------------------------------------------------------------
// C stdlib

namespace owned
{
	namespace _deleter
	{
		struct FILE
		{
			void operator()(::FILE* ptr);
		};
	}

	typedef std::unique_ptr<::FILE, _deleter::FILE> FILE;

	inline FILE fopen(const char *pathname, const char *mode)
	{
		return FILE { ::fopen(pathname, mode) };
	}
}

inline size_t fread(void *ptr, size_t size, size_t nmemb, const owned::FILE& stream)
{
	return fread(ptr, size, nmemb, stream.get());
}

inline size_t fwrite(const void *ptr, size_t size, size_t nmemb, const owned::FILE& stream)
{
	return fwrite(ptr, size, nmemb, stream.get());
}

inline int fseek(const owned::FILE& stream, long offset, int whence)
{
	return fseek(stream.get(), offset, whence);
}

inline int fclose(owned::FILE stream)
{
	return fclose(stream.release());
}

// ----------------------------------------------------------------------------
// SDL

namespace owned
{
	namespace _deleter
	{
		struct SDL_RWops
		{
			void operator()(::SDL_RWops* ptr);
		};
	}

	typedef std::unique_ptr<::SDL_RWops, _deleter::SDL_RWops> SDL_RWops;

	inline SDL_RWops SDL_RWFromFP(::FILE* fp)
	{
		return SDL_RWops { ::SDL_RWFromFP(fp, SDL_FALSE) };
	}

	inline SDL_RWops SDL_RWFromFP(owned::FILE fp)
	{
		return SDL_RWops { ::SDL_RWFromFP(fp.release(), SDL_TRUE) };
	}

	inline SDL_RWops SDL_RWFromConstMem(const void *mem, int size)
	{
		return SDL_RWops { ::SDL_RWFromConstMem(mem, size) };
	}

	inline SDL_RWops SDL_RWFromFile(const char *file, const char *mode)
	{
		return SDL_RWops { ::SDL_RWFromFile(file, mode) };
	}
}

inline Sint64 SDL_RWsize(const owned::SDL_RWops& context)
{
	return context->size(context.get());
}

inline Sint64 SDL_RWseek(const owned::SDL_RWops& context, Sint64 offset, int whence)
{
	return context->seek(context.get(), offset, whence);
}

inline Sint64 SDL_RWtell(const owned::SDL_RWops& context)
{
	return context->seek(context.get(), 0, RW_SEEK_CUR);
}

inline size_t SDL_RWread(const owned::SDL_RWops& context, void *ptr, size_t size, size_t maxnum)
{
	return context->read(context.get(), ptr, size, maxnum);
}

inline size_t SDL_RWwrite(const owned::SDL_RWops& context, const void *ptr, size_t size, size_t num)
{
	return context->write(context.get(), ptr, size, num);
}

inline int SDL_RWclose(owned::SDL_RWops context)
{
	return SDL_RWclose(context.release());
}

#endif
