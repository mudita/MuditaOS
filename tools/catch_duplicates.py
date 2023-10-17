#!/usr/bin/python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# Script detecting source files which are compiled twice and listing libraries where they are compiled
# Usage example: ./catch_duplicates.py ../build-rt1051-Release/compile_commands.json > what_compiles_twice.log

import sys, json

def print_duplicates(filename, occurs):
	print_duplicates.duplicate_number += 1
	print("Duplicate " + str(print_duplicates.duplicate_number) + "\n" + filename)
	for val in occurs:
		print(val)
	print("")
print_duplicates.duplicate_number = 0


def main():
	file_handle = open(sys.argv[1], "r")
	cc_dict = json.load(file_handle)
	known_files = set()

	for x in range(len(cc_dict)):
		filename1 = str(cc_dict[x]['file'])
		directory1 = str(cc_dict[x]['directory'])
		occurs = set()
		occurs.add(directory1)

		if filename1 not in known_files:

			for y in range(len(cc_dict)):
				filename2 = str(cc_dict[y]['file'])
				directory2 = str(cc_dict[y]['directory'])

				if filename1 == filename2 and x != y:
					occurs.add(directory2)

		if len(occurs) > 1:
			print_duplicates(filename1, occurs)
		
		known_files.add(filename1)

	file_handle.close()


if __name__ == '__main__':
    main()
