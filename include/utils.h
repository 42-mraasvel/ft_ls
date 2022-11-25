#ifndef UTILS_H
#define UTILS_H

#include "libft.h"

MONOVEC_DECLARATION(char*, VecStr, vecstr);

typedef enum ResultType {
	Success = 0,
	SystemError = 1,
	GeneralError = 2,
	ArgumentError = 3,
	StatError = 4,
} ResultType;

void format_error(const char* fmt, ...);
void set_program_name(const char* name);
void abort_program(const char* s);
void* malloc_check(void* malloc_result);

#endif
