/**
 ******************************************************************************
 * @file	cmd_tmsxx.c
 * @brief	Menglong Wu\n
 TMSxx项目命令行调试及应用


*/

#ifdef __cplusplus
extern "C" {
#endif
#include <wchar.h>
#include "minishell_core.h"
#include "epollserver.h"
#include "tms_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "signal.h"
#include <ctype.h>


#include "./protocol/tmsxx.h"

#include <stdlib.h>
#include <locale.h>
#include "src/tmsxxdb.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "protocol/md5.h"

extern void PrintfMemory(uint8_t *buf, uint32_t len);
extern int DispRoute(struct tdb_route_line *prl, struct trace_cache *ptc);
extern void DispOneRoute(tdb_route_t *pval);
extern int DispRoute_V2(struct tdb_route *prl, int count, struct trace_cache *ptc);
extern struct ep_t ep;
extern int g_en_connect_cu;


static int sg_frameid = 0;
static int sg_slotid = 0;
static int sg_sockfdid = 0;
// static int sg_portid = 0;
static int sg_sudo = 0;

struct tms_cfg_olp_ref_val sg_olpref[6];
struct tms_cfg_olp_ref_val sg_olpref_def;
struct tms_cfg_opm_ref_val sg_opmref[8];
struct tms_cfg_opm_ref_val sg_opmref_def;
struct tdb_route sg_route[MAX_ROUTE_NODE];


// tb_route 数据库检索
int sg_route_index = 0;
struct tdb_route sg_route_dblast;
int sg_route_page = 0;


// tb_a_trigger_b 数据库检索
int sg_a_trigger_b_index = 0;
struct tdb_a_trigger_b sg_a_trigger_b_dblast;
int sg_a_trigger_b_page = 0;

static struct tms_cfg_sms_val	sg_alarm_sms[7];
static struct tms_cfg_sms_val sg_alarm_sms_def;

// 内部数据结构
struct _dispinf
{
	struct trace_cache tc;
	int index;
};


void PrintOLPDef(struct tms_cfg_olp_ref_val *pval)
{
	printf("default Lv :%6.1f %6.1f %6.1f\n",
		pval->leve0 / 10.0, 
		pval->leve1 / 10.0, 
		pval->leve2 / 10.0);
}

void PrintOPMDef(struct tms_cfg_opm_ref_val *pval)
{
	printf("default Lv :%6.1f %6.1f %6.1f\n",
		pval->leve0 / 10.0, 
		pval->leve1 / 10.0, 
		pval->leve2 / 10.0);
}

int32_t UpdateOTDR(char *fname, char *ip, int16_t port)
{
	int32_t flen;

	FILE *fp = NULL;
	fp = fopen(fname, "rb");
	if (fp == NULL) {
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	printf("flen %d\n", flen);

	fseek(fp, 0, SEEK_SET);
	
	char *pmem = (char*)malloc(flen);
	char *ptmem;

	fread(pmem, 1, flen, fp);

	ptmem = pmem;
	for (int i = 0; i < flen; i++) {
		printf("%c", *ptmem++);
	}

	fclose(fp);
	// TODO 发送
	free(pmem);
	return 0;
}
int inputsms(char *out, int nlen)
{
#define CHAR_LEN 256
#define WCHAR_LEN (CHAR_LEN*2)

	char strinput[CHAR_LEN]= {0};
	wchar_t wstrinput[WCHAR_LEN]= {0};
	char out_unicode[CHAR_LEN]= {0};
	int len, count;

	char *psrc, *pdst;
	// wchar_t *p;
	setlocale(LC_ALL,"zh_CN.utf-8");
	// wscanf(L"%s",wcstr);
	scanf("%255s", strinput);
	// p = (wchar_t*)readline("sms");
	//  printf("wlen = %d",wcslen(p));
	//  memcpy(strinput,p,wcslen(p));
	//  free(p);
	mbstowcs(wstrinput, strinput, WCHAR_LEN);
	perror("wcstombs ");
	count = wcslen(wstrinput);
	printf("len = %d\n", count);

	psrc = (char*)wstrinput;
	pdst = out_unicode;
	for (int i = 0; i < count; i++) {
		*(pdst + (i<<1) + 0) = *(psrc + (i<<2) + 0);
		*(pdst + (i<<1) + 1) = *(psrc + (i<<2) + 1);
		*(pdst + (i<<1) + 2) = 0;
		*(pdst + (i<<1) + 3) = 0;
		printf("c ");
	}

	len = nlen > (count*2) ? (count*2) : nlen;
	memcpy(out, out_unicode, len);
	// memcpy(out, strinput, len);

	// PrintfMemory((uint8_t*)strinput,30);
	// PrintfMemory((uint8_t*)wstrinput,30);
	// PrintfMemory((uint8_t*)out_unicode,30);
	return len+2;
}

int cmd_tms(int argc, char **argv)
{
	// char sbuf[1024];
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
		// int fd;
		// fd = atoi(argv[1]);
		// struct tms_sms_duty duty;
		struct tms_sms_clerk clerk[2];
		struct tms_sms_alarm alarm1[3], alarm2[2];



		// duty.time = 0xa;
		// duty.clerk_count = 2;
		// duty.clerk     = &clerk[0];


		clerk[0].alarm = &alarm1[0];
		strcpy((char*)clerk[0].phone, "12345");
		clerk[1].alarm = &alarm2[0];
		strcpy((char*)clerk[1].phone, "67891");
		clerk[0].alarm_count = 3;
		clerk[1].alarm_count = 2;


		alarm1[0].type = 0x10;
		alarm1[1].type = 0x20;
		alarm1[2].type = 0x30;
		alarm1[0].level = 0x1a;
		alarm1[1].level = 0x2a;
		alarm1[2].level = 0x3a;

		
		
		alarm2[0].type = 0x11;
		alarm2[1].type = 0x21;
		alarm2[0].level = 0xca;
		alarm2[1].level = 0xcb;

		// char sn[128] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		// tms_RetSerialNumber(fd, sn);
		// tms_SetSendSMSAuthorization_old(fd, &duty);
		
	}
	return 0;
}

W_BOOT_CMD(eptms, cmd_tms, "cmd epoll server send");


void cmd_InitTmsxxEnv()
{
	// olp cache
	sg_olpref_def.ref_power = -150;
	sg_olpref_def.leve0 = 100;
	sg_olpref_def.leve1 = 80;
	sg_olpref_def.leve2 = 50;
	for (int i = 0; i < 6; i++) {
		sg_olpref[i].isminitor = 1;
		sg_olpref[i].wave = 1550;
		sg_olpref[i].port = i % 3;
		sg_olpref[i].isminitor = 1;
		sg_olpref[i].wave = 1550;
		sg_olpref[i].ref_power = -150;
		sg_olpref[i].leve0 = 100;
		sg_olpref[i].leve1 = 80;
		sg_olpref[i].leve2 = 50;
	}


	// opm cache


	sg_opmref_def.ref_power = -150;
	sg_opmref_def.leve0 = 100;
	sg_opmref_def.leve1 = 80;
	sg_opmref_def.leve2 = 50;
	for (int i = 0; i < 8; i++) {
		sg_opmref[i].isminitor = 1;
		sg_opmref[i].wave = 1550;
		sg_opmref[i].port = i;
		sg_opmref[i].isminitor = 1;
		sg_opmref[i].wave = 1550;
		sg_opmref[i].ref_power = -150;
		sg_opmref[i].leve0 = 100;
		sg_opmref[i].leve1 = 80;
		sg_opmref[i].leve2 = 50;
	}



	
	for (int i = 0; i < 7; i++) {
		sg_alarm_sms[i].time = i + 1;
		strcpy((char*)sg_alarm_sms[i].phone, "18777399591");
		sg_alarm_sms[i].type  = 1;
		sg_alarm_sms[i].level = 1;
	}
	sg_alarm_sms_def.level = 0;
	sg_alarm_sms_def.type = 1;
	sg_alarm_sms_def.level = 1;
	strcpy((char*)sg_alarm_sms_def.phone, "18777399591");

	

	bzero(sg_route, sizeof(struct tdb_route) * MAX_ROUTE_NODE);
	bzero(&sg_route_dblast, sizeof(struct tdb_route));
	sg_route_index = 0;
	sg_route_page = 0;

	bzero(&sg_a_trigger_b_dblast, sizeof(struct tdb_a_trigger_b));
	sg_a_trigger_b_index = 0;
	sg_a_trigger_b_page = 0;

}
int cf_howline(char *file, int row, int *start, int *end)
{
	int rlen;
	int trow;
	char strout[1024] = "12311l1k2jijaoiweor";
	FILE *fp;
	int startIndex,endIndex,total;
	int len;

	startIndex = 0;
	endIndex = 0;
	if (row < 1) {
		return -1;
	}

	fp = fopen(file, "r");
	if (fp == NULL) {
		return -1;
	}
	


	trow = row;
	startIndex = 0;
	if (trow == 1) {
		goto _Next;
	}
	while(1) {
		rlen = fread(strout, 1, 10, fp);
		if (rlen > 0) {
			for (int i = 0; i < rlen; i++) {
				if (strout[i] == '\n') {
					trow--;
					if (trow == 1) {
						startIndex += i + 1;
						goto _Next;	
					}
					
				}

			}
		}
		else {
			break;
		}
		startIndex += 10;
	}
_Next:;
	fseek(fp, startIndex, SEEK_SET);

	endIndex = startIndex;
	while(1) {
		rlen = fread(strout, 1, 10, fp);
		if (rlen > 0) {
			for (int i = 0; i < rlen; i++) {
				if (strout[i] == '\n') {
					endIndex += i;
					goto _JumpReadLine;	
				}
			}
		}
		else {
			break;
		}
		endIndex += 10;
	}

_JumpReadLine:;
	fseek(fp, startIndex, SEEK_SET);

	total = endIndex - startIndex;
	if (total == 0) {
		return -1;
	}
	while(total > 0) {
		if (total > 1024) {
			len = 1024;
		}
		else {
			len = total;
		}
		rlen = fread(strout, 1, len, fp);
		if (rlen >= 0) {
			total  -= rlen;
			strout[rlen+1] = '\0';
			printf("%4.4d:%s\n", row, strout);
		}
		else {
			break;
		}
	}
	fclose(fp);

	*start = startIndex;
	*end   = endIndex;
	return 0;

}

int cmd_listfile(int argc, char **argv)
{
	char *pfpath;
	int row;
	FILE *fp;
	char strout[1024] = "12311l1k2jijaoiweor";
	size_t rlen;



	if (argc == 2) {
		for (int i = 10; i < 30; i++) {
			int start = 0, end = 0;
			pfpath = argv[1];
			row = i+1;
			cf_howline(pfpath, row, &start, &end);


		}
		return 0;


		pfpath = argv[1];
		fp = fopen(pfpath, "r");
		if (fp == NULL) {
			return -1;
		}
		rlen = 1;
		while(1) {
			rlen = fread(strout, 1, 3, fp);

			if (rlen > 0) {
				strout[rlen] = '\0';
				printf("%s", strout);
			}
			else {
				break;
			}
		}
		putchar('\n');
		fclose(fp);
	}
	else if (argc == 3) {
		int start, end;
		pfpath = argv[1];
		row = atoi(argv[2]);
		cf_howline(pfpath, row, &start, &end);
		printf("start %d end %d\n", start, end);
		return 0;
	}

	return 0;
}
W_BOOT_CMD(lf, cmd_listfile, "cmd epoll server send");
#include "readline/readline.h"
#include "readline/history.h"
extern char *rl_display_prompt ;
int cmd_sql(int argc, char **argv)
{
	char sql[1024];
	// char *ptprompt;
	char *pstr;
	// 不带strcat溢出检查
	
	// strcpy(sql, "sqlite3 /etc/tmsxx.db \"");
	// for (int i = 1; i < argc; i++) {
	// 	strcat(sql, argv[i]);
	// 	strcat(sql, " ");
	// }
	// strcat(sql, "\"");
	pstr = readline((char*)"SQL:>");
	if (pstr != NULL) {
		snprintf(sql, 1024, "sqlite3 /etc/tmsxx.db \"%s\"",pstr);
		system(sql);
		if (*pstr != '\0') {
			add_history(pstr);
		}
		free(pstr);
	}
	return 0;
}
W_BOOT_CMD(sql, cmd_sql, "call sqlite3 console");

static int _cb_Select_commom(tdb_common_t *output, void *ptr, int len)
{
	printf("id %d val1 %d val2 %d val3 %d val4 %d val5 %d \n\
		val6 %d val7 %d val8 %d val9 %d val10 %d val11 %d val12 %d pdata %s\n",
		output->id,output->val1,output->val2,output->val3,output->val4,output->val5,
		output->val6,output->val7,output->val8,output->val9,output->val10,output->val11,output->val12,
		(char*)output->pdata);
	return 0;

}

static int _cb_Select_otdr_rollcall(tdb_otdr_rollcall_t *output, void *ptr)
{
	// 打印部分信息
	tms_Print_tms_getotdr_test_hdr( output->ptest_hdr);
	tms_Print_tms_retotdr_test_param((struct tms_retotdr_test_param*)output->ptest_param);
	return 0;
}

static int _cb_Select_otdr_ref(tdb_otdr_ref_t *output, void *ptr)
{
	// 打印部分信息
	printf("addr %x\n", (int)output->pevent_val);
	tms_Print_tms_retotdr_event(output->pevent_hdr, output->pevent_val);
	tms_Print_tms_retotdr_chain(output->pchain);
	tms_Print_tms_otdr_ref_hdr(output->pref_hdr);
	tms_Print_tms_retotdr_test_param(output->ptest_param);
	return 0;
}

static int _cb_Select_otdr_his_data(tdb_otdr_his_data_t *output, void *ptr)
{
	// 打印部分信息
	tms_Print_tms_retotdr_event(output->pevent_hdr, output->pevent_val);
	tms_Print_tms_retotdr_chain(output->pchain);
	tms_Print_tms_retotdr_test_hdr(output->ptest_hdr);
	tms_Print_tms_retotdr_test_param(output->ptest_param);
	return 0;
}

static int _cb_Select_otdr_alarm_data(tdb_otdr_alarm_data_t *output, void *ptr)
{
	// 打印部分信息
	tms_Print_tms_retotdr_event(output->pevent_hdr, output->pevent_val);
	tms_Print_tms_retotdr_chain(output->pchain);
	tms_Print_tms_retotdr_test_hdr(output->ptest_hdr);
	tms_Print_tms_retotdr_test_param(output->ptest_param);
	return 0;
}

static int _cb_Select_route_page_brief(tdb_route_t *output, void *ptr)
{
	static char devName[][8] = {
			{"Unknow"}, 
			{"PWU"}, //1
			{"MCU"}, //2
			{"OPM"}, //3
			{"OSW"}, 
			{"OTDR"}, 
			{"OLS"}, 
			{"OLP"}, 
			{"SMS"}, 
	};

	if ((uint32_t)output->type_b < sizeof(devName) / sizeof(char[8])) {

		if (output->port_b == (int32_t)0xefffffff) {
			printf("\t%s(%d/%d/%d/c)---\n",
				devName[output->type_b],
				output->frame_b, 
				output->slot_b, 
				output->type_b);
		}
		else {
			printf("\t%s(%d/%d/%d/%d)---\n",
				devName[output->type_b],
				output->frame_b, 
				output->slot_b, 
				output->type_b, 
				output->port_b);
		}
	}
	else {
		printf("\tUnknow(%d/%d/%d/%d)---\n",
			output->frame_b, 
			output->slot_b, 
			output->type_b, 
			output->port_b);
	}

	sg_route_dblast.id = output->id;
	sg_route_page += 1;
	return 0;
}
static int _cb_Select_route_page_detail(tdb_route_t *output, void *ptr)
{
	DispOneRoute(output);	
	sg_route_dblast.id = output->id;
	sg_route_page += 1;
	return 0;
}

static int _cb_Select_a_trigger_b_page(tdb_a_trigger_b_t *output, void *ptr)
{
	sg_a_trigger_b_dblast.id = output->id;
	printf("%d/%d/%d/%d -- %d/%d/%d/%d\n", 
		output->frame_a,output->slot_a, output->type_a, output->port_a,
		output->frame_b,output->slot_b, output->type_b, output->port_b);
	return 0;
}


static int _cb_Select_route_record(tdb_route_t *output, void *ptr)
{
	struct tdb_route_node *pnode = (struct tdb_route_node *)ptr;


	pnode->frame = output->frame_b;
	pnode->slot = output->slot_b;
	pnode->type = output->type_b;
	pnode->port = output->port_b;



	pnode->find = 1;
	return 0;
}
// static int _cb_Select_route_listline(tdb_route_t *output, void *ptr)
// {
// 	// printf(" %d %d %d %d--->%d %d %d %d\n",output->frame_a,output->slot_a,output->type_a, output->slot_a,
// 	// 	output->frame_b,output->slot_b,output->type_b, output->slot_b);

// 	tdb_route_t input;
// 	tdb_route_t mask;
// 	// int from;
// 	// int val;
// 	struct tdb_route_node nodes[8];
// 	int i;

// 	bzero(&input, sizeof(tdb_route_t));
// 	bzero(&mask, sizeof(tdb_route_t));

	
// 	input.frame_a = output->frame_b;
// 	input.slot_a = output->slot_b;
// 	input.type_a = output->type_b;
// 	input.slot_a = output->slot_b;

// 	mask.frame_a = 1;
// 	mask.slot_a = 1;
// 	mask.type_a = 1;
// 	mask.slot_a = 1;
	
// 	nodes[0].frame = output->frame_b;
// 	nodes[0].slot = output->slot_b;
// 	nodes[0].type = output->type_b;
// 	nodes[0].port = output->port_b;
// 	for (i = 1;i < 8; i++) {
// 		nodes[i].find = 0;
// 		tmsdb_Select_route(&input, &mask, _cb_Select_route_record,&nodes[i]);
// 		input.frame_a = nodes[i].frame;
// 		input.slot_a = nodes[i].slot;
// 		input.type_a = nodes[i].type;
// 		input.port_a = nodes[i].port;
// 		printf(" [%d %d %d %d]\n",nodes[i].frame,nodes[i].slot,nodes[i].type,nodes[i].port);

// 		if (nodes[i].find == 0) {
// 			break;
// 		}
// 		if (nodes[i].type == 0) {
// 			break;
// 		}
// 	}

// 	// struct tdb_route_line rl;
// 	// struct trace_cache tc;
// 	// char strout[256];

// 	// tc.strout = strout;

// 	// rl.count = i;
// 	// rl.node = nodes;

// 	// tc.empty = 256;
// 	// tc.offset = 0;
// 	// tc.limit = 180;
// 	// DispRoute(&rl, &tc);
// 	// printf("%s",tc.strout);
	

// 	return -1;
// }
int cmd_select(int argc, char **argv)
{
	int row;

	if (argc >= 3 && memcmp(argv[1], "tb_common", strlen(argv[1])) == 0) {
		tdb_common_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.val1 = atoi(argv[2]);	
		mask.val1 = 1;
		mask.val2 = 0;
		mask.val3 = 0;
		mask.val4 = 0;
		tmsdb_Select_common(&input, &mask, _cb_Select_commom, 0 );
	}
	else if (argc == 3 && memcmp(argv[1], "tb_sn", strlen(argv[1])) == 0) {
		tdb_sn_t input, mask;
		tdb_sn_t *ppout;

		bzero(&mask,  sizeof(tdb_common_t));
		strcpy((char*)&input.sn[0], argv[2]);
		
		mask.sn[0] = 1;
		row = tmsdb_Select_sn(&input, &mask, &ppout);
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%s\n",ppout[r].sn);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_sms", strlen(argv[1])) == 0) {
		tdb_sms_t *ppout = NULL;
		tdb_sms_t input, mask;

		bzero(&mask,  sizeof(tdb_sms_t));	
		// strcpy((char*)&input.phone[0], argv[2]);
		input.time = atoi(argv[2]);
		mask.time = 1;
		// mask.phone[0] = 1;

		row = tmsdb_Select_sms(&input, &mask, &ppout);
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].time);
			printf("%s\t",ppout[r].phone);
			printf("%d\t",ppout[r].type);
			printf("%d\n",ppout[r].level);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_composition", strlen(argv[1])) == 0) {
		tdb_composition_t *ppout = NULL;
		tdb_composition_t input, mask;

		bzero(&mask,  sizeof(tdb_composition_t));	
		input.frame = atoi(argv[2]);

		mask.frame = 1;

		row = tmsdb_Select_composition(&input, &mask, &ppout);
		
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\n",ppout[r].port);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_dev_map", strlen(argv[1])) == 0) {
		tdb_dev_map_t *ppout = NULL;
		tdb_dev_map_t input, mask;

		bzero(&mask, sizeof(tdb_dev_map_t));
		input.frame = atoi(argv[2]);
		mask.frame = 1;
		row = tmsdb_Select_dev_map(&input, &mask, &ppout);
		
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\t",ppout[r].port);
			printf("%s\t",ppout[r].dev_name );
			printf("%s\t",ppout[r].cable_name );
			printf("%s\t",ppout[r].start_name );
			printf("%s\n",ppout[r].end_name );
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc>=  3 && memcmp(argv[1], "tb_any_unit_osw", strlen(argv[1])) == 0) {
		tdb_any_unit_osw_t *ppout = NULL;
		tdb_any_unit_osw_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.any_frame = atoi(argv[2]);	
		mask.any_frame = 1;
		row = tmsdb_Select_any_unit_osw(&input, &mask, &ppout);
		
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].any_frame);
			printf("%d\t",ppout[r].any_slot);
			printf("%d\t",ppout[r].any_type);
			printf("%d\n",ppout[r].any_port);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	// dbselete tb_route type n
	// else if (argc >=  4 && memcmp(argv[1], "tb_route", strlen(argv[1])) == 0) {
	// 	tdb_route_t input;
	// 	tdb_route_t mask;
	// 	int from;
	// 	int val;

	// 	bzero(&input, sizeof(tdb_route_t));
	// 	bzero(&mask, sizeof(tdb_route_t));

	// 	if ( 'h' == argv[2][0] ) {
	// 		input.frame_a = 0;
	// 		input.slot_a = 0;
	// 		input.type_a = 0;
	// 		input.slot_a = 0;

	// 		mask.frame_a = 1;
	// 		mask.slot_a = 1;
	// 		mask.type_a = 1;
	// 		mask.slot_a = 1;
	// 		from = atoi(argv[3]);
	// 		// tmsdb_Select_Page_route(&input, &mask, from, 10, _cb_Select_route_page_brief,NULL);	
	// 	}
	// 	// else if ( 'a' == argv[2][0] ) {
	// 	// 	from = atoi(argv[3]);
	// 	// 	tmsdb_Select_Page_route(&input, &mask, from, 10, _cb_Select_route_listline,NULL);	
	// 	// }
	// 	else if ( 'l' == argv[2][0] ) {
	// 		input.frame_a = 0;
	// 		input.slot_a = 0;
	// 		input.type_a = 0;
	// 		input.slot_a = 0;

	// 		mask.frame_a = 1;
	// 		mask.slot_a = 1;
	// 		mask.type_a = 1;
	// 		mask.slot_a = 1;
	// 		from = atoi(argv[3]);

	// 		tmsdb_Select_Page_route(&input, &mask, from, 10, _cb_Select_route_listline,NULL);	
	// 	}
	// 	return 0;
	// 	struct tdb_route_node nodes[9];
	// 	struct tdb_route_line rl;
	// 	struct trace_cache tc;
	// 	char strout[256];

	// 	tc.strout = strout;

	// 	for (int i = 0;i < 9;i++) {
	// 		nodes[i].id = 10+i;
	// 		nodes[i].frame = i;
	// 		nodes[i].slot = i;
	// 		nodes[i].type = i;
	// 		nodes[i].port = i;
	// 	}

	// 	rl.count = 9;
	// 	rl.node = nodes;
	// 	tc.strout = strout;

	// 	tc.empty = 256;
	// 	tc.offset = 0;
	// 	tc.limit = 180;
	// 	DispRoute(&rl, &tc);
	// 	printf("%s",tc.strout);

	// 	tc.empty = 256;
	// 	tc.offset = 0;
	// 	tc.limit = 180;
	// 	DispRoute(&rl, &tc);
	// 	printf("%s",tc.strout);

	// 	tc.empty = 256;
	// 	tc.offset = 0;
	// 	tc.limit = 180;
	// 	DispRoute(&rl, &tc);
	// 	printf("%s",tc.strout);
	// }
	else if (argc >=  3 && memcmp(argv[1], "tb_osw_cyc", strlen(argv[1])) == 0) {
		tdb_osw_cyc_t *ppout = NULL;
		tdb_osw_cyc_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);
		mask.frame = 1;
		row = tmsdb_Select_osw_cyc(&input, &mask, &ppout);

		printf("row = %d\n",row);
		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\t",ppout[r].port);
			printf("%d\t",ppout[r].iscyc);
			printf("%d\n",ppout[r].interval);

		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_osw_cyc_bak", strlen(argv[1])) == 0) {
		tdb_osw_cyc_bak_t *ppout = NULL;
		tdb_osw_cyc_bak_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_bak_t));
		input.frame = atoi(argv[2]);
		mask.frame = 1;
		row = tmsdb_Select_osw_cyc_bak(&input, &mask, &ppout);

		printf("row = %d\n",row);
		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\t",ppout[r].port);
			printf("%d\t",ppout[r].iscyc);
			printf("%d\t",ppout[r].interval);
			printf("%d\n",ppout[r].nexttest);

		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_rollcall", strlen(argv[1])) == 0) {
		tdb_otdr_rollcall_t input, mask;
		struct tms_getotdr_test_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_rollcall_t));
		bzero(&mtest_hdr, sizeof(struct tms_getotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		itest_hdr.frame = atoi(argv[2]);
		mtest_hdr.frame = 1;

		tmsdb_Select_otdr_rollcall(&input, &mask, _cb_Select_otdr_rollcall, NULL);
		// tmsdb_Select_otdr_rollcall(NULL, NULL, _cb_Select_otdr_rollcall, NULL);
	}

	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_ref", strlen(argv[1])) == 0) {
		tdb_otdr_ref_t input, mask;
		struct tms_otdr_ref_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_ref_t));
		bzero(&mtest_hdr, sizeof(struct tms_otdr_ref_hdr));
		input.pref_hdr = &itest_hdr;
		mask.pref_hdr  = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		mtest_hdr.osw_frame = 1;

		tmsdb_Select_otdr_ref(&input, &mask, _cb_Select_otdr_ref, NULL);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0) {
		tdb_otdr_his_data_t input, mask;

		struct tms_retotdr_test_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_his_data_t));
		bzero(&mtest_hdr, sizeof(struct tms_retotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		mtest_hdr.osw_frame = 1;

		tmsdb_Select_otdr_his_data(&input, &mask, _cb_Select_otdr_his_data, NULL);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_alarm_data", strlen(argv[1])) == 0) {
		tdb_otdr_alarm_data_t input, mask;

		struct tms_retotdr_test_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_his_data_t));
		bzero(&mtest_hdr, sizeof(struct tms_retotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		mtest_hdr.osw_frame = 1;

		tmsdb_Select_otdr_alarm_data(&input, &mask, _cb_Select_otdr_alarm_data, NULL);
	}
	else {
		printf("Usage: input err\n");
	}
	// if(1) {
	// 	tdb_composition_t con,mask;
	// 	con.frame = 1;con.slot = 2;
	// 	mask.frame = 1;mask.slot = 1;
	// 	mask.port = 0;mask.type = 0;
	// 	tmsdb_Delete_composition(0,0);

	// 	tmsdb_Delete_composition(&con, &mask);
	// }
	return 0;
}
W_BOOT_CMD(select, cmd_select, "select tmsxxdb");


