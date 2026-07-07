#ifndef HAMWORLD_H
#define HAMWORLD_H

#include <stddef.h>
#include <string_view>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "jamultypes.h"
#include "ioext.h"
#include "owned_sdl.h"
#include "string_extras.h"

namespace hamworld {

using std::string_view;

HAM_API size_t size_varint(size_t id);
HAM_API void write_varint(std::ostream& o, size_t id);
HAM_API void write_string(std::ostream& o, string_view s);
HAM_API bool read_varint(std::istream& i, size_t* id);
HAM_API bool read_string(std::istream& i, StringDestination buffer);

class HAM_API Section final
{
public:
	std::stringstream stream;

	void write_varint(size_t number);
	void write_string(string_view s);

	size_t read_varint();
	bool read_string(StringDestination buffer);

	std::string save();
};

class HAM_API Save final
{
	owned::SDL_IOStream stream;
	SdlRwStream output;
public:
	explicit Save(const char* fname);
	~Save();

	void header(string_view author, string_view name, string_view app);
	void section(string_view name, string_view data);
};

class HAM_API Load final
{
	owned::SDL_IOStream stream;
	SdlRwStream input;
public:
	explicit Load(const char* fname);
	~Load();

	bool header(StringDestination author, StringDestination name, StringDestination app);
	bool section(std::string* name, Section* section);
};

}  // namespace hamworld

#endif  // HAMWORLD_H
