#include "file.h"
#include "ft_string.h"
#include "parse_args.h"
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
// note: PATH will be NULL in the returned file
ResultType file_from_path(const char* path, File* file) {
	if (stat(path, &file->info) == -1) {
		return StatError;
	}
	file->name = malloc_check(ft_strdup(path));
	file->path = NULL;
	file->type = get_filetype(file);
	return Success;
}

ResultType file_from_dirent(const char* parent, struct dirent* entry, File* file) {
	file->name = malloc_check(ft_strdup(entry->d_name));
	file->path = string_format("%s/%s", parent, file->name);
	if (stat(string_cstr(file->path), &file->info) == -1) {
		file_destroy(file);
		return StatError;
	}
	file->type = get_filetype(file);
	return Success;
}

void file_destroy(File* file) {
	free(file->name);
	string_destroy(file->path);
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

int filecmp_by_name(File* a, File* b) {
	return ft_strcmp(a->name, b->name);
}

int filecmp_by_name_reverse(File* a, File* b) {
	return filecmp_by_name(b, a);
}

int filecmp_by_time(File* a, File* b) {
	if (a->info.st_mtime > b->info.st_mtime) {
		return -1;
	} else if (a->info.st_mtime < b->info.st_mtime) {
		return 1;
	} else {
		return 0;
	}
}

int filecmp_by_time_reverse(File* a, File* b) {
	return filecmp_by_time(b, a);
}

void sort_files(VecFile* files, Arguments* args) {
	if (args->options['r']) {
		if (args->options['t']) {
			vecfile_sort_unstable_by(files, filecmp_by_time_reverse);
		} else {
			vecfile_sort_unstable_by(files, filecmp_by_name_reverse);
		}
	} else if (args->options['t']) {
		vecfile_sort_unstable_by(files, filecmp_by_time);
	} else {
		vecfile_sort_unstable_by(files, filecmp_by_name);
	}
}

void print_file(File* file) {
	printf("File: ");
	if (file->name) {
		printf("Path('%s'), ", file->name);
	}
	printf("Type('%s')", filetype_to_string(file->type));
	printf("\n");
}

MONOVEC_DEFINITIONS(File, VecFile, vecfile);
