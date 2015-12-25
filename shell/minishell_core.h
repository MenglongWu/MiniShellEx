/**
 ******************************************************************************
 * @file	
 * @brief	
 *		TODO:introduce 
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Menglong Wu		20xx-xx-xx		1.xxxxx
 *****************************************************************************
*/
#ifndef _MINI_SHELL_CORE_H_
#define _MINI_SHELL_CORE_H_

struct cmd_table
{
	char *name;
	// int (*fun)(char **argv,int argc);
	int (*fun)(int argc,char **argv);
	char *help;
};

struct env{
	char host[10];
};

extern struct env g_envLocal;
extern void sh_init();
extern void sh_enter();


#define W_BOOT_CMD(name,cmd,help) \
volatile struct cmd_table   __w_boot_cmd_##name __attribute__ ((unused,section (".w_boot_cmd"))) = {(char*)#name,cmd,(char*)help}


#endif

