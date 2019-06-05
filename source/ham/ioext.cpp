#include "ioext.h"
#include <streambuf>

const int BUFSZ = 8192;

class FilePtrBuf : public std::streambuf
{
public:
	explicit FilePtrBuf(FILE* f) : f(f) {}
	~FilePtrBuf() {
		// Because we buffer, when we revert control to the FILE*,
		// we must seek backwards by the amount we haven't read.
		fseek(f, gptr() - egptr(), SEEK_CUR);
	}

private:
	FILE* f;
	char buf[BUFSZ];

	int_type underflow() override
	{
		if (gptr() == egptr() && f)
		{
			size_t size = fread(buf, 1, BUFSZ, f);
			setg(buf, buf, buf + size);
		}
		return gptr() == egptr()
			? traits_type::eof()
			: traits_type::to_int_type(*gptr());
	}

	int_type overflow(int_type ch) override
	{
		int i = fputc(ch, f);
		if (i == EOF)
			return traits_type::eof();
		return traits_type::to_int_type(i);
	}

	std::streamsize xsputn(const char_type* s, std::streamsize n) override
	{
		return fwrite(s, 1, n, f);
	}

	int sync() override
	{
		return fflush(f) ? -1 : 0;
	}

	pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) override
	{
		int seek = SEEK_SET;
		switch (dir)
		{
			case std::ios_base::beg: seek = SEEK_SET; break;
			case std::ios_base::end: seek = SEEK_END; break;
			case std::ios_base::cur: seek = SEEK_CUR; break;
			default: break;
		}
		fseek(f, off, seek);
		return ftell(f);
	}
};

FilePtrStream::FilePtrStream(FILE* f)
	: std::iostream(new FilePtrBuf(f))
	, sb(rdbuf())
{
}
