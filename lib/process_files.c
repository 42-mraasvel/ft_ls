#include "utils.h"
#include "parse_args.h"
#include "file.h"
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

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

	DIR* dirp = opendir(dir->path);
	if (dirp == NULL) {
		//TODO: review error message, error code
		fprintf(stderr, "%s: cannot open directory: %s: %s\n", args->program_name, dir->path, strerror(errno));
		return SystemError;
	}
	VecFile* files = vecfile_construct(2);
	if (files == NULL) {
		fprintf(stderr, "%s: error: %s\n", args->program_name, strerror(errno));
		closedir(dirp);
		return SystemError;
	}
	struct dirent* entry;
	while ((entry = readdir(dirp))) {
		File file;
		if (file_from_dirent(dir->path, entry, &file) != Success) {
			fprintf(stderr, "%s: cannot access file: %s: %s\n", args->program_name, entry->d_name, strerror(errno));
			continue;
		}
		vecfile_push_back(files, file);
	}
	closedir(dirp);
	vecfile_sort_unstable_by(files, filecmp_by_path);
	ResultType result = process_files(files, args);
	vecfile_destroy(files);
	return result;
}
