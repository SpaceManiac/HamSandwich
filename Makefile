PROJECTS := lunatic supreme sleepless loonyland loonyland2 mystic
PREMAKE5 := premake5

ifneq ($(wildcard build/premake5),)
PREMAKE5 := build/premake5
endif

.PHONY: all clean help $(PROJECTS)

all clean help $(PROJECTS): build/Makefile
	@$(MAKE) --no-print-directory -C build $@

build/Makefile: premake5.lua $(addprefix source/,$(PROJECTS))
	@echo "==== Preparing build ===="
	@rm -f build/Makefile
	@$(PREMAKE5) gmake2
