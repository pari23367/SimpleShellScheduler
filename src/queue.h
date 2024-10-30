#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_PROCESSES 200

typedef struct {
    int pid;
    char name[256];
    int completion_time;
    int wait_time;
    int priority;
    struct timeval wait_start_time;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int front, rear;
} Queue;

Queue* create_shared_queue(); // Function prototype
void initQueue(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, Process process);
Process dequeue(Queue *q);
int getQueueSize(Queue *q);
#endif

