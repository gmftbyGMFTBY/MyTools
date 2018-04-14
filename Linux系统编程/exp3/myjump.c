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

#define MAX_LENGTH 200

// this define just for test, need to get this string from the main.c

/*
   this part of the file try to take over the problem of the autojump 
   and give more convience for the users to swim in the sea of the directory.

   The path in the file .autojump must be the absolute pathname
 */

char filename[MAX_LENGTH];
char FILE_NAME[200];

int get_absolute_path(char* path, char save[])
{
    // use pipeline to get the result of the [ cd path; pwd; ] to get the absolute path
    FILE* fp = NULL;
    char cmd[MAX_LENGTH + 10];
    sprintf(cmd, "cd %s; pwd;", path);

    if ((fp = popen(cmd, "r")) == NULL) {
        perror("popen");
        return -1;
    }

    // get the result of the command-pwd
    fgets(save, 200, fp);

    // fix the \n in the path
    save[strlen(save) - 1] = '\0';

    // close the pipeline
    pclose(fp);

    return 0;
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
            // add and check weight
            weight += 1;
            weight = 100000 > weight ? weight : 99999;
            fseek(fp, -strlen(data), SEEK_CUR);
            fprintf(fp, "%-200s %-5d\n", path, weight);
            fflush(fp);
            flag = 1;
            break;
        }
    }

    if (!flag) {
        // do not exist the file, append into the file
        fprintf(fp, "%-200s %-5d\n", argv, 1);
        fflush(fp);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

void change2small(char* path)
{
    // chaneg the big character into small character
    for (int i = 0; i < strlen(path); i ++) {
        if (path[i] >= 65 && path[i] <= 90) {
            path[i] += 32;
        }
    }
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

        /* check whether argv in the path, save the weight, else continue
           consider about the big character and small character
         */
        change2small(data);
        change2small(argv);

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

int myjump(int rw, char* work_path, char* argv)
{
    /*
       read-model(0) : read the file to select the best chosen to jump
       write_model(1): renew the weight and write into the file after `cd` command
     */
    
    // check the file existing
    strcpy(FILE_NAME, work_path);
    strcat(FILE_NAME, ".autojump");
    
    if (access(FILE_NAME, F_OK) == -1) {
        printf("\x1b[31mFile .autojump do not exist in work_path, create!\x1b[0m\n");
        creat(FILE_NAME, 0777);
    }

    if ( !rw ) {
        // read model to jump - argv[1] save the part of the path to find 
        char choose[MAX_LENGTH];
        int res = read_file(argv, choose);

        if (res == -1) {
            // the query string is not in the file
            printf("\x1b[31mCan not find the record about %s !\x1b[0m\n", argv);
            return -1;
        }
        else {
            // find the string in the file, and choose the biggest weight path
            if (choose == NULL) {
                printf("\x1b[31mError!\x1b[0m\n");
                return -1;
            }
            printf("\x1b[34mReady jump to %s !\x1b[0m\n", choose);
            chdir(choose);

            // add the weight 
            write_file(choose);
        }
    }
    else {
        // write model - argv[1] save the path
        // write the absolute path
        char save[MAX_LENGTH];
        if (get_absolute_path(argv, save) == -1) {
            return -1;
        }
        else write_file(save);
    }

    // pwd
    // printf("WORK_PATH: %s\n", getcwd(NULL, 0));
    return 0;
}
