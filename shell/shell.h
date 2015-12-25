#ifndef _MINI_SHELL_H_
#define _MINI_SHELL_H_

// int do_setenv(char **argv,int argc);
// int do_saveenv(char **argv,int argc);
// int do_help(int argc,char **argv);
// int do_null(int argc,char **argv);
// int do_hostname(int argc,char **argv);

// extern int do_sector(int argc,char **argv);
// extern int do_erase(int argc,char **argv);
// extern int do_writepage(int argc,char **argv);
// extern int do_readpage(int argc,char **argv);
// extern int do_nandscan(int argc,char **argv);

// extern int do_format(int argc,char **argv);
// extern int do_ls(int argc,char **argv);
// extern int do_mkdir(int argc,char **argv);
// extern int do_rm(int argc,char **argv);
// extern int do_wfile(int argc,char **argv);
// extern int do_rfile(int argc,char **argv);

// extern int do_copy_sdram2nand(int argc,char **argv);
// extern int do_file2nand(int argc,char **argv);
// extern int do_mount(int argc,char **argv);

#define INIT_CMD \
	{(char*)"cmd1",do_null,(char*)"do nothing"}, \
	{(char*)"cmd1",do_null,(char*)"do nothing"}, \
	{(char*)"cmd2",do_null,(char*)"do nothing"}
	
#endif

