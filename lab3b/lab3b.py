import sys
import csv

superblock_constants = {
	"todal_inodes_idx" : 1,
	"total_blocks_idx" : 2,
	"blocks_per_group_idx" : 5,
	"inodes_per_group_idx" : 6
}

group_constants = {
	"inode_bitmap_block_idx" : 4,
	"block_bitmap_block_idx" : 5
}

inode_constants = {
	"inode_number_idx" : 0,
	"number_of_blocks_idx" : 10,
	"block_pointers_start_idx" : 11,
	"block_pointers_length" : 15
}

bitmap_constants = {
	"map_block_number_idx" : 0,
	"free_block_number_idx" : 1
}

directory_constants = {
	"parent_inode_number_idx" : 0,
	"entry_number_idx" : 1,
	"inode_number_idx" : 4
}


# CHECK 1
# Unallocated block: blocks that are in use but also listed on the free bitmap. Here the INODEs should be listed in increasing order of the inode_num.
# UNALLOCATED BLOCK < block_num > REFERENCED BY (INODE < inode_num > (INDIRECT BLOCK < block_num>) ENTRY < entry_num >) * n
# Example: UNALLOCATED BLOCK < 1035 > REFERENCED BY INODE < 16 > ENTRY < 0 > INODE < 17 > INDIRECT BLOCK < 10 > ENTRY < 0 >
def unallocated_blocks(inode_map, free_data_blocks_set, output_file):
	for inode, block_array in sorted(inode_map.iteritems()):
		for i in range(len(block_array)):
			block = block_array[i]
			if block in free_data_blocks_set:
				output_string = "UNALLOCATED BLOCK < {0} > REFERENCED BY INODE < {1} > ENTRY < {2} >\n".format(block, inode, i)
				output_file.write(output_string)


# CHECK 2
# Duplicately allocated block: blocks that are used by more than one inodes. Here the INODEs should be listed in increasing order of the inode_num.
# MULTIPLY REFERENCED BLOCK < block_num > BY (INODE < inode_num > (INDIRECT BLOCK < block_num>) ENTRY < entry_num >) * n
# Example: MULTIPLY REFERENCED BLOCK < 613 > BY INODE < 24 > ENTRY < 0 > INODE < 25 > ENTRY < 0 > INODE < 26 > ENTRY < 0 >
def duplicate_allocated_block(block_reference_map, output_file):
	for block, references in sorted(block_reference_map.iteritems()):
		if len(references) > 1:
			output_string = "MULTIPLY REFERENCED BLOCK < {0} > BY ".format(block)
			for reference in references:
				output_string += " INODE < {0} > ENTRY < {1} >".format(reference[0],reference[1])
			output_string += "\n"
			output_file.write(output_string)

# CHECK 3
# Unallocated inode: inodes that are in use by directory entries (the inode number of the file entry field) but not shown up in inode.csv. Here the DIRECTORYs should be listed in increasing order of the inode_num.
# UNALLOCATED INODE < inode_num > REFERENCED BY (DIRECTORY < inode_num > ENTRY < entry_num >) * n
# Example: UNALLOCATED INODE < 21 > REFERENCED BY DIRECTORY < 2 > ENTRY < 12 >
def unallocated_inode(directory_csv_reader, inode_list, output_file):
	for row in directory_csv_reader:
		inode_num = int(row[directory_constants["inode_number_idx"]])
		entry_num = int(row[directory_constants["entry_number_idx"]])
		directory_num = int(row[directory_constants["parent_inode_number_idx"]])
		# print("{0},{1},{2}".format(directory_num,entry_num,inode_num))
		if inode_num not in inode_list:
			output_string = "UNALLOCATED INODE < {0} > REFERENCED BY DIRECTORY < {1} > ENTRY < {2}\n".format(inode_num,directory_num,entry_num) 
			output_file.write(output_string)

# CHECK 4
# Missing inode: inodes that are not in use, and not listed on the free bitmap.
# MISSING INODE < inode_num > SHOULD BE IN FREE LIST < block_num >
# Example: MISSING INODE < 34 > SHOULD BE IN FREE LIST < 4 >
def missing_inode(inode_list, output_file):

	print "not implemented"

# CHECK 5
# Incorrect link count: inodes whose link counts do not reflect the number of directory entries that point to them.
# LINKCOUNT < inode_num > IS < link_count > SHOULD BE < link_count >
# Example: LINKCOUNT < 1714 > IS < 3 > SHOULD BE < 2 >
def incorrect_link_count(csv_files, output_file):
	print "not implemented"

# CHECK 6
# Incorrect directory entry: the '.' and '..' entries that don't link to correct inodes.
# INCORRECT ENTRY IN < inode_num > NAME < entry_name > LINK TO < inode_num > SHOULD BE < inode_num >
# Example, INCORRECT ENTRY IN < 1714 > NAME < . > LINK TO < 1713 > SHOULD BE < 1714 >
def incorrect_directory_entry(csv_files, output_file):
	print "not implemented"

