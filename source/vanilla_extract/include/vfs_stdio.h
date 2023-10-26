#ifndef VFS_STDIO_H
#define VFS_STDIO_H

#include "vfs.h"

namespace vanilla
{
	// Given a path like "foo/bar/baz", mkdir "foo" then "foo/bar", but not "baz".
	int mkdir_parents(std::string_view path);

	std::unique_ptr<WriteVfs> open_stdio(std::string_view prefix);
}

#endif
