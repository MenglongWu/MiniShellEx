#include "stdio.h"
#include <minishell_core.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
// #include <xmalloc.h>
// #ifdef __cplusplus
// extern "C" {
// #endif
extern void funtest();
int do_c1_ex(int argc, char **argv);
int do_c2_ex(int argc, char **argv);
int do_up_level(int argc, char **argv);

int do_interface(void *ptr, int argc, char **argv);
struct cmd_prompt cmd_int_vlan[] = {
	PROMPT_NODE(NULL, NULL, (char*)"number", (char*)"specific vlan index[0-100]", NULL),
	PROMPT_NODE(NULL, do_up_level, "ul", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, NULL, NULL, NULL),
};
struct cmd_prompt cmd_interface[] = {
	PROMPT_NODE(cmd_int_vlan, NULL, "vlan", "specific vlan", NULL),
	PROMPT_NODE(NULL, NULL, "rang", "rang of port", NULL),
	PROMPT_NODE(NULL, NULL, "port", "only one port", NULL),
	PROMPT_NODE(NULL, do_up_level, "ul", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, NULL, NULL, NULL),
};

struct cmd_prompt cmd_1[] = {
	PROMPT_NODE(NULL, NULL, "11", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, "12", "abc", NULL),
	PROMPT_NODE(NULL, NULL, "c2", "abc", NULL),
	PROMPT_NODE(NULL, do_up_level, "ul", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, NULL, NULL, NULL),
};

struct cmd_prompt cmd_2[] = {
	PROMPT_NODE(NULL, NULL, "++", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, "--", "abc", NULL),
	PROMPT_NODE(NULL, NULL, "c2", "abc", NULL),
	PROMPT_NODE(NULL, do_up_level, "ul", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, NULL, NULL, NULL),
};
extern char *rl_line_buffer;
int do_interface(void *ptr, int argc, char **argv)
{
	char strout[32];
	int vlan_num;

	printf("%s():%d\n", __FUNCTION__, __LINE__);
	printf("%s ", rl_line_buffer);
	// for (int i = 0; i < argc;i++) {
	//   printf("%d %s \n", i, argv[i]);
	// }
	// return 0;
	if (argc == 3 &&
			memcmp(argv[1], (char*)"vlan", strlen(argv[1])) == 0) {
		vlan_num = atoi(argv[2]);
		snprintf(strout, 32, "vlan/%d", vlan_num);
		sh_editpath(strout);
		sh_down_prompt_level(cmd_int_vlan);
	}
	// sh_down_prompt_level(cmd_interface);


	return 0;
}

struct cmd_prompt cmd_boot[] = {
	// PROMPT_NODE("'param'","<osw frame/slot/port>",NULL,NULL),
	PROMPT_NODE(cmd_1, do_c1_ex, "c111", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(cmd_2, do_c2_ex, "c222", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(cmd_interface, &do_interface, "i--abc", "int to interface", NULL),
	PROMPT_NODE(NULL, NULL, "i--123", "int to interface", NULL),
	PROMPT_NODE(NULL, NULL, "quit", "<osw frame/slot/port>", NULL),
	PROMPT_NODE(NULL, NULL, NULL, NULL, NULL),
};

int do_c1(int argc, char **argv)
{
	// struct cmd_prompt *sh_downlevel(
	//   struct cmd_prompt *level)
	// printf("old  \n");
	// sh_down_prompt_level(cmd_1);
}

int do_c2(int argc, char **argv)
{
	// printf("old  \n");
	// sh_down_prompt_level(cmd_2);
	return 0;
}

int do_up_level(int argc, char **argv)
{
	sh_up_prompt_level();
	return 0;
}

int do_c1_ex(int argc, char **argv)
{
	// struct cmd_prompt *sh_downlevel(
	//   struct cmd_prompt *level)
	printf("go to cmd1\n");
	sh_down_prompt_level(cmd_1);
}

int do_c2_ex(int argc, char **argv)
{
	// struct cmd_prompt *sh_downlevel(
	//   struct cmd_prompt *level)
	printf("go to cmd2\n");
	sh_down_prompt_level(cmd_2);
}

// const char str[] = "asbcdfasf";
#include <linux/list.h>
struct item
{
	char *boot;
	// char a[2];
	char *name;

	struct list_head  list;				///<节点链表
	char *alias;
	char *help;
};

struct group
{
	// char a[6];
	char *name;
	struct list_head list;
	struct item *item_first;
};



struct group head;

// int add_group(struct group *val, struct group *item)
// {
// 	list_add_tail(&(val)->list, &item->list);
// }

void disp_item(struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct item*)list_entry(pos, struct item, list);
		printf("\t[%s] %s  %s  %s\n", pnode->name, pnode->boot, pnode->alias, pnode->help);
		// printf("\t[%s] \n", pnode->name);
	}
}
void disp_group(struct group *val)
{
	struct group *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct group*)list_entry(pos, struct group, list);
		printf("[%s]\n", pnode->name);
		if (pnode->item_first) {
			disp_item(pnode->item_first);	
		}

	}
}

