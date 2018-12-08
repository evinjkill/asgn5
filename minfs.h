#ifndef MINLS_H
#define MINLS_H

#define SUPERBLOCK_ADDR 1024
#define INODE_SIZE 64
#define MINIX_MAGIC 0x4D5A
#define DIRECT_ZONES 7
#define P_TABLE_LOCATION 0x1BE
#define MINIX_PARTITION_TYPE 0x81
#define TABLE_SIGNATURE 0x55aa
#define SECTOR_SIZE 512

struct partition_table { /* A Minix Partition table */
   
   uint8_t bootind; /* Boot magic number (0x80 if bootable. */
   uint8_t start_head; /* Start of partition in CHS */
   uint8_t start_sec;
   uint8_t start_cyl;
   uint8_t type; /* Type of partition (0x81 for MINIX) */
   uint8_t end_head; /* End of partition in CHS */
   uint8_t end_sec;
   uint8_t end_cyl;
   uint32_t lFirst; /* First LBA sector. */
   uint32_t size; /* Size of partition (in sectors). */
};

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
    uint32_t indirect;
    uint32_t two_indirect;
    uint32_t unused;
};



void handle_args(int argc, char **arg);
FILE *file_open_read(char *filename);
int handle_partitions();
struct superblock validate_superblock(FILE *imagefile);
struct partition_table validate_p_table(FILE *imagefile);
void printSecondsToReadable(int32_t time);
void printModeBits(uint16_t mode);



#endif
