#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct QueueNode* add(int patientNum);
struct Queue* create();
int addToQueue(struct Queue* q, int patientNum);
void printQueue(struct Queue* q);
#endif
