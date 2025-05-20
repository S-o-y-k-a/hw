#include <iostream>
#include <stdlib.h>
#include <string>
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
#include <sstream>


//global things
 
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;

#define players_count 2

int server_socket;

//helping functions

bool recv_msg(int*& clients, int client, char*& buf, int& br) {
	br = recv(client, buf, 100000, 0);
    if(br <= 0) {
		std::cout << client<< std::endl;
		pthread_mutex_lock(&mute);
	    std::cout <<  "Failed to hear from client :(. Shutting down the game session.";
		pthread_mutex_unlock(&mute);
	    for(int i = 0; i < players_count; ++i) {
			close(clients[i]);
		}
		delete[] clients;
		return false;
    }
	buf[br] = '\0';
	
	return true;
}

bool send_msg(int*& clients, int client, std::string msg) {
	if(send(client, msg.c_str(), msg.size(), 0) < 0) {
		pthread_mutex_lock(&mute);
    	std:: cout << "Can't reach user :(. Shutting down the game session.";
		pthread_mutex_unlock(&mute);
		for(int i = 0; i < players_count; ++i) {
			close(clients[i]);
		}
		delete[] clients;
		return false;
	}
	return true;
}

void draw_boards(int client_socket, std::vector<std::vector<int>>& r_board, std::vector<std::vector<int>>& s_board, int board_size) {

	std::string r_message;

	for (int i = 0; i < board_size + 1; ++i) {
		r_message += std::string("\033[1;30;47m ") + (i < 10 ? " " : "") + std::to_string(i) + "\033[0m";
	}
	r_message += "\033[1;30;47m \033[0m\n";

	for (int i = 0; i < board_size; ++i) {
		r_message += std::string("\033[1;30;47m ") + (i < 9 ? " " : "") + std::to_string(i + 1) + " \033[0m";
		for (int j = 0; j < board_size; ++j) {
			if (r_board[i][j] == 0)
				r_message += "\033[1;30;47m o \033[0m";
			else if (r_board[i][j] == 1)
				r_message += "\033[1;32;47m o \033[0m";
			else if (r_board[i][j] == 2)
				r_message += "\033[1;31;47m o \033[0m";
			else if (r_board[i][j] == 3)
				r_message += "\033[1;34;47m o \033[0m";
		}
		r_message += "\n";
	}
	r_message += "\n";

	for (int i = 0; i < board_size + 1; ++i) {
		r_message += std::string("\033[1;30;47m ") + (i < 10 ? " " : "") + std::to_string(i) + "\033[0m";
	}
	r_message += "\033[1;30;47m \033[0m\n";

	for (int i = 0; i < board_size; ++i) {
		r_message += std::string("\033[1;30;47m ") + (i < 9 ? " " : "") + std::to_string(i + 1) + " \033[0m";
		for (int j = 0; j < board_size; ++j) {
			if (s_board[i][j] == 0)
				r_message += "\033[1;30;47m o \033[0m";
			else if (s_board[i][j] == 2)
				r_message += "\033[1;31;47m o \033[0m";
			else if (s_board[i][j] == 3)
				r_message += "\033[1;34;47m o \033[0m";
			else
				r_message += "\033[1;30;47m o \033[0m";
		}
		r_message += "\n";
	}
	r_message += "\n";

	if (send(client_socket, r_message.c_str(), r_message.size(), 0) < 0) {
		pthread_mutex_lock(&mute);
		std::cout << "Can't reach user :(";
		pthread_mutex_unlock(&mute);
		close(server_socket);
		exit(errno);
	}
 
}

//game function

