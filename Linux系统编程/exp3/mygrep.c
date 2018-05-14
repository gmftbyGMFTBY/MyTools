#include<regex.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<error.h>
#include<errno.h>
#include<unistd.h>
#include"mygrep.h"

void format_out_grep(char *string, position *all_position, int line_count, int n_flag, int c_flag){
	/* 根据参数格式化输出结果 */
	if(n_flag == 1)
		printf("%d:", line_count);
	if(c_flag == 1)
		print_matched(string, all_position);
	else
		printf("%s", string);
}
/* 根据输入字符串产生用于匹配的preg */
regex_t* preg_gen(char *buffer, int E_flag, int i_flag){
	static regex_t preg;
	int error_code = 0;
	if((error_code = regcomp(&preg, buffer, ((E_flag == 1)? REG_EXTENDED:0) | ((i_flag == 1)? REG_ICASE:0) | REG_NEWLINE)) != 0){
		char error_buffer[MAX_SIZE];
		regerror(error_code,&preg, error_buffer, MAX_SIZE);
		printf("generate preg failed, Reason : %s\n",error_buffer);
		return;
	}
	return &preg;
}
/* 将字符串和此前产生的preg匹配 */
regmatch_t* mygrep(char *regular, char *string, int E_flag, int i_flag){
	regex_t *preg = preg_gen(regular, E_flag, i_flag);
	static regmatch_t pmatch[NMATCH];
	/* Not match, return NULL */
	if(regexec(preg, string, NMATCH, pmatch, REG_NOTEOL) == REG_NOMATCH){
		return NULL;
	}
	return pmatch;
}
/* 此函数为接口，可供其他函数调用，用于判断是否匹配 */
int16_t is_matched(char *regular, char *string, int E_flag, int i_flag){
	regmatch_t *match = mygrep(regular, string, E_flag, i_flag);
	if(!(match == NULL))
		return 1;
	else
		return 0;
}
/* 找出字符串中所有匹配的子串的位置 */
void find_all_match(char *regular, char *string, position* all_position, int E_flag, int i_flag){

	regmatch_t *match = mygrep(regular, string, E_flag, i_flag);
	/* len表示已到达的长度 */
	int len = 0;
	int string_len = strlen(string);
	while (match != NULL)
	{
		/* 在结构数组中插入匹配的起始位置 */
		/*  找到在结构数组中能插入位置的索引 */
		/* 出现rm_ep与rm_so相等的情况， 说明开始匹配空 */
		/* 插入匹配的位置 */
		/* 如果在起始位置匹配空串 */
		if((match ->rm_eo == match->rm_so) && match ->rm_eo == 0) {
		/* 因为匹配为空，因此不需记录位置信息，仅需将位置后移一位 */
			len++;
			/* 如果移动到的位置大于串长，立即结束 */
			if(len > string_len)
				break;
		}
		else{
			int count = 0;
			while ((all_position + count)->end != END_IDEN)
				count++;
			(all_position + count)->start = match->rm_so + len;
			(all_position + count)->end = match->rm_eo - 1 + len;
			/* 计算此次匹配的长度 */
			len += match->rm_eo;
		}
			match = mygrep(regular, string + len, E_flag, i_flag);
	}
	return;
}
/* 打印字符串中所有匹配的子串 */
void print_matched(const char *string, const position *all_position){
	/* 根据all_position中的信息打印字符串中匹配的字串 */
	int count = 0;
	/* i_count记录结构数组最后一个end对应i的索引 */
	int i_count = 0;
	for (int i = 0; i < strlen(string); i++)
	{
		if((all_position->start) == END_IDEN){
			/* 说明对应字符串中没有可打印的匹配 */
			printf("%s", string);
			return;
		}
		if(i < (all_position + count)-> start){
			putchar(string[i]);
		}
		else if(i > (all_position + count) -> end){
			putchar(string[i]);
		}
		else{
			/* start <= i <= end */
			/* 当i = end时，打印彩色字符并使count+1 */
			if(i == (all_position+count) -> end){
				char end[2];
				strncpy(end, string + i, 1);
				end[1] = 0;
				printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, end);
				count++;
				if (((all_position + count) -> end == END_IDEN)|| count >= NMATCH){
					i_count = i + 1;
					break;
				}
			}
			else{
				char print_buffer[MAX_SIZE] = {0};
				size_t size = (all_position + count)->end - (all_position + count)->start;
				memcpy(print_buffer, string + i, size);
				printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, print_buffer);
				i = (all_position + count)->end - 1;
			}
		}
	}
	/* 打印字符串剩余部分 */
	printf("%s", string + i_count);
}
void init_struct(position* pos, int size){
	for (int i = 0; i < size; i++){
		(pos + i)->start = END_IDEN;
		(pos + i)->end = END_IDEN;
	}
}

