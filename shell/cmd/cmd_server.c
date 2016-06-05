/**
 ******************************************************************************
 * @file	cmd_server.c
 * @brief	
 *		epoll server 控制台命令

 	命令基于minishell编写，命令函数原型全部是int (*fun)(int argc,char **argv)
 	格式
 *
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		Menglong Wu		2015-1-6	
 ******************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "minishell_core.h"
#include "epollserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "signal.h"


extern struct ep_t ep;

static int sg_canbreak = 1;

void sig_break(int sig)
{
	sg_canbreak = 1;
	printf("Input cmd \"unhook\" enable Ctrl+C\n");
}
void runserver()
{
	
}

void stopserver()
{
	for(int i = 0;i < 40;++i) {
		//free(p[i]);
	}
}

/**
 * @brief	服务器启动、关闭
 * @section SYNOPSIS
 *	server <arg1> [port]
 * @section DESCRIPTION
 *	- arg1
 *		- run: 运行服务器
 *		- stop: 停止运行
 *	- port
 *		- 默认使用EP_DEF_PORT(6000)端口
 */
 
int cmd_Server(int argc,char **argv)
{
	// if (argc == 0) {
	// 	printf("run..\n");
	// 	ep_Listen(&ep,0);
	// }
	// else if(argc == 1) {
	// 	printf("stop..\n");
	// 	ep_StopServer(&ep);
	// }
	
	// return 0;
	if(argc == 2) {
		if(strcmp(argv[1],"run") == 0) {
			printf("run..\n");
#ifdef TARGET_ARMV7
			ep_Listen(&ep,6001);
#else
			ep_Listen(&ep,6000);
#endif
			ep_RunServer(&ep);
			// runserver();

		}
		else if(strcmp(argv[1],"stop") == 0) {
			printf("stop..\n");
			ep_StopServer(&ep);
			// stopserver();
		}
	}
	return 0;
}
W_BOOT_CMD(server,cmd_Server,"run server");

/**
 * @brief	服务器启动、关闭，
  - SYNOPSIS
 *	@arg connect <ip> [port]
 - ip
 * @arg 
 * @arg stop: 停止运行
 - port
 * @arg 默认使用EP_DEF_PORT(6000)端口

 */
 int cmd_Connect(int argc,char **argv)
{
	struct ep_con_t client;
	char *pstrAddr;
	unsigned short port;

	// goto _Next;
	if (argc < 3) {
		printf("Usage:\n");
		printf("\tconnect <IP> <port>\n");
		return 0;
	}
// _Next:
	// printf("connect\n");
	// return 0;
	pstrAddr = argv[1];
	port     = (unsigned short)atoi(argv[2]);

	printf("Request connect %s:%d\n",pstrAddr,port);
	if (0 == ep_Connect(&ep,&client, pstrAddr, port)) {
	// if (0 == ep_Connect(&ep,&client, "127.0.0.1", 6000)) {
		printf("client %s:%d\n", 
					inet_ntoa(client.loc_addr.sin_addr),
					htons(client.loc_addr.sin_port));	
	}
	return 0;
}
W_BOOT_CMD(connect,cmd_Connect,"connect server");

int cmd_Close(int argc,char **argv)
{

	if (argc == 2) {
		int fd = atoi(argv[1]);
		ep_Close(&ep,NULL,fd);
	}
	return 0;
}

W_BOOT_CMD(close,cmd_Close,"close socket");


int cmd_CreateWorkThread(int argc,char **argv)
{
		
	return 0;
}
W_BOOT_CMD(cwt,cmd_CreateWorkThread,"create work thread");

#include "./protocol/tmsxx.h"
int cmd_epsend(int argc,char **argv)
{
	char sbuf[1024];
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
		
		
		sg_canbreak = 0;
		fd = atoi(argv[1]);
		memset(sbuf, 1, 1024);
		signal(SIGINT, sig_break);
		while(sg_canbreak == 0) {

			// struct tms_dev_base devBase[1];
			// devBase[0].solt_num = 0xaabbcc01;
			// devBase[0].dev_type = 0xaabbcc02;
			// devBase[0].port_num = 0xaabbcc03;
			// // tms_DeviceComposition(fd, 1, devBase);
			// // tms_GetSerialNumber(fd);
			// char sn[128] = {1,2,3,4,5,6,7,8,9,10};
			// tms_SerialNumber(fd, sn);



			// break;
			for (int i = 0;i < 10; i++) {
				send(fd, "abcdefg\n" , 8, 0);
			}
			// printf("send off\n");

			sleep(1);
		}
		
	}
	

	
	return 0;
}
W_BOOT_CMD(epsend,cmd_epsend,"cmd epoll server send");

int cmd_unhook(int argc,char **argv)
{
	signal(SIGINT, NULL);
}
W_BOOT_CMD(unhook,cmd_unhook,"cmd epoll server send");
/**
 * @brief	服务器启动、关闭，
 * @pa la kkeew
 * @secnn la kkeew
 */
int cmd_dwt(int argc,char **argv)
{
		
	return 0;
}
W_BOOT_CMD(dwt,cmd_dwt,"destory work thread");

/**
 * @brief	列出服务器信息
 * @section SYNOPSIS
 *	- inf arg1 arg2
 * @section DESCRIPTION
 *	- arg1
 *		- TODO more
 *	- arg2
 *		- TODO more
 */
int cmd_InfoServer(int argc,char **argv)
{
	struct list_head *pos,*n;
	struct ep_con_t *pnode;
	int index = 1;


	printf("%-4s%8s%16s%24s\n","Index","FD","locate","Remote");
	list_for_each_safe(pos, n, &ep.node_head) {
		pnode = list_entry(pos,struct ep_con_t,list);
		printf("%-4d%8d%16s:%-8d",
		 	index++,
			pnode->sockfd,
			inet_ntoa(pnode->loc_addr.sin_addr),
			htons(pnode->loc_addr.sin_port));
		printf("%16s:%-8d\n",
			inet_ntoa(pnode->rem_addr.sin_addr),
			htons(pnode->rem_addr.sin_port));


		// printf("\nfd :%d\n", pnode->sockfd);
		// printf("%s:", inet_ntoa(pnode->loc_addr.sin_addr));
		// printf("%d-->", htons(pnode->loc_addr.sin_port));
		// printf("%s:", inet_ntoa(pnode->rem_addr.sin_addr));
		// printf("%d\n", htons(pnode->rem_addr.sin_port));

		//printf("%s:%d-->%s:%d\n",
		// printf("%16s:%-8d",inet_ntoa(pnode->loc_addr.sin_addr),
		// 	htons(pnode->loc_addr.sin_port));
		// printf("%16s:%-8d\n",
		// 	inet_ntoa(pnode->rem_addr.sin_addr),
		// 	htons(pnode->rem_addr.sin_port));


	}
	return 0;
}
W_BOOT_CMD(inf,cmd_InfoServer,"list server information");

#ifdef _DEBUG
int cmd_TransFile(int argc,char **argv)
{
	return 0;
}
W_BOOT_CMD(tfile,cmd_TransFile,"debug only transfer file");
#endif


int cmd_WorkQueue(int argc,char **argv)
{
	return 0;
}
W_BOOT_CMD(workqueue,cmd_WorkQueue,"work queue");


#ifdef __cplusplus
}
#endif