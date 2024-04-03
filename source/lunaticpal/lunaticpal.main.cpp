// LunaticPal v2, 2022
// Based on LunaticPal v1 released on Wombat in 2013.
// Converts any image SDL_image can read into a .bmp in the Dr. L palette.

#include <vector>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <tinyfiledialogs.h>
#include "lunaticpal.h"

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
		SDL_Color color = lunaticpal::GetColor(i);
		out.put(color.b);
		out.put(color.g);
		out.put(color.r);
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
				pixels[y * img->w + x] = lunaticpal::GetNearest(GetPixelColor(img, x, y));
			}
		}

		write_bmp(img->w, img->h, pixels.data(), savePath);
	}

	return 0;
}
