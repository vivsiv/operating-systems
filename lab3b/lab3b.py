import sys
import csv
from math import ceil

superblock_constants = {
	"total_inodes_idx" : 1,
	"total_blocks_idx" : 2,
	"block_size" : 3,
	"blocks_per_group_idx" : 5,
	"inodes_per_group_idx" : 6,
	"first_data_block_idx" : 8
}

group_constants = {
	"inode_bitmap_block_idx" : 4,
	"block_bitmap_block_idx" : 5
}

inode_constants = {
	"inode_number_idx" : 0,
	"link_count_idx" : 5,
	"number_of_blocks_idx" : 10,
	"block_pointers_start_idx" : 11,
	"block_pointers_length" : 15,
	"direct_blocks_count" : 12,
	"reserved_inodes" : 11
}

bitmap_constants = {
	"map_block_number_idx" : 0,
	"free_block_number_idx" : 1
}

directory_constants = {
	"parent_inode_number_idx" : 0,
	"entry_number_idx" : 1,
	"inode_number_idx" : 4,
	"name_idx" : 5,
	"root_directory_inode" : 2
}

indirect_block_constants = {
	"indirect_block_ptr_idx" : 0,
	"entry_number_idx" : 1,
	"direct_block_ptr_idx" : 2
}



# CHECK 1
# Unallocated block: blocks that are in use but also listed on the free bitmap. Here the INODEs should be listed in increasing order of the inode_num.
# UNALLOCATED BLOCK < block_num > REFERENCED BY (INODE < inode_num > (INDIRECT BLOCK < block_num>) ENTRY < entry_num >) * n
# Example: UNALLOCATED BLOCK < 1035 > REFERENCED BY INODE < 16 > ENTRY < 0 > INODE < 17 > INDIRECT BLOCK < 10 > ENTRY < 0 >
def unallocated_blocks(inode_map, free_data_blocks_set, indirect_block_map, output_file):
	for inode, block_array in sorted(inode_map.iteritems()):
		for i in range(len(block_array)):
			block = block_array[i]
			if block in free_data_blocks_set:
				block_key = str(block)
				output_string = ""
				if block_key in indirect_block_map.keys():
					indirect_block = indirect_block_map[block_key][1]
					output_string = "UNALLOCATED BLOCK < {0} > REFERENCED BY INODE < {1} > INDIRECT BLOCK {2} ENTRY < {3} >\n".format(block, inode, indirect_block, i)
				else:
					output_string = "UNALLOCATED BLOCK < {0} > REFERENCED BY INODE < {1} > ENTRY < {2} >\n".format(block, inode, i)
				output_file.write(output_string)


# CHECK 2
# Duplicately allocated block: blocks that are used by more than one inodes. Here the INODEs should be listed in increasing order of the inode_num.
# MULTIPLY REFERENCED BLOCK < block_num > BY (INODE < inode_num > (INDIRECT BLOCK < block_num>) ENTRY < entry_num >) * n
# Example: MULTIPLY REFERENCED BLOCK < 613 > BY INODE < 24 > ENTRY < 0 > INODE < 25 > ENTRY < 0 > INODE < 26 > ENTRY < 0 >
def duplicate_allocated_blocks(block_reference_map, indirect_block_map, output_file):
	for block, references in sorted(block_reference_map.iteritems()):
		block_key = str(block)
		if len(references) > 1:
			output_string = "MULTIPLY REFERENCED BLOCK < {0} > BY".format(block)
			for reference in references:
				if block_key in indirect_block_map.keys():
					indirect_block = indirect_block_map[block_key][1]
					output_string +=  " INODE < {0} > INDIRECT BLOCK {1} ENTRY {2}".format(reference[0],indirect_block,reference[1])
				else:
					output_string += " INODE < {0} > ENTRY < {1} >".format(reference[0],reference[1])
			output_string += "\n"
			output_file.write(output_string)

# CHECK 3
# Unallocated inode: inodes that are in use by directory entries (the inode number of the file entry field) but not shown up in inode.csv. Here the DIRECTORYs should be listed in increasing order of the inode_num.
# UNALLOCATED INODE < inode_num > REFERENCED BY (DIRECTORY < inode_num > ENTRY < entry_num >) * n
# Example: UNALLOCATED INODE < 21 > REFERENCED BY DIRECTORY < 2 > ENTRY < 12 >
def unallocated_inodes(directory_csv_reader, inode_list, output_file):
	for row in directory_csv_reader:
		inode_num = int(row[directory_constants["inode_number_idx"]])
		entry_num = int(row[directory_constants["entry_number_idx"]])
		directory_num = int(row[directory_constants["parent_inode_number_idx"]])
		# print("{0},{1},{2}".format(directory_num,entry_num,inode_num))
		if inode_num not in inode_list:
			output_string = "UNALLOCATED INODE < {0} > REFERENCED BY DIRECTORY < {1} > ENTRY < {2} >\n".format(inode_num,directory_num,entry_num) 
			output_file.write(output_string)

