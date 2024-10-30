OS-Scheduler 
OVERVIEW
The OS-Scheduler and Simple Shell project is designed to create a multi-process environment where commands can be prioritized, queued, and scheduled for execution based on their assigned priority. The shell waits for user commands, assigns them priorities, and executes them using a time-sliced scheduler. Written in C, this project utilizes POSIX system calls including fork(), exec(), waitpid(), shared memory, and semaphores for inter-process communication.
