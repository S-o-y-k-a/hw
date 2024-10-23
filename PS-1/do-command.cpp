#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void do_function (int ex_status, double time) {
	std::cout << "Command ended with " <<  ex_status  << " exit code, and took " << time << " seconds.";	
}

void do_command (char** argv) {
	pid_t pid = fork();

	if(pid == -1) {
		perror("Fork failed");
		exit(EXIT_FAILURE);
			 
	}
	else if(pid == 0) {
		execvp(argv[0], argv);
		
		perror("Execution failed");
		exit(EXIT_FAILURE);
	}
	else {
		clock_t time_start = clock();
		int status;
		waitpid(pid, &status, 0);
		clock_t time_end = clock();

		if(WIFEXITED(status)) {
			do_function(status, (double)(time_end - time_start) / CLOCKS_PER_SEC);	
		}												

}
}

int main (int argc, char* argv[]) {

	if(argc < 2) {
		perror("Incorrect number of argiments");
		exit(EXIT_FAILURE);
	} 

	argv[argc] = nullptr;

	do_command(&argv[1]);

	return 0;
}

