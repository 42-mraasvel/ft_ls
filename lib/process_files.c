#include "utils.h"
#include "parse_args.h"
#include "file.h"
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

ResultType process_directory(File* dir, Arguments* args, bool always_print_name);

static bool should_handle_file(File* file, Arguments* args) {
	// skip hidden files if '-a' not present
	return args->options['a'] || !file->name || !ft_starts_with(file->name, ".");
}

static void process_directories(VecFile* directories, Arguments* args) {
	sort_files(directories, args);
	for (int i = 0; i < (int)directories->length; i++) {
		printf("\n");
		File* dir = &directories->table[i];
		process_directory(dir, args, true);
	}
	vecfile_destroy(directories);
}

// print out files
// recursively process directories if -R
ResultType process_files(VecFile* files, Arguments* args) {
	if (files->length == 0) {
		return Success;
	}
	VecFile* nested_directories = malloc_check(vecfile_construct(0));
	for (int i = 0; i < (int)files->length; i++) {
		File* file = &files->table[i];
		// output file
		// - output depends on options: -l
		if (args->options['l']) {
			file_display_long(file);
		} else {
			file_display(file);
		}
		// add to directories if -R specified and file is a directory which is not `.` or `..`
		if (args->options['R'] && file->type == Directory && !is_special_file(file)) {
			if (vecfile_push_back(nested_directories, *file) == -1) {
				abort_program("malloc");
			}
		}
		printf("\n");
	}
	process_directories(nested_directories, args);
	return Success;
}

ResultType process_directory(File* dir, Arguments* args, bool always_print_name) {
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
			format_error("%s: %s\n", entry->d_name, strerror(errno));
			continue;
		} else if (!should_handle_file(&file, args)) {
			file_destroy(&file);
			continue;
		}
		if (vecfile_push_back(files, file) == -1) {
			abort_program("malloc");
		}
	}
	closedir(dirp);
	// Determine if we are printing the filename
#ifdef __linux__
	if (always_print_name || args->options['R']) {
#else
	if (always_print_name) {
#endif
		printf("%s:\n", dir_path);
	}
	sort_files(files, args);
	if (args->options['l']) {
		size_t n = file_sizes(files);
		printf("total %lu\n", n);
	}
	ResultType result = process_files(files, args);
	vecfile_destroy_with(files, file_destroy);
	return result;
}
