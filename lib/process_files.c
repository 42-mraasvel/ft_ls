#include "utils.h"
#include "parse_args.h"
#include "file.h"
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

ResultType process_dir(File* dir, Arguments* args);

// print out files
// recursively process directories if -R
ResultType process_files(VecFile* files, Arguments* args) {
	if (files->length == 0) {
		return Success;
	}
	VecFile* nested_directories = malloc_check(vecfile_construct(0));
	for (int i = 0; i < (int)files->length; i++) {
		File* file = &files->table[i];
		// - skip '.' if -a is not present
		if (!args->options['a'] && ft_starts_with(file->name, ".")) {
			continue;
		}
		// output file
		// - output depends on options: -l
		file_display(file);
		// add to directories if -R specified and file is a directory which is not `.` or `..`
		if (args->options['R'] && file->type == Directory && !is_special_file(file)) {
			if (vecfile_push_back(nested_directories, *file) == -1) {
				abort_program("malloc");
			}
		}
		printf("\n");
	}

	sort_files(nested_directories, args);
	for (int i = 0; i < (int)nested_directories->length; i++) {
		printf("\n");
		File* dir = &nested_directories->table[i];
		printf("%s:\n", string_cstr(dir->path));
		process_dir(dir, args);
	}
	vecfile_destroy(nested_directories);

	return Success;
}

ResultType process_dir(File* dir, Arguments* args) {
	// TODO: review assertion choice
	assert(dir->type == Directory);
	const char* dir_path = dir->name;
	if (dir->path) {
		dir_path = string_cstr(dir->path);
	}
	DIR* dirp = opendir(dir_path);
	if (dirp == NULL) {
		//TODO: review error message, returned error code
		format_error("cannot open directory '%s': %s\n", dir_path, strerror(errno));
		return SystemError;
	}
	VecFile* files = malloc_check(vecfile_construct(2));
	struct dirent* entry;
	while ((entry = readdir(dirp))) {
		File file;
		if (file_from_dirent(dir_path, entry, &file) != Success) {
			format_error("cannot access file '%s': %s\n", entry->d_name, strerror(errno));
			continue;
		}
		if (vecfile_push_back(files, file) == -1) {
			abort_program("malloc");
		}
	}
	closedir(dirp);
	sort_files(files, args);
	ResultType result = process_files(files, args);
	vecfile_destroy_with(files, file_destroy);
	return result;
}
