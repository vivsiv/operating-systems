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
struct ext2_group_desc
{
	uint32_t	bg_block_bitmap;		/* Blocks bitmap block */
	uint32_t	bg_inode_bitmap;		/* Inodes bitmap block */
	uint32_t	bg_inode_table;		/* Inodes table block */
	uint16_t	bg_free_blocks_count;	/* Free blocks count */
	uint16_t	bg_free_inodes_count;	/* Free inodes count */
	uint16_t	bg_used_dirs_count;	/* Directories count */
	uint16_t	bg_pad;
	uint32_t	bg_reserved[3];
};
const int BGD_SIZE = sizeof(struct ext2_group_desc);

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

//RELEVANT INODE DECLARATIONS
struct ext2_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint32_t i_osd2[3];
};

const int IN_SIZE = sizeof(struct ext2_inode);
const int IN_BLOCK_POINTERS_COUNT = 15;

const int IN_MODE_OFFSET = 0;
const int IN_OWNER_OFFSET = 2;
const int IN_FILE_SIZE_OFFSET = 4;
const int IN_ACCESS_TIME_OFFSET = 8;
const int IN_CREATION_TIME_OFFSET = 12;
const int IN_MODIFICATION_TIME_OFFSET = 16;
const int IN_GROUP_OFFSET = 24;
const int IN_LINKS_COUNT_OFFSET = 26;
const int IN_NUMBER_OF_BLOCKS_OFFSET = 28;
const int IN_BLOCK_POINTERS_OFFSET = 40;

const int IN_MODE_REG_FILE = 0x8000;
const int IN_MODE_DIR = 0x4000;
const int IN_MODE_SYM_LINK = 0xA000;
const int IN_MODE_MASK = 0xF000;


//RELEVANT DIRECTORY DECLARATIONS
struct ext2_directory {
    uint32_t  inode;
    uint16_t  rec_len;
    uint8_t   name_len;
    uint8_t   file_type;
    char[255] name;
};

const int DIR_SIZE = sizeof(struct ext2_dir);

const int DIR_PARENT_INODE_NUMBER_OFFSET = 0;
const int DIR_ENTRY_LENGTH_OFFSET = 4;
const int DIR_NAME_LENGTH_OFFSET = 6;
const int DIR_FILE_TYPE_OFFSET = 7;
const int DIR_NAME_OFFSET = 8;