int cmd_delete(int argc, char **argv)
{
	// tmsdb_Delete_otdr_ref(NULL,NULL);

	if (argc == 6 && memcmp(argv[1], "tb_common", strlen(argv[1])) == 0) {
		tdb_common_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.val1 = atoi(argv[2]);	
		input.val2 = atoi(argv[3]);	
		input.val3 = atoi(argv[4]);	
		input.val4 = atoi(argv[5]);	
		mask.val1 = 1;
		mask.val2 = 1;
		mask.val3 = 1;
		mask.val4 = 1;
		tmsdb_Delete_common(&input, &mask);
	}
	if (argc == 2 && memcmp(argv[1], "tb_sn", strlen(argv[1])) == 0) {
		tdb_sn_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		
		tmsdb_Delete_sn(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_sms", strlen(argv[1])) == 0) {
		tdb_sms_t input, mask;

		bzero(&input,  sizeof(tdb_sms_t));
		bzero(&mask,  sizeof(tdb_sms_t));
		input.time = atoi(argv[2]);	
		strcpy((char*)&input.phone[0], argv[3]);
		// input.type = atoi(argv[3]);	

		mask.time =1;
		// mask.type =1;
		mask.phone[0] = 1;
		
		tmsdb_Delete_sms(&input, &mask);
	}

	else if (argc >= 4 && memcmp(argv[1], "tb_composition", strlen(argv[1])) == 0) {
		tdb_composition_t input[1], mask;

		bzero(&mask,  sizeof(tdb_composition_t));
		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	

		mask.frame =1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;

		tmsdb_Delete_composition(&input[0], &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_dev_map", strlen(argv[1])) == 0) {
		tdb_dev_map_t input[1], mask;

		bzero(&mask,  sizeof(tdb_dev_map_t));
		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	
		input[0].type = 1;	
		input[0].port = 2;

		mask.frame =1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;

		tmsdb_Delete_dev_map(&input[0], &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_any_unit_osw", strlen(argv[1])) == 0) {
		tdb_any_unit_osw_t input[1], mask;

		bzero(&mask, sizeof(tdb_any_unit_osw_t));
		input[0].any_frame = atoi(argv[2]);	
		input[0].any_slot  = atoi(argv[3]);	

		mask.any_frame =1;
		mask.any_slot = 1;
		mask.any_type = 0;
		mask.any_port = 0;

		tmsdb_Delete_any_unit_osw(&input[0], &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_osw_cyc", strlen(argv[1])) == 0) {
		tdb_osw_cyc_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);	
		input.slot  = atoi(argv[3]);	

		mask.frame =1;
		mask.slot = 1;

		tmsdb_Delete_osw_cyc(&input, &mask);
	}

	else if (argc >= 4 && memcmp(argv[1], "tb_osw_cyc_bak", strlen(argv[1])) == 0) {
		tdb_osw_cyc_bak_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_bak_t));
		input.frame = atoi(argv[2]);	
		input.slot  = atoi(argv[3]);	

		mask.frame =1;
		mask.slot = 1;


		tmsdb_Delete_osw_cyc_bak(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0) {
		tdb_otdr_his_data_t input, mask;
		struct tms_retotdr_test_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_his_data_t));
		bzero(&mtest_hdr, sizeof(struct tms_retotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		itest_hdr.osw_slot = atoi(argv[3]);
		mtest_hdr.osw_frame = 1;
		mtest_hdr.osw_slot = 1;
		
		tmsdb_Delete_otdr_his_data(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_alarm_data", strlen(argv[1])) == 0) {
		tdb_otdr_alarm_data_t input, mask;
		struct tms_retotdr_test_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_his_data_t));
		bzero(&mtest_hdr, sizeof(struct tms_retotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		itest_hdr.osw_slot = atoi(argv[3]);
		mtest_hdr.osw_frame = 1;
		mtest_hdr.osw_slot = 1;
		
		tmsdb_Delete_otdr_alarm_data(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_rollcall", strlen(argv[1])) == 0) {
		tdb_otdr_rollcall_t input, mask;
		struct tms_getotdr_test_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_rollcall_t));	
		bzero(&mtest_hdr, sizeof(struct tms_getotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		itest_hdr.frame = atoi(argv[2]);
		itest_hdr.slot = atoi(argv[3]);
		mtest_hdr.frame = 1;
		mtest_hdr.slot = 1;
		tmsdb_Delete_otdr_rollcall(&input, &mask);
	}

	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_ref", strlen(argv[1])) == 0) {
		tdb_otdr_ref_t input, mask;
		struct tms_otdr_ref_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_ref_t));	
		bzero(&mtest_hdr, sizeof(struct tms_otdr_ref_hdr));
		input.pref_hdr = &itest_hdr;
		mask.pref_hdr  = &mtest_hdr;

		itest_hdr.osw_frame = atoi(argv[2]);
		itest_hdr.osw_slot = atoi(argv[3]);
		mask.id = 0;
		mtest_hdr.osw_frame = 1;
		mtest_hdr.osw_slot = 1;
		tmsdb_Delete_otdr_ref(&input, &mask);
	}
	else {
		printf("Usage: input err\n");
	}
	return 0;
}
W_BOOT_CMD(delete, cmd_delete, "delete tmsxxdb");

int cmd_insert(int argc, char **argv)
{
	struct tdb_route val[10];
	
	bzero(val, sizeof(struct tdb_route));
	val[0].frame_b = val[1].frame_a = 1;
	val[1].frame_b = val[2].frame_a = 6;
	val[2].frame_b = val[3].frame_a = 3;

	val[4].frame_b = val[5].frame_a = 0;
	val[5].frame_b = val[6].frame_a = 0;
	val[6].frame_b = val[7].frame_a = 2;
	val[7].frame_b = val[8].frame_a = 5;
	// int ret;
	// ret = tmsdb_Check_route_oneline(val,5);

	// ret = tmsdb_Check_route_multiline(val, 10);
	// printf("ret = %d\n", ret);
	if (argc == 7 && memcmp(argv[1], "tb_common", strlen(argv[1])) == 0) {
		tdb_common_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.val1 = atoi(argv[2]);	
		input.val2 = atoi(argv[3]);	
		input.val3 = atoi(argv[4]);	
		input.val4 = atoi(argv[5]);	
		input.pdata = argv[6];
		input.lenpdata = strlen((char*)input.pdata);
		mask.val1 = 1;
		mask.val2 = 1;
		mask.val3 = 1;
		mask.val4 = 1;
		tmsdb_Insert_common(&input, &mask, 1);
	}
	else if (argc == 3 && memcmp(argv[1], "tb_sn", strlen(argv[1])) == 0) {
		tdb_sn_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		strcpy((char*)&input.sn[0], argv[2]);
		
		mask.sn[0] = 1;	
		tmsdb_Insert_sn(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_sms", strlen(argv[1])) == 0) {
		tdb_sms_t input, mask;
		bzero(&input, sizeof(tdb_sms_t));
		bzero(&mask, sizeof(tdb_sms_t));
		input.time = atoi(argv[2]);	
		// input.slot = atoi(argv[3]);	
		strcpy((char*)&input.phone[0], argv[3]);
		// input.type = atoi(argv[3]);	
		input.level = 2;
		
		mask.time =1;
		mask.phone[0] =1;
		tmsdb_Insert_sms(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_composition", strlen(argv[1])) == 0) {
		tdb_composition_t input[2], mask;

		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	
		input[0].type = 0;
		input[0].port = 0;
		input[1].frame = input[0].frame;
		input[1].slot  = input[0].slot ;

		// printf("%d %d %d %d\n",input[1].frame,);
		mask.frame =1;
		mask.slot = 1;
		mask.type = 0;
		mask.port = 0;
		tmsdb_Insert_composition(&input[0], &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_dev_map", strlen(argv[1])) == 0) {
		tdb_dev_map_t input[2], mask;

		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	
		input[0].type = 11;	
		input[0].port = 8;

		snprintf((char*)&input[0].dev_name, 64, "设备名dev_name槽位%d",input[0].slot);
		snprintf((char*)&input[0].cable_name, 64, "光缆名cable_name 槽位 %d",input[0].slot);
		snprintf((char*)&input[0].start_name, 64, "起始站名start_name 槽位%d",input[0].slot);
		snprintf((char*)&input[0].end_name, 64, "结束站名end_name 槽位%d",input[0].slot);
		



		mask.frame =1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;
		tmsdb_Insert_dev_map(&input[0], &mask, 1);
	}
	
	else if(argc >= 3 && memcmp(argv[1], "tb_route", strlen(argv[1])) == 0) {
		tdb_route_t input, mask;

		bzero(&input, sizeof(tdb_route_t));
		bzero(&mask, sizeof(tdb_route_t));

		// dbinsert tb_route hdr fb sb tb pb [ipsrc ipdst]
		if (argc >= 7 && memcmp(argv[2], "hdr", strlen(argv[2])) == 0) {
_AddHdr:;
	printf("add hdr\n");
			sg_route_index = 0;

			sg_route[sg_route_index].frame_b = atoi(argv[3]);	
			sg_route[sg_route_index].slot_b  = atoi(argv[4]);
			sg_route[sg_route_index].type_b  = atoi(argv[5]);
			sg_route[sg_route_index].port_b  = atoi(argv[6]);
			if (argc >= MAX_ROUTE_NODE + 1) {
				sg_route[sg_route_index].ip_src = atoi(argv[7]);
				sg_route[sg_route_index].ip_dst = atoi(argv[8]);
				mask.ip_src = 1;
				mask.ip_dst = 1;
			}
			mask.frame_b = 1;
			mask.slot_b = 1;
			mask.type_b = 1;
			mask.port_b = 1;
			sg_route_index++;

			// todo 检查有效性
			// tmsdb_Insert_route(&input,&mask,1);
		}
		// dbinsert tb_route add fa sa ta pa fb sb tb pb 
		else if (argc >= (MAX_ROUTE_NODE - 1) && memcmp(argv[2], "add", strlen(argv[2])) == 0) {
			if (sg_route_index >= 8) {
				printf("full\n");
			}
			if (sg_route_index == 0) {
				printf("first add hdr\n");
				goto _AddHdr;
			}
			sg_route[sg_route_index].frame_a = sg_route[sg_route_index - 1].frame_b;
			sg_route[sg_route_index].slot_a  = sg_route[sg_route_index - 1].slot_b;
			sg_route[sg_route_index].type_a  = sg_route[sg_route_index - 1].type_b;
			sg_route[sg_route_index].port_a  = sg_route[sg_route_index - 1].port_b;

			sg_route[sg_route_index].frame_b = atoi(argv[3]);	
			sg_route[sg_route_index].slot_b  = atoi(argv[4]);
			sg_route[sg_route_index].type_b  = atoi(argv[5]);
			sg_route[sg_route_index].port_b  = atoi(argv[6]);
			
			mask.frame_b = 1;
			mask.slot_b = 1;
			mask.type_b = 1;
			mask.port_b = 1;

			// todo 检查有效性
			// tmsdb_Insert_route(&input,&mask,1);
			sg_route_index++;
		}
		// dbinsert tb_route tail fa sa ta pa 
		else if (argc >= (MAX_ROUTE_NODE - 1) && memcmp(argv[2], "tail", strlen(argv[2])) == 0) {
			if (sg_route_index >= 8) {
				printf("full\n");
			}
			sg_route[sg_route_index].frame_a = atoi(argv[3]);	
			sg_route[sg_route_index].slot_a  = atoi(argv[4]);
			sg_route[sg_route_index].type_a  = atoi(argv[5]);
			sg_route[sg_route_index].port_a  = atoi(argv[6]);

			mask.frame_a = 1;
			mask.slot_a = 1;
			mask.type_a = 1;
			mask.port_a = 1;

			// todo 检查有效性
			// tmsdb_Insert_route(&input,&mask,1);
		}
		// dbinsert tb_route go
		else if (argc >= 3 && memcmp(argv[2], "go", strlen(argv[2])) == 0) {
			mask.frame_a = 1;
			mask.slot_a = 1;
			mask.type_a = 1;
			mask.port_a = 1;

			for(int i =0; i < sg_route_index; i++) {

				printf(" %d %d %d %d--->%d %d %d %d\n",
					sg_route[i].frame_a,sg_route[i].slot_a,sg_route[i].type_a, sg_route[i].port_a,
					sg_route[i].frame_b,sg_route[i].slot_b,sg_route[i].type_b, sg_route[i].port_b);
			}
			// todo 检查有效性
			// tmsdb_Insert_route(sg_route,&mask,sg_route_index);
			// tmsdb_Check_route_oneline(sg_route, sg_route_index);
		}

		// dbinsert tb_route list
		else if (argc >= 3 && memcmp(argv[2], "list", strlen(argv[2])) == 0) {
			struct trace_cache tc;
			char strout[1024];

			tc.strout = strout;



			tc.empty = 1024;
			tc.offset = 0;
			tc.limit = 180;
			// DispRoute(&rl, &tc);
			DispRoute_V2(sg_route, sg_route_index, &tc);
			printf("sg_route_index %d\n%s",sg_route_index,tc.strout);

			// todo 检查有效性
			// tmsdb_Insert_route(&input,&mask,sg_route_index);
		}
	}
	// dbinsert tb_a_trigger_b fa sa ta pa fb sb tb pb
	else if(argc >= 10 &&memcmp(argv[1], "tb_a_trigger_b", strlen(argv[1])) == 0) {
		tdb_a_trigger_b_t input, mask;

		bzero(&input, sizeof(tdb_a_trigger_b_t));
		bzero(&mask, sizeof(tdb_a_trigger_b_t));

		input.frame_a = atoi(argv[2]);	
		input.slot_a  = atoi(argv[3]);
		input.type_a  = atoi(argv[4]);
		input.port_a  = atoi(argv[5]);

		input.frame_b = atoi(argv[6]);	
		input.slot_b  = atoi(argv[7]);
		input.type_b  = atoi(argv[8]);
		input.port_b  = atoi(argv[9]);

		mask.frame_a = 1;
		mask.slot_a = 1;
		mask.type_a = 1;
		mask.port_a = 1;
		mask.frame_b = 1;
		mask.slot_b = 1;
		mask.type_b = 1;
		mask.port_b = 1;
		tmsdb_Insert_a_trigger_b(&input, &mask, 1);

	}
	else if (argc >= 4 && memcmp(argv[1], "tb_any_unit_osw", strlen(argv[1])) == 0) {
		tdb_any_unit_osw_t input[2], mask;

		input[0].any_frame = atoi(argv[2]);	
		input[0].any_slot  = atoi(argv[3]);	
		input[0].any_type  = 2;	
		input[0].any_port  = 3;

		mask.any_frame = 1;
		mask.any_slot  = 1;
		mask.any_type  = 1;
		mask.any_port  = 1;
		tmsdb_Insert_any_unit_osw(&input[0], &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_osw_cyc", strlen(argv[1])) == 0) {
		tdb_osw_cyc_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);	
		input.slot = atoi(argv[3]);	
		input.type = 1;	
		input.port = 2;

		mask.frame = 1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;
		tmsdb_Insert_osw_cyc(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_osw_cyc_bak", strlen(argv[1])) == 0) {
		tdb_osw_cyc_bak_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);	
		input.slot = atoi(argv[3]);	
		input.type = 1;	
		input.port = 2;
		input.iscyc = 1;
		input.nexttest = 3;

		mask.frame = 1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;
		tmsdb_Insert_osw_cyc_bak(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_rollcall", strlen(argv[1])) == 0) {
		tdb_otdr_rollcall_t input;
		struct tms_getotdr_test_hdr       ref_hdr; 		
		struct tms_getotdr_test_param    test_param; 

		

		
		// 得到所有指针
		input.ptest_hdr 		= &ref_hdr;
		input.ptest_param 	= &test_param;
		
		

		ref_hdr.frame = atoi(argv[2]);
		ref_hdr.slot  = atoi(argv[3]);
		ref_hdr.type  = 1;
		ref_hdr.port  = 2;

		test_param.rang = 31111;
		test_param.wl = 1550;			
		test_param.pw =1280		;		
		test_param.time = 15;				
		test_param.gi = 1.2345;				
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 8.8;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;	
		test_param.reserve1 = 0;	


		tmsdb_Insert_otdr_rollcall(&input,NULL,1);
	}
	else if (argc >= 6 && memcmp(argv[1], "tb_otdr_ref", strlen(argv[1])) == 0) {
		tdb_otdr_ref_t input;
		struct tms_otdr_ref_hdr       ref_hdr; 		
		struct tms_retotdr_test_param test_param; 

		struct tms_retotdr_data_hdr   data_hdr; 
		struct tms_retotdr_data_val   data_val[10];//;= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

		struct tms_retotdr_event_hdr  event_hdr; 
		struct tms_retotdr_event_val event_val[4]= {
			{123, 444, 3.1, 3.0, 3.9, 0.0}, 
			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
			{3, 3, 3.0, 3.2, 3.3, 3.4}};

		struct tms_retotdr_chain      chain; 
		struct tms_cfg_otdr_ref_val   ref_data;

		
		// 得到所有指针
		input.pref_hdr 		= &ref_hdr;
		input.ptest_param 	= &test_param;
		input.pdata_hdr 	= &data_hdr;
		input.pdata_val 	= &data_val[0];
		input.pevent_hdr 	= &event_hdr;
		input.pevent_val 	= &event_val[0];
		input.pchain 		= &chain;
		input.pref_data 	= &ref_data;

		memcpy(data_val,"OTDR DATA",10);
		printf(" %c %c %c %c\n", input.pdata_val[0].data,input.pdata_val[1].data,input.pdata_val[2].data,input.pdata_val[3].data);
		ref_hdr.osw_frame = atoi(argv[2]);
		ref_hdr.osw_slot  = atoi(argv[3]);
		ref_hdr.osw_type  = 1;
		ref_hdr.osw_port  = 2;
		strcpy((char*)ref_hdr.strid,"strid");

		test_param.rang 					= 31111;
		test_param.wl 						= 1550;			
		test_param.pw 						=1280;
		test_param.time 					= 15;				
		test_param.gi 						= 1.2345;				
		test_param.end_threshold 			= 9.0;
		test_param.none_reflect_threshold 	= 8.8;
		test_param.sample 					= 20000000;
		test_param.reserve0 				= 0;
		test_param.reserve1 				= 0;	

		memcpy(data_hdr.dpid , "OTDRData", sizeof("OTDRData"));
		data_hdr.count = 10;

		bzero(event_hdr.eventid, 12);

		memcpy(event_hdr.eventid, "KeyEvents", sizeof("KeyEvents"));
		event_hdr.count = 4;
		PrintfMemory((uint8_t*)&event_hdr.eventid, 16);
		

		strcpy((char*)chain.inf, "OTDRTestResultInfo");
		chain.range = 1200;
		chain.loss = 10;
		chain.att = 1.23;
			

		ref_data.leve0 = 789;
		ref_data.leve1 = 456;
		ref_data.leve2 = 123;

		
		
		tmsdb_Insert_otdr_ref(&input,NULL,1);
	}
	else if (argc >= 6 && 
		(memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0 || 
		 memcmp(argv[1], "tb_otdr_alarm_data", strlen(argv[1])) == 0) ) {


		tdb_otdr_alarm_data_t input_alarm;
		tdb_otdr_his_data_t input;
		struct tms_retotdr_test_hdr   test_hdr; 		
		struct tms_retotdr_test_param test_param; 

		struct tms_retotdr_data_hdr   data_hdr; 
		struct tms_retotdr_data_val   data_val[10];//;= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

		struct tms_retotdr_event_hdr  event_hdr; 
		struct tms_retotdr_event_val event_val[4]= {
			{0, 0, 3.1, 3.0, 3.9, 0.0}, 
			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
			{3, 3, 3.0, 3.2, 3.3, 3.4}};

		struct tms_retotdr_chain      chain; 
		// struct tms_cfg_otdr_ref_val   ref_data;
		struct tms_alarm_line_hdr alarm;

		
		// 得到所有指针
		input.ptest_hdr 	= &test_hdr;
		input.ptest_param 	= &test_param;
		input.pdata_hdr 	= &data_hdr;
		input.pdata_val 	= &data_val[0];
		input.pevent_hdr 	= &event_hdr;
		input.pevent_val 	= &event_val[0];
		input.pchain 		= &chain;

		input_alarm.ptest_hdr 	= &test_hdr;
		input_alarm.ptest_param 	= &test_param;
		input_alarm.pdata_hdr 	= &data_hdr;
		input_alarm.pdata_val 	= &data_val[0];
		input_alarm.pevent_hdr 	= &event_hdr;
		input_alarm.pevent_val 	= &event_val[0];
		input_alarm.pchain 		= &chain;
		input_alarm.palarm        = &alarm;

		memcpy(data_val,"OTDR DATA",10);
		printf(" %c %c %c %c\n", input.pdata_val[0].data,input.pdata_val[1].data,input.pdata_val[2].data,input.pdata_val[3].data);
		test_hdr.osw_frame = atoi(argv[2]);
		test_hdr.osw_slot  = atoi(argv[3]);
		test_hdr.osw_type  = 1;
		test_hdr.osw_port  = 2;
		memcpy(&test_hdr.time[0], "time",5);

		test_param.rang = 31111;
		test_param.wl = 1550;			
		test_param.pw =1280		;		
		test_param.time = 15;				
		test_param.gi = 1.2345;				
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 8.8;
		test_param.sample = 20000000;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;	

		memcpy(data_hdr.dpid , "abcd", 5);
		data_hdr.count = 10;

		memcpy(event_hdr.eventid, "[Name]", sizeof("[Name]"));
		event_hdr.count = 4;
		
		chain.range = 1200;
		chain.loss = 10;
		chain.att = 1.23;
		memcpy(chain.inf, "[abcd]", sizeof("[abcd]"));
		// strcpy(chain.inf,"abcd");

		bzero(&alarm, sizeof(struct tms_alarm_line_hdr));
		// ref_data.leve0 = 789;
		// ref_data.leve1 = 456;
		// ref_data.leve2 = 123;

		if (memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0) {

			tmsdb_Insert_otdr_his_data(&input,NULL,1);
		}
		else if (memcmp(argv[1], "tb_otdr_alarm_data", strlen(argv[1])) == 0) {
			tmsdb_Insert_otdr_alarm_data(&input_alarm,NULL,1);
		}
	}
	
	else {
		printf("Usage: input err\n");
	}
	return 0;
}
W_BOOT_CMD(insert, cmd_insert, "insert tmsxxdb");


// 显示光路由，最多16个节点
void DispOneRoute(tdb_route_t *pval)
{
	tdb_route_t input;
	tdb_route_t nodes[16];
	int count = 16;
	bzero(&input, sizeof(tdb_route_t));

	input.frame_b = pval->frame_b;
	input.slot_b = pval->slot_b;
	input.type_b = pval->type_b;
	input.port_b = pval->port_b;
	printf("DispOneroute\n");
	tmsdb_Select_oneline(&input, nodes, &count);

	struct trace_cache tc;
	char strout[1024];

	tc.empty  = 1024;
	tc.offset = 0;
	tc.limit  = 180;
	tc.strout = strout;
	DispRoute_V2(nodes, count,&tc);
	printf("%s",tc.strout);
}


/**
 * @brief	设备类型字符串转换成对于的设备序号
 * @param	name 设备名可以是osw、opm、olp、otdr、ols、mcu
 * @retval	0 字符串不是有效值
 */
#include <ctype.h>
int32_t DevStr2Int(char *name)
{
	if (strcmp(name,"osw") == 0) {
		return DEV_OSW;
	}
	else if (strcmp(name,"opm") == 0) {
		return DEV_OPM;
	}
	else if (strcmp(name,"olp") == 0) {
		return DEV_OLP;
	}
	else if (strcmp(name,"otdr") == 0) {
		return DEV_OTDR;
	}
	else if (strcmp(name,"ols") == 0) {
		return DEV_OLS;
	}
	else if (strcmp(name,"mcu") == 0) {
		return DEV_MCU;
	}
	else if (strcmp(name,"sms") == 0) {
		return DEV_SMS;
	}
	// return 0;
	return atoi(name);
}

void TestMD5()
{
	printf("testMD5\n");
	unsigned char *m_pcRecvFileContent;
	int len;
	FILE *fp;

	fp = fopen("md5.txt","rb");
	if (fp == NULL) {
		printf("error open file\n");
		return ;
	}
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	printf("flen %d\n", len);
	fseek(fp, 0, SEEK_SET);
	m_pcRecvFileContent = (unsigned char*)malloc(len);
	len = fread(m_pcRecvFileContent, 1, len, fp);
	printf("len = %d\n", len);

	fclose(fp);


	unsigned char md5int[16];
	unsigned char md5str[33];
	
	// CMD5::MD5Int((unsigned char *)m_pcRecvFileContent,len,md5int); //文件校验值计算
	// CMD5::MD5Int2Str(md5int, md5str);
	for (int i = 0; i < 16; i++) {
		printf("%02x",(unsigned char)md5int[i]);
	}
	printf("\n");
	printf("md5: %s\n",md5str);
	free(m_pcRecvFileContent);
}

int cmd_atb(int argc, char **argv)
{
	struct tdb_a_trigger_b input, mask, tinput;
	int flag = 0;

	// atb add <A frame/slot/type/port> <B frame/slot/type/port>
	if (argc == 10 && memcmp(argv[1], "add", strlen(argv[1])) == 0) {
		flag = 1;
	}
	// atb del <A/B frame/slot/type/port> 
	// atb del <A frame/slot/type/port> <B frame/slot/type/port> 
	else if ((argc == 6 || argc == 10) && memcmp(argv[1], "del", strlen(argv[1])) == 0) {
		flag = 2;
	}
		
	// add and del
	if (flag != 0) {

		input.frame_a = atoi(argv[2]);
		input.slot_a  = atoi(argv[3]);
		input.type_a = DevStr2Int(argv[4]);
		if ('c' == (argv[5][0] | 0x20)) {
			input.port_a  = 0xefffffff;
		}
		else {
			input.port_a  = atoi(argv[5]);
		}

		// atb add 
		if (flag == 1) {
			input.frame_b = atoi(argv[6]);
			input.slot_b  = atoi(argv[7]);
			input.type_b = DevStr2Int(argv[8]);
			if ('c' == (argv[5][0] | 0x20)) {
				input.port_b  = 0xefffffff;
			}
			else {
				input.port_b  = atoi(argv[9]);
			}

			tmsdb_Insert_a_trigger_b(&input, &mask,1);

			tms_Insert_TbUnit(sg_sockfdid, NULL, 1, (struct tms_unit*)&input.frame_a);
		}
		// atb del <A/B frame/slot/type/port> 
		else if (flag == 2 && argc == 6) {
_Delete:;
			bzero(&mask, sizeof(tdb_a_trigger_b_t));
			mask.frame_a = 1;
			mask.slot_a = 1;
			mask.type_a = 1;
			mask.port_a = 1;
			tmsdb_Delete_a_trigger_b(&input, &mask);


			input.frame_b = input.frame_a;
			input.slot_b = input.slot_a;
			input.type_b = input.type_a;
			input.port_b = input.port_a;

			bzero(&mask, sizeof(tdb_a_trigger_b_t));
			mask.frame_b = 1;
			mask.slot_b = 1;
			mask.type_b = 1;
			mask.port_b = 1;
			tmsdb_Delete_a_trigger_b(&input, &mask);
		}
		// atb del <A frame/slot/type/port> <B frame/slot/type/port> 
		else if (flag == 2 && argc == 10) {
			memcpy(&tinput, &input, sizeof(struct tdb_a_trigger_b));
			tms_Del_TbUnit(sg_sockfdid, NULL, 1, (struct tms_unit*)&tinput.frame_a);
			goto _Delete;
		}
		
	}
	// atb del all
	else if (argc >= 3 && memcmp(argv[1], "del", strlen(argv[1])) == 0 &&
		memcmp(argv[2], "all", strlen(argv[2])) == 0) {
		tms_DelAll_TbUnit(sg_sockfdid, NULL);
	}
	// atb list page 0 
	else if (argc >= 4 && memcmp(argv[1], "list", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "page", strlen(argv[2])) == 0) {

		tdb_a_trigger_b_t input;
		tdb_a_trigger_b_t mask;
		int from;
		// int val;

		bzero(&input, sizeof(tdb_a_trigger_b_t));
		bzero(&mask, sizeof(tdb_a_trigger_b_t));

		input.frame_a = 0;
		input.slot_a = 0;
		input.type_a = 0;
		input.slot_a = 0;

		// mask.frame_a = 1;
		// mask.slot_a = 1;
		// mask.type_a = 1;
		// mask.slot_a = 1;

		// 查找下一行，DB_MORE表示id大于某值
		if ('n' == argv[3][0]) {
			mask.id = DB_MORE;
			from = 0;
			input.id = sg_a_trigger_b_dblast.id;

		}
		// 从第from行开始查找，找到第10行结束
		else {
			from = atoi(argv[3]);

		}
		
		int tmp;
		tmp = sg_a_trigger_b_dblast.id;
		printf("-----------------------------------------\n");
		if (argc == 5 && memcmp(argv[4], "all", strlen(argv[4])) == 0) {
			// tmsdb_Select_Page_a_trigger_b(&input, &mask, from, 10, _cb_Select_a_trigger_b_page,NULL);	
		}
		else {
			tmsdb_Select_Page_a_trigger_b(&input, &mask, from, 10, _cb_Select_a_trigger_b_page,NULL);	
		}
		if (tmp == sg_a_trigger_b_dblast.id) {
			printf("Is end\n");
		}
	}
	else {
_Usage:;
		printf("\tatb add\n");
		printf("\tatb del\n");
	}


	
	
	
	return 0;
}

int cmd_route(int argc, char **argv)
{
	// unsigned char m_pcRecvFileContent[]="jklijiwoweiejrlkjweoiuronlqkjwleijoqiuq";
	// char cFileMd5Value[40] = {0};
	// printf("input %s\n",m_pcRecvFileContent);
	// CMD5::MD5((unsigned char *)m_pcRecvFileContent,10,cFileMd5Value); //文件校验值计算
	
	// printf("md5: %s\n",cFileMd5Value);
	// test();
	// TestMD5();
	if(argc >= 2 ) {
		tdb_route_t input, mask;

		bzero(&input, sizeof(tdb_route_t));
		bzero(&mask, sizeof(tdb_route_t));

		// route new 
		if (argc >= 2 && memcmp(argv[1], "new", strlen(argv[1])) == 0) {
			sg_route_index = 0;
		}
		// route add fa sa ta pa 
		else if (argc >= 6 && memcmp(argv[1], "add", strlen(argv[1])) == 0) {
			if (sg_route_index >= (MAX_ROUTE_NODE - 1)) {
				printf("node cache full\n");
			}
			if (sg_route_index == 0) {
				printf("new route\n");
				sg_route_index = 1;
				sg_route[0].frame_b = atoi(argv[2]);	
				sg_route[0].slot_b  = atoi(argv[3]);
				// sg_route[0].type_b  = atoi(argv[4]);
				sg_route[0].type_b = DevStr2Int(argv[4]);
				if ('c' == (argv[5][0] | 0x20)) {
					sg_route[0].port_b  = 0xefffffff;
					printf("%d \n",sg_route[0].port_b);
				}
				else {
					sg_route[0].port_b  = atoi(argv[5]);
				}


				sg_route[1].frame_a = sg_route[0].frame_b;
				sg_route[1].slot_a  = sg_route[0].slot_b;
				sg_route[1].type_a  = sg_route[0].type_b;
				sg_route[1].port_a  = sg_route[0].port_b;

				sg_route[1].frame_b = 0;
				sg_route[1].slot_b  = 0;
				sg_route[1].type_b  = 0;
				sg_route[1].port_b  = 0;

				if (argc >= (MAX_ROUTE_NODE + 1)) {
					sg_route[0].ip_src = atoi(argv[6]);
					sg_route[0].ip_dst = atoi(argv[7]);
					mask.ip_src = 1;
					mask.ip_dst = 1;
				}
				mask.frame_b = 1;
				mask.slot_b = 1;
				mask.type_b = 1;
				mask.port_b = 1;

				sg_route_index = 1;
				return 0;
			}

			sg_route[sg_route_index].frame_a = sg_route[sg_route_index - 1].frame_b;
			sg_route[sg_route_index].slot_a  = sg_route[sg_route_index - 1].slot_b;
			sg_route[sg_route_index].type_a  = sg_route[sg_route_index - 1].type_b;
			sg_route[sg_route_index].port_a  = sg_route[sg_route_index - 1].port_b;

			sg_route[sg_route_index].frame_b = atoi(argv[2]);
			sg_route[sg_route_index].slot_b  = atoi(argv[3]);
			// sg_route[sg_route_index].type_b  = atoi(argv[4]);
			sg_route[sg_route_index].type_b = DevStr2Int(argv[4]);
			// sg_route[sg_route_index].port_b  = atoi(argv[5]);
			if ('c' == (argv[5][0] | 0x20)) {
				sg_route[sg_route_index].port_b  = 0xefffffff;
			}
			else {
				sg_route[sg_route_index].port_b  = atoi(argv[5]);
			}


			// 下一个节点末端为0
			sg_route[sg_route_index + 1].frame_a = sg_route[sg_route_index].frame_b;
			sg_route[sg_route_index + 1].slot_a  = sg_route[sg_route_index].slot_b;
			sg_route[sg_route_index + 1].type_a  = sg_route[sg_route_index].type_b;
			sg_route[sg_route_index + 1].port_a  = sg_route[sg_route_index].port_b;

			sg_route[sg_route_index + 1].frame_b = 0;
			sg_route[sg_route_index + 1].slot_b = 0;
			sg_route[sg_route_index + 1].type_b = 0;
			sg_route[sg_route_index + 1].port_b = 0;
			

			mask.frame_b = 1;
			mask.slot_b = 1;
			mask.type_b = 1;
			mask.port_b = 1;

			// todo 检查有效性
			// tmsdb_Insert_route(&input,&mask,1);
			sg_route_index++;
		}
		// route del fa sa ta pa 
		else if (argc >= 6 && memcmp(argv[1], "del", strlen(argv[1])) == 0) {

			// Step 1.查询指定光路所有节点，节点长度最长为8
			tdb_route_t input;
			tdb_route_t nodes[MAX_ROUTE_NODE];
			int count = MAX_ROUTE_NODE;

			input.frame_b = atoi(argv[2]);
			input.slot_b = atoi(argv[3]);
			input.type_b = atoi(argv[4]);
			input.port_b = atoi(argv[5]);

			if (0 != tmsdb_Select_oneline(&input, nodes, &count) ) {
				printf("Route error\n");
			}
			
			// Step 2。如果存在路由则删除
			if (count > 0) {
				mask.id = 1;
				tmsdb_Delete_route(nodes, &mask, count);	

			}
			else {
				printf("Unexit route\n");
			}
			
		}
		// route del all
		else if (argc >= 3 && memcmp(argv[1], "del", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "all", strlen(argv[2])) == 0) {
			tdb_route_t input,mask;

			bzero(&input, sizeof(tdb_route_t));
			bzero(&mask, sizeof(tdb_route_t));

			tmsdb_Delete_route(&input, &mask, 1);
			tms_DelAll_TbRoute(sg_sockfdid, NULL);
		}
		// route go [save]
		else if (argc >= 2 && memcmp(argv[1], "go", strlen(argv[1])) == 0) {
			int ret;
			printf("route go [save]\n");
			mask.frame_a = 1;
			mask.slot_a = 1;
			mask.type_a = 1;
			mask.port_a = 1;

			// 检查有效性
			ret = tmsdb_Check_route_oneline(sg_route, sg_route_index + 1);
			if (0 != ret) {
				printf("Route invaild : code %d\n", ret);
				return -1;
			}

			// route go save
			if (argc >= 3 && memcmp(argv[2], "save", strlen(argv[2])) == 0 ) {
				printf("route go save\n");
				tmsdb_Insert_route(sg_route,&mask,sg_route_index + 1);	
			}
			// route go 
			else {
				printf("download\n");
				// todo 下发到MCU
				struct tms_route down[MAX_ROUTE_NODE];

				for (int i = 0; i < sg_route_index+1; i++) {
					down[i].frame_a	= sg_route[i].frame_a;
					down[i].slot_a	= sg_route[i].slot_a;
					down[i].type_a	= sg_route[i].type_a;
					down[i].port_a	= sg_route[i].port_a;
					down[i].frame_b	= sg_route[i].frame_b;
					down[i].slot_b	= sg_route[i].slot_b;
					down[i].type_b	= sg_route[i].type_b;
					down[i].port_b	= sg_route[i].port_b;
				}
				tms_TbRoute_Insert(sg_sockfdid, NULL, sg_route_index+1, down);
			}			
		}

		// route list
		else if (argc == 2 && memcmp(argv[1], "list", strlen(argv[1])) == 0) {
			struct trace_cache tc;
			char strout[1024];

			tc.strout = strout;
			tc.empty = 1024;
			tc.offset = 0;
			tc.limit = 180;
			DispRoute_V2(sg_route, sg_route_index+1, &tc);
			printf("%s", tc.strout);

		}
		// route list detail fa sa ta pa
		else if (argc >= 7 && memcmp(argv[1], "list", strlen(argv[1])) == 0 &&
					memcmp(argv[2], "detail", strlen(argv[2])) == 0) {
			tdb_route_t input;

			input.frame_b = atoi(argv[3]);
			input.slot_b  = atoi(argv[4]);
			// input.type_b  = atoi(argv[5]);
			// input.port_b  = atoi(argv[6]);
			input.type_b = DevStr2Int(argv[5]);
			if ('c' == (argv[6][0] | 0x20)) {
				input.port_b  = 0xefffffff;
			}
			else {
				input.port_b  = atoi(argv[6]);
			}
			DispOneRoute(&input);
			
		}
		// route list page n [all]
		else if (argc >= 4 && memcmp(argv[1], "list", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "page", strlen(argv[2])) == 0) {
			// route list
			tdb_route_t input;
			tdb_route_t mask;
			int from;
			// int val;

			bzero(&input, sizeof(tdb_route_t));
			bzero(&mask, sizeof(tdb_route_t));

			input.frame_a = 0;
			input.slot_a = 0;
			input.type_a = 0;
			input.slot_a = 0;

			mask.frame_a = 1;
			mask.slot_a = 1;
			mask.type_a = 1;
			mask.slot_a = 1;
			// 查找下一行，DB_MORE表示id大于某值
			if ('n' == argv[3][0]) {
				mask.id = DB_MORE;
				from = 0;
				input.id = sg_route_dblast.id;

			}
			// 从第from行开始查找，找到第10行结束
			else {
				from = atoi(argv[3]);

			}
			
			int tmp;
			tmp = sg_route_dblast.id;
			printf("-----------------------------------------\n");
			if (argc == 5 && memcmp(argv[4], "all", strlen(argv[4])) == 0) {
				tmsdb_Select_Page_route(&input, &mask, from, 10, _cb_Select_route_page_detail,NULL);	
			}
			else {
				tmsdb_Select_Page_route(&input, &mask, from, 10, _cb_Select_route_page_brief,NULL);
			}
			if (tmp == sg_route_dblast.id) {
				printf("Is end\n");
			}
			
			
			
		}
	}
	return 0;
}
void sh_analyse (char *fmt, long len);
// int cmd_tmsall(int argc, char **argv)
// {
// 	int32_t frame;
// 	int32_t slot;
// 	int32_t type;


// 	// printf("%s \n%s \n%s \n%s\n", dev_name, cable_name, start_name, end_name);
// 	// return 0;
// 	char sbuf[1024];
// 	int fd = 0;
// 	int allcmd = 0;
// 	int port = 0;

// 	// tms_RetOTDRTest(0, 0, 0, 0, 3, 0, 0, 0, 0);
// 	// return 0;
// 	fd = sg_sockfdid;
// 	// tms_Tick(5);

	
// 	if (argc == 2 && strcmp(argv[1], "rsn") == 0) {
// 		uint8_t sn[128] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
// 		tms_RetSerialNumber(fd, NULL, &sn);
// 	}
// 	else if (argc == 5 && strcmp(argv[1], "sip") == 0) {
// 		uint8_t ip[16] = "192.168.2.2";
// 		uint8_t mask[16] = "255.255..0.0";
// 		uint8_t gw[16] = "192.168.2.1";

// 		snprintf((char*)ip, 16, "%s", argv[2]);
// 		snprintf((char*)mask, 16, "%s", argv[3]);
// 		snprintf((char*)gw, 16, "%s", argv[4]);
// 		tms_SetIPAddress(fd, NULL, ip, mask, gw);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "gcom") == 0) {
// 		tms_GetDeviceComposition(fd, NULL);//, sg_frameid, sg_slotid);
// 	}

// 	else if(argc == 2 && strcmp(argv[1], "gdev") == 0) {
// 		tms_MCU_GetDeviceType(fd, NULL);//, sg_frameid, sg_slotid);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "gsn") == 0) {
// 		tms_GetSerialNumber(fd, NULL);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "cfgsmsa") == 0) {
// 		struct tms_cfg_sms_val val[6];
// 		for (int i = 0; i < 6; i++) {
// 			val[i].time = i;
// 			memcpy(&val[i].phone[0], "123456789012345", 16);
// 			val[i].type = i;
// 			val[i].level = i & 0x07;
// 		}
// 		tms_CfgSMSAuthorization(fd, NULL, 6, val);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "clsmsa") == 0) {
// 		tms_ClearSMSAuthorization(fd, NULL);
// 	}
// 	// OSW
// 	else if(argc == 3 && strcmp(argv[1], "cfgoswcyc") == 0) {
// 		struct tms_cfg_mcu_osw_cycle_val val[3];
// 		val[0].port = atoi(argv[2]);
// 		val[0].iscyc    = 1;
// 		val[0].interval = 15;
// 		tms_CfgMCUOSWCycle(fd, NULL, sg_frameid, sg_slotid, 1, val);
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "closwcyc") == 0) {
// 		struct tms_cfg_mcu_osw_cycle_val val[3];
// 		val[0].port = atoi(argv[2]);
// 		val[0].iscyc    = 0;
// 		val[0].interval = 15;
// 		tms_CfgMCUOSWCycle(fd, NULL, sg_frameid, sg_slotid, 1, val);	
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "oswsw") == 0) {
// 		port = atoi(argv[2]);
// 		if ( (unsigned int)port < 7) {
// 			tms_MCU_OSWSwitch(fd, NULL, sg_frameid, sg_slotid, port);	
// 		}
// 		else {
// 			printf("Error over range\n");
// 		}
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "oswp") == 0) {
// 		uint8_t dev_name[64];
// 		uint8_t cable_name[64];
// 		uint8_t start_name[64];
// 		uint8_t end_name[64];
		
// 		port = atoi(argv[2]) & 0x07;
// 		snprintf((char*)dev_name, 64, "dev_name port %d", port);
// 		snprintf((char*)cable_name, 64, "cable_name port %d", port);
// 		snprintf((char*)start_name, 64, "start_name port %d", port);
// 		snprintf((char*)end_name, 64, "end_name port %d", port);
		
// 		tms_CfgMCUOSWPort(fd, NULL, sg_frameid, sg_slotid, port, &dev_name, &cable_name, &start_name, &end_name);
// 	}
// 	else if (argc == 2 && strcmp(argv[1], "adjt") == 0) {
// 		int8_t adjtime[20]="abcdefg";
// 		tms_AdjustTime(fd, NULL, &adjtime);
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "trace") == 0) {
// 		tms_Trace(NULL, argv[2], strlen(argv[2]), LEVEL_TC);

// 	}
// 	else if (argc >= 3 && strcmp(argv[1], "cmd") == 0) {
// 		char strout[256];
// 		strout[0] = '\0';

// 		for (int i = 2; i < argc; i++) {
// 			strcat(strout, argv[i]);
// 			strcat(strout, " ");
// 		}

// 		strout[strlen(strout)-1] = '\0';
// 		// printf("len %d send cmd %s\n", strlen(strout), strout);
		
// 		tms_Command(sg_sockfdid, NULL, strout, strlen(strout)+1);

// 	}
// 	else if(argc == 3 && strcmp(argv[1], "oswclear") == 0) {
// 		struct tms_cfg_mcu_any_port_clear_val pval[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
// 		int count = 0;
// 		sscanf(argv[2], "%d/%d/%d/%d/%d/%d/%d/%d", 
// 				&pval[0].any_port, &pval[1].any_port, &pval[2].any_port, &pval[3].any_port, 
// 				&pval[4].any_port, &pval[5].any_port, &pval[6].any_port, &pval[7].any_port);
		
// 		for (int i = 0; i < 8; i++) {
// 			if (pval[i].any_port != -1) {
// 				count++;
// 			}
// 			else {
// 				break;
// 			}
// 		}
// 		// tms_CfgMCUOPMPortClear(fd, NULL, sg_frameid, sg_slotid, count, pval);
// 		tms_CfgMCUOSWPortClear(fd, NULL, sg_frameid, sg_slotid, count, pval);

// 	}
	
// 	else if(argc == 3 && strcmp(argv[1], "encu") == 0) {
// 		g_en_connect_cu = atoi(argv[2]);
// 	}


// 	else if(argc == 3 && strcmp(argv[1], "smstxt") == 0) {
// 		char phone[16] = "18777399591";
// 		//QT里面这一行编译不过的
// 		wchar_t wcstr[20] = L"字符测试123abc";
// 		// uint16_t wcstr[20];// = L"字符测试123abc";
		

// 		tms_SendSMS(fd, NULL, &phone, 20, wcstr);
// 	}


	
// 	else if(argc == 2 && strcmp(argv[1], "rdevc") == 0) {
// 		struct tms_dev_composition_val list[8];
// 		for (int i = 0; i < 8; i++) {
// 			list[i].frame = i;
// 			list[i].slot = i;
// 			list[i].type = 2;
// 			list[i].port = 3;
// 		}
// 		tms_RetDeviceComposition(fd, NULL, 2, list);
// 		// 
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "update") == 0) {
// 		UpdateOTDR("update.txt", "", 12345);
// 	}
// 	else if(argc == 2 && (
// 		strcmp(argv[1], "getotdr") == 0 ||
// 		strcmp(argv[1], "getotdrc") == 0)  ){

// 		struct tms_getotdr_test_param test_param;
// 		test_param.rang =  30000;
// 		test_param.wl	= 1550;
// 		test_param.pw   = 640;
// 		test_param.time = 15;
// 		test_param.gi   = 1.4685;
// 		test_param.end_threshold = 9.0;
// 		test_param.none_reflect_threshold = 2.0;
// 		test_param.reserve0 = 0;
// 		test_param.reserve1 = 0;
// 		test_param.reserve2 = 0;
		
// 		if (strcmp(argv[1], "getotdr") == 0) {
// 			tms_GetOTDRTest(fd, NULL, 0, 8,  0, &test_param);
// 		}
// 		else if(strcmp(argv[1], "getotdrc") == 0) {
// 			tms_GetOTDRTestCycle(fd, NULL, 0, 8,  0, &test_param);	
// 		}
// 	}
// 	else if( argc == 2 && (  
// 			strcmp(argv[1], "retotdr") == 0 || 
// 			strcmp(argv[1], "refotdr") == 0 || 
// 			strcmp(argv[1], "retotdrc") == 0 )	) {

// 		struct tms_otdr_ref_hdr     ref_hdr;
// 		struct tms_retotdr_test_hdr test_hdr;
// 		struct tms_retotdr_test_param test_param;
// 		struct tms_retotdr_data_hdr data_hdr = {0}; 
// 		struct tms_retotdr_data_val data_val[10]= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

// 		struct tms_retotdr_event_hdr event_hdr = {0};
// 		struct tms_retotdr_event_val event_val[4]= {
// 			{0, 0, 3.1, 3.0, 3.9, 0.0}, 
// 			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
// 			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
// 			{3, 3, 3.0, 3.2, 3.3, 3.4}};
// 		struct tms_retotdr_chain chain;
// 		struct tms_cfg_otdr_ref_val ref_data;

// 		ref_hdr.osw_frame = 15;
// 		ref_hdr.osw_slot  = 0;
// 		ref_hdr.osw_type = DEV_OSW;
// 		ref_hdr.osw_port = 1;
// 		ref_hdr.osw_slot  = 0;
// 		ref_hdr.osw_port = 0;



// 		test_hdr.osw_frame = 15;
// 		test_hdr.osw_slot  = 0;
// 		test_hdr.osw_type = DEV_OSW;
// 		test_hdr.osw_port = 1;
// 		memcpy(&test_hdr.time, "[TIME]", 7);
// 		test_hdr.otdr_frame = 1;
// 		test_hdr.otdr_slot = 2;
// 		test_hdr.otdr_type = DEV_OTDR;
// 		test_hdr.otdr_port = 3;




// 		test_param.rang = 31111;
// 		test_param.wl = 1550;			
// 		test_param.pw =1280		;		
// 		test_param.time = 15;				
// 		test_param.gi = 1.2345;				
// 		test_param.end_threshold = 9.0;
// 		test_param.none_reflect_threshold = 8.8;
// 		test_param.sample = 20000000;
// 		test_param.reserve0 = 0;
// 		test_param.reserve1 = 0;	

// 		strcpy((char*)data_hdr.dpid , "OTDRData");
// 		// printf("data.dpid %s\n", data_hdr.dpid);
// 		data_hdr.count = 10;
// 		strcpy((char*)event_hdr.eventid, "KeyEvents");
// 		// printf("event_hdr.eventid %s\n", event_hdr.eventid);
// 		event_hdr.count = 4;
// 		strcpy((char*)chain.inf, "OTDRTestResultInfo");
// 		chain.range = 1200;
// 		chain.loss = 10;
// 		chain.att = 1.23;


// 		ref_data.leve0 = 123;
// 		ref_data.leve1 = 456;
// 		ref_data.leve2 = 789;

// 		if (strcmp(argv[1], "retotdr") == 0) {
// 			// tms_RetOTDRTest(fd, NULL, 
// 			// 	&test_hdr, &test_param, 
// 			// 	&data_hdr, data_val, 
// 			// 	&event_hdr, event_val, 
// 			// 	&chain);
// 			tms_RetOTDRCycle(fd, NULL, 
// 				&test_hdr, &test_param, 
// 				&data_hdr, data_val, 
// 				&event_hdr, event_val, 
// 				&chain);
// 		}
// 		else if (strcmp(argv[1], "retotdrc") == 0) {
// 			tms_RetOTDRTestCycle(fd, NULL, 
// 				&test_hdr, &test_param, 
// 				&data_hdr, data_val, 
// 				&event_hdr, event_val, 
// 				&chain);
// 		}
// 		else {
// 			tms_CfgOTDRRef(fd, NULL, 
// 				&ref_hdr, &test_param, 
// 				&data_hdr, data_val, 
// 				&event_hdr, event_val, 
// 				&chain, 
// 				&ref_data);	
// 		}
		
// 	}
// 	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
// 		// 
// 	}
// 	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
// 		// 
// 	}
// 	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
// 		// 
// 	}
// 	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
// 		// 
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "tick") == 0) {
// 		tms_Tick(fd, NULL);
// 	}
// 	else if(allcmd && strcmp(argv[1], "rdev") == 0) {
// 		tms_MCU_RetDeviceType(fd, NULL, sg_frameid, sg_slotid, 0xc3, 0xd4);
// 	}
// 	else if(allcmd && strcmp(argv[1], "opma") == 0) {
// 		tms_MCU_GetOLPAlarm(fd, NULL, sg_frameid, sg_slotid);
// 	}


// 	else if(argc == 2 && strcmp(argv[1], "olpa") == 0) {
// 		tms_MCU_GetOLPAlarm(fd, NULL, sg_frameid, sg_slotid);
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "olpstart") == 0) {
// 		if ('m' == (argv[2][0] | 0x20)) {
// 			tms_MCU_OLPStartOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_A);
// 		}
// 		else if ('s' == (argv[2][0] | 0x20)) {
// 			tms_MCU_OLPStartOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_B);
// 		}
// 		else {
// 			printf("Error over range\n");
// 			printf("\tM\tmaster line start\n");
// 			printf("\tS\tslave line start\n");
// 		}
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "olpfinish") == 0) {
// 		if ('m' == (argv[2][0] | 0x20)) {
// 			tms_MCU_OLPFinishOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_A);
// 		}
// 		else if ('s' == (argv[2][0] | 0x20)) {
// 			tms_MCU_OLPFinishOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_B);
// 		}
// 		else {
// 			printf("Error over range\n");
// 			printf("\tM\tmaster line finish\n");
// 			printf("\tS\tslave line finish\n");
// 		}
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "cfgolp") == 0){
// 		int32_t port;
// 		uint8_t dev_name[64] =  "dev    name";
// 		uint8_t cable_name[64]= "cable  name";
// 		uint8_t start_name[64]= "start  name";
// 		uint8_t end_name[64]=   "end    name" ;

// 		port = atoi(argv[2]);
// 		strcat((char*)dev_name, argv[2]);
// 		strcat((char*)cable_name, argv[2]);
// 		strcat((char*)start_name, argv[2]);
// 		strcat((char*)end_name, argv[2]);
		
// 		tms_CfgMCUOLPPort(fd, NULL, sg_frameid, sg_slotid, port , &dev_name, &cable_name, &start_name, &end_name);
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "clolp") == 0){
// 		struct tms_cfg_mcu_any_port_clear_val list[2];
// 		list[0].any_port = 0;
// 		list[1].any_port = atoi(argv[2]);
// 		printf("%d %d\n", list[0].any_port, list[1].any_port	);

// 		tms_CfgMCUOLPPortClear(fd, NULL, sg_frameid, sg_slotid, 2, list);
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "cfgolpu") == 0){
// 		struct tms_cfg_mcu_u_any_osw_val list[2];
// 		list[0].any_port  = 3;
// 		list[0].osw_frame = 11;
// 		list[0].osw_slot  = 2;
// 		// list[0].osw_type = 0;
// 		list[0].osw_port  = 3;

// 		list[1].any_port  = atoi(argv[2]);
// 		list[1].osw_frame = 11;
// 		list[1].osw_slot  = 3;
// 		// list[1].osw_type = 0;
// 		list[1].osw_port  = 4;

// 		tms_CfgMCUUniteOLPOSW(fd, NULL, sg_frameid, sg_slotid, 2, list);
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "clolpu") == 0){
// 		struct tms_cfg_mcu_any_port_clear_val list[2];
// 		list[0].any_port = 3;
// 		list[1].any_port = atoi(argv[2]);
// 		printf("%d %d\n", list[0].any_port, list[1].any_port	);

// 		tms_CfgMCUUniteOLPOSWClear(fd, NULL, sg_frameid, sg_slotid, 2, list);
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "cfgopmu") == 0){
// 		struct tms_cfg_mcu_u_any_osw_val list[2];
// 		list[0].any_port  = 0;
// 		list[0].osw_frame = 12;
// 		list[0].osw_slot  = 2;
// 		// list[0].osw_type = 0;
// 		list[0].osw_port  = 3;

// 		list[1].any_port  = atoi(argv[2]);
// 		list[1].osw_frame = 12;
// 		list[1].osw_slot  = 3;
// 		// list[1].osw_type = 0;
// 		list[1].osw_port  = 4;

// 		tms_CfgMCUUniteOPMOSW(fd, NULL, sg_frameid, sg_slotid, 2, list);
// 	}
// 	else if (argc == 3 && strcmp(argv[1], "clopmu") == 0){
// 		struct tms_cfg_mcu_any_port_clear_val list[2];
// 		list[0].any_port = 0;
// 		list[1].any_port = atoi(argv[2]);
// 		printf("%d %d\n", list[0].any_port, list[1].any_port	);

// 		tms_CfgMCUUniteOPMOSWClear(fd, NULL, sg_frameid, sg_slotid, 1, list);
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "olpref") == 0 &&
// 		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

