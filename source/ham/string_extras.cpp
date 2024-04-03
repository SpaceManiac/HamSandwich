#include "string_extras.h"
#include <string.h>
#include <algorithm>
#include <SDL_assert.h>

char* StringDestination::prepare(size_t *len)
{
	if (!ptr || !sz)
	{
		*len = 0;
		return nullptr;
	}
	else if (sz == SZ_STD_STRING)
	{
		std::string* s = static_cast<std::string*>(ptr);
		s->resize(*len);
		return s->data();
	}
	else
	{
		char* buf = (char*) ptr;
		if (*len > sz - 1)
			*len = sz - 1;
		buf[*len] = '\0';
		return buf;
	}
}

void StringDestination::assign(std::string_view s)
{
	size_t len = s.length();
	char* ptr = prepare(&len);
	memcpy(ptr, s.data(), len);
}

span<char> ham_strcpy(span<char> dst, std::string_view src)
{
	// Do like strlcpy, then return the remainder of the buffer.
	if (dst.size() > 0)
	{
		size_t len = std::min(src.size(), dst.size() - 1);
		memcpy(dst.data(), src.data(), len);
		dst[len] = '\0';
		return dst.subspan(len);
	}
	return dst;
}

span<char> ham_sprintf(span<char> dst, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	if (dst.size() > 0)
	{
		int desired_without_null = vsnprintf(dst.data(), dst.size(), format, args);
		SDL_assert(desired_without_null >= 0);
		dst = dst.subspan(std::min((size_t)desired_without_null, dst.size() - 1));
	}

	va_end(args);
	return dst;
}

void string_appendf(std::string* buffer, const char* format, ...)
{
	va_list args, args2;
	va_start(args, format);
	va_copy(args2, args);

	// Try to print to the existing capacity. Maybe we'll get lucky and it'll be enough.
	size_t start = buffer->length();
	buffer->resize(buffer->capacity());
	size_t remaining = buffer->size() - start;
	// NB: snprintf's size argument includes the null terminator, which is redundant
	// with the one that std::string supplies, but writing past std::string's end seems iffy,
	// so just erase the extra '\0' afterwards.
	int desired_without_null = vsnprintf(&(*buffer)[start], remaining, format, args);
	SDL_assert(desired_without_null >= 0);

	if ((size_t)desired_without_null >= remaining)
	{
		// It wasn't enough. Grow the buffer to fit everything and print again.
		buffer->resize(start + desired_without_null + 1);
		desired_without_null = vsnprintf(&(*buffer)[start], buffer->size() - start, format, args2);
		buffer->resize(start + desired_without_null);
	}
	else
	{
		// It was enough. Chop off snprintf's null terminator.
		buffer->resize(start + desired_without_null);
	}

	va_end(args2);
	va_end(args);
}
