#ifndef HAMWORLD_H
#define HAMWORLD_H

#include "jamultypes.h"
#include "ioext.h"
#include "owned_sdl.h"
#include <stddef.h>
#include <string_view>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

namespace hamworld {

using std::string_view;

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
	void assign(string_view s);
};

size_t size_varint(size_t id);
void write_varint(std::ostream& o, size_t id);
void write_string(std::ostream& o, string_view s);
bool read_varint(std::istream& i, size_t* id);
bool read_string(std::istream& i, StringDestination buffer);

class Section final
{
public:
	std::stringstream stream;

	void write_varint(size_t number);
	void write_string(string_view s);

	size_t read_varint();
	bool read_string(StringDestination buffer);

	std::string save();
};

class Save final
{
	owned::SDL_RWops stream;
	SdlRwStream output;
public:
	Save(const char* fname);
	~Save();

	void header(string_view author, string_view name, string_view app);
	void section(string_view name, string_view data);
};

class Load final
{
	owned::SDL_RWops stream;
	SdlRwStream input;
public:
	Load(const char* fname);
	~Load();

	int version();

	bool header(StringDestination author, StringDestination name, StringDestination app);
	bool section(std::string* name, Section* section);
};

}  // namespace hamworld

#endif  // HAMWORLD_H
