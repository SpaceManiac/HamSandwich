PROJECTS := lunatic supreme sleepless loonyland loonyland2 mystic ham
PREMAKE5 := premake5
toolset ?= gcc

ifneq ($(wildcard build/premake5),)
PREMAKE5 := build/premake5
endif

.PHONY: all clean help build/.toolset $(PROJECTS)

all clean help $(PROJECTS): build/Makefile
	@$(MAKE) --no-print-directory -C build $@

ifneq "$(toolset)" "$(shell cat build/.toolset 2>/dev/null)"
DOTCC += build/.toolset
endif

build/Makefile: premake5.lua $(DOTCC) $(addprefix source/,$(PROJECTS))
	@echo "==== Preparing $(toolset) build ===="
	@rm -f build/Makefile
	@$(PREMAKE5) gmake2 --cc=$(toolset)

build/.toolset:
	@echo $(toolset) >$@
