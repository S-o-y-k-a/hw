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
#include <ctime>
#include <unordered_map>


//structs
/*
struct User {
 
	std::string login;
	std::string password;

	int c_socket;
 
	struct Gamescore {
		int sea_fight;
	};
};

struct SeaFight {

	SeaFight(std::string u1, std::string u2) {}
	
	std::string user1;
	std::string user2;

	std::vector<std::vector<int>> map1(10, std::vector<int>(10, 0));
	std::vector<std::vector<int>> map2(10, std::vector<int>(10, 0));
}; 

//global things
 
std::unordered_map<std::string, User> users_online;
 
std::unordered_map<std::string, User> users; */
 
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;

#define board_size 4
#define players_count 2
#define ship_count 3
#define max_score 4

int server_socket;
 

//game handle funclion

void draw_boards(int client_socket, std::vector<std::vector<int>>& r_board, std::vector<std::vector<int>>& s_board) {

	std::string r_message;

	for(int i = board_size - 1; i >= 0; --i) {
		for(int j = 0; j < board_size; ++j) {
			if(r_board[i][j] == 0) {
				r_message += "\033[1;30;47mo";
			}
			else if(r_board[i][j] == 1) {
				r_message += "\033[1;32;47mo";
			} 
			else if(r_board[i][j] == 2) {
				r_message += "\033[1;31;47mo";
			}
			else if(r_board[i][j] == 3) {
				r_message += "\033[1;34;47mo";
			}
			
			r_message += "\033[0m";
		}
		r_message += "\n";
	}
	r_message += "\n";

	for(int i = board_size - 1; i >= 0; --i) {
		for(int j = 0; j < board_size; ++j) {
			if(s_board[i][j] == 0 || s_board[i][j] == 1) {
				r_message += "\033[1;30;47mo";
			}
			else if(s_board[i][j] == 2) {
				r_message += "\033[1;31;47mo";
			} 
			else if(s_board[i][j] == 3) {
				r_message += "\033[1;34;47mo";
			}
			
			r_message += "\033[0m";
		}
		r_message += "\n";
	}

	if(send(client_socket, r_message.c_str(), r_message.size(), 0) < 0) {
		pthread_mutex_lock(&mute);
    	std:: cout << "Can't reach user :(";
		pthread_mutex_unlock(&mute);
		close(server_socket);
		exit(errno);
	}

}

