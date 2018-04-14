#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main()
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
