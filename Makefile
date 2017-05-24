# Tested on macOS Sierra running 10.12.5

CFLAGS+=-O2

all::	lifegen

lifegen:	lifegen.c
	${CC} ${CFLAGS} lifegen.c -o $@

test::	all
	time lifegen

clean::
	rm -f lifegen
