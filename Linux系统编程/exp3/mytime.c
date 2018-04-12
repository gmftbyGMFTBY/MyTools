#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

/*
   mine time command, calculate the time cost of the shell command
   of the executor program.
 */

int mtime(int argc, char* argv[])
{
    pid_t pid;

    if((pid = fork()) < 0) {
        printf("fork failed.\n");
    }
    else if( pid == 0 ) {
        // start subprocess
        argv[argc] = NULL;
        execvp(argv[1], argv + 1);
    }
    else {
        // super process count the time 
        struct timeval t1;
        gettimeofday(&t1);
        wait(0);
        struct timeval t2;
        gettimeofday(&t2);
        fprintf(stdout, "\x1b[31m%d (s), %d (us)\x1b[0m\n", t2.tv_sec - t1.tv_sec, t2.tv_usec - t1.tv_usec);
    }
    return 0;
}
