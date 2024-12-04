#include <iostream>
#include <csignal>
#include <unistd.h>
#include <cstdlib>

int main(int argc, char* argv[]) {

    if(argc != 3) {
    	perror("wrong args number");
	exit(1);
    }


    int signal_num = std::atoi(argv[1]);
    int pid = std::atoi(argv[2]);

    if (kill(pid, signal_num) == -1) {
        perror("signal sending error");
        exit(errno);
    }
    return 0;
}