char *loadattrib(FILE *fp, char *attrib)
{
	static char strout[64];
	static char output[64];

	fgets(strout, 64, fp);
	if (memcmp(strout ,"alias", strlen("alias")-1) == 0) {
		sscanf(strout,"alias = %64s",  output);
		printf("strout %s\n", output);
	}
	else {
		output[0] = '\0';
	}
	return output;
}

// struct item *loaditem(FILE *fp)
// {
// 	char strout[128];
// 	struct item *new_item, *phead;
// 	int file_offset;
// 	char *attribute;

// 	phead = (struct item*)malloc(sizeof (struct item));
// 	INIT_LIST_HEAD(&phead->list);

// 	while(!feof(fp)) {
// 		strout[0] = '\0';
// 		fgets(strout, 128, fp);

// 		// printf("%s %d", strout, );
// 		if (memcmp(strout ,"item", strlen("item")-1) == 0) {
// 			// printf("%s\n", strout);

// 			new_item = (struct item*)malloc(sizeof(struct item ));

// 			sscanf(strout,"item = %12s", new_item->name);
// 			list_add_tail(&new_item->list, &phead->list);

// 			// file_offset = ftell(fp);
// 			attribute = loadattrib(fp,"alias");
// 			new_item->alias =  malloc(sizeof(attribute));
// 			strcpy(new_item->alias, attribute);
// 			fseek(fp, file_offset, SEEK_SET);

// 			// attribute = loadattrib(fp,"help");
// 			// new_item->help =  malloc(sizeof(attribute));
// 			// strcpy(new_item->help, attribute);
// 			// fseek(fp, file_offset, SEEK_SET);


// 			// printf("name %s\n",new_item->alias);


// 		}
// 		else {
// 			break;
// 		}
// 	}
// 	return phead;
// }
// struct group *loadfile(char *file)
// {
// 	FILE *fp;
// 	struct group *phead;

// 	phead = (struct group*)malloc(sizeof (struct group));
// 	INIT_LIST_HEAD(&phead->list);



// 	fp = fopen("list.cfg", "r");
// 	if (fp == NULL) {
// 		return ;
// 	}
// 	char strout[128];
// 	int ret;
// 	struct group *new_group, *val = &head;
// 	struct item *item_head;

// 	int file_offset;

// 	while(!feof(fp)) {
// 		strout[0] = '\0';
// 		fgets(strout, 128, fp);
// 		// printf("%s %d", strout, );
// 		if (memcmp(strout ,"title", strlen("title")-1) == 0) {
// 			// printf("%s\n", strout);

