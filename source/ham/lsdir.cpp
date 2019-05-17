#include "lsdir.h"
#include "jamultypes.h"
#include <string.h>

#ifdef __GNUC__

lsdir::lsdir(const char* directory)
{
	dir = opendir(directory);
}

lsdir::~lsdir()
{
	if (dir)
	{
		closedir(dir);
	}
}

char* lsdir::next()
{
	struct dirent *dp = dir ? readdir(dir) : nullptr;
	return dp ? dp->d_name : nullptr;
}

// end __GNUC__
#elif defined(_MSC_VER)

lsdir::lsdir(const char* directory)
{
	char* buf = new char[strlen(directory) + 4];
	strcpy(buf, directory);
	strcat(buf, "\\*");

	hFile = _findfirst(buf, &filedata);
	first = true;
	delete[] buf;
}

lsdir::~lsdir()
{
	if (hFile != -1)
	{
		_findclose(hFile);
	}
}

char* lsdir::next()
{
	if (hFile == -1)
	{
		return nullptr;
	}
	if (first)
	{
		first = false;
		return filedata.name;
	}
	if (_findnext(hFile, &filedata) != 0)
	{
		_findclose(hFile);
		hFile = -1;
		return nullptr;
	}
	return filedata.name;
}

#endif // _MSC_VER


filterdir::filterdir(const char* directory, const char* extension, int maxlen)
	: inner(directory)
	, extension(extension)
	, maxlen(maxlen)
	, extlen(extension ? strlen(extension) : 0)
{
}

filterdir::iter filterdir::begin()
{
	iter i = iter(*this, nullptr);
	++i;
	return i;
}

filterdir::iter filterdir::end()
{
	return iter(*this, nullptr);
}

filterdir::iter& filterdir::iter::operator++()
{
	while (value = parent.inner.next()) {
		int len = strlen(value);
		if (parent.maxlen > 0 && len >= parent.maxlen) {
			continue;
		}
		if (parent.extension && (len < parent.extlen || strcasecmp(parent.extension, &value[len - parent.extlen]))) {
			continue;
		}
		break;
	}
	return *this;
}
