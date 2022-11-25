#include "utils.h"
#include "file.h"
#include "parse_args.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

// partition input into files and directories
// invariant: files/directories should have at least enough capacity to hold all input files
ResultType partition(Arguments* args, VecStr* input, VecFile* files, VecFile* directories) {
	for (int i = 0; i < (int)input->length; i++) {
		char* path = *vecstr_get(input, i);
		File file;
		ResultType result = file_from_path(path, &file);
		if (result == StatError) {
			format_error("cannot access '%s': %s\n", path, strerror(errno));
			continue;
		} else if (result != Success) {
			return result;
		}
		// allocation push_back errors are not checked because files/directories are allocated with enough capacity
		if (file.type == Directory) {
			vecfile_push_back(directories, file);
		} else {
			vecfile_push_back(files, file);
		}
	}
	return Success;
}
