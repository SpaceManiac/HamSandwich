#ifndef JSPFILE_H
#define JSPFILE_H

#include "globals.h"
#include <vector>

struct JspFrame {
    Bitmap bmp;
    int ofsX, ofsY;
};

struct JspFile {
    std::vector<JspFrame> frames;
    std::string error;

    bool load(std::string fname);
    bool save(std::string fname);
};

#endif // JSPFILE_H
