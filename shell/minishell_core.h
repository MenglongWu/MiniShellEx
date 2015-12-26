/**
 ******************************************************************************
 * @file
 * @brief
 *		TODO:introduce
 *
|     Version    |     Author        |      Date      |    Content
| :------------: | :---------------: | :------------: | :------------
|     V1.0       |    Menglong Wu    |   2014-11-11   | 1.基础接口do_help、sh_init、sh_analyse、sh_enter
|     V1.1.0     |    Menglong Wu    |   2014-12-08   | 1.采用uboot的定义命令方式
|                |                   |                | 2.添加命令排序
 *****************************************************************************
*/

#ifndef _MINI_SHELL_CORE_H_
#define _MINI_SHELL_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef CONFIG_MINISHELL_READLINE
	#define echo_printf printf

#else
	#define echo_printf printk
#endif
	

#if !defined(CMD_ARRAY) && !defined(CMD_SECTION) && !defined(CMD_LDS)
	// #define CMD_LDS
	#define CMD_ARRAY
	// #define CMD_SECTION
#endif

#if defined(CMD_ARRAY) && (defined(CMD_SECTION) || defined(CMD_LDS))
#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"

#elif (defined(CMD_ARRAY) || defined(CMD_SECTION)) && defined(CMD_LDS)
#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"

#elif defined(CMD_SECTION) && (defined(CMD_ARRAY) || defined(CMD_LDS))
#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"

#endif


struct cmd_prompt {
	struct cmd_prompt	*next;
	int (*fun)(void *ptr, int argc, char **argv);
	char *name;
	char *help;
	int type;
#define CMDP_TYPE_STR  0
#define CMDP_TYPE_PASS 1
};


struct cmd_table {
	char *name;
	int (*fun)(int argc, char **argv);
	char *help;
};


#define SHELL_PATH_LEN (96)
struct env {
	char host[10];
	char path[SHELL_PATH_LEN];
};

struct sh_detach_depth
{
	char **cmd;
	int len;
	int count;
	char *seps;
};






extern struct env g_envLocal;

extern int do_help(int argc, char **argv);
extern int do_null(int argc, char **argv);
extern int do_hostname(int argc, char **argv);
extern int sh_enter();
extern void sh_editpath(char *path);
extern void sh_analyse (char *fmt, long len);
extern void sh_editpath(char *path);
extern void sh_sort();
extern void sh_sort_ex(struct cmd_prompt *cmdlist, int count);

extern int do_undo_ex(void *ptr, int argc, char **argv);

extern struct cmd_prompt *sh_down_prompt_level(
	struct cmd_prompt *level);
extern struct cmd_prompt *sh_up_prompt_level(void);
#ifdef CMD_SECTION
	extern volatile int __wcmd_start;
	extern volatile int __wcmd_end;
#endif


#define SH_FUN_NULL do_undo_ex
// #define PROMPT_NODE(a,b,c,d) {(char*)(a), (char*)(b), (struct cmd_prompt	*)(c), (int)(d)}
#ifndef NULL
#define NULL ((void*)0)
#endif
#define PROMPT_NODE(a,f,b,c,d) \
{ \
	.next = (struct cmd_prompt	*)(a), \
	.fun  = (int (*)(int argc, char **argv))f, \
	.name = (char*)(b), \
	.help = (char*)(c), \
	.type = (int)(d), \
}


// W_BOOT_CMD do what
#if defined(CMD_SECTION) || defined(CMD_LDS)
#define W_BOOT_CMD(name,cmd,help) \
	volatile struct cmd_table   __w_boot_cmd_##name __attribute__ ((unused,section (".w_boot_cmd"))) = {(char*)#name,cmd,(char*)help}

// CMD_ARRAY
#else
#define W_BOOT_CMD(name,cmd,help)
#endif





//MINISHELL_START\MINISHELL_END
#ifdef CMD_ARRAY
	#define MINISHELL_START(p) (p = &cmd_tbl_list[0])
	#define MINISHELL_END(p) (p = &cmd_tbl_list[0] + sizeof(cmd_tbl_list)/sizeof(struct cmd_table) - 1)
#endif


#ifdef CMD_SECTION
	#define MINISHELL_START(p) (p = (struct cmd_table*)((char*)&__wcmd_start+sizeof(int)))
	#define MINISHELL_END(p) (p = (struct cmd_table*)((char*)&__wcmd_end  -sizeof(int)))
#endif

#ifdef CMD_LDS //define __w_boot_cmd_start\__w_boot_cmd_end in *.lds file
	#define MINISHELL_START(p) (p = &__w_boot_cmd_start)
	#define MINISHELL_END(p)   (p = &__w_boot_cmd_end)
#endif


#ifdef __cplusplus
}
#endif

#endif

