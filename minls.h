#ifndef MINLS_H
#define MINLS_H

#define SUPERBLOCK_ADDR 1024

void handle_args(int argc, char **arg);
FILE *file_open_read(char *filename);
void handle_partitions();
struct superblock validate_superblock(FILE *imagefile);

#endif