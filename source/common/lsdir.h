#ifndef LSDIR_H
#define LSDIR_H

#ifdef __GNUC__
#include <dirent.h>

class lsdir
{
	DIR* dir;

public:
	explicit lsdir(const char* directory);
	~lsdir();
	const char* next();

private:
	lsdir(const lsdir&) = delete;
	lsdir(lsdir&&) = delete;
	lsdir& operator=(const lsdir&) = delete;
	lsdir& operator=(lsdir&&) = delete;
};

// end __GNUC__
#elif defined(_MSC_VER)
#include <io.h>

class lsdir
{
	long hFile;
	struct _finddata_t filedata;
	bool first;

public:
	explicit lsdir(const char* directory);
	~lsdir();
	const char* next();

private:
	lsdir(const lsdir&) = delete;
	lsdir(lsdir&&) = delete;
	lsdir& operator=(const lsdir&) = delete;
	lsdir& operator=(lsdir&&) = delete;
};

#endif // _MSC_VER
#endif // LSDIR_H
