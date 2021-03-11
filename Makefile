# HamSandwich root makefile

PROJECTS := lunatic supreme sleepless loonyland loonyland2 mystic
PROJECTS += ham vanilla_extract
toolset ?= gcc

all:  # Default target

# -----------------------------------------------------------------------------
# Ensure access to a premake5 binary, since we need it for much of the below.
PREMAKE5 := premake5

# Use build/premake5 if it exists, OR if there is no premake5 on PATH
ifeq ($(wildcard build/premake5),build/premake5)
PREMAKE5 := build/premake5
MAKEFILE_DEPS += $(PREMAKE5)
else ifeq ($(shell which premake5),)
PREMAKE5 := build/premake5
MAKEFILE_DEPS += $(PREMAKE5)
endif

build/premake5:
	@./tools/build/install-deps.sh

# -----------------------------------------------------------------------------
# Have premake5 recreate build/$(toolset)/Makefile when needed.
CHILD_MAKEFILE := build/$(toolset)/Makefile

# Recreate the child makefile if any of the premake Lua changes.
MAKEFILE_DEPS += premake5.lua $(wildcard tools/build/*.lua)

$(CHILD_MAKEFILE): $(MAKEFILE_DEPS)
	@echo "==== Preparing $(toolset) build ===="
	@rm -f $@
	@$(PREMAKE5) gmake2 --cc=$(toolset)
	@output="$$($(PREMAKE5) gmake2_deps --cc=$(toolset))" || printf "%s\n" "$$output"

# Recreate the child makefile if any source directory mtimes change.
$(CHILD_MAKEFILE).d: $(MAKEFILE_DEPS)
	@$(PREMAKE5) gmake2_deps --cc=$(toolset) >/dev/null
-include $(CHILD_MAKEFILE).d

# -----------------------------------------------------------------------------
# Forward the phony targets through to the child makefile.
.PHONY: all clean help $(PROJECTS)
all clean help $(PROJECTS): $(CHILD_MAKEFILE)
	@$(MAKE) --no-print-directory -C build/$(toolset) $@

# -----------------------------------------------------------------------------
# ./run helper
build/$(toolset)/run-config.sh: $(MAKEFILE_DEPS)
	@echo "==== Loading $(toolset) configuration ===="
	@$(PREMAKE5) run-config --cc=$(toolset) >/dev/null

# -----------------------------------------------------------------------------
# ./android helper
build/android/build.gradle: $(MAKEFILE_DEPS) $(addprefix source/,$(PROJECTS))
	@echo "==== Preparing Android build ===="
	@rm -f $@
	@$(PREMAKE5) android-studio
