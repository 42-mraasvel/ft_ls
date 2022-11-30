#ifndef FILE_H
#define FILE_H

#include "utils.h"
#include "parse_args.h"
#include <sys/stat.h>
#include <dirent.h>

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
	char* name;
	String* path;
	struct stat info;
} File;


MONOVEC_DECLARATION(File, VecFile, vecfile);

void sort_files(VecFile* files, Arguments* args);
int filecmp_by_name(File* a, File* b);
bool is_special_file(File* a);

// [WARNING] path given as variable is returned, make sure it stays allocated
ResultType file_from_path(const char* path, File* out);
ResultType file_from_dirent(const char* parent, struct dirent* entry, File* file);
void file_destroy(File* file);
void print_file(File* file);
void file_display(File* file);
void file_display_long(File* file);
size_t file_sizes(VecFile* files);

#endif /* FILE_H */