// 		tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 3, sg_olpref);
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "olpref") == 0 && 
// 		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		
// 		tms_Print_tms_cfg_olp_ref_val(sg_olpref, 3);
// 	}
// 	else if(argc >= 3 && strcmp(argv[1], "olpref") == 0) {
		
// 		int index;
// 		index = atoi(argv[2]);
// 		if (index >= 3) {
// 			printf("out of rang <0~2>\n");
// 			return -1;
// 		}
// 		sg_olpref[index].port = index;
// 		if (sg_olpref[index].port > 2) {
// 			printf("port <0~2>\n");
// 			return -1;
// 		}
// 		// 功率设置/缺省值
// 		if (argc >= 4) {
// 			sg_olpref[index].ref_power = atoi(argv[3]);
// 		}
// 		else {
// 			sg_olpref[index].ref_power = sg_olpref_def.ref_power;
// 		}

// 		// 告警门限设置/缺省值
// 		if (argc == 7) {
// 			sg_olpref[index].leve0 = atoi(argv[4]);
// 			sg_olpref[index].leve1 = atoi(argv[5]);
// 			sg_olpref[index].leve2 = atoi(argv[6]);	
// 		}
// 		else {
// 			sg_olpref[index].leve0 = sg_olpref_def.leve0;
// 			sg_olpref[index].leve1 = sg_olpref_def.leve1;
// 			sg_olpref[index].leve2 = sg_olpref_def.leve2;
// 		}
// 		sg_olpref[index].isminitor = 1;
// 		sg_olpref[index].wave      = 1550;

