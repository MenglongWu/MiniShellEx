#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <minishell_core.h>

#include <readline/readline.h>
#include "build.h"



// #include <stdbool.h>
// extern bool isredefine_group(struct group *val , struct group *gp);
// extern void check_redefine(struct group *val , long len);
// extern bool isredefine_item(struct item *val, struct item *it);

// extern void count_obj(struct group *val , long *count);
// extern struct total_map *malloc_map_obj(struct group *val , long count);
// extern void free_map_obj(struct total_map *map);
// extern void check_redefine(struct line_map *lmap);
int main (int argc , char **argv)
{
	struct group *head;

	head = xmlloadconfig("def.xml");
	if (head == NULL) {
		printf("open file\n");
		return 0;
	}
	disp_group(head);
	// cfilesave("cmd_output.c", head);
	// xmlsaveconfig("out.xml",head);

	int count = 0;
	count_obj(head, &count);
	printf("count = %d\n", count);

	struct total_map *map;
	struct line_map *lmap;


	map = malloc_map_obj(head, count);
	lmap = map->line;

	
	check_redefine(lmap);
	cfilesave("cmd_output.c", head);
	xmlsaveconfig("out.xml",head);
	free_map_obj(map);
	return 0;
}