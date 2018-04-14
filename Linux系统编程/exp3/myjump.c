#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <utime.h>
#include <grp.h>

// this define just for test, need to get this string from the main.c
#define FILE_NAME  "/home/lantian/File/MyTools/Linux系统编程/exp3/.autojump"
#define MAX_LENGTH 200

/*
   this part of the file try to take over the problem of the autojump 
   and give more convience for the users to swim in the sea of the directory.
 */

char filename[MAX_LENGTH];

void innercd(char* path)
{
    chdir(path);
    perror("chdir");
}

int write_file(char* argv)
{
    // the weight bound is 100000
    char data[MAX_LENGTH + 10];
    FILE* fp = fopen(FILE_NAME, "r+");

    int flag = 0;       // flag to record whether find the path in the file

    while (fgets(data, sizeof(data), fp)) {
        int  weight;
        char path[MAX_LENGTH];
        sscanf(data, "%s %d", path, &weight);

        if (strcmp(path, argv) == 0) {
            weight = 100000 > weight ? weight : 99999;
            fseek(fp, -strlen(data), SEEK_CUR);
            fprintf(fp, "%-200s %-5d", path, weight);
            flag = 1;
            break;
        }
    }

    if (!flag) {
        // do not exist the file, append into the file
        fprintf(fp, "%-200s %-5d", argv, 1);
    }

    fclose(fp);
    return 0;
}

int read_file(char* argv, char choose[])
{
    // read the file .autojump and choose the biggest weight path to return 
    char data[MAX_LENGTH + 10];
    FILE* fp = fopen(FILE_NAME, "r+");
    int big_weight = 0;
    int flag = 0;
    
    while (fgets(data, sizeof(data), fp)) {
        int weight;
        char path[MAX_LENGTH];
        sscanf(data, "%s %d", path, &weight);

        // check whether argv in the path, save the weight, else continue
        if (strstr(data, argv) != NULL && weight > big_weight) {
            // the argv query string is in the path string, one of the choosen string
            big_weight = weight;
            flag = 1;
            strcpy(choose, path);
        }
        else continue;
    }

    fclose(fp);
    if (!flag) {
        // the query string is not in the file, try to append
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    /*
       read-model(0) : read the file to select the best chosen to jump
       write_model(1): renew the weight and write into the file after `cd` command
     */
    int rw = 0;

    // check the file existing
    if (access(FILE_NAME, F_OK) == -1) {
        printf("\x1b[31mFile .autojump do not exist in work_path, create!\x1b[0m\n");
        creat(FILE_NAME, 0777);
    }

    if ( !rw ) {
        // read model to jump - argv[1] save the part of the path to find 
        char choose[MAX_LENGTH];
        int res = read_file(argv[1], choose);

        if (res == -1) {
            // the query string is not in the file
            write_file(argv[1]);
            printf("\x1b[31mCn not find the record %s !\x1b[0m\n", argv[1]);
            exit(1);
        }
        else {
            // find the string in the file, and choose the biggest weight path
            if (choose == NULL) {
                printf("\x1b[31mError!\x1b[0m\n");
                exit(1);
            }
            printf("\x1b[32mReady jump to %s !\x1b[0m\n", choose);
            innercd(choose);
        }
    }
    else {
        // write model - argv[1] save the path
        write_file(argv[1]);
    }

    // pwd
    printf("WORK_PATH: %s\n", getcwd(NULL, 0));
    return 0;
}
