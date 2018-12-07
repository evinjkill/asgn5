#ifndef MINLS_H
#define MINLS_H

#define SUPERBLOCK_ADDR 1024
#define INODE_SIZE 64
#define MINIX_MAGIC 0x4D5A

void handle_args(int argc, char **arg);
FILE *file_open_read(char *filename);
void handle_partitions();
struct superblock validate_superblock(FILE *imagefile);

#endif
