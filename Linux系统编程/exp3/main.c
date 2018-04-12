#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char* pwd()
{
    return getcwd(NULL, 0);
}

void cd(char* argv)
{
    if (chdir(argv) == -1) {
        perror("chdir");
        return ;
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

void ls(int count, char* argv[], int flag) 
{
    struct stat buf;
    DIR* dirptr = NULL;
    struct dirent* entry = NULL;

    for (int i = 1; i < count; i++) {
        if (strcmp(argv[i], "-l") == 0) continue;


        // file not exist case
        if (access(argv[i], F_OK) == -1) {
            perror("access");
            continue;
        }

        stat(argv[i], &buf);
        printf("%s:\n", argv[i]);
        if (S_ISDIR(buf.st_mode)) {
            dirptr = opendir(argv[i]);

            char savepoint[100];
            strcpy(savepoint, pwd());
            cd(argv[i]);

                // show the file under the dir
                while (entry = readdir(dirptr)) {
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
                        printf("\t%.12s",4 + ctime(&ss.st_mtime));
    
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
            cd(savepoint);
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
                printf("\t%.12s",4 + ctime(&buf.st_mtime));
                printf(ANSI_COLOR_GREEN "\t%s\n" ANSI_COLOR_RESET, argv[i]);
            }
        }
        printf("\n");

        // cd(savepoint);
    }
}

void mls(int argc, char* argv[], int flag)
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
        ls(2, data, 0);
        free(data[1]);
    }
    else {
        if (argc == 2 && (strcmp(argv[1], "-l") == 0)) {
            argv[2] = (char*) malloc (sizeof(char) * 100);
            strcpy(argv[2], "./");
            ls(3, argv, 1);
            free(argv[2]);
        }
        else ls(argc, argv, flag);
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
    char data[200];
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
    char registe[100];
    int point = 0;
    int i;
    for (i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == ' ' && flag) {
            continue;
        }
        else if (cmd[i] == ' ' && flag == 0) {
            registe[point] = '\0';
            para[head] = (char*)malloc(sizeof(char) * 100);
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
    printf(ANSI_COLOR_RED "  1. cd      - change dir\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  2. ls      - list the path\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "     * -l: show more message\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  3. pwd     - show current path\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  4. rm      - remove the file or dir\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "     * -r: recursive remove the directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "     * -f: force to execute\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "     * -rf: recursive remove and do not ask for user\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  5. mkdir   - create the directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  6. mv      - move the file or directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  7. cp      - copy the file or directory\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  8. exit    - exit the termianl\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "  9. history - show the command history\n" ANSI_COLOR_RESET);
    return ;
}

int main(int argc, char* argv[])
{
    int his_count = 0;
    char cmd[200];
    char* para[200];
    char his[200];      // record the main workpath for the .history
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
            for (int i = 1; i < argc; i++) {
                if (strcmp(para[i], "-l") == 0) {
                    flag = 1;
                    break;
                }
            }
            mls(argc, para, flag);
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
        else if (strcmp(para[0], "exit") == 0) exit(0);
        else {
            if (strlen(cmd) == 1) help();
            // execute the system function to help using this shell
            else system(cmd);
        }
    }
    return 0;
}
