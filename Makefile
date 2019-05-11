PROJECTS := lunatic supreme sleepless
.PHONY: all clean help $(PROJECTS)

all clean help $(PROJECTS): build/Makefile
	@$(MAKE) -s -C build $@

build/Makefile: premake5.lua
	@echo "==== Preparing build ===="
	@rm -f build/Makefile
	@premake5 gmake2
