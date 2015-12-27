
#include "malloc.h"
#include <unistd.h>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "minishell_core.h"

// #  include "readline.h"
// #  include "history.h"
#include <readline/readline.h>
#include <readline/history.h>
const char _seps[] = " ,\t\n";


int do_undo_ex(void *ptr, int argc, char **argv)
{
	return 0;
}

// void sh_init_env()
// {

// }
void sh_detach_fmt (char *fmt,long len, char **cmd, int *count)
{
	// char *cmd[256],
	char *token = NULL;	
	int   index = 0;
	char  seps[] = " ,\t\n";

	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt+len) = '\0';
	token = strtok(fmt,seps);

	while(token != NULL) {
		cmd[index] = token;
		index++;
		token = strtok(NULL,seps);	
	}
	*count = index;
}

void sh_detach_fmt2 (char *fmt,long len, struct sh_detach_depth *depth)
{
	// char *cmd[256],
	char *token = NULL;	
	int   index = 0;
	// char  seps[] = " ,\t\n";

	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt+len) = '\0';
	token = strtok(fmt,depth->seps);
	
	while(token != NULL && index < depth->len) {
		depth->cmd[index] = token;
		index++;
		token = strtok(NULL,depth->seps);	
	}
	depth->count = index;
}
void sh_display_prompt(struct cmd_prompt *pprompt);

int
searchboot(
		int argc, 
		char **argv, 
		struct cmd_prompt *pprompt,
		struct cmd_prompt **plist)
{
	struct cmd_prompt *pstart;
	// struct cmd_prompt *boot;
	int find = 0;
	int index = 0;
	int len;

	// boot = pprompt;

	pstart = pprompt;
	for (index = 0; index < argc; index++) {
		find = 0;
		len = strlen(argv[index]);
		while ( pstart && pstart->name ) {
			if ( pstart->type == CMDP_TYPE_STR &&   
				0 == memcmp(argv[index], pstart->name, len) ) {
				find = 1;
				pstart = pstart->next;
				break;
			}
			if (pstart->type == CMDP_TYPE_PASS) {
				find = 1;
				pstart = pstart->next;
				break;
			}
			pstart++;
		}

		if (find == 0) {
			break;
		}	
	}

	
	// printf("\nindex %d argc %d list %x    ", index, argc, pstart);
	// 当index与argc相等表示完全匹配，
	// 当pstart不为NULL表示有下一层提示

	if (index == argc && pstart) {
		*plist = pstart;
		if (pstart->next == NULL) {
			return 0;
		}
		else {
			return 1;	
		}
	}
	if (index == argc ) {
		*plist = pstart;
		return 2;
	}

	// 当index 小于argc且pstart不为NULL表示前段部分匹配
	// if (pstart) {
	// 	printf("start name %s\n", pstart->name);
	// 	*plist = pstart;
	// 	printf("c\n");
	// 	return 0;
	// }
	*plist = NULL;
	return -1;
}

// #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

void sh_display_prompt(struct cmd_prompt *pprompt)
{
	// int i = 0;
	struct cmd_prompt *ptprompt;

	ptprompt = pprompt;
	while( ptprompt->name ) {
		printf("\t%-12s\t%s\n", ptprompt->name, ptprompt->help);
		ptprompt++;
	}
}
void sh_list(char *text, int len, struct cmd_prompt *boot)
{

}

#define PROMPT_DEPTH (16)
struct cmd_prompt *_prompt_tree[PROMPT_DEPTH];
int         _prompt_index = 0;

extern struct cmd_prompt cmd_boot[];

extern char *rl_display_prompt ;
int funtest2(int a, int b);
int funtest(int a, int b)
{
	_prompt_tree[0] = &cmd_boot[0];
	_prompt_index    = 0;
	
	printf ("a = %d b = %d\n", a, b);
	// printf("[%s]", rl_display_prompt);
	rl_bind_key('?',funtest2);
	// rl_bind_key('\t',funtest2);
	return 0;
}	

struct cmd_prompt *sh_down_prompt_level(
	struct cmd_prompt *level)
{
	if (_prompt_index >= (PROMPT_DEPTH - 2) ) {
		return NULL;
	}
	_prompt_tree[ ++_prompt_index ] = level;
	return _prompt_tree[_prompt_index-1];
}


struct cmd_prompt *sh_up_prompt_level(void)
{
	if (_prompt_index == 0 ) {
		return NULL;
	}
	_prompt_index--;
	return _prompt_tree[_prompt_index+1];
}



