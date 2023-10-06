#ifndef VANILLA_EXTRACT_BASE_ARCHIVE_H
#define VANILLA_EXTRACT_BASE_ARCHIVE_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdint.h>

#include "vanilla_extract.h"

namespace vanilla
{
	class Archive
	{
		Archive(const Archive&) = delete;
		Archive& operator=(const Archive&) = delete;
		Archive(Archive&&) = delete;
		Archive& operator=(Archive&&) = delete;

	public:
		struct Directory
		{
			std::map<std::string, size_t, CaseInsensitive> files;
			std::map<std::string, Directory, CaseInsensitive> directories;
		};

		Directory root;

		Archive() {}

		static const char* navigate(const char* path, Directory*& current);
		static const char* navigate(const char* path, const Directory*& current);

		size_t get_file(const char* path) const;
		const Directory* get_directory(const char* path) const;

		bool list_dir(const char* path, std::set<std::string, CaseInsensitive>& output) const;
	};
}  // namespace vanilla

#endif  // VANILLA_EXTRACT_BASE_ARCHIVE_H
