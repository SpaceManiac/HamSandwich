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
		URL https://www.libsdl.org/release/SDL2-devel-2.30.9-VC.zip
		URL_HASH SHA256=8c91d91e5bcb997d062ec2b553c53832ebf95654d4aa35e8c02a954d4ce752ae
	)
	FetchContent_Declare(SDL2_image
		URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.8.2-VC.zip
		URL_HASH SHA256=d8d593c1f3b2ffc8968b7ae0ba8d0ecf7e44b7476c4c5533a8d523f7039acdd4
	)
	FetchContent_Declare(SDL2_mixer
		URL https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.8.0-VC.zip
		URL_HASH SHA256=47a5713937f8d8b903a9c5555fef3ec73a793ef95abdd078773fc11fcb00ec8a
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
		TYPE BIN COMPONENT generic/executables)

	target_include_directories(SDL2_mixer INTERFACE "${sdl2_mixer_SOURCE_DIR}/include")
	set(sdl2_mixer_LIBS "${sdl2_mixer_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2_mixer INTERFACE "${sdl2_mixer_LIBS}/SDL2_mixer.lib")
	install(FILES
		"${sdl2_mixer_LIBS}/SDL2_mixer.dll"
		TYPE BIN COMPONENT generic/executables)
elseif(APPLE)
	# Like Windows, use the official SDL2 prebuilt binaries.
	add_library(SDL2 INTERFACE)
	add_library(SDL2_image INTERFACE)
	add_library(SDL2_mixer INTERFACE)

	include(FetchContent)
	FetchContent_Declare(SDL2
		URL https://www.libsdl.org/release/SDL2-2.30.9.dmg
		URL_HASH SHA256=e8f69d97dcab8faf41654d915ee1451d38a155e31c20945e974643d8d776ca9b
		DOWNLOAD_NO_EXTRACT TRUE
		PATCH_COMMAND hdiutil mount <DOWNLOADED_FILE> -mountpoint dmg_mount
		COMMAND cp -r dmg_mount dmg_content
		COMMAND hdiutil unmount dmg_mount
	)
	FetchContent_Declare(SDL2_image
		URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.8.2.dmg
		URL_HASH SHA256=593f560ba4072144b52845abfe3b9dc267853539fd171b31a26d941faa2cd2b5
		DOWNLOAD_NO_EXTRACT TRUE
		PATCH_COMMAND hdiutil mount <DOWNLOADED_FILE> -mountpoint dmg_mount
		COMMAND cp -r dmg_mount dmg_content
		COMMAND hdiutil unmount dmg_mount
	)
	FetchContent_Declare(SDL2_mixer
		URL https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.8.0.dmg
		URL_HASH SHA256=aea973d78f2949b0b2404379dfe775ac367c69485c1d25a5c59f109797f18adf
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
	if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
		target_compile_options(SDL2 PRIVATE -Wno-unused-but-set-variable)
	endif()

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
