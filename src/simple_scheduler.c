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
int scheduler_running = 1;

// Signal handler for Ctrl+C
void scheduler_handle_sigint(int sig) {
    printf("\nScheduler terminating due to Ctrl+C\n");
    scheduler_running = 0;
}

// Initialize scheduler settings
void initialize_scheduler(int ncpu, int tslice) {
    NCPU = ncpu;
    TSLICE = tslice;
    //ready_queue = malloc(sizeof(Queue));
    //if (ready_queue == NULL) {
      //  perror("Failed to allocate memory for ready_queue");
        //exit(EXIT_FAILURE);
    //}
    //ready_queue = create_shared_queue();
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
    //enqueue(ready_queue,new_process);
    // Print debug information
    sem_wait(queue_lock);  // Lock the queue
    enqueue(ready_queue, new_process);
    sem_post(queue_lock);  // Release the lock
    printf("Process added to queue: PID=%d, Name=%s, Completion Time=%d, Wait Time=%d, Priority=%d\n", new_process.pid, new_process.name, new_process.completion_time, new_process.wait_time, new_process.priority);
}

void signal_processes() {
    for (int i = 0; i < NCPU; i++) {
        sem_wait(queue_lock);  // Lock the queue for safe dequeue
        if (!isEmpty(ready_queue)) {
            printf("Dequeue-ing a process\n");
            Process process = dequeue(ready_queue);
            pid_t pid = process.pid;
            sem_post(queue_lock);  // Release the lock after dequeueing
            if (pid > 0) {
                kill(pid, SIGCONT);  // Send SIGCONT to resume the process
            }
        } else {
            sem_post(queue_lock);  // Release the lock if queue is empty
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

    printf("Scheduler loop started.\n");

    while (scheduler_running) {
        sleep(TSLICE); // Sleep for the time slice duration

        // Signal NCPU processes to run
        signal_processes();
        printf("Signaling processes to run\n");

        // After TSLICE, send SIGSTOP to running processes
        for (int i = 0; i < NCPU; i++) {
            sem_wait(queue_lock);  // Lock the queue
            if (!isEmpty(ready_queue)) {
                Process process = dequeue(ready_queue);
                pid_t pid = process.pid;
                sem_post(queue_lock);  // Unlock the queue after dequeueing
                if (pid > 0) {
                    kill(pid, SIGSTOP);  // Stop the process after its time slice
                    sem_wait(queue_lock);  // Lock the queue to re-enqueue
                    enqueue(ready_queue, process);
                    sem_post(queue_lock);  // Unlock the queue after enqueueing
                }
            } else {
                sem_post(queue_lock);  // Release the lock if queue is empty
            }
        }
    }
    printf("Scheduler loop exited.\n");
}

