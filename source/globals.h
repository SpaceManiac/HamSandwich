#ifndef GLOBALS_H
#define GLOBALS_H

//#include <sys/types.h>
#include <alx.hpp>
#include <muus/muus.hpp>
#include <string>

using namespace alx;
using namespace muus;

enum {
    DISPLAY_WIDTH = 1024,
    DISPLAY_HEIGHT = 768
};

extern Display display;
extern Font gFont;

//extern AssetContext<Bitmap> graphics;

#endif // GLOBALS_H
