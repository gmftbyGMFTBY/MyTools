#include<ncurses.h>
#include<stdio.h>
#include<getopt.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<error.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>

#define BUFFER_SIZE 1024
#define MAX_SIZE 1024*1024
#define TAB_LENGTH 8
#define PATH_LEN 256
#define INFO_SIZE 256
#define MOUSE 998
#define INIT 999
#define ERROR -1

/* 在终端上打印缓冲区内容 */
void print_buffer(int, int, int, char (*)[BUFFER_SIZE]);
/* 将缓冲区的内容写入文件 */
void save_file(int, char*, char (*)[BUFFER_SIZE]);
/* 生成提示窗口 */
WINDOW *indicate_window(char *msg, WINDOW*);

char file_pathname[PATH_LEN];
int line_count = 0, row_count = 0;
/* nano 程序功能 */
void nano(int argc, char *argv[])
{	
	if (argc == 1)
	{
		printf("Please indicate a file to open\n");
		return;
	}
	if(argc > 2){
		printf("Useage: nano file\n");
		return;
	}
	strncpy(file_pathname, argv[1], PATH_LEN);
	char *temp_buffer;
	/* origin_buffer保存终端上对应的二维数组 */
	/* origin_buffer[x][y]对应终端上第x行第y列对应的字符 */
	char origin_buffer[BUFFER_SIZE][BUFFER_SIZE] = {0};	
	char read_buffer[MAX_SIZE] = {0};
	int current_y = 0, current_x = 0;
	int buffer_size = 0;
	int fd = create_file(argv[1]);
	/* 打开文件失败 */
	if((buffer_size = read(fd, read_buffer, MAX_SIZE)) == -1){
		printf("Can't read from buffer, Reason : %s\n", strerror(errno));
		return;
	}
	/* 如果打开的是新文件 */
	else if(buffer_size == 0){
		/* buffer_size = 0 means open a new file */
		origin_buffer[0][0] = ' ';
	}
	/* 将从文件中读的内容转换为二维数组并保存在origin_buffer中 */
	for(int i = 0; i < strlen(read_buffer); i++){
		if(read_buffer[i] != '\n'){
			origin_buffer[line_count][row_count++] = read_buffer[i];
		}
		else if(read_buffer[i] == '\n'){		
			origin_buffer[line_count][row_count++] = '\n';
			origin_buffer[line_count][row_count++] = 0;
			line_count++;
			row_count = 0;
		}
	}
	line_count++;
	/* 初始化ncurse的窗口 */
	initscr();
	raw();
	/* stdscr为ncurse的默认窗口 */
	keypad(stdscr, TRUE);
	/* 设置捕捉鼠标行为，在这仅捕捉左键点击事件 */
	mousemask(BUTTON1_CLICKED, NULL);
	MEVENT event;
	/* 输入键后不立即在终端中输出 */
	noecho();
	/* 将buffer中的内容打印至终端，并将光标移至(0,0)位置 */
	print_buffer(0, 0, INIT, origin_buffer);
    move(0, 0);
	//char info[INFO_SIZE] = "crtl-x(^x)  exit";
	//WINDOW *indicate = indicate_window(info, stdscr);
	int ch;
	while((ch = getch())!= 0){
		/* 处理stdscr窗口输入的字符或鼠标事件 */
		int y, x;
		switch(ch){
			case KEY_MOUSE:
			/* 如果捕捉到鼠标左键点击事件 */
			/* 将光标移动至对应行的行末 */
				getmouse(&event);
				print_buffer(event.y, event.x, MOUSE, origin_buffer);
				break;
			/* 如果读入的字符为 ^X（control + X) */
			case 24:
			/* 提示用户是否将origin_buffer中的数据写入文件 */
				save_file(fd, read_buffer, origin_buffer);
			/* 退出ncurse的窗口 */
				endwin();
				return;
				break;
			case 9:
			/* 处理输入为tab的情况 */
			/* 一个tab键用四个空格表示 */
			/* 相当于将一维数组origin_buffer[x] 整体后移动4位 ，并在origin[x][y]处插入四个空格*/
				getyx(stdscr, x, y);
				/* 获得当前光标位置 */
				int len = strlen(origin_buffer[x]);
				/* 向后移动四位 */
				for (int i = len + 4; i >= y + 4; i--)
				{
					origin_buffer[x][i] = origin_buffer[x][i - 4];
				}
				origin_buffer[x][len + 4] = 0;
				/* 在origin_buffer[x][y]处插入四个空格 */
				for (int i = 0; i < 4; i++)
				{
					origin_buffer[x][y + i] = ' ';
				}
				for (int i = 0; i <= 3; i++)
					print_buffer(x, y+i, 1, origin_buffer);
				break;
			case 10:
			/* 处理输入为回车符 */
				getyx(stdscr, x, y);
				/* add a \n to origin buffer in pair (x, y)*/
				/* 在origin_buffer[x][y]处插入回车符 */
				for(int i = line_count; i > x + 1;i--){
					/* 给origin_buffer赋值 */
					memset(origin_buffer[i], 0, BUFFER_SIZE);
					strncpy(origin_buffer[i], origin_buffer[i - 1], BUFFER_SIZE);
				}
				/* 将origin_buffer[i]的最后一位赋为0,确保strlen函数得到正确结果 */
				memset(origin_buffer[x + 1], 0, BUFFER_SIZE);
				int n = strlen(origin_buffer[x]);
				/* 将origin_buffer[x][y]处的字符设置为回车符并将origin[x][y]之后的字符串移动到下一行 */
				for (int i = y; i <= n; i++)
				{
					origin_buffer[x + 1][i - y] = origin_buffer[x][i];
					origin_buffer[x][i] = 0;
				}
				origin_buffer[x][y] = '\n';
				/* 行计数器+1 */
				line_count++;
				print_buffer(x, y,1, origin_buffer);
				break;
			case KEY_BACKSPACE:
			/* 处理输入为删除（回退）的情况 */
				getyx(stdscr, x, y);
				/* 如果当前光标在起始位置 */
				if(x == 0 && y == 0){
					/* do nothing */
				}
				/* 如果在一行的行首则删除换行符，将两行合并为一行 */
				else if(y == 0){
					temp_buffer = (char *)malloc(BUFFER_SIZE);
					origin_buffer[x - 1][strlen(origin_buffer[x - 1]) - 1] = 0;
					strcat(origin_buffer[x - 1], origin_buffer[x]);
					for (int i = x; i < line_count; i++){
						strcpy(origin_buffer[i], origin_buffer[i+1]);
					}
					line_count--;
					print_buffer(x, y,0, origin_buffer);
				}
				/* 否则直接删除相应字符，并移动光标 */
				else{
					for (int i = y; i <= strlen(origin_buffer[x]); i++){
							origin_buffer[x][i - 1] = origin_buffer[x][i];
					}
					print_buffer(x, y, 0, origin_buffer);
				}
				break;
			/* 处理输入为上下左右箭头的情况 */ 
			/* 根据位置对应的字符是否为0来进行光标移动 */
			case KEY_RIGHT:
				getyx(stdscr, x, y);
				if(origin_buffer[x][y+1] != 0){
					if(move(x, y+1) == ERR){
						printf("Can't move\n");
						return;
					}
				}
				else if(origin_buffer[x][y+1] == 0){
					if(origin_buffer[x+1][0] != 0){
						if(move(x+1, 0) == ERR){
							printf("Can't move cursor\n");
							return;
						}
					}
					else{
					}
				}
				else{
				/* do nothing */
				}
				break;
			case KEY_LEFT:
				getyx(stdscr, x, y);
				if(y == 0){
					if(x != 0){
						move(x-1, strlen(origin_buffer[x-1])==0?0:strlen(origin_buffer[x-1])-1);
					}
					else{
					/* do nothing */
					}
				}
				else{
					move(x, y - 1);
				}
				break;
			case KEY_UP:
				getyx(stdscr, x, y);
				if(x != 0){
					if(origin_buffer[x-1][y] != 0)
						move(x-1, y);
					else{
						if(strlen(origin_buffer[x-1]) == 0){
							/* do nothing */
						}
						else
							move(x - 1, strlen(origin_buffer[x - 1]) - 1);
					}
				}
				else{
				}
				break;
			case KEY_DOWN:
				getyx(stdscr, x, y);
				if(x < line_count-1){
					if(origin_buffer[x+1][ y] == 0)
						move(x + 1, strlen(origin_buffer[x + 1]) - 1);
					else if(origin_buffer[x+1][y] != 0)
						move(x+1, y);
				}
				else{
				}
				break;
			default:
				getyx(stdscr, x, y);
				/* add character to origin buffer */
				int str_len = strlen(origin_buffer[x]);
				for (int i = str_len; i > y; i--)
				{
					origin_buffer[x][i] = origin_buffer[x][i - 1];
				}
				origin_buffer[x][y] = ch;
				print_buffer(x, y, 1, origin_buffer);
				break;
		}
	}
}
int create_file(char *filename){
	int fd = open(filename,  O_RDWR | O_CREAT, 0644);
	if(fd == 0){
		printf("Can't open file %s, Reason : %s\n", filename, strerror(errno));
	        return ERROR;	
	}
	return fd;
}
void print_buffer(int x, int y, int flag, char (*origin_buffer)[BUFFER_SIZE]){
	/* flag means move cursor forword or backword  where 1 means forword, 0 means backword */
	/* move cursor to pair (0,0) when flag == INIT */
	werase(stdscr);
	for (int i = 0; i < line_count; i++)
	{
		printw("%s", origin_buffer[i]);
	}
	if(flag == 1){
		if(origin_buffer[x][y+1] != 0){
			if(move(x, y+1) == ERR){
				printf("Can't move\n");
				return;
			}
		}                                		
		else if(origin_buffer[x][y+1] == 0)
		{
			if(move(x + 1, 0) == ERR){
				printf("Can't move\n");
				return;
			}
		}
	}
	else if(flag == 0){
		if(y != 0){
			if(move(x, y - 1) == ERR){
				printf("Can't move\n");
				return;
			}
		}
		else if (y == 0)
			if(move(x - 1, strlen(origin_buffer[x - 1]) - 1) == ERR){
				printf("Can't move\n");
				return;
			}
	}
	else if(flag == INIT){
		if(move(0, 2) == ERR){
			printf("Can't move\n");
			return;
		}
	}
	else if(flag == MOUSE){
		if(origin_buffer[x][y] != 0){
			if(move(x, y) == ERR){
				printf("Can't move cursor\n");
				return;
			}
		}
		else{
			if(strlen(origin_buffer[x]) >= 1){
				if(move(x, strlen(origin_buffer[x]) - 1) == ERR){
					printf("Can't move cursor\n");
					return;
				}
			}
		}
	}
	wrefresh(stdscr);	
}
void save_file(int fd, char *read_buffer, char (*origin_buffer)[BUFFER_SIZE]){
	/* convert origin_buffer to writeable buffer */
	char *write_buffer = (char *)malloc(MAX_SIZE);
	for (int i = 0; i < line_count; i++)
	{
		/* won't do anything */
		strncat(write_buffer, origin_buffer[i], MAX_SIZE);
		/* won't do anything */
	}
	/* set the pointer to the begining of file */
	int write_size = strlen(write_buffer);
	/* write_buffer equal 1 means nothing writen to file */
	if(write_size == 1){
		/* unlink file, assuming argv[1] refer to pathname */
		unlink(file_pathname);
		return;
	}
	/* if file already exist, indicate whether to cover origin content */
	/* compare new buffer and origin buffer */
	if(strncmp(write_buffer, read_buffer, BUFFER_SIZE)){
		/* add window to ask whether to cover origin file content */
		char message[PATH_LEN] = "Are you sure to save change [y/n]:";
		char confirm;
		WINDOW *info_window = indicate_window(message, stdscr);
		while(1){
			wscanw(info_window, "%c", &confirm);
			if(confirm == 'n'){
				return;
			}
			else if(confirm == 'y'){
				break;
			}
			else{
				werase(info_window);
				box(info_window, 0, 0);
				wattron(info_window, A_BOLD);
				mvwprintw(info_window, 1, 1, "Please enter yes or no");
				wrefresh(info_window);
				wgetch(info_window);
				mvwprintw(info_window, 1, 1, message);
				wattroff(info_window, A_BOLD);
				wrefresh(info_window);
			}
		}
	}
	/* cover origin content */
	/* clear_old_file(fd); */
	if (lseek(fd, 0, SEEK_SET) == -1)
	{
		perror("Set file offset failed\n");
		return;
	}
	if(write(fd, write_buffer, strlen(write_buffer)) == -1){
		printf("Can't write to file\n");
		return;
	}
	/* truncate left */
	if(ftruncate(fd, strlen(write_buffer)) == -1){
		perror("Can't truncate\n");
		return;
	}
	/* ensure content is writen to disk */
	if(fsync(fd) == -1)
	{
		perror("File saved failed");
	}
	else
		printw("File saved successfully\n");
	refresh();
}


WINDOW *indicate_window(char *msg, WINDOW* stdscr){
	WINDOW *local_window;
	int x, y;
	getmaxyx(stdscr, x, y);		
	local_window = newwin(3, y, x - 3, 0);
	echo();
	box(local_window, 0, 0);
	wattron(local_window, A_BOLD);
	mvwprintw(local_window, 1, 1, msg);
	wattroff(local_window, A_BOLD);
	wrefresh(local_window);
	return local_window;
}