void* sea_fight (void* arg) {

	int* client_sockets = static_cast<int*>(arg);

	for(int i = 0; i < players_count; ++i) {
		std::cout << client_sockets[i] << std::endl;
	}
	
	std::vector<std::vector<std::vector<int>>> real_board(players_count, std::vector<std::vector<int>>(board_size, std::vector<int>(board_size, 0)));

	//player boards filling

	char buffer[1001];
    int bytes_recieved;

	int x1;
	int y1;
	int x2;
	int y2;

	int begin;
	int end;

	for(int i = 0; i < players_count; ++i) {

	BOARD_FILL:

		if(send(client_sockets[i], "Send the coordinates like this: x1y1x2y2\n", 40, 0) < 0) {
			pthread_mutex_lock(&mute);
    		std:: cout << "Can't reach user :(";
			pthread_mutex_unlock(&mute);
			close(server_socket);
			exit(errno);
    	}

		std::cout << "problem here1";
 
    	bytes_recieved = recv(client_sockets[i], buffer, 1000, 0);
    	if(bytes_recieved <= 0) {
			pthread_mutex_lock(&mute);
	    	std::cout <<  "Failed to hear from client :(";
			pthread_mutex_unlock(&mute);
			close(server_socket);
	    	exit(errno);
    	}

		if(bytes_recieved != ship_count * 4) {

			if(send(client_sockets[i], "Incorrect number of coordinates. Try again!\n", 44, 0) < 0) {
				pthread_mutex_lock(&mute);
    			std:: cout << "Can't reach user :(";
				pthread_mutex_unlock(&mute);
				close(server_socket);
				exit(errno);
    		}

			goto BOARD_FILL;

		}

		std::cout << "problem here2";

		for(int j = 0; j < ship_count; ++j) {

			x1 = (buffer[j * 4] - '0');
			y1 = (buffer[j * 4 + 1] - '0');
			x2 = (buffer[j * 4 + 2] - '0');
			y2 = (buffer[j * 4 + 3] - '0');

		if(x1 == x2) {
			begin = std::min(y1, y2);
			end = std::max(y1, y2);

			for(int l = begin; l <= end; ++l) {
				if(real_board[i][l][x1] == 1)  {
					if(send(client_sockets[i], "Incorrect coordinates. Try again!\n", 35, 0) < 0) {
						pthread_mutex_lock(&mute);
    					std:: cout << "Can't reach user :(";
						pthread_mutex_unlock(&mute);
						close(server_socket);
						exit(errno);
    				}

					goto BOARD_FILL;
				}
				real_board[i][l][x1] = 1;
			}
		}
		else if(y1 == y2) {
			begin = std::min(x1, x2);
			end = std::max(x1, x2);

			for(int l = begin; l <= end; ++l) {
				if(real_board[i][y1][l] == 1)  {
					if(send(client_sockets[i], "Incorrect coordinates. Try again!\n", 35, 0) < 0) {
						pthread_mutex_lock(&mute);
    					std:: cout << "Can't reach user :(";
						pthread_mutex_unlock(&mute);
						close(server_socket);
						exit(errno);
    				}

					goto BOARD_FILL;
				}
				real_board[i][y1][l] = 1;
			}
		}
		else {
			if(send(client_sockets[i], "Incorrect coordinates. Try again!\n", 35, 0) < 0) {
				pthread_mutex_lock(&mute);
    			std:: cout << "Can't reach user :(";
				pthread_mutex_unlock(&mute);
				close(server_socket);
				exit(errno);
    		}

			goto BOARD_FILL;
		}

		std::cout << "problem here3";

		
	}

	}

	//game process

	std::cout << "Game starts! \n";

	//try
	for(int i = 0; i < players_count - 1; ++i) {
		draw_boards(client_sockets[i], real_board[i], real_board[i + 1]);
	}
	draw_boards(client_sockets[players_count - 1], real_board[players_count - 1], real_board[0]);
	//try

	std::vector<int> scores(players_count, 0);

	std::srand(std::time({}));
	int cur_player = std::rand() % players_count;

	bool r_break = false;

	while(true) {
		if(send(client_sockets[cur_player], "Send the coordinates like this: xy\n", 39, 0) < 0) {
		pthread_mutex_lock(&mute);
    	std:: cout << "Can't reach user :(";
		pthread_mutex_unlock(&mute);
		exit(errno);
   	}
	
    bytes_recieved = recv(client_sockets[cur_player], buffer, 1000, 0);
    if(bytes_recieved <= 0) {
		pthread_mutex_lock(&mute);
	    std::cout <<  "Failed to hear from client :(";
		pthread_mutex_unlock(&mute);
	    exit(errno);
    }

	int x;
	int y;

	x = (buffer[1] - '0');
	y = (buffer[0] - '0');

	int opponent;
	int o_opponent;

	if(cur_player < players_count - 1) {
		opponent = cur_player + 1;
	}
	else {
		opponent = 0;
	}
	if(opponent < players_count - 1) {
		o_opponent = opponent + 1;
	}
	else {
		o_opponent = 0;
	}

	if(real_board[opponent][x][y] == 1) {

				++ scores[cur_player];
				real_board[opponent][x][y] = 2;

				if(send(client_sockets[opponent], "Your ship has been damaged!\n", 28, 0) < 0) {
					pthread_mutex_lock(&mute);
    				std:: cout << "Can't reach user :(";
					pthread_mutex_unlock(&mute);
					exit(errno);
    			}
				draw_boards(client_sockets[opponent], real_board[opponent], real_board[o_opponent]);

				if(send(client_sockets[cur_player], "Bullseye!\n", 10, 0) < 0) {
					pthread_mutex_lock(&mute);
    				std:: cout << "Can't reach user :(";
					pthread_mutex_unlock(&mute);
					exit(errno);
    			}
				draw_boards(client_sockets[cur_player], real_board[cur_player], real_board[opponent]);
				

			}
			else {

				real_board[opponent][x][y] = 3;
				

				if(send(client_sockets[opponent], "Phew, nothing!\n", 15, 0) < 0) {
					pthread_mutex_lock(&mute);
    				std:: cout << "Can't reach user :(";
					pthread_mutex_unlock(&mute);
					exit(errno);
    			}
				draw_boards(client_sockets[opponent], real_board[opponent], real_board[o_opponent]);

				if(send(client_sockets[cur_player], "Meh, nothing!\n", 14, 0) < 0) {
					pthread_mutex_lock(&mute);
    				std:: cout << "Can't reach user :(";
					pthread_mutex_unlock(&mute);
					exit(errno);
    			}
				draw_boards(client_sockets[cur_player], real_board[cur_player], real_board[opponent]);

				cur_player = opponent;
				
			}

	for(int i = 0; i < players_count; ++i) {
		std::cout << i << " " << scores[i] << "\n";
		if(scores[i] == max_score) {
			r_break = true;
			break;
		}
	}
	
	if(r_break) {
		break;
	}

	}

	for(int i = 0; i < players_count; ++i) {
		if(scores[i] == max_score) {
			if(send(client_sockets[i], "You win :)\n", 11, 0) < 0) {
				pthread_mutex_lock(&mute);
    			std:: cout << "Can't reach user :(";
				pthread_mutex_unlock(&mute);
				exit(errno);
    		}
		}
		else {
			if(send(client_sockets[i], "You lose :(\n", 12, 0) < 0) {
				pthread_mutex_lock(&mute);
    			std:: cout << "Can't reach user :(";
				pthread_mutex_unlock(&mute);
				exit(errno);
    		}
		}
		close(client_sockets[i]);
	}

	return 0;

}
 
