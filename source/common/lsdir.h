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
	char* next();

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
	char* next();

private:
	lsdir(const lsdir&) = delete;
	lsdir(lsdir&&) = delete;
	lsdir& operator=(const lsdir&) = delete;
	lsdir& operator=(lsdir&&) = delete;
};

#endif // _MSC_VER

class filterdir
{
	lsdir inner;
	const char* extension;
	int maxlen;
	int extlen;

public:
	explicit filterdir(const char* directory, const char* extension = nullptr, int maxlen = 260);

	class iter {
		filterdir& parent;
		char* value;

		friend class filterdir;
		iter(filterdir& parent, char* value) : parent(parent), value(value) {}
	public:
		bool operator!=(const iter& other) { return value != other.value; }
		char* operator*() { return value; }
		iter& operator++();
	};

	iter begin();
	iter end();
};

#endif // LSDIR_H
