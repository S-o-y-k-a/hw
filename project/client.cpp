#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

void* get_message(void* arg) {
	int client_socket = *(static_cast<int*>(arg));
	char buffer[1001];

while (true) {
        int rs = recv(client_socket, buffer, 1000, 0);
        if (rs <= 0) {
		      std::cout << "disconnected from server\n";
		      exit(0);
	      }
        buffer[rs] = '\0';

        std::cout << buffer << "\n";
    }
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

  /*std::string name;

  std::cout << "Enter your login\n";

  std::cin >> name; 

  send(client_socket, name.c_str(), name.size(), 0);
*/

  pthread_t recieve_thread;
  pthread_create(&recieve_thread, 0, get_message, &client_socket);
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






/*#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
 
 
int main(){
 
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
 
  // send message to server
  
  char buffer[4096];
  while(true) {
   int br;
   if((br = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) <= 0) {
    std::cout << "Can't reach server\n";
    exit(errno);
   }
   buffer[br] = '\0';
   std::cout << buffer << std::endl;

  std::string mess;
  std::cin >> mess;
  int sent = send(client_socket, mess.c_str(), mess.size(), 0);
  if(sent == -1){
    exit(errno);
  }
  }
 
  close(client_socket);
  return 0;

    }
*/