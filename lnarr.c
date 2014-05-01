/* this is the most basic line number array program for a single input file
 * it is the bulding block for many more speicifc line readers */
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
        memset(((a)+(b)-(c)), 0, ((c)*(sizeof(t)))); \
    }

int main(int argc, char *argv[])
{
   /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
	if(argc!=2) {
		printf("Error. Pls supply argument (name of file).\n");
		exit(EXIT_FAILURE);
	}

   FILE *fin=fopen(argv[1], "r");
   int c;
   unsigned lnsz=0, totlines, j, lnbuf=LNBUF;
   unsigned lidx=0, *lnarr=malloc(lnbuf*sizeof(unsigned));
   memset(lnarr, 0, lnbuf*sizeof(unsigned));

   for(;;) {
        c=fgetc(fin);
        if(c == EOF) break;
        if(c == '\n') {
            CONDREALLOD(lidx, lnbuf, LNBUF, lnarr, unsigned);
            lnarr[lidx++]=lnsz;
            lnsz=0;
        } else
            lnsz++;
   }
   totlines=lidx;
   lnarr=realloc(lnarr, totlines*sizeof(unsigned));

   printf("Input has %u lines, per line chars is:\n", totlines); 
   for(j=0;j<totlines;++j) 
       printf("%u ", lnarr[j]);
   printf("\n"); 

   free(lnarr);
   fclose(fin);

   return 0;
}
