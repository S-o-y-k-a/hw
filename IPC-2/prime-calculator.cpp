#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>


bool is_prime(int num) {
    if (num <= 1){
	    return false;
	}
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int find_prime(int m) {
    int count = 0;
    int num = 2;
    while (true) {
        if (is_prime(num)) {
            ++count;
            if (count == m) {
		    return num;
	    }
        }
        ++num;
    }
}

int main() {
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        while (true) {
            int m;

            if (read(pipe1[0], &m, 4) <= 0) {
                break;
            }

	    std::cout << "[child] Calculating " << m << "-th prime number...\n";

            int result = find_prime(m);

	    std::cout << "[child] Sending calculation result of prime(" << m << ")...\n";
            write(pipe2[1], &result, sizeof(result));
        }

        close(pipe1[0]);
        close(pipe2[1]);
    } else {
        close(pipe1[0]);
        close(pipe2[1]);

        std::string input;

        while (true) {
	    std::string str;
            std::cout << "[parent] Please enter the number: ";
            std::cin >> str;

            if (str == "exit") {
                break;
            }

	    int num = stoi(str);

	    std::cout << "[parent] Sending " << num << " to the child process... \n"; 
            write(pipe1[1], &num, sizeof(num));

	    std::cout << "[parent] Waiting for the responce from the child process...\n";

            int result;

            if (read(pipe2[0], &result, sizeof(result)) > 0) {
                std::cout << "[parent] Recieved calculation result of prime(" << num << ") = " << result << "...\n";
            }
        }

        close(pipe1[1]);
        close(pipe2[0]);

        wait(nullptr);
    }

    return 0;
}

