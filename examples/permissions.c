#include <stdio.h>
#include <sys/xattr.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"

#define BUFFER_SIZE 4096

static char owner_exec_mode(File* file) {
	if (file->info.st_mode & S_ISUID) {
		return file->info.st_mode & S_IXUSR ? 's' : 'S';
	} else if (file->info.st_mode & S_IXUSR) {
		return 'x';
	} else {
		return '-';
	}
}

static char group_exec_mode(File* file) {
	if (file->info.st_mode & S_ISGID) {
		return file->info.st_mode & S_IXGRP ? 's' : 'S';
	} else if (file->info.st_mode & S_IXGRP) {
		return 'x';
	} else {
		return '-';
	}
}

static char other_exec_mode(File* file) {
	if (file->info.st_mode & S_ISVTX) {
		return file->info.st_mode & S_IXOTH ? 't' : 'T';
	} else if (file->info.st_mode & S_IXOTH) {
		return 'x';
	} else {
		return '-';
	}
}

static void print_permissions(File* file) {
	printf("%c%c%c%c%c%c%c%c%c",
		file->info.st_mode & S_IRUSR ? 'r' : '-',
		file->info.st_mode & S_IWUSR ? 'w' : '-',
		owner_exec_mode(file),
		file->info.st_mode & S_IRGRP ? 'r' : '-',
		file->info.st_mode & S_IWGRP ? 'w' : '-',
		group_exec_mode(file),
		file->info.st_mode & S_IROTH ? 'r' : '-',
		file->info.st_mode & S_IWOTH ? 'w' : '-',
		other_exec_mode(file)
	);
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
		return 1;
	}
	argc--; argv++;

	File file;
	if (file_from_path(argv[0], &file) != Success) {
		perror("file_from_path");
		return 1;
	}

	printf("File: ");
	printf("\n");

	print_permissions(&file);
	printf("\n");
	file_destroy(&file);
	return 0;
}
