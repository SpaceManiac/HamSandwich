PROJECTS := lunatic supreme sleepless loonyland loonyland2 mystic
PROJECTS += ham libextract
PREMAKE5 := premake5
toolset ?= gcc

# Use build/premake5 if it exists, OR if there is no premake5 on PATH
ifeq ($(wildcard build/premake5),build/premake5)
PREMAKE5 := build/premake5
MAKEFILE_DEPS += $(PREMAKE5)
else ifeq ($(shell which premake5),)
PREMAKE5 := build/premake5
MAKEFILE_DEPS += $(PREMAKE5)
endif

# Recreate build/Makefile if any of the premake Lua changes.
MAKEFILE_DEPS += premake5.lua $(wildcard tools/build/*.lua)

.PHONY: all clean help $(PROJECTS)

all clean help $(PROJECTS): build/$(toolset)/Makefile
	@$(MAKE) --no-print-directory -C build/$(toolset) $@

build/$(toolset)/Makefile: $(MAKEFILE_DEPS) $(TOOLSET_DEPS)
	@echo "==== Preparing $(toolset) build ===="
	@rm -f $@
	@$(PREMAKE5) gmake2 --cc=$(toolset)
	@output="$$($(PREMAKE5) gmake2_deps --cc=$(toolset))" || printf "%s\n" "$$output"

# Recreate build/Makefile if any source directory mtimes change.
build/$(toolset)/Makefile.d: $(MAKEFILE_DEPS)
	@$(PREMAKE5) gmake2_deps --cc=$(toolset) >/dev/null
-include build/$(toolset)/Makefile.d

build/premake5:
	@./tools/build/install-deps.sh

# Helper to prepare Android build
build/android/build.gradle: $(MAKEFILE_DEPS) $(addprefix source/,$(PROJECTS))
	@echo "==== Preparing Android build ===="
	@rm -f $@
	@$(PREMAKE5) android-studio