// 		sg_olpref_def = sg_olpref[index];
		
// 		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "olpop") == 0) {
// 		tms_GetOLPOP(fd, NULL, sg_frameid, sg_slotid);
// 	}

// 	else if(argc == 2 && strcmp(argv[1], "opmop") == 0) {
// 		tms_GetOPMOP(fd, NULL, sg_frameid, sg_slotid);
// 	}
	
// 	else if(argc == 2 && strcmp(argv[1], "cfgolpm") == 0) {
// 		tms_CfgOLPMode(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_MODE_UNBACK, 1, 0);
// 	}

// 	else if(argc == 3 && strcmp(argv[1], "olpsw") == 0) {
// 		port = atoi(argv[2]);
// 		if ( (unsigned int)port < 7) {
// 			tms_MCU_OLPSwitch(fd, NULL, sg_frameid, sg_slotid, port);	
// 		}
// 		else {
// 			printf("Error over range\n");
// 		}
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "olprep") == 0) {
// 		tms_ReportOLPAction(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_PERSION, OLP_SWITCH_A);
// 	}
	




// 	else if(argc == 2 && strcmp(argv[1], "gver") == 0) {
// 		struct tms_devbase devbase;
// 		if (tms_GetDevBaseByLocation(sg_frameid, sg_slotid, &devbase) == 0) {
// 			printf("f%d/s%d unexit\n", sg_frameid, sg_slotid);
// 		}
// 		tms_GetVersion(sg_sockfdid, NULL, sg_frameid, sg_slotid, devbase.type);
// 	}
// 	else if(allcmd && strcmp(argv[1], "rver") == 0) {
// 		tms_RetVersion(fd, NULL, sg_frameid, sg_slotid, DEV_OPM, (uint8_t*)("1.1.1.1.1"));
// 	}
// 	else if(0 && (allcmd && strcmp(argv[1], "update") == 0)) {
// 		char data[40] = {0};
// 		char fname[256] = "abcdefg";
// 		tms_Update(fd, NULL, 0x01, 0x02, DEV_OPM, (uint8_t(*)[256])&fname, 40, (uint8_t*)data);
// 	}
// 	else if(allcmd && strcmp(argv[1], "ack") == 0) {
// 		tms_Ack(fd, NULL, RET_SUCCESS, 0x1);
// 	}
// 	// else if(argc == 2 && strcmp(argv[1], "opmref") == 0) {
// 	// 	struct tms_cfg_opm_ref_val opmlist[8];
// 	// 	for (int i = 0; i < 8; i++) {
// 	// 		opmlist[i].port = i;
// 	// 		opmlist[i].isminitor = 0x01;
// 	// 		opmlist[i].wave = 1550;
// 	// 		opmlist[i].ref_power = 400;
// 	// 		opmlist[i].leve0 = 0xa1;
// 	// 		opmlist[i].leve1 = 0xb2;
// 	// 		opmlist[i].leve2 = 0xc3;
// 	// 	}
// 	// 	tms_CfgOPMRefLevel(fd, NULL, sg_frameid, sg_slotid, 8, opmlist);
// 	// }
// 	else if(argc == 3 && strcmp(argv[1], "opmref") == 0 &&
// 		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

// 		tms_CfgOPMRefLevel(fd, NULL, sg_frameid, sg_slotid, 8, sg_opmref);
// 	}
// 	else if(argc == 3 && strcmp(argv[1], "opmref") == 0 && 
// 		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
// 		tms_Print_tms_cfg_opm_ref_val(&sg_opmref[0], 8);
// 		// printf("    port  ism  wave  r_power   lv0    lv1  lv2\n");
// 		// for (int i = 0; i < 8; i++) {
// 		// 	printf("%6.1d%5d  %6d%8d %6d%6d%6d\n",
// 		// 		sg_opmref[i].port,
// 		// 		sg_opmref[i].isminitor,
// 		// 		sg_opmref[i].wave,
// 		// 		sg_opmref[i].ref_power,
// 		// 		sg_opmref[i].leve0,
// 		// 		sg_opmref[i].leve1,
// 		// 		sg_opmref[i].leve2);
// 		// }
// 	}
// 	else if(argc >= 3 && strcmp(argv[1], "opmref") == 0) {
		
// 		int index;
// 		index = atoi(argv[2]);
// 		if (index >= 8) {
// 			printf("out of rang <0~7>\n");
// 			return -1;
// 		}
// 		sg_opmref[index].port = index;
		
// 		// 功率设置/缺省值
// 		if (argc >= 4) {
// 			sg_opmref[index].ref_power = atoi(argv[3]);
// 		}
// 		else {
// 			sg_opmref[index].ref_power = sg_opmref_def.ref_power;
// 		}

// 		// 告警门限设置/缺省值
// 		if (argc == 7) {
// 			sg_opmref[index].leve0 = atoi(argv[4]);
// 			sg_opmref[index].leve1 = atoi(argv[5]);
// 			sg_opmref[index].leve2 = atoi(argv[6]);	
// 		}
// 		else {
// 			sg_opmref[index].leve0 = sg_opmref_def.leve0;
// 			sg_opmref[index].leve1 = sg_opmref_def.leve1;
// 			sg_opmref[index].leve2 = sg_opmref_def.leve2;
// 		}
// 		sg_opmref[index].isminitor = 1;
// 		sg_opmref[index].wave      = 1550;

// 		sg_opmref_def = sg_opmref[index];
		
// 		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
// 	}



	

	
// 	else if(allcmd && strcmp(argv[1], "aopm") == 0) {
// 		struct tms_alarm_opm_val alarmopmlist[8];

// 		for (int i = 0; i < 8; i++) {
// 			alarmopmlist[i].port = i;
// 			alarmopmlist[i].levelx = 1;
// 			alarmopmlist[i].power = 200;
// 			memcpy(alarmopmlist[i].time, (uint8_t*)"12345", sizeof("12345"));
// 		}
// 		// tms_AlarmOPM(fd, sg_frameid, sg_slotid, 1, 8, alarmopmlist);
// 		tms_AlarmOPM(fd, NULL, sg_frameid, sg_slotid, 8, alarmopmlist);
// 	}
// 	else if(allcmd && strcmp(argv[1], "aopmc") == 0) {
// 		struct tms_alarm_opm_val alarmopmlist[8];
// 		for (int i = 0; i < 8; i++) {
// 			alarmopmlist[i].port = i;
// 			alarmopmlist[i].levelx = 1;
// 			alarmopmlist[i].power = 200;
// 			memcpy(alarmopmlist[i].time, (uint8_t*)"bbbb", sizeof("bbbb"));
// 		}
// 		// tms_AlarmOPMChange(fd, sg_frameid, sg_slotid, 1, 6, 8, alarmopmlist);
// 		tms_AlarmOPMChange(fd, NULL, sg_frameid, sg_slotid, 6, 8, alarmopmlist);
// 	}
// 	else if(allcmd && strcmp(argv[1], "gopm") == 0) {
// 		tms_GetOPMOP(fd, NULL, sg_frameid, sg_slotid);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "ropm") == 0) {
// 		struct tms_any_op_val opmvallist[2];
// 		opmvallist[0].port = 1;
// 		opmvallist[0].power = 200;
// 		opmvallist[1].port = 2;
// 		opmvallist[1].power = 400;
// 		tms_RetOPMOP(fd, NULL, sg_frameid, sg_slotid, 2, opmvallist);
// 	}
// 	else if(argc == 2 && strcmp(argv[1], "rolp") == 0) {
// 		struct tms_any_op_val opmvallist[2];
// 		opmvallist[0].port = 1;
// 		opmvallist[0].power = 200;
// 		opmvallist[1].port = 2;
// 		opmvallist[1].power = 400;
// 		tms_RetOLPOP(fd, NULL, sg_frameid, sg_slotid, 2, opmvallist);
// 	}
// 	else {
// _Usage:;
// 		printf("Usage:\n");
// 		printf("\tcmd\n");
// 		printf("\t\toswsw <port>\n");
// 		printf("\t\tgdev\n");
// 		printf("\t\trdev\n");
// 		printf("\t\topmp\n");
// 		printf("\t\tolpp\n");
// 		printf("\t\tgver\n");
// 		printf("\t\trver\n");
// 		printf("\t\tack\n");
// 		printf("\t\topmref\n");
// 		printf("\t\taopm\n");
// 		printf("\t\tgopm\n");
// 		printf("\t\tropm\n");
// 	}
// 	return 0;
	

// 	// tms_MCU_GetDeviceType(fd, 0xa1, 0xb2);
// 	// tms_MCU_RetDeviceType(fd, 0xa1, 0xb2, 0xc3, 0xd4);

// 	// tms_MCU_GetOPMRayPower(fd, 0xa1, 0xb2);
// 	// tms_MCU_GetOLPRayPower(fd, 0xa1, 0xb2);


	
// 	return 0;
// }

int cmd_tmsall(int argc, char **argv)
{
	// int32_t frame;
	// int32_t slot;
	// int32_t type;


	// printf("%s \n%s \n%s \n%s\n", dev_name, cable_name, start_name, end_name);
	// return 0;
	// char sbuf[1024];
	int fd = 0;
	int allcmd = 0;
	int port = 0;

	// tms_RetOTDRTest(0, 0, 0, 0, 3, 0, 0, 0, 0);
	// return 0;
	fd = sg_sockfdid;
	// tms_Tick(5);

	if (argc == 2 && strcmp(argv[1], "alarmhw") == 0) {
		struct tms_alarm_hw_val val;
		val.frame = 1;
		val.level = 2;
		val.slot = 3;
		strcpy((char*)val.reason, "abcefawerawe 吴梦龙 reason");
		strcpy((char*)val.time, "2015");

		struct glink_addr gl;
		gl.src = GLINK_4412_ADDR;
		gl.dst = GLINK_MANAGE_ADDR;
		gl.pkid = 1;
		tms_AlarmHW(sg_sockfdid, &gl, 1, &val);
	}
	
	else if (argc == 2 && strcmp(argv[1], "rsn") == 0) {
		uint8_t sn[128] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
		tms_RetSerialNumber(fd, NULL, &sn);
	}
	else if (argc == 5 && strcmp(argv[1], "sip") == 0) {
		uint8_t ip[16] = "192.168.2.2";
		uint8_t mask[16] = "255.255..0.0";
		uint8_t gw[16] = "192.168.2.1";

		snprintf((char*)ip, 16, "%s", argv[2]);
		snprintf((char*)mask, 16, "%s", argv[3]);
		snprintf((char*)gw, 16, "%s", argv[4]);
		tms_SetIPAddress(fd, NULL, ip, mask, gw);
	}
	else if(argc == 2 && strcmp(argv[1], "gcom") == 0) {
		tms_GetDeviceComposition(fd, NULL);//, sg_frameid, sg_slotid);
	}

	else if(argc == 2 && strcmp(argv[1], "gdev") == 0) {
		tms_MCU_GetDeviceType(fd, NULL);//, sg_frameid, sg_slotid);
	}
	else if(argc == 2 && strcmp(argv[1], "gsn") == 0) {
		tms_GetSerialNumber(fd, NULL);
	}
	else if(argc == 2 && strcmp(argv[1], "cfgsmsa") == 0) {
		struct tms_cfg_sms_val val[6];
		for (int i = 0; i < 6; i++) {
            val[i].time = i + 1;
			memcpy(&val[i].phone[0], "123456789012345", 16);
			val[i].type = i;
			val[i].level = i & 0x07;
		}
		tms_CfgSMSAuthorization(fd, NULL, 6, val);
	}
	else if(argc == 2 && strcmp(argv[1], "clsmsa") == 0) {
		tms_ClearSMSAuthorization(fd, NULL);
	}
	// OSW
	else if(argc == 3 && strcmp(argv[1], "cfgoswcyc") == 0) {
		struct tms_cfg_mcu_osw_cycle_val val[3];
		val[0].port = atoi(argv[2]);
		val[0].iscyc    = 1;
		val[0].interval = 15;
        sg_frameid = 0;
        sg_slotid = 1;
		tms_CfgMCUOSWCycle(fd, NULL, sg_frameid, sg_slotid, 1, val);
	}
	else if(argc == 3 && strcmp(argv[1], "closwcyc") == 0) {
		struct tms_cfg_mcu_osw_cycle_val val[3];
		val[0].port = atoi(argv[2]);
		val[0].iscyc    = 0;
		val[0].interval = 15;
        sg_frameid = 0;
        sg_slotid = 1;
		tms_CfgMCUOSWCycle(fd, NULL, sg_frameid, sg_slotid, 1, val);	
	}
	else if(argc == 3 && strcmp(argv[1], "oswsw") == 0) {
		port = atoi(argv[2]);
		if ( (unsigned int)port < 7) {
			tms_MCU_OSWSwitch(fd, NULL, sg_frameid, sg_slotid, port);	
		}
		else {
			printf("Error over range\n");
		}
	}
	else if(argc == 3 && strcmp(argv[1], "oswp") == 0) {
		uint8_t dev_name[64];
		uint8_t cable_name[64];
		uint8_t start_name[64];
		uint8_t end_name[64];
		
		port = atoi(argv[2]) & 0x07;
		snprintf((char*)dev_name, 64, "dev_name port %d", port);
		snprintf((char*)cable_name, 64, "cable_name port %d", port);
		snprintf((char*)start_name, 64, "start_name port %d", port);
        snprintf((char*)end_name, 64, "end_name port %d", port);
        sg_frameid = 0;
        sg_slotid = 1;
        port = 0;
		tms_CfgMCUOSWPort(fd, NULL, sg_frameid, sg_slotid, port, &dev_name, &cable_name, &start_name, &end_name);
	}
	else if (argc == 2 && strcmp(argv[1], "adjt") == 0) {
		int8_t adjtime[20]="abcdefg";
		tms_AdjustTime(fd, NULL, &adjtime);
	}
	else if (argc == 3 && strcmp(argv[1], "trace") == 0) {
		// tms_Trace(NULL, argv[2], strlen(argv[2]), LEVEL_TC);
		printf("trace sssss\n");
		tms_Trace(NULL, argv[2], strlen(argv[2]), LEVEL_R_CMD);
	}
	else if (argc >= 3 && strcmp(argv[1], "cmd") == 0) {
		char strout[256];
		strout[0] = '\0';

		for (int i = 2; i < argc; i++) {
			strcat(strout, argv[i]);
			strcat(strout, " ");
		}

		strout[strlen(strout)-1] = '\0';
		// printf("len %d send cmd %s\n", strlen(strout), strout);
		
		tms_Command(sg_sockfdid, NULL, strout, strlen(strout)+1);

	}
	else if(argc == 3 && strcmp(argv[1], "oswclear") == 0) {
		struct tms_cfg_mcu_any_port_clear_val pval[8] = {{-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}};
		int count = 0;
		sscanf(argv[2], "%d/%d/%d/%d/%d/%d/%d/%d", 
				&pval[0].any_port, &pval[1].any_port, &pval[2].any_port, &pval[3].any_port, 
				&pval[4].any_port, &pval[5].any_port, &pval[6].any_port, &pval[7].any_port);
		
		for (int i = 0; i < 8; i++) {
			if (pval[i].any_port != -1) {
				count++;
			}
			else {
				break;
			}
		}
        sg_frameid = 0;
        sg_slotid = 1;
		// tms_CfgMCUOPMPortClear(fd, NULL, sg_frameid, sg_slotid, count, pval);
		tms_CfgMCUOSWPortClear(fd, NULL, sg_frameid, sg_slotid, count, pval);

	}
	
	else if(argc == 3 && strcmp(argv[1], "encu") == 0) {
		g_en_connect_cu = atoi(argv[2]);
	}


	else if(argc == 3 && strcmp(argv[1], "smstxt") == 0) {

		char phone[16] = "18777399591";
		//QT里面这一行编译不过的
		// wchar_t wcstr[30] =  L"字符测试123abc";
		// wchar_t wcstra[30] = L"字符测试经理问阿哈里斯";
		char a[100]= {0};
		int len;
		len = inputsms(a,100);
		PrintfMemory((uint8_t*)a,30);
		// wprintf(L"out %ls\n",a);
		// uint16_t wcstr[20];// = L"字符测试123abc";
		// printf("---%d---\n",strlen((char*)wcstr));
		// printf("---%d---\n",strlen((char*)wcstra));
		// PrintfMemory((uint8_t*)wcstr,30);
		// PrintfMemory((uint8_t*)wcstra,30);

		tms_SendSMS(fd, NULL, &phone, len, (wchar_t*)a);
	}


	
	else if(argc == 2 && strcmp(argv[1], "rdevc") == 0) {
		struct tms_dev_composition_val list[8];
		struct glink_addr gl;
		gl.src = GLINK_4412_ADDR;
		gl.dst = GLINK_MANAGE_ADDR;
		gl.pkid = 1;

		for (int i = 0; i < 8; i++) {
			list[i].frame = i;
			list[i].slot = i;
			list[i].type = 2;
			list[i].port = 3;
		}
		tms_RetDeviceComposition(fd, &gl, 2, list);
		// 
	}
	else if(argc == 3 && strcmp(argv[1], "update") == 0) {
		// UpdateOTDR("update.txt", "", 12345);
	}
	else if(argc == 2 && (
		strcmp(argv[1], "getotdr") == 0 ||
		strcmp(argv[1], "getotdrc") == 0)  ){

		struct tms_getotdr_test_param test_param;
		test_param.rang =  30000;
		test_param.wl	= 1550;
		test_param.pw   = 640;
		test_param.time = 15;
		test_param.gi   = 1.4685;
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 2.0;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;
		test_param.reserve2 = 0;
		
		if (strcmp(argv[1], "getotdr") == 0) {
			tms_GetOTDRTest(fd, NULL, 0, 5,  DEV_OTDR,0,0, &test_param);
		}
		else if(strcmp(argv[1], "getotdrc") == 0) {
			tms_GetOTDRTestCycle(fd, NULL, 0, 5,  DEV_OTDR,0, 0,&test_param);	
		}
	}
	else if( argc == 2 && (  
			strcmp(argv[1], "retotdr") == 0 || 
			strcmp(argv[1], "refotdr") == 0 || 
			strcmp(argv[1], "retotdrc") == 0 )	) {

		struct tms_otdr_ref_hdr     ref_hdr;
		struct tms_retotdr_test_hdr test_hdr;
		struct tms_retotdr_test_param test_param;
		struct tms_retotdr_data_hdr data_hdr = {{0}}; 
		struct tms_retotdr_data_val data_val[30000];//= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

		struct tms_retotdr_event_hdr event_hdr = {{0}};
		struct tms_retotdr_event_val event_val[4]= {
			{0, 0, 3.1, 3.0, 3.9, 0.0}, 
			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
			{3, 3, 3.0, 3.2, 3.3, 3.4}};
		struct tms_retotdr_chain chain;
		struct tms_cfg_otdr_ref_val ref_data;

        ref_hdr.osw_frame = 0;
        ref_hdr.osw_slot  = 1;
		ref_hdr.osw_type = DEV_OSW;		
		ref_hdr.osw_port = 0;



        test_hdr.osw_frame = 0;
        test_hdr.osw_slot  = 1;
		test_hdr.osw_type = DEV_OSW;
        test_hdr.osw_port = 0;
		memcpy(&test_hdr.time, "[TIME]", 7);
        test_hdr.otdr_frame = 0;
        test_hdr.otdr_slot = 8;
		test_hdr.otdr_type = DEV_OTDR;
        test_hdr.otdr_port = 0;




		test_param.rang = 31111;
		test_param.wl = 1550;			
		test_param.pw =1280		;		
		test_param.time = 15;				
		test_param.gi = 1.2345;				
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 8.8;
		test_param.sample = 20000000;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;	

		strcpy((char*)data_hdr.dpid , "OTDRData");
		// printf("data.dpid %s\n", data_hdr.dpid);
		data_hdr.count = 30000;
		strcpy((char*)event_hdr.eventid, "KeyEvents");
		event_hdr.count = 4;
		strcpy((char*)chain.inf, "OTDRTestResultInfo");
		chain.range = 1200.123;
		chain.loss = 10.123;
		chain.att = 1.23;


        ref_data.leve0 = 7;
        ref_data.leve1 = 6;
        ref_data.leve2 = 5;


		if (strcmp(argv[1], "retotdr") == 0) {
			struct tms_alarm_line_hdr     alarm;
			bzero(&alarm, sizeof(struct tms_alarm_line_hdr));
			printf("retotdr\n");
			// tms_RetOTDRTest(fd, NULL, 
			// 	&test_hdr, &test_param, 
			// 	&data_hdr, data_val, 
			// 	&event_hdr, event_val, 
			// 	&chain);
			tms_RetOTDRCycle_V2(fd, NULL, 
				&alarm,
				&test_hdr, &test_param, 
				&data_hdr, data_val, 
				&event_hdr, event_val, 
				&chain);
		}
		else if (strcmp(argv[1], "retotdrc") == 0) {
			tms_RetOTDRTestCycle(fd, NULL, 
				&test_hdr, &test_param, 
				&data_hdr, data_val, 
				&event_hdr, event_val, 
				&chain);
		}
		else {
			tms_CfgOTDRRef(fd, NULL, 
				&ref_hdr, &test_param, 
				&data_hdr, data_val, 
				&event_hdr, event_val, 
				&chain, 
				&ref_data);	
		}
		
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 2 && strcmp(argv[1], "tick") == 0) {
		tms_Tick(fd, NULL);
	}
	else if(allcmd && strcmp(argv[1], "rdev") == 0) {
		tms_MCU_RetDeviceType(fd, NULL, sg_frameid, sg_slotid, 0xc3, 0xd4);
	}
	else if(allcmd && strcmp(argv[1], "opma") == 0) {
		tms_MCU_GetOLPAlarm(fd, NULL, sg_frameid, sg_slotid);
	}


	else if(argc == 2 && strcmp(argv[1], "olpa") == 0) {
		tms_MCU_GetOLPAlarm(fd, NULL, sg_frameid, sg_slotid);
	}
	else if(argc == 3 && strcmp(argv[1], "olpstart") == 0) {
		if ('m' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPStartOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_A);
		}
		else if ('s' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPStartOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_B);
		}
		else {
			printf("Error over range\n");
			printf("\tM\tmaster line start\n");
			printf("\tS\tslave line start\n");
		}
	}
	else if(argc == 3 && strcmp(argv[1], "olpfinish") == 0) {
		if ('m' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPFinishOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_A);
		}
		else if ('s' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPFinishOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_B);
		}
		else {
			printf("Error over range\n");
			printf("\tM\tmaster line finish\n");
			printf("\tS\tslave line finish\n");
		}
	}
	else if (argc == 3 && strcmp(argv[1], "cfgolp") == 0){
		int32_t port;
		uint8_t dev_name[64] =  "dev    name";
		uint8_t cable_name[64]= "cable  name";
		uint8_t start_name[64]= "start  name";
		uint8_t end_name[64]=   "end    name" ;

		port = atoi(argv[2]);
		strcat((char*)dev_name, argv[2]);
		strcat((char*)cable_name, argv[2]);
		strcat((char*)start_name, argv[2]);
		strcat((char*)end_name, argv[2]);
        sg_frameid = 0;
        sg_slotid = 5;
        port = 0;
		
		tms_CfgMCUOLPPort(fd, NULL, sg_frameid, sg_slotid, port , &dev_name, &cable_name, &start_name, &end_name);
	}
	else if (argc == 3 && strcmp(argv[1], "clolp") == 0){
		struct tms_cfg_mcu_any_port_clear_val list[2];
		list[0].any_port = 0;
		list[1].any_port = atoi(argv[2]);
		printf("%d %d\n", list[0].any_port, list[1].any_port	);

		tms_CfgMCUOLPPortClear(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "cfgolpu") == 0){
		struct tms_cfg_mcu_u_any_osw_val list[2];
        list[0].any_port  = 1;
        list[0].osw_frame = 0;
        list[0].osw_slot  = 1;
		// list[0].osw_type = 0;
        list[0].osw_port  = 0;

		list[1].any_port  = atoi(argv[2]);
        list[1].osw_frame = 0;
        list[1].osw_slot  = 1;
		// list[1].osw_type = 0;
        list[1].osw_port  = 1;
        sg_frameid = 0;
        sg_slotid = 5;

		tms_CfgMCUUniteOLPOSW(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "clolpu") == 0){
		struct tms_cfg_mcu_any_port_clear_val list[2];
		list[0].any_port = 3;
		list[1].any_port = atoi(argv[2]);
		printf("%d %d\n", list[0].any_port, list[1].any_port	);

		tms_CfgMCUUniteOLPOSWClear(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "cfgopmu") == 0){
		struct tms_cfg_mcu_u_any_osw_val list[2];
        list[0].any_port  = 0;
        list[0].osw_frame = 0;
        list[0].osw_slot  = 1;
		// list[0].osw_type = 0;
        list[0].osw_port  = 0;
        sg_frameid = 0;
        sg_slotid = 6;

	//		list[1].any_port  = atoi(argv[2]);
	//        list[1].osw_frame = 0;
	//		list[1].osw_slot  = 3;
	//		// list[1].osw_type = 0;
	//		list[1].osw_port  = 4;

        tms_CfgMCUUniteOPMOSW(fd, NULL, sg_frameid, sg_slotid, 1, list);
	}
	else if (argc == 3 && strcmp(argv[1], "clopmu") == 0){
		struct tms_cfg_mcu_any_port_clear_val list[2];
		list[0].any_port = 0;
		list[1].any_port = atoi(argv[2]);
		printf("%d %d\n", list[0].any_port, list[1].any_port	);
        sg_frameid = 0;
        sg_slotid = 6;
		tms_CfgMCUUniteOPMOSWClear(fd, NULL, sg_frameid, sg_slotid, 1, list);
	}
	else if(argc == 3 && strcmp(argv[1], "olpref") == 0 &&
		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

		tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 3, sg_olpref);
	}
	else if(argc == 3 && strcmp(argv[1], "olpref") == 0 && 
		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		
		tms_Print_tms_cfg_olp_ref_val(sg_olpref, 3);
	}
	else if(argc >= 3 && strcmp(argv[1], "olpref") == 0) {
		
		int index;
		index = atoi(argv[2]);
		if (index >= 3) {
			printf("out of rang <0~2>\n");
			return -1;
		}
		sg_olpref[index].port = index;
		if (sg_olpref[index].port > 2) {
			printf("port <0~2>\n");
			return -1;
		}
		// 功率设置/缺省值
		if (argc >= 4) {
			sg_olpref[index].ref_power = atoi(argv[3]);
		}
		else {
			sg_olpref[index].ref_power = sg_olpref_def.ref_power;
		}

		// 告警门限设置/缺省值
		if (argc == 7) {
			sg_olpref[index].leve0 = atoi(argv[4]);
			sg_olpref[index].leve1 = atoi(argv[5]);
			sg_olpref[index].leve2 = atoi(argv[6]);	
		}
		else {
			sg_olpref[index].leve0 = sg_olpref_def.leve0;
			sg_olpref[index].leve1 = sg_olpref_def.leve1;
			sg_olpref[index].leve2 = sg_olpref_def.leve2;
		}
		sg_olpref[index].isminitor = 1;
		sg_olpref[index].wave      = 1550;

		sg_olpref_def = sg_olpref[index];
		
		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
	}
	else if(argc == 2 && strcmp(argv[1], "olpop") == 0) {
		tms_GetOLPOP(fd, NULL, sg_frameid, sg_slotid);
	}

	else if(argc == 2 && strcmp(argv[1], "opmop") == 0) {
		tms_GetOPMOP(fd, NULL, sg_frameid, sg_slotid);
	}
	
	else if(argc == 2 && strcmp(argv[1], "cfgolpm") == 0) {
		tms_CfgOLPMode(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_MODE_UNBACK, 1, 0, -300);
	}

	else if(argc == 3 && strcmp(argv[1], "olpsw") == 0) {
		port = atoi(argv[2]);
		if ( (unsigned int)port < 7) {
			tms_MCU_OLPSwitch(fd, NULL, sg_frameid, sg_slotid, port);	
		}
		else {
			printf("Error over range\n");
		}
	}
	else if(argc == 2 && strcmp(argv[1], "olprep") == 0) {
		tms_ReportOLPAction(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_PERSION, OLP_SWITCH_A);
	}
	




	else if(argc == 2 && strcmp(argv[1], "gver") == 0) {
		struct tms_devbase devbase;
		if (tms_GetDevBaseByLocation(sg_frameid, sg_slotid, &devbase) == 0) {
			printf("f%d/s%d unexit\n", sg_frameid, sg_slotid);
		}
		tms_GetVersion(fd, NULL, sg_frameid, sg_slotid, devbase.type);
	}
	else if(argc == 2 && strcmp(argv[1], "rver") == 0) {
		tms_RetVersion(fd, NULL, sg_frameid, sg_slotid, DEV_OPM, (uint8_t*)("1.1.1.1.1"));
	}
	else if(0 && (allcmd && strcmp(argv[1], "update") == 0)) {
		char data[40] = {0};
		char fname[16] = "abcdefg";
		tms_Update(fd, NULL, 0x01, 0x02, DEV_OPM, (uint8_t(*)[16])&fname, 40, (uint8_t*)data);
	}
	else if(argc == 2 && strcmp(argv[1], "ack") == 0) {

		// tms_Ack(fd, NULL, RET_SUCCESS, 0x1);

		struct tms_ack pack;
		struct glink_addr gl;
		pack.errcode = 0;
		pack.cmdid = 0x10000001;
		pack.reserve1 = (12 << 16) | (4 << 0);
		pack.reserve2 = (2 << 16)  | (1 << 0);
		printf("%x %x ",pack.reserve1, pack.reserve2);

		gl.dst = 10;
		tms_AckEx(sg_sockfdid, &gl, &pack);
	}
	// else if(argc == 2 && strcmp(argv[1], "opmref") == 0) {
	// 	struct tms_cfg_opm_ref_val opmlist[8];
	// 	for (int i = 0; i < 8; i++) {
	// 		opmlist[i].port = i;
	// 		opmlist[i].isminitor = 0x01;
	// 		opmlist[i].wave = 1550;
	// 		opmlist[i].ref_power = 400;
	// 		opmlist[i].leve0 = 0xa1;
	// 		opmlist[i].leve1 = 0xb2;
	// 		opmlist[i].leve2 = 0xc3;
	// 	}
	// 	tms_CfgOPMRefLevel(fd, NULL, sg_frameid, sg_slotid, 8, opmlist);
	// }
	else if(argc == 3 && strcmp(argv[1], "opmref") == 0 &&
		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

		tms_CfgOPMRefLevel(fd, NULL, sg_frameid, sg_slotid, 8, sg_opmref);
	}
	else if(argc == 3 && strcmp(argv[1], "opmref") == 0 && 
		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		tms_Print_tms_cfg_opm_ref_val(&sg_opmref[0], 8);
		// printf("    port  ism  wave  r_power   lv0    lv1  lv2\n");
		// for (int i = 0; i < 8; i++) {
		// 	printf("%6.1d%5d  %6d%8d %6d%6d%6d\n",
		// 		sg_opmref[i].port,
		// 		sg_opmref[i].isminitor,
		// 		sg_opmref[i].wave,
		// 		sg_opmref[i].ref_power,
		// 		sg_opmref[i].leve0,
		// 		sg_opmref[i].leve1,
		// 		sg_opmref[i].leve2);
		// }
	}
	else if(argc >= 3 && strcmp(argv[1], "opmref") == 0) {
		
		int index;
		index = atoi(argv[2]);
		if (index >= 8) {
			printf("out of rang <0~7>\n");
			return -1;
		}
		sg_opmref[index].port = index;
		
		// 功率设置/缺省值
		if (argc >= 4) {
			sg_opmref[index].ref_power = atoi(argv[3]);
		}
		else {
			sg_opmref[index].ref_power = sg_opmref_def.ref_power;
		}

		// 告警门限设置/缺省值
		if (argc == 7) {
			sg_opmref[index].leve0 = atoi(argv[4]);
			sg_opmref[index].leve1 = atoi(argv[5]);
			sg_opmref[index].leve2 = atoi(argv[6]);	
		}
		else {
			sg_opmref[index].leve0 = sg_opmref_def.leve0;
			sg_opmref[index].leve1 = sg_opmref_def.leve1;
			sg_opmref[index].leve2 = sg_opmref_def.leve2;
		}
		sg_opmref[index].isminitor = 1;
		sg_opmref[index].wave      = 1550;

		sg_opmref_def = sg_opmref[index];
		
		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
	}



	

	
	else if(allcmd && strcmp(argv[1], "aopm") == 0) {
		struct tms_alarm_opm_val alarmopmlist[8];

		for (int i = 0; i < 8; i++) {
			alarmopmlist[i].port = i;
			alarmopmlist[i].levelx = 1;
			alarmopmlist[i].power = 200;
			memcpy(alarmopmlist[i].time, (uint8_t*)"12345", sizeof("12345"));
		}
		// tms_AlarmOPM(fd, sg_frameid, sg_slotid, 1, 8, alarmopmlist);
		tms_AlarmOPM(fd, NULL, sg_frameid, sg_slotid, 8, alarmopmlist);
	}
	else if(argc == 2 && strcmp(argv[1], "aopmc") == 0) {
		struct tms_alarm_opm_val alarmopmlist[8];
		for (int i = 0; i < 8; i++) {
			alarmopmlist[i].port = i;
			alarmopmlist[i].levelx = 1;
			alarmopmlist[i].power = 200;
			memcpy(alarmopmlist[i].time, (uint8_t*)"bbbb", sizeof("bbbb"));
		}
		// tms_AlarmOPMChange(fd, sg_frameid, sg_slotid, 1, 6, 8, alarmopmlist);
		tms_AlarmOPMChange(fd, NULL, sg_frameid, sg_slotid, 6, 8, alarmopmlist);
	}
	else if(allcmd && strcmp(argv[1], "gopm") == 0) {
		tms_GetOPMOP(fd, NULL, sg_frameid, sg_slotid);
	}
	else if(argc == 2 && strcmp(argv[1], "ropm") == 0) {
		struct glink_addr gl;
		struct tms_any_op_val opmvallist[2];
		opmvallist[0].port = 1;
		opmvallist[0].power = 200;
		opmvallist[1].port = 2;
		opmvallist[1].power = 400;
		printf("ssssss\n");
		gl.src = GLINK_4412_ADDR;
		gl.dst = GLINK_MANAGE_ADDR;
		
		gl.pkid = 1;
		tms_RetOPMOP(fd, &gl, sg_frameid, sg_slotid, 2, opmvallist);
	}
	else if(argc == 2 && strcmp(argv[1], "rolp") == 0) {
		struct tms_any_op_val opmvallist[2];
		opmvallist[0].port = 1;
		opmvallist[0].power = 200;
		opmvallist[1].port = 2;
		opmvallist[1].power = 400;
		tms_RetOLPOP(fd, NULL, sg_frameid, sg_slotid, 2, opmvallist);
	}
	else {
		printf("Usage:\n");
		printf("\tcmd\n");
		printf("\t\toswsw <port>\n");
		printf("\t\tgdev\n");
		printf("\t\trdev\n");
		printf("\t\topmp\n");
		printf("\t\tolpp\n");
		printf("\t\tgver\n");
		printf("\t\trver\n");
		printf("\t\tack\n");
		printf("\t\topmref\n");
		printf("\t\taopm\n");
		printf("\t\tgopm\n");
		printf("\t\tropm\n");
	}
	return 0;
	

	// tms_MCU_GetDeviceType(fd, 0xa1, 0xb2);
	// tms_MCU_RetDeviceType(fd, 0xa1, 0xb2, 0xc3, 0xd4);

	// tms_MCU_GetOPMRayPower(fd, 0xa1, 0xb2);
	// tms_MCU_GetOLPRayPower(fd, 0xa1, 0xb2);


	
	return 0;
}
W_BOOT_CMD(tms, cmd_tmsall, "cmd epoll server send");



