#include "palette.h"
#include "globals.h"

static uint8_t pal[256][3] = {
    {0, 0, 0}, {8, 8, 8}, {16, 16, 16}, {24, 24, 24}, {33, 33, 33}, {41, 41, 41}, {49, 49, 49}, {57, 57, 57},
    {65, 65, 65}, {73, 73, 73}, {81, 81, 81}, {89, 89, 89}, {98, 98, 98}, {106, 106, 106}, {114, 114, 114}, {122, 122, 122},
    {130, 130, 130}, {138, 138, 138}, {146, 146, 146}, {154, 154, 154}, {163, 163, 163}, {171, 171, 171}, {179, 179, 179}, {187, 187, 187},
    {195, 195, 195}, {203, 203, 203}, {211, 211, 211}, {219, 219, 219}, {228, 228, 228}, {236, 236, 236}, {244, 244, 244}, {252, 252, 252},
    {0, 0, 0}, {0, 17, 0}, {0, 34, 0}, {0, 50, 0}, {0, 67, 0}, {0, 84, 0}, {0, 101, 0}, {0, 118, 0},
    {0, 134, 0}, {0, 151, 0}, {0, 168, 0}, {0, 185, 0}, {0, 202, 0}, {0, 218, 0}, {0, 235, 0}, {0, 252, 0},
    {16, 252, 16}, {32, 252, 32}, {47, 252, 47}, {63, 252, 63}, {79, 252, 79}, {95, 252, 95}, {110, 252, 110}, {126, 252, 126},
    {142, 252, 142}, {158, 252, 158}, {173, 252, 173}, {189, 252, 189}, {205, 252, 205}, {221, 252, 221}, {236, 252, 236}, {252, 252, 252},
    {0, 0, 0}, {9, 7, 3}, {19, 13, 6}, {28, 20, 10}, {37, 27, 13}, {47, 33, 16}, {56, 40, 19}, {65, 47, 22},
    {75, 53, 26}, {84, 60, 29}, {93, 67, 32}, {103, 73, 35}, {112, 80, 38}, {121, 87, 42}, {131, 93, 45}, {140, 100, 48},
    {147, 110, 61}, {154, 119, 74}, {161, 129, 86}, {168, 138, 99}, {175, 148, 112}, {182, 157, 125}, {189, 167, 137}, {196, 176, 150},
    {203, 186, 163}, {210, 195, 176}, {217, 205, 188}, {224, 214, 201}, {231, 224, 214}, {238, 233, 227}, {245, 243, 239}, {252, 252, 252},
    {0, 0, 0}, {0, 0, 17}, {0, 0, 34}, {0, 0, 51}, {0, 0, 68}, {0, 0, 85}, {0, 0, 102}, {0, 0, 119},
    {0, 0, 136}, {0, 0, 153}, {0, 0, 170}, {0, 0, 187}, {0, 0, 204}, {0, 0, 221}, {0, 0, 238}, {0, 0, 255},
    {16, 16, 255}, {32, 32, 255}, {47, 47, 254}, {63, 63, 254}, {79, 79, 254}, {95, 95, 254}, {110, 110, 254}, {126, 126, 254},
    {142, 142, 253}, {158, 158, 253}, {173, 173, 253}, {189, 189, 253}, {205, 205, 253}, {221, 221, 252}, {236, 236, 252}, {252, 252, 252},
    {0, 0, 0}, {17, 0, 0}, {34, 0, 0}, {50, 0, 0}, {67, 0, 0}, {84, 0, 0}, {101, 0, 0}, {118, 0, 0},
    {134, 0, 0}, {151, 0, 0}, {168, 0, 0}, {185, 0, 0}, {202, 0, 0}, {218, 0, 0}, {235, 0, 0}, {252, 0, 0},
    {252, 16, 16}, {252, 32, 32}, {252, 47, 47}, {252, 63, 63}, {252, 79, 79}, {252, 95, 95}, {252, 110, 110}, {252, 126, 126},
    {252, 142, 142}, {252, 158, 158}, {252, 173, 173}, {252, 189, 189}, {252, 205, 205}, {252, 221, 221}, {252, 236, 236}, {252, 252, 252},
    {0, 0, 0}, {17, 17, 0}, {34, 34, 0}, {51, 51, 0}, {68, 68, 0}, {85, 85, 0}, {102, 102, 0}, {119, 119, 0},
    {136, 136, 0}, {153, 153, 0}, {170, 170, 0}, {187, 187, 0}, {204, 204, 0}, {221, 221, 0}, {238, 238, 0}, {255, 255, 0},
    {255, 255, 16}, {255, 255, 32}, {254, 254, 47}, {254, 254, 63}, {254, 254, 79}, {254, 254, 95}, {254, 254, 110}, {254, 254, 126},
    {253, 253, 142}, {253, 253, 158}, {253, 253, 173}, {253, 253, 189}, {253, 253, 205}, {252, 252, 221}, {252, 252, 236}, {252, 252, 252},
    {0, 0, 0}, {17, 0, 17}, {34, 0, 34}, {51, 0, 51}, {68, 0, 68}, {85, 0, 85}, {102, 0, 102}, {119, 0, 119},
    {136, 0, 136}, {153, 0, 153}, {170, 0, 170}, {187, 0, 187}, {204, 0, 204}, {221, 0, 221}, {238, 0, 238}, {255, 0, 255},
    {255, 16, 255}, {255, 32, 255}, {254, 47, 254}, {254, 63, 254}, {254, 79, 254}, {254, 95, 254}, {254, 110, 254}, {254, 126, 254},
    {253, 142, 253}, {253, 158, 253}, {253, 173, 253}, {253, 189, 253}, {253, 205, 253}, {252, 221, 252}, {252, 236, 252}, {252, 252, 252},
    {0, 0, 0}, {0, 17, 17}, {0, 34, 34}, {0, 51, 51}, {0, 68, 68}, {0, 85, 85}, {0, 102, 102}, {0, 119, 119},
    {0, 136, 136}, {0, 153, 153}, {0, 170, 170}, {0, 187, 187}, {0, 204, 204}, {0, 221, 221}, {0, 238, 238}, {0, 255, 255},
    {16, 255, 255}, {32, 255, 255}, {47, 254, 254}, {63, 254, 254}, {79, 254, 254}, {95, 254, 254}, {110, 254, 254}, {126, 254, 254},
    {142, 253, 253}, {158, 253, 253}, {173, 253, 253}, {189, 253, 253}, {205, 253, 253}, {221, 252, 252}, {236, 252, 252}, {252, 252, 252}
};

