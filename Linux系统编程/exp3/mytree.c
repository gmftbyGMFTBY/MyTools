#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXNAME 2000

void s_print(char *filename,int depth, int flag, int *f,int color);//color为1代表蓝色；2代表绿色
void s_dirwalk(char *dirname,int depth,int *f, int limit, int onlydir);
void listdirtree(char *dirname,int depth,int *f, int limit, int onlydir);

void s_print(char *filename,int depth, int flag, int *f, int color)
{
    for(int i=0;i<depth;i++)
    {
        if(f[i])
            printf("   ");
        else
            printf("│  ");
    }
    if(flag==0)
        printf("├──");
    else
        printf("└──");

    struct stat buf;
    stat(filename,&buf);
    if(color==1)
    {
        printf("\x1b[34m%s\n\x1b[0m",filename);
    }
    else
    {
        printf("%s\n",filename);
    }

}

void listdirtree(char *dirname,int depth,int *f, int limit,int onlydir)
{
    if(depth>limit)
        return;
    struct stat stbuf;
    if((stat(dirname,&stbuf)) == -1)
    {
        fprintf(stderr,"listdirtree:can't read file %s information!\n",dirname);//在屏幕上打印读取文件失败
        return;
    }
    if((stbuf.st_mode & S_IFMT) == S_IFDIR)//文件类型是目录
        s_dirwalk(dirname,depth,f, limit,onlydir);
}

void s_dirwalk(char *dirname,int depth,int *f, int limit,int onlydir)//路径，深度，函数指针
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
    int k = 0,i = 0;
    while(i<n)//记录最后一个文件夹的编号
    {
        if(strcmp(namelist[i]->d_name,".") == 0 || strcmp(namelist[i]->d_name,"..") == 0)//跳过目录'.'和'..'
        {
            i++;
            continue;
        }
        if(namelist[i]->d_type==DT_DIR)
            k = i;
        i++;
    }
    int index = 0,color = 0;
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
            int flag = 0;//标记是否为当前层最后一个文件
            if (index == n - 1) flag = 1;//按照固定输出格式打印文件名
            f[depth] = flag;
            if(onlydir)
            {
                if(namelist[index]->d_type==DT_DIR)
                {
                    if (index == k)
                        flag = 1;
                    else
                        flag = 0;
                    s_print(namelist[index]->d_name, depth, flag, f, 1);
                }
            }
            else
            {
                if(namelist[index]->d_type==DT_DIR)
                    s_print(namelist[index]->d_name, depth, flag, f, 1);
                else
                    s_print(namelist[index]->d_name, depth, flag, f, 0);
            }
            sprintf(name,"%s/%s",dirname,namelist[index]->d_name);//把当前遍历的文件名存到name中
            listdirtree(name,depth + 1,f, limit,onlydir);//递归输出目录树
        }
        free(namelist[index]);
        index++;
    }
    free(namelist);
    closedir(dp);
}

int mytree(int argc,char *argv[])
{
    int f[MAXNAME];
    memset(f,0,sizeof(f));
    switch(argc)
    {
        case 1:
            printf("\x1b[34m.\n\x1b[0m");//打印指定路径
            listdirtree(".",0,f,MAXNAME,0);//tree
            break;
        case 2:
            if(strcmp(argv[1],"-l")!=0)//tree path
            {
                printf("\x1b[34m%s\n\x1b[0m", argv[1]);//打印指定路径
                listdirtree(*++argv, 0, f, MAXNAME,0);//列出指定路径的目录树
            }
            else//tree -l
            {
                printf("\x1b[34m.\n\x1b[0m");//打印指定路径
                listdirtree(".", 0, f, MAXNAME,1);//列出指定路径的目录树
            }
            break;
        case 3://tree path -l
            printf("\x1b[34m%s\n\x1b[0m",argv[1]);//打印指定路径
            listdirtree(*++argv, 0, f, MAXNAME,1);//列出指定路径的目录树
            break;
        case 4://tree path -d int
            printf("\x1b[34m%s\n\x1b[0m",argv[1]);//打印指定路径
            listdirtree(*++argv,0,f,atof(argv[3])-1,0);
            break;
        case 5://tree path -l -d int
            printf("\x1b[34m%s\n\x1b[0m",argv[1]);//打印指定路径
            listdirtree(*++argv,0,f,atof(argv[4])-1,1);
            break;
    }
    return 0;
}
