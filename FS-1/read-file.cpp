#include <unistd.h>
#include <iostream>
#include <fcntl.h>

int main (int argc, char *argv[]) {
	char buffer[50];
	if(argc != 2) {
		return EXIT_FAILURE;
	}
	int fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		perror("Opening file error");
		exit(EXIT_FAILURE);
	}
	ssize_t r_bytes;
	while (r_bytes != -1) {
	       	r_bytes = read(fd, buffer, sizeof(buffer) - 1);	
	buffer[r_bytes] = '\0';
	std::cout << buffer;
	}

	close(fd);

	return 0;
}
