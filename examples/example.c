#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include "libft.h"

MONOVEC_DECLARATION(struct dirent*, DirVec, dirvec);
MONOVEC_DEFINITIONS(struct dirent*, DirVec, dirvec);

void exit_perror(const char* s) {
	perror(s);
	abort();
}

void test_opendir(const char* path) {
	DIR* dir = opendir(path);
	if (!dir) {
		exit_perror("opendir");
	}

	DirVec* v = dirvec_construct(2);
	assert(v);

	printf("Files in: `%s`\n", path);
	struct dirent* ent;
	while ((ent = readdir(dir))) {
		assert(dirvec_push_back(v, ent) == 0);
	}

	if (closedir(dir) == -1) {
		exit_perror("closedir");
	}

	for (int i = 0; i < v->length; i++) {
		printf("%s\n", v->table[i]->d_name);
	}
	dirvec_destroy(v);
}

int main() {
	test_opendir(".");
	return 0;
}
