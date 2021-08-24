#ifndef METADATA_H
#define METADATA_H

struct HamSandwichMetadata
{
	const char* appdata_folder_name;
	const char* const* default_asset_specs;
};

// The implementation for this function is included in the build automatically;
// see `tools/build/appdata.lua`.
extern const HamSandwichMetadata* GetHamSandwichMetadata();

#endif  // METADATA_H
