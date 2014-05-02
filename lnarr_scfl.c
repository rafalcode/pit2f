/* this is the most basic line number array program for a single input file
 * it is the bulding block for many more specific line readers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LNBUF 4
#define GSTRBUF 4 /* general string buffer */
/* quick macro for conditionally enlarging a general native type array */
#define CONDREALLOC(x, b, c, a, t); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
    }

/* quick macro for conditionally enlarging a numeric native type array and setting extended memory to zero */
#define CONDREALLOD(x, b, c, a, t); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
        memset((a)+(b)-(c), 0, (c)*sizeof(t)); \
    }

typedef struct
{
    char *fl;
    unsigned *ua;
    unsigned uasz;
} flpua_t; /* First Line Plus Unsigned Array Type */

void f2flpua_t(char *fname, flpua_t **lnarr_p)
{
    FILE *fin=fopen(fname, "r");
    int c;
    unsigned lnsz=0, lnbuf=LNBUF;
    unsigned lidx=0;
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, LNBUF*sizeof(unsigned)); /* pointer to a line array, packaged as a ptr to flpua_t */
    memset((*lnarr_p)->ua, 0, lnbuf*sizeof(unsigned));

    for(;;) {
        c=fgetc(fin);
        if(c == EOF) break;
        if(c == '\n') {
            CONDREALLOD(lidx, lnbuf, LNBUF, (*lnarr_p)->ua, unsigned);
            (*lnarr_p)->ua[lidx++]=lnsz;
            lnsz=0;
        } else
            lnsz++;
    }
    (*lnarr_p)->uasz=lidx;
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, (*lnarr_p)->uasz*sizeof(unsigned));

    fclose(fin);
    return;
}


flpua_t *crea_flpua_t(void)
{
    flpua_t *lnarr_p=malloc(sizeof(flpua_t));
    lnarr_p->fl=calloc(GSTRBUF, sizeof(char));
    lnarr_p->ua=NULL; /* valgrind very picky about initializations */
    return lnarr_p;
}

void prto_flpua_t(flpua_t *lnarr_p)
{
    unsigned j;
    printf("Input has %u lines, per line chars is (first line is excluded):\n", lnarr_p->uasz); 
    printf("First line is: \"%s\"\n", lnarr_p->fl); 
    for(j=0;j<lnarr_p->uasz;++j) 
        printf((j==lnarr_p->uasz-1)? "%u\n" : "%u ", lnarr_p->ua[j]);
    return;
}

void free_flpua_t(flpua_t **lnarr_p)
{
    free((*lnarr_p)->ua);
    free((*lnarr_p)->fl);
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
