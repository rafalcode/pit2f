/* pit2fwh.c .. pit a fasta file against an isoform.results file
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
#include "pit2f.h"

void usage(void)
{
    printf("pit2fwh is a program to search for fasta id's in an RSEM isoforms.results file\n");
    printf("The mnemonic for the program name is Pit Two Files With Hash\" because it matches up sequence ID lines\n");
    printf("in the (first) fasta file file, against the RSEM found isoforms represented as single lines in the second file\n");
    printf("To use properly, the first argument must be the filename for the fasta file located in the current directory\n");
    printf("The second argument should be the isoforms.results file. Note that the output is sent to STOUT, or screen,\n");
    printf("so it is up to the user to catch it properly\n");
    return;
}

typedef struct /* i_s */
{
    char *idl; /* id line */
    unsigned idlsz; /* size of id line */
    unsigned tsz; /* because summing over the ambiguity characters will include the double and tripled nature of some of them */
    unsigned sy[SSZ];
} i_s; /* sequence index and number of symbols */

typedef struct /*flpua_t */
{
    char *fl;
    char **stra0, **stra1; /* the string array, size of each string will be one less than ua */
    unsigned *ua0, *ua1;
    unsigned uasz, flsz;
} flpua_t; /* First Line Plus Unsigned Array Type */

struct strchainode
{
    i_s *sqip; /* like sqisz, but a ptr to just one single element, not an array */
    struct strchainode *n;
};
typedef struct strchainode snod;

unsigned hashit(char *str, unsigned tsz) /* Dan Bernstein's one */
{
    unsigned long hash = 5381;
    int c;

    char *tstr=str;
    while ((c = *tstr++))
        hash = ((hash << 5) + hash) + c; /*  hash * 33 + c */

    return hash % tsz;
}

snod **tochainharr(i_s *sqisz, unsigned numsq, unsigned tsz)
{
    unsigned i;

    snod **stab=malloc(tsz*sizeof(snod *));
    for(i=0;i<tsz;++i) 
        stab[i]=NULL; /* _is_ a valid ptr, but it's unallocated. Initialization is possible though. */
    snod *tsnod0, *tsnod2;

    unsigned tint;
    for(i=0; i<numsq; ++i) {
        tint=hashit(sqisz[i].idl, tsz);
        if( (stab[tint] == NULL) ) {
            stab[tint]=malloc(sizeof(snod));
            stab[tint]->sqip=sqisz+i;
            stab[tint]->n=NULL;
            continue;
        }
        tsnod2=stab[tint];
        while( (tsnod2 != NULL) ){
            if(!strcmp(tsnod2->sqip->idl, sqisz[i].idl)) {
                goto nxt;
            }
            tsnod0=tsnod2;
            tsnod2=tsnod2->n;
        }
        tsnod0->n=malloc(sizeof(snod));
        tsnod0->n->sqip=sqisz+i;
        tsnod0->n->n=NULL;
nxt:        continue;
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
            printf("\"%s\" ", tsnod2->sqip->idl); 
            tsnod2=tsnod2->n;
        }
        printf("\n"); 
    }
    return;
}

char idlsearch(snod **stab, unsigned tsz, char *line, unsigned lnsz)
{
    char yes=0;
    size_t i;
    snod *tsnod2;

    unsigned tint;
    for(i=0; i<tsz; ++i) {
        tint=hashit(line, tsz);
        if( (stab[tint] == NULL) )
            goto nxt; /* hashtable at that position is empty ... so it's impossible for that string to be there */

        tsnod2=stab[tint];
        while( (tsnod2 != NULL) ) {
            if( (tsnod2->sqip->idlsz == lnsz) & !(strncmp(tsnod2->sqip->idl, line, tsnod2->sqip->idlsz)) ) {
                yes=1; /* i.e. no=0 */
                goto nxt;
            }
            tsnod2=tsnod2->n;
        }
    }
nxt:    return yes;
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
    int gbuf=GBUF, idlbuf=GSTRBUF;
    i_s *sqisz=malloc(gbuf*sizeof(i_s));
    for(i=0;i<gbuf;++i) 
        sqisz[i].idl=calloc(idlbuf, sizeof(char));
    return sqisz;
}

