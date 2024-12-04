#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <cerrno>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(errno);
    }

    if (pid == 0) {
        pid_t parent_pid = getppid();

        std::cout << "I'm a child process attempting murder\n";


        if (kill(parent_pid, SIGKILL) == -1) {
            perror("Failed to kill parent >:(");
            exit(errno);
        }
        std::cout << "Just tried to kill my parent, and still alive.\n";

	for(int i = 0; i < 10; ++i) {
		sleep(1);
		std::cout << "Child here, living my life normally after murdering my parent\n";
	}

    } else {
        std::cout << "I'm a parent process, just living my life normally, unaware of what's going to happen...\n";

	sleep(3);

	std::cout << "I'm a parent, and after some time of existing normally, i'm... well.. still existing";

        wait(nullptr);

        std::cout << "Yo, parent here, still alive after attepmted murder(child exited btw).\n";
    }

    return 0;
}
