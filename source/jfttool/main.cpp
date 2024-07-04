// Quick-and-dirty command-line .jft tool.
// Usage:
//		./run jfttool path/to/font.jft  - Extract .txt and .pngs from font
//		./run jfttool path/to/font.txt  - Collect font from .txt and .pngs
#define __STDC_FORMAT_MACROS
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include <stdint.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <inttypes.h>
#include "lunaticpal.h"

using namespace std;

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

struct Header {
	uint8_t numChars; // # of characters in the font
	uint8_t firstChar; // the first character's ASCII value (they ascend from there)
	uint8_t height; // height in pixels of the font
	uint8_t spaceSize; // # of pixels wide to make spaces
	uint8_t gapSize; // # of pixels between adjacent letters
	uint8_t gapHeight; // # of pixels to descend for a carriage return
	uint32_t dataSize; // the size in bytes of the data of the characters themselves
	uint32_t data; // pointer to the character data
	uint32_t chars[128]; // pointers to each character's data (can't have more than FONT_MAX_CHARS)
};

enum ConversionError {
	None,
	CouldNotReadHeader,
	BadFile,
	Count
};

int usage(const char* argv0) {
	fprintf(stderr, "Usage: one of:\n");
	fprintf(stderr, "    %s <fontname.jft> [--out outFolderName] - Extract .txt and .pngs from font\n", argv0);
	fprintf(stderr, "    %s <fontname.txt> [--out jftFileName] - Collect font from .txt and .pngs\n", argv0);
	return 1;
}

static void assertFolderExists(filesystem::path folderPath) {
	if(folderPath.empty()) { return; }

	filesystem::directory_entry entry{folderPath};
	if(!entry.exists()) {
		filesystem::create_directory(folderPath);
	}
}


ConversionError convertFontToPNG(string fname, char* outFolder) {
	// READING THE FONT
	ifstream in { fname, ios::in | ios::binary };
	Header header;
	if (!read(in, &header)) {
		fprintf(stderr, "couldn't read file header\n");
		return ConversionError::CouldNotReadHeader;
	}

	filesystem::path filePath = fname;

	filesystem::path outFolderPath = "";
	if(outFolder != nullptr) {
		outFolderPath = outFolder;
		filePath = outFolderPath / filePath.filename();
	}
	filePath.replace_extension(".txt");

	ofstream txt { filePath, ios::out };
	txt << "numChars " << (int)header.numChars << endl;
	txt << "firstChar " << (int)header.firstChar << endl;
	txt << "height " << (int)header.height << endl;
	txt << "spaceSize " << (int)header.spaceSize << endl;
	txt << "gapSize " << (int)header.gapSize << endl;
	txt << "gapHeight " << (int)header.gapHeight << endl;
	printf("saved %s\n", filePath.string().c_str());

	size_t ofs = in.tellg();
	for (int i = 0; i < header.numChars; ++i) {
		uint8_t width = in.get();

		SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, width, header.height, 32, SDL_PIXELFORMAT_ABGR8888);
		SDL_LockSurface(surf);

		SDL_Color *pixels = (SDL_Color*) surf->pixels;
		int pitch = surf->pitch / sizeof(SDL_Color);

		for (int y = 0; y < (int)header.height; ++y) {
			for (int x = 0; x < width; ++x) {
				pixels[y * pitch + x] = lunaticpal::GetColor(in.get());
			}
		}
		SDL_UnlockSurface(surf);

		string outFullFileName = "";
		char outfname[64];
		snprintf(outfname, sizeof(outfname), "U+%X.png", i + (int)header.firstChar);
		string outfnameStr = outfname;

		if(!outFolderPath.empty()) {
			outFullFileName = (outFolderPath / outfnameStr).generic_string();
		} else {
			outFullFileName = (filePath.parent_path() / outfnameStr).generic_string();
		}

		printf("writing: %s\n", outFullFileName.c_str());

		if (IMG_SavePNG(surf, outFullFileName.c_str()) != 0) {
			fprintf(stderr, "error: bad save %s: %s\n", outFullFileName.c_str(), IMG_GetError());
		}

		SDL_FreeSurface(surf);
	}
	in.get();  // apparent padding
	size_t diff = (size_t)in.tellg() - ofs;
	if (diff != header.dataSize) {
		fprintf(stderr, "warning: header said dataSize=%" PRIu32 ", but we read %" PRIu32 "\n", header.dataSize, (uint32_t)diff);
	}

	printf("saved U+%X.png through U+%X.png\n", (int)header.firstChar, (int)header.firstChar + (int)header.numChars - 1);

	return ConversionError::None;
}