// 			new_group = (struct group*)malloc(sizeof(struct group ));
// 			INIT_LIST_HEAD(&new_group->list);
// 			sscanf(strout,"title = %12s", new_group->name);
// 			list_add_tail(&new_group->list, &phead->list);
// 			file_offset = ftell(fp);
// 			item_head = loaditem(fp);
// 			new_group->item_first = item_head;
// 			fseek(fp, file_offset, SEEK_SET);
// 			// break;
// 		}
// 	}

// 	fclose(fp);

// 	// disp_group(phead);
// 	return phead;
// }

#include <libxml/parser.h>

#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

struct item * xmlloaditem(xmlNodePtr proot)
{
	struct item *phead;
	struct item *new_node;

	phead = (struct item*)malloc(sizeof (struct item));
	bzero(phead, sizeof(struct item));
	INIT_LIST_HEAD(&phead->list);


	while (proot != NULL)
	{
		if (!xmlStrcmp(proot->name, BAD_CAST("item"))) {
			xmlAttrPtr attrPtr = proot->properties;
			new_node = (struct item*)malloc(sizeof(struct item ));
			bzero(new_node, sizeof(struct item));
			list_add_tail(&new_node->list, &phead->list);

			while(attrPtr != NULL) {

				printf("kkk     att %s \n", attrPtr->name);
				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "name")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "name");
					// printf("get name = %s\n", szAttr);


					new_node->name = (char*)malloc(strlen(szAttr));
					strcpy(new_node->name, szAttr);
					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "boot")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "boot");
					// printf("get boot = %s\n", szAttr);

					new_node->boot = (char*)malloc(strlen(szAttr));
					strcpy(new_node->boot, szAttr);
					// exit (0);


					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "alias")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "alias");
					printf("get alias = %s\n", szAttr);

					new_node->alias = (char*)malloc(strlen(szAttr));
					strcpy(new_node->alias, szAttr);
					// exit (0);


					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "help")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "help");
					printf("get help = %s\n", szAttr);

					new_node->help = (char*)malloc(strlen(szAttr));
					strcpy(new_node->help, szAttr);
					// exit (0);


					xmlFree(szAttr);			
				}

				attrPtr = attrPtr->next;
			}


		}

		proot = proot->next;
	}
	printf("mabe --------------\n");
	printf("new_node->name [   %s  ]\n\n", new_node->name);
	disp_item(phead);
	return phead;
}

struct grout * xmlloadgrout(xmlNodePtr proot)
{
	struct group *phead;
	struct group *new_node;

	phead = (struct group*)malloc(sizeof (struct group));
	bzero(phead, sizeof(struct group));
	INIT_LIST_HEAD(&phead->list);


	while (proot != NULL)
	{
		printf("%s\n", proot->name);
		if (!xmlStrcmp(proot->name, BAD_CAST("group"))) {
			new_node = (struct group*)malloc(sizeof(struct group ));
			bzero(new_node, sizeof(struct group));
			INIT_LIST_HEAD(&new_node->list);
			list_add_tail(&new_node->list, &phead->list);


			xmlAttrPtr attrPtr = proot->properties;

			if (!xmlStrcmp(attrPtr->name,  BAD_CAST "name")) {
				xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "name");
				// printf("get name = %s\n", szAttr);



				// strcpy(new_node->name, szAttr);
				new_node->name = (char*)malloc(strlen(szAttr));
				strcpy(new_node->name, szAttr);



				xmlFree(szAttr);

				new_node->item_first= xmlloaditem(proot->xmlChildrenNode);

				// disp_item(new_node->item_first);
			}


			attrPtr = attrPtr->next;
		}

