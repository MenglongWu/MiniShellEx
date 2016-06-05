#ifndef _BUILD_H_
#define _BUILD_H_
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


// build-structure.c
typedef int (*ergodic_item_func)(struct item *val, void *ptr);
void ergodic_item(struct item *val, ergodic_item_func fun, void *ptr);
typedef int (*ergodic_group_func)(struct group *val, void *ptr);
void ergodic_group(struct group *val, ergodic_group_func fun, void *ptr);


int disp_item(struct item *val);
int disp_group(struct group *val);


// wrxml.c
struct group *xmlloadconfig(char *name);
int xmlsaveconfig(char *name, struct group *val);

// wcfile.c
void cfilesave(char *file, struct group *val);
#endif