int cmd_sip(int argc, char **argv)
{
	if (argc == 4 ) {
		uint8_t ip[16] = "192.168.2.2";
		uint8_t mask[16] = "255.255..0.0";
		uint8_t gw[16] = "192.168.2.1";

		snprintf((char*)ip, 16, "%s", argv[1]);
		snprintf((char*)mask, 16, "%s", argv[2]);
		snprintf((char*)gw, 16, "%s", argv[3]);
		tms_SetIPAddress(sg_sockfdid, NULL, ip, mask, gw);
	}
	return 0;
}
int cmd_gdev(int argc, char **argv)
{

	return 0;
}
int cmd_gver(int argc, char **argv)
{
	int frame, slot, type;
	char strout[64];
	int ret;

	// printf("asdfsadf");
	// gver this
	if (argc == 2 && memcmp(argv[1], "this", strlen(argv[1])) == 0) {
		printf("\t%s %s\n\t%s\n\tBuild Date	%s\n",TARGET_NAME,THIS_VERSION,RELEASE_DATE, BUILD_DATE);
	}
	// gver cu
	else if (argc == 2 && memcmp(argv[1], "cu", strlen(argv[1])) == 0) {
		// tms_GetVersion(sg_, struct glink_addr *paddr, int32_t frame, int32_t slot, int32_t type)
	}




	// 网管请求 mcu 的软件版本
	// gver mcu
	else if (argc == 2 && memcmp(argv[1], "mcu", strlen(argv[1])) == 0) {
		printf("[mcu]\n");
		ret = snprintf(strout, 64,"gver cmd this");
		// tms_Command(NULL, strout, ret + 1, LEVEL_TC );
		tms_Command(sg_sockfdid, NULL, strout, ret + 1);
	}
	// mcu 返回自己的软件版本到网管
	// gver cmd this
	else if (argc == 3 && memcmp(argv[1], "cmd", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "this", strlen(argv[2])) == 0) {
		printf("cmd mcu]\n");
		ret = snprintf(strout, 64,"\t%s %s\n\t%s\n",TARGET_NAME,THIS_VERSION,RELEASE_DATE);
		tms_Trace(NULL, strout, ret + 1, LEVEL_TC );

	}

	// gver dev <frame/slot>
	else if (argc == 5 && memcmp(argv[1], "dev", strlen(argv[1])) == 0) {
		struct tms_devbase devbase;
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);

		ret = snprintf(strout, 64,"gver cmd dev %d %d", frame,slot);
		// tms_Command(sg_sockfdid, NULL, strout, ret + 1);

		// type  = atoi(argv[4]);
		type = DevStr2Int(argv[4]);
		// if (tms_GetDevBaseByLocation(frame, slot, &devbase) == 0) {
		// 	printf("f%d/s%d unexit\n", frame, slot);
		// }
		tms_GetVersion(sg_sockfdid, NULL, frame, slot, type);
	}
	// gver cmd dev <frame/slot>
	else if (argc == 5 && memcmp(argv[1], "cmd", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "dev", strlen(argv[2])) == 0) {
		struct tms_devbase devbase;

		frame = atoi(argv[3]);
		slot  = atoi(argv[4]);
		if (tms_GetDevBaseByLocation(frame, slot, &devbase) == 0) {
			printf("f%d/s%d unexit\n", frame, slot);
		}
		tms_GetVersion(devbase.fd, NULL, devbase.frame, devbase.slot, devbase.type);
	}





	else {
		printf("input error\n");
	}
	return 0;
}