void grep(int argc, char *argv[]){
	/* 用法 grep [选项] ... PATTERN [FILE]...*/
	/* 在每个FILE或是标准输入中查找PATTER */
	/* 选项
	-E 使用扩展的正则表达式 
	-G 使用基本的正则表达式
	-e 显示知名PATTERN
	-i 在匹配时忽略大小写
	-n 匹配文件时指出匹配的行号
	-c 使用标记高亮匹配字符串
	-h 显示帮助信息
	*/
    argv[argc] = NULL;
	int opt;
	int E_flag = 0, G_flag = 0, e_flag = 0, i_flag = 0, n_flag = 0, c_flag = 0, h_flag = 0;
	int line_count = 0;
	char file_name[MAX_SIZE];
	char pattern[MAX_SIZE];
	char *line_content = NULL;
	size_t len = 0;
	ssize_t read;
	position all_position[NMATCH];
	/* 初始化 */
	memset(file_name, 0, MAX_SIZE);
	memset(pattern, 0, MAX_SIZE);
	while ((opt = getopt(argc, argv, "EGf:e:inhc-")) != -1){
		switch (opt)
		{
		case 'E':
			E_flag = 1;
			break;
		case 'G':
			G_flag = 1;
			break;
		/*case 'f':
			f_flag = 0;
			strncpy(file_name, optarg, MAX_SIZE);
			break;*/
		case 'e':
			e_flag = 1;
			strncpy(pattern, optarg, MAX_SIZE);
			break;
		case 'i':
			i_flag = 1;
			break;
		case 'n':
			n_flag = 1;
			break;
		case 'c':
			c_flag = 1;
			break;
		case 'h':
			h_flag = 1;
			break;
		default:
			printf("Usage grep [option]  pattern file where option is -EGfeinh\n");
			return;
			break;
		}
	}
	if(h_flag == 1){
		printf("grep searches the named input files for lines containing a match to the given pattern.\n"
		       "For more detail information, please refer to manual.\n");
		return;
	}
	if(e_flag != 1){
		/* 说明没有-e 参数 */
		if(argv[optind] == NULL){
			/* 参数输入错误 */
			printf("Usage grep [option]  pattern file where option is EGfein\n");
			return;
		}
		strncpy(pattern, argv[optind], MAX_SIZE);
		strncpy(file_name, argv[optind + 1], MAX_SIZE);
	}
	else{
		if(argv[optind] == NULL){
			/* 说明用户参数输入错误 */
			printf("Usage grep [option]  pattern file where option is EGfein\n");
			return;
		}
		strncpy(file_name, argv[optind], MAX_SIZE);
	}
	FILE *fp = fopen(file_name, "r");
	if(fp == NULL){
		printf("Open file failed\n");
		return;
	}
	while((read = getline(&line_content, &len, fp)) != -1){
		line_count++;
		init_struct(all_position, NMATCH);
		if(is_matched(pattern, line_content, E_flag, i_flag)){
			find_all_match(pattern, line_content, all_position, E_flag, i_flag);
			format_out_grep(line_content, all_position, line_count, n_flag, c_flag);
		}
	}
}