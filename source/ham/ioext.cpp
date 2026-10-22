#include "ioext.h"
#include <streambuf>
#include <SDL3/SDL_iostream.h>

constexpr int BUFSZ = 8192;

class SdlRwBuf : public std::streambuf
{
public:
	explicit SdlRwBuf(SDL_IOStream* f) : f(f) {}
	~SdlRwBuf() {
		// Because we buffer, when we revert control to the SDL_IOStream*,
		// we must seek backwards by the amount we haven't read.
		SDL_SeekIO(f, gptr() - egptr(), SDL_IO_SEEK_CUR);
	}

private:
	SDL_IOStream* f;
	char buf[BUFSZ];

	int_type underflow() override
	{
		if (gptr() == egptr() && f)
		{
			size_t size = SDL_ReadIO(f, buf, BUFSZ);
			setg(buf, buf, buf + size);
		}
		return gptr() == egptr()
			? traits_type::eof()
			: traits_type::to_int_type(*gptr());
	}

	int_type overflow(int_type ch) override
	{
		if (traits_type::eq_int_type(ch, traits_type::eof()))
		{
			return 0;
		}
		else
		{
			unsigned char ch2 = ch;
			if (SDL_WriteIO(f, &ch2, 1) < 1)
				return traits_type::eof();
			return ch2;
		}
	}

	std::streamsize xsputn(const char_type* s, std::streamsize n) override
	{
		return SDL_WriteIO(f, s, n);
	}

	int sync() override
	{
		// https://github.com/libsdl-org/SDL/issues/3121
		//return SDL_RWflush(f) ? -1 : 0;
		return 0;
	}

	pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) override
	{
		SDL_IOWhence seek = SDL_IO_SEEK_SET;
		switch (dir)
		{
			case std::ios_base::beg: seek = SDL_IO_SEEK_SET; break;
			case std::ios_base::end: seek = SDL_IO_SEEK_END; break;
			case std::ios_base::cur: seek = SDL_IO_SEEK_CUR; break;
			default: break;
		}
		return SDL_SeekIO(f, off, seek);
	}
};

SdlRwStream::SdlRwStream(SDL_IOStream* f)
	: std::iostream(new SdlRwBuf(f))
	, sb(rdbuf())
{
	if (!f)
	{
		setstate(std::ios_base::failbit);
	}
}
