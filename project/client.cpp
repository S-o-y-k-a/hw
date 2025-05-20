#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

void* get_message(void* arg) {
	int* client_socket = static_cast<int*>(arg);
	char buffer[100001];

while (true) {
        int rs = recv(client_socket[0], buffer, 100000, 0);
        if (rs <= 0) {
		      std::cout << "disconnected from server\n";
		      exit(0);
	      }
        buffer[rs] = '\0';

        std::cout << buffer << "\n";
    }

    delete[] client_socket;
}


int main() {

  // server address 
  struct sockaddr_in server_address;

  // create a socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
        perror("socket creation error");
        exit(errno);
  }

  // fill the server ip and port address 
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8888);

  // connect
  int connected = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  if(connected == -1){
    perror("connection failed");
    exit(errno);
  }

  int* socket_ptr = new int(client_socket);
  pthread_t recieve_thread;
  pthread_create(&recieve_thread, 0, get_message, socket_ptr);
  pthread_detach(recieve_thread);

  // send message to server
  std::string mess;
  while(true) {
  std::getline(std::cin, mess);



  if(mess == "/exit") {
	  send(client_socket, mess.c_str(), mess.size(), 0);
	  close(client_socket);
	  exit(0);
  }
  int sent = send(client_socket, mess.c_str(), mess.size(), 0);
  if(sent == -1){
    exit(errno);
  }
  }

  close(client_socket);
  return 0;
}
