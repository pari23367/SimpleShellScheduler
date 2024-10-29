// simple_scheduler.h
#ifndef SIMPLE_SCHEDULER_H
#define SIMPLE_SCHEDULER_H
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

extern int NCPU;     // Number of CPUs (or processes to run concurrently)
extern int TSLICE;   // Time slice in milliseconds

// Initializes the scheduler with a specified number of CPUs and time slice
void initialize_scheduler(int ncpu, int tslice);

// Adds a new process to the scheduling queue
void add_process(Queue *ready_queue, Process new_process);

// Signals processes to resume or pause based on the scheduling algorithm
void signal_processes();

// Main loop that controls the scheduling process
void scheduler_loop();

#endif // SIMPLE_SCHEDULER_H

