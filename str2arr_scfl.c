/* this is the most basic line number array program for a single input file
 * it is the bulding block for many more specific line readers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct
{
    char *fl;
    char **stra0, **stra1; /* the string array, size of each string will be one less than ua */
    unsigned *ua0, *ua1;
    unsigned uasz, flsz;
} flpua_t; /* First Line Plus Unsigned Array Type */

void f2flpua_t(char *fname, flpua_t **lnarr_p)
{
    FILE *fin=fopen(fname, "r");
    int c;
    unsigned lnsz=0, lnbuf=LNBUF, strbuf=GSTRBUF, lidx=0, j;
    unsigned char seenctok=0;

    for(;;) {
        c=fgetc(fin);
        if(c == EOF) break;
        if(c == '\n') {
            if(lidx !=0) {
                CONDREALLOTDCA2(lidx-1, lnbuf, LNBUF, (*lnarr_p)->ua0, (*lnarr_p)->ua1, (*lnarr_p)->stra0, (*lnarr_p)->stra1, unsigned, j, GSTRBUF);
                (*lnarr_p)->ua1[lidx-1]=lnsz;
                (*lnarr_p)->stra1[lidx-1][lnsz]='\0';
                (*lnarr_p)->stra1[lidx-1]=realloc((*lnarr_p)->stra1[lidx-1], ((*lnarr_p)->ua1[lidx-1]+1)*sizeof(char));
            } else {
                (*lnarr_p)->fl[lnsz]='\0';
                (*lnarr_p)->flsz=lnsz;
                (*lnarr_p)->fl=realloc((*lnarr_p)->fl, ((*lnarr_p)->flsz+1)*sizeof(char));
            }
            lidx++;
            lnsz=0;
            seenctok=0; /* reset */
            strbuf=GSTRBUF;
        } else {
            if(lidx ==0) {
                CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->fl);
                (*lnarr_p)->fl[lnsz]=c;
            } else {
                if(!seenctok) {
                    if(c==CTOK) { /* finish up our "0" counterparts */
//                        CONDREALLOTDCA(lidx-1, lnbuf, LNBUF, (*lnarr_p)->ua0, (*lnarr_p)->stra0, unsigned, j, GSTRBUF);
                        (*lnarr_p)->ua0[lidx-1]=lnsz;
                        (*lnarr_p)->stra0[lidx-1][lnsz]='\0';
                        (*lnarr_p)->stra0[lidx-1]=realloc((*lnarr_p)->stra0[lidx-1], ((*lnarr_p)->ua0[lidx-1]+1)*sizeof(char));
                        lnsz=0;
                        seenctok=1; /* toggle */
                        strbuf=GSTRBUF;
                        (*lnarr_p)->stra1[lidx-1][lnsz]=c; /* this seemd trivial but turned out to be vital in the printing stage ... the undefined zero index caused havoc */
                    } else {
                        CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->stra0[lidx-1]);
                        (*lnarr_p)->stra0[lidx-1][lnsz]=c;
                    }
                } else {
                    CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->stra1[lidx-1]);
                    (*lnarr_p)->stra1[lidx-1][lnsz]=c;
                }
            }
            lnsz++;
        }
    }
    (*lnarr_p)->uasz=lidx-1;
    (*lnarr_p)->ua0=realloc((*lnarr_p)->ua0, (*lnarr_p)->uasz*sizeof(unsigned));
    (*lnarr_p)->ua1=realloc((*lnarr_p)->ua1, (*lnarr_p)->uasz*sizeof(unsigned));
    for(j=(*lnarr_p)->uasz;j<lnbuf;++j) {
        free((*lnarr_p)->stra0[j]);
        free((*lnarr_p)->stra1[j]);
    }
    (*lnarr_p)->stra0=realloc((*lnarr_p)->stra0, (*lnarr_p)->uasz*sizeof(char*));
    (*lnarr_p)->stra1=realloc((*lnarr_p)->stra1, (*lnarr_p)->uasz*sizeof(char*));

    fclose(fin);
    return;
}

flpua_t *crea_flpua_t(void) /* with minimum memory allocations as well */
{
    unsigned j;
    flpua_t *lnarr_p=malloc(sizeof(flpua_t));
    lnarr_p->ua0=calloc(LNBUF, sizeof(unsigned));
    lnarr_p->ua1=calloc(LNBUF, sizeof(unsigned));
    lnarr_p->stra0=malloc(LNBUF*sizeof(char*));
    lnarr_p->stra1=malloc(LNBUF*sizeof(char*));
    lnarr_p->fl=calloc(GSTRBUF, sizeof(char));
    for(j=0;j<LNBUF;++j) {
        lnarr_p->stra0[j]=calloc(GSTRBUF, sizeof(char));
        lnarr_p->stra1[j]=calloc(GSTRBUF, sizeof(char));
    }
    return lnarr_p;
}

void prto_flpua_t(flpua_t *lnarr_p)
{
    unsigned j;
    printf("Input has %u lines (less first line), per line chars is (first line is excluded):\n", lnarr_p->uasz); 
    printf("First line _string_, of length %u, is: \"%s\"\n", lnarr_p->flsz, lnarr_p->fl); 
    printf("Listing of the rest of the lines\n"); 
    for(j=0;j<lnarr_p->uasz;++j) 
        printf("l%3u:\"%s|%s\"(%u)\n", j, lnarr_p->stra0[j], lnarr_p->stra1[j], lnarr_p->ua0[j]+lnarr_p->ua1[j]);
    return;
}

void free_flpua_t(flpua_t **lnarr_p)
{
    unsigned j;
    free((*lnarr_p)->ua0);
    free((*lnarr_p)->ua1);
    free((*lnarr_p)->fl);
    for(j=0;j<(*lnarr_p)->uasz;++j) {
        free((*lnarr_p)->stra0[j]);
        free((*lnarr_p)->stra1[j]);
    }
    free((*lnarr_p)->stra0);
    free((*lnarr_p)->stra1);
    free((*lnarr_p));
    return;
}

int main(int argc, char *argv[])
{
    /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
    if(argc!=2) {
        printf("Error. Pls supply argument (name of file).\n");
        exit(EXIT_FAILURE);
    }

    /* convert this file into a line array */
    flpua_t *lnarr_p=crea_flpua_t();
    f2flpua_t(argv[1], &lnarr_p);
    prto_flpua_t(lnarr_p);

    free_flpua_t(&lnarr_p);

    return 0;
}
