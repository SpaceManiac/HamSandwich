#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base_archive.h"

namespace vanilla {

// ----------------------------------------------------------------------------
// Archive directory listing

#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

bool Archive::CaseInsensitive::operator() (const std::string& lhs, const std::string& rhs) const
{
	return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

const char* Archive::navigate(const char* path, Directory*& current)
{
	const char* last_component = path;
	for (const char* ch = path; *ch; ++ch)
	{
		if (*ch == '\\' || *ch == '/')
		{
			std::string component { last_component, (size_t)(ch - last_component) };
			if (component != ".")
			{
				current = &current->directories[component];
			}
			last_component = ch + 1;
		}
	}
	return last_component;
}

const char* Archive::navigate(const char* path, const Directory*& current)
{
	const char* last_component = path;
	for (const char* ch = path; *ch; ++ch)
	{
		if (*ch == '\\' || *ch == '/')
		{
			std::string component { last_component, (size_t)(ch - last_component) };
			if (component != ".")
			{
				auto iter = current->directories.find(component);
				if (iter == current->directories.end())
					return nullptr;
				current = &iter->second;
			}
			last_component = ch + 1;
		}
	}
	return last_component;
}

size_t Archive::get_file(const char* path) const
{
	const Directory* current = &root;
	const char* last_component = navigate(path, current);
	if (!current || !last_component)
		return SIZE_MAX;

	auto iter = current->files.find(last_component);
	if (iter == current->files.end())
		return SIZE_MAX;

	return iter->second;
}

const Archive::Directory* Archive::get_directory(const char* path) const
{
	const Directory* current = &root;
	const char* last_component = navigate(path, current);
	if (!current || !last_component)
		return nullptr;

	if (!strcmp(last_component, "."))
		return current;

	auto iter = current->directories.find(last_component);
	if (iter == current->directories.end())
		return nullptr;

	return &iter->second;
}

bool Archive::list_dir(const char* path, std::set<std::string>& output) const
{
	const Directory* current = get_directory(path);
	if (!current)
		return false;

	for (const auto& pair : current->files)
		output.insert(pair.first);

	for (const auto& pair : current->directories)
		output.insert(pair.first);

	return true;
}

}  // namespace vanilla
