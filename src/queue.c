#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

//Initialising queue
void initQueue(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

// Function that return 1 if queue is empty
int isEmpty(Queue *q) {
    return (q->front == -1);
}

// Adds new element to our circular queue
void enqueue(Queue *q, Process process) {
    printf("Starting to enqueue\n");

    // Check if the queue is full
    //if ((q->rear + 1) % MAX_PROCESSES == q->front) {
      //  fprintf(stderr, "Queue Overflow: Unable to add process %s\n", process.name);
        //return; // Return early if the queue is full
    //}
    printf("Reached checkpint 1\n");
    // Initialize queue if it is currently empty
    if (isEmpty(q)) {
        printf("Queue was empty before\n");
        q->front = 0;
        q->rear = 0;
        q->processes[q->rear] = process; // Directly assign the process
        printf("Enqueued the process (first element)\n");
        return; // Early return since we added the first element
    }

    // Find the correct position to insert based on priority
    printf("Finding position in queue\n");
    int i = q->rear;

    // Shift elements to make room for the new process
    while (i >= q->front && q->processes[i].priority < process.priority) {
        q->processes[(i + 1) % MAX_PROCESSES] = q->processes[i]; // Shift elements
        i = (i - 1 + MAX_PROCESSES) % MAX_PROCESSES; // Wrap around
    }

    // Update the rear to point to the next position
    q->rear = (q->rear + 1) % MAX_PROCESSES;

    // Insert the new process
    q->processes[q->rear] = process;
    printf("Enqueued the process: %s with priority: %d\n", process.name, process.priority);
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

