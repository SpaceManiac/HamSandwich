// LunaticPal v2, 2022
// Based on LunaticPal v1 released on Wombat in 2013.
// Converts any image SDL_image can read into a .bmp in the Dr. L palette.

#include <vector>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <tinyfiledialogs.h>

const uint8_t palette[256][3] =
{
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
	{142, 253, 253}, {158, 253, 253}, {173, 253, 253}, {189, 253, 253}, {205, 253, 253}, {221, 252, 252}, {236, 252, 252}, {252, 252, 252},
};

uint8_t convert_color(SDL_Color c)
{
	uint8_t r1 = c.r, g1 = c.g, b1 = c.b;

	if (r1 == 0 && g1 == 0 && b1 == 0)
		return 0;

	uint8_t best = 0;
	int best_dist = 0;
	for (int i = 1; i < 256; ++i)
	{
		uint8_t r2 = palette[i][0], g2 = palette[i][1], b2 = palette[i][2];

		double rmean = (r1 + r2) / 2.0;
		double r = r1 - r2;
		double g = g1 - g2;
		int b = b1 - b2;
		double weightR = 2 + rmean / 256.0;
		double weightG = 4.0;
		double weightB = 2 + (255 - rmean) / 256.0;
		double dist = weightR * r * r + weightG * g * g + weightB * b * b;

		if (dist < best_dist || best == 0)
		{
			best = i;
			best_dist = dist;
		}
	}

	return best;
}

void putshort(std::ostream& out, short value)
{
	out.put(value & 0xff);
	out.put((value >> 8) & 0xff);
}

void putlong(std::ostream& out, long value)
{
	out.put(value & 0xff);
	out.put((value >> 8) & 0xff);
	out.put((value >> 16) & 0xff);
	out.put((value >> 24) & 0xff);
}

void write_bmp(int width, int height, const uint8_t* pixels, const char* dest)
{
	std::ofstream out {dest, std::ios_base::binary};

	int padwidth = (width + 3) / 4 * 4;

	int headersize = 1078; // includes palette
	int imagesize = height * padwidth;

	// File Header
	out.put('B');
	out.put('M');
	putlong(out, headersize + imagesize); // size of file
	putshort(out, 0); // reserved
	putshort(out, 0); // reserved
	putlong(out, headersize); // start of pixel data

	// BITMAPINFOHEADER
	putlong(out, 40); // 40 is the size of bitmapinfoheader
	putlong(out, width);
	putlong(out, height);
	putshort(out, 1); // color planes
	putshort(out, 8); // color depth
	putlong(out, 0); // no compression
	putlong(out, imagesize);
	putlong(out, 2851); // horizontal res - copied value from existing
	putlong(out, 2851); // vertical res
	putlong(out, 256); // palette size
	putlong(out, 256); // number of 'important' colors

	// Color Table
	for (int i = 0; i < 256; ++i)
	{
		out.put(palette[i][2]);
		out.put(palette[i][1]);
		out.put(palette[i][0]);
		out.put(0);
	}

	// Image
	const char zeroes[] = {0, 0, 0, 0};
	SDL_assert(out.tellp() == headersize);
	for (int y = height - 1; y >= 0; --y)
	{
		for (int x = 0; x < width; ++x)
		{
			out.put(pixels[y * width + x]);
		}
		out.write(zeroes, padwidth - width);
	}
	SDL_assert(out.tellp() == headersize + imagesize);
}

SDL_Color GetPixelColor(const SDL_Surface* surface, const int x, const int y)
{
	uint32_t pixel = *(uint32_t*)((uint8_t*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
	SDL_Color color = { 0, 0, 0, 255 };
	SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
	return color;
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	while (true)
	{
		const char* openPath = tinyfd_openFileDialog("LunaticPal: Open Image", "", 0, nullptr, nullptr, false);
		if (!openPath)
			break;

		SDL_Surface* img = IMG_Load(openPath);
		if (!img)
		{
			//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading image file", IMG_GetError(), nullptr);
			tinyfd_messageBox("Error loading image", IMG_GetError(), "ok", "error", 0);
			continue;
		}

		const char* filters[1] = { "*.bmp" };
		const char* savePath = tinyfd_saveFileDialog("LunaticPal: Save BMP As", "", 1, filters, "Windows Bitmap");
		if (!savePath)
			break;

		std::vector<uint8_t> pixels(img->w * img->h);
		for (int y = 0; y < img->h; ++y)
		{
			for (int x = 0; x < img->w; ++x)
			{
				pixels[y * img->w + x] = convert_color(GetPixelColor(img, x, y));
			}
		}

		write_bmp(img->w, img->h, pixels.data(), savePath);
	}

	return 0;
}
