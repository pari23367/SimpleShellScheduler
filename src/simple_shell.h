//Header file for our simple_shell.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#define MAX_ARGS 20
#define MAX_COMMAND_LENGTH 1024
#define MAX_HISTORY_SIZE 100
#define CMD_EXIT "exit"
#define CMD_HISTORY "history"
#define MAX_HISTORY 100
#define MAX_DETAILS_STORAGE 10000

int details_length = 0;
int status = 1 ;
int count = 0;

char command_details[MAX_DETAILS_STORAGE];
char* history[MAX_HISTORY];
void handle_sigint(int sig);
long get_time();
void shell_loop();
void handle_command(char* command);
char* read_user_input();
int launch(char*command,int is_background);
int create_process_and_run(char*command,int is_background);
// void execute_command_with_pipes(char* command);
// int spawn_proc(int in, int out, char* cmd);
void split_command_into_args(char* command , char* args[]);
// void split_command_by_pipes(char* command, char* commands[],int* command_counts);
void add_details(char* command, int pid , long start_time , long end_time);
void add_to_history(char* command);
void print_history();
void free_history();
void add_process(int pid);
