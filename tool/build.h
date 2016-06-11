#ifndef _BUILD_H_
#define _BUILD_H_
#include <linux/list.h>

struct item {
	char *boot;
	char *name;
	struct list_head  next;				///<节点链表
	char *alias;
	char *help;
	int flag;
};

struct group {
	char *name;
	struct list_head next;
	struct item *item_first;
	int flag;
#define MS_REDEFINE (1 << 0)
};

struct line_map {
	int type;
	void *addr;		// struct group or struct item address
#define MST_GROUP	1
#define MST_ITEM    2
};

struct total_map {
	struct line_map *line;
	int index;
	int len;
};


// build-structure.c
typedef int (*ergodic_item_func)(struct item *val, void *ptr);
void ergodic_item(struct item *val, ergodic_item_func fun, void *ptr);
typedef int (*ergodic_group_func)(struct group *val, void *ptr);
void ergodic_group(struct group *val, ergodic_group_func fun, void *ptr);

void count_obj(struct group *val , long *count);
struct total_map *malloc_map_obj(struct group *val , long count);
void free_map_obj(struct total_map *map);
void check_redefine(struct line_map *lmap);

int disp_item(struct item *val);
int disp_group(struct group *val);


// wrxml.c
struct group *xmlloadconfig(char *name);
int xmlsaveconfig(char *name, struct group *val);

// wcfile.c
void cfilesave(char *file, struct group *val);
#endif