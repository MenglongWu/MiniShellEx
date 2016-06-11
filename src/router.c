#include <stdio.h>
#include <minishell_core.h>

extern struct cmd_prompt boot_root[];
int main(int argc, char **argv)
{
	sh_whereboot(boot_root);

	struct sh_detach_depth depth;
	char *cmd[12];

	depth.cmd = cmd;
	depth.len = 12;
	depth.seps = " \t";


	
	char strout[100] = "interface fast 1";
	sh_analyse_ex(strout, 20, &depth, NULL);
	sh_enter_ex(&depth, NULL);
	return 0;
}