# CHECK 7
# Invalid block pointer: block pointer that has an invalid block number.
# INVALID BLOCK < block_num > IN INODE < inode_num > (INDIRECT BLOCK < block_num >) ENTRY < entry_num >
# Example: INVALID BLOCK < 1 > IN INODE < 2 > INDIRECT BLOCK < 3 > ENTRY < 4 > or INVALID BLOCK < 1 > IN INODE < 2 > ENTRY < 4 >
def invalid_block_pointer(csv_files, output_file):
	print "not implemented"

def create_super_block_info(superblock_csv_reader):
	sb_info = {}
	for row in superblock_csv_reader:
		sb_info["num_inodes"] = int(row[superblock_constants["inodes_per_group_idx"]])

	return sb_info

def create_bitmap_blocks_sets(group_csv_reader):
	inode_bitmap_blocks = set([])
	data_bitmap_blocks = set([])
	
	for row in group_csv_reader:
		# print row[group_constants["inode_bitmap_block_idx"]]
		# print row[group_constants["block_bitmap_block_idx"]]
		inode_bitmap_blocks.add(int(row[group_constants["inode_bitmap_block_idx"]],16))
		data_bitmap_blocks.add(int(row[group_constants["block_bitmap_block_idx"]],16))
		
	return (inode_bitmap_blocks,data_bitmap_blocks)


def create_inode_structures(inode_csv_reader):
	allocated_inode_map = {}
	block_reference_map = {}
	for row in inode_csv_reader:
		blocks = []

		num_blocks = int(row[inode_constants["number_of_blocks_idx"]])
		if num_blocks > inode_constants["block_pointers_length"]:
			num_blocks = inode_constants["block_pointers_length"]

		for i in range(num_blocks):
			new_block = int(row[inode_constants["block_pointers_start_idx"] + i], 16)
			if new_block == 0:
				break;
			blocks.append(new_block)

			block_key = str(new_block)
			if block_key in block_reference_map.keys():
				block_reference_map[block_key].append((int(row[inode_constants["inode_number_idx"]]),i))
			else:
				block_reference_map[block_key] = [(int(row[inode_constants["inode_number_idx"]]),i)]

		allocated_inode_map[row[inode_constants["inode_number_idx"]]] = blocks
	return (allocated_inode_map,block_reference_map)


def create_free_blocks_sets(bitmap_csv_reader,inode_bitmap_blocks_set,data_bitmap_blocks_set):
	free_inode_blocks = set([])
	free_data_blocks = set([])

	for row in bitmap_csv_reader:
		if int(row[bitmap_constants["map_block_number_idx"]],16) in inode_bitmap_blocks_set:
			free_inode_block = int(row[bitmap_constants["free_block_number_idx"]])
			free_inode_blocks.add(free_inode_block)
		elif int(row[bitmap_constants["map_block_number_idx"]],16) in data_bitmap_blocks_set:
			# print "{0},{1}".format(row[bitmap_constants["map_block_number_idx"]],int(row[bitmap_constants["free_block_number_idx"]]))
			# print row[bitmap_constants["map_block_number_idx"]]
			free_data_block = int(row[bitmap_constants["free_block_number_idx"]])
			free_data_blocks.add(free_data_block)

	return free_inode_blocks,free_data_blocks



def main():
	if len(sys.argv) != 7:
		print "Usage: python <super.csv> <group.csv> <bitmap.csv> <inode.csv> <directory.csv> <inode.csv>"
		exit(1)

	csv_file_names = ["super.csv", "group.csv", "bitmap.csv", "inode.csv", "directory.csv", "inode.csv"]
	csv_readers = {}	

	for i in range(1,len(sys.argv) - 1):
		csv_file = open(sys.argv[i])
		if csv_file == None:
			print "Unable to open file: {0}, should be: {1}".format(csv_file, csv_file_names[i - 1])
			exit(1)
		csv_readers[sys.argv[i].split(".")[0]] = csv.reader(csv_file)

	superblock_info = create_super_block_info(csv_readers["super"])

	bitmap_blocks_tuple = create_bitmap_blocks_sets(csv_readers["group"])
	inode_bitmap_blocks_set = bitmap_blocks_tuple[0]
	data_bitmap_blocks_set = bitmap_blocks_tuple[1]

	free_blocks_tuple = create_free_blocks_sets(csv_readers["bitmap"],inode_bitmap_blocks_set,data_bitmap_blocks_set)
	free_inode_blocks_set = free_blocks_tuple[0]
	free_data_blocks_set = free_blocks_tuple[1]

	inode_structures_tuple = create_inode_structures(csv_readers["inode"])
	inode_map = inode_structures_tuple[0]
	block_reference_map = inode_structures_tuple[1]
	
	output_file = open("lab3b_check.txt", "w")

	unallocated_blocks(inode_map,free_data_blocks_set,output_file)
	duplicate_allocated_block(block_reference_map,output_file)

	inode_list = sorted(map(lambda inode_str: int(inode_str),inode_map.keys()))
	unallocated_inode(csv_readers["directory"],inode_list,output_file)

	

if __name__ == "__main__": main()
