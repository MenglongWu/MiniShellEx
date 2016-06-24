
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
#include <readline/rltypedefs.h>
const char _seps[] = " ,\t\n";

// #define MINISHELL_DBG
int do_undo_ex(void *ptr, int argc, char **argv)
{
	return 0;
}

// void sh_init_env()
// {

// }
/*
	detach string and auto completion
*/
void sh_detach_fmt_ac (char *fmt,long len, char **cmd, int *count)
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

/*
	忘记下面的代码是用来调试上面东西的了
*/
void sh_detach_xx_fmt (char *fmt,long len, struct sh_detach_depth *depth)
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

int sh_check_match(struct cmd_prompt *pprompt, char *text,int *index);
void sh_display_prompt(struct cmd_prompt *pprompt);
void sh_display_match(struct cmd_prompt *plist, char *text);
int sh_completion_head(struct cmd_prompt *plist, char *text,
	int index,int *start, int *end);
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

// 检查有多少个匹配
/**
 * @brief	检查有多少个匹配，条件是pprompt里的内容是按字母顺序排列的
 * @param	pprompt 检查列表
 * @param	text 检查对象
 * @param	index 输出值，从pprompt第几个开始有匹配
 * @retval	0 没有匹配
 * @retval	1 仅有1个匹配
 * @retval	2 有 >= 2个匹配
 */

int sh_check_match(struct cmd_prompt *pprompt, char *text,int  *index)
{
	struct cmd_prompt *ptprompt;
	int len, ret;
	int count = 0;

	ptprompt = pprompt;
	len = strlen(text);
	


	// 找到第一个匹配的
	while( ptprompt->name ) {
		ret = memcmp(ptprompt->name, text, len);
		if (ret == 0) {
			count++;
			goto _FindSecend;
		}
		ptprompt++;
	}
	return 0;

_FindSecend:;
	// 记录匹配个数
	*index = (ptprompt - pprompt);
	ptprompt++;// 移动到下一个
	while( ptprompt->name ) {
		
		ret = memcmp(ptprompt->name, text, len);
		if (ret == 0) {
			return 2;
		}
		else {
			break;
		}
		ptprompt++;
	}	
	return 1;
}
// 列出半匹配字符串
void sh_display_match(struct cmd_prompt *pprompt, char *text)
{
	struct cmd_prompt *ptprompt;
	int len, ret;

	ptprompt = pprompt;
	len = strlen(text);
	printf("\n");
	while( ptprompt->name ) {
		ret = memcmp(ptprompt->name, text, len);
		if (ret == 0) {
#ifdef MINISHELL_DBG
			printf("ret %d %s\n",ret, ptprompt->name);
#else
			printf("%s\n",ptprompt->name);
#endif
		}
		ptprompt++;
	}
	// return 0;
}

// start end返回从start到end需要插入的字符串
int sh_completion_head(struct cmd_prompt *pprompt, char *text,
	int index, int *start, int *end)
{
	struct cmd_prompt *ptprompt, *ptprompt_next;
	int len;

	ptprompt = pprompt + index;
	ptprompt_next = pprompt + index + 1;
	len = strlen(text);
#ifdef MINISHELL_DBG
	printf("\n");
#endif
	// int i = 0;
#ifdef MINISHELL_DBG	
	printf("next  %s \n", ptprompt->name);
#endif
	char *ps,*ps_next;
	ps = ptprompt->name;
	while(*ps == *text && len) {
#ifdef MINISHELL_DBG
		printf("%c %c", *ps, *text);
#endif
		len--;
		ps++;
		text++;
	}

	*start = ps - ptprompt->name;
	ps = ptprompt->name + *start;
	if (ptprompt_next->name == NULL) {
		return 0;
	}
	ps_next = ptprompt_next->name + *start;


	while(*ps == *ps_next) {
#ifdef MINISHELL_DBG
		printf("%c %c", *ps++, *ps_next++);
#endif
		ps++;
		ps_next++;
	}
	*end = ps - ptprompt->name;
#ifdef MINISHELL_DBG	
	printf("sta %d end %d\n", *start, *end);
#endif


	return 0;
}

void sh_list(char *text, int len, struct cmd_prompt *boot)
{

}




