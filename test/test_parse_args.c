#include <criterion/criterion.h>
#include "parse_args.h"
#include "libft.h"

const char* ALLOWED_OPTIONS = "lRart";

/*
Input:
- argv contains no files
- argv contains one file
- argv contains multiple files
- '-' but no option (is read as file)

- no options
- single option after `-`
- multiple options after `-`
- option that is not allowed after `-` [EXCEPTION - ERROR]
- option not allowed nested in other allowed options [EXCEPTION - ERROR]

3x3 + 2 = 11 testcases
*/

static bool only_given_options_enabled(const bool* options, const char* enabled) {
	for (int i = 1; i < 255; i++) {
		bool contains = strchr(enabled, i) != NULL;
		if (contains != options[i]) {
			return false;
		}
	}
	return true;
}

static bool vecstr_contains(const VecStr* vec, const char* s) {
	for (int i = 0; i < vec->length; i++) {
		if (strcmp(vec->table[i], s) == 0) {
			return true;
		}
	}
	return false;
}

static bool only_contains_filenames(const VecStr* files, const char** filenames) {
	for (int i = 0; filenames[i] != NULL; i++) {
		if (!vecstr_contains(files, filenames[i])) {
			return false;
		}
	}
	return true;
}

Test(no_files_no_options, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);
	cr_assert(args.files->length == 0);
	cr_assert(only_given_options_enabled(args.options, ""));
}

Test(no_files_one_option, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-l", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);
	cr_assert(args.files->length == 0);
	cr_assert(only_given_options_enabled(args.options, "l"));
}

Test(no_files_multiple_options, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-lRt", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);
	cr_assert(args.files->length == 0);
	cr_assert(only_given_options_enabled(args.options, "lRt"));
}

Test(one_file_no_options, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "filename", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);

	cr_assert(args.files->length == 1);
	cr_assert(only_given_options_enabled(args.options, ""));
	const char* filenames[] = { "filename", NULL };
	cr_assert(only_contains_filenames(args.files, filenames));
}

Test(one_file_one_option, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "filename", "-l", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);

	cr_assert(args.files->length == 1);
	cr_assert(only_given_options_enabled(args.options, "l"));
	const char* filenames[] = { "filename", NULL };
	cr_assert(only_contains_filenames(args.files, filenames));
}

Test(one_file_multiple_options, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-r", "filename", "-lR", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);

	cr_assert(args.files->length == 1);
	cr_assert(only_given_options_enabled(args.options, "rlR"));
	const char* filenames[] = { "filename", NULL };
	cr_assert(only_contains_filenames(args.files, filenames));
}

Test(multiple_files_no_options, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-", "filename", "filename2", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);

	cr_assert(args.files->length == 3);
	cr_assert(only_given_options_enabled(args.options, ""));
	const char* filenames[] = { "-", "filename", "filename2", NULL };
	cr_assert(only_contains_filenames(args.files, filenames));
}

Test(multiple_files_one_option, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-", "-t", "filename", "x", "y", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);

	cr_assert(args.files->length == 4);
	cr_assert(only_given_options_enabled(args.options, "t"));
	const char* filenames[] = { "-", "filename", "x", "y", NULL };
	cr_assert(only_contains_filenames(args.files, filenames));
}

Test(multiple_files_multiple_options, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-", "-t", "filename", "x", "y", "-lR", "-t", "t", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == Success);

	cr_assert(args.files->length == 5);
	cr_assert(only_given_options_enabled(args.options, "tlR"));
	const char* filenames[] = { "-", "filename", "x", "y", "t", NULL };
	cr_assert(only_contains_filenames(args.files, filenames));
}

Test(forbidden_option, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-l", "-z", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == ArgumentError);
}

Test(forbidden_option_nested, test_parse_args) {
	Arguments args;
	char* argv[] = { "program", "-lzR", NULL };
	size_t argc = ft_count_strings(argv);
	int result = parse_args(&args, argc, argv, ALLOWED_OPTIONS);
	cr_assert(result == ArgumentError);
}
