export TOP := $(shell pwd)
export _DEBUG_ := y

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
	${CC} -o $@ $^ ${CFLAGS} -lholyhttp -L$(TOP)/lib
	${STRIP} $@

cplib:
	@-cp -f ${TOP}/../holyhttp/libholyhttp.so $(TOP)/lib
	@-cp -f ${TOP}/../holyhttp/holyhttp.h $(TOP)/lib
