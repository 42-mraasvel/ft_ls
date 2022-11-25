#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include "utils.h"
#include <stdbool.h>

typedef struct Arguments {
	// simple ascii table to signal which options are present
	bool options[255];
	// contains non-option arguments: files to check or explore
	// empty if none are present
	VecStr* files;
} Arguments;

ResultType parse_args(Arguments* args, int argc, char *argv[], const char* allowed_options);
void args_destroy(Arguments* ptr);
void print_args(Arguments* args);

#endif /* PARSE_ARGS_H */
