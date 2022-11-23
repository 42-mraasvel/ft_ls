#include "parse_args.h"
#include "libft.h"

static int parse_options(Arguments* args, const char* options, const char* allowed_options) {
	for (int i = 0; options[i] != '\0'; i++) {
		char option = (unsigned char)options[i];
		if (ft_strchr(allowed_options, options[i]) == NULL) {
			return -1;
		}
		args->options[option] = true;
	}
	return 0;
}

int parse_args(Arguments* args, int argc, char *argv[], const char* allowed_options) {
	// skip program name
	argc--; argv++;
	args->files = vecstr_construct(0);
	if (!args->files) {
		return -1;
	}
	for (int i = 0; i < argc; i++) {
		// ft_startswith(argv[i], "-");
		if (ft_starts_with(argv[i], "-")) {
			// option
			if (parse_options(args, argv[i] + 1, allowed_options) != 0) {
				goto ERROR;
			}
		} else {
			// file argument
			if (vecstr_push_back(args->files, argv[i]) != 0) {
				goto ERROR;
			}
		}
	}
	return 0;
ERROR:
	args_destroy(args);
	return -1;
}

void args_destroy(Arguments* ptr) {
	vecstr_destroy(ptr->files);
}
