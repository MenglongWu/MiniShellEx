/**
 ******************************************************************************
 * @file	minishell_core.c
 * @brief	Menglong Wu\n
 	MiniShell 核心代码，提供命令行操作，不提供正则表达式

舍弃最初版本里用SHELL_REG_CMD定义数组cmd_tbl_list命令的定义，改成同uboot一样
用W_BOOT_CMD(name,fun,help)三个参数
 *
 @section History
|     Version    |     Author        |      Date      |    Content
| :------------: | :---------------: | :------------: | :------------
|     V1.0       |    Menglong Wu    |   2014-11-11   | 1.基础接口do_help、sh_init、sh_analyse、sh_enter
|     V1.1.0     |    Menglong Wu    |   2014-12-08   | 1.采用uboot的定义命令方式
|                |                   |                | 2.添加命令排序

 @section Platform
	-# Linux 2.6.35-22-generic #33-Ubuntu SMP Sun Sep 19 20:34:50 UTC 2010 i686 GNU/Linux
	-# gcc-4.7.4 gcc/g++
 @section Library
	-# [libreadline.so.5.2]
		-# [libhistory.so.5]
		-# [libncurses.so.5.5]

	-# [libpthread.so.0]
	-# [libc.so.6]
- 2015-4-3,Menglong Wu,MenglongWoo@alilyun.com
 	- Add sh_editpath
 	- Add
 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2>
*/


#ifdef __cplusplus
extern "C" {
#endif

#include "autoconfig.h"
#include "minishell_core.h"





#ifdef CONFIG_MINISHELL_READLINE
	#include "stdio.h"
	#include "malloc.h"
	#include <readline/readline.h>
	#include <readline/history.h>
	#include "shell.h"
#else
	#include "board.h"
	#include "linux/string.h"
	#include "shell.h"
#endif




extern struct cmd_table __w_boot_cmd_start, __w_boot_cmd_end;

#ifdef CMD_SECTION
volatile int __wcmd_start __attribute__ ((section (".w_boot_start"))) = 0;
W_BOOT_CMD(help, do_help, "shell help");
volatile int __wcmd_end   __attribute__ ((section (".w_boot_end"))) = 0;

#endif

//*****************************************************************************
//命令列表

#ifdef CMD_ARRAY
static struct cmd_table cmd_tbl_list[] = {
	// INIT_CMD define in shell/shell.h
	INIT_CMD,

	{(char *)"help", do_help, (char *)"shell help"},
	{(char *)"quit", do_null, (char *)"quit shell"},

	// SHELL_REG_CMD(0,0,0),
	{0, 0, 0},
};

#endif

struct env g_envLocal;


//*****************************************************************************
//默认命令
#if(1) //SHELL_REG_CMD宏才用到该函数
int do_null(int argc, char **argv)
{
	return 0;
}
#endif
/**
 * @brief	Mini Shell自带命令，输出命令帮助
 */


W_BOOT_CMD(null, do_null, "do nothing");
void sh_sort();
int do_help(int argc, char **argv)
{
	// sh_sort();

	struct cmd_table *pstart;//= (struct cmd_table*)((char*)&__wcmd_start+sizeof(int));
	struct cmd_table *pend;// = (struct cmd_table*)((char*)&__wcmd_end-sizeof(int));

	MINISHELL_START(pstart);
	MINISHELL_END(pend);


	// pstart = &__w_boot_cmd_start;
	while((pstart < pend)) {
		echo_printf("  %-12s\t", pstart->name);
		echo_printf("%s\r\n", pstart->help);
		pstart++;
	}
	return 0;
}
#ifdef CMD_LDS
W_BOOT_CMD(help, do_help, "shell help");
#endif

/**
 * @brief	Mini Shell自带命令，修改命令提示符名
 */
int do_hostname(int argc, char **argv)
{
	if(argc != 2) {
		puts("Usage:\n");
		puts("\thostname <name>\n");
	}
	else {
		int len;
		len = strlen(argv[1]);
		if (len > 10) {
			len = 10;
		}
		memcpy(g_envLocal.host, argv[1], len);
		g_envLocal.host[9] = '\0';
	}
	return 0;
}
W_BOOT_CMD(hostname, do_hostname, "set hostname");


//*****************************************************************************
//核心代码

/**
 * @brief	按照字母顺序排序编译后生成的Mini Shell命令
 */

void sh_sort()
{
	struct cmd_table *pstart;
	struct cmd_table *pend;
	struct cmd_table tmp, *pmin, *pfind;

	MINISHELL_START(pstart);
	MINISHELL_END(pend);



	while(pstart < pend) {
		pmin = pstart;

		for(pfind = pstart;  pfind < pend; pfind++) {
			if(strcmp(pmin->name, pfind->name) > 0) {
				pmin = pfind;
			}
		}

		tmp     = *pmin;
		*pmin    = *pstart;
		*pstart = tmp;
		pstart++;
	}
}

#ifdef CONFIG_MINISHELL_EX
/**
 * @brief	按照字母顺序排序编译后生成的Mini Shell命令
 */

void sh_sort_ex(struct cmd_prompt *cmdlist, int count)
{
	struct cmd_prompt *pstart;
	struct cmd_prompt *pend;
	struct cmd_prompt tmp, *pmin, *pfind;


	pstart = cmdlist;
	pend   = cmdlist + count - 1;


	// for(pstart;pstart < pend;pstart++) {
	while(pstart < pend) {
		pmin = pstart;

		for(pfind = pstart;  pfind < pend; pfind++) {
			if(strcmp(pmin->name, pfind->name) > 0) {
				pmin = pfind;
			}
		}

		tmp     = *pmin;
		*pmin    = *pstart;
		*pstart = tmp;
		pstart++;
	}
}
#endif


/**
 * @brief	Mini Shell分析命令，找到命令则执行
 * @param	fmt 若干个"  ,\t\n"分隔的字符串
 * @param	len fmt字符串长度
 * @remark	fmt中所有字符串总数不得超过30个
 */
void sh_analyse (char *fmt, long len)
{
	//char (*cmd)[10];
	char *cmd[256], *token = NULL;
	unsigned int count = 0;
	char seps[]   = " ,\t\n";

	struct cmd_table *pstart;
	struct cmd_table *pend;
	int find = 0;

	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt + len) = '\0';
	token = strtok(fmt, seps);

	if(token != NULL) {
		cmd[count] = token;

		MINISHELL_START(pstart);
		MINISHELL_END(pend);

		while((pstart < pend)) {
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
			pstart->fun(count, (char **)cmd);
		}
		else {
			echo_printf("%s: command not found\n", token);
		}
	}
}

