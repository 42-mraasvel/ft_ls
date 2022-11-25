#include "parse_args.h"
#include "file.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define ALLOWED_OPTIONS "lRart"

ResultType partition(Arguments* args, VecStr* input, VecFile* files, VecFile* directories);
ResultType process_dir(File* dir, Arguments* args);

static ResultType do_partition(Arguments* args, VecStr* input, VecFile** files, VecFile** directories) {
	*directories = vecfile_construct(input->length);
	*files = vecfile_construct(input->length);
	if (!directories || !files) {
		vecfile_destroy(*directories);
		vecfile_destroy(*files);
		return SystemError;
	}
	return partition(args, args->files, *files, *directories);
}

static ResultType process_arguments(Arguments* args) {
	// 1. partition arguments into directories and files -- report but ignore error on inaccessable files
	VecFile* files = NULL;
	VecFile* directories = NULL;
	if (do_partition(args, args->files, &files, &directories) != Success) {
		return SystemError;
	}
	// 2. sort and process arguments
	// sort is affected by -r, -t (reverse sort, sort by time last modified)
	vecfile_sort_unstable_by(files, filecmp_by_path);
	vecfile_sort_unstable_by(directories, filecmp_by_path);
	ResultType process_files(VecFile* files, Arguments* args);
	if (process_files(files, args) != Success) {
		// TODO: can it fail?
		abort();
	}
	if (files->length > 0 && directories->length > 0) {
		printf("\n");
	}
	// Process given directories
	for (int i = 0; i < (int)directories->length; i++) {
		if (i != 0) {
			printf("\n");
		}
		File* dir = &directories->table[i];
		if (args->files->length > 1) {
			printf("%s:\n", dir->path);
		}
		process_dir(dir, args);
	}
	vecfile_destroy(files);
	vecfile_destroy(directories);
}

static char* program_name;

int main(int argc, char* argv[]) {
	set_program_name(argv[0]);
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
			format_error("cannot access '%s': %s\n", ".", strerror(errno));
			return GeneralError;
		}
		result = process_dir(&cwd, &args);
	} else {
		result = process_arguments(&args);
	}
	args_destroy(&args);
	return result;
}
