#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>

int main() {
	
	std::string files[2];

	off_t offsets[2];

	unsigned bytes_copy;

	std::cout << "Enter the name of source file:";

	std::cin >> files[0];

	std::cout << "Enter the name of destination file:";

	std::cin >> files[1];

	std::cout << "Enter the initial offset in source file(in bytes):";

	std::cin >> offsets[0];

	std::cout << "Enter the initial offset in destination file(in bytes):";

	std::cin >> offsets[1];

	std::cout << "Enter the number of bytes to copy:";

	std::cin >> bytes_copy;

	int fd_1 = open(files[0].data(), O_RDONLY);
	if(fd_1 == -1) {
		perror("Opening source file path error");
		exit(EXIT_FAILURE);
	}

	int fd_2 = open(files[1].data(), O_WRONLY);
	if(fd_2 == -1) {
		perror("Opening destination file path error");
		exit(EXIT_FAILURE);
	}

	char* buffer = new char[bytes_copy];

	if(lseek(fd_1, offsets[0], SEEK_SET) == -1) {
		perror("Setting offset failed");
		exit(EXIT_FAILURE);
	}

	if(read(fd_1, buffer, sizeof(buffer)) == -1) {
		perror("Reading data error");
		exit(EXIT_FAILURE);
	}

	if(lseek(fd_2, offsets[1], SEEK_SET) == -1) {
		perror("Setting offset failed");
	}

	if(write(fd_2, buffer, sizeof(buffer)) == -1) {
		perror("Writing data error");
	}

	delete[] buffer;

	std::cout << "Successfully copied " << bytes_copy << " bytes from the source file to the destination file" ;

	return 0;
}
