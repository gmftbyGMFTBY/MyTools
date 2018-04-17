#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    char p[10];
    memset(p, '\0', sizeof(p));

    int w = 0;
    while (1) {
        p[w] = getchar();
        if (p[w - 2] == 0x1b && p[2 - 1] == '[' && p[w] == 'A') {
            printf("get the upper arrow!\n");
            exit(0);
        }
    }
    return 0;
}