int cmd_opm(int argc, char **argv)
{
	int frame,slot;
	int opm_frame,opm_slot, opm_port;
	int osw_frame,osw_slot, osw_port;
	unsigned int index;
	// opm 与 osw 关联关系配置
	// opm unit <olp frame> <olp slot> <olp port>  <osw frame> <osw slot> <osw port>
	if(argc == 8 && memcmp(argv[1], "unit", strlen(argv[1])) == 0 ) {
		struct tms_cfg_mcu_u_any_osw_val list[1];

		opm_frame = atoi(argv[2]);
		opm_slot = atoi(argv[3]);
		opm_port = atoi(argv[4]);


		osw_frame = atoi(argv[5]);
		osw_slot = atoi(argv[6]);
		osw_port = atoi(argv[7]);


		if ((uint32_t)opm_frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)opm_slot >= 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)opm_port >= 128) {
			printf("port out of rang (0~128)\n");
			return -1;
		}

		if ((uint32_t)osw_frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)osw_slot >= 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)osw_port >= 128) {
			printf("port out of rang (0~128)\n");
			return -1;
		}

		list[0].any_port = opm_port;
		list[0].osw_frame = osw_frame;
		list[0].osw_slot = osw_slot;
		list[0].osw_port = osw_port;

		tms_CfgMCUUniteOPMOSW(sg_sockfdid, NULL, opm_frame, opm_slot, 1, list);
	}
	// opm unit clear <opm frame/slot/port>
	else if(argc == 6 && memcmp(argv[1], "unit", strlen(argv[1])) == 0  && 
			memcmp(argv[2], "clear", strlen(argv[2])) == 0) {
		struct tms_cfg_mcu_any_port_clear_val list[2];

		opm_frame = atoi(argv[3]);
		opm_slot  = atoi(argv[4]);
		opm_port  = atoi(argv[5]);
		list[0].any_port = opm_port;
		tms_CfgMCUUniteOPMOSWClear(sg_sockfdid, NULL, opm_frame, opm_slot, 1, list);

	}





	// OPM 光功率查询
	// opm op <frame/slot>
	else if(argc == 4 && strcmp(argv[1], "op") == 0) {
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		tms_GetOPMOP(sg_sockfdid, NULL, frame, slot);
	}
	// opm alarm <frame/slot>
	else if(argc == 4 && memcmp(argv[1], "alarm", strlen(argv[1])) == 0) {
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		tms_MCU_GetOPMAlarm(sg_sockfdid, NULL, frame, slot);
	}

	// OPM 参考电压查询
	// 
	// opm ref go <opm frame/slot>
	else if(argc == 5  && strcmp(argv[1], "ref") == 0 &&
			memcmp(argv[2], "go", strlen(argv[2])) == 0) {
		frame = atoi(argv[3]);
		slot  = atoi(argv[4]);
		tms_CfgOPMRefLevel(sg_sockfdid, NULL, frame, slot, 8, sg_opmref);
	}

		// opm ref clear <olp frame/slot>
	else if(argc == 5 && strcmp(argv[1], "ref") == 0 &&
		memcmp(argv[2], "clear", strlen(argv[2])) == 0) {

		frame = atoi(argv[3]);
		slot = atoi(argv[4]);
		for (int i = 0; i < 8; i++) {
			sg_opmref[i].isminitor = 1;
			sg_opmref[i].wave = 1550;
			sg_opmref[i].port = i;
			sg_opmref[i].isminitor = 1;
			sg_opmref[i].wave = 1550;
			sg_opmref[i].ref_power = -600;
			sg_opmref[i].leve0 = 100;
			sg_opmref[i].leve1 = 80;
			sg_opmref[i].leve2 = 50;
		}
		tms_CfgOPMRefLevel(sg_sockfdid, NULL, frame, slot, 3, sg_opmref);
	}	
	// opm ref level <lv0> <lv1> <lv2>
	else if(argc == 6 && strcmp(argv[1], "ref") == 0 &&
			memcmp(argv[2], "level", strlen(argv[2])) == 0) {

		int level0, level1, level2;
		level0 = atoi(argv[3]);
		level1 = atoi(argv[4]);
		level2 = atoi(argv[5]);

		if (level0 > level1 && level1 > level2) {
			sg_opmref_def.leve0 = level0;
			sg_opmref_def.leve1 = level1;
			sg_opmref_def.leve2 = level2;
		}
		else {
			printf("input error: reqest lv0 > lv1 > lv2\n");
		}
	}
	// opm ref auto <opm frame/slot>
	else if(argc == 5 && strcmp(argv[1], "ref") == 0 && 
		memcmp(argv[2], "auto", strlen(argv[2])) == 0) {

		tms_ReadOPMAuto();
		frame = atoi(argv[3]);
		slot  = atoi(argv[4]);
		tms_GetOPMOP(sg_sockfdid, NULL, frame, slot);
	}

	// opm ref enable <index>
	else if(argc >= 4 && strcmp(argv[1], "ref") == 0 && 
			memcmp(argv[2], "enable", strlen(argv[2])) == 0) {
		
		for (int i = 3; i < argc; i++) {
			index = atoi(argv[i]);
			if (index > 7) {
				printf("out of range <0~7>\n");
				return -1;
			}
			sg_opmref[index].isminitor = 1;
		}
	}
	// opm ref disable <index>
	else if(argc >= 4 && strcmp(argv[1], "ref") == 0 && 
			memcmp(argv[2], "disable", strlen(argv[2])) == 0) {
		
		for (int i = 3; i < argc; i++) {
			index = atoi(argv[i]);
			if (index > 7) {
				printf("out of range <0~7>\n");
				return -1;
			}
			sg_opmref[index].isminitor = 0;
		}
		
	}
	// opm ref list
	else if(argc == 3 && strcmp(argv[1], "ref") == 0 && 
		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		tms_Print_tms_cfg_opm_ref_val(&sg_opmref[0], 8);
		PrintOPMDef(&sg_opmref_def);
	}

	// opm ref <port> <power> [lv0] [lv1] [lv2]
	else if(argc >= 4 && strcmp(argv[1], "ref") == 0) {
		
		int index;
		index = atoi(argv[2]);
		if (index >= 8) {
			printf("out of rang <0~7>\n");
			return -1;
		}
		sg_opmref[index].port = index;
		
		// 功率设置/缺省值
		if (argc >= 4) {
			double tmp = atof(argv[3]) * 10.0;
			sg_opmref[index].ref_power = (int)tmp;
		}
		else {
			sg_opmref[index].ref_power = sg_opmref_def.ref_power;
		}

		// 告警门限设置/缺省值
		if (argc == 7) {
			double tmp;
			tmp = atof(argv[4]) * 10;
			sg_opmref[index].leve0 = (int)tmp;
			tmp = atof(argv[5]) * 10;
			sg_opmref[index].leve1 = (int)tmp;
			tmp = atof(argv[6]) * 10;
			sg_opmref[index].leve2 = (int)tmp;
		}
		else {
			sg_opmref[index].leve0 = sg_opmref_def.leve0;
			sg_opmref[index].leve1 = sg_opmref_def.leve1;
			sg_opmref[index].leve2 = sg_opmref_def.leve2;
		}
		sg_opmref[index].isminitor = 1;
		sg_opmref[index].wave      = 1550;

		sg_opmref_def = sg_opmref[index];
		
		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
	}
	else if(argc == 2 && strcmp(argv[1], "alarm") == 0) {
		struct tms_alarm_opm_val alarmopmlist[8];
		for (int i = 0; i < 8; i++) {
			alarmopmlist[i].port = i;
			alarmopmlist[i].levelx = 1;
			alarmopmlist[i].power = 200;
			memcpy(alarmopmlist[i].time, (uint8_t*)"bbbb", sizeof("bbbb"));
		}
		// tms_AlarmOPMChange(fd, sg_frameid, sg_slotid, 1, 6, 8, alarmopmlist);
		tms_AlarmOPMChange(sg_sockfdid, NULL, sg_frameid, sg_slotid, 6, 8, alarmopmlist);
	}
	return 0;
}
W_BOOT_CMD(opm, cmd_opm, "cmd epoll server send");

int cmd_olp(int argc, char **argv)
{

	int frame,slot,port;
	int olp_frame,olp_slot,olp_port;
	int osw_frame,osw_slot,osw_port;
	unsigned int index;
	// olp pos <frame> <slot> <port>
	if (argc == 5 && memcmp(argv[1], "pos", strlen(argv[1])) == 0) {
		uint8_t dev_name[64];
		uint8_t cable_name[64];
		uint8_t start_name[64];
		uint8_t end_name[64];
		char *pname = NULL;

		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		port  = atoi(argv[4]);

		if ((uint32_t)frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)slot > 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)port > 128) {
			printf("port out of rang (0~128)\n");
			return -1;
		}

		if(argc == 6) {
			pname = argv[5];
		}
		snprintf((char*)dev_name, 64, "dev_name port %d %s", port,pname);
		snprintf((char*)cable_name, 64, "cable_name port %d %s", port,pname);
		snprintf((char*)start_name, 64, "start_name port %d %s", port,pname);
		snprintf((char*)end_name, 64, "end_name port %d %s", port,pname);

		tms_CfgMCUOLPPort(sg_sockfdid, NULL, frame, slot, port, &dev_name, &cable_name, &start_name, &end_name);
	}

	// olp pos clear <frame> <slot> <port>
	else if (argc == 6 && memcmp(argv[1], "pos", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "clear", strlen(argv[2])) == 0) {
		struct tms_cfg_mcu_any_port_clear_val val;

		frame  = atoi(argv[3]);
		slot   = atoi(argv[4]);
		port   = atoi(argv[5]);

		if ((uint32_t)frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)slot > 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)port > 3) {
			printf("port out of rang (0~3)\n");
			return -1;
		}
		val.any_port = port;
		tms_CfgMCUOLPPortClear(sg_sockfdid, NULL, frame, slot, 1, &val);
	}


	// olp unit <olp frame> <olp slot> <olp port>  <osw frame> <osw slot> <osw port>
	else if(argc == 8 && memcmp(argv[1], "unit", strlen(argv[1])) == 0 ) {
		struct tms_cfg_mcu_u_any_osw_val list[1];

		olp_frame = atoi(argv[2]);
		olp_slot = atoi(argv[3]);
		olp_port = atoi(argv[4]);


		osw_frame = atoi(argv[5]);
		osw_slot = atoi(argv[6]);
		osw_port = atoi(argv[7]);


		if ((uint32_t)olp_frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)olp_slot >= 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)olp_port >= 3) {
			printf("port out of rang (0~3)\n");
			return -1;
		}

		if ((uint32_t)osw_frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)osw_slot >= 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)osw_port >= 128) {
			printf("port out of rang (0~128)\n");
			return -1;
		}

		list[0].any_port = olp_port;
		list[0].osw_frame = osw_frame;
		list[0].osw_slot = osw_slot;
		list[0].osw_port = osw_port;

		tms_CfgMCUUniteOLPOSW(sg_sockfdid, NULL, olp_frame, olp_slot, 1, list);
	}
		// opm unit clear <opm frame/slot/port>
	else if(argc == 6 && memcmp(argv[1], "unit", strlen(argv[1])) == 0  && 
			memcmp(argv[2], "clear", strlen(argv[2])) == 0) {
		struct tms_cfg_mcu_any_port_clear_val list[2];

		olp_frame = atoi(argv[3]);
		olp_slot  = atoi(argv[4]);
		olp_port  = atoi(argv[5]);
		list[0].any_port = olp_port;
		tms_CfgMCUUniteOLPOSWClear(sg_sockfdid, NULL, olp_frame, olp_slot, 1, list);

	}
	// olp op <frame/slot>
	else if(argc == 4 && strcmp(argv[1], "op") == 0) {
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		tms_GetOLPOP(sg_sockfdid, NULL, frame, slot);
	}
	// olp alarm <frame/slot>
	else if(argc == 4 && memcmp(argv[1], "alarm", strlen(argv[1])) == 0) {
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		tms_MCU_GetOLPAlarm(sg_sockfdid, NULL, frame, slot);
	}
	// olp mode  <olp frame/slot>    a/b b/u 
	else if(argc == 6 && memcmp(argv[1], "mode", strlen(argv[1])) == 0) {
		int mode;
		int protect;
		
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);


		if ('a' == (argv[4][0] | 0x20)) {
			protect = OLP_SWITCH_A;
		}
		else  {
			protect = OLP_SWITCH_B;
		}

		if ('u' == (argv[5][0] | 0x20)) {
			mode = OLP_SWITCH_MODE_UNBACK;
		}
		else {
			mode = OLP_SWITCH_MODE_BACK;
		}
		tms_CfgOLPMode(sg_sockfdid, NULL, frame, slot, mode, 5, protect, -300);
	}

	// olp sw <olp frame/slot> M/S 
	else if(argc == 5 && strcmp(argv[1], "sw") == 0) {
		frame = atoi(argv[2]);
		slot = atoi(argv[3]);

		if ('m' == (argv[4][0] | 0x20)) {
			port = OLP_SWITCH_A;
		}
		else if  ('s' == (argv[4][0] | 0x20)) {
			port = OLP_SWITCH_B;
		}

		if ( (unsigned int)port < 7) {
			tms_MCU_OLPSwitch(sg_sockfdid, NULL, frame, slot, port);	
		}
		else {
			printf("Error over range\n");
		}
	}
	// else if(argc == 2 && strcmp(argv[1], "olprep") == 0) {
	// 	tms_ReportOLPAction(sg_sockfdid, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_PERSION, OLP_SWITCH_A);
	// }
	// olp ref go <olp frame/slot>
	else if(argc == 5 && strcmp(argv[1], "ref") == 0 &&
		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

		frame = atoi(argv[3]);
		slot = atoi(argv[4]);
		tms_CfgOLPRefLevel(sg_sockfdid, NULL, frame, slot, 3, sg_olpref);
	}

	// olp ref clear <olp frame/slot>
	else if(argc == 5 && strcmp(argv[1], "ref") == 0 &&
		memcmp(argv[2], "clear", strlen(argv[2])) == 0) {

		frame = atoi(argv[3]);
		slot = atoi(argv[4]);
		for (int i = 0; i < 6; i++) {
			sg_olpref[i].isminitor = 1;
			sg_olpref[i].wave = 1550;
			sg_olpref[i].port = i % 3;
			sg_olpref[i].isminitor = 1;
			sg_olpref[i].wave = 1550;
			sg_olpref[i].ref_power = -900;
			sg_olpref[i].leve0 = 100;
			sg_olpref[i].leve1 = 80;
			sg_olpref[i].leve2 = 50;
		}
		// todo 切换到主路，切换后稍稍延时

		tms_CfgOLPRefLevel(sg_sockfdid, NULL, frame, slot, 3, sg_olpref);
		// todo 切换到备路，切换后稍稍延时
		tms_CfgOLPRefLevel(sg_sockfdid, NULL, frame, slot, 3, sg_olpref + 3);
	}	


	// olp ref level <lv0> <lv1> <lv2>
	else if(argc == 6 && strcmp(argv[1], "ref") == 0 &&
		memcmp(argv[2], "level", strlen(argv[2])) == 0) {

		int level0, level1, level2;
		level0 = atoi(argv[3]);
		level1 = atoi(argv[4]);
		level2 = atoi(argv[5]);

		if (level0 > level1 && level1 > level2) {
			sg_olpref_def.leve0 = level0;
			sg_olpref_def.leve1 = level1;
			sg_olpref_def.leve2 = level2;
		}
		else {
			printf("input error: reqest lv0 > lv1 > lv2\n");
		}
	}
	// olp ref auto <olp frame/slot>
	else if(argc == 5 && strcmp(argv[1], "ref") == 0 && 
		memcmp(argv[2], "auto", strlen(argv[2])) == 0) {

		tms_ReadOLPAuto();
		frame = atoi(argv[3]);
		slot  = atoi(argv[4]);
		tms_GetOLPOP(sg_sockfdid, NULL, frame, slot);
	}

	// olp ref enable <index>
	else if(argc >= 4 && strcmp(argv[1], "ref") == 0 && 
			memcmp(argv[2], "enable", strlen(argv[2])) == 0) {
		
		for (int i = 3; i < argc; i++) {
			index = atoi(argv[i]);
			if (index > 2) {
				printf("out of range <0~2>\n");
				return -1;
			}
			sg_olpref[index].isminitor = 1;	
		}
	}
	// olp ref disable <index>
	else if(argc >= 4 && strcmp(argv[1], "ref") == 0 && 
			memcmp(argv[2], "disable", strlen(argv[2])) == 0) {
		
		for (int i = 3; i < argc; i++) {
			index = atoi(argv[i]);
			if (index > 2) {
				printf("out of range <0~2>\n");
				return -1;
			}
			sg_olpref[index].isminitor = 0;	
		}
		
	}

	// olp ref list
	else if(argc == 3 && strcmp(argv[1], "ref") == 0 && 
		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		
		printf("Master line\n");
		tms_Print_tms_cfg_olp_ref_val(sg_olpref, 3);
		printf("Slave line\n");
		tms_Print_tms_cfg_olp_ref_val(sg_olpref + 3, 3);
		PrintOLPDef(&sg_olpref_def);
	}
	else if(argc >= 4 && strcmp(argv[1], "ref") == 0) {
		
		int index;
		index = atoi(argv[2]);
		if (index >= 3) {
			printf("out of rang <0~2>\n");
			return -1;
		}
		sg_olpref[index].port = index;
		if (sg_olpref[index].port > 2) {
			printf("port <0~2>\n");
			return -1;
		}
		// 功率设置/缺省值
		if (argc >= 4) {
			sg_olpref[index].ref_power = atoi(argv[3]);
		}
		else {
			sg_olpref[index].ref_power = sg_olpref_def.ref_power;
		}

		// 告警门限设置/缺省值
		if (argc == 7) {
			sg_olpref[index].leve0 = atoi(argv[4]);
			sg_olpref[index].leve1 = atoi(argv[5]);
			sg_olpref[index].leve2 = atoi(argv[6]);	
		}
		else {
			sg_olpref[index].leve0 = sg_olpref_def.leve0;
			sg_olpref[index].leve1 = sg_olpref_def.leve1;
			sg_olpref[index].leve2 = sg_olpref_def.leve2;
		}
		sg_olpref[index].isminitor = 1;
		sg_olpref[index].wave      = 1550;

		sg_olpref_def = sg_olpref[index];
		
		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
	}
	else {
		printf("Usarg:\n");
		printf("\tin put help\n");
	}
	return 0;
}

