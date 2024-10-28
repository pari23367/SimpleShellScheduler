#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

//Initialising queue
void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

// Function that return 1 if queue is empty
int isEmpty(Queue *q) {
    return (q->front == -1);
}

// Adds new element to our circular queue
void enqueue(Queue *q, Process process) {
    if ((q->rear + 1) % MAX_PROCESSES == q->front) {
        fprintf(stderr, "Overflow\n");
        return;
    }
    if (isEmpty(q)) {
        q->front = 0;
    }
    
    // Find the correct position to insert based on priority
    int i = q->rear;
    while (i >= q->front && q->processes[i].priority < process.priority) {
        q->processes[(i + 1) % MAX_PROCESSES] = q->processes[i]; // Shift elements to make room
        i = (i - 1 + MAX_PROCESSES) % MAX_PROCESSES; // Wrap around
    }

    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->processes[q->rear] = process;
}

//Removes and returns first element of queue
Process dequeue(Queue *q) {
    if (isEmpty(q)) {
        fprintf(stderr, "Underflow\n");
        exit(EXIT_FAILURE);
    }
    Process process = q->processes[q->front];
    if (q->front == q->rear) {
        q->front = q->rear = -1;
    } 
    else {
        q->front = (q->front + 1) % MAX_PROCESSES;
    }
    return process;
}

