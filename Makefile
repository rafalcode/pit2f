# this project starts with the simplest of line number array programs
# and builds in on it to incorporate most specific versions of the same idea.
#
# the project is named pit2f, because I wanted to pit two files against each other
CC=gcc
CFLAGS=-g -Wall# -pg # note the gprof option
EXES=lnarr

# lnarr, simple line arrays from a single file
lnarr: lnarr.c
	${CC} ${CFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXES} core