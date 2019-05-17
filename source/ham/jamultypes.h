/* these are basic types I just can't live without */

#ifndef JAMTYPES_H
#define JAMTYPES_H

// Global typedefs and basic compiler compatibility stuff

typedef unsigned char	byte;
typedef unsigned short	word;
typedef unsigned long	dword;

#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

#endif // HAMTYPES_H
