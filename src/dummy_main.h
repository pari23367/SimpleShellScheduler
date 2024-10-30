#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void signal_handlr(int sig) {}

int dummy_main(int argc, char **argv);

int main(int argc, char **argv){
    signal(SIGCONT, signal_handlr);
    pause();
    int ret = dummy_main(argc, argv);
    return ret;
}

#define main dummy_main