W_BOOT_CMD(olp, cmd_olp, "cmd epoll server send");
#include <stdio.h>


int cmd_otdr(int argc, char **argv)
{
	int frame,slot,port;
	struct tms_getotdr_test_param test_param;
	test_param.rang =  30000;
	test_param.wl	= 1550;
	test_param.pw   = 640;
	test_param.time = 15;
	test_param.gi   = 1.4685;
	test_param.end_threshold = 9.0;
	test_param.none_reflect_threshold = 2.0;
	test_param.reserve0 = 0;
	test_param.reserve1 = 0;
	test_param.reserve2 = 0;

	char *pbuf;
	int ret;

	struct tms_retotdr_test_hdr   *ptest_hdr;
	struct tms_retotdr_test_param *ptest_param;
	struct tms_retotdr_data_hdr   *pdata_hdr;
	struct tms_retotdr_data_val   *pdata_val;
	struct tms_retotdr_event_hdr  *pevent_hdr;
	struct tms_retotdr_event_val  *pevent_val;
	struct tms_retotdr_chain      *pchain;

	// otdr test <osw frame/slot/port>
	if (argc == 5 && memcmp(argv[1], "test", strlen(argv[1])) == 0) {
		frame = atoi(argv[2]);
		slot = atoi(argv[3]);
		port = atoi(argv[4]);


		tms_GetOTDRTest(sg_sockfdid, NULL, frame, slot, DEV_OTDR,port, 0,&test_param);
		return 0;
	}

	// otdr cyc <osw frame/slot/port>
	else if (argc == 5 && memcmp(argv[1], "cyc", strlen(argv[1])) == 0) {
		frame = atoi(argv[2]);
		slot = atoi(argv[3]);
		port = atoi(argv[4]);

		tms_GetOTDRTestCycle(sg_sockfdid, NULL, frame, slot,  DEV_OTDR,port, 0,&test_param);
		return 0;
	}

	// otdr alarmtest <osw frame/slot/port>
	else if (argc == 5 && memcmp(argv[1], "alarmtest", strlen(argv[1])) == 0) {
		frame = atoi(argv[2]);
		slot = atoi(argv[3]);
		port = atoi(argv[4]);

		tms_AnyGetOTDRTest(sg_sockfdid, NULL, frame, slot, DEV_OTDR,port, 0,&test_param, ID_GET_ALARM_TEST);
		return 0;
	}




	// otdr ref test <filename>  <osw frame/slot/port>
	else if (argc == 7 && memcmp(argv[1], "ref", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "test", strlen(argv[2])) == 0) {
		frame = atoi(argv[4]);
		slot = atoi(argv[5]);
		port = atoi(argv[6]);

		tms_ReadySaveFile(argv[3]);
		tms_GetOTDRTest(sg_sockfdid, NULL, frame, slot,  DEV_OTDR,port, 0,&test_param);
	}
	// otdr ref go <filename> <osw frame/slot/port> <lv0/lv1/lv2>
	else if (argc >= 7 && memcmp(argv[1], "ref", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "go", strlen(argv[2])) == 0) {
		frame = atoi(argv[4]);
		slot = atoi(argv[5]);
		port = atoi(argv[6]);


		struct tms_otdr_ref_hdr ref_hdr;
		struct tms_cfg_otdr_ref_val ref_data;

		pbuf = NULL;
		ret = tms_OpenOTDRBin(&pbuf, argv[3]);
		if (-1 == ret) {
			printf("error\n");
		}
		ptest_hdr   = (struct tms_retotdr_test_hdr   *)(pbuf  ); 
		ptest_param = (struct tms_retotdr_test_param *)(((char*)ptest_hdr)   + sizeof(struct tms_retotdr_test_hdr)); 
		pdata_hdr   = (struct tms_retotdr_data_hdr   *)(((char*)ptest_param) + sizeof(struct tms_retotdr_test_param)); 
		pdata_val   = (struct tms_retotdr_data_val   *)(((char*)pdata_hdr) + sizeof(struct tms_retotdr_data_hdr)); 
		pevent_hdr  = (struct tms_retotdr_event_hdr  *)(((char*)pdata_val) + sizeof(struct tms_retotdr_data_val) * (pdata_hdr->count)); 
		pevent_val  = (struct tms_retotdr_event_val  *)(((char*)pevent_hdr) + sizeof(struct tms_retotdr_event_hdr)); 
		pchain      = (struct tms_retotdr_chain      *)(((char*)pevent_val) + sizeof(struct tms_retotdr_event_val) * (pevent_hdr->count));


		ref_hdr.osw_frame = frame;
		ref_hdr.osw_slot  = slot;
		ref_hdr.osw_port  = port;
		ref_hdr.otdr_port = 0;

		if (argc >= 10) {
			ref_data.leve0 = atoi(argv[7]);
			ref_data.leve1 = atoi(argv[8]);
			ref_data.leve2 = atoi(argv[9]);
		}
		else {
			ref_data.leve0 = 9;
			ref_data.leve1 = 4;
			ref_data.leve2 = 2;
		}

		tms_CfgOTDRRef(sg_sockfdid, NULL, 
				&ref_hdr, ptest_param, 
				pdata_hdr, pdata_val, 
				pevent_hdr, pevent_val, 
				pchain, 
				&ref_data);	
		free(pbuf);
	}
	// otdr ref list <filename>
	else if (argc == 4 && memcmp(argv[1], "ref", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		pbuf = NULL;
		ret = tms_OpenOTDRBin(&pbuf, argv[3]);
		if (-1 == ret) {
			printf("error\n");
		}
		ptest_hdr   = (struct tms_retotdr_test_hdr   *)(pbuf  ); 
		ptest_param = (struct tms_retotdr_test_param *)(((char*)ptest_hdr)   + sizeof(struct tms_retotdr_test_hdr)); 
		pdata_hdr   = (struct tms_retotdr_data_hdr   *)(((char*)ptest_param) + sizeof(struct tms_retotdr_test_param)); 
		pdata_val   = (struct tms_retotdr_data_val   *)(((char*)pdata_hdr) + sizeof(struct tms_retotdr_data_hdr)); 
		pevent_hdr  = (struct tms_retotdr_event_hdr  *)(((char*)pdata_val) + sizeof(struct tms_retotdr_data_val) * (pdata_hdr->count)); 
		pevent_val  = (struct tms_retotdr_event_val  *)(((char*)pevent_hdr) + sizeof(struct tms_retotdr_event_hdr)); 
		pchain      = (struct tms_retotdr_chain      *)(((char*)pevent_val) + sizeof(struct tms_retotdr_event_val) * (pevent_hdr->count));

		tms_Print_tms_retotdr_event(pevent_hdr, pevent_val);
		tms_Print_tms_retotdr_chain(pchain);
		tms_Print_tms_retotdr_test_hdr(ptest_hdr);
		tms_Print_tms_retotdr_test_param(ptest_param);

		free(pbuf);
	}
	// otdr produce <otdr frame/slot>
	else if (argc == 4 && memcmp(argv[1], "produce", strlen(argv[1])) == 0) {
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		tms_MCU_GetOTDRParam(sg_sockfdid, NULL, frame, slot);
	}
	else {
		printf("input error\n");
	}
	return 0;	
}

// 设备信息配置与获取
int cmd_produce(int argc, char **argv)
{
	int frame,slot, type;

	// prod <dev frame/slot/type>
	if (argc == 4) {
		frame = atoi(argv[1]);
		slot  = atoi(argv[2]);
		type = DevStr2Int(argv[3]);

		tms_GetDevProduce(sg_sockfdid, NULL, frame, slot, type);
	}
	else {
		printf("input error\n");
	}
	return 0;
}

int cmd_sms(int argc, char **argv)
{
	// wchar_t s[]=L"垃圾而垃圾而垃圾而";
	// char sf[] = "监理文件";
	// setlocale(LC_ALL,"chs") ;
	// wprintf(L"%s\n",s);
	// printf("%s\n",sf);
	uint32_t index;
	// int frame, slot;

	// sms msg <sms frame/slot>
	if (argc == 4 && memcmp(argv[1], "msg", strlen(argv[1])) == 0) {
		// frame = atoi(argv[2]);
		// slot  = atoi(argv[3]);



		char phone[16] = "18777399591";
		//QT里面这一行编译不过的
		// wchar_t wcstr[30] =  L"字符测试123abc";
		// wchar_t wcstra[30] = L"字符测试经理问阿哈里斯";
		char a[100]= {0};
		int len;
		len = inputsms(a,100);
		PrintfMemory((uint8_t*)a,30);
		// wprintf(L"out %ls\n",a);
		// uint16_t wcstr[20];// = L"字符测试123abc";
		// printf("---%d---\n",strlen((char*)wcstr));
		// printf("---%d---\n",strlen((char*)wcstra));
		// PrintfMemory((uint8_t*)wcstr,30);
		// PrintfMemory((uint8_t*)wcstra,30);

		tms_SendSMS(sg_sockfdid, NULL, &phone, len, (wchar_t*)a);
	}

	// sms alarm <row> <date> <phone> <type> <level>
	else if (argc == 7 && memcmp(argv[1], "alarm", strlen(argv[1])) == 0) {
		index = atoi(argv[2]);
		if (index > 6) {
			return -1;
		}


		if (memcmp(argv[3], "mon", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 1;
		}
		else if (memcmp(argv[3], "tues", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 2;
		}
		else if (memcmp(argv[3], "wed", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 3;	
		}
		else if (memcmp(argv[3], "thur", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 4;
		}
		else if (memcmp(argv[3], "fri", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 5;
		}
		else if (memcmp(argv[3], "sat", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 6;
		}
		else if (memcmp(argv[3], "sun", strlen(argv[3])) == 0) {
			sg_alarm_sms[index].time = 7;
		}
		else {
			printf("input error\n");
		}
		strncpy((char*)sg_alarm_sms[index].phone,argv[4],16);


		if (memcmp(argv[5], "line", strlen(argv[5])) == 0) {
			sg_alarm_sms[index].type = SMSAT_LINE;
		}
		else if (memcmp(argv[5], "power", strlen(argv[5])) == 0) {
			sg_alarm_sms[index].type = SMSAT_POWERDOWN;
		}
		else if (memcmp(argv[5], "hw", strlen(argv[5])) == 0) {
			sg_alarm_sms[index].type = SMSAT_HW;
		}
		else if (memcmp(argv[5], "com", strlen(argv[5])) == 0) {
			sg_alarm_sms[index].type = SMSAT_COMMM;
		}
		else if (memcmp(argv[5], "olp", strlen(argv[5])) == 0) {
			sg_alarm_sms[index].type = SMSAT_OLP;
		}
		else {
			printf("input error");
		}


		if (memcmp(argv[6], "lv0", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_LV0;
		}
		else if (memcmp(argv[6], "lv1", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_LV1;
		}
		else if (memcmp(argv[6], "lv2", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_LV2;
		}
		else if (memcmp(argv[6], "occur", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = 1;
		}


		else if (memcmp(argv[6], "switch", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_OLP_SWITCH;
		}
		else if (memcmp(argv[6], "manual", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_OLP_MANUAL;
		}
		else if (memcmp(argv[6], "auto", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_OLP_AUTO;
		}
		else if (memcmp(argv[6], "back", strlen(argv[6])) == 0) {
			sg_alarm_sms[index].level = SMSAL_OLP_BACK;
		}
		else {
			printf("input error");
		}

	}

	// sms alarm list
	else if (argc == 3 && memcmp(argv[1], "alarm", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		tms_Print_tms_cfg_sms_val(sg_alarm_sms,7);
	}

	// sms alarm go
	else if (argc == 3 && memcmp(argv[1], "alarm", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "go", strlen(argv[2])) == 0) {
		tms_CfgSMSAuthorization(sg_sockfdid, NULL, 7, sg_alarm_sms);
	}
	// sms alarm clear
	else if (argc == 3 && memcmp(argv[1], "alarm", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "clear", strlen(argv[2])) == 0) {
		tms_ClearSMSAuthorization(sg_sockfdid, NULL);
	}

	
	return 0;	
}
W_BOOT_CMD(sms, cmd_sms, "cmd epoll server send");
void AnalyseComposition(int argc, char **argv)
{
	// int frame,slot,type,port;
	struct tms_devbase dev;
	int index;
	// 内容必须是 n Row x 4 Col的数据
	// -----frame----slot----type----port----
	// 参数不满足 n Row x 4 Col
	if (argc & 0x03) {
		return ;
	}
	printf("!!! update composition !!!\n");
	index = 0;
	while(index < argc) {
		dev.fd    = -1;
		dev.frame = atoi(argv[index]); index++;
		dev.slot  = atoi(argv[index]); index++;
		dev.type  = atoi(argv[index]); index++;
		dev.port  = atoi(argv[index]); index++;

		tms_AddDev(dev.frame, dev.slot, &dev);
	}
}


// 
int cmd_sn(int argc, char **argv)
{
	char strout[64];
	int ret;

	printf("send sn\n");
	// sn 
	if (argc == 1) {
		ret = snprintf(strout, 64,"sn request");
		// tms_Command(sg_sockfdid, NULL, strout, ret + 1);
		tms_GetSerialNumber(sg_sockfdid, NULL);
	}
	// sn request
	else if(argc == 2 && memcmp(argv[1], "request", strlen(argv[1])) == 0) {
		// todo 程序数据库
		// ret = snprintf(strout, 64,"sn reply aaaajlkjiljaallakwejr");
		// tms_Trace(NULL, strout, ret + 1, LEVEL_R_CMD);
	}

	// sn reply 'asfasdfassdf'
	else if(argc == 3 && memcmp(argv[1], "reply", strlen(argv[1])) == 0) {
		printf("sn:%s\n", argv[2]);
	}
	return 0;
}
int cmd_setip(int argc, char **argv)
{
	// char ip[16mask[16],gw[16];
	int iip,imask,igw;
	char strout[64];

	if (argc == 4) {
		uint8_t ip[16] = "192.168.2.2";
		uint8_t mask[16] = "255.255..0.0";
		uint8_t gw[16] = "192.168.2.1";

		iip   = inet_addr(argv[1]);
		imask = inet_addr(argv[2]);
		igw   = inet_addr(argv[3]);
		if ((iip & imask) != (igw & imask)) {
			printf("invaild\n");
			return -1;
		}
		snprintf((char*)ip, 16, "%s", argv[1]);
		snprintf((char*)mask, 16, "%s", argv[2]);
		snprintf((char*)gw, 16, "%s", argv[3]);
		snprintf(strout, 64, "ping %s", argv[1]);


		// int ret = system(strout);
		// printf("ret = %d\n", ret);

		// tms_SetIPAddress(fd, NULL, ip, mask, gw);
	}
	return 0;
}
int cmd_update(int argc, char **argv)
{
	struct tms_devbase oneframe[12];
	int havedev;
	char strout[256];
	int ret;
	struct trace_cache tc;

	// update com request
	// 收到网管请求更新设备信息表
	if(argc == 3 && memcmp(argv[1], "com", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "request", strlen(argv[2])) == 0) {

		// 遍历16个机框各槽位
		for (int i = 0; i < 16; i++) {
			havedev = 0;
			tms_GetFrame(i, &oneframe);
			tc.strout = strout;
			tc.limit = 200;
			tc.empty = 256;
			tc.offset = 0;

			ret = snprintf(strout, 64, "update com refurbish");
			tc.offset += ret;
			
			for (int k = 0; k < 12; k++) {
				if (oneframe[k].fd != 0) {
					havedev = 1;
					ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, " %d %d %d %d ",
							oneframe[k].frame, 
							oneframe[k].slot, 
							oneframe[k].type, 
							oneframe[k].port);
					tc.offset += ret;
				}
			}
			if (havedev) {
				// tms_Command(fd, NULL, tc.strout, tc.offset + 1);
				printf("ret disp update\n");
				tms_Trace(NULL, tc.strout, tc.offset + 1, LEVEL_R_CMD);
			}
			
		}
	}
	// update com update
	// 设备返回设备信息表
	else if (argc >= 3 && memcmp(argv[1], "com", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "refurbish", strlen(argv[2])) == 0) {

		AnalyseComposition(argc - 3, argv + 3);
	}
	// update bin file <frame/slot/type>
	else if(argc >= 6 && memcmp(argv[1], "bin", strlen(argv[1])) == 0) {
		char *ppath = argv[2];		// 文件路径
		int frame, slot, type;
		char target[16];  			// 
		char *pbuf;					// 文件内容
		int flen;					// 文件长度
		FILE *fp;

		frame = atoi(argv[3]);
		slot  = atoi(argv[4]);
		if (strcmp(argv[5], "pwu") == 0) {
			type  = DEV_PWU;
			strcpy(target, TARGET_PWU);
		}
		else if (strcmp(argv[5], "mcu") == 0) {
			type  = DEV_MCU;
			strcpy(target, TARGET_MCU);
		}
		else if (strcmp(argv[5], "opm") == 0) {
			type  = DEV_OPM;
			strcpy(target, TARGET_OPM);
		}
		else if (strcmp(argv[5], "osw") == 0) {
			type  = DEV_OSW;
			strcpy(target, TARGET_OSW);
		}
		else if (strcmp(argv[5], "otdr") == 0) {
			type  = DEV_OTDR;
			strcpy(target, TARGET_OTDR);
			// todo 计算MD5
		}
		else if (strcmp(argv[5], "ols") == 0) {
			type  = DEV_OLS;
			strcpy(target, TARGET_OLS);
		}
		else if (strcmp(argv[5], "olp") == 0) {
			type  = DEV_OLP;
			strcpy(target, TARGET_OLP);
		}
		else if (strcmp(argv[5], "sms") == 0) {
			type  = DEV_SMS;
			strcpy(target, TARGET_SMS);
			// todo 计算MD5
		}

		// 读取bin文件
		printf("Open file [%s]. ", ppath);
		fp = fopen((char*)ppath, "rb");
		if (NULL == fp) {
			printf("open file %s fail\n",ppath);
			return -1;
		}
		// TODO upp头并偏移
		fseek(fp, 0, SEEK_END);
		flen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		pbuf = (char*)malloc(flen);
		if (NULL == pbuf) {
			return -1;
		}
		fread(pbuf, 1, flen, fp);
		fclose(fp);


		tms_Update(sg_sockfdid, 	NULL, 
				frame,  slot, type, 
				(uint8_t(*)[16])&target, flen, (uint8_t*)pbuf);

		free(pbuf);
	}
	return 0;
}
W_BOOT_CMD(update, cmd_update, "cmd epoll server send");

int cmd_enable(int argc, char **argv)
{
	int onoff;
	if(argc == 3 && strcmp(argv[1], "cu") == 0) {
		g_en_connect_cu = atoi(argv[2]);
	}
	if(argc == 3 && strcmp(argv[1], "sqlecho") == 0) {
		onoff = atoi(argv[2]);
		tmsdb_Echo(onoff);       	// 关闭数据库回显

	}
	return 0;
}
W_BOOT_CMD(enable, cmd_enable, "cmd epoll server send");
int cmd_remotecmd(int argc, char **argv)
{
	for (int i = 0;i < argc;i++) {
		printf("%s ",argv[i]);
	}
	if (argc >= 2) {//} && strcmp(argv[1], "") == 0) {
		char strout[256];
		strout[0] = '\0';

		for (int i = 1; i < argc; i++) {
			strcat(strout, argv[i]);
			strcat(strout, " ");
		}

		strout[strlen(strout)-1] = '\0';
		printf("len %d send cmd %s\n", strlen(strout), strout);
		
		tms_Command(sg_sockfdid, NULL, strout, strlen(strout)+1);
	}
	return 0;
}

W_BOOT_CMD(r, cmd_remotecmd, "cmd epoll server send");
int cmd_term_connect(int argc,char **argv)
{
	struct ep_con_t client;
	char *pstrAddr;
	unsigned short port;
	char strout[64];
	int ret;

	// goto _Next;
	if (argc < 3) {
		printf("Usage:\n");
		printf("\tconnect <IP> <port>\n");
		return 0;
	}

	pstrAddr = argv[1];
	port     = (unsigned short)atoi(argv[2]);

	printf("Request connect %s:%d\n",pstrAddr,port);
	if (0 == ep_Connect(&ep,&client, pstrAddr, port)) {
		ret = snprintf(strout, 64, "int s %d", client.sockfd);
		printf("ret = %d\n",ret);
		sh_analyse(strout, ret+1);

		// 发送 序列号请求 ID_GET_SN 必须在 Im Trace 之前，
		// 保证先将Trace网管的地址加入MCU的缓存
		ret = snprintf(strout, 64, "sn");
		// tms_Command(client.sockfd, NULL, strout, ret + 1);

		if (argc == 4) {
			struct glink_addr gl;
			gl.src = GLINK_MANAGE_ADDR + atoi(argv[3]);
			gl.dst = GLINK_CU_ADDR;
			gl.pkid = 1;
			printf("------------gl.src %x--------\n",gl.src);
			tms_GetSerialNumber(client.sockfd, &gl);
		}
		else {
			tms_GetSerialNumber(client.sockfd, NULL);
		}

		ret = snprintf(strout, 64, "im add trace");
		tms_Command(client.sockfd, NULL, strout, ret + 1);

		ret = snprintf(strout, 64, "update com request");
		tms_Command(client.sockfd, NULL, strout, ret + 1);
	}
	return 0;
}
W_BOOT_CMD(tconnect, cmd_term_connect, "terminal connect");
int cmd_im(int argc, char **argv)
{
	if (argc == 2 && memcmp(argv[1], "trace", strlen(argv[1])) == 0) {
		char strout[] = "im add trace";
		tms_Command(sg_sockfdid, NULL, strout, sizeof(strout));
	}
	else if (argc == 3 && memcmp(argv[1], "add", strlen(argv[1])) == 0) {	
		int fd = tms_GetTempFd();
		// tms_AddManage(0, fd, MT_TRACE);
	}


	else if (argc == 2 && memcmp(argv[1], "leave", strlen(argv[1])) == 0) {
		char strout[] = "im del trace";
		tms_Command(sg_sockfdid, NULL, strout, sizeof(strout));
	}
	else if (argc == 3 && memcmp(argv[1], "del", strlen(argv[1])) == 0) {	
		int fd = tms_GetTempFd();
		tms_DelManage(0, fd);
	}

	return 0;	
}
W_BOOT_CMD(im, cmd_insert, "which dev");

int list(struct ep_con_t *pconNode, void *ptr)
{
	int fd = pconNode->sockfd;
	if (fd != 4) {
		printf("scan fd %d \n", fd);
		tms_MCU_GetDeviceType(fd, NULL);//, 0, 6);
	}
	return 0;
}
int cmd_tmsscan(int argc, char **argv)
{
	int fd = 0;
	if (argc == 2) {
		if (memcmp(argv[1], "all", strlen(argv[1])) == 0) {
			printf("scan all\n");
			ep_Ergodic(&ep, list, NULL);
		}
		else if (fd = atoi(argv[1]), fd != 0 && fd != 4) {
			printf("scan fd :%d\n", fd);
			sg_sockfdid = fd;
			tms_MCU_GetDeviceType(fd, NULL);//, 0, 6);
		}
	} 
	else if(argc == 3) {
		if (memcmp(argv[1], "fd", strlen(argv[1])) == 0) {
			struct tms_devbase dev;

			fd = atoi(argv[2]);			
			if (tms_GetDevBaseByFd(fd, &dev) == 0) {
				printf("frame %2.2d slot %2.2d type %d port %d\n", dev.frame, dev.slot, dev.type, dev.port);
			}
			else {
				printf("un find\n");
			}

		}
	}
	else if(argc == 4) {
		if (memcmp(argv[1], "fs", strlen(argv[1])) == 0) {
			struct tms_devbase dev;
			int frame, slot;

			frame = atoi(argv[2]);
			slot  = atoi(argv[3]);
			tms_GetDevBaseByLocation(frame, slot, &dev);
			printf("frame %2.2d slot %2.2d type %d port %d\n", dev.frame, dev.slot, dev.type, dev.port);
			
		}
	}
	else {
		printf("Usage:\n");
		printf("  scan <fdid>\n");
		printf("  cmd\n");
		printf("  fd<1~N> \tSpecify scan socket fdid\n");
		printf("  all     \tSpecify scan all connected socket fdid\n");
	}
	return 0;
}
W_BOOT_CMD(scan, cmd_tmsscan, "cmd epoll server send");

void sh_analyse (char *fmt, long len);
int cmd_intface(int argc, char **argv)
{
	int frame = 0, slot = 0, sockfd = 0;
	struct tms_devbase oneframe[12];

	if (argc == 2) {
		char c = 0;
		char unuse = 0;
		char unuse1 = 0;// 必须初始化，某些编译器可能不对其复值，
						// "int 1/2"作为输入sscanf返回的unuse1是随机数
		

		sscanf(argv[1], "%d/%d%c", &frame, &slot, &unuse1);
		sscanf(argv[1], "%d%c%c", &frame, &c, &unuse);
		if (c == '/' && 
			(unuse >= '0' && unuse <= '9') &&
			unuse1 == 0) {

			if ((unsigned int)frame > 15 || (unsigned int)slot > 11) {
				printf("Error over range\n");
				return 0;
			}
			tms_GetFrame(frame, &oneframe);
			
			//todo frame slot合法性检测
			if (oneframe[slot].fd != 0) 
			{
				sg_sockfdid = oneframe[slot].fd;
				sg_frameid = frame;
				sg_slotid = slot;
				char path[36];
				struct sockaddr_in remoteAddr;
				socklen_t 		 len;
				len = sizeof(struct sockaddr_in);
				getpeername(sg_sockfdid, (struct sockaddr*)&remoteAddr , &len);

				// snprintf(path, 36, "f%d/s%d", sg_frameid, sg_slotid);
				snprintf(path, 36, "%s:f%d/s%d", inet_ntoa(remoteAddr.sin_addr), sg_frameid, sg_slotid);
				sh_editpath(path);
			}
			else {
				// sg_frameid = frame;
				// sg_slotid = slot;
				printf("Unknow interface %d/%d waiting\n", frame, slot);
			}
			
		}
	}
	else if (argc == 3 && memcmp(argv[1], "sockfd", strlen(argv[1])) == 0) {
			sockfd = atoi(argv[2]);
			sg_sockfdid = sockfd;
			printf("Change sockfd %d\n", sg_sockfdid);

			
			struct tms_devbase dev;
			char path[36];//192.168.100.xx:fxx/sxx>
			struct sockaddr_in remoteAddr;
			socklen_t 		 len;
			len = sizeof(struct sockaddr_in);
			getpeername(sockfd, (struct sockaddr*)&remoteAddr , &len);
			if (tms_GetDevBaseByFd(sg_sockfdid, &dev) == 0) {
				sg_frameid = dev.frame;
				sg_slotid  = dev.slot;
				snprintf(path, 36, "%s:f%d/s%d", inet_ntoa(remoteAddr.sin_addr), sg_frameid, sg_slotid);
				printf("path:%s\n", path);
			}
			else {
				snprintf(path, 36, "%s:unknow", inet_ntoa(remoteAddr.sin_addr));
			}
			
			sh_editpath(path);
		}
	else {
		printf("Usage:\n");
		printf("  interface frame <1~N> slot <1~N>\n");
	}
	return 0;
}
W_BOOT_CMD(interface, cmd_intface, "cmd epoll server send");


int cmd_device(int argc, char **argv)
{
	int frame = 0, slot = 0;
	struct tms_devbase oneframe[12];

	if (argc == 2 || argc == 3) {
		char c = 0;
		char unuse = 0;
		char unuse1 = 0;// 必须初始化，某些编译器可能不对其复值，
						// "int 1/2"作为输入sscanf返回的unuse1是随机数
		

		sscanf(argv[1], "%d/%d%c", &frame, &slot, &unuse1);
		sscanf(argv[1], "%d%c%c", &frame, &c, &unuse);
		if (c == '/' && 
			(unuse >= '0' && unuse <= '9') &&
			unuse1 == 0) {

			if ((unsigned int)frame > 15 || (unsigned int)slot > 11) {
				printf("Error over range\n");
				return 0;
			}
			tms_GetFrame(frame, &oneframe);
			
			//todo frame slot合法性检测
			if (oneframe[slot].fd != 0) 
			{
				// sg_sockfdid = oneframe[slot].fd;
				sg_frameid = frame;
				sg_slotid = slot;
				char path[36];
				struct sockaddr_in remoteAddr;
				socklen_t 		 len;
				len = sizeof(struct sockaddr_in);
				getpeername(sg_sockfdid, (struct sockaddr*)&remoteAddr , &len);

				// snprintf(path, 36, "f%d/s%d", sg_frameid, sg_slotid);
				snprintf(path, 36, "%s:f%d/s%d", inet_ntoa(remoteAddr.sin_addr), sg_frameid, sg_slotid);
				printf("path %s\n",path);
				sh_editpath(path);
			}
			else {
				// sg_frameid = frame;
				// sg_slotid = slot;
				printf("Unknow device %d/%d waiting\n", frame, slot);
			}


			if (argc == 3 && memcmp(argv[2], "do", strlen(argv[2])) == 0) {
				sg_frameid = frame;
				sg_slotid = slot;
				char path[36];
				struct sockaddr_in remoteAddr;
				socklen_t 		 len;
				len = sizeof(struct sockaddr_in);
				getpeername(sg_sockfdid, (struct sockaddr*)&remoteAddr , &len);

				// snprintf(path, 36, "f%d/s%d", sg_frameid, sg_slotid);
				snprintf(path, 36, "%s:f%d/s%d", inet_ntoa(remoteAddr.sin_addr), sg_frameid, sg_slotid);
				printf("path %s\n",path);
				sh_editpath(path);
			}
			
		}
		
	}
	return 0;
}
W_BOOT_CMD(dev, cmd_device, "sudo");








int cmd_osw(int argc, char **argv)
{
	unsigned int frame,slot,port;
	// osw pos <frame> <slot> <port>
	if (argc == 5 && memcmp(argv[1], "pos", strlen(argv[1])) == 0) {
		uint8_t dev_name[64];
		uint8_t cable_name[64];
		uint8_t start_name[64];
		uint8_t end_name[64];
		char *pname = NULL;

		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		port  = atoi(argv[4]);

		if ((uint32_t)frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)slot > 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)port > 128) {
			printf("port out of rang (0~128)\n");
			return -1;
		}

		if(argc == 6) {
			pname = argv[5];
		}
		snprintf((char*)dev_name, 64, "dev_name port %d %s", port,pname);
		snprintf((char*)cable_name, 64, "cable_name port %d %s", port,pname);
		snprintf((char*)start_name, 64, "start_name port %d %s", port,pname);
		snprintf((char*)end_name, 64, "end_name port %d %s", port,pname);

		tms_CfgMCUOSWPort(sg_sockfdid, NULL, frame, slot, port, &dev_name, &cable_name, &start_name, &end_name);
	}

	// osw pos clear <frame> <slot> <port>
	else if (argc == 6 && memcmp(argv[1], "pos", strlen(argv[1])) == 0 &&
			memcmp(argv[2], "clear", strlen(argv[2])) == 0) {
		struct tms_cfg_mcu_any_port_clear_val val;

		frame  = atoi(argv[3]);
		slot   = atoi(argv[4]);
		port   = atoi(argv[5]);

		if ((uint32_t)frame >= 16 ) {
			printf("frame out of rang (0~15)\n");
			return -1;
		}
		if ((uint32_t)slot > 12) {
			printf("slot out of rang (0~12)\n");
			return -1;
		}
		if ((uint32_t)port > 128) {
			printf("port out of rang (0~128)\n");
			return -1;
		}
		val.any_port = port;
		printf("clear\n");
		tms_CfgMCUOSWPortClear(sg_sockfdid, NULL, frame, slot, 1, &val);
	}

	// osw sw <frame> <slot> <port>
	else if(argc == 5 && strcmp(argv[1], "sw") == 0) {
		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		port  = atoi(argv[4]);

		if (frame > 16 && slot > 12) {
			printf("param error\n");
		}
		if ( (unsigned int)port <= 7) {
			tms_MCU_OSWSwitch(sg_sockfdid, NULL, frame, slot, port);	
		}
	}

	// osw cyc <frame slot port>  <enable/disable>  <interval>
	else if(argc == 7 && strcmp(argv[1], "cyc") == 0) {
		struct tms_cfg_mcu_osw_cycle_val val[3];
		int en;
		int interval;

		frame = atoi(argv[2]);
		slot  = atoi(argv[3]);
		port  = atoi(argv[4]);

		// enable
		if ('e' == (argv[5][0] | 0x20)) {
			en = 1;
		}
		else {
			en = 0;
		}
		interval  = atoi(argv[4]);

		val[0].port = port;
		val[0].iscyc    = en;
		val[0].interval = interval;
        sg_frameid = 0;
        sg_slotid = 1;
		tms_CfgMCUOSWCycle(sg_sockfdid, NULL, frame, slot, 1, val);
	}
	else {
		printf("Usarg:\n");
		printf("\tin put help\n");
	}
	return 0;
}


int cmd_sudo(int argc, char **argv)
{
	if(0 == strcmp(argv[0], "sudo") ) {
		sg_sudo = 1;
		// todo:命令嵌套
		sg_sudo = 0;
	}
	else if(0 == strcmp(argv[0], "sudo") ) {
		sg_sudo = 0;
		
	}
	else {
		printf("Usage:\n");
		printf("  sudo <cmd>   supper\n");	
		printf("  unsudo <cmd> unsupper\n");	
	}
	return 0;
}
W_BOOT_CMD(sudo, cmd_sudo, "sudo");
W_BOOT_CMD(unsudo, cmd_sudo, "unsudo");


// 用字符串打印路由信息


/*
<id> x.x.x.x --- x.x.x.x
OTDR(01/2/03)---OSW(01/2/03)---OSW(01/2/03)---OSW(01/2/03)---OSW(01/2/03)---
*/
int DispRoute(struct tdb_route_line *prl, struct trace_cache *ptc)
{
	static char devName[][8] = {
		{"NULL"}, 
		{"PWU"}, //1
		{"MCU"}, //2
		{"OPM"}, //3
		{"OSW"}, 
		{"OTDR"}, 
		{"OLS"}, 
		{"OLP"}, 
		{"SMS"}, 
	};
	struct tdb_route_node *pnode = prl->node;
	int ret;
	struct sockaddr_in addr;
	
	
	// <id> ip -- ip
	addr.sin_addr.s_addr = prl->ip_src;
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"\n<%d> %s -- ",
				pnode->id, inet_ntoa(addr.sin_addr));
	ptc->offset += ret;

	addr.sin_addr.s_addr = prl->ip_dst;
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"%s\n    ",
				inet_ntoa(addr.sin_addr));
	ptc->offset += ret;

	// dev(f/s/p)---
	for (int i = 0,k = 0;i < prl->count; i++) {
		if ((uint32_t)pnode->type < sizeof(devName) / sizeof(char[8])) {
			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"%s(%d/%d/%d)---",
				devName[pnode->type],
				pnode->frame, pnode->slot, pnode->port);
			ptc->offset += ret;
		}
		if (k++ >= 4) {
			k = 0;
			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"\n    ");
			ptc->offset += ret;
		}
		pnode++;
	}
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"\n");
	ptc->offset += ret;
	return 0;
}

/*
打印设备级联关系
<id> x.x.x.x --- x.x.x.x
OTDR(01/2/03)---OSW(01/2/03)---OSW(01/2/03)---OSW(01/2/03)---OSW(01/2/03)---
*/
int DispRoute_V2(struct tdb_route *prl, int count, struct trace_cache *ptc)
{
	static char devName[][8] = {
		{"Unknow"}, 
		{"OPW"}, //1
		{"MCU"}, //2
		{"OPM"}, //3
		{"OSW"}, 
		{"OTDR"}, 
		{"OLS"}, 
		{"OLP"}, 
		{"SMS"}, 
	};
	struct tdb_route *pnode = prl;
	int ret;
	struct sockaddr_in addr;
	
	
	// <id> ip -- ip
	addr.sin_addr.s_addr = prl->ip_src;
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"<%d> %s -- ",
				pnode->id, inet_ntoa(addr.sin_addr));
	ptc->offset += ret;

	addr.sin_addr.s_addr = prl->ip_dst;
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"%s\n    ",
				inet_ntoa(addr.sin_addr));
	ptc->offset += ret;

	// dev(f/s/p)---
	for (int i = 0,k = 0;i < count; i++) {
		if ((uint32_t)pnode->type_b < sizeof(devName) / sizeof(char[8])) {
			// only for debug
			// ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
			// 	"%s(%d/%d/%d)",
			// 	devName[pnode->type_a],
			// 	pnode->frame_a, pnode->slot_a, pnode->port_a);
			// ptc->offset += ret;

			if (pnode->port_b == (int32_t)0xefffffff) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
					"%s(%d/%d/%d/c)---",
					devName[pnode->type_b],
					pnode->frame_b, pnode->slot_b, pnode->type_b);
				ptc->offset += ret;
			}
			else {
				// ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				// 	"%s(%d/%d/%d/%d)",
				// 	devName[pnode->type_a],
				// 	pnode->frame_a, pnode->slot_a, pnode->type_a, pnode->port_a);
				// ptc->offset += ret;

				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
					"%s(%d/%d/%d/%d)---",
					devName[pnode->type_b],
					pnode->frame_b, pnode->slot_b, pnode->type_b, pnode->port_b);
				ptc->offset += ret;
			}
			
		}
		if (k++ >= 4) {
			k = 0;
			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"\n    ");
			ptc->offset += ret;
		}
		pnode++;
	}
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"\n");
	ptc->offset += ret;
	return 0;
}


