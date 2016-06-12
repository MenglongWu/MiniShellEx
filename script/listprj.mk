#################################################################
# rule
# path must be a directory, exist path/project.mk,path/filelist.mk
#
# pixxx=script/default
# pixxx_arg=[all,mlib,elf...] one of us, [all, mlib,elf...] is top path Makefile operation
	pi2=script/minishellex_example
		pi2_arg=elf
	pi3=script/tool-build
		pi3_arg=elf
	pi4=script/test
		pi4_arg=elf

	pi1=script/libminishell
		pi1_arg=mlib

	# pi2=script/prj_liba
	# 	pi2_arg=mlib

	# pi3=script/prj_libb
	# 	pi3_arg=mlib

# default project item
export DP=pi1
