#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>


/* global variables */
long long int number_in_circle;
pthread_mutex_t mutexm;

void* do_toss(void* t)
{
	long long int i;
	long long int p_number_in_circle = 0;
	long long int toss = (long long int) t;
	double x, y;
	
	unsigned int seed = time(NULL);
	
	for (i = 0;i<toss;i++) {
		x = 2.0*rand_r(&seed)/(RAND_MAX+1.0)-1.0;
		y = 2.0*rand_r(&seed)/(RAND_MAX+1.0)-1.0;
		
		if (x*x+y*y<=1)
			p_number_in_circle++;
	}
	
	pthread_mutex_lock(&mutexm);
	number_in_circle += p_number_in_circle;
	pthread_mutex_unlock(&mutexm);

	return NULL;
}

int main (int argc, char *argv[])
{
    // argv[1] = core number, argv[2] = number_of_tosses
	
	double pi_estimate;
	long long int number_of_tosses, toss, thread_count;
	pthread_t* thread_handles;
	long thread;
    
	thread_count = strtoll(argv[1], NULL, 10);
	number_of_tosses = strtoll(argv[2], NULL, 10);
	
	
	thread_handles = malloc (thread_count*sizeof(pthread_t));
	
	pthread_mutex_init(&mutexm, NULL);
	toss = number_of_tosses/thread_count;
	number_in_circle = 0;
	
	for (thread=0;thread<thread_count;thread++)
		pthread_create(&thread_handles[thread], NULL, do_toss, (void*)toss);


	for (thread=0;thread<thread_count;thread++)
		pthread_join(thread_handles[thread], NULL);


	pi_estimate = 4*number_in_circle/((double) number_of_tosses);
	printf("pi = %lf", pi_estimate);
	pthread_mutex_destroy(&mutexm);
	free(thread_handles);
	return 0;
}
