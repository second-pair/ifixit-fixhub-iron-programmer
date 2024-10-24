#  Author:  Blair Edwards
#  Date Started:  2024-09-13

#  Major props to this lad for giving me pretty much all of the information I needed.
#  https://makefiletutorial.com/

#  First version of my C easy-compilation makefile; which I'm anticipating I will want to have in my back pocket.
#  This will end up happily compiling regular C programmes and those that implement GTK4.
#  Compilations we're targeting:
#   -  x86_64-unknown-linux-gnu -> <PROG_NAME>.linux.bin
#   -  aarch64-unknown-linux-gnu -> <PROG_NAME>.arm-linux.bin
#   -  x86_64-pc-windows-gnu -> <PROG_NAME>.win.exe

#  Project Information
PROG_NAME?=ifixit-fixhub-iron-programmer
FOR_GTK4=1
PATH_OBJ?=./obj
ifeq ($(FOR_GTK4), 1)
	PATH_BIN?=/gtk4-build/bin
else
	PATH_BIN?=./bin
endif

#  Common Variables
MAKEFLAGS=-j$(shell nproc || printf 4)
GIT_HASH=$(shell git log -n 1 --pretty=format:%H)
CMD_GDB=gdb

#  Compiler
FLAGS_DBG=-O0 -g
FLAGS_REL=-O3 -g
CMD_COMP=gcc -Wall -Wconversion
CMD_OBJCOPY=objcopy
CMD_STRIP=strip
SRC_C_EXTRA=
SRC_ARC_EXTRA=
LIBS=-l m
ifeq ($(FOR_GTK4), 1)
	LIBS_LINUX=$(LIBS) `pkg-config --cflags --libs gtk4`
	LIBS_WIN=$(LIBS) `PKG_CONFIG_LIBDIR=$(SYSROOT_WIN)/lib/pkgconfig:$(SYSROOT_WIN)/share/pkgconfig PKG_CONFIG_SYSROOT_DIR=$(SYSROOT_WIN) pkg-config --cflags --libs gtk4`
	LIBS_ARM=$(LIBS) `PKG_CONFIG_LIBDIR=$(SYSROOT_ARM)/usr/lib/pkgconfig:$(SYSROOT_ARM)/usr/share/pkgconfig PKG_CONFIG_SYSROOT_DIR=$(SYSROOT_ARM) pkg-config --cflags --libs gtk4`
else
	LIBS_LINUX=$(LIBS)
	LIBS_WIN=$(LIBS)
	LIBS_ARM=$(LIBS)
endif

#  Platform-Specific Variables
TAG_REL=rel
TAG_DBG=dbg
TAG_LINUX=linux
TAG_WIN=win
TAG_ARM=arm-linux
EXT_PROG_LINUX=bin
EXT_PROG_WIN=exe
EXT_PROG_ARM=bin
PFX_LINUX=x86_64-linux-gnu
PFX_WIN=x86_64-w64-mingw32
PFX_ARM=aarch64-linux-gnu
SYSROOT_LINUX=/
SYSROOT_WIN=/mingw64
SYSROOT_ARM=/aarch64
LINK_FLAGS_LINUX=-Xlinker --sysroot=/
LINK_FLAGS_WIN=-Xlinker --sysroot=/mingw64
LINK_FLAGS_ARM=-Xlinker --sysroot=/aarch64


#  High-Level Targets
all:  linux win arm-linux
linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).$(EXT_PROG_LINUX)
win:  $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_WIN).$(EXT_PROG_WIN)
arm-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_ARM).$(EXT_PROG_ARM)
dbg-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_LINUX).$(EXT_PROG_LINUX)
dbg-win:  $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_WIN).$(EXT_PROG_WIN)
dbg-arm-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_ARM).$(EXT_PROG_ARM)

#  Runners
run-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).$(EXT_PROG_LINUX)
	cd $(PATH_BIN); ./$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).$(EXT_PROG_LINUX)
run-win:  $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_WIN).$(EXT_PROG_WIN)
	cd $(PATH_BIN); ./$(PROG_NAME).$(TAG_REL).$(TAG_WIN).$(EXT_PROG_WIN)
