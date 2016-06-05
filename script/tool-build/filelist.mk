# SRCS += shell/minishell_core.c 

# SRCS-y = src/main.c
# SRCS-(CONFIG_MINISHELL_CORE) += src/shell.c
# SRCS-(CONFIG_MINISHELL_CORE) += shell/minishell_core.c
# SRCS-(CONFIG_MINISHELL_EX) += shell/minishell_core_ex.c
# SRCS-y += shell/minishell_core_ex.c

SRCS-y += tool/build.c
SRCS-y += tool/rwxml.c
SRCS-y += tool/build-structure.c
SRCS-y += tool/wcfile.c
SRCS-y += shell/minishell_core.c
SRCS-y += shell/minishell_core_ex.c
