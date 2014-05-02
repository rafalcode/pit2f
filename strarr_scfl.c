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
    if((x)==((b)-2)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
        (a2)=realloc((a2), (b)*sizeof(char*)); \
        for((i)=(b)-(c);(i)<(b);++(i)) \
            (a2)[(i)]=malloc((strbuf)*sizeof(char)); \
        memset((a)+(b)-(c), 0, (c)*sizeof(t)); \
    }

typedef struct
{
    char *fl;
    char **stra; /* the string array, size of each string will be one less than ua */
    unsigned *ua;
    unsigned uasz, flsz;
} flpua_t; /* First Line Plus Unsigned Array Type */

void f2flpua_t(char *fname, flpua_t **lnarr_p)
{
    FILE *fin=fopen(fname, "r");
    int c;
    unsigned lnsz=0, lnbuf=LNBUF, strbuf=GSTRBUF, lidx=0, j;

    for(;;) {
        c=fgetc(fin);
        if(c == EOF) break;
        if(c == '\n') {
            if(lidx !=0) {
                CONDREALLOTDCA(lidx-1, lnbuf, LNBUF, (*lnarr_p)->ua, (*lnarr_p)->stra, unsigned, j, GSTRBUF);
                (*lnarr_p)->ua[lidx-1]=lnsz;
                (*lnarr_p)->stra[lidx-1][lnsz]='\0';
            } else {
                (*lnarr_p)->fl[lnsz]='\0';
                (*lnarr_p)->flsz=lnsz;
            }
            lidx++;
            lnsz=0;
            strbuf=0;
        } else {
            if(lidx ==0) {
                CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->fl);
                (*lnarr_p)->fl[lnsz]=c;
            } else {
                CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->stra[lidx-1]);
                (*lnarr_p)->stra[lidx-1][lnsz]=c;
            }
            lnsz++;
        }
    }
    (*lnarr_p)->uasz=lidx-1;
    (*lnarr_p)->fl=realloc((*lnarr_p)->fl, ((*lnarr_p)->flsz+1)*sizeof(char)); /* the plus one is because flsz leaves out nullchar */
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, (*lnarr_p)->uasz*sizeof(unsigned));
    for(j=(*lnarr_p)->uasz;j<lnbuf;++j) 
        free((*lnarr_p)->stra[j]);
    (*lnarr_p)->stra=realloc((*lnarr_p)->stra, (*lnarr_p)->uasz*sizeof(char*));

    fclose(fin);
    return;
}

flpua_t *crea_flpua_t(void) /* with minimum memory allocations as well */
{
    unsigned j;
    flpua_t *lnarr_p=malloc(sizeof(flpua_t));
    lnarr_p->ua=calloc(LNBUF, sizeof(unsigned));
    lnarr_p->stra=malloc(LNBUF*sizeof(char*));
    lnarr_p->fl=calloc(GSTRBUF, sizeof(char));
    for(j=0;j<LNBUF;++j) 
        lnarr_p->stra[j]=calloc(GSTRBUF, sizeof(char));
    return lnarr_p;
}

void prto_flpua_t(flpua_t *lnarr_p)
{
    unsigned j;
    printf("Input has %u lines (less first line), per line chars is (first line is excluded):\n", lnarr_p->uasz); 
    printf("First line _string_, of length %u, is: \"%s\"\n", lnarr_p->flsz, lnarr_p->fl); 
    for(j=0;j<lnarr_p->uasz;++j) 
        printf((j==lnarr_p->uasz-1)? "%u\n" : "%u ", lnarr_p->ua[j]);
    return;
}

void free_flpua_t(flpua_t **lnarr_p)
{
    unsigned j;
    free((*lnarr_p)->ua);
    free((*lnarr_p)->fl);
    for(j=0;j<(*lnarr_p)->uasz;++j) 
        free((*lnarr_p)->stra[j]);
    free((*lnarr_p)->stra);
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
