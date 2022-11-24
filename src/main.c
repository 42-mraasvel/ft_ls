#include "parse_args.h"
#include "file.h"
#include <stdio.h> //REMOVE

#define ALLOWED_OPTIONS "lRart"

ResultType partition(Arguments* args, VecFile* files, VecFile* directories);

ResultType do_partition(Arguments* args, VecFile** files, VecFile** directories) {
	*directories = vecfile_construct(args->files->length);
	*files = vecfile_construct(args->files->length);
	if (!directories || !files) {
		vecfile_destroy(*directories);
		vecfile_destroy(*files);
		return SystemError;
	}
	return partition(args, *files, *directories);
}

int main(int argc, char* argv[]) {
	int result = Success;
	Arguments args;
	ResultType parse_result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	if (parse_result != Success) {
		return parse_result;
	}
	// 1. partition files into directories and files -- report error on nonexisting files
	VecFile* files = NULL;
	VecFile* directories = NULL;
	if (do_partition(&args, &files, &directories) != Success) {
		args_destroy(&args);
		return SystemError;
	}

	printf("-- Files --\n");
	for (int i = 0; i < files->length; i++) {
		print_file(&files->table[i]);
	}

	printf("-- Dirs --\n");
	for (int i = 0; i < directories->length; i++) {
		print_file(&directories->table[i]);
	}

	// vecfile_sort_unstable_by(files, filecmp_by_path);
	// vecfile_sort_unstable_by(directories, filecmp_by_path);

	// 2. sort list of files, go over list of files, print them out as if they are a single directory
	// process_files(files);
	// sort(dirs);

	// 3. for each given directory (sorted), repeat the process of selecting files/directories and printing them out
	// for dir in dirs:
	// 	process_dir(dir)
	vecfile_destroy(files);
	vecfile_destroy(directories);
	args_destroy(&args);
	return result;
}
