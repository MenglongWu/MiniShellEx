_NAMEDIR2_		= name_dir2
DIR_$(_NAMEDIR2_) 	= src/
SRCS_$(_NAMEDIR2_) 	= _modsi3.S _udivsi3.S div0.c
SRCS += $(foreach TMP, \
	$(SRCS_$(_NAMEDIR2_)), \
	$(DIR_$(_NAMEDIR2_))$(TMP))
