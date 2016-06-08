#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "build.h"

////////////////////////////////////////////////////////////////////////
// load xml file
static struct item * _xmlloaditem(xmlNodePtr proot)
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

					new_node->name = (char*)malloc(strlen((char*)szAttr) + 1);
					strcpy(new_node->name, (char*)szAttr);
					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "boot")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "boot");
					new_node->boot = (char*)malloc(strlen((char*)szAttr) + 1);
					strcpy(new_node->boot, (char*)szAttr);


					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "alias")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "alias");
#ifdef DEBUG
					printf("get alias = %s\n", szAttr);
#endif

					new_node->alias = (char*)malloc(strlen((char*)szAttr) + 1);
					strcpy(new_node->alias, (char*)szAttr);

					xmlFree(szAttr);			
				}

				if (!xmlStrcmp(attrPtr->name,  BAD_CAST "help")) {
					xmlChar* szAttr = xmlGetProp(proot,  BAD_CAST  "help");
#ifdef DEBUG
					printf("get help = %s\n", (char*)szAttr);
#endif

					new_node->help = (char*)malloc(strlen((char*)szAttr) + 1);
					strcpy(new_node->help, (char*)szAttr);

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
				new_node->name = (char*)malloc(strlen((char*)szAttr) + 1);
				strcpy(new_node->name, (char*)szAttr);
				xmlFree(szAttr);
				new_node->item_first= _xmlloaditem(proot->xmlChildrenNode);
			}
			attrPtr = attrPtr->next;
		}
		proot = proot->next;
	}
	return phead;
}

struct group *xmlloadconfig(char *name)
{
	xmlDocPtr pdoc = NULL;
	xmlNodePtr proot = NULL, pcur = NULL;
	// xmlAttrPtr attrPtr = NULL;
	struct group *pret = NULL;


	/*****************打开xml文档********************/
	xmlKeepBlanksDefault(0);//必须加上，防止程序把元素前后的空白文本符号当作一个node
	pdoc = xmlReadFile (name, "UTF-8", XML_PARSE_RECOVER);//libxml只能解析UTF-8格式数据
	if (pdoc == NULL)
	{
		printf ("error:can't open file!\n");
		goto _error;
	}

	proot = xmlDocGetRootElement (pdoc);
	if (proot == NULL)
	{
		printf("error: file is empty!\n");
		goto _error;
	}

	pcur = proot->xmlChildrenNode;

	
	pret = xmlloadgroup(pcur);
_error:;
	// free libxml2 memory
	xmlFreeDoc(pdoc);
	// xmlCleanupParser();
	return pret;

}


// End load xml file


////////////////////////////////////////////////////////////////////////
// write xml file
static int _xmlwrite_item(struct item *val, void *ptr)
{
	xmlNsPtr proot = (xmlNsPtr)ptr;
	xmlNodePtr child_node;

	
	/* 
		maybe this    error xmlNsPtr -> xmlNodePtr
		I don't goog as libxml2.so
		so far "xmlNewChild((xmlNodePtr)proot" no error occur
		this code will be change not long after
	*/
	child_node = xmlNewChild((xmlNodePtr)proot, NULL, BAD_CAST "item",BAD_CAST "");
	xmlNewProp(child_node, BAD_CAST "name", BAD_CAST val->name);
	xmlNewProp(child_node, BAD_CAST "alias", BAD_CAST val->alias);
	xmlNewProp(child_node, BAD_CAST "help", BAD_CAST val->help);

	xmlAddChild((xmlNodePtr)proot, child_node);
	return 0;
}
static void xmlwrite_item(xmlNsPtr proot, struct item* val)
{
	ergodic_item(val, _xmlwrite_item, proot);
}

static int _xmlwrite_group(struct group *val, void *ptr)
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
	return 0;
}
static void xmlwrite_group(xmlNodePtr proot, struct group* val)
{
	ergodic_group(val, _xmlwrite_group, proot);
}



int xmlsaveconfig(char *name, struct group *val)
{
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr proot = NULL;

	// Creates a new document, a node and set it as a root node

	doc = xmlNewDoc(BAD_CAST "1.0");
	proot = xmlNewNode(NULL, BAD_CAST "minishell");


	xmlDocSetRootElement(doc, proot);

	// ergodic all group and item, write it in memory
	xmlwrite_group(proot, val);

	// from memory to file
	xmlSaveFormatFileEnc(name, doc, "UTF-8", 1);


	/*free the document */
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();//debug memory for regression tests
	return 0;
}

// End  write xml file


