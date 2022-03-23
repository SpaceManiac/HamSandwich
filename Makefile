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

# Emscripten has a *directory* named "cmake" as a child of a directory in PATH,
# and GNU make attempts to execute this directory, which is ridiculous.
CMAKE ?= $(shell which cmake)
ifeq ($(CMAKE),)
$(error 'cmake' is missing from PATH; install it or set CMAKE= manually.)
endif

BUILD_ROOT = build
BUILD_DIR = $(BUILD_ROOT)/cmake-$(preset)
BUILD_NINJA = $(BUILD_DIR)/build.ninja
CMAKE_ARGS = --preset $(preset) -B $(BUILD_DIR)

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
