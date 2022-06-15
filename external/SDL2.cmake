# SDL2, SDL2_image, and SDL2_mixer library targets
add_library(SDL2 INTERFACE)
add_library(SDL2_image INTERFACE)
add_library(SDL2_mixer INTERFACE)

if(EMSCRIPTEN)
	target_compile_options(SDL2 INTERFACE -sUSE_SDL=2)
	target_compile_options(SDL2_image INTERFACE -sUSE_SDL_IMAGE=2)
	target_compile_options(SDL2_mixer INTERFACE -sUSE_SDL_MIXER=2 -sSDL2_IMAGE_FORMATS=['bmp'])
	target_link_options(SDL2 INTERFACE -sUSE_SDL=2)
	target_link_options(SDL2_image INTERFACE -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=['bmp'])
	target_link_options(SDL2_mixer INTERFACE -sUSE_SDL_MIXER=2)
elseif(ANDROID)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(ANDROID_BUILD_TYPE "debug")
	else()
		set(ANDROID_BUILD_TYPE "release")
	endif()
	set(ANDROID_OBJDIR "${CMAKE_CURRENT_SOURCE_DIR}/../build/android/SDL2/intermediates/ndkBuild/${ANDROID_BUILD_TYPE}/obj/local/${ANDROID_ABI}")

	target_include_directories(SDL2 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2/include")
	target_include_directories(SDL2_image INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image")
	target_include_directories(SDL2_mixer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_mixer")
	target_link_libraries(SDL2 INTERFACE "${ANDROID_OBJDIR}/libSDL2.so")
	target_link_libraries(SDL2_image INTERFACE "${ANDROID_OBJDIR}/libSDL2_image.so")
	target_link_libraries(SDL2_mixer INTERFACE "${ANDROID_OBJDIR}/libSDL2_mixer.so")
elseif(WIN32)
	# On MSVC, use the official prebuilt binaries.
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(SDL_PLATFORM "x86")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SDL_PLATFORM "x64")
	endif()

	include(FetchContent)
	# SDL 2.0.8 is the real pinned version, but SDL 2.0.9 fixes a Windows-only bug:
	# https://github.com/libsdl-org/SDL/commit/c04dca0dad2696d7dcd51427c8183b19ebff8a60
	FetchContent_Declare(SDL2
		URL https://www.libsdl.org/release/SDL2-devel-2.0.9-VC.zip
		URL_HASH SHA256=ea266ef613f88433f493498f9e72e6bed5d03e4f3fde5b571a557a754ade9065
	)
	FetchContent_Declare(SDL2_image
		URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.4-VC.zip
		URL_HASH SHA256=4e15fad9de43d738b476e422eef1910432443cead60d2084b3ef01d3f4a76087
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
	# Like Windows, use the official SDL2 prebuild binaries.
	include(FetchContent)
	# SDL 2.0.8 is the real pinned version, but SDL 2.0.9 fixes a Windows-only bug:
	# https://github.com/libsdl-org/SDL/commit/c04dca0dad2696d7dcd51427c8183b19ebff8a60
	FetchContent_Declare(SDL2
		URL https://www.libsdl.org/release/SDL2-2.0.8.dmg
		URL_HASH SHA256=74dd2cb6b18e35e8181523590115f10f1da774939c21ce27768a2a80ba57ad5f
		DOWNLOAD_NO_EXTRACT TRUE
		PATCH_COMMAND hdiutil mount <DOWNLOADED_FILE> -mountpoint dmg_mount
		COMMAND cp -r dmg_mount dmg_content
		COMMAND hdiutil unmount dmg_mount
	)
	FetchContent_Declare(SDL2_image
		URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.4.dmg
		URL_HASH SHA256=7efbf1733ba02d5ce278ae27b0e35baeb6d0eaf0e5f56f187e608681aec98b39
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
	# Use system libraries.
	find_package(SDL2 REQUIRED)
	find_library(SDL2_image_LIBRARIES SDL2_image REQUIRED)
	find_library(SDL2_mixer_LIBRARIES SDL2_mixer REQUIRED)
	target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(SDL2 INTERFACE ${SDL2_LIBRARIES})
	target_link_libraries(SDL2_image INTERFACE ${SDL2_image_LIBRARIES})
	target_link_libraries(SDL2_mixer INTERFACE ${SDL2_mixer_LIBRARIES})
	target_compile_options(SDL2 INTERFACE ${SDL2_CFLAGS_OTHER})

	# Bundle Steam runtime libraries.
	include(FetchContent)
	FetchContent_Declare(steam_sdl2
		URL https://repo.steampowered.com/steamrt/pool/main/libs/libsdl2/libsdl2-2.0-0_2.0.20+dfsg-1~steamrt1.1+srt1_amd64.deb
		URL_HASH SHA256=152c8f4d1fc364a5eaea83a243f44d43568e8fd69dd4e45e93ecdd4eb685e9fb
		PATCH_COMMAND "${CMAKE_COMMAND}" -E tar xf data.tar.xz
	)
	FetchContent_Declare(steam_sdl2_image
		URL https://repo.steampowered.com/steamrt/pool/main/libs/libsdl2-image/libsdl2-image-2.0-0_2.0.5+dfsg1-3~steamrt1.2+srt1_amd64.deb
		URL_HASH SHA256=b144c57f8cc3b0ddf9ffcfbab138a3c6477b72d23a52d4176390aa2e8a1423ec
		PATCH_COMMAND "${CMAKE_COMMAND}" -E tar xf data.tar.xz
	)
	FetchContent_Declare(steam_sdl2_mixer
		URL https://repo.steampowered.com/steamrt/pool/main/libs/libsdl2-mixer/libsdl2-mixer-2.0-0_2.0.4.~reimport-0+steamrt1.2+srt1_amd64.deb
		URL_HASH SHA256=78860d794928a0f7a31cd4ee83f395151dedb74f0e2613c6833438aa0d3f1d54
		PATCH_COMMAND "${CMAKE_COMMAND}" -E tar xf data.tar.xz
	)
	FetchContent_Declare(steam_libpng
		URL https://repo.steampowered.com/steamrt/pool/main/libp/libpng/libpng12-0_1.2.46-3ubuntu4.3+steamrt1.1+srt1_amd64.deb
		URL_HASH SHA256=07f3e5c0873fddccdfff6cc83d9652c69685260ea7b07fc30b17bc8bb583d625
		PATCH_COMMAND "${CMAKE_COMMAND}" -E tar xf data.tar.xz
	)
	FetchContent_MakeAvailable(steam_sdl2 steam_sdl2_image steam_sdl2_mixer steam_libpng)

	install(FILES "${steam_sdl2_SOURCE_DIR}/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0.18.2" RENAME "libSDL2-2.0.so.0" TYPE BIN COMPONENT generic/executables)
	# Patch the rpath for the SDL library so that calls to `SDL_LoadObject` can find `libpng12.so.0`.
	install(CODE [[
		execute_process(
			COMMAND patchelf --set-rpath \$ORIGIN "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./libSDL2-2.0.so.0"
			COMMAND_ERROR_IS_FATAL ANY
		)
	]] COMPONENT generic/executables)
	install(FILES "${steam_sdl2_image_SOURCE_DIR}/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0.2.3" RENAME "libSDL2_image-2.0.so.0" TYPE BIN COMPONENT generic/executables)
	install(FILES "${steam_sdl2_mixer_SOURCE_DIR}/usr/lib/x86_64-linux-gnu/libSDL2_mixer-2.0.so.0.2.2" RENAME "libSDL2_mixer-2.0.so.0" TYPE BIN COMPONENT generic/executables)
	install(FILES "${steam_libpng_SOURCE_DIR}/lib/x86_64-linux-gnu/libpng12.so.0.46.0" RENAME "libpng12.so.0" TYPE BIN COMPONENT generic/executables)
endif()
