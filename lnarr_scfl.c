/* lnarr_scfl.c builds on lnarr.c by merely slupring the first line into memory, and only counting the length of the rest of the lines. Prototyoe for strarr_fcl.c
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
    if((x)==((b)-2)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(char)); \
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
    unsigned uasz, flsz;
} flpua_t; /* First Line Plus Unsigned Array Type */

void f2flpua_t(char *fname, flpua_t **lnarr_p)
{
    FILE *fin=fopen(fname, "r");
    int c;
    unsigned lnsz=0, lnbuf=LNBUF, strbuf=GSTRBUF, lidx=0;
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, lnbuf*sizeof(unsigned)); /* pointer to a line array, packaged as a ptr to flpua_t */
    (*lnarr_p)->fl=realloc((*lnarr_p)->fl, strbuf*sizeof(char));
    memset((*lnarr_p)->ua, 0, lnbuf*sizeof(unsigned));
    memset((*lnarr_p)->fl, '\0', strbuf*sizeof(char));

    for(;;) {
        c=fgetc(fin);
        if(c == EOF) break;
        if(c == '\n') {
            if(lidx !=0) {
                CONDREALLOD(lidx-1, lnbuf, LNBUF, (*lnarr_p)->ua, unsigned);
                (*lnarr_p)->ua[lidx-1]=lnsz;
            } else {
                (*lnarr_p)->fl[lnsz]='\0';
                (*lnarr_p)->flsz=lnsz;
            }
            lidx++;
            lnsz=0;
        } else {
            if(lidx ==0) {
                CONDREALLOCP(lnsz, strbuf, GSTRBUF, (*lnarr_p)->fl);
                (*lnarr_p)->fl[lnsz]=c;
            }
            lnsz++;
        }
    }
    (*lnarr_p)->uasz=lidx-1;
    (*lnarr_p)->fl=realloc((*lnarr_p)->fl, ((*lnarr_p)->flsz+1)*sizeof(char)); /* the plus one is because flsz leaves out nullchar */
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, (*lnarr_p)->uasz*sizeof(unsigned));

    fclose(fin);
    return;
}

flpua_t *crea_flpua_t(void)
{
    flpua_t *lnarr_p=malloc(sizeof(flpua_t));
    lnarr_p->ua=NULL; /* valgrind very picky about initializations */
    lnarr_p->fl=NULL;
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
