/* rdfasta.c: a multifasta summarizer iof mine repurposed to a read & store fasta idlines. merely a protype for pit2f.c's 1st arg
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

#define GBUF 4
#define SSZ 5 /* DNA symbol alphabet size is 4 ? Of course, but the fifth is for N's which sometimes appear, and the sixth is for anything elses */

/* quick macro for conditionally enlarging a char pointer, space always available for final null char */
#define CONDREALLOCP(x, b, c, a); \
    if((x)==((b)-2)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(char)); \
    }

typedef struct /* i_s */
{
    char *idl; /* id line */
    unsigned idlsz; /* size of id line */
    unsigned tsz; /* because summing over the ambiguity characters will include the double and tripled nature of some of them */
    unsigned sy[SSZ];
} i_s; /* sequence index and number of symbols */

void prti_s(i_s *sqisz, int sz)
{
    int i;
    for(i=0;i<sz;++i) {
        printf("Seq: %u IDL: \"%s\" (#c %u) ", i, sqisz[i].idl, sqisz[i].idlsz); 
        printf("SYMS: TOT=%u/A=%u/C=%u/G=%u/T=%u/?:%u\n", sqisz[i].tsz, sqisz[i].sy[0], sqisz[i].sy[1], sqisz[i].sy[2], sqisz[i].sy[3], sqisz[i].sy[4]);
    }
}

i_s *crea_i_s(void)
{
    int i;
    int gbuf=GBUF, idlbuf=GBUF;
    i_s *sqisz=malloc(gbuf*sizeof(i_s));
    for(i=0;i<gbuf;++i) 
        sqisz[i].idl=calloc(idlbuf, sizeof(char));
    return sqisz;
}

void flpis_t(char *fname, i_s **sqisz, int *numsq)
{
    FILE *fin;
    if(!(fin=fopen(fname, "r")) ) {
        printf("Error. Cannot open the presented filename\n");
        exit(EXIT_FAILURE);
    }

    unsigned char idline=0, begline=1; /* booleans */
    unsigned lidx=0, idcidx=0 /*charidx just for the title line */;
    int i, c, sqidx=-1; /* this is slightly dangerous, you need very much to knwo what you're doing */
    int gbuf=GBUF, idlbuf=GBUF;

    while( ( (c = fgetc(fin)) != EOF) ) {
        if(c =='\n') {
            if(idline) {
                (*sqisz)[sqidx].idl[idcidx]='\0';
                (*sqisz)[sqidx].idlsz=idcidx;
                (*sqisz)[sqidx].idl=realloc((*sqisz)[sqidx].idl, ((*sqisz)[sqidx].idlsz+1)*sizeof(char));
            }
            idline=0;
            idcidx=0;
            idlbuf=GBUF;
            lidx++;
            begline=1;
        } else if(c == '>') {
            if(begline) {
                idline =1;
                begline=0; 
                sqidx++;
                if(sqidx==gbuf) {
                    gbuf+=GBUF;
                    sqisz=realloc(sqisz, gbuf*sizeof(i_s));
                }
                (*sqisz)[sqidx].tsz=0;
                for(i=0;i<SSZ;++i)
                    (*sqisz)[sqidx].sy[i]=0;
            }
            // // do we want to have the > in the sequence? Prob not, for matching.
            // else
            // (*sqisz)[sqidx].idl[idcidx]=c;
        } else if (idline) { /* we won't store the sequence, just get symbol counts */
            CONDREALLOCP(idcidx, idlbuf, GBUF, (*sqisz)[sqidx].idl);
            (*sqisz)[sqidx].idl[idcidx]=c;
            idcidx++;
        } else if(!idline) { /* we won't store the sequence, just get symbol counts */
            (*sqisz)[sqidx].tsz++;
            switch(c) {
                case 'A': case 'a':
                    (*sqisz)[sqidx].sy[0]++; break;
                case 'C': case 'c':
                    (*sqisz)[sqidx].sy[1]++; break;
                case 'G': case 'g':
                    (*sqisz)[sqidx].sy[2]++; break;
                case 'T': case 't':
                    (*sqisz)[sqidx].sy[3]++; break;
                default:
                    (*sqisz)[sqidx].sy[4]++;
            }
        }
    }
    fclose(fin);
    *numsq=sqidx+1;
    for(i=*numsq;i<gbuf;++i) 
        free((*sqisz)[i].idl);
    (*sqisz)=realloc((*sqisz), (*numsq)*sizeof(i_s));

    return;
}

int main(int argc, char *argv[])
{
    /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
    if(argc!=2) {
        printf("Error. Pls supply 1 argument:; fasta file name.\n");
        exit(EXIT_FAILURE);
    }

    i_s *sqisz=crea_i_s();
    int numsq, i;
    flpis_t(argv[1], &sqisz, &numsq);
    prti_s(sqisz, numsq);
    /* the summary comes at the end because otherwise, with many sequences, it goes off-screen */
    printf("Num fasta sequences in file: %u.\n", numsq);

    for(i=0;i<numsq;++i) 
        free(sqisz[i].idl);
    free(sqisz);
    return 0;
}
