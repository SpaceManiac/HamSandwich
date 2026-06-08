#ifndef IOEXT_H
#define IOEXT_H

#include <istream>
#include <memory>

struct SDL_IOStream;

class SdlRwStream : public std::iostream
{
	std::unique_ptr<std::streambuf> sb;
public:
	explicit SdlRwStream(SDL_IOStream *f);
};

#endif  // IOEXT_H
