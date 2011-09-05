# Lunatic Makefile
# Tad Hardesty, aka SpaceManiac, 2011

# Configuration
DIRECTX=D:/libraries/directx-sdk
GLOBAL_OPTIONS=-std=c++0x
LIBS=-lwinmm -ld3d9 -ld3dx9 -lgdi32 -ldsound
COMPILER_OPTIONS=-Iinclude/ -I${DIRECTX}/Include/ -Wall ${GLOBAL_OPTIONS} -masm=intel
LINKER_OPTIONS=-L${DIRECTX}/Lib/x86/ ${LIBS} ${GLOBAL_OPTIONS}

OPTIONS_RELEASE=${COMPILER_OPTIONS} -DNDEBUG -DEXPANDO -O2 -s
OPTIONS_DEBUG=${COMPILER_OPTIONS} -g -D_DEBUG -DLOG -DEXPANDO
OPTIONS_DEMO=${COMPILER_OPTIONS} ${OPTIONS_RELEASE} -DDEMO

GENERATE_COMMAND=@python tools/make.py
MAKE_COMMAND=@make -f build/generated.make

# Misc targets
build: release debug demo

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

# Run targets
run: release
	@cd game/
	@lunatic.exe

run-debug: debug
	@cd game/
	@lunatic_debug.exe

run-demo: demo
	@cd game/
	@lunatic_demo.exe
