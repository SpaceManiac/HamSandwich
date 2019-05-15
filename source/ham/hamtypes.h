#ifndef HAMTYPES_H
#define HAMTYPES_H

// Global typedefs and basic compiler compatibility stuff

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

#endif // HAMTYPES_H
