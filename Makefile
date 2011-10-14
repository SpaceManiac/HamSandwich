# Lunatic Makefile
# Tad Hardesty, aka SpaceManiac, 2011

# Configuration
DIRECTX=D:\\libraries\\directx-sdk
GLOBAL_OPTIONS=
LIBS=-lwinmm -ldsound -lalleg44 -llogg -lvorbisfile -lvorbis -logg -lvorbisenc
COMPILER_OPTIONS=-std=c++0x -Iinclude -Wall -I${DIRECTX}/Include/ -DALLEGRO_MINGW32 -DEXPANDO
LINKER_OPTIONS=-static-libgcc -static-libstdc++ -Linclude -L${DIRECTX}/Lib/x86/ ${LIBS}

OPTIONS_DEBUG=${COMPILER_OPTIONS} -D_DEBUG -DLOG -g
OPTIONS_RELEASE=${COMPILER_OPTIONS} -DNDEBUG -O2 -s -mwindows
OPTIONS_DEMO=${COMPILER_OPTIONS} -DNDEBUG -DDEMO -O2 -s -mwindows

GENERATE_COMMAND=@python tools/make.py
MAKE_COMMAND=@make -f build/generated.make

# Misc targets
build: release debug #demo

clean:
	@rm -rf build/*
	@rm -rf bin/*

# Project targets
release:
	${GENERATE_COMMAND} lunatic "${OPTIONS_RELEASE}" "${LINKER_OPTIONS}"
	${MAKE_COMMAND}
	@cp bin/lunatic.exe game/lunatic.exe

debug:
	${GENERATE_COMMAND} lunatic_debug "${OPTIONS_DEBUG}" "${LINKER_OPTIONS}"
	${MAKE_COMMAND}
	@cp bin/lunatic_debug.exe game/lunatic_debug.exe

demo:
	${GENERATE_COMMAND} lunatic_demo "${OPTIONS_DEMO}" "${LINKER_OPTIONS}"
	${MAKE_COMMAND}
	@cp bin/lunatic_demo.exe game/lunatic_demo.exe

# Run targets
run: release
	@cd game/ && ./lunatic.exe window

run-debug: debug
	@cd game/ && ./lunatic_debug.exe "window"

gdb-debug: debug
	@cd game/ && gdb lunatic_debug.exe

run-demo: demo
	@cd game/ && ./lunatic_demo.exe window
