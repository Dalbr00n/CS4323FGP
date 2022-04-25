//Author: Daniel Albrecht
//Email: daniel.albrecht@okstate.edu
//Date: 04/12/2022
//Program Description: This program incorporates semaphores to establish
// a synchronization structure across multiple theads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//Create variables for semaphore and command line arguements
sem_t mutex;
int value = 0;


//No input parameters and no return value
void* thread(void* arg)
{
	sem_wait(&mutex);

// -----------------------//
    if(value == 0) value++;
    else value--;
    printf("Value: %d\n", value);
// -----------------------//
	//signal
	sem_post(&mutex);
}

//Doctor thread, will contain functions
void* doctor(void* arg)
{
	sem_wait(&mutex);
	printf("Wait for patient\n");
	sem_post(&mutex);
	sleep(2);
	sem_wait(&mutex);
	printf("performMedicalCheckup\n");
	sem_post(&mutex);
	sleep(2);
	
	sem_wait(&mutex);
	printf("Accept Payment\n");
	sem_post(&mutex);
	sleep(2);
	
	// sem_wait(&mutex);
	// printf("\n");
	// sem_post(&mutex);

}

//Patient thread, will contain patient
void* patient(void* arg)
{
	sleep(2);
	sem_wait(&mutex);
	printf("getMedicalCheckup\n");
	sem_post(&mutex);
	sleep(2);
	
	sem_wait(&mutex);
	printf("makePayment\n");
	sem_post(&mutex);
	sleep(2);
	
	sem_wait(&mutex);
	printf("leaveClinic\n");
	sem_post(&mutex);
	sleep(2);
	
}




int main(int argc, char* argv[])
{
    //Grab variable and place into integer holder
    // value = atoi(argv[1]);
    // printf("Value: %d\n", value);

    //initailize mutex lock and create threads for doctor and patient
	sem_init(&mutex, 0, 1);
	pthread_t t1,t2;
	pthread_create(&t1,NULL,doctor,NULL);
	pthread_create(&t2,NULL,patient,NULL);

    //end threads when completed
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

    //destroy semaphore
	sem_destroy(&mutex);
	return 0;
}
