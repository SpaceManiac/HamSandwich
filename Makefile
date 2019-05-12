PROJECTS := lunatic supreme sleepless
PREMAKE5 := premake5

ifneq ($(wildcard build/premake5),)
PREMAKE5 := build/premake5
endif

.PHONY: all clean help $(PROJECTS)

all clean help $(PROJECTS): build/Makefile
	@$(MAKE) -s -C build $@

build/Makefile: premake5.lua
	@echo "==== Preparing build ===="
	@rm -f build/Makefile
	@$(PREMAKE5) gmake2
