#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fs/super.h>

static char *imagefile_string, *part = NULL, *subpart = NULL;
static char *path = "/";


int main(int argc, char **argv) {
   FILE *imagefile; 
   struct superblock s_block;
   int zone_size, inode_map_size, zone_map_size, blocksize, inode_table_size;
   fpos_t inode_table_addr;
   int path_size;
   struct inode current_inode;
   handle_args(argc, argv);
   imagefile = file_open_read(imagefile_string);
   
   /* TODO: Handle file partitions if needed */
   if(part)
      handle_partitions();
   
   /* For now, assuming there is no partitions */
   
   /* Get the superblock from the imagefile and verify it is a valid MINIX block */
   s_block = validate_superblock(imagefile);
   /* Grab constants from the superblock */
   blocksize = s_blok.blocksize;
   zone_size = blocksize << s_block.log_zone_size;
   inode_map_size = s_block.i_blocks * blocksize;
   zone_map_size = s_block.z_blocks * blocksize;
   inode_table_addr = (SUPERBLOCK_ADDR * 2) + inode_map_size + zone_map_size;
   inode_table_size = s_block.ninodes * INODE_SIZE / blocksize;
   
   
   /* Set file position to begining of the inodes */
   if(fsetpos(imagefile, &inode_table_addr) != 0) {
      perror(NULL);
      exit(errno);
   }
   
   /* Read in the root inode from the imagefile */
   if(fread(&current_inode, sizeof(struct inode), 1, imagefile) != sizeof(struct inode)) {
      fprintf(stderr, "Unable to read root inode\n");
      exit(EXIT_FAILURE);
   }
   
   if(strncmp(path, "/", 1) == 0) {
      file_found = 1;
   }
   
   while(!file_found) {
      /* TODO: Grab next directory/file in path, save as file_name */

      
   }
   
   /* Current inode is the one we want to list */ 

   //
      
   return 0;
}


/* Handles the arguments passed in */
void handle_args(int argc, char **argv) {
   extern char *optarg;
   extern int optind;
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
   if(s_block.magic != MINIX_MAGIC) {
      fprintf(stderr, "Not a valid MINIX superblock\n");
      exit(-1);
   }
   
   return s_block;
}


   
