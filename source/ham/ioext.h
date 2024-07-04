#ifndef IOEXT_H
#define IOEXT_H

#include <istream>
#include <memory>

struct SDL_RWops;

class FilePtrStream : public std::iostream
{
	std::unique_ptr<std::streambuf> sb;
public:
	explicit FilePtrStream(FILE *f);
};

class SdlRwStream : public std::iostream
{
	std::unique_ptr<std::streambuf> sb;
public:
	explicit SdlRwStream(SDL_RWops *f);
};

#endif  // IOEXT_H
