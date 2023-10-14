#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

int main() {
    int p[2];
    char buf[5];
    int n;
    pipe(p);
    if (fork() == 0) {
        n = read(p[0], buf, sizeof buf);
        if (n > 0) {
            printf("received %s\n", buf);
            write(p[1], "pong", 4);
        }
        close(p[0]);
        close(p[1]);
    } else {
        write(p[1], "ping", 4);
        n = read(p[0], buf, sizeof buf);
        if (n > 0) {
            printf("received %s\n", buf);
        }
        close(p[0]);
        close(p[1]);
    }
    exit(0);
}