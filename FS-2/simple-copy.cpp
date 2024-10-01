#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {
	if (argc != 3) {
		return EXIT_FAILURE;
	}

	int fd_r = open(argv[1], O_RDONLY);
	if(fd_r == -1) {
		perror("Reading file error");
		exit(EXIT_FAILURE);
	}
	int fd_c = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if(fd_c == -1) {
		perror("Creating file error");
		exit(EXIT_FAILURE);
	}

	char buffer[50];
	ssize_t bytes_r;
	while (bytes_r > 0) {
		bytes_r = read(fd_r, buffer, sizeof(buffer) - 1);
		buffer[bytes_r] = '\0';

		write(fd_c, buffer, bytes_r);
	}

	close(fd_r);
	close(fd_c);


	return 0;
}
