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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void show_file_mode(struct stat *buf)
{
    // show the file status
    int i;
    char buff[10] = "----------";
    switch(buf->st_mode & S_IFMT) {
        case S_IFIFO:
            buff[0] = 'f';
            break;
        case S_IFDIR:
            buff[0] = 'd';
            break;
        case S_IFSOCK:
            buff[0] = 's';
            break;
        case S_IFBLK:
            buff[0] = 'b';
            break;
        case S_IFLNK:
            buff[0] = 'l';
            break;
    }
    
    if(buf->st_mode & S_IRUSR ) buff[1] = 'r';
    if(buf->st_mode & S_IWUSR ) buff[2] = 'w';
    if(buf->st_mode & S_IXUSR ) buff[3] = 'x';
    if(buf->st_mode & S_IRGRP ) buff[4] = 'r';
    if(buf->st_mode & S_IWGRP ) buff[5] = 'w';
    if(buf->st_mode & S_IXGRP ) buff[6] = 'x';
    if(buf->st_mode & S_IROTH ) buff[7] = 'r';
    if(buf->st_mode & S_IWOTH ) buff[8] = 'w';
    if(buf->st_mode & S_IXOTH ) buff[9] = 'x';

    for(i = 0; i < 10; i++) {
        printf("%c", buff[i]);
    }
    printf(" ");
    return ;
}

void file_gid_uid(int uid, int gid)
{
    // show the uid and gid
    struct passwd *ptr;
    struct group *str;
    ptr = getpwuid(uid);
    str = getgrgid(gid);
    printf("\t%s\t%s", ptr->pw_name, str-> gr_name);
}

int compare(const void* lhs, const void* rhs)
{
    return strcmp(((struct dirent*)lhs)->d_name, ((struct dirent*)rhs)->d_name);
}

void ls(int count, char* argv[], int flag, int time) 
{
    struct stat buf;
    DIR* dirptr = NULL;
    struct dirent* entry = NULL;

    for (int i = 1; i < count; i++) {
        if (strcmp(argv[i], "-l") == 0) continue;

        // file not exist case
        if (access(argv[i], F_OK) == -1) {
            // perror("access");
            continue;
        }

        stat(argv[i], &buf);
        printf("%s:\n", argv[i]);
        if (S_ISDIR(buf.st_mode)) {
            dirptr = opendir(argv[i]);

            char savepoint[100];
            strcpy(savepoint, pwd());
            // cd(argv[i]);
            chdir(argv[i]);

            // show the file under the dir

            // sort test
            struct dirent en[100];
            int number = 0;
            while (entry = readdir(dirptr)) {
                en[number++] = *entry;
            }
            qsort(en, number, sizeof(en[0]), compare);
            
            for (int i =0; i < number; i++){
                entry = &en[i];
                stat(entry->d_name, &buf);

                if (flag == 0) {
                    if (entry->d_name[0] == '.') continue;
                    if (S_ISDIR(buf.st_mode)) printf(ANSI_COLOR_BLUE "  %s\n" ANSI_COLOR_RESET, entry->d_name);
                    else printf("  %s\n", entry->d_name);
                }
                else if (flag == 1) {
                    // mode
                    show_file_mode(&buf);

                    // file number
                    struct stat ss;
                    stat(entry->d_name, &ss);
                    if (S_ISDIR(ss.st_mode)) {
                        // this place may occur the error about the [Permission denyed]
                        DIR* dd = opendir(entry->d_name);
                        struct dirent* ee = NULL;
                        int count = 0;
                        while (ee = readdir(dd)) count ++;
                        printf("%d", count);
                    }
                    else printf("1");

                    // uid, gid
                    file_gid_uid(ss.st_uid, ss.st_gid);

                    // size
                    printf("\t%lld", ss.st_size);

                    // time
                    if (time == 0) printf("\t%.12s", 4 + ctime(&ss.st_mtime));
                    else if (time == 1) printf("\t%.12s", 4 + ctime(&ss.st_ctime));
                    else if (time == 2) printf("\t%.12s", 4 + ctime(&ss.st_atime));
 
                    // name
                    if (S_ISDIR(ss.st_mode)) {
                        printf(ANSI_COLOR_BLUE "\t%s\n" ANSI_COLOR_RESET, entry->d_name);
                    }
                    else {
                        printf("\t%s\n", entry->d_name);
                    }
                }
            }
            closedir(dirptr);
            chdir(savepoint);
        }
        else {
            if (flag == 0) {
                printf("  %s\n", argv[i]);
            }
            else if (flag == 1) {
                show_file_mode(&buf);
                printf("1\t");
                file_gid_uid(buf.st_uid, buf.st_gid);
                printf("\t%lld", buf.st_size);

                if (time == 0) printf("\t%.12s", 4 + ctime(&buf.st_mtime));
                else if (time == 1) printf("\t%.12s", 4 + ctime(&buf.st_ctime));
                else if (time == 2) printf("\t%.12s", 4 + ctime(&buf.st_atime));

                printf(ANSI_COLOR_GREEN "\t%s\n" ANSI_COLOR_RESET, argv[i]);
            }
        }
        printf("\n");

        // cd(savepoint);
    }
}

void mls(int argc, char* argv[], int flag, int time)
{
    /*
    char p[100];
    strcpy(p, argv[0]);
    if (argc == 1) {
        system("ls");
        return;
    }
    for (int i = 1; i < argc; i++) {
        strcat(p, " ");
        strcat(p, argv[i]);
    }
    system(p);
    */
    if (argc == 1) {
        char* data[2];
        data[1] = (char*) malloc (sizeof(char) * 100);
        strcpy(data[1], "./");
        ls(2, data, 0, time);
        free(data[1]);
    }
    else {
        if (argc == 2 && (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "-lc") == 0 || strcmp(argv[1], "-lu") == 0)) {
            argv[2] = (char*) malloc (sizeof(char) * 100);
            strcpy(argv[2], "./");
            ls(3, argv, 1, time);
            free(argv[2]);
        }
        else ls(argc, argv, flag, time);
    }
}

