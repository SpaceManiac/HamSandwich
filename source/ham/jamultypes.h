/* these are basic types I just can't live without */

#ifndef JAMTYPES_H
#define JAMTYPES_H

#include <stdint.h>

// Global typedefs and basic compiler compatibility stuff

typedef uint8_t  byte;
typedef uint16_t word;
typedef uint32_t dword;

#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

#endif // HAMTYPES_H
