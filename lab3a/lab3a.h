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
