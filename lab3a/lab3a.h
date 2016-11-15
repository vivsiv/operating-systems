#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>


// RELEVANT SUPERBLOCK DECLARATIONS
const int SB_OFFSET = 1024;
const int SB_SIZE = 1024;

const int SB_INODES_COUNT_OFFSET = 0;
const int SB_BLOCKS_COUNT_OFFSET = 4;
const int SB_FIRST_DATA_BLOCK_OFFSET = 20;
const int SB_BLOCK_SIZE_OFFSET = 24;
const int SB_FRAGMENT_SIZE_OFFSET = 28;
const int SB_BLOCKS_PER_GROUP_OFFSET = 32;
const int SB_FRAGMENTS_PER_GROUP_OFFSET = 36;
const int SB_INODES_PER_GROUP_OFFSET = 40;
const int SB_MAGIC_NUMBER_OFFSET = 56;

const uint16_t EXT2_SUPER_MAGIC = 0xEF53;

typedef struct superblock_info {
	uint32_t inodes_count;
	uint32_t blocks_count;
	uint32_t first_data_block;
	unsigned long block_size;
	long fragment_size;
	uint32_t blocks_per_group;
	uint32_t fragments_per_group;
	uint32_t inodes_per_group;
	uint16_t magic_number;
} SBInfo;


//RELEVANT BLOCK GROUP DESCRIPTOR DECLARATIONS
const int BGD_SIZE = 32;

const int BGD_BLOCK_BITMAP_OFFSET = 0;
const int BGD_INODE_BITMAP_OFFSET = 4;
const int BGD_INODE_TABLE_OFFSET = 8;
const int BGD_FREE_BLOCKS_COUNT_OFFSET = 12;
const int BGD_FREE_INODES_COUNT_OFFSET = 14;
const int BGD_USED_DIRS_COUNT_OFFSET = 16;

typedef struct group_descriptor_info {
	int group_num;
	uint32_t block_bitmap;
	uint32_t inode_bitmap;
	uint32_t inode_table;
	uint16_t free_blocks_count;
	uint16_t free_inodes_count;
	uint16_t used_dirs_count;
} GDInfo;

//RELEVANT BITMAP DECLARATIONS
const int BLOCK_BITMAP_TYPE = 0;
const int INODE_BITMAP_TYPE = 1;

