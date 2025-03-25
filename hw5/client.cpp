#include <iostream>
#include <unistd.h>
#include <cstring>
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
  
while(true) {

  std::string mess;
  std::getline(std::cin, mess);
  int sent = send(client_socket, mess.c_str(), mess.size(), 0);
  if(sent == -1){
    exit(errno);
  }


  char buf[30];

  int bytes_recieved;
  bytes_recieved = recv(client_socket, buf, 29, 0);
  buf[bytes_recieved] = '\0';

  std::string b = buf;

  std::cout << b;

}
  close(client_socket);
  return 0;
}
