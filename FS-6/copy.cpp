#include <fcntl>
#include <unistd.h>

int main (int argc, char* argv[]) {

	if(argc != 3) {
		perror("Wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	fd_1 = open(argv[1], O_RDONLY);
	fd_2 = open(argv[2], O_WRONLY);

	char buffer[100];
	size_t bytes_r = 1;;

	while(bytes_r != -1) {
	
		
	}
}
