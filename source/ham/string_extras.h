#ifndef STRING_EXTRAS_H
#define STRING_EXTRAS_H

#include <stdint.h>
#include <cstddef>
#include <string>
#include <string_view>

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

#endif
