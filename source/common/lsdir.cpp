#include "lsdir.h"

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

const char* lsdir::next()
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

const char* lsdir::next()
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
