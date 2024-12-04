#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ucontext.h>
#include <cerrno>

void signal_handler(int sig, siginfo_t* info, void* context) {
    if (sig == SIGUSR1) {
        std::cout << "Received a SIGUSR1 signal ";

        pid_t sender_pid = info->si_pid;
        std::cout << "from process " << sender_pid;

        uid_t sender_uid = info->si_uid;
        std::cout << " executed by " << sender_uid;

        struct passwd* pwd = getpwuid(sender_uid);
        if (pwd) {
            std::cout << " " << pwd->pw_name << "\n";
        } else {
                perror("getpwuid failed");
		exit(errno);
        }

        ucontext_t* uctx = static_cast<ucontext_t*>(context);

        std::cout << "State of the context: EIP = " << static_cast<int>(uctx->uc_mcontext.gregs[REG_RIP]) << ",  EAX = " << static_cast<int>(uctx->uc_mcontext.gregs[REG_RAX]) << ",  EBX "  << static_cast<int>(uctx->uc_mcontext.gregs[REG_RBX]) << ".\n";
    }
}

int main() {

    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("sigaction failed");
        exit(errno);
    }
    while (true) {
	std::cout << getpid() << "\n";
        sleep(10);
    }

    return 0;
}
