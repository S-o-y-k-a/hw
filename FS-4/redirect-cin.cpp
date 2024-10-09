#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char** argv) {
	if(argc != 2) {
		perror("Incorrect arguments");
		exit(EXIT_FAILURE);
	}
	close(0);

	int fd = open(argv[1], O_RDONLY);

	if(fd == -1) {
		perror("File reading error");
		exit(EXIT_FAILURE);
	}

	std::string s;

	std::cin >>  s;

	std::string tmp = s;

	for(int i = 0; i < s.length(); i ++) {
		tmp[i] = s[s.length() - i];
	}

	s = tmp;

	std::cout << s;
}
