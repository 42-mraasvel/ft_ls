#include "parse_args.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

static ResultType parse_options(Arguments* args, const char* options, const char* allowed_options) {
	for (int i = 0; options[i] != '\0'; i++) {
		unsigned int option = (unsigned char)options[i];
		if (ft_strchr(allowed_options, options[i]) == NULL) {
			format_error("invalid option -- %c\n", options[i]);
			fprintf(stderr, "usage: ls [-@ABCFGHILOPRSTUWabcdefghiklmnopqrstuvwxy1%%,] [--color=when] [-D format] [file ...]\n");
			return ArgumentError;
		}
		args->options[option] = true;
	}
	return Success;
}

// invariants
// - args is NOT NULL and uninitialized
// - argc is the number of strings in argv
// - allowed_options is NOT NULL
ResultType parse_args(Arguments* args, int argc, char *argv[], const char* allowed_options) {
	args->files = malloc_check(vecstr_construct(0));
	// skip program name
	argc--; argv++;
	ft_memset(args->options, false, 255 * sizeof(bool));
	for (int i = 0; i < argc; i++) {
		if (ft_starts_with(argv[i], "-") && ft_strlen(argv[i]) > 1) {
			// option
			if (parse_options(args, argv[i] + 1, allowed_options) != Success) {
				args_destroy(args);
				return ArgumentError;
			}
		} else {
			// file argument
			if (vecstr_push_back(args->files, argv[i]) != 0) {
				abort_program("malloc");
			}
		}
	}
	return Success;
}

void args_destroy(Arguments* ptr) {
	vecstr_destroy(ptr->files);
}

void print_args(Arguments* args) {
	printf("-- Arguments --\n");
	printf("Files: [ ");
	for (int i = 0; i < (int)args->files->length; i++) {
		printf("`%s`, ", args->files->table[i]);
	}
	printf(" ]\n");
	printf("Options: [ ");
	for (int i = 0; i < 255; i++) {
		if (args->options[i]) {
			printf("%c, ", (unsigned char)i);
		}
	}
	printf(" ]\n-- End Arguments --\n");
}
