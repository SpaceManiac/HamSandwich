#ifndef METADATA_H
#define METADATA_H

#include <vector>
#include <string>
#include "vanilla_extract.h"

struct AssetSpec
{
	const char* mountpoint;
	const char* kind;
	const char* param;
	bool optional;

	bool should_auto_mount() const;
	bool set_enabled(bool enabled);
};

struct HamSandwichMetadata
{
	const char* appdata_folder_name;
	const AssetSpec* default_asset_specs;
};

struct AddonSpec
{
	vanilla::WriteVfs* source_vfs;
	std::string filename;

	bool is_enabled() const;
	void set_enabled(bool enabled);

	static std::vector<AddonSpec> SearchAddons(vanilla::WriteVfs* vfs);
};

#endif  // METADATA_H