/*
    _prompt_tree look as a stack structue.Stack len is PROMPT_DEPTH
    
       _prompt_tree
       --------------
       |PROMPT_DEPTH|
       -------------- 
       |            |     |--------------------->  boot_2
       --------------     |                        |
       |            |     |                        |-- cmd21
       --------------     |                        |-- cmd22
       |     3      |     |                        |-- cmd23
       --------------     |                        |-- (null)
       |     2      |     |    |-------> boot_1
       --------------     |    |         |
   |-> |     1      |  ---|    |         |-- cmd11
   |   ------------ -          |         |-- cmd12
   |   |     0      |  --------|         |-- cmd13
   |   --------------                    |-- cmd14
   |                                     |-- (null)
   |   _prompt_index

	_prompt_index determine the minishell core search where.
	boot_n is a "struct cmd_prompt" array, the end with all null at the arrary last element.
	The "name" element is the command name, such as "cmd11" "cmd12".
	When user press Enter, the minishell will search curent boot_n array.
	If found "name" match string "first word" then call "func" element else undo any thing.

Example:
	when _prompt_index = 0 input string :
		1. "cmd11 opt1 opt2 opt3"
		2. "cmd12 opt1 opt2 opt3"
		3. "cmd21 opt1 opt2 opt3"

		command "cmd11" and "cmd12" can be found,but "cmd21" can't be.

	when _prompt_index = 1 input string the same

		command "cmd21" can be found,but "cmd11" and "cmd12" can't be.
 */
#define PROMPT_DEPTH (16)

/*
when call sh_whereboot() could be set _prompt_tree[0] point new "boot_new"
boot_unuse only be use for avoid "Segmentation fault" and reminded developer call it.
*/
static struct cmd_prompt boot_unuse[] = {
	PROMPT_NODE(NULL    ,      NULL,
	(char *)"must call sh_whereboot()",
	(char *)NULL,
	(int)  NULL),
};
struct cmd_prompt *_prompt_tree[PROMPT_DEPTH] = {boot_unuse};
int                _prompt_index = 0;

/**
 * @brief	push new boot in prompt
 * @param	null
 * @retval	not zero success
 * @retval	NULL the stack is full
 */
struct cmd_prompt *sh_down_prompt_level(
    struct cmd_prompt *level)
{
	if (_prompt_index >= (PROMPT_DEPTH - 2) ) {
		return NULL;
	}
	_prompt_tree[ ++_prompt_index ] = level;
	return _prompt_tree[_prompt_index - 1];
}

/**
 * @brief	pop the top boot level
 * @param	null
 * @retval	not zero success
 * @retval	NULL the stack at the end
 */
struct cmd_prompt *sh_up_prompt_level(void)
{
	if (_prompt_index == 0 ) {
		return NULL;
	}
	_prompt_index--;
	return _prompt_tree[_prompt_index+1];
}


extern int rl_line_buffer_len, rl_end;
extern char *rl_display_prompt ;

/*
	the function must be calld
	core know from which "struct cmd_prompt" start with.
	and then call sh_enter_ex
*/
void sh_whereboot(struct cmd_prompt *boot_new)
{
	_prompt_tree[0] = &boot_new[0];
	_prompt_index    = 0;	
}
#if 0
// this compile have warning !!!
static int _dk_listmatch(int key, rl_command_func_t function);
static int _dk_autocompletion(int key, rl_command_func_t function);
#endif

static int _dk_listmatch(int key, int function);
static int _dk_autocompletion(int key, int function);
/*
	bind default key process 
	maybe after allow user change key map
*/
int def_keybind()
{
	rl_bind_key('?',_dk_listmatch);
	rl_bind_key('\t',_dk_autocompletion);
	return 0;
}	


/*
	default key program process
	press key '?'
	list may be match string
*/
static int _dk_listmatch(int key, int function)
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


	sh_detach_fmt_ac(pbuf, len, cmd, &count);
	
	struct cmd_prompt *plist;
	int ret;
#ifdef MINISHELL_DBG
	printf("index %d %d %d\n", _prompt_index, count, len);
#endif

	ret = searchboot(count, cmd, _prompt_tree[_prompt_index], &plist);

	// printf("ret = %d\n", ret);
	if (ret == 0 || ret == 1) {
		// sh_display_prompt(plist);
		printf("\n");
		sh_display_prompt(plist);

	}
	else if (ret == 2){

		printf("\n\t<cr>        Enter \n");
	}
#ifdef MINISHELL_USE_MALLOC
	free(pbuf);
#endif
	printf("%s%s", rl_prompt, rl_line_buffer);
	return 0;
}	

/*
	default key program process
	press key 'tab'
	auto completion string which match prefix string
*/
static int _dk_autocompletion(int key, int function)
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

	/*
	 split input string,such as input is "str1 str2 str3"
	 cmd[0] = str1,  cmd[1] = str1,  cmd[2] = str3,
	 count = 3
	 max split beyond array cmd length
	 */
	int last = strlen(rl_line_buffer);

	sh_detach_fmt_ac(pbuf, len, cmd, &count);

	/*
		if last word is end with ' '
	*/
	if (rl_line_buffer[last-1] == ' ') {
		int lastlen;
		char *p;

		lastlen = strlen(cmd[count - 1]);
		p = cmd[count-1];
		p[lastlen] = ' ';
		p[lastlen +1] = '\0';
	}

	
	struct cmd_prompt *plist;
	int ret;
