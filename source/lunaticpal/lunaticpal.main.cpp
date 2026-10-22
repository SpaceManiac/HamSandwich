// LunaticPal v2, 2022
// Based on LunaticPal v1 released on Wombat in 2013.
// Converts any image SDL_image can read into a .bmp in the Dr. L palette.

#include <vector>
#include <fstream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "lunaticpal.h"
#include "../vanilla_extract/include/owned_sdl.h"

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

SDL_Color GetPixelColor(SDL_Surface* surface, const int x, const int y)
{
	uint32_t pixel = *(uint32_t*)((uint8_t*)surface->pixels + y * surface->pitch + x * SDL_BYTESPERPIXEL(surface->format));
	SDL_Color color = { 0, 0, 0, 255 };
	SDL_GetRGBA(pixel, SDL_GetPixelFormatDetails(surface->format), SDL_GetSurfacePalette(surface), &color.r, &color.g, &color.b, &color.a);
	return color;
}

void open_file();
void save_file(owned::SDL_Surface img);

void quit()
{
	SDL_Event quit;
	quit.type = SDL_EVENT_QUIT;
	SDL_PushEvent(&quit);
}

void SDLCALL on_open_file(void *, char const* const* filelist, int filter)
{
	if (!filelist)
	{
		// Error.
		fprintf(stderr, "SDL_ShowOpenFileDialog: %s\n", SDL_GetError());
		return quit();
	}
	if (!filelist[0])
	{
		// Cancelled.
		return quit();
	}

	owned::SDL_Surface img { IMG_Load(filelist[0]) };
	if (!img)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading image file", SDL_GetError(), nullptr);
		open_file();
		return;
	}
	save_file(std::move(img));
}

void open_file()
{
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING, "LunaticPal: Open Image");
	SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFILE, on_open_file, nullptr, props);
	SDL_DestroyProperties(props);
}

void SDLCALL on_save_file(void *ud, char const* const* filelist, int filter)
{
	owned::SDL_Surface img { (SDL_Surface*)ud };

	if (!filelist)
	{
		// Error.
		fprintf(stderr, "SDL_ShowSaveFileDialog: %s\n", SDL_GetError());
		return quit();
	}
	if (!filelist[0])
	{
		// Cancelled.
		return quit();
	}

	std::vector<uint8_t> pixels(img->w * img->h);
	for (int y = 0; y < img->h; ++y)
	{
		for (int x = 0; x < img->w; ++x)
		{
			pixels[y * img->w + x] = lunaticpal::GetNearest(GetPixelColor(img.get(), x, y));
		}
	}

	write_bmp(img->w, img->h, pixels.data(), filelist[0]);

	open_file();
}

void save_file(owned::SDL_Surface img)
{
	static const SDL_DialogFileFilter FILTERS[] = {
		{ "Windows Bitmap", "bmp" },
	};

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING, "LunaticPal: Save BMP As");
	SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, (void*)FILTERS);
	SDL_SetNumberProperty(props, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, std::size(FILTERS));
	SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_SAVEFILE, on_save_file, img.release(), props);
	SDL_DestroyProperties(props);
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	if (!SDL_Init(SDL_INIT_EVENTS))
	{
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}

	open_file();
	while (true)
	{
		SDL_Event event;
		if (!SDL_WaitEvent(&event))
		{
			fprintf(stderr, "SDL_WaitEvent: %s\n", SDL_GetError());
			return 1;
		}

		switch (event.type)
		{
			case SDL_EVENT_QUIT:
				return 0;
		}
	}
}
