#ifndef JAMULTYPES_H
#define JAMULTYPES_H

/* these are basic types I just can't live without */
#include <stdint.h>
typedef uint8_t  byte;
typedef uint16_t word;
typedef uint32_t dword;

// Define strcasecmp for Windows.
#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

// HAM_API is set in CMakeLists.txt if needed (for Windows .dll),
// otherwise it's set to nothing here.
#ifndef HAM_API
	#define HAM_API
#endif

#endif // JAMULTYPES_H
