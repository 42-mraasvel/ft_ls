#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

// invariant: stat info is initialized
static FileType get_filetype(File* file) {
	FileType type = Unknown;
	if (S_ISREG(file->info.st_mode)) {
		type = RegularFile;
	} else if (S_ISDIR(file->info.st_mode)) {
		type = Directory;
	} else if (S_ISCHR(file->info.st_mode)) {
		type = CharacterDevice;
	} else if (S_ISBLK(file->info.st_mode)) {
		type = BlockDevice;
	} else if (S_ISFIFO(file->info.st_mode)) {
		type = Fifo;
	} else if (S_ISLNK(file->info.st_mode)) {
		type = SymbolicLink;
	} else if (S_ISSOCK(file->info.st_mode)) {
		type = Socket;
	}
	return type;
}

// invariant: file is not NULL
ResultType file_from_path(const char* path, File* file) {
	file->path = path;
	if (stat(path, &file->info) == -1) {
		return StatError;
	}
	file->type = get_filetype(file);
	return Success;
}

static const char* filetype_to_string(FileType type) {
	switch (type) {
		case Unknown:
			return "Unknown";
		case Directory:
			return "Directory";
		case RegularFile:
			return "RegularFile";
		case CharacterDevice:
			return "CharacterDevice";
		case BlockDevice:
			return "BlockDevice";
		case Fifo:
			return "Fifo";
		case SymbolicLink:
			return "SymbolicLink";
		case Socket:
			return "Socket";
	}
	return "";
}

int filecmp_by_path(File* a, File* b) {
	return ft_strcmp(a->path, b->path);
}

void print_file(File* file) {
	printf("File: ");
	if (file->path) {
		printf("Path('%s'), ", file->path);
	}
	printf("Type('%s')", filetype_to_string(file->type));
	printf("\n");
}

MONOVEC_DEFINITIONS(File, VecFile, vecfile);
