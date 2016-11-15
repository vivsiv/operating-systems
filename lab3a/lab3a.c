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

	//32 bit value identifying the first data block, in other word the id of the block containing the superblock structure.
	sb->first_data_block = *(uint32_t *)(read_buf + SB_FIRST_DATA_BLOCK_OFFSET);

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


	FILE *sb_csv = fopen("super.csv", "w");
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

	SBInfo *sb = (SBInfo *) malloc(sizeof(SBInfo));
	read_super_block(disk_fd, sb);


	free(sb);
}