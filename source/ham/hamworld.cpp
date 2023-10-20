#include "hamworld.h"
#include "log.h"
#include "appdata.h"
#include <string.h>

namespace hamworld {

static const int CODE_LENGTH = 8;
static const char CODE[CODE_LENGTH + 1] = "HAMSWCH!";

static const int VERSION = 1;

size_t size_varint(size_t id)
{
	size_t i = 1;
	while (id > 127)
	{
		id >>= 7;
		i++;
	}
	return i;
}

void write_varint(std::ostream& o, size_t id)
{
	while (id > 127)
	{
		o.put((id & 127) | 128);
		id >>= 7;
	}
	o.put(id);
}

void write_string(std::ostream& o, string_view s)
{
	write_varint(o, s.size());
	o.write(s.data(), s.size());
	//o.put(0);
}

bool read_varint(std::istream& i, size_t* id)
{
	int shift = 0;
	char ch;
	*id = 0;
	do
	{
		ch = i.get();
		if (!i)
			return false;
		*id += (ch & 127) << shift;
		shift += 7;
	} while (ch & 128);
	return true;
}

bool read_string(std::istream& i, StringDestination buffer)
{
	size_t src_len;
	if (!read_varint(i, &src_len))
		return false;
	size_t dest_len = src_len;
	char* buf = buffer.prepare(&dest_len);
	if (buf && dest_len && !i.read(buf, dest_len))
		return false;
	if (src_len > dest_len && !i.ignore(src_len - dest_len))
		return false;
	return true;
}

void Section::write_varint(size_t id)
{
	hamworld::write_varint(stream, id);
}

void Section::write_string(string_view s)
{
	hamworld::write_string(stream, s);
}

size_t Section::read_varint()
{
	size_t result;
	if (!hamworld::read_varint(stream, &result))
	{
		throw std::runtime_error("error in Section::read_varint");
	}
	return result;
}

bool Section::read_string(StringDestination buffer)
{
	return hamworld::read_string(stream, buffer);
}

std::string Section::save()
{
	return stream.str();
}

Save::Save(const char* fname)
	: stream(AppdataOpen_Write_SDL(fname))
	, output(stream.get())
{
}

Save::~Save()
{
	output.put(0);
}

void Save::header(string_view author, string_view name, string_view app)
{
	output << CODE;
	write_varint(output, VERSION);  // version

	Section header;
	header.write_string(author);
	header.write_string(name);
	header.write_string(app);
	header.write_varint(0);  // empty metadata table
	section(string_view(), header.save());
}

void Save::section(string_view name, string_view body)
{
	size_t size = size_varint(name.length()) + name.length() + body.length();
	write_varint(output, size);
	write_string(output, name);
	output.write(body.data(), body.size());
}

Load::Load(const char* fname)
	: stream(AssetOpen_SDL_Owned(fname))
	, input(stream.get())
{
}

Load::~Load()
{
}

bool Load::header(StringDestination author, StringDestination name, StringDestination app)
{
	char buf[CODE_LENGTH];
	if (!input.read(buf, CODE_LENGTH) || strncmp(buf, CODE, CODE_LENGTH))
		return false;

	if (input.get() != VERSION)
		return false;

	std::string hdrname;
	Section hdr;
	if (!section(&hdrname, &hdr) || !hdrname.empty())
		return false;

	if (!hdr.read_string(author) || !hdr.read_string(name) || !hdr.read_string(app))
		return false;

	return true;
}

bool Load::section(std::string *name, Section *sec)
{
	size_t len;
	if (!read_varint(input, &len))
		return false;

	// read that much
	std::string buffer(len, '\0');
	if (!input.read(buffer.data(), len))
		return false;

	sec->stream.clear();
	sec->stream.str(buffer);

	if (!sec->read_string(name))
		return false;

	return true;
}

}  // namespace hamworld
