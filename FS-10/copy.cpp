#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main (int argc, char* argv[]) {

	if(argc != 3) {
		perror("Invalid number of arguments");
		exit(EXIT_FAILURE);
	}

	int fd_11 = open(argv[1], O_RDONLY);
	if(fd_11 == -1) {
		perror("Opening file path error");
		exit(EXIT_FAILURE);
	}

	int fd_12 = open(argv[1], O_RDONLY);
	if(fd_12 == -1) {
	
		perror("Opening file path error");
		exit(EXIT_FAILURE);
	}

	int fd_2 = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0664);
	if(fd_2 == -1) {
		perror("Opening file path failure");
	}

	off_t bytes_data;
	char buffer[1024];
	off_t hole_size;
	off_t all = 0, hole = 0;
	off_t temp_all;

	while(true) {
		
		bytes_data = all;
		all = lseek(fd_11, all, SEEK_HOLE);

		bytes_data = all - bytes_data;

		while(bytes_data > 0) { 

			if (read(fd_12, buffer, (bytes_data < sizeof(buffer) ?  bytes_data : sizeof(buffer)))  == -1) {
				perror("Unable to read data");
				exit(EXIT_FAILURE);
			}

			if ( write(fd_2, buffer, (bytes_data < sizeof(buffer) ? bytes_data : sizeof(buffer))) == -1) {
				perror("Unable to write data");
				exit(EXIT_FAILURE);
			}
			

			bytes_data -= sizeof(buffer);

		}
		
		temp_all = all;
		hole_size = all;
		all = lseek(fd_11, all, SEEK_DATA);
		hole_size = all - hole_size;

		if(hole_size < 1) {
			break;
		}

		lseek(fd_11, temp_all, SEEK_DATA);

		hole+= hole_size;

		lseek(fd_2, hole_size, SEEK_CUR);


	}
		
	std::cout << "Successfully copied" << temp_all << "bytes(data: "  << temp_all - hole << ", hole: " << hole << ").\n"; 

	close(fd_11);
	close(fd_12);
	close(fd_2);
	return 0;

}
