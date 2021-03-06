/* lnarr.c: a text file is input and th array giving the length of each line is output. Preform protoype for strarr_fcl.c
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
    unsigned *ua;
    unsigned uasz;
} ua_bt; /* unsigned array basic type, simply the array with it size, that's all */

void f2ua_bt(char *fname, ua_bt **lnarr_p)
{
    FILE *fin=fopen(fname, "r");
    int c;
    unsigned lnsz=0, lnbuf=LNBUF;
    unsigned lidx=0;
    (*lnarr_p)->ua=realloc((*lnarr_p)->ua, LNBUF*sizeof(unsigned)); /* pointer to a line array, packaged as a ptr to ua_bt */
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


ua_bt *crea_ua_bt(void)
{
    ua_bt *lnarr_p=malloc(sizeof(ua_bt)); /* because it starts as NULL: pointer to a line array, packaged as a ptr to ua_bt */
    lnarr_p->ua=NULL; /* valgrind very picky about initializations */
    return lnarr_p;
}

void prto_ua_bt(ua_bt *lnarr_p)
{
    unsigned j;
    printf("Input has %u lines, per line chars is:\n", lnarr_p->uasz); 
    for(j=0;j<lnarr_p->uasz;++j) 
        printf((j==lnarr_p->uasz-1)? "%u\n" : "%u ", lnarr_p->ua[j]);
    return;
}

void free_ua_bt(ua_bt **lnarr_p)
{
    free((*lnarr_p)->ua);
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
    ua_bt *lnarr_p=crea_ua_bt();
    f2ua_bt(argv[1], &lnarr_p);
    prto_ua_bt(lnarr_p);

    free_ua_bt(&lnarr_p);

    return 0;
}
