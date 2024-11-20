#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <ctime>

class Integer {
private:
  int inner_value = 0;
  sem_t sem;
public:
  Integer(int value) :inner_value(value) {
  	if(sem_init(&sem, 0, 1) != 0) {
		perror("semaphore initialization error");
		exit(EXIT_FAILURE);
	}
  }

  ~Integer() {
  
	  if(sem_destroy(&sem) != 0) {
	  	perror("semaphore destruction failure");
		exit(EXIT_FAILURE);
	  }

  }

  void inc() {
    sem_wait(&sem);	  
    inner_value++;
    sem_post(&sem);
  }
  int get_inner_value() {
    return inner_value;
  }
};

void* increment(void* arg) {
  Integer* num = (Integer*)arg;
  for (int i = 0; i < 10000; ++i) {
    num->inc();
  }
  return nullptr;
}

int main(int argc, char** argv) {

  time_t start = clock();  
	
  Integer* num = new Integer(0);
  int thread_count = 50;
  pthread_t* tids = new pthread_t[thread_count];

  for (int i = 0; i < thread_count; ++i) {
    pthread_create(&tids[i], NULL, increment, num);  
  }

  for (int i = 0; i < thread_count; ++i) {
    pthread_join(tids[i], NULL);
  }

  time_t end = clock();

  std::cout << "Final value is " << num->get_inner_value() << std::endl;
  std::cout << "processor time needed: " << end - start << "\n";
  
  delete[] tids;
  delete num;
  return 0;
}
