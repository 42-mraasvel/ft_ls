#include "utils.h"
#include <stdarg.h>
#include <stdio.h>

MONOVEC_DEFINITIONS(char*, VecStr, vecstr);

static const char* program_name = NULL;

void set_program_name(const char* name) {
	program_name = name;
}

void format_error(const char* fmt, ...) {
	if (program_name) {
		fprintf(stderr, "%s: ", program_name);
	}
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
