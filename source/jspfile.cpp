#include "jspfile.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "palette.h"

using namespace std;

constexpr int JSPEDIT2_MAGIC = 0xBEEFED02;

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
    if (!read(in, &count)) {
        error = "couldn't read ct";
        return false;
    }
    cout << "count: " << count << endl;

    // read header
    std::vector<FrameInfo> frameInfo;
    for (uint16_t i = 0; i < count; ++i) {
        FrameInfo info;
        if (!read(in, &info)) {
            error = "couldn't read frameinfo";
            return false;
        }

        //cout << "frame #" << i << ": " << info.width << "x" << info.height << ", (" << info.ofsX << "," << info.ofsY << "), size = " << info.size << endl;
        frameInfo.push_back(info);
    }

    //cout << "at " << in.tellg() << endl;

    // read image
    frames.clear();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    for (uint16_t i = 0; i < count; ++i) {
        //cout << "decoding frame #" << i << endl;

        FrameInfo info = frameInfo[i];
        std::shared_ptr<SDL_Texture> image(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, info.width, info.height), SDL_DestroyTexture);
        if (!image) {
            cout << "could not create " << info.width << "x" << info.height << " image for frame #" << i << endl;
            error = "could not create image";
            return false;
        }

        SDL_SetRenderTarget(renderer, image.get());

        int x = 0, y = 0;
        uint32_t read = 0;
        while (read < info.size) {
            uint8_t value = in.get();
            ++read;
            if (value >= 128) {
                // transparency run
                for (uint8_t j = 128; j < value; ++j) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderDrawPoint(renderer, x, y);
                    if (++x >= info.width) {
                        x = 0; ++y;
                    }
                }
            } else {
                // color run
                for (uint8_t j = 0; j < value; ++j) {
                    uint8_t index = in.get();
                    ++read;
                    SDL_Color color = palette::getColor(index);
                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                    if (++x >= info.width) {
                        x = 0; ++y;
                    }
                }
            }

            if (!in) {
                error = "couldn't read images";
                return false;
            }
        }

        frames.push_back({ image, info.ofsX, info.ofsY });
    }
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    return true;
}

bool JspFile::save(string fname) {
    cout << "saving " << fname << endl;

    ofstream out { fname, ios::out | ios::binary };
    if (!out) {
        error = "could not open file for writing";
        return false;
    }

    // write count
    uint16_t count = frames.size();
    write(out, &count);

    // perform RLE compression
    std::vector<FrameInfo> frameInfo;
    std::vector<std::vector<uint8_t>> frameData;

    for (uint16_t i = 0; i < count; ++i) {
        std::cout << "encoding frame #" << i << endl;
        JspFrame frame = frames[i];

        int w, h;
        SDL_QueryTexture(frame.bmp.get(), nullptr, nullptr, &w, &h);
        FrameInfo info { (uint16_t) w, (uint16_t) h, frame.ofsX, frame.ofsY, 0, 0 };
        if (i == 0) {
            // extra data space on first image set to magic value
            info.extra = JSPEDIT2_MAGIC;
        }

        // convert bitmap to palette
        uint32_t bmp_size = w * h;
        std::vector<uint8_t> temp(bmp_size);
        //std::cout << "  palette conversion" << endl;
        // TODO
        /*frame.bmp.lock(ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_READONLY);
        for (int y = 0; y < info.height; ++y) {
            for (int x = 0; x < info.width; ++x) {
                temp[y * info.width + x] = palette::getExact(frame.bmp.getPixel(x, y));
            }
        }
        frame.bmp.unlock();*/

        // worst possible scenario is 1.5 bytes per pixel (alternating transparency and solidity)
        std::vector<uint8_t> data(bmp_size * 2);
        uint32_t pos = 0, n = 0;  // pos tracks output, n tracks pixels

        //std::cout << "  run length encoding" << endl;
        while (n < bmp_size) {
            int len = 0;
            uint8_t c = temp[n];

            // a continuous run must end at any of:
            // the end of the bitmap
            // the end of the line (row)
            // a different solid/transparent status
            // a 127-length run

            if (c == 0) {
                // determine transparent run length
                while (n+len <= bmp_size && (n % info.width) + len < info.width && len < 127 && temp[n+len] == 0) ++len;
                // write
                data[pos++] = 128 + len;
                n += len;
            } else {
                // determine solid run length
                while (n+len <= bmp_size && (n % info.width) + len < info.width && len < 127 && temp[n+len] != 0) ++len;
                // write
                data[pos++] = len;
                for (int j = 0; j < len; ++j) data[pos++] = temp[n+j];
                n += len;
            }
        }

        // add to list
        info.size = pos;
        frameInfo.push_back(info);
        frameData.push_back(data);
    }

    // write header
    for (uint16_t i = 0; i < count; ++i) {
        write(out, &frameInfo[i]);
    }

    // write image
    for (uint16_t i = 0; i < count; ++i) {
        out.write((char*) &frameData[i][0], frameInfo[i].size);
    }

    return true;
}
