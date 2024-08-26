#include "ioext.h"
#include <streambuf>
#include <SDL_rwops.h>

constexpr int BUFSZ = 8192;

class SdlRwBuf : public std::streambuf
{
public:
	explicit SdlRwBuf(SDL_RWops* f) : f(f) {}
	~SdlRwBuf() {
		// Because we buffer, when we revert control to the SDL_RWops*,
		// we must seek backwards by the amount we haven't read.
		SDL_RWseek(f, gptr() - egptr(), RW_SEEK_CUR);
	}

private:
	SDL_RWops* f;
	char buf[BUFSZ];

	int_type underflow() override
	{
		if (gptr() == egptr() && f)
		{
			size_t size = SDL_RWread(f, buf, 1, BUFSZ);
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
			char ch2 = ch;
			if (SDL_RWwrite(f, &ch2, 1, 1) < 1)
				return traits_type::eof();
			return ch2;
		}
	}

	std::streamsize xsputn(const char_type* s, std::streamsize n) override
	{
		return SDL_RWwrite(f, s, 1, n);
	}

	int sync() override
	{
		// https://github.com/libsdl-org/SDL/issues/3121
		//return SDL_RWflush(f) ? -1 : 0;
		return 0;
	}

	pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) override
	{
		int seek = RW_SEEK_SET;
		switch (dir)
		{
			case std::ios_base::beg: seek = RW_SEEK_SET; break;
			case std::ios_base::end: seek = RW_SEEK_END; break;
			case std::ios_base::cur: seek = RW_SEEK_CUR; break;
			default: break;
		}
		return SDL_RWseek(f, off, seek);
	}
};

SdlRwStream::SdlRwStream(SDL_RWops* f)
	: std::iostream(new SdlRwBuf(f))
	, sb(rdbuf())
{
	if (!f)
	{
		setstate(std::ios_base::failbit);
	}
}
