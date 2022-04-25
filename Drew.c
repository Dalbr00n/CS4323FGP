// Author: Drew Nguyen
// Email: drew.nguyen@okstate.edu
// Doctor functions, synchronisation, mutex's and semaphore's


#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//here are the Semaphores we use for Synchronisation
pthread_mutex_t mutexQueue,mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condQueue, waitQueue, sofaQueue, checkupQueue, getCheckupQueue, makePaymentQueue = PTHREAD_COND_INITIALIZER;



//"Waiting for patient"
//Patient will wait until he got the turn
void* waitForPatients(void* args) {
    while (1) {
        pthread_mutex_lock(&mutexQueue); // Lock to Thread that Called
        printf("Medical Professional (Thread ID:%lu): Waiting for Patient\n", pthread_self());
        while (sofa_count == 0) { // Conditional Wait for Patient Entry Onto Sofa
            pthread_cond_wait(&checkupQueue, &mutexQueue);
        }
        //pthread_cond_signal(&sofaQueue);
        pthread_mutex_unlock(&mutexQueue);
        performMedicalCheckup(queue);
    }
}


//invoke when getMedicalCheckup() is invoked
//After "Patient X: Arriving the clinic" AND
//After "Patient X: Sitting on a sofa in the waiting room"
//"Checking Patient X"
void performMedicalCheckup(int patient) {
    pthread_mutex_lock(&mutexQueue); // Lock to Thread that Called
    printf("Medical Professional (Thread ID:%lu): Checking Patient %d\n", pthread_self(), patient);
    pthread_mutex_unlock(&mutexQueue);
}

//invoke after Patient: makePayment()
//After "Patient: Getting Checkup" AND
//After "Making Payment to Medical Professional X"
//"Accepting Payment from Patient X"
void acceptPayment(struct Medical_professional M,int patient){
//pthread
  pthread_mutex_lock(&mutexQueue); // Lock to Thread that Called
  printf("Medical Professional: %d (Thread ID:%lu): Accepting Payment %d\n", M.Id,  pthread_self(),patient);
  pthread_mutex_unlock(&mutexQueue);
}



//Medical professional struct here
//its has two attributes, one if Doctors ID and 2nd is the status of the doctor that is waiting,checkup or payment 
struct Medical_professional{
  int Id;
  char status[10];
};
  void Change_id(int I,struct Medical_professional M){
    M.Id=I;
  }
  void update_status_to_wait(struct Medical_professional M){
    strcpy(M.status,"waiting");
  }
  void update_status_to_checkup(struct Medical_professional M){
    strcpy(M.status,"checkup");
  }
   void update_status_to_payment(struct Medical_professional M){
     strcpy(M.status,"payment");
  }


//Queue struct here
//Other queue related structs here
// this struct has Queue Node structure with Patient Number and Next Patient Store in it 
struct QueueNode{
   int patientNum;
   struct QueueNode* next;
};
struct Queue {
   struct QueueNode *front, *rear;
   int max_size;
   int size;
};
struct QueueNode* add(int x) {
   struct QueueNode* temp = (struct QueueNode*)malloc(sizeof(struct QueueNode));
   temp->patientNum = x;
   temp->next = NULL;
   return temp;
}

struct Queue* create() {
   struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
   queue->front = NULL;
   queue->rear = NULL;
}

int addToQueue(struct Queue* q, int patientNum) {
   if(q->size >= q->max_size) {
      return 0;
   }
   else {
      struct QueueNode* temp = add(patientNum);
      q->size += 1;
      if(q->rear == NULL) {
         q->front = temp;
         q->rear = temp;
      }
      q->rear->next = temp;
      q->rear = temp;
   }
   return 1;
}

void printQueue(struct Queue* q) {
   struct QueueNode* current = q->front;
   int i = 0;
   while(current != NULL) {
      printf("Patient %d\n", current->patientNum);
      i += 1;
      current = current->next;
   }
}



//doctor threads
//This thread perform all the functions related to Doctor like Waiting, Peform MEdical Checkup and Accept Payment
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
    printf("Accept Payemt\n");
    sem_post(&mutex);
    sleep(2);
}

//Patient thread, will contain patient
//This thread perform all the functions related to Patient like Get Medical Checkuo, Make Payment and Left the Clinic
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
void printfun(){
printf("Average Wait Time : %d",numberOfMedicalProfessionals*perPatientCheckupTime);

}

//main function here {
int main(int argc, char** argv)
{

//number of med professionals, N_m
int numberOfMedicalProfessionals;
numberOfMedicalProfessionals=argv[1];//COmmand line Input used

//time required for each patient for checkup (ms)
int perPatientCheckupTime;
perPatientCheckupTime=argv[2];//COmmand line Input used

//queue structs here

//print statements here
printfun();//Print Function for Average Wait TIme
//}
 }