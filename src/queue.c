#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>


//Initialising queue
void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

int getQueueSize(Queue *q) {
    if (isEmpty(q)) {
        return 0;
    }
    return q->rear - q->front + 1;
}

Queue* create_shared_queue(sem_t **queue_lock) {
    // Create shared memory for the queue
    int shm_fd = shm_open("/my_queue", O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("Failed to open shared memory for queue");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(Queue)) == -1) {
        perror("Failed to set size of shared memory for queue");
        exit(EXIT_FAILURE);
    }
    Queue* queue = mmap(NULL, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED) {
        perror("Failed to map shared memory for queue");
        exit(EXIT_FAILURE);
    }
//    printf("queue inside\n");
    // Initialize queue structure
    initQueue(queue);
//    printf("Top of the queue%s\n", queue->processes[queue->front].name);
    // Create shared memory for the semaphore
    int sem_fd = shm_open("/my_semaphore", O_CREAT | O_RDWR, 0666);
    if (sem_fd < 0) {
        perror("Failed to open shared memory for semaphore");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(sem_fd, sizeof(sem_t)) == -1) {
        perror("Failed to set size of shared memory for semaphore");
        exit(EXIT_FAILURE);
    }
    *queue_lock = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, sem_fd, 0);
    if (*queue_lock == MAP_FAILED) {
        perror("Failed to map shared memory for semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphore with value 1 (unlocked state)
    if (sem_init(*queue_lock, 1, 1) == -1) {
        perror("Failed to initialize semaphore");
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
  //  printf("Starting to enqueue process %s\n", process.name);
    // q->processes[q->front].name
   // printf("Check if the queue is full");
    if (q->rear >= MAX_PROCESSES - 1) {
        fprintf(stderr, "Queue Overflow: Unable to add process %s\n", process.name);
        return;
    }
    if (q->front == -1) {
        q->front = 0;
    }
    // Increment rear to add the new process at the next position
    q->rear++;

    //printf("Find the correct position to insert based on priority (higher priority first)");
    int i = q->rear;
    while (i > q->front && q->processes[i - 1].priority < process.priority) {
        q->processes[i] = q->processes[i - 1]; // Shift elements to the right
        i--;
    }

    //printf("Insert the new process at the found position");
    q->processes[i] = process;
    //printf("Enqueued process: %s with priority: %d\n", process.name, process.priority);
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
    if (q->front > q->rear) {
        q->front = -1;
        q->rear = -1;
    }
    return process;
}
