#include "simple_shell.h"
#include "queue.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

Queue * ready_queue;
int NCPU;
int TSLICE;
sem_t *queue_lock;

void handle_sigint(int sig){
    printf("\nCaught Signle Ctrl+C\n");  //if pressed ctrl +c , process will terminate and this msg will be shown
    printf("%s",command_details);  
    status = 0; // status was 1 , now it is 0 
    
}

void signal_handlr(int sig){}

long get_time() {
    struct timeval current_time;
    if (gettimeofday(&current_time, NULL) != 0) {  // func to get exact time of day 
        printf("Error in getting time.\n");  //error msg shoown 
        exit(1);
    }
    long time_milli = (current_time.tv_sec * 1000) + (current_time.tv_usec / 1000);   //time in micro+milli
    return time_milli;   //returned in milli
}

void shell_loop() {
    //ready_queue = create_shared_queue(); 
    while(status) {  // while the status is one it will loop infinitely
        // Printing the shell prompt
        printf("iiitd@possum:~$ ");
        
        //Recoring start time of each command
        long start_time = get_time();
        
        // Reading user input
        char* command = read_user_input();
    
        // Handle the command
        handle_command(command); 

        //Recording End time of each command
        long end_time = get_time();
        
        //Store the details into the global array details
        add_details(command ,getpid(),start_time,end_time);  // adding details of every command inthe array

        free(command); // De-allocating memory after use

    }
}

char* read_user_input() {
    char* input = malloc(MAX_COMMAND_LENGTH);//dynamic allocation 
    
    // Error handling
    if (!input) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {  //taking input frm user 
        perror("Error reading input");
    }

    // Remove newline character
    input[strcspn(input, "\n")] = '\0';
    return input;
}

void handle_command(char* command) {
    
    //For support backgrounding of commands with &
    int is_background = 0;

    // Check if command ends with '&'
    int len = strlen(command);
    if (len > 0 && command[len - 1] == '&') {
        is_background = 1; // Set background flag
        command[len - 1] = '\0'; // Remove '&' from command
    }

    // if input is the exit command, then we break out of our infinite loop
    if (strcmp(command, CMD_EXIT) == 0) {
        free(command);
        exit(0);
    }

    // If input is the history command, then we print the history that we have defined of size max 100
    if (strcmp(command, CMD_HISTORY) == 0) {
        print_history();
    }
    
    // Otherwise, it's a normal command
    else {
        add_to_history(command);
        status = launch(command,is_background);
    }
}

int launch(char* command, int is_background) { //launches commmand and returns status
    int status;
    status = create_process_and_run(command, is_background);
    return status;
}
// Function to extract and validate priority value
int get_priority(char* priority_arg) {
    int priority = 1; // Default priority

    if (priority_arg != NULL) {
        priority = atoi(priority_arg); // Convert priority to integer
        
        // Validate priority (should be between 1 and 4)
        if (priority < 1 || priority > 4) {
            fprintf(stderr, "Invalid priority value. Must be between 1 and 4.\n");
            exit(EXIT_FAILURE);
        }
    }

    return priority;
}

