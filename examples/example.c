#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

void exit_perror(const char* s) {
	perror(s);
	abort();
}

void test_opendir(const char* path) {
	DIR* dir = opendir(path);
	if (!dir) {
		exit_perror("opendir");
	}

	printf("Files in: `%s`\n", path);
	struct dirent* ent;
	while ((ent = readdir(dir))) {
		printf("`%s`\n", ent->d_name);
	}

	if (closedir(dir) == -1) {
		exit_perror("closedir");
	}
}

int main() {
	test_opendir(".");
	return 0;
}
