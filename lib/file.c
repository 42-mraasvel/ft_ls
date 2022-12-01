#include "file.h"
#include "ft_string.h"
#include "parse_args.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

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
	file->path = malloc_check(string_from(path));
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

static char filetype_to_mode(FileType type) {
	switch (type) {
		case BlockDevice:
			return 'b';
		case CharacterDevice:
			return 'c';
		case Directory:
			return 'd';
		case SymbolicLink:
			return 'l';
		case Socket:
			return 's';
		case Fifo:
			return 'p';
		default:
			return '-';
	}
}

bool is_special_file(File* a) {
	// a == "." || a == ".."
	return ft_strcmp(a->name, ".") == 0 || ft_strcmp(a->name, "..") == 0;
}

static const char* get_file_name(File* a) {
	return a->name;
}

int filecmp_by_name(File* a, File* b) {
	return ft_strcmp(get_file_name(a), get_file_name(b));
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
		return filecmp_by_name(a, b);
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

size_t file_sizes(VecFile* files) {
	size_t n = 0;
	for (int i = 0; i < (int)files->length; i++) {
		File* file = &files->table[i];
		n += file->info.st_blocks;
	}
	return n;
}

void file_display(File* file) {
	printf("%s", file->name);
}

/* Long Listing Format */

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

static void print_filemode(File* file) {
	printf("%c", filetype_to_mode(file->type));
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

static void print_nlinks(File* file) {
	printf(" %d", file->info.st_nlink);
}

static void print_owner(File* file) {
	struct passwd* pwd = getpwuid(file->info.st_uid);
	printf(" ");
	if (!pwd || !pwd->pw_name) {
		printf("%d", file->info.st_uid);
	} else {
		printf("%s", pwd->pw_name);
	}
}

static void print_group(File* file) {
	printf(" ");
	struct group* grp = getgrgid(file->info.st_gid);
	if (!grp || !grp->gr_name) {
		printf("%d", file->info.st_gid);
	} else {
		printf("%s", grp->gr_name);
	}
}

static void print_filesize(File* file) {
	printf(" %lld", file->info.st_size);
}

static void print_date_last_modified(File* file) {
	// 6 months
	printf(" ");
	// MONTH DAY [ YEARS if last_modified > 6 months ELSE HOUR:MINUTE:SECOND ]

	// "DDD MMM DD HH:MM:SS YYYY\n\0"
	char* date = ctime(&file->info.st_mtimespec.tv_sec);
	printf("%.3s %.2s ", date + 4, date + 8);

	// print year if more than 6 months ago, otherwise print HH:MM:SS
	time_t t = time(NULL);
	const long SIX_MONTHS = 60 * 60 * 24 * 31 * 6;
	if (t - SIX_MONTHS > file->info.st_mtimespec.tv_sec) {
		printf("%.4s", date + 20);
	} else {
		printf("%.5s", date + 11);
	}
	printf(" ");
}

void file_display_long(File* file) {
	print_filemode(file);
	print_permissions(file);
	// print extended attributes
	print_nlinks(file);
	print_owner(file);
	print_group(file);
	print_filesize(file);
	print_date_last_modified(file);
	file_display(file);
}

MONOVEC_DEFINITIONS(File, VecFile, vecfile);
MONOVEC_DEFINITIONS(int, VecInt, vecint);
MONOVEC_DEFINITIONS(String*, VecString, vecstring);
MONOVEC_DEFINITIONS(LongListingRow, VecRow, vecrow);

typedef String* (*ColumnStringMaker)(File*);

static String* string_mode(File* file) {
	// TODO: add extended attributes
	return malloc_check(string_format("%c%c%c%c%c%c%c%c%c%c",
		filetype_to_mode(file->type),
		file->info.st_mode & S_IRUSR ? 'r' : '-',
		file->info.st_mode & S_IWUSR ? 'w' : '-',
		owner_exec_mode(file),
		file->info.st_mode & S_IRGRP ? 'r' : '-',
		file->info.st_mode & S_IWGRP ? 'w' : '-',
		group_exec_mode(file),
		file->info.st_mode & S_IROTH ? 'r' : '-',
		file->info.st_mode & S_IWOTH ? 'w' : '-',
		other_exec_mode(file)
	));
}

static String* string_nlinks(File* file) {
	return malloc_check(string_format("%d", file->info.st_nlink));
}

static void long_listing_fill_row(LongListingRow* row, File* file) {
	row->columns = malloc_check(vecstring_construct(10));

	static const ColumnStringMaker makers[] = {
		[MODE] = string_mode,
		[NLINKS] = string_nlinks
		// OWNER_NAME,
		// GROUP_NAME,
		// FILE_SIZE,
		// DATE_MONTH,
		// DATE_DAY,
		// DATE_TIME, // Year or HH:MM
		// FILE_NAME
	};

	for (int i = 0; i < (int)(sizeof(makers) / sizeof(ColumnStringMaker)); i++) {
		String* s = (makers[i])(file);
		if (vecstring_push_back(row->columns, s) == -1) {
			abort_program("malloc");
		}
	}
}

static void long_listing_fill_rows(LongListing* listing, VecFile* files) {
	for (int i = 0; i < (int)files->length; i++) {
		long_listing_fill_row(&listing->rows->table[i], &files->table[i]);
	}
}

static void long_listing_fill_padding(LongListing* listing) {
	int columns = listing->rows->table[0].columns->length;
	for (int column = 0; column < columns; column++) {
		int max = 0;
		for (int row = 0; row < (int)listing->rows->length; row++) {
			max = ft_max(max, listing->rows->table[row].columns->table[column]->chars->length);
		}
		if (vecint_push_back(listing->padding, max) == -1) {
			abort_program("malloc");
		}
	}
}

LongListing* create_long_listing(VecFile* files) {
	if (files->length == 0) {
		return NULL;
	}
	LongListing* listing = malloc_check(malloc(1 * sizeof(LongListing)));
	listing->files = files;
	listing->rows = malloc_check(vecrow_construct(files->length));
	listing->padding = malloc_check(vecint_construct(files->length));
	long_listing_fill_rows(listing, files);
	long_listing_fill_padding(listing);
	return listing;
}

static void string_destroy_double(String** s) {
	string_destroy(*s);
}

static void long_listing_row_destroy(LongListingRow* row) {
	if (!row) {
		return;
	}
	vecstring_destroy_with(row->columns, string_destroy_double);
}

void long_listing_destroy(LongListing* listing) {
	if (!listing) {
		return;
	}
	vecrow_destroy_with(listing->rows, long_listing_row_destroy);
	vecint_destroy(listing->padding);
}

void long_listing_print(LongListing* listing, int index) {
	for (int i = 0; i < (int)listing->rows->length; i++) {
		LongListingRow* row = &listing->rows->table[i];
		int minimum_field_width = listing->padding->table[i];
		for (int col = 0; col < (int)row->columns->length; col++) {
			printf("%*s", minimum_field_width, string_cstr(row->columns->table[col]));
		}
	}
}
