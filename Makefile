# this project starts with the simplest of line number array programs
# and builds in on it to incorporate most specific versions of the same idea.
#
# the project is named pit2f, because I wanted to pit two files against each other
CC=gcc
CFLAGS=-g -Wall# -pg # note the gprof option
EXES=lnarr lnarr_scfl strarr_scfl str2arr_scfl rdfasta pit2f pit2f_d

# lnarr, simple line arrays from a single file
lnarr: lnarr.c
	${CC} ${CFLAGS} -o $@ $^

# lnarr_scfl, first variation of the lnarr. the first line is treated specially
lnarr_scfl: lnarr_scfl.c
	${CC} ${CFLAGS} -o $@ $^

# strarr_scfl, first line is held in a string, the rest of the lines are also stored ... a double array in introduced into the struct */
strarr_scfl: strarr_scfl.c
	${CC} ${CFLAGS} -o $@ $^

# We hive off str2arr_scfl from strarr_scfl, because we want to split each line (except the first) by a certain one character token
str2arr_scfl: str2arr_scfl.c
	${CC} ${CFLAGS} -o $@ $^

# rdfasta, I use a previously coded summarize fasta program to grab the idline only. I discard the sequence!
rdfasta: rdfasta.c
	${CC} ${CFLAGS} -o $@ $^

# Finally we can get around to combining the two files */
pit2f: pit2f.c
	${CC} ${CFLAGS} -o $@ $^

# The debug version of above .. i.e. printing of struct contents.
pit2f_d: pit2f.c
	${CC} ${CFLAGS} -DDEBUG -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXES} vgcore.* core*
