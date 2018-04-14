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

#define MAX_LENGTH 200
#define WORK_PATH "/home/lantian/File/MyTools/Linux系统编程/exp3/"

// define the time struct
struct utimebuf {
    time_t actime;
    time_t modtime;
};

char* pwd()
{
    return getcwd(NULL, 0);
}

void cd(char* argv)
{
    // myjump write-model
    char pause[MAX_LENGTH];
    strcpy(pause, argv);
    myjump(1, WORK_PATH, pause);

    if (chdir(argv) == -1) {
        perror("chdir");
        return ;
    }
}

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
            if (acc || (!acc && !modifaction)) {
                // default is access status
                if (!S_ISDIR(buf.st_mode)) {
                    if ((fd = open(argv[i], O_RDONLY | O_TRUNC)) < 0) {
                        perror("open");
                        continue;
                    }
                }
                else {
                    if ((dir = opendir(argv[i])) < 0) {
                        perror("opendir");
                        continue;
                    }
                    closedir(dir);
                }
            }
            else if (modifaction){
                if (!S_ISDIR(buf.st_mode)) {
                    if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
                        perror("open");
                        continue;
                    }
                }
                else {
                    if ((dir = opendir(argv[i])) < 0) {
                        perror("opendir");
                        continue;
                    }
                    closedir(dir);
                }
            }
            
            close(fd);
            timebuf.actime  = buf.st_atime;
            timebuf.modtime = buf.st_mtime;

            if (utime(argv[i], &timebuf) < 0) {
                perror("utime");
                continue;
            }
        }
    }
}

void cat(int argc, char* argv[])
{
    // cat function try to write the content into the stdout
    char data[MAX_LENGTH];
    for (int i = 1; i < argc; i++) {
        printf(ANSI_COLOR_RED "%s:\n" ANSI_COLOR_RESET, argv[i]);

        if (access(argv[i], F_OK) == -1) {
            // file do not exist
            perror("access");
            continue;
        }

        FILE* fp = fopen(argv[i], "r");
        while (fgets(data, sizeof(data), fp)) {
            fputs(data, stdout);
        }
        fputs("\n", stdout);
    }
}

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
                    printf("  %s\n", entry->d_name);
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

void delete_file(char* filename, int flag)
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
            return ;
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
        return ;
    }
}

void mrm(int argc, char* argv[], int flag)
{
    /* flag - 0: ask for user normally
       flag - 1: do not ask for user normally
       flag - 2: ask for user and recursive
       flag - 3: do not ask for user and recursive
     */ 
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "-rf") == 0) continue;
        if (access(argv[i], F_OK) != -1) {
            delete_file(argv[i], flag);
        }
        else {
            printf("rm: cannot remove '%s': No such file or directory\n", argv[i]);
        }
    }
}

void mmkdir(int argc, char* argv[], int flag)
{
    if (flag == 0) {
        for (int i = 1; i < argc; i++) {
            if (mkdir(argv[i], 0777) == -1) {
                // the execute permission for the dia should added, else false to create
                // the file or dir, even change into this dirname will be wrong!
                perror("mkdir");
                printf("create %s dir error!\n", argv[i]);
            }
            else {
                printf("create %s dir successfully!\n", argv[i]);
            }
        }
    }
    else {
        // -p model
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0) continue;
            else {
                // recursive mkdir the dir
                // append the character '/' as the end character
                argv[i][strlen(argv[i])] = '/';
                argv[i][strlen(argv[i])] = '\0';

                char* dirname[2];
                dirname[1] = (char*)malloc(sizeof(char) * 100);
                int point = 0;
                for (int j = 0; j < strlen(argv[i]); j ++) {
                    if (j == 0 && argv[i][j] == '/') continue;
                    else if (j == 0 && argv[i][j] != '/') dirname[1][point++] = argv[i][j];
                    else if (j != 0 && argv[i][j] == '/') {
                        // e.g. /home/ has been deceted 
                        dirname[1][point++] = argv[i][j];
                        dirname[1][point]   = '\0';
                        printf("%s\n", dirname[1]);
                        if (access(dirname[1], F_OK) != -1) {
                            // this dir exist
                            continue;
                        }
                        else {
                            // create the dir 
                            mmkdir(2, dirname, 0);
                        }
                    }
                    else {
                        // e.g. /hom
                        dirname[1][point++] = argv[i][j];
                    }
                }
                free(dirname[1]);
            }
        }
    }
}

int write_history(char* cmd, int count, char* his) 
{
    FILE* fd;
    fd = fopen(his, "a+");
    fprintf(fd, "  %d\t%s\n", count, cmd);
    fclose(fd);
}

int read_history(int argc, char* argv[], char* his)
{
    // the file is .history in the workpath 
    FILE* fd;
    char data[MAX_LENGTH];
    fd = fopen(his, "r");
    while (fgets(data, sizeof(data), fd)) {
        fputs(data, stdout);
    }
    fclose(fd);
    return 0;
}

