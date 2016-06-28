#include <stdio.h>
#include <string.h>
#include <minishell_core.h>
#include "build.h"


////////////////////////////////////////////////////////////////////////
// wirte minishell struct cmd_prompt  in c file

int __cfilesave_def_do_x(struct item *val, void *ptr)
{
	if (val->flag & MS_REDEFINE) {
		return 0;
	}

	if ('\0' == val->name[0]) {
		return 0;
	}
	FILE *fp = (FILE *)ptr;

	fprintf(fp, "static int do_%s(void *ptr, int argc, char **argv);\n", val->name);
	return 0;
}

static int _cfilesave_def_do_x(struct group *val, void *ptr)
{
	if (val->item_first) {
		ergodic_item(val->item_first, __cfilesave_def_do_x, 	ptr);
	}
	return 0;
}

static int _cfilesave_def_boot_x(struct group *val, void *ptr)
{
	if (val->flag & MS_REDEFINE) {
		return 0;
	}
	FILE *fp = (FILE *)ptr;
	fprintf(fp, "extern struct cmd_prompt boot_%s[];\n", val->name);
	return 0;
}
static int _cfilesave_item(struct item *val, void *ptr)
{
	FILE *fp = (FILE *)ptr;
	char *alias;
	char *help;

	if (val->boot && strcmp(val->boot, "") != 0) {
		fprintf(fp, "\tPROMPT_NODE(boot_%s%4s", val->boot, ",");
	}
	else {
		fprintf(fp, "\tPROMPT_NODE(NULL %4s", ",");
	}


	if (0 != val->name[0]) {
		fprintf(fp, "      do_%s",  val->name);
	}
	else {
		fprintf(fp, "      NULL");

	}



	if (val->alias) {
		alias = val->alias;
	}
	else {
		alias = val->name;
	}
	fprintf(fp, ",\n\t\t (char*)\"%s\"  ,",
	        alias);


	if (val->help) {
		help = val->help;
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

	if (val->type) {
		if (strcmp((char*)val->type, "pass") == 0) {
			fprintf(fp, "\n\t\t (int)  CMDP_TYPE_PASS),\n");
		}
		else if(0) {
			// TODO user define
		}
	}
	else {
		fprintf(fp, "\n\t\t (int)  NULL),\n");
	}
	// int type = CMDP_TYPE_STR;
	// switch (type) {
	// case CMDP_TYPE_STR:
	// 	fprintf(fp, "\n\t\t (int)  NULL),\n");
	// 	break;
	// case CMDP_TYPE_PASS:
	// 	fprintf(fp, "\n\t\t (int)  CMDP_TYPE_PASS),\n");
	// 	break;
	// default:
	// 	printf("un know type\n");
	// 	break;
	// }

	return 0;
}

static int _cfilesave_group(struct group *val, void *ptr)
{
	if (val->flag & MS_REDEFINE) {
		return 0;
	}

	FILE *fp = (FILE *)ptr;
	fprintf(fp, "struct cmd_prompt boot_%s[] = {\n", val->name);
	if (val->item_first) {
		ergodic_item(val->item_first, _cfilesave_item, 	ptr);
	}
	fprintf(fp, "\tPROMPT_NODE(NULL    ,      NULL, (char *)NULL, (char *)NULL, (int *) NULL),\n");
	fprintf(fp, "};\n");
	return 0;
}


static int __cfilesave_fun(struct item *val, void *ptr)
{
	if (val->flag & MS_REDEFINE) {
		return 0;
	}

	if ('\0' == val->name[0]) {
		return 0;
	}
	FILE *fp = (FILE *)ptr;
	fprintf(fp,
	        "static int do_%s(void *ptr, int argc, char **argv)\n"
	        "{\n"
		"\tprintf(\"%%s\\n\", __FUNCTION__);\n"
	        "\treturn 0;\n"
	        "}\n\n",
	        val->name);

	return 0;
}

static int _cfilesave_fun(struct group *val, void *ptr)
{
	if (val->item_first) {
		ergodic_item(val->item_first, __cfilesave_fun, 	ptr);
	}
	return 0;
}

void cfilesave(char *file, struct group *val)
{
	FILE *fp;

	fp = fopen(file, "wr");

	fprintf(fp, "#include <minishell_core.h>\n\n");
	fprintf(fp, "#include <stdio.h>\n\n");
	ergodic_group(val, _cfilesave_def_do_x,     fp);
	fprintf(fp, "\n\n\n");

	ergodic_group(val, _cfilesave_def_boot_x, 	fp);
	fprintf(fp, "\n\n\n");


	ergodic_group(val, _cfilesave_group, 	    fp);
	fprintf(fp, "\n\n\n");

	ergodic_group(val, _cfilesave_fun,          fp);
	fprintf(fp, "\n\n\n");

	fclose(fp);
}

// End  wirte minishell struct cmd_prompt  in c file

