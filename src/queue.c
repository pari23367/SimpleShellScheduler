#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

//Initialising queue
void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

Queue* create_shared_queue() {
    int shm_fd = shm_open("/my_queue", O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(shm_fd, sizeof(Queue)) == -1) {
        perror("Failed to set size of shared memory");
        exit(EXIT_FAILURE);
    }
    
    Queue* queue = mmap(NULL, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }
    
    return queue;
}

// Function that return 1 if queue is empty
int isEmpty(Queue *q) {
    return (q->front == -1);
}

// Adds a new process to the queue with priority ordering
void enqueue(Queue *q, Process process) {
    printf("Starting to enqueue process %s\n", process.name);

    printf("Check if the queue is full");
    if (q->rear >= MAX_PROCESSES - 1) {
        fprintf(stderr, "Queue Overflow: Unable to add process %s\n", process.name);
        return;
    }

    // Increment rear to add the new process at the next position
    q->rear++;

    printf("Find the correct position to insert based on priority (higher priority first)");
    int i = q->rear;
    while (i > q->front && q->processes[i - 1].priority < process.priority) {
        q->processes[i] = q->processes[i - 1]; // Shift elements to the right
        i--;
    }

    printf("Insert the new process at the found position");
    q->processes[i] = process;
    printf("Enqueued process: %s with priority: %d\n", process.name, process.priority);
}

// Removes and returns the first process in the queue
Process dequeue(Queue *q) {
    if (isEmpty(q)) {
        fprintf(stderr, "Queue Underflow\n");
        exit(EXIT_FAILURE);
    }

    // Get the process at the front of the queue
    Process process = q->processes[q->front];
    
    // Move the front index to the next element
    q->front++;

    // Reset the queue if it's empty after the dequeue operation
    if (isEmpty(q)) {
        q->front = 0;
        q->rear = -1;
    }

    return process;
}
