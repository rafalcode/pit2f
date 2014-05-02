/* this is the most basic line number array program for a single input file
 * it is the bulding block for many more specific line readers */
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
    unsigned j;

    /* convert this file into a line array */
   ua_bt *lnarr_p=malloc(sizeof(ua_bt)); /* because it starts as NULL: pointer to a line array, packaged as a ptr to ua_bt */
   lnarr_p->ua=NULL; /* valgrind very pocky about initiaizations */
   f2ua_bt(argv[1], &lnarr_p);

   printf("Input has %u lines, per line chars is:\n", lnarr_p->uasz); 
   for(j=0;j<lnarr_p->uasz;++j) 
       printf("%u ", lnarr_p->ua[j]);
   printf("\n"); 

   free_ua_bt(&lnarr_p);

   return 0;
}
