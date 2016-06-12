#include "stdio.h"
#include <minishell_core.h>
#include "shell.h"
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
int main()
{
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
	printf("[%s]", plist->name);
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
