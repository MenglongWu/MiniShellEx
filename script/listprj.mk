#################################################################
# rule
# path must be a directory, exist path/project.mk,path/filelist.mk
#
# pixxx=script/default
# pixxx_arg=[all,mlib,elf...] one of us, [all, mlib,elf...] is top path Makefile operation
	pi1=script/minishellex_example
		pi1_arg=elf
	pi4=script/tool-build
		pi4_arg=elf
	pi5=script/test
		pi5_arg=elf

	pi6=script/libminishell
		pi6_arg=mlib

	pi2=script/prj_liba
		pi2_arg=mlib

	pi3=script/prj_libb
		pi3_arg=mlib

# default project item
export DP=pi1
