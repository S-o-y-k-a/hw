#include <iostream>
#include <csignal>
#include <unistd.h>

int sigusr2_count = 0;

void handle_SIGUSR1(int signum) {
}

void handle_SIGINT(int signum) {
    std::cout << "number of SIGUSR2 signals received: " << sigusr2_count << "\n";
    exit(0); 
}

void handle_SIGUSR2(int signum) {
    sigusr2_count++;
}

int main() {
	
    struct sigaction sa_usr1;
    struct sigaction sa_usr2;
    struct sigaction sa_int;

    sa_usr1.sa_handler = handle_SIGUSR1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, nullptr);

    sa_usr2.sa_handler = handle_SIGUSR2;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;
    sigaction(SIGUSR2, &sa_usr2, nullptr);

    sa_int.sa_handler = handle_SIGINT;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, nullptr);

    while (true) {
        std::cout << "I am still alive!!" << "\n";
        sleep(5);
    }

    return 0;
}
