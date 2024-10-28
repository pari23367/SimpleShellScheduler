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