		proot = proot->next;
	}
	printf("................\n");
	disp_group(phead);
	return phead;
}
void fun()
{
	struct item head;
	struct item it1;
	struct item it2;
	INIT_LIST_HEAD(&head.list);
	strcpy(it1.name ,"it1");
	strcpy(it2.name ,"it2");

	list_add_tail(&it1.list, &head.list);
	list_add_tail(&it2.list, &head.list);
	disp_item(&head);
}
int main (int argc , char **argv)
{
	// fun();
	// return 0;
	xmlDocPtr pdoc = NULL;
	xmlNodePtr proot = NULL, pcur = NULL;
	xmlAttrPtr attrPtr = NULL;

	/*****************打开xml文档********************/
	xmlKeepBlanksDefault(0);//必须加上，防止程序把元素前后的空白文本符号当作一个node
	pdoc = xmlReadFile ("def.xml", "UTF-8", XML_PARSE_RECOVER);//libxml只能解析UTF-8格式数据

	if (pdoc == NULL)
	{
		printf ("error:can't open file!\n");
		exit (1);
	}

	/*****************获取xml文档对象的根节对象********************/
	proot = xmlDocGetRootElement (pdoc);

	if (proot == NULL)
	{
		printf("error: file is empty!\n");
		exit (1);
	}

	/*****************查找书店中所有书籍的名称********************/
	pcur = proot->xmlChildrenNode;

	xmlNodePtr pitem;


	struct group *head;

	head = xmlloadgrout(pcur);
	// disp_group(head);

	create_cmdboot(NULL, head);
	// while (pcur != NULL)
	// {
	// 	printf("%s\n", pcur->name);
	// 	pitem = pcur->xmlChildrenNode;
	// 	while (pitem != NULL)
	// 	{	
	// 		printf("%s\n", pitem->name);
	// 		pitem = pitem->next;
	// 	}
	// 	pcur = pcur->next;
	// }
	return 0;
	//   return 0;
	while (pcur != NULL)
	{
		//如同标准C中的char类型一样，xmlChar也有动态内存分配，字符串操作等 相关函数。例如xmlMalloc是动态分配内存的函数；xmlFree是配套的释放内存函数；xmlStrcmp是字符串比较函数等。
		//对于char* ch="book", xmlChar* xch=BAD_CAST(ch)或者xmlChar* xch=(const xmlChar *)(ch)
		//对于xmlChar* xch=BAD_CAST("book")，char* ch=(char *)(xch)




		printf("%s \n", pcur->name);

		attrPtr = pcur->properties;
		while (attrPtr != NULL)
		{
			printf("[%s]\n", attrPtr->name);
			// if (!xmlStrcmp(attrPtr->name, BAD_CAST "attribute"))
			// {
			//     xmlChar* szAttr = xmlGetProp(pcur,BAD_CAST "attribute");
			//     // coutxmlFree(szAttr);
			// }
			attrPtr = attrPtr->next;
		}

		if (!xmlStrcmp(pcur->name, BAD_CAST("group")))
		{
			xmlNodePtr nptr=pcur->xmlChildrenNode;
			while (pcur != NULL)
			{
				if (!xmlStrcmp(nptr->name, BAD_CAST("item")))
				{
					printf("item: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
					break;
					// continue;
				}

				if (!xmlStrcmp(nptr->name, BAD_CAST("price")))
				{
					printf("price: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
					break;
				}
				// pcur = pcur->next;
			}
		}
		pcur = pcur->next;
	}

	/*****************释放资源********************/
	xmlFreeDoc (pdoc);
	xmlCleanupParser ();
	xmlMemoryDump ();
	return 0;
}




void create_cmditem(FILE *fp, struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;
	char *boot;
	char *alias;
	char *help;

	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct item*)list_entry(pos, struct item, list);


		
		printf("name %s\n", pnode->name);
		if (pnode->boot && strcmp(pnode->boot, "") != 0) {
			fprintf(fp, "\tPROMPT_NODE(boot_%s",pnode->boot);
		}
		else {
			fprintf(fp, "\tPROMPT_NODE(NULL");
		}


		if (pnode->name) {
			fprintf(fp, ",\tdo_%s",
				pnode->name);
		}
		else {
			fprintf(fp, ",\tNULL");

		}



		if (pnode->alias) {
			alias = pnode->alias;
		}
		else {
			alias = pnode->name;
		}
		fprintf(fp, ",\n\t\t (char*)\"%s\"  ",
				alias);


		if (pnode->help) {
			help = pnode->help;
			fprintf(fp, ",\n\t\t (char*)\"%s\"\n",
				help);
		}
		else {
			help = "NULL";
			fprintf(fp, ",\n\t\t (char*)NULL\n");
		}
		fprintf(fp, ",\n\t\t NULL),\n");
		
		
	}
}

