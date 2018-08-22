/* strarr_scfl.c building on lnarr.c to produce a program which slurps each line of a text file nto a SINGLE array EXCEPT for the first line however
   Copyright (C) 2014  Ramon Fallon

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
   */
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
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(char)); \
        memset((a)+(b)-(c), '\0', (c)*sizeof(char)); \
    }

/* quick macro for conditionally enlarging a DOUBLE char pointer, space always available for final null char */
#define CONDREALLODCP(x, b, c, a, i, strbuf); \
    if((x)==((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(char*)); \
        for((i)=(b)-(c);(i)<(b);++(i)) \
            (a)[(i)]=calloc((strbuf), sizeof(char)); \
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
            (a2)[(i)]=calloc((strbuf), sizeof(char)); \
        memset((a)+(b)-(c), 0, (c)*sizeof(t)); \
    }

struct strchainode
{
    char *ln;
    struct strchainode *n;
    int idx; // the index corresponding to this mp element: it's the sort of thing youex
    int dp; // number of actual duplicates
};
typedef struct strchainode snod;

typedef struct /* stra_t */
{
    char **stra; /* the string array, size of each string will be one less than ua */
    unsigned *ua; /* an array of unsigned's */
    unsigned uasz; /* size of the array of unsigned's */
} stra_t; /* Array of strings type */

unsigned hashit(char *str, unsigned tsz) /* Dan Bernstein's one */
{
    unsigned long hash = 5381;
    int c;

    char *tstr=str;
    while ((c = *tstr++))
        hash = ((hash << 5) + hash) + c; /*  hash * 33 + c */

    return hash % tsz;
}

unsigned givehtsz(unsigned mnf)
{
    unsigned htsz=2*mnf/3;
    // try to grab a prime ... well just avoid 5-multiples, 3-multiples, and evens
    if(!(htsz%5)) 
        htsz++; // incrment by 1 if multiple of 5
    if(!(htsz%3)) 
        htsz++;
    if(!(htsz%2)) 
        htsz++;
    return htsz;
}
snod **tochainharr(stra_t *lnarr_p, unsigned tsz)
{
    unsigned i;

    snod **stab=malloc(tsz*sizeof(snod *));
    for(i=0;i<tsz;++i) 
        stab[i]=NULL;
    snod *tsnod0, *tsnod2;

    unsigned tint;
    for(i=0;i<lnarr_p->uasz;++i) {
        if(!lnarr_p->ua[i]) continue; // empty line

        tint=hashit(lnarr_p->stra[i], tsz); 
        // this hash entry is new.
        if( (stab[tint] == NULL) ) {
            stab[tint]=malloc(sizeof(snod));
            stab[tint]->ln=lnarr_p->stra[i];
            stab[tint]->idx=i;
            stab[tint]->dp=0;
            stab[tint]->n=NULL;
            continue;
        }
        tsnod2=stab[tint];
        while( (tsnod2 != NULL) ) {
            if(!strcmp(tsnod2->ln, lnarr_p->stra[i])) {
                tsnod2->dp++;
                goto getout;
            }
            tsnod0=tsnod2;
            tsnod2=tsnod2->n;
        }
        tsnod0->n=malloc(sizeof(snod));
        tsnod0->n->ln=lnarr_p->stra[i];
        tsnod0->n->idx=i;
        tsnod0->n->dp=0;
        tsnod0->n->n=NULL;
getout:
        continue;
    }
    return stab;
}

void prtchaharr(snod **stab, unsigned tsz)
{
    unsigned i;
    snod *tsnod2;
    for(i=0;i<tsz;++i) {
        printf("Tablepos %i: ", i); 
        tsnod2=stab[i];
        while(tsnod2) {
            printf("\"%s\" ", tsnod2->ln);
            tsnod2=tsnod2->n;
        }
        printf("\n"); 
    }
    return;
}

void freechainharr(snod **stab, size_t tsz)
{
    int i;
    snod *tsnod0, *tsnod2;
    for(i=0; i<tsz; ++i) {
        if( (stab[i] != NULL) ) {
            while( (stab[i]->n != NULL) ) {
                tsnod0=stab[i];
                tsnod2=stab[i]->n;
                while((tsnod2->n != NULL) ){
                    tsnod0=tsnod2;
                    tsnod2=tsnod2->n;
                }
                free(tsnod0->n);
                tsnod0->n=NULL;
            }
            free(stab[i]);
        }
    }
    free(stab);
    return;
}

void f2stra_t(char *fname, stra_t **lnarr_p)
{
    FILE *fin=fopen(fname, "r");
    int c;
    unsigned lnsz=0, lnbuf=LNBUF, strbuf=GSTRBUF, lidx=0, j;

    for(;;) {
        c=fgetc(fin);
        if(c == EOF) break;
        if(c == '\n') {
            (*lnarr_p)->ua[lidx]=lnsz;
            (*lnarr_p)->stra[lidx][lnsz]='\0';
            CONDREALLOTDCA(lidx, lnbuf, LNBUF, (*lnarr_p)->ua, (*lnarr_p)->stra, unsigned, j, GSTRBUF);
            lidx++;
            lnsz=0;
            strbuf=GSTRBUF;
        } else {
            CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->stra[lidx]);
            (*lnarr_p)->stra[lidx][lnsz]=c;
            lnsz++;
        }
    }
    (*lnarr_p)->uasz=lidx;
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, (*lnarr_p)->uasz*sizeof(unsigned));
    for(j=(*lnarr_p)->uasz;j<lnbuf;++j) 
        free((*lnarr_p)->stra[j]);
    (*lnarr_p)->stra=realloc((*lnarr_p)->stra, (*lnarr_p)->uasz*sizeof(char*));

    fclose(fin);
    return;
}

stra_t *crea_stra_t(void) /* with minimum memory allocations as well */
{
    unsigned j;
    stra_t *lnarr_p=malloc(sizeof(stra_t));
    lnarr_p->ua=calloc(LNBUF, sizeof(unsigned));
    lnarr_p->stra=malloc(LNBUF*sizeof(char*));
    for(j=0;j<LNBUF;++j) 
        lnarr_p->stra[j]=calloc(GSTRBUF, sizeof(char));
    return lnarr_p;
}

void prto_stra_t(stra_t *lnarr_p)
{
    unsigned j;
    printf("Input file has %u lines. iLines with metadata as follows:\n", lnarr_p->uasz); 
    for(j=0;j<lnarr_p->uasz;++j) 
        printf("l%3u:\"%s\"(%u)\n", j, lnarr_p->stra[j], lnarr_p->ua[j]);
    return;
}

void free_stra_t(stra_t **lnarr_p)
{
    unsigned j;
    free((*lnarr_p)->ua);
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

    /* convert this 1-column file into a array of lines */
    stra_t *lnarr_p=crea_stra_t(); /* lnarr_p, line array pointer */
    f2stra_t(argv[1], &lnarr_p);
    unsigned htsz=givehtsz(lnarr_p->uasz);
    snod **stab = tochainharr(lnarr_p, htsz);
    // prto_stra_t(lnarr_p); // verbose printing of ln array
    prtchaharr(stab, htsz);
    printf("Number of lines in file (including blanks lines if any: %u\n", lnarr_p->uasz); 

    freechainharr(stab, htsz);
    free_stra_t(&lnarr_p);

    return 0;
}
