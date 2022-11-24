#ifndef FILE_H
#define FILE_H

#include "utils.h"
#include <sys/stat.h>

typedef enum FileType {
	Unknown,
	Directory,
	RegularFile,
	CharacterDevice,
	BlockDevice,
	Fifo,
	SymbolicLink,
	Socket
} FileType;

typedef struct File {
	enum FileType type;
	const char* path;
	struct stat info;
} File;

MONOVEC_DECLARATION(File, VecFile, vecfile);

// [WARNING] path given as variable is returned, make sure it stays allocated
ResultType file_from_path(const char* path, File* out);
void print_file(File* file);
// File file_from_dirent()

#endif /* FILE_H */
