#include "parse_args.h"
#include "file.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define ALLOWED_OPTIONS "lRart"

ResultType partition(Arguments* args, VecStr* input, VecFile* files, VecFile* directories);
ResultType process_directory(File* dir, Arguments* args, bool always_print_name);

static void process_directories(Arguments* args, VecFile* directories) {
	for (int i = 0; i < (int)directories->length; i++) {
		if (i != 0) {
			printf("\n");
		}
		File* dir = &directories->table[i];
		process_directory(dir, args, args->files->length > 1);
	}
}

static ResultType process_arguments(Arguments* args) {
	// 1. partition arguments into directories and files -- report but ignore error on inaccessable files
	VecFile* files = malloc_check(vecfile_construct(args->files->length));;
	VecFile* directories = malloc_check(vecfile_construct(args->files->length));;
	if (partition(args, args->files, files, directories) != Success) {
		return SystemError;
	}
	// 2. sort and process arguments
	// sort is affected by -r, -t (reverse sort, sort by time last modified)
	sort_files(files, args);
	sort_files(directories, args);
	ResultType process_files(VecFile* files, Arguments* args);
	if (process_files(files, args) != Success) {
		// TODO: can it fail?
		abort();
	}
	if (files->length > 0 && directories->length > 0) {
		printf("\n");
	}
	process_directories(args, directories);
	vecfile_destroy_with(files, file_destroy);
	vecfile_destroy_with(directories, file_destroy);
	return Success;
}

static char* program_name;

static int sort_filenames(char** a, char** b) {
	return ft_strcmp(*a, *b);
}

int main(int argc, char* argv[]) {
#ifdef TEST_BUILD
	set_program_name("ls");
#else
	set_program_name(argv[0]);
#endif
	int result = Success;
	Arguments args;
	ResultType parse_result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	if (parse_result != Success) {
		return GeneralError;
	}
	VecStr* input = args.files;
	if (args.files->length == 0) {
		File cwd;
		if (file_from_path(".", &cwd) != Success) {
			format_error("%s: %s\n", ".", strerror(errno));
			return GeneralError;
		}
		result = process_directory(&cwd, &args, false);
		file_destroy(&cwd);
	} else {
		// sort filenames for error message on macOS being in sorted order
		vecstr_sort_unstable_by(args.files, sort_filenames);
		result = process_arguments(&args);
	}
	args_destroy(&args);
	return result;
}
