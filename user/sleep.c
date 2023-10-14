#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

static char help[] = "Try 'sleep --help' for more information.\n";

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("sleep: missing operand\n");
        printf("%s",&help);
        exit(0);
    }
    int n = atoi(argv[1]);
    if (strcmp(argv[1], "--help") == 0) {
        printf("Usage: sleep NUMBER\n");
        printf("Pause for NUMBER*0.1 seconds.\n");
    }
    else if (n != 0) {
        printf("(nothing happens for a little while)\n");
        sleep(n);
    }
    else 
    {
        printf("sleep: invalid time interval %s\n");
        printf("%s",&help);
    }
    
    exit(0);
}
