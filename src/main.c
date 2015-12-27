#include "stdio.h"
#include <minishell_core.h>

// #ifdef __cplusplus
// extern "C" {
// #endif

int do_c1_ex(int argc, char **argv);
int do_c2_ex(int argc, char **argv);
int do_up_level(int argc, char **argv);

struct cmd_prompt cmd_1[] = {
  // {(struct cmd_prompt  *)(NULL),(char*)("ddd"), (char*)(), (int)(d)},
  PROMPT_NODE(NULL, NULL,"11", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,"12", "abc", NULL),
  PROMPT_NODE(NULL, NULL,"c2", "abc", NULL),
  PROMPT_NODE(NULL, do_up_level,"ul", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,"quit", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,NULL, NULL, NULL),
};

struct cmd_prompt cmd_2[] = {
  PROMPT_NODE(NULL, NULL,"++", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,"--", "abc", NULL),
  PROMPT_NODE(NULL, NULL,"c2", "abc", NULL),
  PROMPT_NODE(NULL, do_up_level,"ul", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,"quit", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,NULL, NULL, NULL),
};



struct cmd_prompt cmd_boot[] = {
  // PROMPT_NODE("'param'","<osw frame/slot/port>",NULL,NULL),
  PROMPT_NODE(cmd_1, do_c1_ex,"c111", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(cmd_2, do_c2_ex,"c222", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(cmd_2, SH_FUN_NULL,"c12", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,"quit", "<osw frame/slot/port>", NULL),
  PROMPT_NODE(NULL, NULL,NULL, NULL, NULL),
};


int do_c1(int argc, char **argv)
{
  // struct cmd_prompt *sh_downlevel(
  //   struct cmd_prompt *level)
  // printf("old  \n");
  sh_down_prompt_level(cmd_1);
}

int do_c2(int argc, char **argv)
{
  // printf("old  \n");
  sh_down_prompt_level(cmd_2);
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
  printf("new\n");
  sh_down_prompt_level(cmd_1);
}

int do_c2_ex(int argc, char **argv)
{
  // struct cmd_prompt *sh_downlevel(
  //   struct cmd_prompt *level)
  sh_up_prompt_level();
}

// const char str[] = "asbcdfasf";
void main()
{
  funtest();
  initialize_readline();
  struct sh_detach_depth depth;
  char *cmd[12];

  depth.cmd = cmd;
  depth.len = 12;
  depth.seps = " \t";
  sh_enter_ex(&depth);
}


typedef int rl_icpfunc_t (char *);
typedef struct {
  char *name;     /* User printable name of the function. */
  rl_icpfunc_t *func;   /* Function to call to do the job. */
  char *doc;      /* Documentation for this function.  */
} COMMAND;

com_cd (
    char *arg)
{

}
COMMAND commands[] = {
  { "cd", com_cd, "Change to directory DIR" },
  { "delete", com_cd, "Delete FILE" },
  { "help", com_cd, "Display this text" },
  { "?", com_cd, "Synonym for `help'" },
  { "list", com_cd, "List files in DIR" },
  { "ls", com_cd, "Synonym for `list'" },
  { "pwd", com_cd, "Print the current working directory" },
  { "quit", com_cd, "Quit using Fileman" },
  { "rename", com_cd, "Rename FILE to NEWNAME" },
  { "stat", com_cd, "Print out statistics on FILE" },
  { "view", com_cd, "View the contents of FILE" },
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};


char **
_sh_fileman_completion (
    const char *text,
    int start, int end);
static char *
_sh_command_generator (
    const char *text,
    int state);
extern const char *rl_readline_name ;
extern rl_attempted_completion_function;
initialize_readline ()
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
  if (start == 0)
    matches = rl_completion_matches (text, _sh_command_generator);

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
  while (name = _prompt_tree[_prompt_index][list_index].name) {
    list_index++;
    if (strncmp (name, text, len) == 0)
      return (_sh_dupstr(name));
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}

// #ifdef __cplusplus
// }
// #endif
