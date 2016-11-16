#include "lab3a.h"

static int IMG_SIZE_BYTES;

void error(char *msg){
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void read_indirect_block(const int disk_fd, const uint32_t block_number, const SBInfo *sb, int level){
	ssize_t bytes_read;
	uint32_t *pointer_buf = (uint32_t *) malloc(sb->block_size);

	bytes_read = pread(disk_fd, (void *)pointer_buf, sb->block_size, block_number * sb->block_size);
	if (bytes_read < sb->block_size) error("pread indirect block");

	FILE *ind_csv = fopen("indirect.csv", "a");
	if (ind_csv == NULL) error("Opening CSV file");

	int blocks_analyzed = 0;
	int pointers_per_block = sb->block_size / sizeof(uint32_t);
	while (blocks_analyzed < pointers_per_block){
		uint32_t new_block_number = *(pointer_buf + blocks_analyzed);
		if (new_block_number != 0){
			fprintf(ind_csv,"%x,%u,%x\n", block_number, blocks_analyzed, new_block_number);
			if (level > 1) read_indirect_block(disk_fd, new_block_number, sb, level - 1);
		}
		blocks_analyzed++;
	}

	fclose(ind_csv);
	free(pointer_buf);
}

void read_directory(const int disk_fd, const int parent_inode_number, const uint32_t block_number, int *entry_number, const SBInfo *sb){
	ssize_t bytes_read;
	char *read_buf = (char *) malloc(sb->block_size);

	bytes_read = pread(disk_fd, (void *)read_buf, sb->block_size, block_number * sb->block_size);
	if (bytes_read < sb->block_size) error("pread directory");


	FILE *dir_csv = fopen("directory.csv", "a");
	if (dir_csv == NULL) error("Opening CSV file");

	char* entry_pointer = read_buf;
	uint32_t bytes_scanned = 0;
	while (bytes_scanned < sb->block_size){
		//32 bit inode number of the file entry. A value of 0 indicate that the entry is not used.
		uint32_t inode_number = *(uint32_t *)(entry_pointer + DIR_PARENT_INODE_NUMBER_OFFSET);
		//16bit unsigned displacement to the next directory entry from the start of the current directory entry. 
	    uint16_t entry_length = *(uint16_t *)(entry_pointer + DIR_ENTRY_LENGTH_OFFSET);
	    //8 bit unsigned value indicating how many bytes of character data are contained in the name.
	    uint8_t  name_length = *(uint8_t *)(entry_pointer + DIR_NAME_LENGTH_OFFSET);
	    //Name of the entry. The ISO-Latin-1 character set is expected in most system. The name must be no longer than 255 bytes after encoding.
	    char name[name_length + 3];
	    name[0] = '"';
	    for (int i = 1; i <= name_length; i++){
	    	name[i] = *(char *)(entry_pointer + DIR_NAME_OFFSET + (i - 1));
	    }
	    name[name_length + 1] = '"';
	    name[name_length + 2] = '\0';

	    if (inode_number != 0){
			fprintf(dir_csv,"%d,%d,%u,%u,%u,%s\n",
				parent_inode_number,
				*entry_number,
				entry_length,
				name_length,
				inode_number,
				&name[0]
			);
		}

		entry_pointer += entry_length;
		bytes_scanned += entry_length;
		(*entry_number)++;
	}

	fclose(dir_csv);
	free(read_buf);
}


//has pointers to directories
void read_indirect_directory(const int disk_fd, const int parent_inode_number, 
	const uint32_t block_number, int *entry_number, const SBInfo *sb, int level){

	ssize_t bytes_read;
	uint32_t *pointer_buf = (uint32_t *) malloc(sb->block_size);

	bytes_read = pread(disk_fd, (void *)pointer_buf, sb->block_size, block_number * sb->block_size);
	if (bytes_read < sb->block_size) error("pread indirect directory");

	int blocks_analyzed = 0;
	int pointers_per_block = sb->block_size / sizeof(uint32_t);
	while (blocks_analyzed < pointers_per_block){
		uint32_t new_block_number = *(pointer_buf + blocks_analyzed);
		if (new_block_number != 0){
			if (level == 1) read_directory(disk_fd, parent_inode_number, new_block_number, entry_number, sb);
			else read_indirect_directory(disk_fd, parent_inode_number, new_block_number, entry_number, sb, level - 1);
		}
		blocks_analyzed++;
	}

	free(pointer_buf);
}

void read_inode(const int disk_fd, const GDInfo *gd, const SBInfo *sb, const int inode_number, const int inode_table_idx){
	//getting starting offset of inode
	off_t inode_location = (gd->inode_table * sb->block_size) + (inode_table_idx * IN_SIZE);

	ssize_t bytes_read;
	char *read_buf = (char *) malloc(IN_SIZE);

	bytes_read = pread(disk_fd, (void *)read_buf, IN_SIZE, inode_location);
	if (bytes_read < IN_SIZE) error("pread inode");


	//16 bit value used to indicate the format of the described file and the access rights
	uint16_t mode = *(uint16_t *)(read_buf + IN_MODE_OFFSET);

	//We only recognize a (small) subset of the file types: ‘f’ for regular file, ‘d’ for directory, and ‘s’ for symbolic links. For other types, use ‘?’.
	char file_type;
	if ((mode & IN_MODE_MASK) == IN_MODE_REG_FILE) file_type = 'f';
	else if ((mode & IN_MODE_MASK) == IN_MODE_DIR) file_type = 'd';
	else if ((mode & IN_MODE_MASK) == IN_MODE_SYM_LINK) file_type = 's';
	else file_type = '?';

	//16 bit user id associated with the file (lower 16 bits)
	uint16_t owner = *(uint16_t *)(read_buf + IN_OWNER_OFFSET);

	//In revision 0, (signed) 32 bit value indicating the size of the file in bytes. 
	//In revision 1 and later revisions, and only for regular files, this represents the lower 32-bit of the file size; the upper 32-bit is located in the i_dir_acl.
	uint32_t file_size = *(uint32_t *)(read_buf + IN_FILE_SIZE_OFFSET);

	//32 bit value representing the number of seconds since january 1st 1970 of the last time this inode was accessed.
	uint32_t access_time = *(uint32_t *)(read_buf + IN_ACCESS_TIME_OFFSET);

	//32 bit value representing the number of seconds since january 1st 1970, of when the inode was created.
	uint32_t creation_time = *(uint32_t *)(read_buf + IN_CREATION_TIME_OFFSET);

	//32 bit value representing the number of seconds since january 1st 1970, of the last time this inode was modified.
	uint32_t modification_time = *(uint32_t *)(read_buf + IN_MODIFICATION_TIME_OFFSET);

	//16 bit value of the POSIX group having access to this file. (lower 16 bits)
	uint16_t group = *(uint16_t *)(read_buf + IN_GROUP_OFFSET);

	//16 bit value indicating how many times this particular inode is linked (referred to). 
	uint16_t links_count = *(uint16_t *)(read_buf + IN_LINKS_COUNT_OFFSET);

	//32-bit value representing the total number of 512-bytes blocks reserved to contain the data of this inode, regardless if these blocks are used or not
	uint32_t i_blocks = *(uint32_t *)(read_buf + IN_NUMBER_OF_BLOCKS_OFFSET);
	uint32_t number_of_blocks = i_blocks / (2 << sb->block_size);

	//15 x 32bit block numbers pointing to the blocks containing the data for this inode. 
	//The first 12 blocks are direct blocks.
	//The 13th entry in this array is the block number of the first indirect block; which is a block containing an array of block ID containing the data.
	//The 14th entry in this array is the block number of the first doubly-indirect block; which is a block containing an array of indirect block IDs
	//The 15th entry in this array is the block number of the triply-indirect block
	//A value of 0 in this array effectively terminates it with no further block being defined.
	uint32_t *block_pointers = (uint32_t *)(read_buf + IN_BLOCK_POINTERS_OFFSET);

	//go through the data blocks and handle them accordingly
	int entry_number = 0;
	for (int i = 0; i < number_of_blocks; i++){
		if (i < 12 && file_type == 'd') read_directory(disk_fd, inode_number, block_pointers[i], &entry_number, sb);
		else if (i == 12) {
			if (file_type == 'd') read_indirect_directory(disk_fd, inode_number, block_pointers[i], &entry_number, sb, 1);
			read_indirect_block(disk_fd, block_pointers[i], sb, 1);
		}
		else if (i == 13) {
			if (file_type == 'd') read_indirect_directory(disk_fd, inode_number, block_pointers[i], &entry_number, sb, 2);
			read_indirect_block(disk_fd, block_pointers[i], sb, 2);
		}
		else if (i == 14) {
			if (file_type == 'd') read_indirect_directory(disk_fd, inode_number, block_pointers[i], &entry_number, sb, 3);
			read_indirect_block(disk_fd, block_pointers[i], sb, 3);
		}
	}

	FILE *in_csv = fopen("inode.csv", "a");
	if (in_csv == NULL) error("Opening CSV file");

	fprintf(in_csv,"%d,%c,%o,%u,%u,%u,%x,%x,%x,%u,%u",
		inode_number,
		file_type,
		mode,
		owner,
		group,
		links_count,
		creation_time,
		modification_time,
		access_time,
		file_size,
		number_of_blocks
	);

	for (int j = 0; j < IN_BLOCK_POINTERS_COUNT; j++){
		fprintf(in_csv, ",%x", block_pointers[j]);
	}
	fprintf(in_csv, "\n");

	fclose(in_csv);
	free(read_buf);
}

void read_bitmaps(int disk_fd, const GDInfo *gd, const SBInfo *sb, const int bitmap_type){
	int bitmap_block_num;
	int global_block_num;
	int bitmap_size;
	//start counting blocks based on which group number this is
	if (bitmap_type == BLOCK_BITMAP_TYPE) {
		bitmap_block_num = gd->block_bitmap;
		global_block_num = (gd->group_num * sb->blocks_per_group) + sb->first_data_block;
		bitmap_size = sb->blocks_per_group;
	}
	else {
		bitmap_block_num = gd->inode_bitmap;
		global_block_num = (gd->group_num * sb->inodes_per_group) + 1;
		bitmap_size = sb->inodes_per_group;
	}

	ssize_t bytes_read;
	char *read_buf = (char *) malloc(sb->block_size);
	off_t bitmap_offset = bitmap_block_num * sb->block_size;
	bytes_read = pread(disk_fd, (void *)read_buf, sb->block_size, bitmap_offset);
	if (bytes_read < sb->block_size) error("pread bitmap");

	FILE *bitmap_csv = fopen("bitmap.csv", "a");
	if (bitmap_csv == NULL) error("Opening CSV file");
	
	//Each bit represent the current state of a block within that block group, where 1 means "used" and 0 "free/available". 
	//The first block of this block group is represented by bit 0 of byte 0, the second by bit 1 of byte 0. 
	//The 8th block is represented by bit 7 (most significant bit) of byte 0 while the 9th block is represented by bit 0 (least significant bit) of byte 1.
	//TODO: round up here
	for (int i = 0; i < (bitmap_size / 8); i++){
		//grabbing 8 bits of the bitmap
		uint8_t bitmap_byte = *(uint8_t *)(read_buf + i);
		// fprintf(stdout, "bitmap byte %d is %x\n", i, bitmap_byte);
		for (int j = 0; j < 8; j++){
			//check if bit is free
			if ((bitmap_byte & (1 << j)) == 0){
				fprintf(bitmap_csv, "%x,%d\n", bitmap_block_num, global_block_num);
			}
			else if (bitmap_type == INODE_BITMAP_TYPE) {
				//fprintf(stdout, "checking allocated inode %d\n", (bitmap_byte & (1 << j)));
				//where in the inode table is this block
				int inode_table_idx = (i * 8) + j;
				//fprintf(stdout, "calling read inode\n");
				read_inode(disk_fd, gd, sb, global_block_num, inode_table_idx);
			}
			global_block_num++;
		}
	}

	fclose(bitmap_csv);
	free(read_buf);
}


void read_group_descriptor(int disk_fd, const SBInfo *sb){
	int n_groups = sb->blocks_count / sb->blocks_per_group;
	//group descriptor table starts at first block after superblock
	off_t bgd_offset = (sb->first_data_block + 1) * sb->block_size;

	ssize_t bytes_read;
	char *read_buf = (char *) malloc(BGD_SIZE);
	if (read_buf == NULL) error("malloc");

	FILE *gd_csv = fopen("group.csv", "a");
	if (gd_csv == NULL) error("Opening CSV file");

	int group_start_block = bgd_offset / sb->block_size;
	int group_end_block = group_start_block + sb->blocks_per_group;
	//want to iterate through the n_groups group descriptors
	for (int i = 0; i < n_groups; i++){
		bzero(read_buf, BGD_SIZE);
		bytes_read = pread(disk_fd, (void *)read_buf, BGD_SIZE, bgd_offset);
		if (bytes_read < BGD_SIZE) error("pread block group descriptor");

		GDInfo *gd = (GDInfo *) malloc(sizeof(GDInfo));
		if (gd == NULL) error("malloc");

		gd->group_num = i;

		//32bit block id of the first block of the "block bitmap" for the group represented.
		gd->block_bitmap = *(uint32_t *)(read_buf + BGD_BLOCK_BITMAP_OFFSET);
		if (gd->block_bitmap < group_start_block || gd->block_bitmap > group_end_block){
			fprintf(stderr, "Group %d: blocks %d-%d, free block map starts at %d\n", 
				gd->group_num, group_start_block, group_end_block, gd->block_bitmap);

			bgd_offset += BGD_SIZE;
			if (i % 2 != 0){
				group_start_block += sb->blocks_per_group;
				group_end_block += sb->blocks_per_group;
			}
			else {
				group_start_block += (sb->blocks_per_group - 1);
				group_end_block += (sb->blocks_per_group - 1);
			}
			free(gd);
			continue;
		}

		//32bit block id of the first block of the "inode bitmap" for the group represented.
		gd->inode_bitmap = *(uint32_t *)(read_buf + BGD_INODE_BITMAP_OFFSET);
		if (gd->inode_bitmap < group_start_block || gd->inode_bitmap > group_end_block){
			fprintf(stderr, "Group %d: blocks %d-%d, free Inode map starts at %d\n", 
				gd->group_num, group_start_block, group_end_block, gd->inode_bitmap);

			bgd_offset += BGD_SIZE;
			if (i % 2 != 0){
				group_start_block += sb->blocks_per_group;
				group_end_block += sb->blocks_per_group;
			}
			else {
				group_start_block += (sb->blocks_per_group - 1);
				group_end_block += (sb->blocks_per_group - 1);
			}
			free(gd);
			continue;
		}

		//32bit block id of the first block of the "inode table" for the group represented.
		gd->inode_table = *(uint32_t *)(read_buf + BGD_INODE_TABLE_OFFSET);
		if (gd->inode_table < group_start_block || gd->inode_table > group_end_block){
			fprintf(stderr, "Group %d: blocks %d-%d, Inode table starts at %d\n", 
				gd->group_num, group_start_block, group_end_block, gd->inode_table);

			bgd_offset += BGD_SIZE;
			if (i % 2 != 0){
				group_start_block += sb->blocks_per_group;
				group_end_block += sb->blocks_per_group;
			}
			else {
				group_start_block += (sb->blocks_per_group - 1);
				group_end_block += (sb->blocks_per_group - 1);
			}
			free(gd);
			continue;
		}

		//16bit value indicating the total number of free blocks for the represented group.
		gd->free_blocks_count = *(uint16_t *)(read_buf + BGD_FREE_BLOCKS_COUNT_OFFSET);

		//16bit value indicating the total number of free inodes for the represented group.
		gd->free_inodes_count = *(uint16_t *)(read_buf + BGD_FREE_INODES_COUNT_OFFSET);

		//16bit value indicating the number of inodes allocated to directories for the represented group.
		gd->used_dirs_count = *(uint16_t *)(read_buf + BGD_USED_DIRS_COUNT_OFFSET);

		fprintf(gd_csv,"%u,%u,%u,%u,%x,%x,%x\n",
			sb->blocks_per_group,
			gd->free_blocks_count,
			gd->free_inodes_count,
			gd->used_dirs_count,
			gd->inode_bitmap,
			gd->block_bitmap,
			gd->inode_table
		);

		read_bitmaps(disk_fd, gd, sb, BLOCK_BITMAP_TYPE);
		read_bitmaps(disk_fd, gd, sb, INODE_BITMAP_TYPE);
		

		bgd_offset += BGD_SIZE;
		if (i % 2 != 0){
			group_start_block += sb->blocks_per_group;
			group_end_block += sb->blocks_per_group;
		}
		else {
			group_start_block += (sb->blocks_per_group - 1);
			group_end_block += (sb->blocks_per_group - 1);
		}

		free(gd);
	}

	fclose(gd_csv);
	free(read_buf);
}

void read_super_block(int disk_fd, SBInfo *sb){
	ssize_t bytes_read;

	char *read_buf = (char *) malloc(SB_SIZE);
	if (read_buf == NULL) error("malloc");

	bytes_read = pread(disk_fd, (void *)read_buf, SB_SIZE, SB_OFFSET);
	if (bytes_read < SB_SIZE) error("pread superblock");

	//32 bit value indicating the total number of inodes, both used and free, in the file system
	sb->inodes_count = *(uint32_t *)(read_buf + SB_INODES_COUNT_OFFSET);

	//The block size is computed using this 32bit value as the number of bits to shift left the value 1024
	uint32_t block_size_shift = *(uint32_t *)(read_buf + SB_BLOCK_SIZE_OFFSET);
	sb->block_size = 1024 << block_size_shift;
	//Block size must be reasonable (e.g. power of two between 512-64K)
	if (sb->block_size < 512 || sb->block_size > 65536 || sb->block_size % 2 != 0){
		fprintf(stderr, "Superblock - invalid block size: %lu\n", sb->block_size);
		exit(1);
	}

	int image_size_blocks;
	if (IMG_SIZE_BYTES % sb->block_size != 0) image_size_blocks = (IMG_SIZE_BYTES / sb->block_size) + 1;
	else image_size_blocks = (IMG_SIZE_BYTES / sb->block_size);
	fprintf(stdout, "image_size_blocks: %d\n", image_size_blocks);

	//32 bit value indicating the total number of blocks in the system including all used, free and reserved
	sb->blocks_count = *(uint32_t *)(read_buf + SB_BLOCKS_COUNT_OFFSET);
	//Total blocks and first data block must be consistent with the file size
	if (sb->blocks_count > image_size_blocks){
		fprintf(stderr, "Superblock - invalid block count %u > image size %d\n", image_size_blocks, sb->blocks_count);
		exit(1);
	}

	//32 bit value identifying the first data block, in other word the id of the block containing the superblock structure.
	sb->first_data_block = *(uint32_t *)(read_buf + SB_FIRST_DATA_BLOCK_OFFSET);
	//First data block must be consistent with the file size
	if (sb->first_data_block > image_size_blocks){
		fprintf(stderr, "Superblock - invalid first block %u > image size %d\n", image_size_blocks, sb->first_data_block);
		exit(1);
	}
	

	//The fragment size is computed using this 32bit value as the number of bits to shift left the value 1024. Note that a negative value would shift the bit right rather than left.
	int32_t fragment_size_shift = *(int32_t *)(read_buf + SB_FRAGMENT_SIZE_OFFSET);
	if (fragment_size_shift > 0) sb->fragment_size = 1024 << fragment_size_shift;
	else sb->fragment_size = 1024 >> (-fragment_size_shift);

	//32 bit value indicating the total number of blocks per group
	sb->blocks_per_group = *(uint32_t *)(read_buf + SB_BLOCKS_PER_GROUP_OFFSET);
	//Blocks per group must evenly divide into total blocks
	if (sb->blocks_count % sb->blocks_per_group != 0){
		fprintf(stderr, "Superblock - %u blocks, %u blocks/group\n", sb->blocks_count, sb->blocks_per_group);
		exit(1);
	}

	//32 bit value indicating the total number of fragments per group
	sb->fragments_per_group = *(uint32_t *)(read_buf + SB_FRAGMENTS_PER_GROUP_OFFSET);

	//32 bit value indicating the total number of inodes per group
	sb->inodes_per_group = *(uint32_t *)(read_buf + SB_INODES_PER_GROUP_OFFSET);
	//Inodes per group must evenly divide into total inodes
	if (sb->inodes_count % sb->inodes_per_group != 0){
		fprintf(stderr, "Superblock - %u inodes, %u inodes/group\n", sb->inodes_count, sb->inodes_per_group);
		exit(1);
	}

	//16 bit value identifying the file system as Ext2. The value is currently fixed to EXT2_SUPER_MAGIC of value 0xEF53.
	sb->magic_number = *(uint16_t *)(read_buf + SB_MAGIC_NUMBER_OFFSET);
	//Magic number must be correct
	if (sb->magic_number != EXT2_SUPER_MAGIC) {
		fprintf(stderr, "Superblock - invalid magic: %x\n", sb->magic_number);
		exit(1);
	}


	FILE *sb_csv = fopen("super.csv", "a");
	if (sb_csv == NULL) error("Opening CSV file");
	fprintf(sb_csv, "%x,%u,%u,%lu,%ld,%u,%u,%u,%u\n",
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



int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stdout, "Usage: lab3a <disk-image>\n");
		exit(0);
	}

	//open the provided disk image
	int disk_fd = open(argv[1], O_RDONLY);
	if (disk_fd < 0) error("Opening disk image");
	struct stat stat_buf;
	fstat(disk_fd, &stat_buf);
	IMG_SIZE_BYTES = stat_buf.st_size;
	fprintf(stdout, "image_size_bytes: %d\n", IMG_SIZE_BYTES);

	SBInfo *sb = (SBInfo *) malloc(sizeof(SBInfo));
	if (sb == NULL) error("malloc");
	read_super_block(disk_fd, sb);

	read_group_descriptor(disk_fd, sb);

	free(sb);
}