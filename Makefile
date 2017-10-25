export TOP := $(shell pwd)
export _DEBUG_ := y

TARGET := holydemo
OBJS := main.o
SUBDIRS := 
LIBFILE := ${TOP}/../holyhttp/libholyhttp.so

CFLAGS-${_DEBUG_} += -g -ggdb
CFLAGS-y += -I${TOP}
CFLAGS-y += -lholyhttp
CFLAGS-y += -Wall -Wno-missing-braces
CFLAGS-y += -Werror
CFLAGS-${_DEBUG_} += -DDEBUG_ON=1

########## DO NOT MODIFY THE BELOW ##########
export CFLAGS := ${CFLAGS-y}

include ${TOP}/common.mk

all: cplib subs ${TARGET}

${TARGET}: ${OBJS} ${SUBOBJS}
	${CC} -o $@ $^ ${CFLAGS}
	${STRIP} $@

cplib:
	@-cp -f ${LIBFILE} /lib
	@-cp -f ${LIBFILE} /usr/lib
	@-cp -f ${LIBFILE} /lib64
	@-cp -f ${LIBFILE} /usr/lib64