run-arm-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_ARM).$(EXT_PROG_ARM)
	cd $(PATH_BIN); ./$(PROG_NAME).$(TAG_REL).$(TAG_ARM).$(EXT_PROG_ARM)
run-dbg-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_LINUX).$(EXT_PROG_LINUX)
	cd $(PATH_BIN); $(CMD_GDB) ./$(PROG_NAME).$(TAG_DBG).$(TAG_LINUX).$(EXT_PROG_LINUX)
run-dbg-win:  $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_WIN).$(EXT_PROG_WIN)
	cd $(PATH_BIN); $(CMD_GDB) ./$(PROG_NAME).$(TAG_DBG).$(TAG_WIN).$(EXT_PROG_WIN)
run-dbg-arm-linux:  $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_ARM).$(EXT_PROG_ARM)
	cd $(PATH_BIN); $(CMD_GDB) ./$(PROG_NAME).$(TAG_DBG).$(TAG_ARM).$(EXT_PROG_ARM)

#  Git Management
init:
	git submodule init
	git submodule update --remote --recursive
update:
	git submodule sync --recursive
	git submodule update --remote --recursive
	git fetch

#  Cleanup
clean:
	rm -f $(PATH_OBJ)/*.$(TAG_REL).*.o $(PATH_OBJ)/*.$(TAG_DBG).*.o \
	$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).* $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).*
clean-all:  clean
	rm -f libserialport.$(TAG_LINUX).a libserialport.$(TAG_WIN).a libserialport.$(TAG_ARM).a


#  Serial
serial:  libserialport.$(TAG_LINUX).a libserialport.$(TAG_WIN).a libserialport.$(TAG_ARM).a
libserialport.$(TAG_LINUX).a:
	(cd libserialport && \
	git reset --hard && \
	git clean -fx && \
	rm -rf .libs && \
	./autogen.sh && \
	./configure && \
	make && \
	cp .libs/libserialport.a ../libserialport.$(TAG_LINUX).a)
libserialport.$(TAG_WIN).a:
	(cd libserialport && \
	git reset --hard && \
	git clean -fx && \
	rm -rf .libs && \
	./autogen.sh && \
	./configure --host=$(PFX_WIN) && \
	make && \
	cp .libs/libserialport.dll.a ../libserialport.$(TAG_WIN).a)
libserialport.$(TAG_ARM).a:
	(cd libserialport && \
	git reset --hard && \
	git clean -fx && \
	rm -rf .libs && \
	./autogen.sh && \
	./configure --host=$(PFX_ARM) && \
	make && \
	cp .libs/libserialport.a ../libserialport.$(TAG_ARM).a)


#  Pre-Calculated Strings - Release
CMD_COMP_PROG_REL_LINUX=$(PFX_LINUX)-$(CMD_COMP) $(LINK_FLAGS_LINUX) $(FLAGS_REL) $(SRC_C_EXTRA) $(OBJ_REL_LINUX) $(ARC_LINUX) $(LIBS_LINUX)
CMD_COMP_PROG_REL_WIN=$(PFX_WIN)-$(CMD_COMP) $(LINK_FLAGS_WIN) $(FLAGS_REL) $(SRC_C_EXTRA) $(OBJ_REL_WIN) $(ARC_WIN) $(LIBS_WIN)
CMD_COMP_PROG_REL_ARM=$(PFX_ARM)-$(CMD_COMP) $(LINK_FLAGS_ARM) $(FLAGS_REL) $(SRC_C_EXTRA) $(OBJ_REL_ARM) $(ARC_ARM) $(LIBS_ARM)
CMD_COMP_OBJ_REL_LINUX=$(PFX_LINUX)-$(CMD_COMP) $(FLAGS_REL) $(LIBS_LINUX) -c
CMD_COMP_OBJ_REL_WIN=$(PFX_WIN)-$(CMD_COMP) $(FLAGS_REL) $(LIBS_WIN) -c
CMD_COMP_OBJ_REL_ARM=$(PFX_ARM)-$(CMD_COMP) $(FLAGS_REL) $(LIBS_ARM) -c
OBJ_REL_LINUX=$(foreach name,$(wildcard *.c),$(subst .c,,$(PATH_OBJ)/$(name)).$(TAG_REL).$(TAG_LINUX).o)
OBJ_REL_WIN=$(foreach name,$(wildcard *.c),$(subst .c,,$(PATH_OBJ)/$(name)).$(TAG_REL).$(TAG_WIN).o)
OBJ_REL_ARM=$(foreach name,$(wildcard *.c),$(subst .c,,$(PATH_OBJ)/$(name)).$(TAG_REL).$(TAG_ARM).o)

CMD_SYMBOLS_PROG_REL_WIN=objcopy --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).sym
CMD_STRIP_PROG_REL_WIN=strip --strip-debug --strip-unneeded $@
CMD_DBGLINK_PROG_REL_WIN=objcopy --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).sym $@

#  Pre-Calculated Strings - Debug
CMD_COMP_PROG_DBG_LINUX=$(PFX_LINUX)-$(CMD_COMP) $(LINK_FLAGS_LINUX) $(FLAGS_DBG) $(SRC_C_EXTRA) $(OBJ_DBG_LINUX) $(ARC_LINUX) $(LIBS_LINUX)
CMD_COMP_PROG_DBG_WIN=$(PFX_WIN)-$(CMD_COMP) $(LINK_FLAGS_WIN) $(FLAGS_DBG) $(SRC_C_EXTRA) $(OBJ_DBG_WIN) $(ARC_WIN) $(LIBS_WIN)
CMD_COMP_PROG_DBG_ARM=$(PFX_ARM)-$(CMD_COMP) $(LINK_FLAGS_ARM) $(FLAGS_DBG) $(SRC_C_EXTRA) $(OBJ_DBG_ARM) $(ARC_ARM) $(LIBS_ARM)
CMD_COMP_OBJ_DBG_LINUX=$(PFX_LINUX)-$(CMD_COMP) $(FLAGS_DBG) $(LIBS_LINUX) -c
CMD_COMP_OBJ_DBG_WIN=$(PFX_WIN)-$(CMD_COMP) $(FLAGS_DBG) $(LIBS_WIN) -c
CMD_COMP_OBJ_DBG_ARM=$(PFX_ARM)-$(CMD_COMP) $(FLAGS_DBG) $(LIBS_ARM) -c
OBJ_DBG_LINUX=$(foreach name,$(wildcard *.c),$(subst .c,,$(PATH_OBJ)/$(name)).$(TAG_DBG).$(TAG_LINUX).o)
OBJ_DBG_WIN=$(foreach name,$(wildcard *.c),$(subst .c,,$(PATH_OBJ)/$(name)).$(TAG_DBG).$(TAG_WIN).o)
OBJ_DBG_ARM=$(foreach name,$(wildcard *.c),$(subst .c,,$(PATH_OBJ)/$(name)).$(TAG_DBG).$(TAG_ARM).o)

#  Auto-Found Archives
ARC_LINUX=$(wildcard ./*.$(TAG_LINUX).a)
ARC_LINUX+=$(foreach arcDir,$(SRC_ARC_EXTRA),$(wildcard $(arcDir)/*.$(TAG_LINUX).a))
ARC_WIN=$(wildcard ./*.$(TAG_WIN).a)
ARC_WIN+=$(foreach arcDir,$(SRC_ARC_EXTRA),$(wildcard $(arcDir)/*.$(TAG_WIN).a))
ARC_ARM=$(wildcard ./*.$(TAG_ARM).a)
ARC_ARM+=$(foreach arcDir,$(SRC_ARC_EXTRA),$(wildcard $(arcDir)/*.$(TAG_ARM).a))

#  Creating the necessary directories.
$(PATH_OBJ):
	mkdir -p $@
$(PATH_BIN):
	mkdir -p $@

#  Compiling the programme object files - release.
$(PATH_OBJ)/%.$(TAG_REL).$(TAG_LINUX).o:  %.c $(PATH_OBJ)
	$(CMD_COMP_OBJ_REL_LINUX) $< -o $@
$(PATH_OBJ)/%.$(TAG_REL).$(TAG_WIN).o:  %.c $(PATH_OBJ)
	$(CMD_COMP_OBJ_REL_WIN) $< -o $@
$(PATH_OBJ)/%.$(TAG_REL).$(TAG_ARM).o:  %.c $(PATH_OBJ)
	$(CMD_COMP_OBJ_REL_ARM) $< -o $@
#  Compiling the programme object files - debug.
$(PATH_OBJ)/%.$(TAG_DBG).$(TAG_LINUX).o:  %.c $(PATH_OBJ)
		$(CMD_COMP_OBJ_DBG_LINUX) $< -o $@
$(PATH_OBJ)/%.$(TAG_DBG).$(TAG_WIN).o:  %.c $(PATH_OBJ)
		$(CMD_COMP_OBJ_DBG_WIN) $< -o $@
$(PATH_OBJ)/%.$(TAG_DBG).$(TAG_ARM).o:  %.c $(PATH_OBJ)
		$(CMD_COMP_OBJ_DBG_ARM) $< -o $@

#  Compiling the programme binary files - release.
$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).$(EXT_PROG_LINUX):  $(OBJ_REL_LINUX) $(PATH_BIN) libserialport.$(TAG_LINUX).a
	$(CMD_COMP_PROG_REL_LINUX) -o $@
	$(CMD_OBJCOPY) --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).sym
	$(CMD_STRIP) --strip-debug --strip-unneeded $@
	$(CMD_OBJCOPY) --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_LINUX).sym $@
$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_WIN).$(EXT_PROG_WIN):  $(OBJ_REL_WIN) $(PATH_BIN) libserialport.$(TAG_WIN).a
	$(CMD_COMP_PROG_REL_WIN) -o $@
	$(PFX_WIN)-$(CMD_OBJCOPY) --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_WIN).sym
	$(PFX_WIN)-$(CMD_STRIP) --strip-debug --strip-unneeded $@
	$(PFX_WIN)-$(CMD_OBJCOPY) --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_WIN).sym $@
$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_ARM).$(EXT_PROG_ARM):  $(OBJ_REL_ARM) $(PATH_BIN) libserialport.$(TAG_ARM).a
	$(CMD_COMP_PROG_REL_ARM) -o $@
	$(PFX_ARM)-$(CMD_OBJCOPY) --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_ARM).sym
	$(PFX_ARM)-$(CMD_STRIP) --strip-debug --strip-unneeded $@
	$(PFX_ARM)-$(CMD_OBJCOPY) --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_REL).$(TAG_ARM).sym $@
#  Compiling the programme binary files - debug.
$(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_LINUX).$(EXT_PROG_LINUX):  $(OBJ_DBG_LINUX) $(PATH_BIN) libserialport.$(TAG_LINUX).a
	$(CMD_COMP_PROG_DBG_LINUX) -o $@
	$(CMD_OBJCOPY) --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_LINUX).sym
	$(CMD_STRIP) --strip-debug --strip-unneeded $@
	$(CMD_OBJCOPY) --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_LINUX).sym $@
$(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_WIN).$(EXT_PROG_WIN):  $(OBJ_DBG_WIN) $(PATH_BIN) libserialport.$(TAG_WIN).a
	$(CMD_COMP_PROG_DBG_WIN) -o $@
	$(PFX_WIN)-$(CMD_OBJCOPY) --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_WIN).sym
	$(PFX_WIN)-$(CMD_STRIP) --strip-debug --strip-unneeded $@
	$(PFX_WIN)-$(CMD_OBJCOPY) --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_WIN).sym $@
$(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_ARM).$(EXT_PROG_ARM):  $(OBJ_DBG_ARM) $(PATH_BIN) libserialport.$(TAG_ARM).a
	$(CMD_COMP_PROG_DBG_ARM) -o $@
	$(PFX_ARM)-$(CMD_OBJCOPY) --only-keep-debug $@ $(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_ARM).sym
	$(PFX_ARM)-$(CMD_STRIP) --strip-debug --strip-unneeded $@
	$(PFX_ARM)-$(CMD_OBJCOPY) --add-gnu-debuglink=$(PATH_BIN)/$(PROG_NAME).$(TAG_DBG).$(TAG_ARM).sym $@


#  Remove these from implicit compilations.
.PHONY:  all linux win arm-linux dbg-linux dbg-win dbg-arm-linux clean clean-all
