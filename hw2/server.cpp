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

pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;


void* handle_client(void* arg) {
    
    int* client_socket_ptr = static_cast<int*>(arg);
    int client_socket = *client_socket_ptr;
    
    char buffer[1001];
    while (true) {
        int rs = recv(client_socket, buffer, 1000, 0);
        if (rs <= 0) break;
        buffer[rs] = '\0';
        
        pthread_mutex_lock(&mute);
        std::cout << "Got message: " << buffer << "\n";
        pthread_mutex_unlock(&mute);
    }
    
    close(client_socket); 
    
    pthread_mutex_lock(&mute);
    std::cout << "Client disconnected: ";
    pthread_mutex_unlock(&mute);

    delete client_socket_ptr;
    
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

