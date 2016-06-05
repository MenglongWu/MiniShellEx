#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <minishell_core.h>

#include <readline/readline.h>
#include "build.h"






int main (int argc , char **argv)
{
	struct group *head;

	head = xmlloadconfig("def.xml");
	if (head == NULL) {
		printf("open file\n");
		return 0;
	}
	disp_group(head);

	cfilesave("cmd_output.c", head);
	xmlsaveconfig("out.xml",head);
	return 0;
}