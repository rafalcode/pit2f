# this project starts with the simplest of line number array programs
# and builds upon it to incorporate most specific versions of the same idea.
#
# the project is named pit2f, named so as it pits two files against each other: "pit" (as verb) is the same as "to match up"
#
CC=gcc
DBGCFLAGS=-g -Wall# -pg # note the gprof option
CFLAGS=-O2
EXES=lnarr lnarr_scfl strarr_scfl str2arr_scfl rdfasta pit2f pit2f_d hamin2 pit2fyu pit2fwh pit2fwh_d pit2d

# lnarr, simple line arrays from a single file
lnarr: lnarr.c
	${CC} ${DBGCFLAGS} -o $@ $^

# lnarr_scfl, first variation of the lnarr. the first line is treated specially
lnarr_scfl: lnarr_scfl.c
	${CC} ${DBGCFLAGS} -o $@ $^

# strarr_scfl, first line is held in a string, the rest of the lines are also stored ... a double array in introduced into the struct */
strarr_scfl: strarr_scfl.c
	${CC} ${DBGCFLAGS} -o $@ $^

# We hive off str2arr_scfl from strarr_scfl, because we want to split each line (except the first) by a certain one character token
str2arr_scfl: str2arr_scfl.c
	${CC} ${DBGCFLAGS} -o $@ $^

# rdfasta, I use a previously coded summarize fasta program to grab the idline only. I discard the sequence!
rdfasta: rdfasta.c
	${CC} ${DBGCFLAGS} -o $@ $^

# Finally we can get around to combining the two files */
pit2f: pit2f.c
	${CC} ${CFLAGS} -o $@ $^

# This version includes a simple break because it assume unique matches
pit2fyu: pit2f.c
	${CC} ${CFLAGS} -DYESUNIQUE -o $@ $^

# The debug version of above .. i.e. printing of struct contents.
pit2f_d: pit2f.c
	${CC} ${DBGCFLAGS} -DDEBUG -o $@ $^

# For real speed increases you need to hash .. with is With Hash
pit2fwh: pit2fwh.c
	${CC} ${CFLAGS} -o $@ $^

# The debug version of above .. i.e. printing of struct contents.
pit2fwh_d: pit2fwh.c
	${CC} ${DBGCFLAGS} -DDEBUG -o $@ $^

# We now want to compare two directories of fasta files and see which id match .. builds upon pit2fwh
pit2d: pit2d.c
	${CC} ${CFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXES} vgcore.* core*
