#include "jspfile.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "palette.h"

using namespace std;

constexpr int JSPEDIT2_MAGIC = 0xBEEFED00;

template<typename T>
static istream& read(istream& in, T* ptr) {
    in.read((char*) ptr, sizeof(T));
    return in;
}

template<typename T>
static ostream& write(ostream& out, T* ptr) {
    out.write((char*) ptr, sizeof(T));
    return out;
}

struct FrameInfo {
    uint16_t width;
    uint16_t height;
    int16_t ofsX;
    int16_t ofsY;
    uint32_t size;
    uint32_t extra;
};

bool JspFile::load(string fname) {
    cout << "loading " << fname << endl;

    ifstream in { fname, ios::in | ios::binary };

    // read count
    uint16_t count;
    read(in, &count);
    cout << "count: " << count << endl;

    // read header
    std::vector<FrameInfo> frameInfo;
    for (uint16_t i = 0; i < count; ++i) {
        FrameInfo info;
        if (!read(in, &info)) {
            error = "input failed";
            return false;
        }

        cout << "frame #" << i << ": " << info.width << "x" << info.height << ", (" << info.ofsX << "," << info.ofsY << "), size = " << info.size << endl;
        frameInfo.push_back(info);
    }

    cout << "at " << in.tellg() << endl;

    // read image
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    frames.clear();
    for (uint16_t i = 0; i < count; ++i) {
        cout << "decoding frame #" << i << endl;

        FrameInfo info = frameInfo[i];
        Bitmap image { info.width, info.height };
        if (!image) {
            cout << "could not create " << info.width << "x" << info.height << " image for frame #" << i << endl;
            error = "could not create image";
            return false;
        }
        gfx::SetTarget target { image };

        //ALLEGRO_LOCKED_REGION* lock = image.lock(ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

        int x = 0, y = 0;
        uint32_t read = 0;
        while (read < info.size) {
            uint8_t value = in.get();
            ++read;
            if (value >= 128) {
                // transparency run
                for (uint8_t j = 128; j < value; ++j) {
                    al_put_pixel(x, y, Color { 0, 0, 0, 0 });
                    if (++x >= info.width) {
                        x = 0; ++y;
                    }
                }
            } else {
                // color run
                for (uint8_t j = 0; j < value; ++j) {
                    uint8_t index = in.get();
                    ++read;
                    al_put_pixel(x, y, palette::getColor(index));
                    if (++x >= info.width) {
                        x = 0; ++y;
                    }
                }
            }
        }

        frames.push_back({ image, info.ofsX, info.ofsY });
    }

    return true;
}

