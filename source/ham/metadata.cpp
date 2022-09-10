#include "metadata.h"
#include <string>

bool AssetSpec::should_auto_mount() const
{
	if (!optional)
		return true;

	std::string enabled_file = param;
	enabled_file.append(".enabled");
	FILE* f = fopen(enabled_file.c_str(), "rb");
	if (f)
	{
		fclose(f);
		return true;
	}
	return false;
}
