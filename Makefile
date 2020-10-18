PROJECTS := lunatic supreme sleepless loonyland loonyland2 mystic ham
PREMAKE5 := premake5
toolset ?= gcc

# Use build/premake5 if it exists, OR if there is no premake5 on PATH
ifneq ($(wildcard build/premake5),)
PREMAKE5 := build/premake5
else ifeq (,$(shell which premake5))
PREMAKE5 := build/premake5
MAKEFILE_DEPS += $(PREMAKE5)
endif

# Recreate build/Makefile if $toolset changes.
ifneq "$(toolset)" "$(shell cat build/.toolset 2>/dev/null)"
TOOLSET_DEPS += build/.toolset
endif

# Recreate build/Makefile if any of the premake Lua changes.
MAKEFILE_DEPS += premake5.lua $(wildcard tools/build/*.lua)

.PHONY: all clean help build/.toolset $(PROJECTS)

all clean help $(PROJECTS): build/Makefile
	@$(MAKE) --no-print-directory -C build $@

build/Makefile: $(MAKEFILE_DEPS) $(TOOLSET_DEPS) $(addprefix source/,$(PROJECTS))
	@echo "==== Preparing $(toolset) build ===="
	@rm -f $@
	@$(PREMAKE5) gmake2 --cc=$(toolset)

build/.toolset:
	@mkdir -p build
	@echo $(toolset) >$@

build/premake5:
	@./tools/build/install-deps.sh

# Helper to prepare Android build
build/android/build.gradle: $(MAKEFILE_DEPS)
	@echo "==== Preparing Android build ===="
	@rm -f $@
	@$(PREMAKE5) android-studio
