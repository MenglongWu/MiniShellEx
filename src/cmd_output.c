#include <minishell_core.h>

int do_show (void *ptr, int argc, char **argv);
int do_interface (void *ptr, int argc, char **argv);
int do_ip (void *ptr, int argc, char **argv);
int do_vlan (void *ptr, int argc, char **argv);
int do_Hostname (void *ptr, int argc, char **argv);
int do_show_run (void *ptr, int argc, char **argv);
int do_show_vlan (void *ptr, int argc, char **argv);
int do_fastetherne (void *ptr, int argc, char **argv);
int do_gethernet (void *ptr, int argc, char **argv);
int do_router (void *ptr, int argc, char **argv);
int do_address (void *ptr, int argc, char **argv);
int do_1 (void *ptr, int argc, char **argv);
int do_2 (void *ptr, int argc, char **argv);
int do_3 (void *ptr, int argc, char **argv);



struct cmd_prompt boot_root[];
struct cmd_prompt boot_show[];
struct cmd_prompt boot_interface[];
struct cmd_prompt boot_ip[];
struct cmd_prompt boot_vlan[];



struct cmd_prompt boot_root[] = {
	PROMPT_NODE(boot_show   ,      do_show ,
		 (char*)"show "  ,
		 (char*)"show something",
		 (int)  NULL),
	PROMPT_NODE(boot_interface   ,      do_interface ,
		 (char*)"interface "  ,
		 (char*)"enter interface",
		 (int)  NULL),
	PROMPT_NODE(boot_ip   ,      do_ip ,
		 (char*)"ip "  ,
		 (char*)"enter interface",
		 (int)  NULL),
	PROMPT_NODE(boot_vlan   ,      do_vlan ,
		 (char*)"vlan "  ,
		 (char*)"config vlan",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_Hostname ,
		 (char*)"Hostname "  ,
		 (char*)"config vlan",
		 (int)  NULL),
};
struct cmd_prompt boot_show[] = {
	PROMPT_NODE(NULL    ,      do_show_run ,
		 (char*)"run"  ,
		 (char*)"help 2",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_show_vlan ,
		 (char*)"vlan"  ,
		 (char*)"help 3",
		 (int)  NULL),
};
struct cmd_prompt boot_interface[] = {
	PROMPT_NODE(NULL    ,      do_fastetherne ,
		 (char*)"fastetherne "  ,
		 (char*)"help 2",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_gethernet ,
		 (char*)"gethernet "  ,
		 (char*)"help 3",
		 (int)  NULL),
};
struct cmd_prompt boot_ip[] = {
	PROMPT_NODE(NULL    ,      do_router ,
		 (char*)"router "  ,
		 (char*)"help 2",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_address ,
		 (char*)"address "  ,
		 (char*)"help 3",
		 (int)  NULL),
};
struct cmd_prompt boot_vlan[] = {
	PROMPT_NODE(NULL    ,      do_1 ,
		 (char*)"c31"  ,
		 (char*)NULL,
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_2 ,
		 (char*)"c321"  ,
		 (char*)NULL,
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_3 ,
		 (char*)"c323"  ,
		 (char*)NULL,
		 (int)  NULL),
};



int do_show (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_interface (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_ip (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_vlan (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_Hostname (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_show_run (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_show_vlan (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_fastetherne (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_gethernet (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_router (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_address (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_1 (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_2 (void *ptr, int argc, char **argv)
{
	return 0;
};

int do_3 (void *ptr, int argc, char **argv)
{
	return 0;
};




