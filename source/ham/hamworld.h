#ifndef HAMWORLD_H
#define HAMWORLD_H

#include "jamultypes.h"
#include <stddef.h>
#include <string_view>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

namespace hamworld {

using std::string_view;

class Buffer
{
	void* ptr;
	size_t sz;
public:
	Buffer(std::string* str)
		: ptr(str), sz(SIZE_MAX) {}
	Buffer(char* ch, size_t sz)
		: ptr(ch), sz(sz) {}
	Buffer(std::nullptr_t)
		: ptr(nullptr), sz(0) {}

	template<size_t N>
	Buffer(char (&array)[N])
		: ptr(array), sz(N) {}

	char* prepare(size_t* sz);
	void assign(string_view s);
};

size_t size_varint(size_t id);
void write_varint(std::ostream& o, size_t id);
void write_string(std::ostream& o, string_view s);
bool read_varint(std::istream& i, size_t* id);
bool read_string(std::istream& i, Buffer buffer);

class Section
{
public:
	std::stringstream stream;

	void write_varint(size_t number);
	void write_string(string_view s);

	size_t read_varint();
	bool read_string(Buffer buffer);

	virtual std::string save();
};

class Save final
{
	std::ofstream output;
public:
	Save(const char* fname);
	~Save();

	void header(string_view author, string_view name, string_view app);
	void section(string_view name, string_view data);
};

class Load final
{
	std::ifstream input;
public:
	Load(const char* fname);
	~Load();

	int version();

	bool header(Buffer author, Buffer name, Buffer app);
	bool section(std::string* name, Section* section);
};

}  // namespace hamworld

#endif  // HAMWORLD_H
