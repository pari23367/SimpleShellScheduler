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
void add_process(Queue *ready_queue, pid_t pid, const char *name, int completion_time, int wait_time, int priority) {
    printf("Adding process to queue\n");
    Process new_process;
    new_process.pid = pid;
    snprintf(new_process.name, sizeof(new_process.name), "%s", name); // Copy the name safely
    new_process.completion_time = completion_time;
    new_process.wait_time = wait_time;
    new_process.priority = priority;
    printf("Adding adding\n");
    enqueue(ready_queue, new_process);  // Pass the Process instance
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
