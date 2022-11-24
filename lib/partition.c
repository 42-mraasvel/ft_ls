#include "utils.h"
#include "file.h"
#include "parse_args.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

// partition input into files and directories
ResultType partition(Arguments* args, VecFile* files, VecFile* directories) {
	VecStr* input = args->files;
	for (int i = 0; i < (int)input->length; i++) {
		char* path = *vecstr_get(input, i);
		File file;
		ResultType result = file_from_path(path, &file);
		if (result == StatError) {
			fprintf(stderr, "%s: cannot access '%s': %s\n", args->program_name, path, strerror(errno));
			continue;
		} else if (result != Success) {
			return result;
		}
		if (file.type == Directory) {
			vecfile_push_back(directories, file);
		} else {
			vecfile_push_back(files, file);
		}
	}
	return Success;
}
