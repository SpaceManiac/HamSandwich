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
elseif(MSVC)
	# On MSVC, use the official prebuilt binaries.
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(SDL_PLATFORM "x86")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SDL_PLATFORM "x64")
	endif()

	include(FetchContent)
	FetchContent_Declare(SDL2
		URL https://www.libsdl.org/release/SDL2-devel-2.0.8-VC.zip
		URL_HASH SHA256=68505e1f7c16d8538e116405411205355a029dcf2df738dbbc768b2fe95d20fd
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
	target_link_libraries(SDL2 INTERFACE "${sdl2_LIBS}/SDL2.lib" "${sdl2_LIBS}/SDL2main.lib")
	install(FILES "${sdl2_LIBS}/SDL2.dll" TYPE BIN COMPONENT Executables)

	target_include_directories(SDL2_image INTERFACE "${sdl2_image_SOURCE_DIR}/include")
	set(sdl2_image_LIBS "${sdl2_image_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2_image INTERFACE "${sdl2_image_LIBS}/SDL2_image.lib")
	install(FILES
		"${sdl2_image_LIBS}/SDL2_image.dll"
		"${sdl2_image_LIBS}/libpng16-16.dll"
		"${sdl2_image_LIBS}/zlib1.dll"
		TYPE BIN COMPONENT Executables)

	target_include_directories(SDL2_mixer INTERFACE "${sdl2_mixer_SOURCE_DIR}/include")
	set(sdl2_mixer_LIBS "${sdl2_mixer_SOURCE_DIR}/lib/${SDL_PLATFORM}")
	target_link_libraries(SDL2_mixer INTERFACE "${sdl2_mixer_LIBS}/SDL2_mixer.lib")
	install(FILES
		"${sdl2_mixer_LIBS}/SDL2_mixer.dll"
		"${sdl2_mixer_LIBS}/LICENSE.ogg-vorbis.txt"
		"${sdl2_mixer_LIBS}/libogg-0.dll"
		"${sdl2_mixer_LIBS}/libvorbis-0.dll"
		"${sdl2_mixer_LIBS}/libvorbisfile-3.dll"
		TYPE BIN COMPONENT Executables)
else()
	# On GCC, stick to system libraries.
	find_package(SDL2 REQUIRED)
	find_library(SDL2_IMAGE_LIBRARIES SDL2_image REQUIRED)
	find_library(SDL2_MIXER_LIBRARIES SDL2_mixer REQUIRED)
	target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(SDL2 INTERFACE ${SDL2_LIBRARIES})
	target_link_libraries(SDL2_image INTERFACE ${SDL2_IMAGE_LIBRARIES})
	target_link_libraries(SDL2_mixer INTERFACE ${SDL2_MIXER_LIBRARIES})
endif()
