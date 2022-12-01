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
	file->date = NULL;
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
	file->date = NULL;
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

MONOVEC_DEFINITIONS(File, VecFile, vecfile);
MONOVEC_DEFINITIONS(int, VecInt, vecint);
MONOVEC_DEFINITIONS(String*, VecString, vecstring);
MONOVEC_DEFINITIONS(LongListingRow, VecRow, vecrow);

typedef String* (*ColumnStringMaker)(File*);

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

static String* string_mode(File* file) {
	// TODO: add extended attributes
	return string_format("%c%c%c%c%c%c%c%c%c%c%c",
		filetype_to_mode(file->type),
		file->info.st_mode & S_IRUSR ? 'r' : '-',
		file->info.st_mode & S_IWUSR ? 'w' : '-',
		owner_exec_mode(file),
		file->info.st_mode & S_IRGRP ? 'r' : '-',
		file->info.st_mode & S_IWGRP ? 'w' : '-',
		group_exec_mode(file),
		file->info.st_mode & S_IROTH ? 'r' : '-',
		file->info.st_mode & S_IWOTH ? 'w' : '-',
		other_exec_mode(file),
		' '
	);
}

static String* string_nlinks(File* file) {
	return string_format("%d", file->info.st_nlink);
}

static String* string_owner_name(File* file) {
	struct passwd* pwd = getpwuid(file->info.st_uid);
	if (!pwd || !pwd->pw_name) {
		return string_format("%d", file->info.st_uid);
	} else {
		return string_format("%s", pwd->pw_name);
	}
}

static String* string_group_name(File* file) {
	struct group* grp = getgrgid(file->info.st_gid);
	if (!grp || !grp->gr_name) {
		return string_format(" %d", file->info.st_gid);
	} else {
		return string_format(" %s", grp->gr_name);
	}
}

static String* string_filesize(File* file) {
	return string_format(" %d", (int)file->info.st_size);
}

static String* string_month(File* file) {
	return string_format("%.3s", file->date + 4);
}

static String* string_day(File* file) {
	return string_format("%.2s", file->date + 8);
}

static String* string_time(File* file) {
	time_t t = time(NULL);
	const long SIX_MONTHS = 60 * 60 * 24 * 31 * 6;
	if (t - SIX_MONTHS > file->info.st_mtimespec.tv_sec) {
		return string_format("%.4s", file->date + 20);
	} else {
		return string_format("%.5s", file->date + 11);
	}
}

static String* string_filename(File* file) {
	return string_format(" %s", file->name);
}

static LongListingRow long_listing_row_create(File* file) {
	static const ColumnStringMaker makers[] = {
		string_mode,
		string_nlinks,
		string_owner_name,
		string_group_name,
		string_filesize,
		string_month,
		string_day,
		string_time,
		string_filename
	};

	LongListingRow row;
	file->date = ctime(&file->info.st_mtimespec.tv_sec);
	row.columns = malloc_check(vecstring_construct(10));
	for (int i = 0; i < (int)(sizeof(makers) / sizeof(ColumnStringMaker)); i++) {
		String* s = malloc_check((makers[i])(file));
		if (vecstring_push_back(row.columns, s) == -1) {
			abort_program("malloc");
		}
	}
	return row;
}

static void long_listing_fill_rows(LongListing* listing, VecFile* files) {
	for (int i = 0; i < (int)files->length; i++) {
		LongListingRow row = long_listing_row_create(&files->table[i]);
		if (vecrow_push_back(listing->rows, row) == -1) {
			abort_program("malloc");
		}
	}
}

static void long_listing_fill_padding(LongListing* listing) {
	int columns = listing->rows->table[0].columns->length;
	for (int column = 0; column < columns; column++) {
		int max = 0;
		for (int row = 0; row < (int)listing->rows->length; row++) {
			int len = (int)string_length(listing->rows->table[row].columns->table[column]);
			max = ft_max(max, len);
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
	LongListingRow* row = &listing->rows->table[index];
	for (int col = 0; col < (int)row->columns->length; col++) {
		int minimum_field_width = listing->padding->table[col];
		if (col >= FILE_NAME) {
			minimum_field_width = 0;
		} else if (col > 0) {
			minimum_field_width += 1;
		}
		printf("%*s", minimum_field_width, string_cstr(row->columns->table[col]));
	}
}
