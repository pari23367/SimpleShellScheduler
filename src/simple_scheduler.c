// simple_scheduler.c

#include "simple_scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

// Global variables for the scheduler
extern Queue *ready_queue; 
extern int NCPU;       // Number of CPUs (processes to run concurrently)
extern int TSLICE;     // Time slice in milliseconds
extern sem_t *queue_lock;

// Global flag for scheduler loop control
volatile sig_atomic_t scheduler_running = 1;

// Signal handler for Ctrl+C
void scheduler_handle_sigint(int sig) {
    printf("\nScheduler terminating due to Ctrl+C\n");
    scheduler_running = 0;
    exit(0);
}

// Initialize scheduler settings
void initialize_scheduler(int ncpu, int tslice) {
    NCPU = ncpu;
    TSLICE = tslice;
    initQueue(ready_queue);
}

// Add a new process to the scheduler
void add_process(Queue *ready_queue, Process new_process) {
    //printf("Reached add process function\n");
    //printf("process name=%s\n",new_process.name);
    sem_wait(queue_lock);  // Lock the queue
    //printf("sem wait\n");
    printf("enqueue\n");
    enqueue(ready_queue, new_process);
    sem_post(queue_lock);  // Release the lock
    //printf("Process added to queue: PID=%d, Name=%s, Completion Time=%d, Wait Time=%d, Priority=%d\n", new_process.pid, new_process.name, new_process.completion_time, new_process.wait_time, new_process.priority);
}

void signal_processes() {
    for (int i = 0; i < NCPU; i++) {
        sem_wait(queue_lock);  // Lock the queue for safe dequeue
        //printf("Sem wait under signal_process\n");
        if (!isEmpty(ready_queue)) {
            //printf("Dequeue-ing a process\n");
            Process process = dequeue(ready_queue);
            pid_t pid = process.pid;
            sem_post(queue_lock);  // Release the lock after dequeueing
            //printf("Sem post after dequeue\n");
            if (pid > 0) {
                kill(pid, SIGCONT);  // Send SIGCONT to resume the process
            }
        } else {
            sem_post(queue_lock);  // Release the lock if queue is empty
            //printf("Sem post if queue is empty why signalling processes\n");
        }
    }
}

void scheduler_loop() {
    // Set up the SIGINT handler
    struct sigaction sa;
    sa.sa_handler = scheduler_handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    //printf("Scheduler loop started.\n");

    while (scheduler_running) {
        sleep(TSLICE); // Sleep for the time slice duration
        //printf("About to sem wait scheduler\n");
        //sem_wait(queue_lock);
        //printf("Scheduler sem waited\n");
        // Signal NCPU processes to run
        signal_processes();
        //printf("Signaling processes to run\n");

    }
    printf("Scheduler loop exited.\n");
}

