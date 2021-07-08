PROGRAMNAME := xcbutilisbloat
VERSION ?= $(shell git describe --tags)

EXEC = $(PROGRAMNAME)
# Uncomment static or shared library which ever you want
# INSTALLEXEC = $(STATIC)
INSTALLEXEC = $(SHARED)

RM ?= rm -f
CP ?= cp -f

# User compiled programs go in /usr/local/bin link below has good explination
# This is library so it goes is /usr/local/lib
# https://unix.stackexchange.com/questions/8656/usr-bin-vs-usr-local-bin-on-linux
PREFIX ?= /usr/local
DEST ?= $(PREFIX)/lib
# Directory for header file to be installed in
DESTINCLUDE ?= $(PREFIX)/include

# FREETYPEINC := $(shell pkgconf --cflags freetype2)
FREETYPEINC := $(shell pkg-config --cflags freetype2)
INCS = $(FREETYPEINC)
BASICFLAGS = -std=c99 -Wall
CFLAGS += $(BASICFLAGS) $(INCS)
CPPFLAGS += -DPROGRAMNAME="\"$(PROGRAMNAME)\"" -DVERSION="\"$(VERSION)\""
# Included libraries
# For some reason the include path for xcb render util in <xcb/xcb_renderutil.h>
# but the library include (.so file name) is xcb-render-util why though???
LDFLAGS += -lfontconfig -lfreetype -lxcb -lxcb-render# -lxcb-render-util
# Extra flags for debugging
CFDEBUG = -g -pedantic -Wextra -Wno-deprecated-declarations -Wlong-long \
		   -Wconversion -Wsign-conversion -Wreturn-local-addr

SRC := font.c util.c
HDR := xcbutilisbloat.h
OBJ := $(SRC:.c=.o)
STATIC = lib$(EXEC).a
SHARED = lib$(EXEC).so
SHARED0 = $(SHARED).0
SHAREDVERSION = $(SHARED).$(VERSION)

.PHONY: clean debug install uninstall

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

all: $(INSTALLEXEC)

$(STATIC): $(OBJ)
	ar rcs $@ $^
	ranlib $@

$(SHARED): CFLAGS += -fPIC
$(SHARED): $(OBJ)
	$(CC) -shared -Wl,-soname,$(SHARED0) -o $(SHAREDVERSION) $(OBJ) $(LDFLAGS)

debug: CFLAGS += $(CFDEBUG)
debug: all

TESTFLAGS = $(BASICFLAGS) $(CFDEBUG)
TESTLIBS = -lc -lxcb -l$(EXEC)
ifeq ($(INSTALLEXEC),$(STATIC))
test: debug
	$(CC) -Wall $(TESTFLAGS) -c test.c
	$(CC) -o test test.o -L. $(TESTLIBS)
	./test
else ifeq ($(INSTALLEXEC),$(SHARED))
test: debug
	ldconfig -n .
	ln -sf $(SHARED0) $(SHARED) 
	$(CC) $(TESTFLAGS) -c test.c
	$(CC) -o test test.o -L. $(TESTLIBS)
	LD_LIBRARY_PATH="." ./test
endif

ifeq ($(INSTALLEXEC),$(STATIC))
install: all
	install $(INSTALLEXEC) $(DEST)
	$(CP) $(HDR) $(DESTINCLUDE) 
else ifeq ($(INSTALLEXEC),$(SHARED))
install: all
	install $(SHAREDVERSION) $(DEST)
	ln -sf $(DEST)/$(SHARED0) $(DEST)/$(SHARED)
	ldconfig $(DEST)
	$(CP) $(HDR) $(DESTINCLUDE) 
endif

uninstall:
	$(RM) $(DEST)/$(STATIC)* $(DEST)/$(SHARED)* $(DESTINCLUDE)/$(HDR)

clean:
	$(RM) $(OBJ) lib$(EXEC)* test test.o
