#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>

#define MAX_PROCESSES 50

typedef struct {
    int pid;
    char name[256];
    int completion_time;
    int wait_time;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int front, rear;
} Queue;

void initQueue(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, Process process);
Process dequeue(Queue *q);

#endif

