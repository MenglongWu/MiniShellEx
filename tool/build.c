/**
 ******************************************************************************
 * @file	msbuild.c
 * @brief	MiniShell-Ex auto make 

MiniShell-Ex auto make struct cmd_prompt in a c file
 *
 @section Platform
	-# 
 @section Library
	-# 
- 2016-6-3,MenglongWu,MenglongWoo@aliyun.com
 	

 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2>
*/
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <minishell_core.h>

#include <readline/readline.h>
#include "build.h"
#include <stdbool.h>

char *in_xml;
char *out_xml;
char *out_c;

void usage()
{
	printf("Usage\n");
	printf("msbuild <input xml file> <output c file>\n");
}

bool check_exist(char *name)
{
	FILE *fp;

	fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	fclose(fp);
	return true;
}
int main (int argc , char **argv)
{
	if (argc < 3) {
		usage();
		exit(0);
	}


	in_xml = argv[1];
	out_c  = argv[2];
	if (check_exist(in_xml) == false) {
		printf("input file [%s] no find\n", in_xml);
		exit(0);
	}
	if (check_exist(out_c) == true) {
		printf("input file [%s] have exit, over write?[y/n] :", out_c);

		char ch;

		scanf("%c", &ch);
		switch (ch | 0x20) {
		case 'y':
			break;
		default:
			exit(0);
			break;
		}

	}


	struct group *head;

	head = xmlloadconfig(in_xml);
	if (head == NULL) {
		printf("open file\n");
		return 0;
	}
	disp_group(head);

	int count = 0;
	count_obj(head, &count);

	struct total_map *map;
	struct line_map *lmap;


	map = malloc_map_obj(head, count);
	lmap = map->line;


	check_redefine(lmap);
	cfilesave(out_c, head);
	// xmlsaveconfig(out_xml,head);
	free_map_obj(map);

}
