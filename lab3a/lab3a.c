#include "lab3a.h"

void error(char *msg){
	fprintf(stderr, "%s\n", msg);
	exit(1);
}


void read_super_block(int disk_fd, SBInfo *sb){
	ssize_t bytes_read;

	char *read_buf = (char *) malloc(SB_SIZE);

	bytes_read = pread(disk_fd, (void *)read_buf, SB_SIZE, SB_OFFSET);
	if (bytes_read < SB_SIZE) error("pread superblock");

	//32 bit value indicating the total number of inodes, both used and free, in the file system
	sb->inodes_count = *(uint32_t *)(read_buf + SB_INODES_COUNT_OFFSET);

	//32 bit value indicating the total number of blocks in the system including all used, free and reserved
	sb->blocks_count = *(uint32_t *)(read_buf + SB_BLOCKS_COUNT_OFFSET);
	//TODO: sanity check for block count
	// Superblock - invalid block count 200000 > image size 50000

	//32 bit value identifying the first data block, in other word the id of the block containing the superblock structure.
	sb->first_data_block = *(uint32_t *)(read_buf + SB_FIRST_DATA_BLOCK_OFFSET);
	//TODO: Sanity check for first data block
	// Superblock - invalid first block 100000 > image size 50000

	//The block size is computed using this 32bit value as the number of bits to shift left the value 1024
	uint32_t block_size_shift = *(uint32_t *)(read_buf + SB_BLOCK_SIZE_OFFSET);
	sb->block_size = 1024 << block_size_shift;
	if (sb->block_size < 512 || sb->block_size > 64000 || sb->block_size % 2 != 0){
		fprintf(stderr, "Superblock - invalid block size: %lu\n", sb->block_size);
		exit(1);
	}

	//The fragment size is computed using this 32bit value as the number of bits to shift left the value 1024. Note that a negative value would shift the bit right rather than left.
	int32_t fragment_size_shift = *(int32_t *)(read_buf + SB_FRAGMENT_SIZE_OFFSET);
	if (fragment_size_shift > 0) sb->fragment_size = 1024 << fragment_size_shift;
	else sb->fragment_size = 1024 >> (-fragment_size_shift);

	//32 bit value indicating the total number of blocks per group
	sb->blocks_per_group = *(uint32_t *)(read_buf + SB_BLOCKS_PER_GROUP_OFFSET);
	if (sb->blocks_count % sb->blocks_per_group != 0){
		fprintf(stderr, "Superblock - %u blocks, %u blocks/group\n", sb->blocks_count, sb->blocks_per_group);
		exit(1);
	}

	//32 bit value indicating the total number of fragments per group
	sb->fragments_per_group = *(uint32_t *)(read_buf + SB_FRAGMENTS_PER_GROUP_OFFSET);

	//32 bit value indicating the total number of inodes per group
	sb->inodes_per_group = *(uint32_t *)(read_buf + SB_INODES_PER_GROUP_OFFSET);
	if (sb->inodes_count % sb->inodes_per_group != 0){
		fprintf(stderr, "Superblock - %u inodes, %u inodes/group\n", sb->inodes_count, sb->inodes_per_group);
		exit(1);
	}

	//16 bit value identifying the file system as Ext2. The value is currently fixed to EXT2_SUPER_MAGIC of value 0xEF53.
	sb->magic_number = *(uint16_t *)(read_buf + SB_MAGIC_NUMBER_OFFSET);
	if (sb->magic_number != EXT2_SUPER_MAGIC) {
		fprintf(stderr, "Superblock - invalid magic: %x\n", sb->magic_number);
		exit(1);
	}


	FILE *sb_csv = fopen("super.csv", "a");
	if (sb_csv == NULL) error("Opening CSV file");
	fprintf(sb_csv, "%x, %u, %u, %lu, %ld, %u, %u, %u, %u\n",
		sb->magic_number,
		sb->inodes_count,
		sb->blocks_count,
		sb->block_size,
		sb->fragment_size,
		sb->blocks_per_group,
		sb->inodes_per_group,
		sb->fragments_per_group,
		sb->first_data_block
	);
	fclose(sb_csv);
	free(read_buf);
}

