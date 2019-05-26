#ifndef JSPFILE_H
#define JSPFILE_H

#include "globals.h"
#include <vector>
#include <memory>

struct JspFrame {
    std::shared_ptr<SDL_Texture> bmp;
    int16_t ofsX, ofsY;

    JspFrame() = default;
    JspFrame(const JspFrame &other) = default;
    JspFrame(int w, int h);
};

struct JspFile {
    std::vector<JspFrame> frames;
    std::string error;

    bool load(std::string fname);
    bool save(std::string fname);
};

#endif // JSPFILE_H