ConversionError convertTextToFont(string fname, char* outFileName) {
	// WRITING THE FONT
	Header header;
	memset(&header, 0, sizeof(Header));


	printf("FILE NAME: %s\n", fname.c_str());
	ifstream txt { fname, ios::in };
	std::string key;
	int value;
	while (txt >> key >> value) {
		if (key == "numChars") {
			header.numChars = value;
		} else if (key == "firstChar") {
			header.firstChar = value;
		} else if (key == "height") {
			header.height = value;
			printf("height: %d\n", value);
		} else if (key == "spaceSize") {
			header.spaceSize = value;
		} else if (key == "gapSize") {
			header.gapSize = value;
		} else if (key == "gapHeight") {
			header.gapHeight = value;
		} else {
			fprintf(stderr, "warning: don't know what '%s' is\n", key.c_str());
		}
	}

	if (header.numChars == 0 || header.firstChar == 0 || header.height == 0) {
		fprintf(stderr, "error: missing numChars, firstChar, or height\n");
		return ConversionError::BadFile;
	}

	std::vector<uint8_t> data;
	for (int i = 0; i < header.numChars; ++i) {
		char infname[64];
		snprintf(infname, sizeof(infname), "U+%X.png", i + (int)header.firstChar);
		string inFileNameStr = infname;

		filesystem::path filePath = fname;
		string fullFileName = (filePath.parent_path() / inFileNameStr).generic_string();

		SDL_Surface* surf = IMG_Load(fullFileName.c_str());
		if (!surf) {
			fprintf(stderr, "error: bad %s: %s\n", infname, IMG_GetError());
			return ConversionError::BadFile;
		}
		if (surf->h != header.height) {
			fprintf(stderr, "error: height=%" PRIu8 ", but %s is %dpx tall\n", header.height, infname, surf->h);
			return ConversionError::BadFile;
		}

		if (surf->format->format != SDL_PIXELFORMAT_ABGR8888) {
			fprintf(stderr, "note: converting format\n");
			SDL_Surface* surf2 = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_ABGR8888, 0);
			SDL_FreeSurface(surf);
			surf = surf2;
		}

		SDL_LockSurface(surf);

		SDL_Color *pixels = (SDL_Color*) surf->pixels;
		int pitch = surf->pitch / sizeof(SDL_Color);

		data.push_back(surf->w);
		for (int y = 0; y < surf->h; ++y) {
			for (int x = 0; x < surf->w; ++x) {
				data.push_back(lunaticpal::GetNearest(pixels[y * pitch + x]));
			}
		}
		SDL_UnlockSurface(surf);
		SDL_FreeSurface(surf);
	}
	printf("read U+%X.png through U+%X.png\n", (int)header.firstChar, (int)header.firstChar + (int)header.numChars - 1);

	header.dataSize = data.size();

	if(outFileName != nullptr) {
		fname = outFileName;
	} else {
		fname.erase(fname.size() - 4);
		fname.append(".jft");
	}

	ofstream out { fname, ios::out | ios::binary };
	write(out, &header);
	out.write((char*) data.data(), data.size());
	out.put(static_cast<char>(0xCD));  // apparent padding

	printf("saved %s\n", fname.c_str());

	return ConversionError::None;
}

int main(int argc, char **argv) {
	SDL_Init(0);

	char* out = nullptr;

	// CASE 2: prgName fileName
	// CASE 4: prgName fileName --out folderName/fileName
	if(argc == 4) {
		if(strncmp(argv[2], "--out", strlen("--out")) != 0) {
			return usage(argv[0]);
		}
		out = argv[3];
	} else if (argc != 2) {
		return usage(argv[0]);
	}

	string fname = argv[1];
	if (fname.size() >= 4 && fname.find(".jft") == fname.size() - 4) {

		if(out) {
			filesystem::path outPath = out;
			assertFolderExists(out);
		}

		return convertFontToPNG(fname, out) != ConversionError::None;
	} else if (fname.size() && fname.find(".txt") == fname.size() - 4) {

		if(out) {
			filesystem::path outPath = out;
			if(outPath.extension() != ".jft") {
				fprintf(stderr, "ERROR: out path must have .jft extension\n");
				return usage(argv[0]);
			}
			auto root = outPath.parent_path();
			assertFolderExists(root);
		}

		return convertTextToFont(fname, out) != ConversionError::None;
	} else {
		return usage(argv[0]);
	}

	return 0;
}
