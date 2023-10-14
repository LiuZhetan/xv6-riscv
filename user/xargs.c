#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
/*
parse args of main, return 0 if empty args or 
line_limit(default 1) for "-n" option.

argc: argc passed for main function, it will change to the number of arguments of exec_args
argv: argv passed for main function
exec_args: the args for command to run
*/
int parse_args(int *argc, char *argv[], char *exec_args[]) {
    int line_limit = 1;
    if (*argc < 2) {
        fprintf(2,"Input your command\n");
        return 0;
    }
    int i, j = 0;
    int cut = 1;
    if (*argc >= 4 && 
        strcmp(argv[1], "-n") == 0 &&  
        (line_limit = atoi(argv[2])) != 0) {
        i = 3;
        cut = 3;
    }
    else {
        i = 1;
        cut = 1;
    }

    for (; i < *argc; i++, j++) {
            // cut "argv"
            exec_args[j] = argv[i];
        }

    // end with a pointer of address 0
    exec_args[j] = 0;
    *argc -= cut;
    return line_limit;
}

void run_cmd(char *argv[]) {
    if (fork()==0) {
        int res = exec(argv[0], argv);
        fprintf(2, "exec %s failed, return %d\n", argv[0], res);
    }
    else {
        wait(0);
    }
}

/*
main logic for xargs.

args: the args for command to run, get from int parse_args() exec_args
init_len: the number of arguments of args initially
argv_max_len: size of args
line_limit: return value from parse_args
*/
int xargs(char *argv[], int init_len, int argv_max_len, int line_limit) {
    int i = 0, n;
    char buf[1024], slot[2];                   // use slot to judge "\\n"
    // see buf as a table of string, 
    // line_count point to row and argc point to column
    // argc point to "\0" of argv originally
    int line_count = 0, argc;   
    argc = init_len;
    int arg_pointer = 0;

    if ((n = read(0, buf, sizeof(buf))) > 0) {
        // read data from stdin or pipe
        if (n > (sizeof buf) - 1) {
            fprintf(2,"args list is two long, at most %d bytes, buffer overflow!\n", (sizeof buf) - 1);
            return 0;
        }
        buf[n] = '\0';

        // deal with ""
        if (buf[0] == '"' && n - 2 > 0 && buf[n - 2] == '"') {
            // check for '"'
            i = 1;
            buf[n-2] = '\\';
            buf[n-1] = 'n';
            arg_pointer = 1;
        }
        else {
            buf[n-1] = '\\';
            buf[n] = 'n';
        }

        for (; i <= n; i++) {
            slot[0] = slot[1];
            slot[1] = buf[i];

            // deal with illigel "
            if (slot[1] == '"' && slot[0] != '\\') {
                fprintf(2,"Illegal char '\"' at %d!\n", i);
                return 0;
            }

            if (slot[1] == ' ') {
                if (slot[0] != ' ') {
                    if (argc == argv_max_len) {
                        fprintf(2,"arguments are two much, at most %d arguments for every %d line!\n", 
                            argv_max_len, line_limit);
                        return 0;
                    }
                    argv[argc] = &buf[arg_pointer];
                    argc++;
                }
                buf[i] = 0;                     // read blank，replace with '\0'
                arg_pointer = i + 1;            // update pointer
            }

            if (strcmp(slot,"\\n") == 0) {
                // read a "\n"
                line_count += 1;
                buf[i] = 0, buf[i-1]=0;         // replace "\n" with "\0\0"
                argv[argc] = &buf[arg_pointer];
                argc++;
                if (line_count % line_limit == 0) {
                    argv[argc] = 0;             // fill 0 for argv
                    run_cmd(argv);              // run command
                    argc = init_len;            // reset argc
                }
                arg_pointer = i + 1;            // update pointer
            }
        }
    }
    if (argc > init_len) {
        argv[argc] = 0;
        run_cmd(argv);
    }
    return 1;
}

int main(int argc, char *argv[]) {
    char* exec_args[32];
    int line_limit = 1;

    if ((line_limit = parse_args(&argc,argv,exec_args)) == 0) 
        exit(0);

    xargs(exec_args, argc, sizeof exec_args / sizeof exec_args[0], line_limit);

    exit(0);
}