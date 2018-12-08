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
   struct dirent dir_entry;
   int zone_size, inode_map_size, zone_map_size, blocksize, inode_table_size;
   int bytes_left, zone_bytes_read, current_zone_size, i;
   int inode_table_addr;
   int file_found = 0;
   int path_size;
   int block_index;
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

   /* If verbose, print the useful info from the superblock. */
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

   /* Grab constants from the superblock */
   blocksize = s_block.blocksize;
   zone_size = blocksize << s_block.log_zone_size;
   inode_map_size = s_block.i_blocks * blocksize;
   zone_map_size = s_block.z_blocks * blocksize;
   inode_table_addr = (SUPERBLOCK_ADDR * 2) + inode_map_size + zone_map_size;
   inode_table_size = s_block.ninodes * INODE_SIZE / blocksize;
   block_index = 2+s_block.i_blocks+s_block.z_blocks;

   //TODO print calculated values?
   
   
   printf("location before seek: %ld\n",ftell(imagefile));
   /* Set file position to begining of the inodes */
   if(fseek(imagefile, block_index*blocksize, SEEK_SET) != 0) {
      perror(NULL);
      exit(errno);
   }

   printf("location after seek: %ld\n",ftell(imagefile));


   /* Read in the root inode from the imagefile */
   if(fread(&current_inode, sizeof(struct inode), 1, imagefile) != 1) {
      fprintf(stderr, "Unable to read root inode\n");
      exit(EXIT_FAILURE);
   }
   
   /* If verbose, print useful info from root inode. */
   if(verbose) {
      printf("\n");
      printf("File inode:\n");
      printf("  uint16_t mode           0x%4x ",current_inode.mode);
      printModeBits(current_inode.mode);
      printf("  uint16_t links %15u\n",current_inode.links);
      printf("  uint16_t uid %17u\n",current_inode.uid);
      printf("  uint16_t gid %17u\n",current_inode.gid);
      printf("  uint32_t size %16u\n",current_inode.size);
      printf("  uint32_t atime %15d --- ",current_inode.atime);
      printSecondsToReadable(current_inode.atime);
      printf("  uint32_t mtime %15d --- ",current_inode.mtime);
      printSecondsToReadable(current_inode.mtime);
      printf("  uint32_t ctime %15d --- ",current_inode.ctime);
      printSecondsToReadable(current_inode.ctime);
      printf("\n");
   }


   printf("location after read: %ld\n",ftell(imagefile));
   
   
   if(strncmp(path, "/", 1) == 0) {
      file_found = 1;
   }
   
   while(!file_found) {
      /* TODO: Grab next directory/file in path, save as file_name */

      
   }
   
   /* Current inode is the one we want to list */ 

   /* Check if the inode is for a directory */
   if(current_inode.mode & 0xF000 == 0xF000) {
      bytes_left = current_inode.size;
      /* Loop over all dirints in the directory and print them out */
      while(bytes_left > 63) {
         /* First look in all direct zones for entires */
         for(i = 0; i < DIRECT_ZONES; i++) {
            if(current_inode.zone[i] != 0) {
               /* Seek the file pointer to the correct zone */
               if(fseek(imagefile, (s_block.firstdata + current_inode.zone[i]) * zone_size, SEEK_SET) != 0) {
                  perror(NULL);
                  exit(errno);
               }
               /* Start copying and printing data from zone */
               for(zone_bytes_read = 0; zone_bytes_read < zone_size; zone_bytes_read += 64) {
                  /* Read in dirent and display data */
                  if(fread(&dir_entry, sizeof(struct dirent), 1, imagefile) != 1) {
                     fprintf(stderr, "Unable to read dirent\n");
                     exit(EXIT_FAILURE);
                  }
                  if(dir_entry.inode != 0) 
                     printf("%.60s\n", dir_entry.name);
                  bytes_left -= 64;
                  if(bytes_left <= 0) 
                     return 0;
               }
            }
         }
      }
   }
   /* Check if the inode is for a regular file */
   

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
   
   /* Set file position to begining of superblock */
   if(fseek(imagefile, position, SEEK_SET) != 0) {
      perror(NULL);
      exit(errno);
   }
   
   /* Read in superblock struct from imagefile */
   if(fread(&s_block, sizeof(struct superblock), 1, imagefile) != 1) {
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

   return s_block;
}


/* Prints out time in the form: DayName MonthName Day Hour:Minute:Second Year
 */
void printSecondsToReadable(int32_t time) {

}


/* Prints out mode in tthe form: (drwxrwxrwx) */
void printModeBits(uint16_t mode) {

}





