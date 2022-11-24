#ifndef UTILS_H
#define UTILS_H

#include "libft.h"

MONOVEC_DECLARATION(char*, VecStr, vecstr);

typedef enum ResultType {
	Success = 0,
	SystemError = 1,
	ArgumentError = 2,
	StatError = 3,
} ResultType;

#endif
