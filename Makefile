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
MAKEFILE_DEPS += build/.toolset
endif

# Recreate build/Makefile if any of the premake Lua changes.
MAKEFILE_DEPS += premake5.lua $(wildcard tools/build/*.lua)

.PHONY: all clean help build/.toolset $(PROJECTS)

all clean help $(PROJECTS): build/Makefile
	@$(MAKE) --no-print-directory -C build $@

build/Makefile: $(MAKEFILE_DEPS) $(addprefix source/,$(PROJECTS))
	@echo "==== Preparing $(toolset) build ===="
	@rm -f build/Makefile
	@$(PREMAKE5) gmake2 --cc=$(toolset)

build/.toolset:
	@mkdir -p build
	@echo $(toolset) >$@

build/premake5:
	@./tools/build/install-deps.sh
