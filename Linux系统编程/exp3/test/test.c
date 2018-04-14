#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    char data[200];
    FILE* fp = fopen("lantian", "w");
    char name[5][200] = {"~/File/Study",
                         "~/Download",
                         "~/Desktop",
                         "~/File/NIHAO",
                         "/home/lantian/admir_repo"};
    int  weight[5] = {101, 4, 5, 7, 10};
    for (int i = 0; i < 5; i++) {
        fprintf(fp, "%-200s %-5d\n", name[i], weight[i]);
    }
    return 0;
}
