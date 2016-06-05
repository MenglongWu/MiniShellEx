#include "minishell_core.h"


struct cmd_table cmd_tbl_list[] = {

	{(char *)"help", do_help, (char *)"shell help"},
	{(char *)"quit", do_null, (char *)"quit shell"},
	{0, 0, 0},
};