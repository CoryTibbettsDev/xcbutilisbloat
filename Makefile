PROGRAMNAME = xcbutilisbloat
VERSION = 0.1

EXEC = ${PROGRAMNAME}
# Uncomment static or shared library which ever you want
# INSTALLEXEC = $(STATIC)
INSTALLEXEC = $(SHARED)

CP = cp -f

# User compiled programs go in /usr/local/bin link below has good explination
# This is library so it goes is /usr/local/lib
# https://unix.stackexchange.com/questions/8656/usr-bin-vs-usr-local-bin-on-linux
PREFIXDIR = /usr/local
DESTDIR = $(PREFIXDIR)/lib
# Need to copy header files for our library for compiler
DESTINCLUDEDIR = $(PREFIXDIR)/include

# FREETYPEINC = $(shell pkg-config --cflags freetype2)
FREETYPEINC = $(shell pkgconf --cflags freetype2)
INCS = $(FREETYPEINC)
BASICFLAGS = -std=c99 -Wall -pedantic 
# Extra flags for debugging
CFDEBUG = -g -Wextra -Wno-deprecated-declarations -Wlong-long \
		   -Wconversion -Wsign-conversion -Wreturn-local-addr
CFLAGS = $(BASICFLAGS) -Os $(INCS) ${CPPFLAGS}
CPPFLAGS = -DPROGRAMNAME="\"$(PROGRAMNAME)\"" -DVERSION="\"$(VERSION)\""
# Included libraries
LDFLAGS = -lc -lxcb -lfontconfig -lfreetype
# If Linux we add bsd c standard library
# TARGETOS = $(shell uname -s)
# ifeq ($(TARGETOS),Linux)
# 	LDFLAGS += -lbsd
# endif

SRC = font.c util.c
HDR = xcbutilisbloat.h
OBJ = ${SRC:.c=.o}
STATIC = lib${EXEC}.a
SHARED = lib${EXEC}.so
SHARED0 = $(SHARED).0
SHAREDVERSION = $(SHARED).$(VERSION)

.PHONY: all clean debug install uninstall

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(INSTALLEXEC)

$(STATIC): ${OBJ}
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
	$(CC) -fPIC $(TESTFLAGS) -c test.c
	$(CC) -o test test.o -L. $(TESTLIBS)
	LD_LIBRARY_PATH="." ./test
endif

ifeq ($(INSTALLEXEC),$(STATIC))
install: all
	install $(INSTALLEXEC) $(DESTDIR)
	$(CP) $(HDR) $(DESTINCLUDEDIR) 
else ifeq ($(INSTALLEXEC),$(SHARED))
install: all
	install $(SHAREDVERSION) $(DESTDIR)
	ln -sf $(DESTDIR)/$(SHARED0) $(DESTDIR)/$(SHARED)
	ldconfig $(DESTDIR)
	$(CP) $(HDR) $(DESTINCLUDEDIR) 
endif

uninstall:
	${RM} ${DESTDIR}/${STATIC}* ${DESTDIR}/${SHARED}* $(DESTINCLUDEDIR)/$(HDR)

clean:
	${RM} ${OBJ} lib$(EXEC)* test test.o