SDL_Color palette::getColor(uint8_t index) {
    if (index == 0) {
        return { 0, 0, 0, 0 }; // transparent
    }
    return { pal[index][0], pal[index][1], pal[index][2], 255 };
}

uint8_t palette::getExact(SDL_Color color) {
    if (color.a < 128) return 0;

    for (int i = 1; i < 256; ++i) {
        SDL_Color c = getColor(i);
        if (!memcmp(&c, &color, sizeof(SDL_Color))) {
            return i;
        }
    }
    printf("Exactness test failed: %d,%d,%d,%d\n", color.r, color.g, color.b, color.a);
    return 0;
}

uint8_t palette::getNearest(SDL_Color color) {
    if (color.a < 128) return 0;

    uint8_t r1 = color.r, g1 = color.g, b1 = color.b;

    //if (r1 == 0 && g1 == 0 && b1 == 0) return 0;

    uint8_t best = 0;
    double best_dist = 0;
    for (int i = 1; i < 256; ++i) {
        uint8_t r2 = pal[i][0], g2 = pal[i][1], b2 = pal[i][2];

        double rmean = (r1 + r2) / 2.0;
        double r = r1 - r2;
        double g = g1 - g2;
        int b = b1 - b2;
        double weightR = 2 + rmean / 256.0;
        double weightG = 4.0;
        double weightB = 2 + (255 - rmean) / 256.0;
        double dist = weightR * r * r + weightG * g * g + weightB * b * b;

        if (dist < best_dist || best == 0) {
            best = i;
            best_dist = dist;
        }
        if (dist == 0) break;
    }

    return best;
}

bool palette::reduceImage(SDL_Surface *image) {
    bool result = false;

    SDL_LockSurface(image);
    SDL_Color *pixels = (SDL_Color *) image->pixels;
    int pitch = image->pitch / sizeof(SDL_Color);

    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            SDL_Color original = pixels[y * pitch + x];
            uint8_t index = getNearest(original);
            SDL_Color actual = getColor(index);
            if (memcmp(&original, &actual, sizeof(SDL_Color))) {
                pixels[y * pitch + x] = actual;
                result = true;
            }
        }
    }

    SDL_UnlockSurface(image);
    return result;
}