#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main (int argc, char *argv[]) {
	if(argc != 2) {
	return EXIT_FAILURE;
	}

	int fd = open(argv[1], O_WRONLY);

	if(fd == -1) {
		perror("Reading file error");
		exit(EXIT_FAILURE);
	}

	struct stat st;

	fstat(fd, &st);

	int fsize = st.st_size;

	char buffer[100] = {'\0'};

	for(int i = 0; fsize - i > 0; i += 100 ) {
		if(write(fd, buffer, sizeof(buffer)) == -1) {
			perror("Write error");
			exit(EXIT_FAILURE);
		}

	}

	int u = unlink(argv[1]);
	
	return 0;
}
