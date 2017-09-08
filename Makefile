export TOP := $(shell pwd)
export _DEBUG_ := y
export CC := gcc
export STRIP := strip

TARGET := holydemo
OBJS := main.o
SUBDIRS := 

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
	@-cp -f ${TOP}/lib/libholyhttp.so /lib
	@-cp -f ${TOP}/lib/libholyhttp.so /usr/lib
	@-cp -f ${TOP}/lib/libholyhttp.so /lib64
	@-cp -f ${TOP}/lib/libholyhttp.so /usr/lib64

