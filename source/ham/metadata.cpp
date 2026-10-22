#include "metadata.h"
#include <string>
#include "owned_stdio.h"

bool AssetSpec::should_auto_mount() const
{
	if (!optional)
		return true;

	std::string enabled_file = param;
	enabled_file.append(".enabled");
	return owned::fopen(enabled_file.c_str(), "rb") != nullptr;
}

bool AddonSpec::is_enabled() const
{
	std::string disabled_file = filename;
	disabled_file.append(".disabled");
	return source_vfs->open_sdl(disabled_file.c_str()) == nullptr;
}

void AddonSpec::set_enabled(bool enabled)
{
	std::string disabled_file = filename;
	disabled_file.append(".disabled");
	if (enabled)
	{
		source_vfs->delete_file(disabled_file.c_str());
	}
	else
	{
		source_vfs->open_write_sdl(disabled_file.c_str());
	}
}