#ifdef MINISHELL_DBG
	printf("index %d %d %d\n", _prompt_index, count, len);
#endif
	ret = searchboot(count - 1, cmd, _prompt_tree[_prompt_index], &plist);

	if (ret == 0 || ret == 1) {
		// todo 自动填充
		// sh_display_prompt(plist);
#ifdef MINISHELL_DBG
		printf("cmd %s %s\n", cmd[count - 1], plist->name);
#endif

		int index;
		ret = sh_check_match(plist, cmd[count - 1], &index);
#ifdef MINISHELL_DBG
		printf("match count %d from %d\n", ret, index);
#endif
		int start, end;

		switch(ret) {
		case 0:
			break;
		case 1:// only 1 match
			printf("\n");
			sh_completion_head(plist, cmd[count - 1],
				index, &start, &end);
			{
			char strout[24];
			char *ps;
			struct cmd_prompt *ptprompt;
			ptprompt = plist + index;
			ps = ptprompt->name;
			end = strlen(ps) + 1;
#ifdef MINISHELL_DBG
			printf("ps %s %d %d\n", ps, start, end );
#endif
			memcpy(strout,ps + start, end - start);
			strout[end - start] = '\0';
			printf("%s%s", rl_prompt, rl_line_buffer);
			rl_insert_text(strout);
			// add a ' ' at the last word,make sure next press TAB don't match this
			rl_insert_text(" ");					
			}
			break;
		case 2:// more then 1 match
			printf("\n");
			sh_completion_head(plist, cmd[count - 1],
				index, &start, &end);
			char strout[24];
			char *ps;
			struct cmd_prompt *ptprompt;
			ptprompt = plist + index;
			ps = ptprompt->name;
			memcpy(strout,ps + start, end - start);
			strout[end - start] = '\0';
			printf("%s%s", rl_prompt, rl_line_buffer);
			sh_display_match(plist, cmd[count - 1]);
			rl_insert_text(strout);

			// printf(" auto after [%s]\n", rl_line_buffer);
			printf("%s%s", rl_prompt, rl_line_buffer);
			break;
		default:
			break;
		}
		

		
	}
	else if (ret == 2){
		// printf("\t<cr>        Enter \n");
	}
#ifdef MINISHELL_USE_MALLOC
	free(pbuf);
#endif
	// printf("%s%s", rl_prompt, rl_line_buffer);
	return 0;
}

int sh_sethostname(char *name)
{

	int len;
	len = strlen(name);
	if (len > 10) {
		len = 10;
	}
	memcpy(g_envLocal.host, name, len);
	g_envLocal.host[len] = '\0';
	g_envLocal.host[9] = '\0';
	printf("g_envLocal.host %s\n", g_envLocal.host);
	return 0;
}


void sh_analyse_ex (char *fmt, long len, struct sh_detach_depth *depth2, void *ptr)
{
	//char (*cmd)[10];
	char *cmd[256], *token = NULL;
	unsigned int count = 0;
	char seps[]   = " ,\t\n";

	// struct cmd_table *pstart;
	// struct cmd_table *pend;
	struct cmd_prompt *pstart;
	int find = 0;

	// sh_detach_fmt_ac(fmt,len,cmd,&count);
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
	// sh_detach_xx_fmt(fmt,len, &depth2);
	sh_detach_xx_fmt(fmt,len, depth2);
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
				// for (int i = 0; i < depth2->count;i++) {
				// 	printf("%d %s \n", i, depth2->cmd[i]);
				// }
				// pstart->fun(NULL, depth2->count, (char **)cmd);
				// pstart->fun(NULL, depth2->count, (char **)&depth2->cmd[0]);
				pstart->fun(ptr, depth2->count, (char **)&depth2->cmd[0]);
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

int sh_enter_ex(struct sh_detach_depth *env, void *ptr)
{
	char shell_prompt[256];
	char *input = (char *)NULL;
	struct sh_detach_depth *penv;

	def_keybind();
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
		local.seps = (char*)_seps;
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
			sh_analyse_ex(input, strlen(input), penv, ptr);
			// if(0 == strcmp(input, "quit")) {
			// 	printf("\r\n");
			// 	break;
			// }
		}
	}

	return 0;
}
