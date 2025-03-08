# SDL2, SDL2_image, and SDL2_mixer library targets
if(EMSCRIPTEN)
	add_library(SDL2 INTERFACE)
	add_library(SDL2_image INTERFACE)
	add_library(SDL2_mixer INTERFACE)

	target_compile_options(SDL2 INTERFACE -sUSE_SDL=2)
	target_compile_options(SDL2_image INTERFACE -sUSE_SDL_IMAGE=2)
	target_compile_options(SDL2_mixer INTERFACE -sUSE_SDL_MIXER=2 -sSDL2_IMAGE_FORMATS=['bmp'])
	target_link_options(SDL2 INTERFACE -sUSE_SDL=2)
	target_link_options(SDL2_image INTERFACE -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=['bmp'])
	target_link_options(SDL2_mixer INTERFACE -sUSE_SDL_MIXER=2)
elseif(ANDROID)
	add_library(SDL2 INTERFACE)
	add_library(SDL2_image INTERFACE)
	add_library(SDL2_mixer INTERFACE)

	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(ANDROID_BUILD_TYPE "debug")
	else()
		set(ANDROID_BUILD_TYPE "release")
	endif()
	set(ANDROID_OBJDIR "${CMAKE_CURRENT_SOURCE_DIR}/../build/android/SDL2/intermediates/ndkBuild/${ANDROID_BUILD_TYPE}/obj/local/${ANDROID_ABI}")

	target_include_directories(SDL2 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2/include")
	target_link_libraries(SDL2 INTERFACE "${ANDROID_OBJDIR}/libSDL2.so")

	target_include_directories(SDL2_image INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image/include")
	target_link_libraries(SDL2_image INTERFACE "${ANDROID_OBJDIR}/libSDL2_image.so")

	target_include_directories(SDL2_mixer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_mixer/include")
	target_link_libraries(SDL2_mixer INTERFACE "${ANDROID_OBJDIR}/libSDL2_mixer.so")
elseif(WIN32)
	# On MSVC, use the official prebuilt binaries.
	add_library(SDL2 INTERFACE)
	add_library(SDL2_image INTERFACE)
	add_library(SDL2_mixer INTERFACE)

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(SDL_PLATFORM "x86")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SDL_PLATFORM "x64")
	endif()

	include(FetchContent)
	FetchContent_Declare(SDL2
		URL https://www.libsdl.org/release/SDL2-devel-2.0.22-VC.zip
		URL_HASH SHA256=32adc96d8b25e5671189f1f38a4fc7deb105fbb1b3ed78ffcb23f5b8f36b3922
	)
	FetchContent_Declare(SDL2_image
		URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip
		URL_HASH SHA256=a180f9b75c4d3fbafe02af42c42463cc7bc488e763cfd1ec2ffb75678b4387ac
	)
	FetchContent_Declare(SDL2_mixer
		URL https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-VC.zip
		URL_HASH SHA256=258788438b7e0c8abb386de01d1d77efe79287d9967ec92fbb3f89175120f0b0
	)
	FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer)

	target_include_directories(SDL2 INTERFACE "${sdl2_SOURCE_DIR}/include")
	set(sdl2_LIBS "${sdl2_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2 INTERFACE "${sdl2_LIBS}/SDL2main.lib" "${sdl2_LIBS}/SDL2.lib")
	install(FILES "${sdl2_LIBS}/SDL2.dll" TYPE BIN COMPONENT generic/executables)

	target_include_directories(SDL2_image INTERFACE "${sdl2_image_SOURCE_DIR}/include")
	set(sdl2_image_LIBS "${sdl2_image_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2_image INTERFACE "${sdl2_image_LIBS}/SDL2_image.lib")
	install(FILES
		"${sdl2_image_LIBS}/SDL2_image.dll"
		"${sdl2_image_LIBS}/libpng16-16.dll"
		"${sdl2_image_LIBS}/zlib1.dll"
		TYPE BIN COMPONENT generic/executables)

	target_include_directories(SDL2_mixer INTERFACE "${sdl2_mixer_SOURCE_DIR}/include")
	set(sdl2_mixer_LIBS "${sdl2_mixer_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2_mixer INTERFACE "${sdl2_mixer_LIBS}/SDL2_mixer.lib")
	install(FILES
		"${sdl2_mixer_LIBS}/SDL2_mixer.dll"
		"${sdl2_mixer_LIBS}/LICENSE.ogg-vorbis.txt"
		"${sdl2_mixer_LIBS}/libogg-0.dll"
		"${sdl2_mixer_LIBS}/libvorbis-0.dll"
		"${sdl2_mixer_LIBS}/libvorbisfile-3.dll"
		TYPE BIN COMPONENT generic/executables)
elseif(APPLE)
	# Like Windows, use the official SDL2 prebuilt binaries.
	add_library(SDL2 INTERFACE)
	add_library(SDL2_image INTERFACE)
	add_library(SDL2_mixer INTERFACE)

	include(FetchContent)
	FetchContent_Declare(SDL2
		URL https://www.libsdl.org/release/SDL2-2.0.22.dmg
		URL_HASH SHA256=72974672b8359057aa2f6d467c8adae8182a6caedd660e3936e23c3c683c3801
		DOWNLOAD_NO_EXTRACT TRUE
		PATCH_COMMAND hdiutil mount <DOWNLOADED_FILE> -mountpoint dmg_mount
		COMMAND cp -r dmg_mount dmg_content
		COMMAND hdiutil unmount dmg_mount
	)
	FetchContent_Declare(SDL2_image
		URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.dmg
		URL_HASH SHA256=313b2b92544bc05cafeb7c566572d08c961f15f25877608ebf86658bd458d815
		DOWNLOAD_NO_EXTRACT TRUE
		PATCH_COMMAND hdiutil mount <DOWNLOADED_FILE> -mountpoint dmg_mount
		COMMAND cp -r dmg_mount dmg_content
		COMMAND hdiutil unmount dmg_mount
	)
	FetchContent_Declare(SDL2_mixer
		URL https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.dmg
		URL_HASH SHA256=ed9c904121ae763cf7d3c7bf08b95cd0e23d370f112c787a17e871bd2c764fe3
		DOWNLOAD_NO_EXTRACT TRUE
		PATCH_COMMAND hdiutil mount <DOWNLOADED_FILE> -mountpoint dmg_mount
		COMMAND cp -r dmg_mount dmg_content
		COMMAND hdiutil unmount dmg_mount
	)
	FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer)

	target_include_directories(SDL2 INTERFACE "${sdl2_SOURCE_DIR}/dmg_content/SDL2.framework/Headers")
	target_compile_options(SDL2 INTERFACE -F "${sdl2_SOURCE_DIR}/dmg_content")  # so SDL_mixer finds <SDL2/SDL.h>
	target_link_libraries(SDL2 INTERFACE "${sdl2_SOURCE_DIR}/dmg_content/SDL2.framework/SDL2")
	install(DIRECTORY "${sdl2_SOURCE_DIR}/dmg_content/SDL2.framework" TYPE BIN COMPONENT generic/executables)

	target_include_directories(SDL2_image INTERFACE "${sdl2_image_SOURCE_DIR}/dmg_content/SDL2_image.framework/Headers")
	target_link_libraries(SDL2_image INTERFACE "${sdl2_image_SOURCE_DIR}/dmg_content/SDL2_image.framework/SDL2_image")
	install(DIRECTORY "${sdl2_image_SOURCE_DIR}/dmg_content/SDL2_image.framework" TYPE BIN COMPONENT generic/executables)

	target_include_directories(SDL2_mixer INTERFACE "${sdl2_mixer_SOURCE_DIR}/dmg_content/SDL2_mixer.framework/Headers")
	target_link_libraries(SDL2_mixer INTERFACE "${sdl2_mixer_SOURCE_DIR}/dmg_content/SDL2_mixer.framework/SDL2_mixer")
	install(DIRECTORY "${sdl2_mixer_SOURCE_DIR}/dmg_content/SDL2_mixer.framework" TYPE BIN COMPONENT generic/executables)
	install(FILES "${sdl2_mixer_SOURCE_DIR}/dmg_content/SDL2_mixer.framework/Frameworks/Ogg.framework/Resources/LICENSE.ogg-vorbis.txt" TYPE BIN COMPONENT generic/executables)
else()
	# Compile SDL2 with options to include only the file formats we care about.
	# Otherwise the versions that come with the Steam runtime will take a hard
	# dependency on a variety of file formats we don't want to have to ship the
	# .so files for in Itch versions.
	set(SDL_SHARED ON CACHE BOOL "" FORCE)
	set(SDL_STATIC OFF CACHE BOOL "" FORCE)
	set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE) # We'll handle it ourselves.
	add_subdirectory("SDL2")

	# Images: BMP, PNG, ICO
	set(SDL2IMAGE_BMP ON)
	set(SDL2IMAGE_PNG ON)
	set(SDL2IMAGE_BACKEND_STB ON)
	add_subdirectory("SDL2_image")

	# Sound effects: WAV, MP3 (not Mixer: Sun/NeXT)
	# Music: Ogg, MIDI, WAV
	set(SDL2MIXER_DEPS_SHARED ON)
	set(SDL2MIXER_WAVE ON)
	set(SDL2MIXER_VORBIS "STB")
	set(SDL2MIXER_MIDI ON)
	# Prefer Fluidsynth over Timidity for now because it can be passed a sf2 path directly rather than needing a .cfg file.
	# TODO: Both require manual soundfont setup. Maybe bundle FluidR3_GM.sf2?
	set(SDL2MIXER_MIDI_FLUIDSYNTH ON)
	set(SDL2MIXER_MIDI_TIMIDITY OFF)
	set(SDL2MIXER_MP3 OFF)  # TODO: https://github.com/SpaceManiac/HamSandwich/issues/21
	set(SDL2MIXER_WAVPACK OFF)
	set(SDL2MIXER_MOD OFF)
	set(SDL2MIXER_OPUS OFF)
	add_subdirectory("SDL2_mixer")

	# Install relevant .so files
	install(
		TARGETS SDL2 SDL2_image SDL2_mixer
		LIBRARY COMPONENT generic/executables
		RUNTIME COMPONENT generic/executables
	)

	# Install fluidsynth to Supreme only.
	find_library(fluidsynth_LIBRARIES fluidsynth REQUIRED)
	install_as_soname(supreme/executables ${fluidsynth_LIBRARIES})
	install(FILES "/usr/share/doc/libfluidsynth-dev/copyright" RENAME "LICENSE.fluidsynth.txt" TYPE BIN COMPONENT supreme/executables)
endif()
