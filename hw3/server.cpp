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
#include <vector>
#include <string>

struct Client {

	int socket;
	std::string name;

};

std::vector<Client> clients;

pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;

void msg_everyone (int client_socket, std::string msg) {

	pthread_mutex_lock(&mute);

	for(int i = 0; i < clients.size(); ++i) {
	
		if(client_socket != clients[i].socket) {
			send(clients[i].socket, msg.c_str(), msg.size(), 0);
		}
	}

	pthread_mutex_unlock(&mute);

}

void list (int client_socket) {

	pthread_mutex_lock(&mute);

	send(client_socket, "List of clients: \n", 18, 0);

	for(int i = 0; i < clients.size(); ++i) {
		send(client_socket, clients[i].name.c_str(), clients[i].name.size(), 0);
		send(client_socket, "\n", 1, 0);
	}

	pthread_mutex_unlock(&mute);
}


void* handle_client(void* arg) {
    
    int client_socket = *(static_cast<int*>(arg));

    char name[20];
    
    char buffer[1001];


	recv(client_socket, name, 19, 0);

	Client client;
	client.socket = client_socket;
	client.name = name;
	clients.push_back(client);


    while (true) {
        int rs = recv(client_socket, buffer, 1000, 0);
        if (rs <= 0) break;
        buffer[rs] = '\0';
	std::string buf = buffer;

	if(buf == "/exit") {
		pthread_mutex_lock(&mute);
		for(auto it = clients.begin(); it != clients.end(); ++it) {
			if(client_socket == it->socket) {
				close(it->socket);
				clients.erase(it);
				break;
			}
		}
		pthread_mutex_unlock(&mute);
		pthread_exit(0);
	}
	else if(buf == "/list") {
		list(client_socket);
	}
	else {
		msg_everyone(client_socket, buffer);
	}
    }
    
    close(client_socket); 
    
    pthread_mutex_lock(&mute);
    std::cout << "Client disconnected: ";
    pthread_mutex_unlock(&mute);
    
    return 0;
}

int main() {
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