void create_cmditem_define(FILE *fp, struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct item*)list_entry(pos, struct item, list);
		fprintf(fp,
				"int do_%s(void *ptr, int argc, char **argv);\n",
				pnode->name);
	}
}

// void create_cmdgroup_define(FILE *fp, struct group *val)
// {
// }

void create_cmditem_fb(FILE *fp, struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct item*)list_entry(pos, struct item, list);
		fprintf(fp,
				"int do_%s(void *ptr, int argc, char **argv)\n"
				"{\n"
				"\treturn 0;\n"
				"};\n\n",
				pnode->name);
	}
}

void create_cmdboot(char *file, struct group *val)
{
	FILE *fp;
	struct group *pnode;
	struct list_head *pos, *n;

	fp = fopen("cmd_output.c", "wr");


	fprintf(fp, "#include <minishell_core.h>\n\n");
	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct group*)list_entry(pos, struct group, list);
		create_cmditem_define(fp, pnode->item_first);
	}
	fprintf(fp, "\n\n\n");


	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct group*)list_entry(pos, struct group, list);
		// create_cmditem_define(fp, pnode->item_first);
		printf("boot name %s\n", pnode->name);
		fprintf(fp, "struct cmd_prompt boot_%s[];\n", pnode->name);
	}
	fprintf(fp, "\n\n");


	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct group*)list_entry(pos, struct group, list);
		fprintf(fp, "struct cmd_prompt boot_%s[] = {\n", pnode->name);
		create_cmditem(fp, pnode->item_first);
		fprintf(fp, "};\n");
	}
	fprintf(fp, "\n\n\n");


	list_for_each_safe(pos, n, &val->list) {
		pnode = (struct group*)list_entry(pos, struct group, list);
		create_cmditem_fb(fp, pnode->item_first);
	}
	fprintf(fp, "\n\n\n");

	fclose(fp);
}

int main3()
{
	struct group *head;
	// head = loadfile(NULL);
	// disp_group(head);
	printf("create file\n");
	create_cmdboot(NULL, head);
	// createxml();
	// sh_cmdboot(cmd_tbl_list);
	// sh_enter();
	// void (*fun)(int);
	// fun = (void(*)(void))do_interface;
	sh_whereboot(cmd_boot);
	// funtest();
	// initialize_readline();
	struct sh_detach_depth depth;
	char *cmd[12];

	depth.cmd = cmd;
	depth.len = 12;
	depth.seps = " \t";


	sh_enter_ex(&depth, NULL);

	return 0;
}


// typedef int rl_icpfunc_t (char *);
// typedef struct {
// 	char *name;     /* User printable name of the function. */
// 	rl_icpfunc_t *func;   /* Function to call to do the job. */
// 	char *doc;      /* Documentation for this function.  */
// } COMMAND;

// com_cd (
//     char *arg)
// {

// }
// COMMAND commands[] = {
// 	{ "cd", com_cd, "Change to directory DIR" },
// 	{ "delete", com_cd, "Delete FILE" },
// 	{ "help", com_cd, "Display this text" },
// 	{ "?", com_cd, "Synonym for `help'" },
// 	{ "list", com_cd, "List files in DIR" },
// 	{ "ls", com_cd, "Synonym for `list'" },
// 	{ "pwd", com_cd, "Print the current working directory" },
// 	{ "quit", com_cd, "Quit using Fileman" },
// 	{ "rename", com_cd, "Rename FILE to NEWNAME" },
// 	{ "stat", com_cd, "Print out statistics on FILE" },
// 	{ "view", com_cd, "View the contents of FILE" },
// 	{ (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
// };


