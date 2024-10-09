#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	
	if(argc != 2) {
		perror("Incorrect number of arguments");
	}
	int fd = open(argv[1], O_WRONLY);
	if(fd == -1) {
		perror("Opening file error");
		exit(EXIT_FAILURE);
	}

	write(fd, "first line\n", 11);
	int newfd = 4;	
	newfd =  dup2(fd, newfd);
	if(newfd == -1) {
		perror("Copying file descriptor fail");
		exit(EXIT_FAILURE);
	}

	write(newfd, "second line", 11);

}
