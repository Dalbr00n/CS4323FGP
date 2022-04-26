// Lucas Stott lstott@okstate.edu
// CS-4323 Groud D
// Final Group Project 
// Run gcc main.c -o main -lpthread && ./main 2 100 3 3 10 100

// gcc main.c -o main -lpthread && ./main 2 100 3 3 10 100
// Arguments (2, 100, 3, 3, 10 , 100)
// 1. Number Medical Proffesions (Nm)
// 2. Number of Patients (Np)
// 3. Waiting Room Capacity (Nw)
// 4. Number of Sofas (Ns)
// 5. Maximum Arrival Time Between Patients (ms)
// 6. Check-up Duration (ms)

#include <stdio.h>
#include <string.h>
#include <pthread.h>    // Threading Resource
#include <stdlib.h>
#include <unistd.h>     // Unix Layer
#include <semaphore.h>  // Semaphore Resource
#include <time.h>       // Timer -> Seeded in Main

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
sem_t sem_doctors;
sem_t sem_entry;
sem_t sem_exit;

/* Patient Thread for (Entering Clinic) or (Leaving Without Checkup) */
void* patientArrival(void* args) {
    /* Mutex Lock for Print Statement */
    pthread_mutex_lock(&outputQueue);
    int patient = *(int *)args;
    printf("Patient: %d (Thread ID:%lu): Arriving At Clinic\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    
    /* Critical Section Semaphore Unlock Pass to Function */
    sem_getvalue(&sem_waitingRoom, &sem_value); // Passes value sem_waitingRoom to sem_value 
    if(sem_value == 0) // If sem_value is 0 -> waitingRoom is Full 
        leaveClinic(patient);
    else enterWaitingRoom(patient);
}

/* Patient Leaves Clinic -> No Checkup and Patient Thread Killed */
void leaveClinic(int patient) {
    /* Mutex Lock for Print Statement */
    pthread_mutex_lock(&outputQueue); 
    printf("Patient: %d (Thread ID:%lu): Leaving the Clinic without checkup\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    pthread_kill(); // Kills thread
}

/* Patient Enters Waiting room Queue | Waiting for Open Sofa Seat */
void enterWaitingRoom(int patient) {
    /* Mutex Lock for Print Statement */
    pthread_mutex_lock(&outputQueue);
    printf("Patient: %d (Thread ID:%lu): Entering Waiting Room\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);

    /* Critical Section */
    sem_wait(&sem_waitingRoom); // Incriment Semaphore sem_waitingRoom
    sitOnSofa(patient); 
}

/* Patient Sits on Sofa Queue | Waiting for Doctor */ 
void sitOnSofa(int patient) {
    /* Mutex Lock for Print Statement */
    pthread_mutex_lock(&outputQueue); 
    printf("Patient: %d (Thread ID:%lu): Sitting On Sofa\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);

    /* Critical Section */

    sem_wait(&sem_sofaCount);

    sem_post(&sem_waitingRoom);
    sem_post(&sem_doctors);

    sem_wait(&sem_entry);
    getMedicalCheckup(patient);
    sem_post(&sem_exit);

    sem_wait(&sem_entry);
    makePayment(patient);
    sem_post(&sem_exit);

    sem_wait(&sem_entry);
    leaveClinic(patient);
    sem_post(&sem_exit);
}

/* Patient Starts Checkup With Medical Professional */
void getMedicalCheckup(int patient) {

    usleep(duration * 1000);
    pthread_mutex_lock(&outputQueue); 
    printf("Patient: %d (Thread ID:%lu): Getting Medical Checkup\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    

}

/* Patient Makes Payment */
void makePayment(int patient) {

    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    printf("Patient: %d (Thread ID:%lu): Making Payment\n", patient,  pthread_self());
    pthread_mutex_unlock(&outputQueue);
    
}

/* Medical Profession Performs Checkup on Patient */
void performMedicalCheckup(int patient) {

    usleep(duration * 1000);
    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    printf("Medical Professional (Thread ID:%lu): Checking Patient %d\n", pthread_self(), patient);
    pthread_mutex_unlock(&outputQueue);
   
}

/* Medical Professional Accepts Patients Payment */
void acceptPayment(int patient) {

    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    printf("Medical Professional (Thread ID:%lu): Accepting Payment from Patient %d\n", pthread_self(), patient);
    pthread_mutex_unlock(&outputQueue);

}

/* Rotating Queue for Medical Professional to Process Patients */ 
void* waitForPatients(void* args) {
    pthread_mutex_lock(&outputQueue); // Lock to Thread that Called
    int doctor = *(int *)args;
    while (1) {
        printf("Medical Professional (Thread ID:%lu): Waiting for Patient\n", pthread_self());
        pthread_mutex_unlock(&outputQueue);

        sem_post(&sem_sofaCount);
        sem_wait(&sem_doctors);

        //sem_wait(&sem_exit);
        performMedicalCheckup(doctor);
        //sem_post(&sem_entry);

        sem_wait(&sem_exit);
        acceptPayment(doctor);
        sem_post(&sem_entry);
    }
}

int main(int argc, char* argv[]) {

    /* Arguments Passed Through Command Line */
    /* 6 Arguments (doctors, patient_size, waitingRoom_size, sofas_size, arrival, duration) */
    doctors = atoi(argv[1]);                        // Number of Medical Professionals (Nm)
    patients_size = atoi(argv[2]);                   // Number of Patients (Np) 
    waitingRoom_size = atoi(argv[3]);               // Waiting Room Capacity (Nw)
    sofas_size = atoi(argv[4]);                     // Number of Sofas (Ns)
    arrival = atoi(argv[5]);                        // Patient Max Arrival Time (ms) 
    duration = atoi(argv[6]);                       // Duration of Checkup (ms)

    /* ArrayList in Heap Memory */
    patients = malloc(sizeof(int)*patients_size);   // Patient Arraylist 
    waitingRoom = malloc(sizeof(int)*patients_size);// Waiting Room Arraylist 
    sofas = malloc(sizeof(int)*patients_size);      // Sofa Arraylist

    /* Random seed Generator */
    srand(time(NULL)); 
    
    /* Initialized Variables */
    max_capacity = sofas_size + waitingRoom_size;   // WaitRoom + Sofas
    int time_arrival = arrival * 1000;              // Turn into Milliseconds 
    int wait_time = 0;

    /* Semaphore Initialize */
    sem_init(&sem_waitingRoom, 0, waitingRoom_size); 
    sem_init(&sem_sofaCount, 0, sofas_size);
    sem_init(&sem_doctors, 0, doctors);
    sem_init(&sem_entry, 0, 0);
    sem_init(&sem_exit, 0, 1);

    // sem_init(&semEmpty, 0, waitingRoom_size); // Semaphore Queue
    // sem_init(&semEmpty, 0, waitingRoom_size);  
    // sem_init(&semEmpty, 0, sofas_size);
    // sem_init(&semFull, 0, 0); // Semaphore Empty 
    // sem_init(&semFull, 0, 0);

    
    /* threadPool Initialize */ 
    pthread_t tw[doctors];                         // Medical Professional Threads 
    pthread_t tp[patients_size];                   // Patient Threads

    /* Thread out all Medical Professional Procecess */ 
    for (int i = 0; i < doctors; i++) { 
        if (pthread_create(&tw[i], NULL, &waitForPatients, &i) != 0) {
            perror("Failed to create the thread");
        }
    }

    /* Thread out Patients at Random Time using time_arrival (0 < time_arrival) */ 
    for (int i = 0; i < patients_size; i++) { // Patient Thread Creation
        wait_time = rand()%time_arrival;
        printf("Arrival Time for Patient[%i]: %d\n",i, wait_time);
        usleep(wait_time);
        if (pthread_create(&tp[i], NULL, &patientArrival, &i) != 0) {
            perror("Failed to create the thread");
        }
    }
    
    /* Close out Medical Professional Threads */
    for (int i = 0; i < doctors; i++) {
        if (pthread_join(tw[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }

    pthread_mutex_destroy(&outputQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}

