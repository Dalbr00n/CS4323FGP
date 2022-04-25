// Lucas Stott lstott@okstate.edu
// CS-4323 Groud D
// Final Group Project 
// Run gcc main.c -o main -lpthread && ./main 2 100 3 3 10 100


#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

int max_capacity;
int doctors; 
int arrival;
int duration;
int wait_count = 0;
int sofa_count = 0;
int patient_count = 0;
int current_capacity = 0;
int queue = 0;
int *waitingRoom;
int waitingRoom_size;
int *sofas;
int sofas_size;
int *patients; 
int patients_size;
int sem_value;

pthread_mutex_t outputQueue = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condQueue, waitQueue, sofaQueue, checkupQueue, getCheckupQueue, makePaymentQueue = PTHREAD_COND_INITIALIZER;
//sem_t semEmpty; // Change Cond to Semaphore for WaitingRoom > Sofa > GetCheckup
//sem_t semFull;  // Other Semaphore for Post() Wait() functions
sem_t sem_waitingRoom;
sem_t sem_sofaCount;
sem_t sem_register;


void* patientArrival(void* args) {
    pthread_mutex_lock(&outputQueue);
    int patient = *(int *)args;
    printf("Patient: %d (Thread ID:%lu): Arriving At Clinic\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);

    
    sem_getvalue(&sem_waitingRoom, &sem_value);
    if(sem_value == 0) leaveClinic(patient);
    else enterWaitingRoom(patient);
}

void leaveClinic(int patient) {
    pthread_mutex_lock(&outputQueue); // Lock to Main Threads
    queue++;
    printf("Patient: %d (Thread ID:%lu): Leaving the Clinic without checkup\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    pthread_kill();
}

void enterWaitingRoom(int patient) {
    sem_wait(&sem_waitingRoom);
    pthread_mutex_lock(&outputQueue);
    printf("Patient: %d (Thread ID:%lu): Entering Waiting Room\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    
    sem_wait(&sem_sofaCount);
    sitOnSofa(patient);
}

void sitOnSofa(int patient) {
    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    printf("Patient: %d (Thread ID:%lu): Sitting On Sofa\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    
    pthread_cond_signal(&waitQueue);
    pthread_cond_signal(&checkupQueue);
    getMedicalCheckup(patient);
    pthread_cond_signal(&getCheckupQueue);
    makePayment(patient);
    pthread_cond_signal(&makePaymentQueue);
    leaveClinic(patient);
}

void getMedicalCheckup(int patient) {
    pthread_mutex_lock(&outputQueue); 
    printf("Patient: %d (Thread ID:%lu): Getting Medical Checkup\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    pthread_cond_signal(&getCheckupQueue);
}

void makePayment(int patient) {
    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    printf("Patient: %d (Thread ID:%lu): Making Payment\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    pthread_cond_signal(&makePaymentQueue);
}

void performMedicalCheckup(int patient) {
    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    printf("Medical Professional (Thread ID:%lu): Checking Patient %d\n", pthread_self(), patient);
    pthread_mutex_unlock(&outputQueue);
}

void* waitForPatients(void* args) {
    while (1) {
        pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
        printf("Medical Professional (Thread ID:%lu): Waiting for Patient\n", pthread_self());
        while (sofa_count == 0) { // Conditional Wait for Patient Entry Onto Sofa
            pthread_cond_wait(&checkupQueue, &outputQueue);
        }
        pthread_cond_signal(&sofaQueue);
        pthread_mutex_unlock(&outputQueue);
        performMedicalCheckup(queue);
    }
}

int main(int argc, char* argv[]) {

    doctors = atoi(argv[1]); // Doctor Size
    patients_size = atoi(argv[2]); // Gets Argument for Number of Patients 
    patients = malloc(sizeof(int)*patients_size); // Dynamic Heap Memory
    waitingRoom_size = atoi(argv[3]); // Waiting Room Size
    waitingRoom = malloc(sizeof(int)*patients_size); // Dynamic Heap Memory
    sofas_size = atoi(argv[4]); // Amount of Sofas
    sofas = malloc(sizeof(int)*patients_size); // Dynamic Heap Memory
    arrival = atoi(argv[5]); // Wait time between Patients Arrival / Random 
    duration = atoi(argv[6]); // Duration of Checkup

    srand(time(NULL)); // Random Number Seed
    
    max_capacity = sofas_size + waitingRoom_size; // WaitRoom + Sofas
    int time_arrival = arrival * 1000; // Turn into Milliseconds 

    //sem_init(&semEmpty, 0, waitingRoom_size); // Semaphore Queue
    // sem_init(&semEmpty, 0, waitingRoom_size);  
    // sem_init(&semEmpty, 0, sofas_size);
    sem_init(&sem_waitingRoom, 0, waitingRoom_size); 
    sem_init(&sem_sofaCount, 0, sofas_size);
    sem_init(&sem_register, 0, 1);
    //sem_init(&semFull, 0, 0); // Semaphore Empty 
    //sem_init(&semFull, 0, 0);

    

    pthread_t tw[doctors];  // Worker Threads 
    pthread_t tp[patients_size]; // Patient Threads

    for (int i = 0; i < doctors; i++) { // Initialize Worker Threads 
        if (pthread_create(&tw[i], NULL, &waitForPatients, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }

    for (int i = 0; i < patients_size; i++) { // Patient Thread Creation
        int wait_time = rand()%time_arrival;
        printf("Arrival Time for Patient[%i]: %d\n",i, wait_time);
        usleep(wait_time);
        if (pthread_create(&tp[i], NULL, &patientArrival, &i) != 0) {
            perror("Failed to create the thread");
        }
        //increment queue
    }
    
    for (int i = 0; i < doctors; i++) {
        if (pthread_join(tw[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }

    pthread_mutex_destroy(&outputQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}