int create_process_and_run(char* command, int is_background) {
    int status = fork(); // Creating a child process to run the command
    //int priority = 1;
    if (strchr(command, '|')) {
        printf("Cannot execute command with pipes\n");
        exit(EXIT_FAILURE);
    }

    if (strncmp(command, "submit", 6) != 0) {
        printf("Commands must start with 'submit'\n");
        exit(EXIT_FAILURE);
    }
    char* args[MAX_ARGS];
    split_command_into_args(command, args);
    printf("checkpoint 1\n");
    if (args[1] == NULL) {
        fprintf(stderr, "Error: No executable specified after 'submit'.\n");
        exit(EXIT_FAILURE);
    }

    int priority = get_priority(args[2]);
    printf("Checkpoint3\n");
    Process new_process;
    new_process.pid = status; // Save the child's PID
    snprintf(new_process.name, sizeof(new_process.name), "%s", args[1]); // Command name
    //printf(new_process.name);
    new_process.completion_time = 0; // Initialize as needed
    new_process.wait_time = 0; // Initialize as needed
    new_process.priority = priority; // Your logic to get priority
    printf("Process created\n");
    // Assuming ready_queue is declared and accessible
    add_process(ready_queue, new_process);
    // Parent process
    printf("Checkpint after adding process");
        
    // Fork() returns a negative value if something goes wrong
    if (status < 0) {
        printf("Child process couldn't be formed correctly\n");
        exit(0);
    }

    // Fork() returns 0 for child process
    else if (status == 0) {
        signal(SIGCONT,signal_handlr);
        printf("checkpoint2\n");
        //Do something like this calls scheduler's add method to queue, and at T = 0 scheduler runs all proccesses it has in the queue ?? acc to priority
        pause(); // Wait for a signal from the scheduler
        printf("Checkpoint after pause\n");
        // Execute the specified program without additional arguments
        execlp(args[1], args[1], NULL); // Pass only the executable

        // If exec fails, print error and exit
        fprintf(stderr, "Execution failed for command: %s\n", args[1]);
        exit(EXIT_FAILURE);
    } else {
        if (!is_background) {
            // If it's not a background process, wait for the child
            waitpid(status, NULL, 0); // Wait for the child process to complete
        } else {
            // For background processes, don't wait
            printf("Process running in background with PID: %d\n", status);
        }
    }
    return 1; // Needs to return 1 to keep the shell running
}

void split_command_into_args(char* command, char* args[]) {
    char* token = strtok(command, " ");
    int i = 0;
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // NULL terminate the args array
}


void add_details(char* command , int pid, long start_time, long end_time) {
    char buffer[256];
    long elapsed_time = (end_time - start_time) / 1000; // Convert milliseconds to seconds
    time_t rawtime = start_time / 1000; // Convert start time to seconds
    struct tm* timeinfo = localtime(&rawtime);
    
    //Formatting the detail string
    snprintf(buffer, sizeof(buffer),"Command: %s,\nProcess ID: %d,\n Start time: %s,\n End time: %s,\n Elapsed time: %ld ms\n\n",command,pid,asctime(timeinfo),asctime(timeinfo), elapsed_time);

    // Append to global command details string
    if (details_length + strlen(buffer) < MAX_DETAILS_STORAGE) { //adds only if less than max_details_storage
        strcat(command_details, buffer);
        details_length += strlen(buffer);
    }
}

// History-related functions
void add_to_history(char* command) {//adding commands to hostry if not full
    if (count < MAX_HISTORY) {
        history[count] = strdup(command);
        count++;
    } else {
        printf("History is full.\n");
    }
}

void print_history() {//printing commands from list
    for (int i = 0; i < count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

void free_history() { //deallocating mem
    for (int i = 0; i < count; i++) {
        free(history[i]);
    }
}

int main(int argc, char *argv[]) {
    // Check for the correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NCPU> <TSLICE (ms)>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse and validate NCPU
    NCPU = atoi(argv[1]);
    if (NCPU <= 0) {
        fprintf(stderr, "Error: NCPU must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    // Parse and validate TSLICE
    TSLICE = atoi(argv[2]);
    if (TSLICE <= 0) {
        fprintf(stderr, "Error: TSLICE must be a positive integer in milliseconds.\n");
        return EXIT_FAILURE;
    }

    // Initialize the command details array
    command_details[0] = '\0';

    // Signal handling for Ctrl + C
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    
    ready_queue = create_shared_queue(&queue_lock);

    // RUNNING THE SCHEDULER
    int scheduler_pid = fork();
    if (scheduler_pid == 0) {
        initialize_scheduler(NCPU,TSLICE);// Child process for the scheduler
        scheduler_loop();  // This function will run independently
        exit(0);           // Ensure it exits once done
    } else if (scheduler_pid < 0) {
        fprintf(stderr, "Failed to fork scheduler process.\n");
        return EXIT_FAILURE;
    }
    
    printf("Reached here\n");
    // Start the main shell loop
    shell_loop();

    // Free history at the end
    free_history();
    return 0;
}
