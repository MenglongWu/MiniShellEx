#include "stdio.h"
#include <minishell_core.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>


#include <linux/list.h>
struct item
{
	char *boot;
	char *name;
	struct list_head  next;				///<节点链表
	char *alias;
	char *help;
};

struct group
{
	char *name;
	struct list_head next;
	struct item *item_first;
};



//////////////////////////////////////////////////////////
// ergodic link list

typedef void (*ergodic_item_func)(struct item *val, void *ptr);
void ergodic_item(struct item *val, ergodic_item_func fun, void *ptr)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item*)list_entry(pos, struct item, next);
		fun(pnode, ptr);
		// printf("\t[%s] %s  %s  %s\n", pnode->name, pnode->boot, pnode->alias, pnode->help);
		// printf("\t[%s] -> %s\n", pnode->name, pnode->boot);
	}
}


typedef void (*ergodic_group_func)(struct group *val, void *ptr);
void ergodic_group(struct group *val, ergodic_group_func fun, void *ptr)
{
	struct group *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct group*)list_entry(pos, struct group, next);
		fun(pnode, ptr);
	}
}

// End ergodic link list


//////////////////////////////////////////////////////////
// show link list

static void _disp_item(struct item *val)
{
	printf("\t\%s\n", val->name);
}
void disp_item(struct item *val)
{
	ergodic_item(val, _disp_item, NULL);
}


static void _disp_group(struct group *val)
{
	printf("%s\n", val->name);
	if (val->item_first) {
		disp_item(val->item_first);
	}
}
void disp_group(struct group *val)
{
	ergodic_group(val, _disp_group, NULL);
}

// End show link list


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
	INIT_LIST_HEAD(&phead->next);


	while (proot != NULL)
	{
		if (!xmlStrcmp(proot->name, BAD_CAST("item"))) {
			xmlAttrPtr attrPtr = proot->properties;
			new_node = (struct item*)malloc(sizeof(struct item ));
			bzero(new_node, sizeof(struct item));
			list_add_tail(&new_node->next, &phead->next);

			while(attrPtr != NULL) {
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
#ifdef DEBUG
					printf("get alias = %s\n", szAttr);
#endif

					new_node->alias = (char*)malloc(strlen(szAttr));
					strcpy(new_node->alias, szAttr);
					// exit (0);


					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "help")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "help");
#ifdef DEBUG
					printf("get help = %s\n", szAttr);
#endif

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
	return phead;
}

struct group * xmlloadgroup(xmlNodePtr proot)
{
	struct group *phead;
	struct group *new_node;

	phead = (struct group*)malloc(sizeof (struct group));
	bzero(phead, sizeof(struct group));
	INIT_LIST_HEAD(&phead->next);


	while (proot != NULL)
	{
#ifdef DEBUG
		printf("%s\n", proot->name);
#endif
		if (!xmlStrcmp(proot->name, BAD_CAST("group"))) {
			new_node = (struct group*)malloc(sizeof(struct group ));
			bzero(new_node, sizeof(struct group));
			INIT_LIST_HEAD(&new_node->next);
			list_add_tail(&new_node->next, &phead->next);


			xmlAttrPtr attrPtr = proot->properties;

			if (!xmlStrcmp(attrPtr->name,  BAD_CAST "name")) {
				xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "name");
				// printf("get name = %s\n", szAttr);



				// strcpy(new_node->name, szAttr);
				new_node->name = (char*)malloc(strlen(szAttr));
				strcpy(new_node->name, szAttr);



				xmlFree(szAttr);

				new_node->item_first= xmlloaditem(proot->xmlChildrenNode);
			}


			attrPtr = attrPtr->next;
		}

		proot = proot->next;
	}
	return phead;
}

int xmlsavecfg_item(xmlNodePtr proot, struct item* val)
{
	struct item *pnode;
	struct list_head *pos, *n;


	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item*)list_entry(pos, struct item, next);
	}
}



static void _xmlwrite_item(struct item *val, void *ptr)
{
	xmlNsPtr proot = (xmlNodePtr)ptr;
	xmlNodePtr child_node;

	child_node = xmlNewChild(proot, NULL, BAD_CAST "item",BAD_CAST "");
	xmlNewProp(child_node, BAD_CAST "name", BAD_CAST val->name);
	xmlNewProp(child_node, BAD_CAST "alias", BAD_CAST val->alias);
	xmlNewProp(child_node, BAD_CAST "help", BAD_CAST val->help);

	// if (val->item_first) {
	// 	xmlwrite_item(val->item_first);
	// }
	xmlAddChild(proot, child_node);
}
void xmlwrite_item(xmlNsPtr proot, struct item* val)
{
	ergodic_item(val, _xmlwrite_item, proot);
}

static void _xmlwrite_group(struct group *val, void *ptr)
{
	xmlNodePtr proot = (xmlNodePtr)ptr;
	xmlNodePtr child_node;

	xmlNsPtr group_node = xmlNewNode(NULL, BAD_CAST "group");
	xmlNewProp(group_node, BAD_CAST "name", BAD_CAST val->name);
	xmlAddChild(proot,group_node);

	if (val->item_first) {
		xmlwrite_item(group_node, val->item_first);
	}
	xmlAddChild(proot,group_node);

}
void xmlwrite_group(xmlNodePtr proot, struct group* val)
{
	ergodic_group(val, _xmlwrite_group, proot);
}

