#include <minishell_core.h>

#include <stdio.h>

static int do_show(void *ptr, int argc, char **argv);
static int do_interface(void *ptr, int argc, char **argv);
static int do_vlan(void *ptr, int argc, char **argv);
static int do_hostname(void *ptr, int argc, char **argv);
static int do_quit_system(void *ptr, int argc, char **argv);
static int do_show_run(void *ptr, int argc, char **argv);
static int do_show_vlan(void *ptr, int argc, char **argv);
static int do_show_interface(void *ptr, int argc, char **argv);
static int do_set_hostname(void *ptr, int argc, char **argv);
static int do_fastethernet(void *ptr, int argc, char **argv);
static int do_gigabitethernet(void *ptr, int argc, char **argv);
static int do_arp(void *ptr, int argc, char **argv);
static int do_flow(void *ptr, int argc, char **argv);
static int do_ip(void *ptr, int argc, char **argv);
static int do_quit(void *ptr, int argc, char **argv);
static int do_detection(void *ptr, int argc, char **argv);
static int do_filter(void *ptr, int argc, char **argv);
static int do_max_learning_num(void *ptr, int argc, char **argv);
static int do_trust(void *ptr, int argc, char **argv);
static int do_on(void *ptr, int argc, char **argv);
static int do_off(void *ptr, int argc, char **argv);
static int do_unuse(void *ptr, int argc, char **argv);
static int do_address(void *ptr, int argc, char **argv);
static int do_gw(void *ptr, int argc, char **argv);
static int do_mac(void *ptr, int argc, char **argv);



struct cmd_prompt boot_root[];
struct cmd_prompt boot_show[];
struct cmd_prompt boot_hostname[];
struct cmd_prompt boot_interface[];
struct cmd_prompt boot_fastethernet[];
struct cmd_prompt boot_gigabitethernet[];
struct cmd_prompt boot_vlan[];
struct cmd_prompt boot_interface_root[];
struct cmd_prompt boot_arp[];
struct cmd_prompt boot_detection[];
struct cmd_prompt boot_filter[];
struct cmd_prompt boot_max_learning_num[];
struct cmd_prompt boot_flow[];
struct cmd_prompt boot_interface_ip[];
struct cmd_prompt boot_ip_addr[];



