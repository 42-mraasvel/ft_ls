#ifndef FILE_H
#define FILE_H

#include "utils.h"
#include "parse_args.h"
#include "ft_string.h"
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
	char* date;
	char* referenced_name;
	bool has_extended_attributes;
} File;

typedef enum LongListingIndex {
	MODE = 0,
	NLINKS = 1,
	OWNER_NAME = 2,
	GROUP_NAME = 3,
	FILE_SIZE = 4,
	DATE_MONTH = 5,
	DATE_DAY = 6,
	DATE_TIME = 7, // Year or HH:MM
	FILE_NAME = 8
} LongListingIndex;

MONOVEC_DECLARATION(int, VecInt, vecint);
MONOVEC_DECLARATION(String*, VecString, vecstring);

typedef struct LongListingRow {
	VecString* columns;
} LongListingRow;

MONOVEC_DECLARATION(LongListingRow, VecRow, vecrow);
MONOVEC_DECLARATION(File, VecFile, vecfile);

typedef struct LongListing {
	VecFile* files;
	// Vector of rows, each row has a list of columns
	VecRow* rows;
	// Vector of min field width (max string len) for each row
	VecInt* padding;
} LongListing;

LongListing* create_long_listing(VecFile* files);
void long_listing_print(LongListing* listing, int index);
void long_listing_destroy(LongListing* listing);

void sort_files(VecFile* files, Arguments* args);
int filecmp_by_name(File* a, File* b);
bool is_special_file(File* a);

// [WARNING] path given as variable is returned, make sure it stays allocated
ResultType file_from_path(const char* path, File* out);
ResultType file_from_dirent(const char* parent, struct dirent* entry, File* file);
void file_destroy(File* file);
void file_display(File* file);
size_t file_sizes(VecFile* files);

#endif /* FILE_H */