# CHECK 4
# Missing inode: inodes that are not in use, and not listed on the free bitmap.
# MISSING INODE < inode_num > SHOULD BE IN FREE LIST < block_num >
# Example: MISSING INODE < 34 > SHOULD BE IN FREE LIST < 4 >
def missing_inodes(superblock_info, allocated_inode_list, free_inode_list, first_inode_bitmap, output_file):
	for i in range(inode_constants["reserved_inodes"] + 1, superblock_info["total_inodes"] + 1):
		if i not in allocated_inode_list and i not in free_inode_list:
			free_list_num = (i / superblock_info["num_groups"]) + first_inode_bitmap
			output_string = "MISSING INODE < {0} > SHOULD BE IN FREE LIST < {1} >\n".format(i,free_list_num)
			output_file.write(output_string)

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



def create_super_block_info(superblock_csv_reader):
	sb_info = {}
	for row in superblock_csv_reader:
		sb_info["total_blocks"] = int(row[superblock_constants["total_blocks_idx"]])
		sb_info["total_inodes"] = int(row[superblock_constants["total_inodes_idx"]])
		sb_info["block_size"] = int(row[superblock_constants["block_size"]])
		sb_info["inodes_per_group"] = int(row[superblock_constants["inodes_per_group_idx"]])
		sb_info["first_data_block"] = int(row[superblock_constants["first_data_block_idx"]])
		sb_info["num_groups"] = int(ceil(sb_info["total_blocks"] / float(sb_info["block_size"])))

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


# CHECK 5
# Incorrect link count: inodes whose link counts do not reflect the number of directory entries that point to them.
# LINKCOUNT < inode_num > IS < link_count > SHOULD BE < link_count >
# Example: LINKCOUNT < 1714 > IS < 3 > SHOULD BE < 2 >

# CHECK 7
# Invalid block pointer: block pointer that has an invalid block number.
# INVALID BLOCK < block_num > IN INODE < inode_num > (INDIRECT BLOCK < block_num >) ENTRY < entry_num >
# Example: INVALID BLOCK < 1 > IN INODE < 2 > INDIRECT BLOCK < 3 > ENTRY < 4 > or INVALID BLOCK < 1 > IN INODE < 2 > ENTRY < 4 >
def create_inode_structures(superblock_info, inode_csv_reader, directory_reference_map, indirect_block_map, output_file):
	allocated_inode_map = {}
	block_reference_map = {}

	first_data_block = superblock_info["first_data_block"]
	last_data_block = first_data_block + superblock_info["total_blocks"]
	block_size = superblock_info["block_size"]

	for row in inode_csv_reader:
		blocks = []

		inode_number = int(row[inode_constants["inode_number_idx"]])
		num_blocks = int(row[inode_constants["number_of_blocks_idx"]])


		max_block_pointer = 0
		if num_blocks < inode_constants["direct_blocks_count"]:
			max_block_pointer = num_blocks
		elif num_blocks < (inode_constants["direct_blocks_count"] + block_size):
			max_block_pointer = 12
		elif num_blocks < (inode_constants["direct_blocks_count"] + (block_size * block_size)):
			max_block_pointer = 13
		else:
			max_block_pointer = 14

		zeroReached = False
		for i in range(max_block_pointer):
			new_block = int(row[inode_constants["block_pointers_start_idx"] + i], 16)
			block_key = str(new_block)
			if new_block == 0:
				zeroReached = True
			
			if new_block < first_data_block or new_block > last_data_block:
				output_string = ""
				if block_key in indirect_block_map.keys():
					indirect_block = indirect_block_map[block_key][1]
					output_string = "INVALID BLOCK < {0} > IN INODE < {1} > INDIRECT BLOCK {2} ENTRY < {3} >\n".format(new_block, inode_number, indirect_block, i)
				else:
					output_string = "INVALID BLOCK < {0} > IN INODE < {1} > ENTRY < {2} >\n".format(new_block, inode_number, i)
				output_file.write(output_string)
			elif not zeroReached:
				blocks.append(new_block)
				
				if block_key in block_reference_map.keys():
					block_reference_map[block_key].append((inode_number,i))
				else:
					block_reference_map[block_key] = [(inode_number,i)]
			
		inode_key = str(inode_number)
		allocated_inode_map[inode_key] = blocks

		if inode_key in directory_reference_map.keys():
			link_count = int(row[inode_constants["link_count_idx"]])
			referenced_directories_count = len(directory_reference_map[inode_key])
			if link_count != referenced_directories_count:
				output_string = "LINKCOUNT < {0} > IS < {1} > SHOULD BE < {2} >\n".format(inode_number,link_count,referenced_directories_count)
				output_file.write(output_string)


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

	return (free_inode_blocks,free_data_blocks)


