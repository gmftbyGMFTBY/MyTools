#include<stdio.h>
#include<dirent.h>
#include<regex.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>

int state[5] = {0,0,0,0,0};//分别是-name,-prune,-ctime,-mtime,-print
//存放-name命令时的文件名
char name[512];
//存放-prune指定的名字
char pname[512];
//存放-ctime指定的参数, 如-8
char ctim[32];

int pow10(int i)
{
    if(i == 0) return 1;
    if(i == 1) return 10;
    return 10*pow10(i-1);
}

int isMatch(const char *s, const char *p)//第一个参数为待查找的路径，第二个参数为文件名。若匹配，返回1；否则，返回0
{
    if (s == NULL && p == NULL)
        return 1;
    int m = strlen(s);
    int n = strlen(p);
    if (m == 0 && n == 0)
        return 1;
    if(strcmp(s,p)==0)
        return 1;
    char errbuf[1024];
    regex_t reg;
    int err,nm = 1024;
    regmatch_t pmatch[nm];

    if(regcomp(&reg,p,REG_EXTENDED) != 0){
        printf("\x1b[31mSomething wrong with the regex pattern string\n\x1b[0m");
        return 0;
    }

    err = regexec(&reg,s,nm,pmatch,0);

    if(err != REG_NOMATCH && (strlen(s) == (pmatch[0].rm_eo - pmatch[0].rm_so)))
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

void listDir(char* path)
{
    DIR* dir;
    struct dirent* mydirent;
    //存放子目录的全名, 用于递归
    char childPath[512];

    if((dir = opendir(path)) != NULL)
    {
        while((mydirent = readdir(dir)) != NULL)
        {
            //存放该文件的全路径名
            char fullName[1024];
            sprintf(fullName, "%s/%s", path, mydirent->d_name);

            int flag = 1;
            //如果是.和.. 不用进入里面遍历
            if(strcmp(mydirent->d_name, ".") == 0 || strcmp(mydirent->d_name, "..") == 0) continue;
            //如果指定了-name 但 该文件不符合指定的名字, flag==0
            if(flag && state[0]==1 && !isMatch(mydirent->d_name, name)) flag = 0;
            //如果指定了-prune且匹配文件名, 则flag==0
            if(state[1]==1 && isMatch(fullName, pname)) flag = 0;
            //-ctime
            if(flag && state[2]==1)
            {
                //查询文件详细信息
                struct stat buf;
                stat(fullName, &buf);

                //求出当前时间-文件ctime时间, 算出差值
                time_t t;
                time(&t);
                //一天86400秒, 算出差几天
                int diff = (t - buf.st_ctim.tv_sec)/86400;

                //ctim[]里存的是-12这种字符串, 这一步提取出后面的12字符串并转化为int存在num里
                int num = atof(ctim + 1);
                if(diff > num) flag = 0;
            }

            if(flag) printf("%s\n", fullName);//如果完整通过, 则输出全路径名
            if(mydirent->d_type & DT_DIR)    //如果是个子目录,递归继续遍历
            {
                //得到子目录全名递归进入里面遍历
                sprintf(childPath, "%s/%s", path, mydirent->d_name);
                listDir(fullName);
            }
        }
        closedir(dir);
    }
}

int myfind(int argc,char *argv[])
{
    if(argc<2)
    {
        printf("\x1b[31mWrong paramter\n\x1b[0m");
        return -1;
    }
    char *path = argv[1];
    for(int i=2;i<argc-1;i++)
    {
        if(strcmp(argv[i],"-name")==0)//实现find path -name name
        {
            if(i+1>=argc)//如果未指定输入文件名
            {
                printf("\x1b[31mPlease input the file name\n\x1b[0m");
                return -1; 
            }
            strcpy(name,argv[i+1]);
            //表示当前命令为-name
            state[0] = 1;
        }
        else if(strcmp(argv[i], "-prune")==0)//除了给出的文件名其余全部打印。支持正则表达式
        {
            //如果指定了-prune但没输入目录名
            if(i+1>=argc)
            {
                printf("\x1b[31mPlease input the file name\n\x1b[0m");
                return -1;
            }
            //排除目录按照通配符匹配来排除, 预先在头尾加上通配符*
            //输入di1, 会将dir1, dir2都排除
            strcat(pname, argv[i+1]);
            state[1] = 1;
        }
        else if(strcmp(argv[i], "-ctime")==0)//查找在指定时间之时被更改的文件或目录，单位以24小时计算
        {
            //如果指定了-ctime但没输入时间
            if(i>=argc-1)
            {
                printf("\x1b[31mPlease input the query time\n\x1b[0m");
                return -1;
            }
            strcpy(ctim, argv[i+1]);
            state[2] = 1;
        }
        else if(strcmp(argv[i], "-print")==0)//打印该目录下的所有文件
        {
            state[4] = 1;
        }
    }

    listDir(path);

    return 0;
}