int funtest2(int cnt, int key)
{
	int len    = strlen(rl_line_buffer);
#ifdef MINISHELL_USE_MALLOC
	char *pbuf = (char*)malloc(len);
#else
	char buf[1024];
	char *pbuf = buf;
#endif
	char *cmd[256];
	int count;
	// rl_vi_put (2,'?');
	// return 0;
	memcpy(pbuf, rl_line_buffer, len);


	sh_detach_fmt(pbuf, len, cmd, &count);
	
	struct cmd_prompt *plist;
	int ret;
	printf("index %d\n", _prompt_index);
	ret = searchboot(count, cmd, _prompt_tree[_prompt_index], &plist);
	// printf("ret = %d\n", ret);
	if (ret == 0 || ret == 1) {
		// sh_display_prompt(plist);
		sh_display_prompt(plist);
	}
	else if (ret == 2){
		printf("\t<cr>        Enter \n");
	}
	// else if (ret == 0) {
	// else {

	// 	printf("\t<cr> Enter %x\n", plist);
	// }
	


#ifdef MINISHELL_USE_MALLOC
	free(pbuf);
#endif
	printf("%s%s", rl_prompt, rl_line_buffer);
	// rl_on_new_line();
	return 0;
}	

void sh_analyse_ex (char *fmt, long len, struct sh_detach_depth *depth2)
{
	//char (*cmd)[10];
	char *cmd[256], *token = NULL;
	unsigned int count = 0;
	char seps[]   = " ,\t\n";

	// struct cmd_table *pstart;
	// struct cmd_table *pend;
	struct cmd_prompt *pstart;
	int find = 0;

	// sh_detach_fmt(fmt,len,cmd,&count);
	// char str1[] = "abcd";
	// char str2[] = "defg";

	// cmd[0] = str1;
	// cmd[1] = str2;
	struct sh_detach_depth depth;
	depth.cmd = cmd;
	depth.len = 4;
	depth.count = 0;
	depth.seps = NULL;
	// printf("[%x]", depth2->cmd +1);
	// sh_detach_fmt2(fmt,len, &depth2);
	sh_detach_fmt2(fmt,len, depth2);
	if (depth2->count) {
		pstart = _prompt_tree[_prompt_index];
		while(pstart->name) {
			if(0 == memcmp(depth2->cmd[0], pstart->name, strlen(depth2->cmd[0]))) {
				find = 1;
				break;
			}
			pstart++;
		}

		if(find == 1) {
			if (pstart->fun) {
				pstart->fun(NULL, depth2->count, (char **)cmd);
			}
		}
		else {
			// printf("%s: command not found\n", token);
			printf("%s: command not found\n", depth2->cmd[0]);
		}
	}
	return ;
	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt + len) = '\0';
	token = strtok(fmt, seps);

	if(token != NULL) {
		cmd[count] = token;
		pstart = _prompt_tree[_prompt_index];
		while(pstart->name) {
			if(0 == memcmp(cmd[0], pstart->name, strlen(cmd[0]))) {
				find = 1;
				break;
			}
			pstart++;
		}

		//step 2:提取参数
		if(find == 1) {
			while(token != NULL) {
				cmd[count] = token;
				count++;
				token = strtok(NULL, seps);
			}
			if (pstart->fun) {
				pstart->fun(NULL, count, (char **)cmd);
			}
		}
		else {
			printf("%s: command not found\n", token);
		}
	}
}

int sh_enter_ex(struct sh_detach_depth *env)
{
	char shell_prompt[256];
	char *input = (char *)NULL;
	struct sh_detach_depth *penv;
	sh_sort();
	// sh_init();
	memcpy(g_envLocal.host, "MiniShell\0", 10);
	g_envLocal.path[0] = '\0';

	char *cmd[256];
	struct sh_detach_depth local;
	
	if (env) {
		penv = env;
	}
	else {
		local.cmd = cmd;
		local.len = 256;
		local.count = 0;
		local.seps = _seps;
		local.cmd[1] = "abcd";
		penv = &local;
	}
	// penv = &local;

	while(1) {

		if(input) {
			free(input);
			input = NULL;
		}

		snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s>", g_envLocal.host, g_envLocal.path);
		input = readline(shell_prompt);
		if(!input) {
			return -1;
		}
		if (*input != '\0') {
			add_history(input);
			sh_analyse_ex(input, strlen(input), penv);
			if(0 == strcmp(input, "quit")) {
				printf("\r\n");
				break;
			}
		}
	}

	return 0;
}
