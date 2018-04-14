#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

void purge()
{
    char data[200];
    FILE* fp = fopen("lantian", "r+");
    
    while (fgets(data, sizeof(data), fp)) {
        int weight;
        char path[200];
        sscanf(data, "%s %d", path, &weight);

        if (strcmp(path, "/home/lantian/File/NIHAO") != 0) {
            fseek(fp, -strlen(data), SEEK_CUR);
            fprintf(fp, " ", path, weight);
            break;
        }
    }
}

int create()
{
    char data[200];
    FILE* fp = fopen("lantian", "w");
    char name[5][200] = {"~/File/Study",
                         "~/Downloads",
                         "~/Desktop",
                         "~/File/NIHAO",
                         "~/admir_repo"};
    FILE* pp = NULL;
    int  weight[5] = {101, 4, 5, 7, 10};
    for (int i = 0; i < 5; i++) {
        char apath[200];
        char tt[200];
        sprintf(tt, "cd %s; pwd;", name[i]);
        pp = popen(tt, "r");
        fgets(apath, sizeof(apath), pp);
        apath[strlen(apath) - 1] = '\0';
        fprintf(fp, "%-200s %-5d\n", apath, weight[i]);
        pclose(pp);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    purge();
    // create();
    return 0;
}
