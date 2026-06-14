# Compile SDL3 with options to include only the file formats we care about.
# Otherwise the versions that come with the Steam runtime will take a hard
# dependency on a variety of file formats we don't want to have to ship the
# .so files for in Itch versions.

# -----------------------------------------------------------------------------
# SDL3

# PreseedMSVCCache.cmake presupposes that MSVC 19+ has these, but our
# cross-compilation environment currently doesn't, so override it.
set(HAVE_ROAPI_H OFF)
set(HAVE_WINDOWS_GAMING_INPUT_H OFF)

set(SDL_TEST_LIBRARY OFF)
add_subdirectory("SDL3")

# -----------------------------------------------------------------------------
# SDL3_image

set(SDLIMAGE_VENDORED ON)

# Images: BMP, PNG, ICO
set(SDLIMAGE_BMP ON)
set(SDLIMAGE_PNG ON)
set(SDLIMAGE_BACKEND_STB ON)
# Everything else off
set(SDLIMAGE_ANI OFF)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JPG OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG OFF)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)
set(SDLIMAGE_PNG_LIBPNG OFF)

add_subdirectory("SDL3_image")
if(WIN32)
	# Work around error in `SDL3_image/src/stb_image.h`:
	# > always_inline function '_mm_load_si128' requires target feature 'sse2',
	# > but would be inlined into function 'stbi__idct_simd' that is compiled
	# > without support for 'sse2'
	target_compile_definitions(SDL3_image-shared PRIVATE STBI_NO_SIMD)
endif()

# -----------------------------------------------------------------------------
# SDL3_mixer

set(SDLMIXER_VENDORED ON)

# Formats: WAV, Ogg, MP3, MIDI, Sun/NeXT (au)
set(SDLMIXER_WAVE ON)
set(SDLMIXER_AU ON) # Sun/NeXT
set(SDLMIXER_MP3 ON)
set(SDLMIXER_MP3_DRMP3 ON)
set(SDLMIXER_MIDI ON)
set(SDLMIXER_MIDI_TIMIDITY ON)
set(SDLMIXER_VORBIS_STB ON) # Ogg
# Everything else off
set(SDLMIXER_AIFF OFF)
set(SDLMIXER_VOC OFF)
set(SDLMIXER_FLAC OFF)
set(SDLMIXER_GME OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_MIDI_FLUIDSYNTH OFF)
set(SDLMIXER_MP3_MPG123 OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
set(SDLMIXER_VORBIS_TREMOR OFF)
set(SDLMIXER_WAVPACK OFF)

add_subdirectory("SDL3_mixer")

# -----------------------------------------------------------------------------
# Shared library installation
if(TARGET SDL3-shared)
	# https://gitlab.kitware.com/cmake/cmake/-/issues/20979
	install(
		TARGETS
			SDL3-shared
			SDL3_image-shared
			SDL3_mixer-shared
		ARCHIVE EXCLUDE_FROM_ALL
		LIBRARY COMPONENT generic/executables
		RUNTIME COMPONENT generic/executables
	)
endif()

if(BUILD_SHARED_LIBS)
	add_library(SDL3 ALIAS SDL3-shared)
	add_library(SDL3_image ALIAS SDL3_image-shared)
	add_library(SDL3_mixer ALIAS SDL3_mixer-shared)
else()
	add_library(SDL3 ALIAS SDL3-static)
	add_library(SDL3_image ALIAS SDL3_image-static)
	add_library(SDL3_mixer ALIAS SDL3_mixer-static)
endif()
