#ifndef STRING_EXTRAS_H
#define STRING_EXTRAS_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <string_view>
#include <SDL_stdinc.h>

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

// A cheap standin for std::span, which isn't available until C++20.
// `span<char>` is suitable for fixed-size output buffers.
// Prefer `std::string` instead for growable output buffers.
// Prefer `std::string_view` instead for string inputs.
template<class T>
class span
{
	T* _data;
	size_t _size;
public:
	constexpr span() noexcept : _data(nullptr), _size(0) {}
	constexpr span(SDL_OUT_Z_CAP(count) T* first, size_t count) : _data(first), _size(count) {}
	// This is the magic that safely constructs a span from a char array.
	template<size_t N>
	constexpr span(T (&arr)[N]) noexcept : _data(arr), _size(N) {}
	constexpr span(const span& other) noexcept = default;
	// Real std::span is constructible from std::string but we don't actually want that.

	constexpr span& operator=(const span& other) noexcept = default;

	constexpr T& operator[](size_t idx) const { return _data[idx]; }
	constexpr T* data() const noexcept { return _data; }
	constexpr size_t size() const noexcept { return _size; }
	constexpr bool empty() const noexcept { return _size == 0; }

	constexpr span subspan(size_t offset, size_t count = SIZE_MAX)
	{
		return span(_data + offset, count == SIZE_MAX ? _size - offset : count);
	}
};

// Like strlcpy. Always leaves a null terminator. Returns the remaining slice of the buffer.
// Use in preference to strcpy, strcat, strncpy, strncat, strlcpy, strlcat.
span<char> ham_strcpy(span<char> dst, std::string_view src);
// Like sprintf or snprintf but auto-detects destination buffer length.
span<char> ham_sprintf(span<char> dst, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2);

// sprintf-style append to a std::string.
void string_appendf(std::string* buffer, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2);

// fprintf-like to an SDL_RWops.
struct SDL_RWops;
int SDL_RWprintf(SDL_RWops* rw, SDL_PRINTF_FORMAT_STRING const char* format, ...) SDL_PRINTF_VARARG_FUNC(2);

#endif
