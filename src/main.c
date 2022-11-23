#include "parse_args.h"

#define ALLOWED_OPTIONS "lRart"

enum FileType {
	Directory,
	RegularFile,
	SymLink,
};

struct File {
	enum FileType type;
	const char* path;
};

int main(int argc, char* argv[]) {
	int result = Success;
	Arguments args;
	ResultType parse_result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	if (parse_result != Success) {
		return parse_result;
	}
	// // 1. partition files into directories and files -- report error on nonexisting files
	// VecFile* files;
	// VecFile* dirs;
	// if (partition_args(args.files, &files, &dirs) == SystemError) {
	// 	args_destroy(&args);
	// 	return SystemError;
	// }

	// // 2. sort list of files, go over list of files, print them out as if they are a single directory
	// process_files(files);
	// sort(dirs);

	// // 3. for each given directory (sorted), repeat the process of selecting files/directories and printing them out
	// for dir in dirs:
	// 	process_dir(dir)
	args_destroy(&args);
	return result;
}
