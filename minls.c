#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "minfs.h"
//#include <fs/super.h>

static char *imagefile_string, *part = NULL, *subpart = NULL;
static char *path = "/";
static int verbose = 0;


int main(int argc, char **argv) {
   FILE *imagefile; 
   struct superblock s_block;
   int zone_size, inode_map_size, zone_map_size, blocksize, inode_table_size;
   int inode_table_addr;
   int file_found = 0;
   int path_size;
   
   fpos_t *pos;
   struct inode current_inode;


   handle_args(argc, argv);
   imagefile = file_open_read(imagefile_string);
   
   /* TODO: Handle file partitions if needed */
   if(part) {
      handle_partitions();
   }
   
   /* For now, assuming there is no partitions */
   
   /* Get the superblock from the imagefile and verify it is a valid MINIX block */
   s_block = validate_superblock(imagefile);
   /* Grab constants from the superblock */
   blocksize = s_block.blocksize;
   zone_size = blocksize << s_block.log_zone_size;
   inode_map_size = s_block.i_blocks * blocksize;
   zone_map_size = s_block.z_blocks * blocksize;
   inode_table_addr = (SUPERBLOCK_ADDR * 2) + inode_map_size + zone_map_size;
   inode_table_size = s_block.ninodes * INODE_SIZE / blocksize;
   
  fprintf(stderr,"inode table addr: %d\n",inode_table_addr);

   printf("location: %ld\n",ftell(imagefile));
   /* Set file position to begining of the inodes */
   if(fseek(imagefile, inode_table_addr, SEEK_SET) != 0) {
      perror(NULL);
      exit(errno);
   }

   printf("location: %ld\n",ftell(imagefile));


   /* Read in the root inode from the imagefile */
   if(fread(&current_inode, sizeof(struct inode), 1, imagefile) != 1) {
      fprintf(stderr, "Unable to read root inode\n");
      exit(EXIT_FAILURE);
   }
   
   printf("location: %ld\n",ftell(imagefile));
   if(strncmp(path, "/", 1) == 0) {
      file_found = 1;
   }
   
   while(!file_found) {
      /* TODO: Grab next directory/file in path, save as file_name */

      
   }


   if(verbose) {
      printf("\n");
      printf("File inode:\n");
      printf("  uint16_t mode           0x%4x\n",current_inode.mode);
      printf("  uint16_t links %15u\n",current_inode.links);
      printf("  uint16_t uid %17u\n",current_inode.uid);
      printf("  uint16_t gid %17u\n",current_inode.gid);
      printf("  uint32_t size %16u\n",current_inode.size);
      printf("  uint32_t atime %15d\n",current_inode.atime);
      printf("  uint32_t mtime %15d\n",current_inode.mtime);
      printf("  uint32_t ctime %15d\n",current_inode.ctime);
      printf("\n");

   }

   
   /* Current inode is the one we want to list */ 

   //TODO Print the listing

   return 0;
}


/* Handles the arguments passed in */
void handle_args(int argc, char **argv) {
   extern char *optarg;
   extern int optind;
   int c, error = 0;
   int pflag = 0, sflag = 0;
   static char usage[] = "usage: %s [-v] [-p part [-s subpart]] imagefile [path]";

   while((c = getopt(argc, argv, "vp:s:")) != -1)
      switch(c) {
         case 'v':
            verbose = 1;
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
      exit(EXIT_FAILURE);
   }
   /* Check if subpartition was provided w/o partition */
   else if(sflag && !pflag) {
      fprintf(stderr, "%s: cannot have subpartition without partition\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   /* Check for any invalid flags */
   else if(error) {
      fprintf(stderr, usage, argv[0]);
      exit(EXIT_FAILURE);
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
   int position = SUPERBLOCK_ADDR;
   size_t temp = 0;
   
   /* Set file position to begining of superblock */
   if(fseek(imagefile, position, SEEK_SET) != 0) {
      perror(NULL);
      exit(errno);
   }
   
   /* Read in superblock struct from imagefile */
   if(temp = fread(&s_block, sizeof(struct superblock), 1, imagefile) != 1) {
      fprintf(stderr, "Unable to read superblock\n");
      //fprintf(stderr, "struct superblock size: %d\n",sizeof(struct superblock));
      //fprintf(stderr, "Read: %d\n",temp);
      exit(EXIT_FAILURE);
   }
   
   /* Verify this is a valid MINIX superblock */
   if(s_block.magic != MINIX_MAGIC) {
      fprintf(stderr, "Not a valid MINIX superblock\n");
      exit(EXIT_FAILURE);
   }

   /* If verbose, print the useful infor. */
   if(verbose) {
      printf("\n");
      printf("Superblock Contents:\n");
      printf("Stored Fields:\n");
      printf("  ninodes %12u\n",s_block.ninodes);
      printf("  i_blocks %11d\n",s_block.i_blocks);
      printf("  z_blocks %11d\n",s_block.z_blocks);
      printf("  firstdata %10u\n",s_block.firstdata);
      printf("  log_zone_size %6d\n",s_block.log_zone_size);
      printf("  max_file %11u\n",s_block.max_file);
      printf("  magic         0x%x\n",s_block.magic);
      printf("  zones %14u\n",s_block.zones);
      printf("  blocksize %10u\n",s_block.blocksize);
      printf("  subversion %9u\n",s_block.subversion);
      printf("\n");
   }
   
   return s_block;
}


   
