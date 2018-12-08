#ifndef MINLS_H
#define MINLS_H

#define SUPERBLOCK_ADDR 1024
#define INODE_SIZE 64
#define MINIX_MAGIC 0x4D5A
#define DIRECT_ZONES 7


struct superblock { /* Minix Version 3 Superblock from fs/super.h */

    uint32_t ninodes; /* number of inodes in the fs. */
    uint16_t pad1; /* padding */
    int16_t i_blocks; /* number of blocks used by inode bit map */
    int16_t z_blocks; /* number of blocks used by the zone bit map */
    uint16_t firstdata; /* number of first data zone */
    int16_t log_zone_size; /* log2 of blocks per zone */
    int16_t pad2; /* padding */
    uint32_t max_file; /* maximum file size */
    uint32_t zones; /* number of zones on disk */
    int16_t magic; /* magic number */
    int16_t pad3; /* padding */
    uint16_t blocksize; /* block size in bytes */
    uint8_t subversion; /* fs sub-version */
};

struct inode { /* Sucture of a minix inode on disk */

    uint16_t mode;
    uint16_t links;
    uint16_t uid;
    uint16_t gid;
    uint32_t size;
    int32_t atime;
    int32_t mtime;
    int32_t ctime;
    uint32_t zone[DIRECT_ZONES];
    uint32_t indrect;
    uint32_t two_indirect;
    uint32_t unused;
};



void handle_args(int argc, char **arg);
FILE *file_open_read(char *filename);
void handle_partitions();
struct superblock validate_superblock(FILE *imagefile);

#endif