char **
_sh_fileman_completion (
		const char *text,
		int start, int end);
static char *
_sh_command_generator (
		const char *text,
		int state);
extern const char *rl_readline_name ;
// extern char **rl_attempted_completion_function)(const char *,int , int );
extern rl_completion_func_t *rl_attempted_completion_function;
void initialize_readline ()
{
	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = "FileMan";

	/* Tell the completer that we want a crack first. */
	rl_attempted_completion_function = _sh_fileman_completion;
}

char **
_sh_fileman_completion (
		const char *text,
		int start, int end)
{
	char **matches;

	matches = (char **)NULL;

	/* If this word is at the start of the line, then it is a command
	   to complete.  Otherwise it is the name of a file in the current
	   directory. */
	// printf("\n[text %s]\n", text);
	if (start == 0) {
		matches = rl_completion_matches (text, _sh_command_generator);
	}

	return (matches);
}

/*
 *rl_complete_internal
 * gen_completion_matches
 *   rl_completion_matches
 *     _sh_command_generator
 *       malloc
 * free
 */
#define PROMPT_DEPTH (16)
extern struct cmd_prompt *_prompt_tree[PROMPT_DEPTH];
extern int         _prompt_index;

	static char *
_sh_dupstr (s)
	char *s;
{
	char *r;

	r = xmalloc (strlen (s) + 1);
	// printf("mem %x\n", r);
	strcpy (r, s);
	return (r);
}
/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char *
_sh_command_generator (
		const char *text,
		int state)
{
	static int list_index, len;
	char *name;
	/* If this is a new word to complete, initialize now.  This includes
	   saving the length of TEXT for efficiency, and initializing the index
	   variable to 0. */
	if (!state) {
		list_index = 0;
		len = strlen (text);
	}

	/* Return the next name which partially matches from the command list. */
	// while (name = commands[list_index].name) {
	// while (name = _prompt_tree[_prompt_index][list_index].name) {
	struct cmd_prompt *p;
	p = _prompt_tree[_prompt_index];
	// p = gp_index;
	while (name = p[list_index].name) {
		list_index++;
		if (strncmp (name, text, len) == 0) {
			return (_sh_dupstr(name));
		}
	}

	/* If no names matched, then return NULL. */
	return ((char *)NULL);
}
extern char *rl_line_buffer;
char *
_sh_command_generator3 (
		const char *text,
		int state)
{
	static int list_index, len;
	char *name;
	char buf[1024];
	char *pbuf = buf;

	char *cmd[256];
	int count;
	// rl_vi_put (2,'?');
	// return 0;
	memcpy(pbuf, rl_line_buffer, len);


	sh_detach_xx_fmt(pbuf, len, cmd, &count);

	struct cmd_prompt *plist;
	int ret;
	printf("index %d\n", _prompt_index);
	ret = searchboot(count, cmd, _prompt_tree[_prompt_index], &plist);
	/* If this is a new word to complete, initialize now.  This includes
	   saving the length of TEXT for efficiency, and initializing the index
	   variable to 0. */
	if (!state) {
		list_index = 0;
		len = strlen (text);
	}
	// printf("[%s]", plist->name);
	/* Return the next name which partially matches from the command list. */
	// while (name = commands[list_index].name) {
	while (name = _prompt_tree[_prompt_index][list_index].name) {
		list_index++;
		if (strncmp (name, text, len) == 0) {
			return (_sh_dupstr(name));
		}
	}

	/* If no names matched, then return NULL. */
	return ((char *)NULL);
}

// #ifdef __cplusplus
// }
// #endif
