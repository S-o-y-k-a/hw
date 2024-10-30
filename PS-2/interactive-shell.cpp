#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <string>

std::string tmp;

char** v_to_ch(int start, int end, std::vector<std::string>& vector) {
	char** arr = new char*[end - start + 1];
	for(int i = 0; i < end - start; ++i) {
		arr[i] = vector[start + i].data();
	}
	arr[end - start] = nullptr;

	return arr;
}

int execute (char** args, bool silence, short append_status, std::string append) {

	pid_t pid = fork();

	if(pid == -1) {
		perror("Fork failed");
		exit(EXIT_FAILURE);
			 
	}
	else if(pid == 0) {

		if(append_status == 1) {
			if (close(1) == -1) {
				perror("Closing output failure");
				exit(EXIT_FAILURE);
			}	

		int fd = open(append.data(), O_WRONLY | O_CREAT | O_TRUNC, 0664);

		if(fd == -1) {
			perror("Opening file error");
			exit(EXIT_FAILURE);
		}
	        }

		if(append_status == 2) {
			if (close(1) == -1) {
				perror("Closing output failure");
				exit(EXIT_FAILURE);
			}	

		int fd = open(append.data(), O_WRONLY | O_CREAT | O_APPEND, 0664);

		if(fd == -1) {
			perror("Opening file error");
			exit(EXIT_FAILURE);
		}
	        }

		if(silence) {
		
			if(close(1) == -1) {
				perror("Closing output error");
				exit(EXIT_FAILURE);
			}
			if(close(2) == -1) {
				perror("Closing error stream error");
				exit(EXIT_FAILURE);
			}

			std::string log = tmp + ".log";
			int fd = open(log.data(), O_WRONLY | O_CREAT | O_TRUNC, 0664);

			if(fd == -1) {
				perror("Opening file error");
				exit(EXIT_FAILURE);
			}

			if(dup2(fd, 2) == -1) {
				perror("Dup2 error");
				exit(EXIT_FAILURE);
			}

		}
		
		if (execvp(args[0], args) == -1) {
			perror("Execution failed");
			exit(EXIT_FAILURE);
		}
	}
	else {
		tmp = std::to_string(pid);
		int status;
		waitpid(pid, &status, 0);

		if(WIFEXITED(status)) {
			return status;	
	}
	}
	return -1;

}

int main () {

	std::string args;

	bool silence = false;

	int status = 0;

	int starting_point = 0;

	bool exit = false;

	bool exit_command = false;

	short append_status = 0;

	while(!exit) {

	std::vector<std::string> args_v;

	std::cout << "shell: ";	
	std::getline(std::cin, args);

	int str_start = 0;
	for(int i = 0; i <= args.length(); ++i) {
	
		if(args[i] == ' ' || i ==  args.length()) {
		
			args_v.push_back(args.substr(str_start, i - str_start));		
			str_start = i + 1;
		}
	}

	exit_command = false;
	append_status = 0;
	starting_point = 0;
	status = 0;
	silence = false;

	
	for(int i = 0; (i < args_v.size()) && !exit && !exit_command; ++i) {
	
		if(args_v[i] == "exit") {
			exit = true;
		}
		else if (args_v[i] == "&&") {
			status = execute(v_to_ch(starting_point, (append_status != 0 ? i - 2 : i), args_v), silence, append_status, (append_status != 0 ? args_v[i - 1] : "\0"));
				if(status != 0) {
					exit_command = true;	
				}
				starting_point = i + 1;
				append_status = 0;
				silence = false;
		}
		else if (args_v[i] == "||") {
			status = execute(v_to_ch(starting_point, (append_status != 0 ? i - 2 : i), args_v), silence, append_status, (append_status != 0 ? args_v[i - 1] : "\0"));
				if(status == 0) {
					exit_command = true;
				}
				starting_point = i + 1;
				append_status = 0;
				silence = false;
		}
		else if (args_v[i] == ";") {
			execute(v_to_ch(starting_point, (append_status != 0 ? i - 2 : i), args_v), silence, append_status, (append_status != 0 ? args_v[i - 1] : "\0"));
				starting_point = i + 1;
				append_status = 0;
				silence = false;
		}
		else if (i == args_v.size() - 1) {
			char** arr = v_to_ch(starting_point, append_status != 0 ? i - 1 : i + 1, args_v);
			execute(arr, silence, append_status, (append_status != 0 ? args_v[i] : "\0"));
				exit_command = true;
		}
		else if (args_v[i] == ">>") {
				append_status = 2;
		}
		else if (args_v[i] ==  ">") {
				append_status = 1;
		}
		else if(args_v[i] == "silent") {
				silence = true;
				++ starting_point;
		}

		}
	
	}

	return 0;
}
