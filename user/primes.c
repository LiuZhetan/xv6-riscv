#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

int size = sizeof(int);

void __primes(int read_pipe, int initial, int max_process) {
    if (max_process == 0) {
        return;
    }
    int p[2];
    int x, buf, i;
    pipe(p);
    if (fork() == 0) {
        // child
        close(p[1]);
        __primes(p[0], 0, max_process-1);
        close(p[0]);
    }
    else {
        // parent
        close(p[0]);
        if (initial) {
            for (i = 2; i < 36; i++)
                write(p[1], &i, size);
        }
        else if(read(read_pipe, &x, size) && x != 0) {
            printf("prime %d\n",x);
            while (read(read_pipe, &buf, size)) {
                if (buf % x != 0)
                    write(p[1], &buf, size);
            }
        }
        close(p[1]);
        wait(0);
    }
}

int main() {
    if (fork() == 0) {
        __primes(0, 1, 20);
    }
    else {
        wait(0);
    }
    exit(0);
}