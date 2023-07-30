#include <pthread.h>

struct station {
	int passengersCount;  			//passengers waiting in the station
	int freeSeats;	   				//freeSeats in the train
	int checkSeats;					//to check if there are enough seats for the passengers
	pthread_cond_t train_arrive;	//condition variable for the train arrive
	pthread_cond_t train_leave;		//condition variable for the train leave
	pthread_mutex_t mutexLock;
	
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);