#################################################################
# you can edit it for all target
# create all project target with sequence

.PHONY:allp
allp:pi2 pi3 pi4 pi1

pi2:pi1
	$(MAKE) DP=pi2 --no-print-directory
pi4:pi3 auto
	$(MAKE) DP=pi4 --no-print-directory

pi3:pi1
	$(MAKE) DP=pi3 --no-print-directory
auto:
	./release-$(ARCH)/msbuild example/xml/switch.xml example/cmd_output.c

pi1:
	$(MAKE) DP=pi1 --no-print-directory
# $(MAKE) DP=pi4 --no-print-directory
# $(MAKE) allstrip
	

# clean all project output
.PHONY:allclean
allclean:
	$(MAKE) DP=pi2 clean --no-print-directory

	$(MAKE) DP=pi3 clean --no-print-directory
	$(MAKE) DP=pi1 clean --no-print-directory
# $(MAKE) DP=pi4 clean --no-print-directory
	
# remote all output file and empty directory which create by Makefile
.PHONY:alldisclean
alldistclean:
	$(MAKE) DP=pi2 disclean --no-print-directory

	$(MAKE) DP=pi3 disclean --no-print-directory
	$(MAKE) DP=pi1 disclean --no-print-directory
	$(MAKE) DP=pi4 disclean --no-print-directory
	
# strip all output file STRTAB section
.PHONY:allstrip
allstrip:
	$(MAKE) DP=pi2 strip --no-print-directory
	$(MAKE) DP=pi3 strip --no-print-directory
	$(MAKE) DP=pi1 strip --no-print-directory
# $(MAKE) DP=pi4 strip --no-print-directory
