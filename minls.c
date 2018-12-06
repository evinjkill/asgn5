#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fs/super.h>

static char *imagefile_string, *part = NULL, *subpart == NULL;
static char *path = "/";


int main(int argc, char **argv) {
   FILE *fp; 
   struct superblock s_block;
   handle_args(argc, argv);
   imagefile = file_open_read(imagefile);
   
   /* TODO: Handle file partitions if needed */
   if(part)
      handle_partitions();
   
   /* For now, assuming there is no partitions */
   
   s_block = validate_superblock(imagefile);
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
   if(optind < argc)
      imagefile_string = argv[optind];
   
   /* Second optional unflagged arg is the path */
   if(optind + 1 < argc)
      path = argv[optind + 1];
}


FILE *file_open_read(char *filename) {
   FILE *fp;
   if(!(fp = fopen(filename, "r"))) {
      perror(NULL);
      exit(errno);
   }
   return fp;
}

   
/* Called if partitioning is requested */
void handle_partitions() {
      
}


struct superblock validate_superblock(FILE *imagefile) {
   struct superblock s_block;
   fpos_t position = SUPERBLOCK_ADDR;
   
   /* Set file position to begining of superblock */
   if(fsetpos(imagefile, &position) != 0) {
      perror(NULL);
      exit(errno);
   }
   
   /* Read in superblock struct from imagefile */
   if(fread(&s_block, sizeof(struct superblock), 1, imagefile) != sizeof(struct superblock)) {
      fprintf(stderr, "Unable to read superblock\n");
      exit(-1);
   }
   
   /* Verify this is a valid MINIX superblock */
   if(s_block.magic != 0x4D5A) {
      fprintf(stderr, "Not a valid MINIX superblock\n");
      exit(-1);
   }
   
   return s_block;
}


   
