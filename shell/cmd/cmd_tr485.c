#ifdef __cplusplus
extern "C" {
#endif

#include "minishell_core.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

int cmd_Tr485(int argc,char **argv)
{
	if (argc == 1){
		printf("Usage:\n");
		printf("\ttr485 <inf>\n");
		printf("\ttr485 <data> <user data>\n");
		printf("\ttr485 <data> -f <file path>\n");
		printf("\ttr485 <token> <addr>\n");
		printf("\ttr485 <name> <local name>\n");
		printf("\ttr485 <name> <name>\n");
	}
	if (argc >= 2) {
		int fd;
		fd = atoi(argv[1]);	
		send(fd, "abcd" , 5, 0);
	}
	

	
	return 0;
}
W_BOOT_CMD(tr485,cmd_Tr485,"cmd tr485");

#ifdef __cplusplus
}
#endif