void sh_editpath(char *path)
{
	int len;
	len = strlen(path);
	if (len > SHELL_PATH_LEN) {
		len = SHELL_PATH_LEN;
	}
	memcpy(g_envLocal.path, path, len);
	g_envLocal.path[len] = '\0';
	g_envLocal.path[SHELL_PATH_LEN - 1] = '\0';
}


/**
 * @brief	Mini Shell入口函数，完成一切初始化，
 * @remark	单行命令长度不得超过256字节
 */
#ifdef CONFIG_MINISHELL_READLINE

int sh_enter()
{
	char shell_prompt[256];
	char *input = (char *)NULL;
	sh_sort();
	// sh_init();
	memcpy(g_envLocal.host, "MiniShell\0", 10);
	g_envLocal.path[0] = '\0';


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
			sh_analyse(input, strlen(input));
			if(0 == strcmp(input, "quit")) {
				printf("\r\n");
				break;
			}
		}
	}

	return 0;
}
#else

int sh_enter()
{
	char input[256];
	sh_sort();
	// sh_init();
	memcpy(g_envLocal.host, "MiniShell\0", 10);

	while(1) {

		echo_printf("%s>", g_envLocal.host);
		gets_s(input, 256);
		sh_analyse(input, 256);

		if(0 == strcmp(input, "quit")) {
			echo_printf("\r\n");
			break;
		}
	}
	return 0;
}

#endif






#ifdef __cplusplus
}
#endif
