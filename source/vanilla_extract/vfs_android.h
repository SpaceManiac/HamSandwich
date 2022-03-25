#ifndef VANILLA_EXTRACT_ANDROID_H
#define VANILLA_EXTRACT_ANDROID_H
#if defined(__ANDROID__) && __ANDROID__

#include <memory>

namespace vanilla
{
	class Vfs;

	std::unique_ptr<Vfs> open_android();
}

#endif  // __ANDROID__
#endif  // VANILLA_EXTRACT_ANDROID_H
