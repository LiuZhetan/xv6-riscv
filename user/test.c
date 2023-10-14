#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

int main() {
    if (fork()==0) {
        // 此代码无法执行echo
        char* argv[3] = {"echo", "hello", 0};

        printf("begin exec echo:\n");
        int res = exec(argv[0], argv);
        printf("exec %s failed, return %d\n", argv[0], res);
        

        // 此代码可以执行
        /*char* argv[] = {"echo", "hello"};
        printf("begin exec echo:\n");
        int res = exec(argv[0], argv);
        printf("exec %s failed\n", argv[0]);
        printf("return %d\n", res);*/
    }
    else {
        wait(0);
    }
    
    exit(0);
}