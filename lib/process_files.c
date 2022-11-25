#include "utils.h"
#include "parse_args.h"
#include "file.h"
#include <stdio.h>

// print out files
// recursively process directories if -R
ResultType process_files(VecFile* files, Arguments* args) {
	if (files->length == 0) {
		return Success;
	}
	bool first = true;
	for (int i = 0; i < (int)files->length; i++) {
		if (!first) {
			printf("\n");
		}
		// - skip '.' if -a is not present
		if (ft_starts_with(files->table[i].path, ".")) {
			continue;
		}
		first = false;
		// output file
		// - output depends on options: -l
		printf("%s", files->table[i].path);
		// add to directories if -R specified and file is a directory
	}
	printf("\n");
	return Success;
}

ResultType process_dir(File* dir, Arguments* args) {
	// TODO: review assertion choice
	assert(dir->type == Directory);
	// for each direntry
	// add it to the list of files
	// process the files
	return Success;
}