// 用字符串打印机框信息
int DispFrame(struct tms_devbase *pframe, uint32_t flag, struct trace_cache *ptc)
{
	static char devName[][8] = {
		{"      "}, 
		{"OPW   "}, //1
		{"MCU   "}, //2
		{"OPM   "}, //3
		{"SW    "}, 
		{"OTDR  "}, 
		{"OLS   "}, 
		{"OLP   "}, 
		{"GSM   "}, 
	};
	int ret;

	if (ptc == NULL) {
		return -1;
	}
	printf("ptc = %x",(int)ptc);
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
			"-------------------------------------------------\n");
	ptc->offset += ret;

	for (int k = 0; k < 12; k++) {
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
				"|%2.2d ", k);
		ptc->offset += ret;		
	}

	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
			"|\n-------------------------------------------------\n");
	ptc->offset += ret;

	for (int i = 0; i < 5; i++) {
		for (int k = 0; k < 12; k++) {
			// 防止溢出
			if ((uint32_t)pframe[k].type < sizeof(devName) / sizeof(char[8])) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
						"|%2c ", (devName[pframe[k].type][i]));
				ptc->offset += ret;
			}
		}
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "|\n");
		ptc->offset += ret;
	}
	for (int k = 0; k < 12; k++) {
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"|%2.2d ", pframe[k].port);
		ptc->offset += ret;
	}

	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
		"|\n-------------------------------------------------\n");
	ptc->offset += ret;

	if (flag & 0x01) {
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "Socket Fd:\n");
		ptc->offset += ret;
		for (int i = 0; i < 12; i++) {
			if (pframe[i].fd == 0) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "%2.2d:no    ", i);
				ptc->offset += ret;
			}
			else {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "%2.2d:%-5.0d ", i, pframe[i].fd);
				ptc->offset += ret;
			}
			if (i == 5) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "\n");
				ptc->offset += ret;
			}
		}
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "\n");	
		ptc->offset += ret;
	}
	return 0;
}

int Dispinf(struct tms_devbase *pframe, uint32_t flag, struct trace_cache *ptc)
{
	// struct ep_con_t *pnode = pconNode;
	// struct _dispinf *p = (struct _dispinf*)ptr;
	
	// int ret;
	struct sockaddr_in locateAddr,remoteAddr;
	socklen_t 		 len;
	int ret;
	int fd;

	if (ptc->offset > ptc->limit) {
		printf("%s", ptc->strout);
		tms_Trace(NULL, ptc->strout, ptc->offset, LEVEL_TC);
		ptc->offset = 0;
	}
	for (int i = 0; i < 12; i++) {
		if (pframe[i].fd != 0) {
			fd = pframe[i].fd;

			len = sizeof(struct sockaddr_in);
			getsockname(fd, (struct sockaddr*)&locateAddr, &len);
			len = sizeof(struct sockaddr_in);
			getpeername(fd, (struct sockaddr*)&remoteAddr, &len);


			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"%-4d%8d%16s:%-8d",
				i,
				fd,
				inet_ntoa(locateAddr.sin_addr),
				htons(locateAddr.sin_port));
			ptc->offset += ret;

			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"%16s:%-8d\n",
					inet_ntoa(remoteAddr.sin_addr),
					htons(remoteAddr.sin_port));
			ptc->offset += ret;
		}
	}
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"\n");
	ptc->offset += ret;
	return 0;	
}


#define FLAG_DISP_FRAME 1
#define FLAG_DISP_CON 2
#define FLAG_DISP_ALL 0xffff

int cmd_Disp(int argc, char **argv)
{
	int havedev;
	int frametotal = 0, slottotal = 0;
	int slot = 0;
	char strout[1024];
	struct tms_devbase oneframe[12];
	// struct _dispinf cbval;
	struct trace_cache tc;
	int ret;

	int flag = 0;

	printf(" h             who      1211111111     \n");
	// 打印设备连接状态，有两种显示方式、frame、connect
	if (argc == 2) {
		if(memcmp(argv[1], "frame", strlen(argv[1])) == 0) {
			flag = FLAG_DISP_FRAME;
		}
		else if(memcmp(argv[1], "connect", strlen(argv[1])) == 0) {
			flag = FLAG_DISP_CON;
		}
		else if(memcmp(argv[1], "all", strlen(argv[1])) == 0) {
			flag = FLAG_DISP_ALL;
		}
	} 


	if (argc == 2 && (flag & (FLAG_DISP_FRAME + FLAG_DISP_CON) ) ) {

		// 遍历16个机框各槽位
		for (int i = 0; i < 16; i++) {
			havedev = 0;
			slot = 0;
			tms_GetFrame(i, &oneframe);
			for (int k = 0; k < 12; k++) {
				if (oneframe[k].fd != 0) {
					havedev = 1;
					slottotal++;
					slot++;
				}
			}

			// 只有该机框有设备才打印
			if (havedev == 1) {
				frametotal++;
				tc.strout = strout;
				tc.limit = 500;
				tc.empty = 1024;
				tc.offset = 0;
				ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, 
					"\nFrame:%2.2d Slot count:%2.2d\n", i, slot);
				tc.offset += ret;


				if (flag & FLAG_DISP_FRAME) {
					DispFrame(oneframe , -1, &tc);
					printf("%s",tc.strout);
					tms_Trace(NULL, tc.strout, tc.offset + 1, LEVEL_TC);	
				}
				
				if (flag & FLAG_DISP_CON) {
					tc.strout = strout;
					tc.limit = 800;
					tc.empty = 1024;
					tc.offset = 0;
					ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, "%s%-4s%8s%16s%24s\n",
						"Connect:\n",
						"Slot ","FD","locate","Remote");
					tc.offset += ret;

					Dispinf(oneframe , -1, &tc);
					printf("%s",tc.strout);
					tms_Trace(NULL, tc.strout, tc.offset + 1, LEVEL_TC);
				}
				
			}
		}

		// 打印总共有多少机框和槽位
		tc.strout = strout;
		tc.limit = 500;
		tc.empty = 1024;
		tc.offset = 0;
		ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, 
				"                    Total Frame:%2.2d Total Slot:%2.2d\n", frametotal, slottotal);
		tc.offset += ret;
		printf("%s", tc.strout);
		tms_Trace(NULL, tc.strout, tc.offset + 1, LEVEL_TC);
	}


	// 显示当前向那个设备执行shell结果，只有trace端才显示
	else if(argc == 2 && memcmp(argv[1], "cur", strlen(argv[1])) == 0) {
		tc.strout = strout;
		tc.limit = 500;
		tc.empty = 1024;
		tc.offset = 0;
		ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, 
				"fd %d frame %d slot %d\n", sg_sockfdid, sg_frameid, sg_slotid);
		tc.offset += ret;
		printf("%s", strout);
		tms_Trace(NULL, tc.strout, tc.offset + 1, LEVEL_TC);	
	}




	// disp update
	// 本地执行：请求板卡组成信息
	else if(argc == 2 && memcmp(argv[1], "update", strlen(argv[1])) == 0) {
		ret = snprintf(strout, 64, "update com request");
		tms_Command(sg_sockfdid, NULL, strout, ret + 1);

		
		goto _Clear;
	}
	// disp updatev2
	// 本地执行：请求板卡组成信息
	else if(argc == 2 && memcmp(argv[1], "updatev2", strlen(argv[1])) == 0) {
		// TMSxxV1.2新协议
		tms_GetDeviceCompositionRT(sg_sockfdid, NULL);

		goto _Clear;
	}
	

	// update clear 
	// 本地执行：清除本地设备信息表 struct tms_devbase
	else if(argc == 2 && memcmp(argv[1], "clear", strlen(argv[1])) == 0 ) {

_Clear:;
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 12; j++) {
				tms_RemoveByLocation(i, j);
			}
		}
	}
	else {
		printf("Usage:\n");
		printf("  disp cur\n");
		printf("  disp frame\n");
		printf("  disp connect\n");
		printf("  disp all\n");
	}
	
	return 0;

}
W_BOOT_CMD(disp, cmd_Disp, "Display more information");



#ifdef __cplusplus
}
#endif