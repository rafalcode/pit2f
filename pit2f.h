/* header file with macros for the pit2f program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GBUF 4
#define SSZ 5 /* DNA symbol alphabet size is 4 ? Of course, but the fifth is for N's which sometimes appear, and the sixth is for anything elses */
#define CTOK ' '
#define LNBUF 3
#define GSTRBUF 4 /* general string buffer */

/* quick macro for conditionally enlarging a general native type array */
#define CONDREALLOC(x, b, c, a, t); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
    }

/* quick macro for conditionally enlarging a char pointer, space always available for final null char */
#define CONDREALLOCP(x, b, c, a); \
    if((x)==((b)-2)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(char)); \
    }

/* quick macro for conditionally enlarging a DOUBLE char pointer, space always available for final null char */
#define CONDREALLODCP(x, b, c, a, i, strbuf); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(char*)); \
        for((i)=(a)+(b)-(c);(i)<(a)+(b);++(i)) \
        (a)[(i)]=malloc((strbuf)*sizeof(char)); \
    }

/* quick macro for conditionally enlarging a numeric native type array and setting extended memory to zero */
#define CONDREALLOD(x, b, c, a, t); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
        memset((a)+(b)-(c), 0, (c)*sizeof(t)); \
    }

/* quick macro for conditionally enlarging a twin digit and char array based on same lnbuf, a2 MUST be a double char ptr */
#define CONDREALLOTDCA(x, b, c, a, a2, t, i, strbuf); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
        (a2)=realloc((a2), (b)*sizeof(char*)); \
        for((i)=(b)-(c);(i)<(b);++(i)) \
        (a2)[(i)]=malloc((strbuf)*sizeof(char)); \
        memset((a)+(b)-(c), 0, (c)*sizeof(t)); \
    }

/* quick macro for conditionally enlarging a TWO twin digit and char array based on same lnbuf, a2 MUST be a double char ptr */
#define CONDREALLOTDCA2(x, b, c, a, a1, a2, a3, t, i, strbuf); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
        (a1)=realloc((a1), (b)*sizeof(t)); \
        (a2)=realloc((a2), (b)*sizeof(char*)); \
        (a3)=realloc((a3), (b)*sizeof(char*)); \
        for((i)=(b)-(c);(i)<(b);++(i)) { \
        (a2)[(i)]=malloc((strbuf)*sizeof(char)); \
        (a3)[(i)]=malloc((strbuf)*sizeof(char)); \
        } \
        memset((a)+(b)-(c), 0, (c)*sizeof(t)); \
        memset((a1)+(b)-(c), 0, (c)*sizeof(t)); \
    }
