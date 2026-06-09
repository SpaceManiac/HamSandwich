#ifndef STRING_EXTRAS_H
#define STRING_EXTRAS_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <SDL3/SDL_stdinc.h>

// A variant describing several possible places a string might be written.
class StringDestination
{
	void* ptr;
	size_t sz;
	constexpr static size_t SZ_STD_STRING = SIZE_MAX;
public:
	// Empty destination that cannot actually be written to.
	StringDestination(std::nullptr_t)
		: ptr(nullptr), sz(0) {}
	// Simple pointer-and-size destination. Space is reserved for NUL.
	StringDestination(char* ch, size_t sz)
		: ptr(ch), sz(sz) {}
	// A destination which will overwrite the given std::string.
	StringDestination(std::string* str)
		: ptr(str), sz(SZ_STD_STRING) {}

	// Fixed-size character array destination. Space is reserved for NUL.
	template<size_t N>
	StringDestination(char (&array)[N])
		: ptr(array), sz(N) {}

	// Attempt to reserve enough space for `*len` characters, not including the null terminator.
	// The amount of space actually available is written back to `*len`.
	// Returns nullptr and 0 if there is no space at all.
	char* prepare(size_t* len);
	// Assign as much of a given string_view as possible to this destination.
	void assign(std::string_view s);
};

// Like strlcpy. Always leaves a null terminator. Returns the remaining slice of the buffer.
// Use in preference to strcpy, strcat, strncpy, strncat, strlcpy, strlcat.
std::span<char> ham_strcpy(std::span<char> dst, std::string_view src);
// To avoid mistakes. Use operator= or .assign instead.
void ham_strcpy(std::string& dst, std::string_view src) = delete;

// Like sprintf or snprintf but auto-detects destination buffer length.
std::span<char> ham_sprintf(std::span<char> dst, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2);
// To avoid mistakes. Use .clear() and string_appendf instead.
void ham_sprintf(std::string& dst, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2) = delete;

// sprintf-style append to a std::string.
void string_appendf(std::string* buffer, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2);

// fprintf-like to an SDL_IOStream.
struct SDL_IOStream;
size_t SDL_RWprintf(SDL_IOStream* rw, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2);

#endif
