CC=gcc

CFLAGS=
LFLAGS=
OUTNAME="hex2ansi256"
# LFLAGS=-lpthread

.PHONY: all
all: main

main: main.c
	${CC} -o hex2ansi256 $^ ${CFLAGS} ${LFLAGS}

.PHONY: clean
	@rm hex2ansi256
