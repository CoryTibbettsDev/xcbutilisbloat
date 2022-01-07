PROG := xcbutilisbloat
VERSION = `git describe --always --tags`

RM ?= rm -f
CP ?= cp -f

DESTDIR ?=
PREFIX ?= ${DESTDIR}/usr/local
LIBDIR ?= ${PREFIX}/lib
INCDIR ?= ${PREFIX}/include

# FREETYPEINC := `pkgconf --cflags freetype2`
FREETYPEINC := `pkg-config --cflags freetype2`
INC = ${FREETYPEINC}
BASICFLAGS = -std=c99 -Wall
CFLAGS += ${BASICFLAGS} ${INC}
CPPFLAGS += -DPROG="\"${PROG}\"" -DVERSION="\"${VERSION}\""
# Included libraries
# For some reason the include path for xcb render util is <xcb/xcb_renderutil.h>
# but the library include (the .so file name) is xcb-render-util
# why so inconsistent though???
LDFLAGS += -lfontconfig -lfreetype -lxcb -lxcb-render -lxcb-render-util
# Extra flags for debugging
CFDEBUG = -g -pedantic -Wextra -Wno-deprecated-declarations -Wlong-long \
		-Wconversion -Wsign-conversion -Wreturn-local-addr

SRC := font.c util.c
HDR := xcbutilisbloat.h
OBJ := ${SRC:.c=.o}
STATIC = lib${PROG}.a
SHARED = lib${PROG}.so
SHARED0 = ${SHARED}.0
SHAREDVERSION = ${SHARED}.${VERSION}

INSTALLEXEC ?=
ifeq (${LIBTYPE},static)
	INSTALLEXEC = ${STATIC}
else
	INSTALLEXEC = ${SHARED}
endif

.PHONY: clean debug version install uninstall

all: ${INSTALLEXEC}

version:
	printf "%s Version: %s\n" "${PROG}" "${VERSION}"

.SUFFIXES: .c .o
.c.o:
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

${STATIC}: ${OBJ}
	ar rcs $@ $^
	ranlib $@

${SHARED}: CFLAGS += -fPIC
${SHARED}: ${OBJ}
	${CC} -shared -Wl,-soname,${SHARED0} -o ${SHAREDVERSION} ${OBJ} ${LDFLAGS}

debug: CFLAGS += ${CFDEBUG}
debug: all

TESTFLAGS = ${BASICFLAGS} ${CFDEBUG}
TESTLIBS = -lc -l${PROG} -lfontconfig -lfreetype -lxcb #-lxcb-render -lxcb-render-util
ifeq (${INSTALLEXEC},${STATIC})
test: debug
	${CC} -Wall ${TESTFLAGS} -c test.c
	${CC} -o test test.o -L. ${TESTLIBS}
	./test
else ifeq (${INSTALLEXEC},${SHARED})
test: debug
	ldconfig -n .
	ln -sf ${SHARED0} ${SHARED}
	${CC} ${TESTFLAGS} -c test.c
	${CC} -o test test.o -L. ${TESTLIBS}
	LD_LIBRARY_PATH="." ./test
endif

ifeq (${INSTALLEXEC},${STATIC})
install: all
	install -D ${INSTALLEXEC} ${LIBDIR}
	${CP} ${HDR} ${INCDIR}
else ifeq (${INSTALLEXEC},${SHARED})
install: all
	install -D ${SHAREDVERSION} ${LIBDIR}
	ln -sf ${LIBDIR}/${SHARED0} ${LIBDIR}/${SHARED}
	ldconfig ${LIBDIR}
	${CP} ${HDR} ${INCDIR}
endif

uninstall:
	${RM} ${LIBDIR}/${STATIC}* ${LIBDIR}/${SHARED}* ${INCDIR}/${HDR}

clean:
	${RM} ${OBJ} lib${PROG}* test test.o
