// simple_scheduler.c

#include "simple_scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Global variables for the scheduler
Queue *ready_queue; 
int NCPU;       // Number of CPUs (processes to run concurrently)
int TSLICE;     // Time slice in milliseconds

// Initialize scheduler settings
void initialize_scheduler(int ncpu, int tslice) {
    NCPU = ncpu;
    TSLICE = tslice;
    ready_queue = malloc(sizeof(Queue));
    if (ready_queue == NULL) {
        perror("Failed to allocate memory for ready_queue");
        exit(EXIT_FAILURE);
    }
    initQueue(ready_queue);
}

// Add a new process to the scheduler
void add_process(Queue *ready_queue, Process new_process) {
    printf("Reached add process function\n");
    printf("process name=%s\n",new_process.name);
    //if (ready_queue->rear >= MAX_PROCESSES - 1) {
      //  printf("Queue is full. Cannot add process.\n");
        //return; // Optionally handle this case (e.g., by resizing or rejecting new processes)
    //}

    //printf("Incrementing rear\n");
    // Increment rear and add the new process to the queue
    //ready_queue->rear++;
    //printf("Rear=%d\n",ready_queue->rear);
    //ready_queue->processes[ready_queue->rear] = new_process; // Add the process to the queue
    enqueue(ready_queue,new_process);
    // Print debug information
    printf("Process added to queue: PID=%d, Name=%s, Completion Time=%d, Wait Time=%d, Priority=%d\n", new_process.pid, new_process.name, new_process.completion_time, new_process.wait_time, new_process.priority);
}

//To run a process
void signal_processes() {
    for (int i = 0; i < NCPU; i++) {
        if (!isEmpty(ready_queue)) { // Check if the queue is not empty
            printf("Dequeue-ing a process\n");
            Process process = dequeue(ready_queue);  // Dequeue a Process
            pid_t pid = process.pid;
            if (pid > 0) {
                // Send SIGCONT to resume the process
                kill(pid, SIGCONT);
            }
        }
    }
}

//Main loop that will run
void scheduler_loop() {
    while (1) {
        sleep(TSLICE / 1000); // Sleep for the time slice duration

        // Signal NCPU processes to run
        signal_processes();
        printf("Signaling processes to run\n");

        // After TSLICE, send SIGSTOP to running processes
        for (int i = 0; i < NCPU; i++) {
            if (!isEmpty(ready_queue)) { // Check if the queue is not empty
                Process process = dequeue(ready_queue);
                pid_t pid = process.pid;
                if (pid > 0) {
                    kill(pid, SIGSTOP);
                    enqueue(ready_queue, process); // Re-add to the queue
                }
            }
        }
    }
}
