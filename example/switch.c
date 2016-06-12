#include <stdio.h>
#include <minishell_core.h>

extern struct cmd_prompt boot_root[];

struct user_ptr
{
	char *name;
	int a;
};


int main(int argc, char **argv)
{
	sh_whereboot(boot_root);

	struct sh_detach_depth depth;
	char *cmd[12];

	depth.cmd = cmd;
	depth.len = 12;
	depth.seps = " \t";

	struct user_ptr upt;
	upt.name = "This is a minishell_core demo, modeled layer 3 Switch\n";
	upt.a = 1001;

	
	sh_enter_ex(&depth, &upt);
	return 0;
}