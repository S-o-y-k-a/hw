#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mutex>
#include <pthread.h>
#include <string>
#include <vector>
#include <sstream>
 
struct User {
 
	std::string login;
	std::string password;
 
	struct Gamescore {
		int sea_fight;
	};
};
 
std::unordered_map<std::string, User> users_online;
 
std::unordered_map<std::string, User> users;
 
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;
 
 
void* handle_client(void* arg) {

    User cur_user;
 
    int client_socket = *(static_cast<int*>(arg));
 
    if(send(client_socket, "Sign up or register (press r/s)\n", 32, 0) < 0) {
    	std:: cout << "Can't reach user :(";
	exit(errno);
    }
 
    char buffer[1001];
    int bytes_recieved;
 
    if(bytes_recieved = recv(client_socket, buffer, 1000, 0) <= 0) {
	    std::cout <<  "Failed to hear from client :(";
	    close(client_socket);
	    exit(errno);
    }
    else if(buffer[0] == 'r') {
	    while(true) {
	    	if(send(client_socket, "Enter your desired nickname: \n", 30, 0) <= 0) {
                	std::cout << "Failed to reach user :(";
                	continue;
            	}
		if((bytes_recieved = recv(client_socket, buffer, 1000, 0)) <= 0) {
            		std::cout <<  "Failed to hear from client :(";
            		continue;
		}
		buffer[bytes_recieved] = '\0';
		std::string name = buffer;
 
		//PTHREAD_MUTEX_LOCK
		for(int i = 0; i < users.size(); ++i) { //SOYKA ZAMENI NA USER CONTAINS!!!!!!!! 
			if(users[i].login == name) {
				if(send(client_socket, "Nickname is already in use :( \n", 31, 0) != 0) {
        	                	std::cout << "Failed to reach user :(";
                	       		continue;
				}

			}
		}
 
		if(send(client_socket, "Great! Now, create a password: \n", 32, 0) <= 0) {
			std::cout << "Failed to reach user :(";
                        continue;
                }
 
		if(bytes_recieved = recv(client_socket, buffer, 1000, 0) <= 0) {
                        std::cout <<  "Failed to hear from client :(";
                        continue;
                }
 
		buffer[bytes_recieved] = '\0';
 
		std::string password = buffer;
 
		cur_user.login = name;
		cur_user.password = password;
 
		pthread_mutex_lock(&mute);
		users.insert(cur_user.login, cur_user);
		pthread_mutex_unlock(&mute);
 
		if(send(client_socket, "Yay! Congratulations, your account has been created succesfully! Have fun playing :) \n", 86, 0) <= 0) {
                        std::cout << "Failed to reach user :(";
                        continue;
                }

		for(auto user : users_online) {
			std::cout << user.login << '\n';
		}

		break;
 
 
 
	    }
 
    }
    else if(buffer[0] == 's') {
	    while(true) {
	    	if(send(client_socket, "Enter your login: \n", 20, 0) <= 0) {
	    		std::cout << "Failed to reach user :(";
			close(client_socket);
			break;
		    }
		int br;
		char buffer[4096];
		if((br = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) <= 0) {
			std::cout << "Failed to reach user\n";
			close(client_socket);
			break;

		}
		buffer[br] = '\0';
		std::string s;
		s = buffer;
		bool exist = false;
		for(auto user : users) {
			if(user.login == s) {
				exist = true;
					
			} 
		}
		if()
	    }
    }
    /*
    else {
	    if(send() != 0) {
		    std::cout << "Failed to reach user :(";
		    exit(errno);
	    }
    } */
 
 
    while (true) {
        int rs = recv(client_socket, buffer, 1000, 0);
        if (rs <= 0) break;
        buffer[rs] = '\0';
 
	pthread_mutex_lock(&mute);
 
	std::cout << buffer << "\n";
 
        pthread_mutex_unlock(&mute);
    }
 
    close(client_socket);
 
    pthread_mutex_lock(&mute);
    std::cout << "Client disconnected: ";
    pthread_mutex_unlock(&mute);
 
    return 0;
}
 
int main() {


  int bd_fd;

  /*
  if(bd_fd = open("bd.txt", O_RDWR | O_CREAT, 0664) == -1) {

  	
  }
  */ 

  // create a socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
      perror("socket creation error");
      exit(errno);
  }
 
  // create an endpoint
 
  // socket address
  struct sockaddr_in server_address;
  // internet protocol = AF_INET
  server_address.sin_family = AF_INET;
  // accept or any address (bind the socket to all available interfaces)
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  // port
  server_address.sin_port = htons(8888);
 
  // Bind server_socket to server_address
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
      perror("bind failed");
      exit(errno);
  }
 
  // Listen for incoming connections
  if (listen(server_socket, 10) < 0) {
    perror("listen failed");
    exit(errno);
  }
  std::cout << "Waiting for connection\n";
 
  while(true) {
      int client_socket;
      struct sockaddr_in client_address;
      unsigned int client_addr_len = sizeof(client_address);
 
      // Accept incoming connection
      if ((client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0) {
          perror("accept failed");
          exit(errno);
      }
 
      std::cout << "Connection accepted from:" << inet_ntoa(client_address.sin_addr) << "\n";
      pthread_t thread;
 
      if (pthread_create(&thread, nullptr, handle_client, &client_socket) == 0) {
		pthread_detach(thread);
      }
      else {
      	perror("Thread creation failed.");
	close(client_socket);
      }
 
    }
 
    close(server_socket);
    pthread_mutex_destroy(&mute);
    return 0;
}
 
