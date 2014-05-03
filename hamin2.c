/*  just focusing on doing a simple hash function with
 *  simple function to read in a file ignoring hashes 
 *  of course it's meant to read variablesrbut this
 *  started to get into has htabels and stuff and I ran out of time */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRPSZ 4
#define ELS 7
#define TSZ 13

struct strchainode
{
    char *s;
    struct strchainode *n;
};
typedef struct strchainode snod;

unsigned int hashit(char *str, unsigned int tsz) /* I've got a feeling that might be Dan Bernstein's one */
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /*  hash * 33 + c */

    return hash % tsz;
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
                free(tsnod0->n->s);
                free(tsnod0->n);
                tsnod0->n=NULL;
            }
            free(stab[i]->s);
            free(stab[i]);
            printf("%ith element freed\n", i); 
        }
    }
    free(stab);
    return;
}

int main(int argc, char *argv[])
{
    int i;
    /*  now build thre hash table to see what we're working with */
    char *vnames[ELS]={"abcc", "abbc", "caab", "cbcc", "cacc", "cccc", "cbbb"};
    /*  we're goign to be dealig with some pretty big number */

    unsigned int tsz=TSZ;
    snod **stab=malloc(tsz*sizeof(snod *));
    for(i=0;i<tsz;++i) 
        stab[i]=NULL; /* _is_ a valid ptr, but it's unallocated. Initialization is possible though. */
    snod *tsnod0, *tsnod2;

    unsigned int tint;
    for(i=0; i<ELS; ++i) {
        tint=hashit(vnames[i], tsz);
        if( (stab[tint] == NULL) ) {
            stab[tint]=malloc(sizeof(snod));
            stab[tint]->s=malloc((GRPSZ+1)*sizeof(char));
            stab[tint]->n=NULL;
            strcpy(stab[tint]->s, vnames[i]);
            continue;
        }
        tsnod2=stab[tint];
        while( (tsnod2 != NULL) ){
            tsnod0=tsnod2;
            tsnod2=tsnod2->n;
        }
        tsnod0->n=malloc(sizeof(snod));
        tsnod0->n->s=malloc((GRPSZ+1)*sizeof(char));
        tsnod0->n->n=NULL;
        strcpy(tsnod0->n->s, vnames[i]);
    }

    for(i=0;i<tsz;++i) {
        printf("%i: ", i); 
        tsnod2=stab[i];
        while(tsnod2) {
            printf("%s ", tsnod2->s); 
            tsnod2=tsnod2->n;
        }
        printf("\n"); 
    }

    freechainharr(stab, tsz);

    return 0;
}
