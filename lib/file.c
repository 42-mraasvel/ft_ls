#include "file.h"
#include "ft_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

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
	file->parent = NULL;
	if (stat(path, &file->info) == -1) {
		return StatError;
	}
	file->type = get_filetype(file);
	return Success;
}

ResultType file_from_dirent(const char* parent, struct dirent* entry, File* file) {
	//TODO: leaking data, need memory allocation protection and general memory allocation management plan
	//Best way to handle it is probably to either tag as `Owned/Borrowed` and create a general file_destroy function
	//Then Vec can have a vec_destroy_with, which takes a delete function
	//That way the wrapping/overarching code doesn't have to deal with it: it's delegated to destructor functions
	file->path = ft_strdup(entry->d_name);
	file->parent = parent;
	String* full_path = string_format("%s/%s", parent, entry->d_name);
	if (stat(string_cstr(full_path), &file->info) == -1) {
		return StatError;
	}
	string_destroy(full_path);
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
