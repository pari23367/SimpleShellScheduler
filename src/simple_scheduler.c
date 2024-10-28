// simple_scheduler.c

#include "simple_scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// Global variables for the scheduler
ProcessQueue ready_queue;
int NCPU;       // Number of CPUs (processes to run concurrently)
int TSLICE;     // Time slice in milliseconds

// Initialize scheduler settings
void initialize_scheduler(int ncpu, int tslice) {
    NCPU = ncpu;
    TSLICE = tslice;
    initialize_queue(&ready_queue);
}

// Add a new process to the scheduler
void add_process(pid_t pid) {
    enqueue(&ready_queue, pid);
}

//To run a process
void signal_processes() {
    for (int i = 0; i < NCPU; i++) {
        pid_t pid = dequeue(&ready_queue);
        if (pid > 0) {
            // Send SIGCONT to resume the process
            kill(pid, SIGCONT);
        }
    }
}

//Main loop that will run
void scheduler_loop() {
    while (1) {
        sleep(TSLICE / 1000); // Sleep for the time slice duration

        // Signal NCPU processes to run
        signal_processes();

        // After TSLICE, send SIGSTOP to running processes
        for (int i = 0; i < NCPU; i++) {
            pid_t pid = dequeue(&ready_queue);
            if (pid > 0) {
                kill(pid, SIGSTOP);
                enqueue(&ready_queue, pid); // Re-add to the queue
            }
        }
    }
