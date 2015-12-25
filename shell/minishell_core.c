/**
 ******************************************************************************
 * @file	minishell_core.c
 * @brief	
 *		MiniShell 核心代码，提供命令行操作，不提供正则表达式

 	舍弃最初版本里用SHELL_REG_CMD定义数组cmd_tbl_list命令的定义，改成同uboot一样
 	用W_BOOT_CMD(name,fun,help)三个参数
*/
/*
 *--------------------------------------------------
 * version	|    author    |    date    |    content
 * V1.0			Menglong Wu		2014-11-11	1.基础接口do_help、sh_init、sh_analyse、sh_enter
 * V1.0.1		Menglong Wu		2014-12-08	1.采用uboot的定义命令方式
 											2.添加命令排序
 *****************************************************************************
*/

 
 
#include "minishell_core.h"
#include "board.h"
#include "linux/string.h"
#include "shell.h"

// int do_help(int argc,char **argv);
extern struct cmd_table __w_boot_cmd_start,__w_boot_cmd_end;
//*****************************************************************************
//命令列表

//static
//  struct cmd_table cmd_tbl_list[] = 
// {
// 	// INIT_CMD
// 	// SHELL_REG_CMD(0,0,0)
// 	{"abc",do_help,"fffff"},
// 	{"ddf",do_help,"sssss"},
// 	{0,0,0},
// };

struct env g_envLocal;


//*****************************************************************************
//默认命令
#if(1) //SHELL_REG_CMD宏才用到该函数
int do_null(int argc,char **argv)
{
	return 0;
}
#endif
/**
 * @brief	Mini Shell自带命令，输出命令帮助
 */


W_BOOT_CMD(null,do_null,"shell help");

int do_help(int argc,char **argv)
{


	struct cmd_table *pstart;
	struct cmd_table *pend;

	pstart = &__w_boot_cmd_start;
	pend   = &__w_boot_cmd_end;


	// printk("  %8.8x %s\n",&pstart->name,pstart->name);
	// pstart = &__w_boot_cmd_null;
	// printk("%s\n",cmd_tbl_list[0].name);
	// printk("%s\n",cmd_tbl_list[1].name);
	// printk("  %8.8x %s\n",&pstart->name,pstart->name);
	// pstart++;
	// printk("  %8.8x %s\n",&pstart->name,pstart->name);
	// pstart++;
	// printk("  %8.8x %s\n",&pstart->name,pstart->name);

	
	// pstart = &__w_boot_cmd_start;
	while((pstart < pend)) {
		printk("  %-12s\t",pstart->name);
		printk("%s\r\n",pstart->help);
		pstart++;
	}
	return 0;
}


/**
 * @brief	Mini Shell自带命令，修改命令提示符名
 */
int do_hostname(int argc,char **argv)
{
	if(argc != 2) {
	 	puts("Usage:\n");
	 	puts("\thostname <name>\n");
	}
	else {
		memcpy(g_envLocal.host,argv[1],10);
		g_envLocal.host[9] = '\0';
	}
	return 0;	
}



//*****************************************************************************
//核心代码
#if(0)
void sh_init()
{
	struct cmd_table *pstart;
	struct cmd_table *pend;

	pstart = &__w_boot_cmd_start;
	pend   = &__w_boot_cmd_end;
	
}
#endif

/**
 * @brief	按照字母顺序排序编译后生成的Mini Shell命令
 */

void sh_sort()
{
	struct cmd_table *pstart;
	struct cmd_table *pend;
	struct cmd_table tmp,*pmin,*pfind;
	
	pstart = &__w_boot_cmd_start;
	pend = &__w_boot_cmd_end;
	

	
	// for(pstart;pstart < pend;pstart++) {
	while(pstart < pend) {
		pmin = pstart;

		for(pfind = pstart;  pfind < pend;pfind++) {
			if(strcmp(pmin->name,pfind->name) > 0) {
				pmin = pfind;
			}
		}
		tmp     = *pmin;
		*pmin    = *pstart;
		*pstart = tmp;

		pstart++;
	}	
}



/**
 * @brief	Mini Shell分析命令，找到命令则执行
 * @param	fmt 若干个"  ,\t\n"分隔的字符串
 * @param	len fmt字符串长度
 * @remark	fmt中所有字符串总数不得超过30个
 */


void sh_analyse (char *fmt,long len)
{
	//char (*cmd)[10];
	char *cmd[30],*token = NULL;	
	unsigned int count = 0;
	char seps[]   = " ,\t\n";

	struct cmd_table *pstart;
	struct cmd_table *pend;
	int find = 0;

	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt+len) = '\0';
	token = strtok(fmt,seps);
	
	if(token != NULL) {
		cmd[count] = token;
		// pstart = &cmd_tbl_list[0];
		// pend = pstart + sizeof(cmd_tbl_list) / sizeof(struct cmd_table) - 1;
		pstart = &__w_boot_cmd_start;
		pend = &__w_boot_cmd_end;


		// pstart = &__w_boot_cmd_null;
		while((pstart < pend)) {
			if(0 == strcmp(cmd[0],pstart->name)) {
				find = 1;
				break;
			}
			pstart++;
		}

		//step 2:提取参数
		if(find == 1) 
		{
			while(token != NULL) {
				cmd[count] = token;
				count++;
				token = strtok(NULL,seps);	
			}
			pstart->fun(count,(char**)cmd);
		}
		else {
			printk("%s: command not found\n",token);
		}
	}
}



/**
 * @brief	Mini Shell入口函数，完成一切初始化，
 * @remark	单行命令长度不得超过256字节
 */

void sh_enter()
{
	char input[256];
	// sh_sort();
	//sh_init();
	memcpy(g_envLocal.host,"MiniShell\0",10);
	while(1) 
	{

		printk("%s>",g_envLocal.host);
		// printk("%s>","shell");
		gets_s(input,256);
		

		sh_analyse(input,256);
		
		if(0 == strcmp(input,"quit")) {
			printk("\r\n");
			break;
		}
	}
}


W_BOOT_CMD(help,do_help,"shell help");
W_BOOT_CMD(hostname,do_hostname,"set hostname");