struct cmd_prompt boot_root[] = {
	PROMPT_NODE(boot_show   ,      do_show,
		 (char*)"show"  ,
		 (char*)"show something",
		 (int)  NULL),
	PROMPT_NODE(boot_interface   ,      do_interface,
		 (char*)"interface"  ,
		 (char*)"enter interface",
		 (int)  NULL),
	PROMPT_NODE(boot_vlan   ,      do_vlan,
		 (char*)"vlan"  ,
		 (char*)"Configure VLAN",
		 (int)  NULL),
	PROMPT_NODE(boot_hostname   ,      do_hostname,
		 (char*)"hostname"  ,
		 (char*)"Config Hostname",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_quit_system,
		 (char*)"quit"  ,
		 (char*)"Exit from current command view",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_show[] = {
	PROMPT_NODE(NULL    ,      do_show_run,
		 (char*)"run"  ,
		 (char*)"Current configuration ",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_show_vlan,
		 (char*)"vlan"  ,
		 (char*)"VLAN configuration information",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_show_interface,
		 (char*)"interface"  ,
		 (char*)"Status and configuration information for the interface ",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_hostname[] = {
	PROMPT_NODE(NULL    ,      do_set_hostname,
		 (char*)"name"  ,
		 (char*)"Set hostname",
		 (int)  CMDP_TYPE_PASS),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_interface[] = {
	PROMPT_NODE(boot_fastethernet   ,      do_fastethernet,
		 (char*)"fastethernet"  ,
		 (char*)"FastEthernet interface",
		 (int)  NULL),
	PROMPT_NODE(boot_gigabitethernet   ,      do_gigabitethernet,
		 (char*)"gigabitethernet"  ,
		 (char*)"GigabitEthernet interface",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_fastethernet[] = {
	PROMPT_NODE(NULL    ,      NULL,
		 (char*)"[1-10]"  ,
		 (char*)"ethernet port interface",
		 (int)  CMDP_TYPE_PASS),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_gigabitethernet[] = {
	PROMPT_NODE(NULL    ,      NULL,
		 (char*)"[11-12]"  ,
		 (char*)"ethernet port interface",
		 (int)  CMDP_TYPE_PASS),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_vlan[] = {
	PROMPT_NODE(NULL    ,      NULL,
		 (char*)"[1-1024]"  ,
		 (char*)"VLAN interface",
		 (int)  CMDP_TYPE_PASS),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_interface_root[] = {
	PROMPT_NODE(boot_arp   ,      do_arp,
		 (char*)"arp"  ,
		 (char*)"Configure ARP for the interface ",
		 (int)  NULL),
	PROMPT_NODE(boot_flow   ,      do_flow,
		 (char*)"flow"  ,
		 (char*)"Flow control command",
		 (int)  NULL),
	PROMPT_NODE(boot_interface_ip   ,      do_ip,
		 (char*)"ip"  ,
		 (char*)"Specify IP configurations for the system",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_quit,
		 (char*)"quit"  ,
		 (char*)"Exit from current command view",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_arp[] = {
	PROMPT_NODE(boot_detection   ,      do_detection,
		 (char*)"detection"  ,
		 (char*)"Configure ARP for the interface",
		 (int)  NULL),
	PROMPT_NODE(boot_filter   ,      do_filter,
		 (char*)"filter"  ,
		 (char*)"Filter ARP packets",
		 (int)  NULL),
	PROMPT_NODE(boot_max_learning_num   ,      do_max_learning_num,
		 (char*)"max-learn"  ,
		 (char*)"Set the maximum number of dynamic arp entries learned on the interface",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_detection[] = {
	PROMPT_NODE(NULL    ,      do_trust,
		 (char*)"trust"  ,
		 (char*)"Specify port trust state",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_filter[] = {
	PROMPT_NODE(NULL    ,      do_on,
		 (char*)"on"  ,
		 (char*)"turn on arp filter",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_off,
		 (char*)"off"  ,
		 (char*)"turn off arp filter",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_max_learning_num[] = {
	PROMPT_NODE(NULL    ,      do_unuse,
		 (char*)"INTEGER[0-256]"  ,
		 (char*)"Maximum number of dynamic arp entries learned on the interface",
		 (int)  CMDP_TYPE_PASS),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_flow[] = {
	PROMPT_NODE(NULL    ,      do_on,
		 (char*)"on"  ,
		 (char*)"turn on flow",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      do_off,
		 (char*)"off"  ,
		 (char*)"turn off flow",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_interface_ip[] = {
	PROMPT_NODE(boot_ip_addr   ,      do_address,
		 (char*)"address"  ,
		 (char*)"IP address",
		 (int)  NULL),
	PROMPT_NODE(boot_ip_addr   ,      do_gw,
		 (char*)"gw"  ,
		 (char*)"Gate way",
		 (int)  NULL),
	PROMPT_NODE(boot_ip_addr   ,      do_mac,
		 (char*)"mac"  ,
		 (char*)"MAC",
		 (int)  NULL),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};
struct cmd_prompt boot_ip_addr[] = {
	PROMPT_NODE(NULL    ,      do_unuse,
		 (char*)"x.x.x.x"  ,
		 (char*)"IP address",
		 (int)  CMDP_TYPE_PASS),
	PROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),
};



static int do_show(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_interface(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_vlan(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_hostname(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_quit_system(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_show_run(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_show_vlan(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_show_interface(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_set_hostname(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_fastethernet(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_gigabitethernet(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_arp(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_flow(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_ip(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_quit(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_detection(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_filter(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_max_learning_num(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_trust(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_on(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_off(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_unuse(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_address(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_gw(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

static int do_mac(void *ptr, int argc, char **argv)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}