int analyse(char* cmd, char* para[])
{
    strcat(cmd, " \0");
    int head = 0;
    int flag = 0;       // 1 - ' ' before
    char registe[MAX_LENGTH];
    int point = 0;
    int i;
    for (i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == ' ' && flag) {
            continue;
        }
        else if (cmd[i] == ' ' && flag == 0) {
            registe[point] = '\0';
            para[head] = (char*)malloc(sizeof(char) * MAX_LENGTH);
            strcpy(para[head], registe);
            head ++;
            point = 0;
            flag = 1;
        }
        else {
            registe[point++] = cmd[i];
            flag = 0;
        }
    }

    return head;
}

void help()
{
    // print the help menu
    printf(ANSI_COLOR_RED "Help Menu:\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  1.  cd      - change dir\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  2.  ls      - list the path\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -l: show more message\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -lu: show access message\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -lc: show state modifaction message\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  3.  pwd     - show current path\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  4.  rm      - remove the file or dir\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -r: recursive remove the directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -f: force to execute\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -rf: recursive remove and do not ask for user\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  5.  mkdir   - create the directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -p: recursive create the directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  6.  mv      - move the file or directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  7.  cp      - copy the file or directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  8.  exit    - exit the termianl\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  9.  history - show the command history\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  10. cat     - show the content of the file\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  11. touch   - create or change the tiemstamps\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -a: create or change access timestamps\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "      * -m: create or chaneg modifaction timestamps\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  12. time    - count the time of the commands\n" ANSI_COLOR_RESET);
    return ;
}

int main(int argc, char* argv[])
{
    int his_count = 0;
    char  cmd[MAX_LENGTH];
    char* para[MAX_LENGTH];
    char  his[MAX_LENGTH];      // record the main workpath for the .history
    strcpy(his, pwd());
    // Check or create the .history file
    strcat(his, "/.history");
    if (access(his, F_OK) == -1) {
        creat(his, 0777);
    }

    while (1) {
        // show the cmd 
        printf(ANSI_COLOR_GREEN "lantian@GMFTBY:%s$ " ANSI_COLOR_RESET, pwd());
        memset(cmd, '\0', sizeof(cmd));
        gets(cmd);
        his_count ++;

        // append into .history 
        write_history(cmd, his_count, his);

        argc = analyse(cmd, para);
        
        if (strcmp(para[0], "cd") == 0) cd(para[1]);
        else if (strcmp(para[0], "pwd") == 0) {
            printf("%s\n", pwd());
        }
        else if (strcmp(para[0], "ls") == 0) {
            int flag = 0;
            int time = 0;
            for (int i = 1; i < argc; i++) {
                if (strcmp(para[i], "-l") == 0) {
                    flag = 1;
                    break;
                }
                if (strcmp(para[i], "-lc") == 0) {
                    // modifaction time
                    time = 1;
                    break;
                }
                if (strcmp(para[i], "-lu") == 0) {
                    // access time
                    time = 2;
                    break;
                }
            }
            mls(argc, para, flag, time);
        }
        else if (strcmp(para[0], "rm") == 0) {
            int flag = 0;       // flag - 0: ask users normally, default
                                // flag - 1: do not ask users normally
                                // flag - 2: recursive and ask users
                                // flag - 3: recursive and do not ask users
            for (int i = 1; i < argc; i++) {
                if (strcmp(para[i], "-r") == 0) {
                    flag = 2;
                    break;
                }
                else if (strcmp(para[i], "-f") == 0) {
                    flag = 1;
                    break;
                }
                else if (strcmp(para[i], "-rf") == 0) {
                    flag = 3;
                    break;
                }
            }
            mrm(argc, para, flag);
        }
        else if (strcmp(para[0], "mkdir") == 0) {
            // choose the -p model for the command
            // in order to fix the path, append the character '/'
            int flag = 0;
            for (int i = 1; i < argc; i++) {
                if (strcmp(para[i], "-p") == 0) {
                    flag = 1;
                    break;
                }
            }
            mmkdir(argc, para, flag);
        }
        else if (strcmp(para[0], "cp") == 0) {
            // cp, use libmcp.a as the lib
            mcp(argc, para);
        }
        else if (strcmp(para[0], "mv") == 0) {
            // mv, use mcp and mrm
            // cp to copy the file (regular file, dir file)
            // mcp(argc, para);
            // mrm to delete the file, only delete the source file
            mrm(2, para, 3);
        }
        else if (strcmp(para[0], "history") == 0) {
            // read the .bash_history file to show the history
            read_history(argc, argv, his);
        }
        else if (strcmp(para[0], "cat") == 0) {
            cat(argc, para);
        }
        else if (strcmp(para[0], "touch") == 0) {
            int access = 0;
            int modifaction = 0;
            for (int i = 1; i < argc; i ++) {
                if (strcmp(para[i], "-a") == 0) access = 1;continue;
                if (strcmp(para[i], "-m") == 0) modifaction = 1;continue;
            }
            touch(argc, para, access, modifaction);
        }
        else if (strcmp(para[0], "time") == 0) {
            mtime(argc, para);
        }
        else if (strcmp(para[0], "j") == 0) {
            // the autojump extension of this ltsh
            myjump(0, WORK_PATH, para[1]);
        }
        else if (strcmp(para[0], "exit") == 0) exit(0);
        else {
            if (strlen(cmd) == 1) help();
            // execute the system function to help using this shell
            else system(cmd);
        }
    }
    return 0;
}
