#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXNAME 2000

void s_print(char *filename,int depth);
void s_dirwalk(char *dirname,int depth);
void listdirtree(char *dirname,int depth);
void listdirtree1(char *dirname,int depth,int lim);
void s_dirwalk1(char *dirname,int depth,int lim);


void s_print(char *filename,int depth)
{
    while(depth-- > 0)
        printf("│   ");
    printf("└── ");
    struct stat buf;
    stat(filename,&buf);
    if(S_ISDIR(buf.st_mode))
        printf("\x1b[34m%s\x1b[0m\n",filename);
    else
        printf("%s\n",filename);
}

void listdirtree1(char *dirname,int depth,int lim)
{
    if(depth>lim)   return;
    struct stat stbuf;
    if((stat(dirname,&stbuf)) == -1)
    {
        fprintf(stderr,"listdirtree:can't read file %s information!\n",dirname);//在屏幕上打印读取文件失败
        return;
    }
    if((stbuf.st_mode & S_IFMT) == S_IFDIR)//文件类型是目录
        s_dirwalk1(dirname,depth,lim);
}

void s_dirwalk1(char *dirname,int depth,int lim)//路径，深度，函数指针
{
    char name[MAXNAME];
    DIR *dp;
    if((dp = opendir(dirname)) == NULL)//打开路径失败
    {
        fprintf(stderr,"s_dirwalk:can't open %s\n",dirname);
        return;
    }
    struct dirent **namelist;
    int n;
    n = scandir(dirname,&namelist,0,alphasort);
    if(n < 0)
    {
        printf("scandir return %d\n",n);
        return;
    }
    int index = 0;
    while(index<n)
    {
        if(strcmp(namelist[index]->d_name,".") == 0 || strcmp(namelist[index]->d_name,"..") == 0)//跳过目录'.'和'..'
            ;
        else if(strlen(dirname) + strlen(namelist[index]->d_name) + 2 > sizeof(name))//之前开辟的空间不够
        {
            fprintf(stderr,"s_dirwalk:%s/%s is too long!\n",dirname,namelist[index]->d_name);
            return;
        }
        else
        {
            s_print(namelist[index]->d_name,depth);//按照固定输出格式打印文件名
            sprintf(name,"%s/%s",dirname,namelist[index]->d_name);//把当前遍历的文件名存到name中
            listdirtree1(name,depth + 1,lim);//递归输出目录树
        }
        free(namelist[index]);
        index++;
    }
    free(namelist);
    closedir(dp);
}

void listdirtree(char *dirname,int depth)
{
    struct stat stbuf;
    if((stat(dirname,&stbuf)) == -1)
    {
        fprintf(stderr,"listdirtree:can't read file %s information!\n",dirname);//在屏幕上打印读取文件失败
        return;
    }
    if((stbuf.st_mode & S_IFMT) == S_IFDIR)//文件类型是目录
        s_dirwalk(dirname,depth);
}

void s_dirwalk(char *dirname,int depth)//路径，深度，函数指针
{
    char name[MAXNAME];
    DIR *dp;
    if((dp = opendir(dirname)) == NULL)//打开路径失败
    {
        fprintf(stderr,"s_dirwalk:can't open %s\n",dirname);
        return;
    }
    struct dirent **namelist;
    int n;
    n = scandir(dirname,&namelist,0,alphasort);
    if(n < 0)
    {
        printf("scandir return %d\n",n);
        return;
    }
    int index = 0;
    while(index<n)
    {
        if(strcmp(namelist[index]->d_name,".") == 0 || strcmp(namelist[index]->d_name,"..") == 0)//跳过目录'.'和'..'
            ;
        else if(strlen(dirname) + strlen(namelist[index]->d_name) + 2 > sizeof(name))//之前开辟的空间不够
        {
            fprintf(stderr,"s_dirwalk:%s/%s is too long!\n",dirname,namelist[index]->d_name);
            return;
        }
        else
        {
            s_print(namelist[index]->d_name,depth);//按照固定输出格式打印文件名
            sprintf(name,"%s/%s",dirname,namelist[index]->d_name);//把当前遍历的文件名存到name中
            listdirtree(name,depth + 1);//递归输出目录树
        }
        free(namelist[index]);
        index++;
    }
    free(namelist);
    closedir(dp);
}

int main(int argc,char **argv)
{
    if(argc == 1)//没有参数
        listdirtree(".",0);//默认当前路径
    else if(argc==2)//一个路径参数
    {
        printf("%s\n",argv[1]);//打印指定路径
        listdirtree(*++argv,0);//列出指定路径的目录树
    }
    else if(argc==3)//一个路径参数,一个控制输出层数的参数
    {
        printf("%s\n",argv[1]);//打印指定路径
        listdirtree1(*++argv,0,atof(argv[2])-1);
        //listdirtree(*++argv,0);//列出指定路径的目录树
    }
    return 0;
}
