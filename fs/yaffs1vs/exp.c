/***********************************************************************************
Author:			Andy.Wu
Date:			2006.6.8
Description:	For test yaffs
************************************************************************************/

#include "yaffsfs.h"

void ListDi2r(const char *DirName)
{
	yaffs_DIR *d;
	yaffs_dirent *de;
	struct yaffs_stat s;
	char str[100];
			
	d = yaffs_opendir(DirName);
	
	if(!d)
	{
		printk("opendir failed\n");
	}
	else
	{
		printk("%s\n", DirName);
		while((de = yaffs_readdir(d)) != NULL)
		{
			sprintf(str,"%s/%s",DirName,de->d_name);
			yaffs_lstat(str,&s);
			printk("%20s        %8d     %5d    ", de->d_name, s.yst_size, s.yst_mode);
			
			switch(s.yst_mode & S_IFMT)
			{
				case S_IFREG: printk("data file"); break;
				case S_IFDIR: printk("directory"); break;
				case S_IFLNK: printk("symlink");   break;
				default: printk("unknown"); break;
			}
		
			printk("\n");		
		}
		
		yaffs_closedir(d);
	}
	
	printk("FreeSpace: %d\n\n", yaffs_freespace(DirName));
}

void ListDir(const char *mountpt)
{

	int i;
	yaffs_DIR *d;
	struct yaffs_dirent *de;
	struct yaffs_stat stat;
	char tempstr[255];
	int longlist = 1;
	d = yaffs_opendir(mountpt);

	if (!d) {
		//printk("opendir failed, %s\n", yaffs_error_str());
		return ;
	}

	for (i = 0; (de = yaffs_readdir(d)) != NULL; i++) {
		if (longlist) {
			//printf( "%s/%s\n", mountpt, de->d_name);
			yaffs_lstat(tempstr, &stat);
			printk("%-25s\t\n",
				de->d_name);
		} else {
			printk("%s\n", de->d_name);
		}
	}

	yaffs_closedir(d);
	printk("FreeSpace: %d\n\n", yaffs_freespace(mountpt));
	return ;
}


void main2(void)
{
	int i = 0;
	int f = 0;
	char buf[100];
	struct yaffs_stat s;
	
	yaffs_StartUp();
	// yaffs_mount("/boot");
	
	// yaffs_mount("/c");
	// yaffs_mkdir("/boot/mydoc", 0);
	// yaffs_mkdir("/c/mydoc", 0);
	// yaffs_mkdir("/d/data0", 0);
	// yaffs_mkdir("/d/data1", 0);
	
	// f = yaffs_open("/d/file1.gsk", O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
	// yaffs_close(f);
	
	// f = yaffs_open("/d/data0/file2.gsk", O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
	// yaffs_close(f);

	// yaffs_unlink("/d/data0/file1");
	// ListDir("/boot");
	// ListDir("/boot");
}
