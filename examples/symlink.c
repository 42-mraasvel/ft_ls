#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	argc--; argv++;
	if (argc == 0) {
		fprintf(stderr, "expected filename\n");
		return 1;
	}
	struct stat info;
	if (lstat(argv[0], &info) < 0) {
		perror("lstat");
	}
	return 0;
}
