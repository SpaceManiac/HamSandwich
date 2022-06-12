# Convenient root makefile. Invokes `cmake` and then `cmake --build` as needed.

# Fallback settings. Keep this in sync with `./run`!
ifndef preset
mode ?= debug
ifndef os
# Detect OS and architecture.
ifeq ($(MSYSTEM),MINGW32)
os := windows
arch := i686
else ifeq ($(MSYSTEM),MINGW64)
os := windows
arch := x86_64
else ifneq ($(MSYSTEM),)
$(error The project cannot be built in '$(MSYSTEM)' mode. Return to the Start Menu and open 'MSYS2 MinGW x86' or 'MSYS2 MinGW x64')
else ifeq ($(shell uname -sm),Linux x86_64)
os := linux
arch := x86_64
else ifeq ($(shell uname -sm),Darwin x86_64)
os := macos
arch := x86_64
else
$(error Unknown system '$(shell uname -sm)'; set preset= manually.)
endif  # uname -sm
else ifndef arch
ifneq (,$(filter $(os),windows emscripten))
arch := i686
else ifneq (,$(filter $(os),linux macos))
arch := x86_64
else
$(error Unknown os '$(os)'; set arch= manually.)
endif  # filter on os
endif  # os, arch
preset := $(os)-$(arch)-$(mode)
endif  # preset

# Activate Emscripten SDK if needed.
$(shell tools/build/install-deps.sh >&2)
ifeq ($(os),emscripten)
$(shell tools/emscripten/install-emsdk.sh >&2)
endif

# Variables
CMAKE ?= tools/bootstrap/cmake
BUILD_DIR = build/cmake-$(preset)
BUILD_NINJA = $(BUILD_DIR)/build.ninja

ifeq ($(MAKECMDGOALS),)
# If no targets were specified, synthesize a default target.
.PHONY: _default
_default: $(BUILD_NINJA)
	@$(CMAKE) --build $(BUILD_DIR)
else
# If targets were specified, pass them through to `cmake --build`.
.PHONY: $(MAKECMDGOALS)
$(word 1,$(MAKECMDGOALS)):: $(BUILD_NINJA)
	@$(CMAKE) --build $(BUILD_DIR) -t $(MAKECMDGOALS)
$(MAKECMDGOALS)::
	@#

endif

# If the CMake cache doesn't already exist, configure it for the first time.
# If it does already exist, `cmake --build` will reconfigure if needed.
$(BUILD_NINJA):
	@$(CMAKE) --preset $(preset) -B $(BUILD_DIR)
