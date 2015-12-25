/***********************************************************************************
Author:			Andy.Wu
Date:			2006.6.8
Description:	For test yaffs
************************************************************************************/

#include "yaffsfs.h"

// void ListDir22(const char *DirName)
// {
	// yaffs_DIR *d;
	// yaffs_dirent *de;
	// struct yaffs_stat s;
	// char str[100];
			
	// d = yaffs_opendir(DirName);
	
	// if(!d)
	// {
	// 	printk("opendir failed\n");
	// }
	// else
	// {
	// 	printk("%s\n", DirName);
	// 	while((de = yaffs_readdir(d)) != NULL)
	// 	{
	// 		sprintf(str,"%s/%s",DirName,de->d_name);
	// 		yaffs_lstat(str,&s);
	// 		printk("%20s        %8d     %5d    ", de->d_name, s.yyst_size, s.yyst_mode);
			
	// 		switch(s.yyst_mode & S_IFMT)
	// 		{
	// 			case S_IFREG: printk("data file"); break;
	// 			case S_IFDIR: printk("directory"); break;
	// 			case S_IFLNK: printk("symlink");   break;
	// 			default: printk("unknown"); break;
	// 		}
		
	// 		printk("\n");		
	// 	}
		
	// 	yaffs_closedir(d);
	// }
	
	// printk("FreeSpace: %d\n\n", yaffs_freespace(DirName));
// }



