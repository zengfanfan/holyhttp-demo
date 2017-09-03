export TOP := $(shell pwd)
export _DEBUG_ := y
export CC := gcc
export STRIP := strip

TARGET := holydemo
OBJS := main.o
SUBDIRS := 
LIBPATH := $(shell realpath ../holyhttp)

CFLAGS-${_DEBUG_} += -g -ggdb
CFLAGS-y += -I${TOP}
CFLAGS-y += -I${LIBPATH}
CFLAGS-y += -L${LIBPATH} -lholyhttp
CFLAGS-y += -D_GNU_SOURCE -D__USE_XOPEN
CFLAGS-y += -Wall -Wno-missing-braces
#CFLAGS-y += -Werror
CFLAGS-${_DEBUG_} += -DDEBUG_ON=1

########## DO NOT MODIFY THE BELOW ##########
export CFLAGS := ${CFLAGS-y}

include ${TOP}/common.mk

${TARGET}: ${OBJS} ${SUBOBJS}
	${CC} -o $@ $^ ${CFLAGS}
	${STRIP} $@

