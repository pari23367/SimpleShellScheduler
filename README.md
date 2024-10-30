# OS-Scheduler 

## OVERVIEW
The OS-Scheduler and Simple Shell project implements a priority-based process scheduling system with a simple command-line shell. This shell accepts commands from the user, queues them with a specified priority, and schedules them for execution based on time slices. The program is written in C and utilizes POSIX system calls like `fork()`, `execvp()`, and `waitpid()`, along with shared memory and semaphores for inter-process communication.

---

## PROJECT STRUCTURE

```
├── bin
│   ├── Makefile
│   ├── fib.c
│   ├── helloworld.c
│   ├── file.txt
├── src
│   ├── simple_shell.c
│   ├── simple_shell.h
│   ├── queue.h
│   ├── queue.c
│   ├── simple_scheduler.c
│   ├── simple_scheduler.h
│   ├── Makefile
├── DESIGN.md
├── Makefile
└── README.md

```

---

## INSTALLATION

To build and set up the project:

1. **Clone the repository**:

   ```bash
   git clone git@github.com:YourUsername/OS-Scheduler-SimpleShell.git
   ```

2. **Install Necessary Software**: Ensure you have a Unix-based OS with POSIX support, a C compiler (e.g., `gcc`), and `make`. On Windows, use WSL with a Linux distribution (e.g., Ubuntu).

3. **Compile the Project**:
   - Navigate to the `src` directory and run:

     ```bash
     make
     ```

   - Compile the test programs (`fib.c` and `helloworld.c`) by running:

     ```bash
     cd bin
     make
     ```

---

## USAGE

### Running the Shell and Scheduler

1. **Start the Simple Shell**: Run the shell with the desired number of CPUs (`NCPU`) and a time slice in milliseconds (`TSLICE`):

   ```bash
   ./src/simple_shell <NCPU> <TSLICE>
   ```

   For example:

   ```bash
   ./src/simple_shell 2 500
   ```

   This runs the shell with 2 CPUs and a 500 ms time slice.

2. **Submit Commands**: In the shell prompt, submit commands with a priority (1 is the highest, 4 is the lowest):

   ```bash
   submit <command> <priority>
   ```

   Example:

   ```bash
   submit ./bin/helloworld 2
   ```

3. **Background Commands**: To run a command in the background, add `&`:

   ```bash
   submit ./bin/fib 40 &
   ```

4. **Shell Commands**:
   - **Exit**: Type `exit` to terminate the shell.
   - **History**: Type `history` to view previous commands with their timestamps.

---

## SAFETY MECHANISM: Dummy Main

A `dummy main` function is used as a safety mechanism to ensure child processes do not accidentally start running before the scheduler is ready. This setup:
- **Controlled Start**: Each process is paused immediately upon creation and only resumes when the scheduler signals it.
- **Safe Scheduling**: Prevents premature execution, ensuring reliable command handling by the scheduler.

---

## REFERENCES

1. **POSIX System Calls**: `man` pages for `fork`, `execvp`, `waitpid`, `sem_open`, `mmap`, and other relevant POSIX functions.
2. **Lecture Notes**: Lecture slides and notes on system calls, shell programming, and scheduling .

---

## CONTRIBUTORS
- **Manaswi Singh** (2023307)
- **Paridhi Kotarya** (2023367)