void read_bitmaps(int disk_fd, const int group_num, uint32_t bitmap_block_num, const SBInfo *sb, const int bitmap_type){
	FILE *bitmap_csv = fopen("bitmap.csv", "a");
	if (bitmap_csv == NULL) error("Opening CSV file");

	ssize_t bytes_read;
	char *read_buf = (char *) malloc(sb->block_size);
	off_t bitmap_offset = bitmap_block_num * sb->block_size;
	bytes_read = pread(disk_fd, (void *)read_buf, sb->block_size, bitmap_offset);
	if (bytes_read < sb->block_size) error("pread bitmap");

	int block_number;
	//start counting blocks based on which group number this is
	if (bitmap_type == BLOCK_BITMAP_TYPE) block_number = (group_num * sb->blocks_per_group) + 1;
	else block_number = (group_num * sb->inodes_per_group) + 1;

	//Each bit represent the current state of a block within that block group, where 1 means "used" and 0 "free/available". 
	//The first block of this block group is represented by bit 0 of byte 0, the second by bit 1 of byte 0. 
	//The 8th block is represented by bit 7 (most significant bit) of byte 0 while the 9th block is represented by bit 0 (least significant bit) of byte 1.
	for (int i = 0; i < sb->block_size; i++){
		//grabbing 8 bits of the bitmap
		uint8_t bitmap_byte = *(uint8_t *)(read_buf + i);
		// fprintf(stdout, "bitmap byte %d is %x\n", i, bitmap_byte);
		for (int j = 0; j < 8; j++){
			//check if bit is free
			if ((bitmap_byte & (1 << j)) == 0){
				fprintf(bitmap_csv, "%x,%d\n", bitmap_block_num, block_number);
			}
			block_number++;
			//bitmap_byte = bitmap_byte >> 1;
		}
		
	}

	fclose(bitmap_csv);
	free(read_buf);
}

void read_group_descriptor(int disk_fd, const SBInfo *sb){
	int n_groups = sb->blocks_count / sb->blocks_per_group;
	//group descriptor table starts at block after superblock
	off_t bgd_offset = (sb->first_data_block + 1) * sb->block_size;

	ssize_t bytes_read;
	char *read_buf = (char *) malloc(BGD_SIZE);

	FILE *gd_csv = fopen("group.csv", "a");
	if (gd_csv == NULL) error("Opening CSV file");

	//want to iterate through the n_groups group descriptors
	for (int i = 0; i < n_groups; i++){
		bzero(read_buf, BGD_SIZE);
		bytes_read = pread(disk_fd, (void *)read_buf, BGD_SIZE, bgd_offset);
		if (bytes_read < BGD_SIZE) error("pread block group descriptor");

		//TODO: calculate the blocks per group from this and sanity check
		//Group 7: 100000 blocks, superblock says 50000

		//32bit block id of the first block of the "block bitmap" for the group represented.
		uint32_t block_bitmap = *(uint32_t *)(read_buf + BGD_BLOCK_BITMAP_OFFSET);
		//TODO: sanity check the bit map is within this groups blocks
		//Group 7: blocks 100000-150000, free block map starts at 165000

		//32bit block id of the first block of the "inode bitmap" for the group represented.
		uint32_t inode_bitmap = *(uint32_t *)(read_buf + BGD_INODE_BITMAP_OFFSET);
		//TODO: sanity check the inode map is within this groups blocks
		//Group 7: blocks 100000-150000, free Inode map starts at 165000

		//32bit block id of the first block of the "inode table" for the group represented.
		uint32_t inode_table = *(uint32_t *)(read_buf + BGD_INODE_TABLE_OFFSET);
		//TODO: sanity check the inode table is within this group's blocks
		//Group 7: blocks 100000-150000, Inode table starts at 165000

		//16bit value indicating the total number of free blocks for the represented group.
		uint16_t free_blocks_count = *(uint16_t *)(read_buf + BGD_FREE_BLOCKS_COUNT_OFFSET);

		//16bit value indicating the total number of free inodes for the represented group.
		uint16_t free_inodes_count = *(uint16_t *)(read_buf + BGD_FREE_INODES_COUNT_OFFSET);

		//16bit value indicating the number of inodes allocated to directories for the represented group.
		uint16_t used_dirs_count = *(uint16_t *)(read_buf + BGD_USED_DIRS_COUNT_OFFSET);

		fprintf(gd_csv,"%u, %u, %u, %u, %x, %x, %x\n",
			sb->blocks_per_group,
			free_blocks_count,
			free_inodes_count,
			used_dirs_count,
			inode_bitmap,
			block_bitmap,
			inode_table
		);

		read_bitmaps(disk_fd, i, block_bitmap, sb, BLOCK_BITMAP_TYPE);
		read_bitmaps(disk_fd, i, inode_bitmap, sb, INODE_BITMAP_TYPE);
		

		bgd_offset += BGD_SIZE;
	}

	fclose(gd_csv);
	free(read_buf);
}



int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stdout, "Usage: lab3a <disk-image>\n");
		exit(0);
	}

	//open the provided disk image
	int disk_fd = open(argv[1], O_RDONLY);
	if (disk_fd < 0) error("Opening disk image");

	SBInfo *sb = (SBInfo *) malloc(sizeof(SBInfo));
	read_super_block(disk_fd, sb);

	read_group_descriptor(disk_fd, sb);

	free(sb);
}