int xmlsaveconfig(char *name, struct group *val)
{
	xmlDocPtr doc = NULL;       /* document pointer */

	xmlNodePtr proot = NULL, node = NULL, node1 = NULL;/* node pointers */

	// Creates a new document, a node and set it as a root node

	doc = xmlNewDoc(BAD_CAST "1.0");

	proot = xmlNewNode(NULL, BAD_CAST "minishell");


	xmlDocSetRootElement(doc, proot);

	xmlwrite_group(proot, val);
	// xmlsavecfg_group(proot, val);
	//creates a new node, which is "attached" as child node of proot node.

	// xmlNewChild(proot, NULL, BAD_CAST "node1",BAD_CAST "content of node1");

	// // xmlNewProp() creates attributes, which is "attached" to an node.

	// node=xmlNewChild(proot, NULL, BAD_CAST "node3", BAD_CAST"node has attributes");

	// xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");

	// //Here goes another way to create nodes.

	// node = xmlNewNode(NULL, BAD_CAST "node4");

	// node1 = xmlNewText(BAD_CAST"other way to create content");

	// xmlAddChild(node, node1);

	// xmlAddChild(proot, node);

	//Dumping document to stdio or file

	xmlSaveFormatFileEnc(name, doc, "UTF-8", 1);

	/*free the document */

	xmlFreeDoc(doc);

	xmlCleanupParser();

	xmlMemoryDump();//debug memory for regression tests
}

int main (int argc , char **argv)
{
	
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

	head = xmlloadgroup(pcur);
	disp_group(head);
	create_cmdgroup(NULL, head);
	// create_cmdgroup(NULL, head);

	xmlsaveconfig("out.xml",head);
	return 0;
}




void create_cmditem(FILE *fp, struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;
	char *boot;
	char *alias;
	char *help;

	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item*)list_entry(pos, struct item, next);
		if (pnode->boot && strcmp(pnode->boot, "") != 0) {
			fprintf(fp, "\tPROMPT_NODE(boot_%s%4s",pnode->boot, ",");
		}
		else {
			fprintf(fp, "\tPROMPT_NODE(NULL %4s", ",");
		}


		if (pnode->name) {
			fprintf(fp, "      do_%s",
				pnode->name);
		}
		else {
			fprintf(fp, "      NULL");

		}



		if (pnode->alias) {
			alias = pnode->alias;
		}
		else {
			alias = pnode->name;
		}
		fprintf(fp, ",\n\t\t (char*)\"%s\"  ,",
				alias);


		if (pnode->help) {
			help = pnode->help;
			fprintf(fp, "\n\t\t (char*)\"%s\",",
				help);
		}
		else {
			help = "NULL";
			fprintf(fp, "\n\t\t (char*)NULL,");
		}

		// TODO type 
		// allow user change the item type, 
		// only support CMDP_TYPE_STR,CMDP_TYPE_PASS now
		int type = CMDP_TYPE_STR;
		switch (type) {
		case CMDP_TYPE_STR:
			fprintf(fp, "\n\t\t (int)  NULL),\n");
			break;
		case CMDP_TYPE_PASS:
			fprintf(fp, "\n\t\t (int)  CMDP_TYPE_PASS),\n");
			break;
		default:
			printf("un know type\n");
			break; 
		}
	}
}

void create_cmditem_define(FILE *fp, struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;

	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item*)list_entry(pos, struct item, next);
	
		fprintf(fp,
			"int do_%s(void *ptr, int argc, char **argv);\n",
			pnode->name);
	
		
	}
}



void create_cmditem_fb(FILE *fp, struct item *val)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item*)list_entry(pos, struct item, next);
		fprintf(fp,
				"int do_%s(void *ptr, int argc, char **argv)\n"
				"{\n"
				"\treturn 0;\n"
				"};\n\n",
				pnode->name);
	}
}

void create_cmdgroup(char *file, struct group *val)
{
	FILE *fp;
	struct group *pnode;
	struct list_head *pos, *n;

	fp = fopen("cmd_output.c", "wr");


	fprintf(fp, "#include <minishell_core.h>\n\n");
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct group*)list_entry(pos, struct group, next);
		create_cmditem_define(fp, pnode->item_first);
	}
	fprintf(fp, "\n\n\n");


	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct group*)list_entry(pos, struct group, next);
		// create_cmditem_define(fp, pnode->item_first);
		fprintf(fp, "struct cmd_prompt boot_%s[];\n", pnode->name);
	}
	fprintf(fp, "\n\n");


	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct group*)list_entry(pos, struct group, next);
		fprintf(fp, "struct cmd_prompt boot_%s[] = {\n", pnode->name);
		create_cmditem(fp, pnode->item_first);
		fprintf(fp, "};\n");
	}
	fprintf(fp, "\n\n\n");


	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct group*)list_entry(pos, struct group, next);
		create_cmditem_fb(fp, pnode->item_first);
	}
	fprintf(fp, "\n\n\n");

	fclose(fp);
}
