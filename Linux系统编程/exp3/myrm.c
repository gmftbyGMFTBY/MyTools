#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <utime.h>

int delete_file(char* filename, int flag)
{
    // remember to chdir before unlink the file
    struct stat buf;
    int res = stat(filename, &buf);

    if (flag == 0 || flag == 2) {
        printf("Do you want to rerun ( rm %s ) and permit these operations? [y/N] ", filename);
        char res[10];
        gets(res);
        if (res[0] != 'y') return;
    }

    if (!S_ISDIR(buf.st_mode)) {
        unlink(filename);
        return;
    }

    else {
        if (flag == 0 || flag == 1) {
            printf("Can not delete the dir by rm, need -r paramter\n");
            return -1;
        }
        // dir file
        DIR* dirptr = NULL;
        struct dirent* entry = NULL;
        dirptr = opendir(filename);
        
    
        while (entry = readdir(dirptr)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // change dir
            chdir(filename);
            delete_file(entry->d_name, flag);
            chdir("..");
        }

        // delete the dir file at last
        rmdir(filename);
        return 0;
    }
}

int mrm(int argc, char* argv[], int flag)
{
    /* flag - 0: ask for user normally
       flag - 1: do not ask for user normally
       flag - 2: ask for user and recursive
       flag - 3: do not ask for user and recursive
     */
    int detectp = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "-rf") == 0) continue;
        if (access(argv[i], F_OK) != -1) {
            delete_file(argv[i], flag);
        }
        else {
            detectp = -1;
            printf("rm: cannot remove '%s': No such file or directory\n", argv[i]);
        }
    }
    if (detectp == -1) return -1;
    else return 0;
}

