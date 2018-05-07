#ifndef GREP_H
#define GREP_H
#define MAX_SIZE 256
#define NMATCH 20
#define END_IDEN -5
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct print_position{
	int start;
	int end;
};
typedef struct print_position position;

void init_struct(position*, int);
void format_out_grep(char *string, position *all_position, int line_count, int n_flag, int c_flag);
regmatch_t* mygrep(char *regular, char *string, int E_flag, int i_flag);
regex_t* preg_gen(char *buffer, int E_flag, int i_flag);
int16_t is_matched(char *regular, char *string, int E_flag, int i_flag);
void find_all_match(char *regular, char *string, position*, int E_flag, int i_flag);
void print_matched(const char *string, const position*);
void grep(int argc, char *argv[]);
#endif