/* void* handle_client(void* arg) {

    User cur_user;
 
    int client_socket = *(static_cast<int*>(arg));

	

    //register/sign up
 
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
 
		pthread_mutex_lock(&mute);
		if(users.contains(name)) {
			if(send(client_socket, "Nickname is already in use :( \n", 31, 0) != 0) {
        	                std::cout << "Failed to reach user :(";
                	       		continue;
			}

		}
		pthread_mutex_unlock(&mute);
 
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
    } */
    /*
    else {
	    if(send() != 0) {
		    std::cout << "Failed to reach user :(";
		    exit(errno);
	    }
    } 
 
 
    while (true) {
		}
		if()
	    }
    }
    
    else {
	    if(send() != 0) {
		    std::cout << "Failed to reach user :(";
		    exit(errno);
	    }
    } */
 

//playing
/*
    while (true) {

	    User user2;

	    if(users_online.size() > 1) {
		    for(auto user : users_online) {
		    	if(user.login != cur_user.login) {
			
				Seafight game;

				send(client_socket, "new game opened\n", 17, 0);

				user2 = user;

				send(user2.c_socket, "new game opened"\n, 17, 0);

				
			
			}
		    }
	    }
    } 
 
 
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
} */

//main
 
int main() {


  int bd_fd;

  /*
  if(bd_fd = open("bd.txt", O_RDWR | O_CREAT, 0664) == -1) {

  	
  }
  */ 

  // create a socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
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
      //int client_socket;
      struct sockaddr_in client_address;
      unsigned int client_addr_len = sizeof(client_address);

      // Accept incoming connection

	  int sockets[players_count];

	  for(int i = 0; i < players_count; ++i) {
      	if ((sockets[i] = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0) {
          	perror("accept failed");
          	exit(errno);
      	}
		std::cout << i + 1 << " player connected.\n";
		if(send(sockets[i], "You're connected! Wait a little... \n", 36, 0) <= 0) {
	    	std::cout << "Failed to reach user :(";
			close(sockets[i]);
			break;

	    }
		}

		
	  
        pthread_t thread;

         
      if (pthread_create(&thread, nullptr, sea_fight, sockets) == 0) {

		pthread_detach(thread);
      }
      else {
      	perror("Thread creation failed.");
		for(int i = 0; i < players_count; ++i) {
			close(sockets[i]);
		}
      }
	
    }

    close(server_socket);
    pthread_mutex_destroy(&mute);
    return 0;
}