void flpis_t(char *fname, i_s **sqisz, unsigned *numsq)
{
    FILE *fin;
    if(!(fin=fopen(fname, "r")) ) {
        printf("Error. Cannot open the presented filename\n");
        exit(EXIT_FAILURE);
    }

    unsigned char idline=0, begline=1; /* booleans */
    unsigned lidx=0, idcidx=0 /*charidx just for the title line */;
    int i, c, sqidx=-1; /* this is slightly dangerous, you need very much to knwo what you're doing */
    int gbuf=GBUF, idlbuf=GSTRBUF;

    while( ( (c = fgetc(fin)) != EOF) ) {
        if(c =='\n') {
            if(idline) {
                (*sqisz)[sqidx].idl[idcidx]='\0';
                (*sqisz)[sqidx].idlsz=idcidx;
                (*sqisz)[sqidx].idl=realloc((*sqisz)[sqidx].idl, ((*sqisz)[sqidx].idlsz+1)*sizeof(char));
            }
            idline=0;
            idcidx=0;
            idlbuf=GSTRBUF;
            lidx++;
            begline=1;
        } else if(c == '>') {
            if(begline) {
                idline =1;
                begline=0; 
                sqidx++;
                if(sqidx==gbuf) {
                    gbuf+=GBUF;
                    (*sqisz)=realloc((*sqisz), gbuf*sizeof(i_s));
                    for(i=gbuf-GBUF;i<gbuf;++i) 
                        (*sqisz)[i].idl=calloc(GSTRBUF, sizeof(char));
                }
                (*sqisz)[sqidx].tsz=0;
                for(i=0;i<SSZ;++i)
                    (*sqisz)[sqidx].sy[i]=0;
            }
            // // do we want to have the > in the sequence? Prob not, for matching.
            // else
            // (*sqisz)[sqidx].idl[idcidx]=c;
        } else if (idline) { /* we won't store the sequence, just get symbol counts */
            CONDREALLOCP(idcidx, idlbuf, GSTRBUF, (*sqisz)[sqidx].idl);
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
                    if( (c==CTOK0) | (c==CTOK2) ) { /* finish up our "0" counterparts */
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
        printf("l%3u:\"%s\" (#c %u) |%s\n", j, lnarr_p->stra0[j], lnarr_p->ua0[j], lnarr_p->stra1[j]);
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
    if(argc!=3) {
        usage();
        if(argc==2)
           if( !strncmp(argv[1], "-h", 2) | !strncmp(argv[1], "--help", 6) )
            exit(EXIT_SUCCESS);
        exit(EXIT_FAILURE);
    }

    /* OK, we going to read in our fasta first, well, beacuase it's the first argument */
    i_s *sqisz=crea_i_s();
    int i, j;
    unsigned numsq;
    flpis_t(argv[1], &sqisz, &numsq);
    unsigned htsz=2*numsq/3; /* our hash table size */
    snod **stab=tochainharr(sqisz, numsq, htsz); /* we now set up a hash table along side our sequence names from the fasta file */
#ifdef DEBUG
    prti_s(sqisz, numsq);
    /* the summary comes at the end because otherwise, with many sequences, it goes off-screen */
    printf("Num fasta sequences in file: %u.\n", numsq);
#endif

    /* OK, now we turn our attention to the isoforms.results file */
    flpua_t *lnarr_p=crea_flpua_t();
    f2flpua_t(argv[2], &lnarr_p);
#ifdef DEBUG
    prto_flpua_t(lnarr_p);
    printf("\nOK, both files are now in appropriate structs in memory\n\n");
    /* the following line relegated to DEBUG version only ... the user isn't interested. */
    printf("_Output Description_: 2nd file lines (#l:%u) whose 1st _word_ matches with any sequence id line in 1st (fasta) file (#seqs:%u).\n", lnarr_p->uasz+1, numsq);
#endif

    for(j=0;j<lnarr_p->uasz;++j)  {/* inner loop over isofrm.res */
        if( (idlsearch(stab, htsz, lnarr_p->stra0[j], lnarr_p->ua0[j]) ) )
            printf("%s%s\n", lnarr_p->stra0[j], lnarr_p->stra1[j]);
#ifdef YESUNIQUE
        break;
#endif
    }

    freechainharr(stab, htsz);
    /* kill the fasta */
    for(i=0;i<numsq;++i) 
        free(sqisz[i].idl);
    free(sqisz);

    /* kill the isoform.res file */
    free_flpua_t(&lnarr_p);

    return 0;
}
