#include "simple_shell.h"

void handle_sigint(int sig){
    printf("\nCaught Signle Ctrl+C\n");  //if pressed ctrl +c , process will terminate and this msg will be shown
    printf("%s",command_details);  
    status = 0; // status was 1 , now it is 0 
    
}

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
        exit(EXIT_FAILURE);
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

int create_process_and_run(char* command, int is_background) {
    int status = fork(); // Creating a child process to run the command


    // Fork() returns a negative value if something goes wrong
    if (status < 0) {
        printf("Child process couldn't be formed correctly\n");
        exit(0);
    } 
    
    // Fork() returns 0 for child process
    else if (status == 0) {
    // Handle commands with pipes, if there is any '|' in the command
        if (strchr(command, '|')) {
            execute_command_with_pipes(command);
        }
        char* args[MAX_ARGS]; // Defining max size of args array
        
        // Parse the command into arguments array
        split_command_into_args(command, args);
        
        execvp(args[0], args); // Execute the command
        printf("Should not reach here");
        exit(EXIT_FAILURE);
    } 
    
    else { 
        // Parent process
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


int spawn_proc(int in, int out, char* cmd) {
    int pid;
    char* args[MAX_ARGS];
    split_command_into_args(cmd, args);

    if ((pid = fork()) == 0) {
        if (in != 0) {
            dup2(in, 0); // for all other processes we are redirecting std in
            close(in);
        }
        if (out != 1) {
            dup2(out, 1); // stdout redirect
            close(out);
        }
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void execute_command_with_pipes(char* command) {
    int fd[2];
    int command_count = 0;
    char* commands[MAX_ARGS];

    split_command_by_pipes(command, commands, &command_count);

    int in_fd = 0; // Input file descriptor for the first command

    for (int i = 0; i < command_count - 1; i++) {
        pipe(fd);
        spawn_proc(in_fd, fd[1], commands[i]);
        close(fd[1]); // Close the write end of the pipe in the parent
        in_fd = fd[0]; // Save the read end for the next command
    }

    // Handle the last command
    if (in_fd != 0) {
        dup2(in_fd, 0); // Redirect stdin from the last pipe
        close(in_fd);
    }
    char* last_args[MAX_ARGS];
    split_command_into_args(commands[command_count - 1], last_args);
    execvp(last_args[0], last_args);
    perror("execvp failed");
    exit(EXIT_FAILURE);
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


void split_command_by_pipes(char* command, char* commands[], int* command_count) {
    char* token = strtok(command, "|");
    *command_count = 0;
    
    while (token != NULL) {
        commands[(*command_count)++] = token;
        token = strtok(NULL, "|");
    }
    commands[*command_count] = NULL; // NULL terminate the commands array
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

int main() {
    //Initialising the details array
    command_details[0] = '\0';

    //Signal handling for Ctrl + C
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);


    shell_loop();
    free_history(); // Free history at the end
    return 0;
}
