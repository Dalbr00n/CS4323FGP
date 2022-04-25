#include "nathan.h"

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

int main(int argc, char *argv[]) {
   if(argc != 5) {
      printf("usage: nathan <max_patients> <waiting_room_cap> <num_sofas> <max_wait(ms)>\n");
      return 0;
   }

   int numPatients = atoi(argv[1]);
   int waitingRoomCapacity = atoi(argv[2]);
   int numSofas = atoi(argv[3]);
   int numStanding = waitingRoomCapacity - numSofas;
   int maxArrivalTime = atoi(argv[4]);

   struct Queue* sofas = create();
   sofas->max_size = numSofas;

   struct Queue* standing = create();
   standing->max_size = waitingRoomCapacity - numSofas;

   srand(time(NULL));

   for(int i = 0; i < numPatients; i++) {
      if(addToQueue(sofas, i) == 1) {
         printf("Patient %d is sitting in a sofa!\n", i);
      } else if(addToQueue(standing, i) == 1) {
         printf("Patient %d is standing!\n", i);
      } else {
         printf("Patient %d has been refused!\n", i);
      }
      int randomTime = rand() % atoi(argv[2]);
      printf("sleeping for %d milliseconds now\n", randomTime);

      struct timespec tim, tim2;
      tim.tv_sec = randomTime / 1000;
      tim.tv_nsec = randomTime % 1000 * 1000000L;
      nanosleep(&tim, &tim2);
   }
   printf("Sitting:\n");
   printQueue(sofas);
   printf("Standing:\n");
   printQueue(standing);
   return 0;
}