void* sea_fight (void* arg) {

	int ship_count = 0;
    int max_score = 0;
	int board_size;
	int* client_sockets = static_cast<int*>(arg);
	char* buffer = new char[100001];
    int bytes_recieved;

	//preparing for the game

	std::srand(std::time(nullptr));
	int cur_player = std::rand() % players_count;

	//board size

	BOARD_SIZE:

	std::string msg = "What should be the board size? Send the number:\n";

	if(!send_msg(client_sockets, client_sockets[cur_player], msg)) {
		delete[] buffer;
		return 0;
	}

	if(!recv_msg(client_sockets, client_sockets[cur_player], buffer, bytes_recieved)) {
		delete[] buffer;
		return 0;
	}

	board_size = std::stoi(buffer);
	cur_player = (cur_player + 1) % players_count;
	msg = "Your opponent wants to set the board size to ";
	msg += std::to_string(std::stoi(buffer));
	msg += ". Do you agree? y/n";
	
	if(!send_msg(client_sockets, client_sockets[cur_player], msg)) {
		delete[] buffer;
		return 0;
	}
	if(!recv_msg(client_sockets, client_sockets[cur_player], buffer, bytes_recieved)) {
		delete[] buffer;
		return 0;
	}
	if(buffer[0] == 'y'){	
	}
	else {
		goto BOARD_SIZE;
	}

	//ships size

	std::vector<int> ships;
	std::vector<int> save_ships;
	int value;

	SHIP_SIZE:

	msg = "Now lets determine ship sizes and count. Choose to your liking!\nThat's how: abc would mean you have a one-sized ships,\nb two-sized ships and so on.\n";

	if(!send_msg(client_sockets, client_sockets[cur_player], msg)) {
		delete[] buffer;
		return 0;
	}

	if(!recv_msg(client_sockets, client_sockets[cur_player], buffer, bytes_recieved)) {
		delete[] buffer;
		return 0;
	}

	cur_player = (cur_player + 1) % players_count;

	ships.resize(bytes_recieved);

	for(int i = 0; i < bytes_recieved; ++i) {
		ships[i] = buffer[i] - '0';
		ship_count += ships[i];
		max_score += ships[i] * (i + 1);
	}
	
	save_ships = ships;

	msg = "Your opponent wants to set the ship count to ";
	msg += std::to_string(ship_count);
	msg += " and sizes of ships: ";
	for(int i = 0; i < bytes_recieved; ++i) {
		msg += buffer[i];
	}
	msg += ". Do you agree? y/n\n";

	if(!send_msg(client_sockets, client_sockets[cur_player], msg)) {
		delete[] buffer;
		return 0;
	}

	if(!recv_msg(client_sockets, client_sockets[cur_player], buffer, bytes_recieved)) {
		delete[] buffer;
		return 0;
	}

	if(buffer[0] == 'y'){
		
	}
	else {
		goto SHIP_SIZE;
	}

	//player boards filling

	std::vector<std::vector<std::vector<int>>> real_board(players_count, std::vector<std::vector<int>>(board_size, std::vector<int>(board_size, 0)));


	std::vector<int> coords;
	int count = 0;

	int begin;
	int end;

	for(int i = 0; i < players_count; ++i) {

	if(!send_msg(client_sockets, client_sockets[i], "Now let's fill the board, ship after ship!\n")) {
		delete[] buffer;
		return 0;
	}

	BOARD_FILL:

	ships = save_ships;

	int ships_filled = 0;

	NEW_SHIP:

	coords = {};

	msg = "Remaining ships: ";
	for(int i = 0; i < ships.size(); ++i) {
		msg += std::to_string(ships[i]);
		msg += " ";
	}
	msg += "\n";

	if(!send_msg(client_sockets, client_sockets[i], msg)) {
		delete[] buffer;
		return 0;
	}

	msg = "Send the coordinates of the ";
	msg += std::to_string(ships_filled + 1);
	msg += " ship like this: x1 y1 x2 y2\n";

	if(!send_msg(client_sockets, client_sockets[i], msg)) {
		delete[] buffer;
		return 0;
	}	

	buffer[0] = {0};
	if(!recv_msg(client_sockets, client_sockets[i], buffer, bytes_recieved)) {
		delete[] buffer;
		return 0;
	}

	std::istringstream iss(buffer);
	//count = 0;
	while(iss >> value) {
		coords.push_back(value);
	}

	if(coords.size() != 4) {
		if(!send_msg(client_sockets, client_sockets[i], "Incorrect number of coordinates. Try again!\n")) {
					delete[] buffer;
					return 0;
				}
		goto NEW_SHIP;
	}

	if(!iss.eof()) {
		if(!send_msg(client_sockets, client_sockets[i], "Incorrect coordinates. Try again!\n")) {
			delete[] buffer;
			return 0;
		}
		goto NEW_SHIP;	
	}

	for(int i = 0; i < 4; ++i) {
			-- coords[i]; 
			if(coords[i] < 0 || coords[i] >= board_size) {
				if(!send_msg(client_sockets, client_sockets[i], "Coordinates out of border. Try again!\n")) {
					delete[] buffer;
					return 0;
				}
				goto NEW_SHIP;
			}
	}

	if(coords[0] == coords[2]) {
		begin = std::min(coords[1], coords[3]);
		end = std::max(coords[1], coords[3]);

		if (end - begin >= ships.size()) {
			if(!send_msg(client_sockets, client_sockets[i], "There's no ship with such value! Try again!\n")) {
				delete[] buffer;
				return 0;
			}
			goto NEW_SHIP;
		}
			
			
		if(ships[end - begin] > 0) {
		}
		else {
			if(!send_msg(client_sockets, client_sockets[i], "Incorrect ship value! Try again!\n")) {
				delete[] buffer;
				return 0;
			}
			goto NEW_SHIP;
		}


		for(int l = begin; l <= end; ++l) {
			if(real_board[i][l][coords[0]] == 1)  {
				if(!send_msg(client_sockets, client_sockets[i], "New ship overrides old one! Try again!\n")) {
					delete[] buffer;
					return 0;
				}
				goto NEW_SHIP;
			}
			real_board[i][l][coords[0]] = 1;
		}
	}
	else if(coords[1] == coords[3]) {
		begin = std::min(coords[0], coords[2]);
		end = std::max(coords[0], coords[2]);

		if (end - begin >= ships.size()) {
			if(!send_msg(client_sockets, client_sockets[i], "There's no ship with such value! Try again!\n")) {
				delete[] buffer;
				return 0;
			}
			goto NEW_SHIP;
		}

		if(ships[end - begin] > 0) {
		}
		else {
			if(!send_msg(client_sockets, client_sockets[i], "Incorrect ship value! Try again!\n")) {
				delete[] buffer;
				return 0;
			}
			goto NEW_SHIP;
		}

		for(int l = begin; l <= end; ++l) {
			if(real_board[i][l][coords[1]] == 1)  {
				if(!send_msg(client_sockets, client_sockets[i], "New ship overrides old one! Try again!\n")) {
					delete[] buffer;
					return 0;
				}
				goto NEW_SHIP;
			}
			real_board[i][l][coords[1]] = 1;
		}
	}
	else {

			if(!send_msg(client_sockets, client_sockets[i], "Incorrect coordinates. Try again!\n")) {
						delete[] buffer;
						return 0;
			}
			goto NEW_SHIP;
		}	

		--ships[end - begin];

		if(ships_filled < ship_count - 1) {
			++ships_filled;
			goto NEW_SHIP;
		}

	}

	//game process

	std::cout << "Game starts! \n";

	//try
	for(int i = 0; i < players_count - 1; ++i) {
		draw_boards(client_sockets[i], real_board[i], real_board[i + 1], board_size);
	}
	draw_boards(client_sockets[players_count - 1], real_board[players_count - 1], real_board[0], board_size);
	//try

	std::vector<int> scores(players_count, 0);

	

	bool r_break = false;

	std::vector<int> xy(2, 0);

	int opponent;
	int o_opponent;

	SEND_COORDINATES:

	while(true) {
		if(!send_msg(client_sockets, client_sockets[cur_player], "Send the coordinates like this: x y\n")) {
			delete[] buffer;
			return 0;
		}

		if(!recv_msg(client_sockets, client_sockets[cur_player], buffer, bytes_recieved)) {
			delete[] buffer;
			return 0;
		}

		std::istringstream iss(buffer);
		count = 1;
		while(iss >> xy[count]) {
			--count;
			if(count < 0) {
				break;
			}
		}

		if(!iss.eof()) {
			if(!send_msg(client_sockets, client_sockets[cur_player], "Incorrect coordinates. Try again!\n")) {
				delete[] buffer;
				return 0;
			}
			goto SEND_COORDINATES;	
		}	

		for(int i = 0; i < 2; ++i) {
			-- xy[i]; 
			if(xy[i] < 0 || xy[i] >= board_size) {
				if(!send_msg(client_sockets, client_sockets[cur_player], "Coordinates out of border. Try again!\n")) {
					delete[] buffer;
					return 0;
				}
				goto SEND_COORDINATES;
			}
		}

		opponent = (cur_player + 1) % players_count;
		o_opponent = (cur_player + 2) % players_count;
		
		if(real_board[opponent][xy[0]][xy[1]] == 1) {

				++ scores[cur_player];
				real_board[opponent][xy[0]][xy[1]] = 2;

				if(!send_msg(client_sockets, client_sockets[opponent], "Your ship has been damaged!\n")) {
					delete[] buffer;
					return 0;
				}

				draw_boards(client_sockets[opponent], real_board[opponent], real_board[o_opponent], board_size);

				if(!send_msg(client_sockets, client_sockets[cur_player], "Bullseye!\n")) {
					delete[] buffer;
					return 0;
				}

				draw_boards(client_sockets[cur_player], real_board[cur_player], real_board[opponent], board_size);
				

			}
		else if(real_board[opponent][xy[0]][xy[1]] == 0)  {

				real_board[opponent][xy[0]][xy[1]] = 3;		
				
				if(!send_msg(client_sockets, client_sockets[opponent], "Phew, nothing!\n")) {
					delete[] buffer;
					return 0;
				}

				draw_boards(client_sockets[opponent], real_board[opponent], real_board[o_opponent], board_size);

				if(!send_msg(client_sockets, client_sockets[cur_player], "Meh, nothing!\n")) {
					delete[] buffer;
					return 0;
				}

				draw_boards(client_sockets[cur_player], real_board[cur_player], real_board[opponent], board_size);

				cur_player = opponent;
				
		}
		else if(real_board[opponent][xy[0]][xy[1]] == 2 || real_board[opponent][xy[0]][xy[1]] == 3) {
			if(!send_msg(client_sockets, client_sockets[cur_player], "Already shoot there! Try again!\n")) {
					delete[] buffer;
					return 0;
			}
			goto SEND_COORDINATES;
		}


	for(int i = 0; i < players_count; ++i) {
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
			if(!send_msg(client_sockets, client_sockets[i], "You win :)\n")) {
					delete[] buffer;
					return 0;
			}
		}
		else {
			if(!send_msg(client_sockets, client_sockets[i], "You lose :(\n")) {
					delete[] buffer;
					return 0;
			}
		}
		close(client_sockets[i]);
	}

	delete[] buffer;
	delete[] client_sockets;

	return 0;

}

//main
 
int main() {


  int bd_fd;

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

	  int* sockets = new int[players_count];

	  for(int i = 0; i < players_count; ++i) {
      	if ((sockets[i] = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0) {
          	perror("accept failed");
          	-- i;
			continue;
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
