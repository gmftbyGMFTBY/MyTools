#include "main.h"

void touch(int argc, char* argv[], int acc, int modifaction)
{
    // touch command try to change the file's timestamps, use current timestamps
    // If the file do not exist, try to create one
    // touch can affect the file or the dir, and the system call is [utime]
    struct stat buf;
    struct utimbuf timebuf;
    DIR* dir = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "-m") == 0) continue;
        if (access(argv[i], F_OK) == -1) {
            // file do not exist, create one
            creat(argv[i], 0777);
            continue;
        }
        else {
            // file exist, change the timestamps - [access time, modifacation time]
            if (stat(argv[i], &buf)) {
                perror("stat");
            }

            int fd;
            if (acc) {
                // default is access status
                if (!S_ISDIR(buf.st_mode)) {
                    if ((fd = open(argv[i], O_RDONLY | O_TRUNC)) < 0) {
                        perror("open");
                        continue;
                    }
                    close(fd);
                }
                else {
                    if ((dir = opendir(argv[i])) == NULL) {
                        perror("opendir");
                        continue;
                    }
                    closedir(dir);
                }
            }
            else if (modifaction || (!acc && !modifaction)){
                if (!S_ISDIR(buf.st_mode)) {
                    if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
                        perror("open");
                        continue;
                    }
                    close(fd);
                }
                else {
                    if ((dir = opendir(argv[i])) == NULL) {
                        perror("opendir");
                        continue;
                    }
                    closedir(dir);
                }
            }
            
            timebuf.actime  = buf.st_atime;
            timebuf.modtime = buf.st_mtime;

            if (utime(argv[i], &timebuf) < 0) {
                perror("utime");
                continue;
            }
        }
    }
}

