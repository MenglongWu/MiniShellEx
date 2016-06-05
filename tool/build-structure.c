#include <stdio.h>
#include <linux/list.h>
#include "build.h"

//////////////////////////////////////////////////////////
// ergodic link list
void ergodic_item(struct item *val, ergodic_item_func fun, void *ptr)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item*)list_entry(pos, struct item, next);
		fun(pnode, ptr);
	}
}

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

static int _disp_item(struct item *val, void *ptr)
{
	printf("\t\%s\n", val->name);
	return 0;
}
int disp_item(struct item *val)
{
	ergodic_item(val, _disp_item, NULL);
	return 0;
}


static int _disp_group(struct group *val, void *ptr)
{
	printf("%s\n", val->name);
	if (val->item_first) {
		disp_item(val->item_first);
	}
	return 0;
}
int disp_group(struct group *val)
{
	ergodic_group(val, _disp_group, NULL);
	return 0;
}

// End show link list
