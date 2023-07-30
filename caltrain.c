#include <pthread.h>
#include "caltrain.h"
#include <stdlib.h>




void station_init(struct station *station)
{
	station->freeSeats = 0;
	station->passengersCount = 0;
	station->checkSeats = 0;

	//init the mutex and the condation variables
	if(pthread_mutex_init(&(station->mutexLock), NULL) != 0){
		perror("pthread_mutex_init() error");
		exit(1);
	}

	if(pthread_cond_init(&(station->train_arrive), NULL) != 0){
		perror("pthread_cond_init() error");
		exit(1);
	}

	if(pthread_cond_init(&(station->train_leave), NULL) != 0){
		perror("pthread_cond_init() error");
		exit(1);
	}
}

//count represents number of available seats in the train
void station_load_train(struct station *station, int count)
{
	station->freeSeats = count;													
	pthread_mutex_lock(&(station->mutexLock));
	station->checkSeats = count;												//to check if the passenger can seat or not
	if(station->freeSeats > 0 && station->passengersCount > 0){					//if that condition was false return promptly 
		pthread_cond_broadcast(&(station->train_arrive));						//signal all passengers
		pthread_cond_wait(&(station->train_leave), &(station->mutexLock));		//wait for train_leave condition variable
	}
	pthread_mutex_unlock(&(station->mutexLock));
}


void station_wait_for_train(struct station *station)
{
	pthread_mutex_lock(&(station->mutexLock));
	station->passengersCount++;
	pthread_cond_wait(&(station->train_arrive), &(station->mutexLock));		//wait for train_arrive condition variable
	station->checkSeats--;
	while (station->checkSeats < 0)											
	{
		pthread_cond_wait(&(station->train_arrive), &(station->mutexLock));	//wait for another train if the freeseats weren't enough
		station->checkSeats--;
	}
	pthread_mutex_unlock(&(station->mutexLock));
}


void station_on_board(struct station *station)
{
	pthread_mutex_lock(&(station->mutexLock));
	if(station->freeSeats > 0 && station->passengersCount > 0){
		station->freeSeats--; station->passengersCount--;				//after a passenger has been seated
	}
	pthread_mutex_unlock(&(station->mutexLock));

	if(station->freeSeats == 0 || station->passengersCount == 0)		//if this condition happened the train should leave
		pthread_cond_signal(&(station->train_leave));					
}
