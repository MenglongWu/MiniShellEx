#include <stdio.h>
#include <linux/list.h>
#include "build.h"
#include <malloc.h>
#include <string.h>

//////////////////////////////////////////////////////////
// ergodic link list
void ergodic_item(struct item *val, ergodic_item_func fun, void *ptr)
{
	struct item *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct item *)list_entry(pos, struct item, next);
		if (0 != fun(pnode, ptr) ) {
			break;
		}
	}
}

void ergodic_group(struct group *val, ergodic_group_func fun, void *ptr)
{
	struct group *pnode;
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, &val->next) {
		pnode = (struct group *)list_entry(pos, struct group, next);
		if (0 != fun(pnode, ptr)) {
			break;
		}
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
// #include <stdbool.h>



static int __count_item(struct item *val, void *ptr)
{
	long *count = (long *)ptr;

	(*count) += 1;
	return 0;
}
static int _count_group(struct group *val, void *ptr)
{
	long *count = (long *)ptr;

	(*count) += 1;
	// printf("%s\n", val->name);
	ergodic_item(val->item_first, __count_item, 	    ptr);
	return 0;
}

void count_obj(struct group *val , long *count)
{
	ergodic_group(val, _count_group, 	    count);
}


static int __map_item(struct item *val, void *ptr)
{
	struct total_map *map = (struct total_map *)ptr;
	struct line_map *lmap;

	lmap = map->line + map->index;
	lmap->addr = val;
	lmap->type = MST_ITEM;
	// printf("item\n");

	map->index += 1;
	return 0;
}
static int _map_group(struct group *val, void *ptr)
{
	struct total_map *map = (struct total_map *)ptr;
	struct line_map *lmap;

	lmap = map->line + map->index;
	lmap->addr = val;
	lmap->type = MST_GROUP;

	// printf("group\n");

	map->index += 1;
	ergodic_item(val->item_first, __map_item, 	    ptr);
	return 0;
}


struct total_map *malloc_map_obj(struct group *val , long count)
{
	struct total_map *map = NULL;
	// last is all zero
	count = count + 1;

	map = (struct total_map *)malloc( sizeof(struct total_map));
	if (map == NULL) {
		return NULL;
	}
	bzero(map, sizeof(struct total_map));


	map->line = (struct line_map *)malloc( sizeof(struct line_map) * count );
	if (map->line == NULL) {
		free(map);
		return NULL;
	}
	bzero(map->line, sizeof(struct line_map) * count);
	map->index = 0;
	map->len = count;


	ergodic_group(val, _map_group, 	    map);

	return map;
}

void free_map_obj(struct total_map *map)
{
	if (map && map->line) {
		free(map->line);
		free(map);
	} else if(map) {
		free(map);
	}
}


void check_redefine(struct line_map *lmap)
{
	struct line_map *ilmap, *jlmap;
	struct group *igp, *jgp;
	struct item  *iit, *jit;

	for (ilmap = lmap; ilmap->type; ilmap++) {
		for (jlmap = lmap; jlmap != ilmap; jlmap++) {

			if (ilmap->type != jlmap->type) {
				continue;
			}

			switch(ilmap->type) {
			case MST_GROUP:
				igp = (struct group *)ilmap->addr;
				jgp = (struct group *)jlmap->addr;

				// printf("group %s %s\n", igp->name,jgp->name);
				if ( strcmp(igp->name, jgp->name) == 0) {
					// printf("group redefine %s\n", igp->name);
					igp->flag |= MS_REDEFINE;
					goto _Next;
				}

				break;
			case MST_ITEM:
				iit = (struct item *)ilmap->addr;
				jit = (struct item *)jlmap->addr;
				// printf("	item %s %s\n", iit->name, jit->name);
				if ( strcmp(iit->name, jit->name) == 0) {
					// printf("	item redefine %s\n", iit->name);
					iit->flag |= MS_REDEFINE;
					goto _Next;
				}
				break;
			}
		}
_Next:
		;
	}
}


