/* pit2f .. pit a fasta file against an isoform.results file */
#include "pit2f.h"

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
        printf("Usage error. Pls supply 2 arguments: 1) fasta file name. 2) isofrms.res file in which you want to look for fasta idlines.\n");
        exit(EXIT_FAILURE);
    }

    /* OK, we goign to read in our fasta first, well, beacuase it's the first argument */
    i_s *sqisz=crea_i_s();
    int numsq, i;
    flpis_t(argv[1], &sqisz, &numsq);
    prti_s(sqisz, numsq);
    /* the summary comes at the end because otherwise, with many sequences, it goes off-screen */
    printf("Num fasta sequences in file: %u.\n", numsq);

    /* OK, now we turn our attention to the isoforms.results file */
    flpua_t *lnarr_p=crea_flpua_t();
    f2flpua_t(argv[1], &lnarr_p);
    prto_flpua_t(lnarr_p);

    /* OK, so both are now in memory */
    printf("\nOK, both files are now in appropriate structs in memory\n\n");




    /* kill the fasta */
    for(i=0;i<numsq;++i) 
        free(sqisz[i].idl);
    free(sqisz);

    /* kill the isoform.res file */
    free_flpua_t(&lnarr_p);

    return 0;
}
