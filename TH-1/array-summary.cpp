#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

struct arr_piece {

	int* arr;

	int start;
	int end;

	long long int sum;

};


void* thread_f(void* arg) {


	for(int i = ((arr_piece*)arg)->start; i < ((arr_piece*)arg)->end; ++i) {
		((arr_piece*)arg)->sum += ((arr_piece*)arg)->arr[i];
	}

	return nullptr;

}

int main (int argc, char* argv[]) {

	if(argc != 3) {
		perror("Incorrect number of args");
		exit(EXIT_FAILURE);
	}

	int N = std::atoi(argv[1]);
	int M = std::atoi(argv[2]);

	int* arr = new int[N];

	for(int i = 0; i < N; ++i) {
		arr[i] = std::rand() % 10;
	}

	time_t nt_start = clock();

	long long no_threads_sum = 0;

	for(int i = 0; i < N; ++i) {
		no_threads_sum += arr[i];
	}

	//std::cout << no_threads_sum << "\n";

	time_t nt_end = clock();

	//with threads


	time_t t_start = clock();

	pthread_t* threads = new pthread_t[M];

	arr_piece* pieces = new arr_piece[M];


	int piece_size = N / M;

	for (int i = 0; i < M; ++i) {
		
		pieces[i].start = piece_size * i;
		pieces[i].end = (i == ( M - 1) ? N : piece_size * (i + 1));
		pieces[i].sum = 0;
		pieces[i].arr = arr;



		if((pthread_create(&threads[i], NULL, thread_f, &pieces[i])) != 0) {
		
			perror("Thread creating failure");
			exit(EXIT_FAILURE);

		}


	
	}

	long long sum = 0;
	for(int i = 0; i < M; ++i) {
		
		if((pthread_join(threads[i], nullptr)) != 0) {
			
			perror("Thread join failure");
			exit(EXIT_FAILURE);
		}

		sum += pieces[i].sum;
	}


	delete[] pieces;
	delete[] threads;

	time_t t_end = clock();

	//std::cout << sum << "\n";

	delete[] arr;

	std::cout << "Time spent withoud threads: " << (nt_end - nt_start) / CLOCKS_PER_SEC << "\n";
	std::cout << "Time spent with threads: " << (t_end - t_start) / CLOCKS_PER_SEC << "\n";

	return 0;
}
