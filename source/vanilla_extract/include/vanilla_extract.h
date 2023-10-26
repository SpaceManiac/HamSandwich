#ifndef VANILLA_EXTRACT_H
#define VANILLA_EXTRACT_H
// Convenience include for core + all included providers.

#include "vfs.h"

#if __has_include("vfs_stdio.h")
#include "vfs_stdio.h"
#endif
#if __has_include("vfs_nsis.h")
#include "vfs_nsis.h"
#endif
#if __has_include("vfs_inno.h")
#include "vfs_inno.h"
#endif
#if __has_include("vfs_inno3.h")
#include "vfs_inno3.h"
#endif
#if __has_include("vfs_zip.h")
#include "vfs_zip.h"
#endif
#if __has_include("vfs_android.h")
#include "vfs_android.h"
#endif

#endif  // VANILLA_EXTRACT_H
