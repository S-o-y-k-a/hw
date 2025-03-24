#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <fcntl.h>


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
  server_address.sin_addr.s_addr = inet_addr("146.190.62.39");
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(80);

  // connect
  int connected = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  if(connected == -1){
    perror("connection failed");
    exit(errno);
  }

  // send message to server
  std::string mess = "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n";
  int sent = send(client_socket, mess.c_str(), mess.size(), 0);
  if(sent == -1){
    exit(errno);
  }

  int fd = open("f.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
  if(fd == -1) {
  	close(client_socket);
	std::cout << "file creation error";
	exit(errno);
  }

  char buffer[2049];
  int bytes_recieved;
  while(bytes_recieved = recv(client_socket, buffer, 2048, 0) > 0) {
	  buffer[bytes_recieved] = 0;
	  write(fd, buffer, bytes_recieved);
  }


  close(client_socket);
  close(fd);
  return 0;
}
