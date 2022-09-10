#ifndef METADATA_H
#define METADATA_H

struct AssetSpec
{
	const char* mountpoint;
	const char* kind;
	const char* param;
	bool optional;

	bool should_auto_mount() const;
};

struct HamSandwichMetadata
{
	const char* appdata_folder_name;
	const AssetSpec* default_asset_specs;
};

#endif  // METADATA_H
