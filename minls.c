#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char *imagefile, *part, *subpart;
static char *path = "/";


int main(int argc, char **argv) {
   
   handle_args(argc, argv);
   return 0;
}


/* Handles the arguments passed in */
void handle_args(int argc, char **argv) {
   extern char *optarg;
   extern int optid;
   int c, error = 0;
   int pflag = 0, sflag=0, vflag=0;
   static char usage[] = "usage: %s [-v] [-p part [-s subpart]] imagefile [path]";

   while((c = getopt(argc, argv, "vp:s:")) != -1)
      switch(c) {
         case 'v':
            vflag = 1;
            break;
         case 'p':
            pflag = 1;
            part = optarg;
            break;
         case 's':
            sflag = 1;
            subpart = optarg;
            break;
         case '?':
            error = 1;
            break;
      }
   
   /* Make sure imagefile is provided */
   if((optind + 1) > argc) {
      fprintf(stderr, "%s: missing imagefile\n", argv[0]);
      fprintf(stderr, usage, argv[0]);
      exit(1);
   }
   /* Check if subpartition was provided w/o partition */
   else if(sflag && !pflag) {
      fprintf(stderr, "%s: cannot have subpartition without partition\n", argv[0]);
      exit(1);
   }
   /* Check for any invalid flags */
   else if(error) {
      fprintf(stderr, usage, argv[0]);
      exit(1);
   }

   /* First unflagged arg is imagefile */
   if(optind < argc) {
      imagefile = argv[optind];
   }
   /* Second optional unflagged arg is the path */
   if(optind + 1 < argc) {
      path = argv[optind + 1];
   }
}


/* Called if partitioning is requested */
void find_partitions() {
      
}


   
