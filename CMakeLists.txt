cmake_minimum_required(VERSION 2.8)
project(Supreme)

# platform settings
if(MINGW)
	add_definitions(-DALLEGRO_MINGW32)
	# static-link libgcc and libstdc++
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
	# don't include compile flags as arguments to windres
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <DEFINES> -O coff -i <SOURCE> -o <OBJECT>")
endif()

# includes
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

# extra options
add_definitions(-std=gnu++11 -Wall -DEXPANDO)
add_definitions(-Wno-write-strings
				-Wno-char-subscripts
				-Wno-unused-variable
				-Wno-unused-but-set-variable)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_definitions(-D_DEBUG -DLOG -g)
else()
	add_definitions(-mwindows)
endif()
# for scanninglunatic, add -DLEVELLIST

# sources
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/lunatic.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")
list(REMOVE_ITEM SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/source/monsterlist.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/monsterai1.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/monsterai2.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/monsterai3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/monsterai4.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/textitems.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/textrooms.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/options.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/spcldialog.cpp")

# executable
add_executable(Supreme ${SOURCES})
target_link_libraries(Supreme alleg44)
if (WIN32)
	target_link_libraries(Supreme ws2_32 winmm)
endif()
target_link_libraries(Supreme logg vorbisfile vorbis ogg vorbisenc)