# CHECK 6
# Incorrect directory entry: the '.' and '..' entries that don't link to correct inodes.
# INCORRECT ENTRY IN < inode_num > NAME < entry_name > LINK TO < inode_num > SHOULD BE < inode_num >
# Example, INCORRECT ENTRY IN < 1714 > NAME < . > LINK TO < 1713 > SHOULD BE < 1714 >
def create_directory_structures(directory_csv_reader, output_file):
	referenced_inodes = set([])
	directory_reference_map = {}
	parents_map = {}
	for row in directory_csv_reader:
		inode_number = int(row[directory_constants["inode_number_idx"]])
		parent_inode = int(row[directory_constants["parent_inode_number_idx"]])
		name = row[directory_constants["name_idx"]]

		if inode_number != parent_inode:
			parents_map[str(inode_number)] = parent_inode
		# print parents_map

		referenced_inodes.add(inode_number)

		inode_key = str(inode_number)
		if inode_key in directory_reference_map.keys():
			directory_reference_map[inode_key].append(parent_inode)
		else:
			directory_reference_map[inode_key] = [parent_inode]


		if name == ".":
			if parent_inode != inode_number:
				output_string = "INCORRECT ENTRY IN < {0} > NAME < {1} > LINK TO < {2} > SHOULD BE < {3} >\n".format(parent_inode,name,inode_number,parent_inode)
				output_file.write(output_string)
		if name == "..":
			if parent_inode == directory_constants["root_directory_inode"]:
				if parent_inode != inode_number:
					output_string = "INCORRECT ENTRY IN < {0} > NAME < {1} > LINK TO < {2} > SHOULD BE < {3} >\n".format(parent_inode,name,inode_number,parent_inode)
					output_file.write(output_string)
			else:
				actual_parent = parents_map[str(parent_inode)]
				if inode_number != actual_parent:
					output_string = "INCORRECT ENTRY IN < {0} > NAME < {1} > LINK TO < {2} > SHOULD BE < {3} >\n".format(parent_inode,name,inode_number,actual_parent)
					output_file.write(output_string)
			

	return (referenced_inodes,directory_reference_map)


def create_indirect_block_map(indirect_csv_reader):
	indirect_block_map = {}
	for row in indirect_csv_reader:
		indirect_block = int(row[indirect_block_constants["indirect_block_ptr_idx"]],16)
		direct_block = int(row[indirect_block_constants["direct_block_ptr_idx"]],16)
		entry_num = int(row[indirect_block_constants["entry_number_idx"]])

		direct_block_key = str(direct_block)
		if direct_block_key in indirect_block_map.keys():
			indirect_block_map[direct_block_key].append((indirect_block,entry_num))
		else:
			indirect_block_map[direct_block_key] = [(indirect_block,entry_num)]

	return indirect_block_map


def main():
	if len(sys.argv) != 7:
		print "Usage: python <super.csv> <group.csv> <bitmap.csv> <inode.csv> <directory.csv> <indirect.csv>"
		exit(1)

	csv_file_names = ["super.csv", "group.csv", "bitmap.csv", "inode.csv", "directory.csv", "indirect.csv"]
	csv_readers = {}	

	for i in range(1,len(sys.argv)):
		csv_file = open(sys.argv[i])
		if csv_file == None:
			print "Unable to open file: {0}, should be: {1}".format(csv_file, csv_file_names[i - 1])
			exit(1)
		csv_readers[sys.argv[i].split(".")[0]] = csv.reader(csv_file)

	output_file = open("lab3b_check.txt", "w")

	superblock_info = create_super_block_info(csv_readers["super"])
	indirect_block_map = create_indirect_block_map(csv_readers["indirect"])

	bitmap_blocks_tuple = create_bitmap_blocks_sets(csv_readers["group"])
	inode_bitmap_blocks_set = bitmap_blocks_tuple[0]
	data_bitmap_blocks_set = bitmap_blocks_tuple[1]

	free_blocks_tuple = create_free_blocks_sets(csv_readers["bitmap"],inode_bitmap_blocks_set, data_bitmap_blocks_set)
	free_inode_blocks_set = free_blocks_tuple[0]
	free_data_blocks_set = free_blocks_tuple[1]

	directory_structures_tuple = create_directory_structures(csv_readers["directory"],output_file)
	directory_inode_list = directory_structures_tuple[0]
	directory_reference_map = directory_structures_tuple[1]
	# print directory_reference_map

	inode_structures_tuple = create_inode_structures(superblock_info, csv_readers["inode"], directory_reference_map, indirect_block_map, output_file)
	inode_map = inode_structures_tuple[0]
	block_reference_map = inode_structures_tuple[1]
	
	duplicate_allocated_blocks(block_reference_map, indirect_block_map, output_file)

	inode_list = sorted(map(lambda inode_str: int(inode_str), inode_map.keys()))
	unallocated_inodes(csv_readers["directory"], inode_list, output_file)

	first_inode_bitmap = sorted(inode_bitmap_blocks_set)[0]
	missing_inodes(superblock_info, directory_inode_list, free_inode_blocks_set, first_inode_bitmap, output_file)

	unallocated_blocks(inode_map, free_data_blocks_set, indirect_block_map, output_file)


	

if __name__ == "__main__": main()
