# SDL2, SDL2_image, and SDL2_mixer library targets
add_library(SDL2 INTERFACE)
add_library(SDL2_image INTERFACE)
add_library(SDL2_mixer INTERFACE)
add_library(SDL2_ttf INTERFACE)

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
	target_link_libraries(SDL2 INTERFACE "${ANDROID_OBJDIR}/libSDL2.so")

	target_include_directories(SDL2_image INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image")
	target_link_libraries(SDL2_image INTERFACE "${ANDROID_OBJDIR}/libSDL2_image.so")

	target_include_directories(SDL2_mixer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_mixer")
	target_link_libraries(SDL2_mixer INTERFACE "${ANDROID_OBJDIR}/libSDL2_mixer.so")
elseif(WIN32)
	# On MSVC, use the official prebuilt binaries.
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
	FetchContent_Declare(SDL2_ttf
		URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-VC.zip
		URL_HASH SHA256=aab0d81f1aa6fe654be412efc85829f2b188165dca6c90eb4b12b673f93e054b
	)
	FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer SDL2_ttf)

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

	target_include_directories(SDL2_ttf INTERFACE "${sdl2_ttf_SOURCE_DIR}/include")
	target_link_directories(SDL2_ttf INTERFACE "${sdl2_ttf_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2_ttf INTERFACE "SDL2_ttf.lib")
	install(FILES
		"${sdl2_ttf_SOURCE_DIR}/lib/${SDL_PLATFORM}/SDL2_ttf.dll"
		"${sdl2_ttf_SOURCE_DIR}/lib/${SDL_PLATFORM}/libfreetype-6.dll"
		"${sdl2_ttf_SOURCE_DIR}/lib/${SDL_PLATFORM}/LICENSE.freetype.txt"
		"${sdl2_ttf_SOURCE_DIR}/lib/${SDL_PLATFORM}/zlib1.dll"
		TYPE BIN COMPONENT sdl2_ttf/executables)
elseif(APPLE)
	# Like Windows, use the official SDL2 prebuild binaries.
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
	FetchContent_Declare(SDL2_ttf
		URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.dmg
		URL_HASH SHA256=318bc896ea89e776a6cd07fc71aff11571e05631390fcdd360dc831cc88cf849
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
	find_library(SDL2_ttf_LIBRARIES SDL2_ttf REQUIRED)
	target_link_libraries(SDL2 INTERFACE SDL2::SDL2)
	target_link_libraries(SDL2_image INTERFACE ${SDL2_image_LIBRARIES})
	target_link_libraries(SDL2_mixer INTERFACE ${SDL2_mixer_LIBRARIES})
	target_link_libraries(SDL2_ttf INTERFACE ${SDL2_ttf_LIBRARIES})

	# Patch the rpath for the SDL library so that calls to `SDL_LoadObject` can find `libpng12.so.0`.
	find_library(sdl2_original SDL2)
	set(sdl2_with_rpath "${CMAKE_CURRENT_BINARY_DIR}/libSDL2-2.0.so.0")
	add_custom_command(
		OUTPUT "${sdl2_with_rpath}"
		COMMAND "${CMAKE_COMMAND}" -E copy "${sdl2_original}" "${sdl2_with_rpath}"
		COMMAND patchelf --set-rpath \$ORIGIN "${sdl2_with_rpath}"
		DEPENDS "${sdl2_original}"
		VERBATIM
	)
	set(sdl2_rpath_depends "${sdl2_with_rpath}")
	install(FILES "${sdl2_with_rpath}" TYPE BIN COMPONENT generic/executables)

	# Install SDL2_image .so
	install_as_soname(generic/executables ${SDL2_image_LIBRARIES})
	find_library(png_LIBRARIES png REQUIRED)
	install_as_soname(generic/executables ${png_LIBRARIES})

	# Install SDL2_mixer .so
	install_as_soname(generic/executables ${SDL2_mixer_LIBRARIES})
	find_library(ogg_LIBRARIES ogg REQUIRED)
	find_library(vorbis_LIBRARIES vorbis REQUIRED)
	find_library(vorbisfile_LIBRARIES vorbisfile REQUIRED)
	install_as_soname(generic/executables ${ogg_LIBRARIES} ${vorbis_LIBRARIES} ${vorbisfile_LIBRARIES})
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_mixer/external/libogg-1.3.2/COPYING" TYPE BIN COMPONENT generic/executables RENAME "LICENSE.ogg.txt")
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_mixer/external/libvorbis-1.3.5/COPYING" TYPE BIN COMPONENT generic/executables RENAME "LICENSE.vorbis.txt")

	# SDL2_ttf is only used by JspEdit, not installed
endif()

add_custom_target(sdl2_rpath ALL DEPENDS "${sdl2_rpath_depends}")
