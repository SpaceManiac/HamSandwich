#ifndef VFS_ANDROID_H
#define VFS_ANDROID_H

#include "vfs.h"

#if defined(__ANDROID__) && __ANDROID__
namespace vanilla
{
	std::unique_ptr<Vfs> open_android(const char* prefix = nullptr);
}
#endif  // __ANDROID__

#endif  // VFS_ANDROID_H
