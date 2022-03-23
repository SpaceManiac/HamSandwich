# Flags that apply to our code only, not external code.
if(MSVC)
	add_compile_options(
		-D_CRT_SECURE_NO_WARNINGS
		-DNOMINMAX
	)
elseif(EMSCRIPTEN)
	# Emscripten essentials
	add_link_options(
		-sALLOW_MEMORY_GROWTH=1
		--use-preload-cache
		-sENVIRONMENT=web
		-sFORCE_FILESYSTEM=1
		-sEXTRA_EXPORTED_RUNTIME_METHODS=['ENV']
	)
	# Coroutine support
	add_compile_options(
		-DUSE_COROUTINES
		-fcoroutines-ts
		-Werror=unused-result
	)
elseif(ANDROID)
	add_compile_options(
		-fsigned-char
		-fexceptions
	)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	link_libraries(ws2_32 winmm)
endif()

# Libraries
add_subdirectory("vanilla_extract")
add_subdirectory("ham")

# For games only, use -no-pie so that the executables are double-clickable in Thunar.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_link_options(
		-no-pie
	)
endif()

# Games
# Use EXCLUDE_FROM_ALL to exclude a game from the launcher and distribution.
# Comment out an entry to exclude it from the build entirely.
add_subdirectory("lunatic" EXCLUDE_FROM_ALL)
add_subdirectory("supreme")
add_subdirectory("sleepless")
add_subdirectory("loonyland")
add_subdirectory("loonyland2")
add_subdirectory("mystic")

# Launcher
if(NOT EMSCRIPTEN AND NOT ANDROID)
	add_subdirectory("launcher")
endif()
