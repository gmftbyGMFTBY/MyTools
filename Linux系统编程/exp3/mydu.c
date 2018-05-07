#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<error.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>

#define ERROR -1
#define SUCCESS 0
#define MAX_SIZE 256

int helpflag = 0, aflag = 0, size_b_flag = 0, size_k_flag = 0, size_m_flag = 0, size_g_flag = 0, size_t_flag = 0, humanflag = 0;

/* size is unsigned 64-bit integer */
static blksize_t block_size;
struct disk_usage{
	/* size 代表文件夹的大小 in bytes */
	long long int size;
	/* block_number 代表文件夹占512b大小的磁盘块数 */
	blkcnt_t block_number;
	/* 代表文件系统下一个磁盘块的大小 */
	/* 当文件中存在"hole"时，其值除单位和size大小不同 */
};

typedef struct disk_usage usage;
/* return folder total size */
blkcnt_t cacl_block(blksize_t block_size, blkcnt_t block_number){
	/* 参数中的block_number为结构体中的number，每块的大小为512b;
	/* 为了统计1024b大小的块的数量，需要计算 */
	return block_number / 2;
}

void format_out(long long int size, long long int block_number, int b_flag, int k_flag, int m_flag, int g_flag, int t_flag, char *relative_path)
{
	long long int  folder_size = size;
	if (b_flag == 1)
	{
		printf("%lld\t%s\n", folder_size , relative_path);
	}
	else if(k_flag == 1){
		printf("%lldK\t%s\n", folder_size / 1024 + 1, relative_path);
	}
	else if(m_flag == 1){
		printf("%lldM\t%s\n", (folder_size / 1024) / 1024 + 1, relative_path);		
	}
	else if(g_flag == 1){
		printf("%lldG\t%s\n", ((folder_size / 1024) / 1024) / 1024 + 1, relative_path);
	}
	else if(t_flag == 1){
		printf("%lldT\t%s\n", (((folder_size / 1024) / 1024) / 1024) / 1024 + 1, relative_path);
	}
	/* default format is k_flag */
	else{
		printf("%lld\t%s\n", block_number, relative_path);
	}
}
usage*  mydu(char *pathname)
{
	DIR *dirp;
	struct dirent *dp;
	struct stat buffer;
	long long int total_size = 0;
	blkcnt_t block_number = 0;

	if ((dirp = opendir(pathname)) == NULL)
	{
		perror("Can't open dir at mydu");
		return ERROR;
	}
	while((dp = readdir(dirp)) != NULL){
		/* ! If no flag */
		char relative_path[MAX_SIZE];
		strncpy(relative_path, pathname, MAX_SIZE);
		strcat(relative_path, "/");
		strcat(relative_path, dp->d_name);
		if (stat(relative_path, &buffer) == -1)
		{
			perror("Can't get file state at mydu");
			return ERROR;
		}
		block_size = buffer.st_blksize;
		if ((buffer.st_mode & __S_IFMT) == __S_IFDIR)
		{
			/* Don't take hidden folder into consideration */
			if((!strcmp(dp->d_name, ".")) || (!strcmp(dp->d_name, ".."))){
			}
			else{
				char newpath[MAX_SIZE];
				usage *disk = mydu(relative_path);
				long long int folder_size = buffer.st_size + disk->size;
				blkcnt_t folder_block_number = buffer.st_blocks + disk->block_number;
				total_size += folder_size;
				block_number += folder_block_number;
				format_out(folder_size, cacl_block(block_size,folder_block_number),size_b_flag, size_k_flag, size_m_flag, size_g_flag, size_t_flag, relative_path);
			}
		}
		/* 处理普通文件 */
		/* reg means regular */
		else if((buffer.st_mode & __S_IFMT) == __S_IFREG){
			total_size += buffer.st_size;
			block_number += buffer.st_blocks;
			if (aflag == 1)
				format_out(buffer.st_size, cacl_block(block_size, buffer.st_blocks), size_b_flag, size_k_flag, size_m_flag, size_g_flag, size_t_flag, relative_path);				
		}
	}
	usage *return_value;
	/* 为结构分配内存 */
	return_value = malloc(sizeof(usage));
	return_value->block_number = block_number;
	return_value->size = total_size;
	return return_value;
}

int du(int argc, char *argv[])
{
    argv[argc] = NULL;
	int opt;
	char pathname[MAX_SIZE];
	usage *folder_usage;
	folder_usage = malloc(sizeof(usage));
	blkcnt_t block_number_in_k;
	while ((opt = getopt(argc, argv, "ahB:b")) != -1)
	{
        printf("%c\n", opt);
		switch(opt){
			case 'B':
				if(!strcmp("K", optarg))
					size_k_flag = 1;
				else if(!strcmp("M", optarg))		
					size_m_flag = 1;
				else if(!strcmp("G", optarg))
					size_g_flag = 1;
				else if(!strcmp("T", optarg))
					size_t_flag = 1;
				else
				{
					printf("Usage: mydu -B[K|M|G|T]\n");
					return ERROR;
				}
				break;
			case 'a':
				aflag = 1;
				break;
			case 'h':
				humanflag = 1;
				break;
			case 'b':
				size_b_flag = 1;
                break;
			default:
				printf("Usage mydu [option] arg\n");
				return ERROR;
	    }
	}
	if(helpflag == 1){
		printf("print help information\n");
		return SUCCESS;
	}
	if(argv[optind] == NULL){
		/* mydu current directory */
		/* 不仅计算文件夹中文件占用的磁盘数，也计算文件夹所占大小 */
		folder_usage = mydu(".");
		block_number_in_k = cacl_block(block_size, folder_usage->block_number) + block_size / 1024;
		if (errno == 0)
			format_out(folder_usage->size + block_size, block_number_in_k,size_b_flag, size_k_flag, size_m_flag, size_g_flag, size_t_flag, ".");
	}
	else{
        if (argv[optind + 1] != NULL) {
            printf("Can not analyse the paramter\n");
            return ERROR;
        }
        else {
            folder_usage = mydu(argv[optind]);
		    block_number_in_k = cacl_block(block_size, folder_usage->block_number) + block_size / 1024;	
		    if (errno == 0)
			    format_out(folder_usage->size+block_size, block_number_in_k, size_b_flag, size_k_flag, size_m_flag, size_g_flag, size_t_flag, argv[optind]);							
        }
    }
}
