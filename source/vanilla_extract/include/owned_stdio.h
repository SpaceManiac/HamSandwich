#ifndef OWNED_STDIO_H
#define OWNED_STDIO_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <memory>
#include <SDL_log.h>

namespace owned
{
	namespace _deleter
	{
		struct FILE
		{
			void operator()(::FILE* ptr)
			{
				if (ptr && fclose(ptr) != 0)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "fclose: %s", strerror(errno));
				}
			}
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

[[nodiscard]] // If you don't care about the return value, use .reset() instead.
inline int fclose(owned::FILE stream)
{
	return fclose(stream.release());
}

#endif
