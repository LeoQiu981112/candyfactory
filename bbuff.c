#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "bbuff.h"



void* buffer[BUFFER_SIZE];

pthread_mutex_t mutex;
sem_t isEmpty;
sem_t isFull;
int candynum;


void bbuff_init(){
	sem_init(&isEmpty,0,BUFFER_SIZE);
	sem_init(&isFull,0,0);
	candynum=0;
}


void bbuff_blocking_insert(void* item){
	sem_wait(&isEmpty);
	pthread_mutex_lock(&mutex);
	buffer[candynum++]=item;
	pthread_mutex_unlock(&mutex);
	sem_post(&isFull);
}

void* bbuff_blocking_extract(void){
	sem_wait(&isFull);
	pthread_mutex_lock(&mutex);
	void* top_candy=buffer[--candynum];
	pthread_mutex_unlock(&mutex);
	sem_post(&isEmpty);
	return top_candy;
}


_Bool bbuff_is_empty(void){
	if(candynum==0){
		return true;
	}
	else{
		return false;
	}
}
