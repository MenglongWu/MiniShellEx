#include "yaffsfs.h"

// static const char *yaffs_error_str(void)
// {
// 	int error = yaffsfs_GetLastError();

// 	if (error < 0)
// 		error = -error;

// 	switch (error) {
// 	case EBUSY: return "Busy";
// 	case ENODEV: return "No such device";
// 	case EINVAL: return "Invalid parameter";
// 	// case ENFILE: return "Too many open files";
// 	case EBADF:  return "Bad handle";
// 	// case EACCES: return "Wrong permissions";
// 	case EXDEV:  return "Not on same device";
// 	case ENOENT: return "No such entry";
// 	case ENOSPC: return "Device full";
// 	// case EROFS:  return "Read only file system";
// 	// case ERANGE: return "Range error";
// 	case ENOTEMPTY: return "Not empty";
// 	// case ENAMETOOLONG: return "Name too long";
// 	case ENOMEM: return "Out of memory";
// 	// case EFAULT: return "Fault";
// 	case EEXIST: return "Name exists";
// 	case ENOTDIR: return "Not a directory";
// 	case EISDIR: return "Not permitted on a directory";
// 	// case ELOOP:  return "Symlink loop";
// 	case 0: return "No error";
// 	default: return "Unknown error";
// 	}
// }

// int cmd_yaffs_dev_ls(void)
// {
// 	struct yaffs_dev *dev;
// 	int flash_dev;
// 	int free_space;

// 	yaffs_dev_rewind();

// 	while (1) {
// 		dev = yaffs_next_dev();
// 		if (!dev)
// 			break;
// 		flash_dev =
// 			((unsigned) dev->driver_context - (unsigned) nand_info)/
// 				sizeof(nand_info[0]);
// 		printk("%-10s %5d 0x%05x 0x%05x %s",
// 			dev->param.name, flash_dev,
// 			dev->param.start_block, dev->param.end_block,
// 			dev->param.inband_tags ? "using inband tags, " : "");

// 		free_space = yaffs_freespace(dev->param.name);
// 		if (free_space < 0)
// 			printk("not mounted\n");
// 		else
// 			printk("free 0x%x\n", free_space);

// 	}

// 	return 0;
// }

static char *yaffs_file_type_str(struct yaffs_stat *stat)
{
	switch (stat->yst_mode & S_IFMT) {
	case S_IFREG: return "regular file";
	case S_IFDIR: return "directory";
	case S_IFLNK: return "symlink";
	default: return "unknown";
	}
}







void cmd_yaffs_ls(const char *mountpt,int longlist)
{
	int i;
	yaffs_DIR *d;
	struct yaffs_dirent *de;
	struct yaffs_stat stat;
	char tempstr[255];

	d = yaffs_opendir(mountpt);

	if (!d) {
	// 	printk("opendir failed, %s\n", yaffs_error_str());
		return ;
	}

	for (i = 0; (de = yaffs_readdir(d)) != NULL; i++) {
		if (longlist) {
			sprintf(tempstr, "%s/%s", mountpt, de->d_name);
			yaffs_lstat(tempstr, &stat);
			printk("%-25s\t%7ld",
					de->d_name,
					(long)stat.yst_size);
			printk(" %5d %s\n",
					stat.yst_ino,
					yaffs_file_type_str(&stat));
			
		} else {
			printk("%s\t", de->d_name);
		}
	}
	puts("\n");

	yaffs_closedir(d);
	printk("FreeSpace: %d\n\n", yaffs_freespace(mountpt));
}

int cmd_yaffs_check(const char *fname, const char *type)
{
	int retval = 0;
	int ret;
	struct yaffs_stat stat;

	ret = yaffs_stat(fname, &stat);
	if (ret < 0) {
		printk("%s not found\n", fname);
		return -1;
	}

	printk("%s is a %s\n", fname, yaffs_file_type_str(&stat));

	if (strcmp(type, "REG") == 0 &&
	    (stat.yst_mode & S_IFMT) != S_IFREG)
		retval = -1;

	if (strcmp(type, "DIR") == 0 &&
	    (stat.yst_mode & S_IFMT) != S_IFDIR)
		retval = -1;

	if (retval == 0)
		printk("check ok\n");
	else
		printk("check failed\n");

	return retval;
}


int cmd_yaffs_mkdir(const char *dir)
{
	int retval = yaffs_mkdir(dir, 0);

	if (retval < 0) {
		// printk("yaffs_mkdir returning error: %d, %s\n",
		// 	retval, yaffs_error_str());
		return -1;
	}
	return 0;
}


int cmd_yaffs_rmdir(const char *dir)
{
	int retval = yaffs_rmdir(dir);

	if (retval < 0) {
		// printk("yaffs_rmdir returning error: %d, %s\n",
		// 	retval, yaffs_error_str());
		return -1;
	}
	return 0;
}




int cmd_yaffs_rm(const char *path)
{
	int retval = yaffs_unlink(path);

	if (retval < 0) {
		printk("yaffs_unlink returning error: %d\n",
		 	retval);//, yaffs_error_str());
		return -1;
	}

	return 0;
}


int cmd_yaffs_mv(const char *oldPath, const char *newPath)
{
	int retval = yaffs_rename(newPath, oldPath);

	if (retval < 0) {
		// printk("yaffs_unlink returning error: %d, %s\n",
		// 	retval, yaffs_error_str());
		return -1;
	}

	return 0;
}





int read_a_file(char *fn)
{
	int h;
	int i = 0;
	unsigned char b[2048];

	for(int i = 0;i < 3000;i++) {
		h = yaffs_open(fn, O_RDWR, 0);
		if (h < 0) {
			printk("File not found\n");
			return -1;
		}

		
			while (yaffs_read(h, &b, 1024) > 0) {
				// printk("%02x ", b);
				// i++;
				// if (i > 32) {
				// 	printk("\n");
				// 	i = 0;;
				// }
			}
		
		// printk("\n");
		yaffs_close(h);
	}
	return 0;
}

int cmd_yaffs_read_file(char *fn)
{
	return read_a_file(fn);
}


#ifdef _DEBUG_
int make_a_file_dbg(char *yaffsName, char bval, int sizeOfFile)
{
	int outh;
	int i;
	int written;
	unsigned char buffer[100];

	outh = yaffs_open(yaffsName,
				O_CREAT | O_RDWR | O_TRUNC,
				S_IREAD | S_IWRITE);
	if (outh < 0) {
		printk("Error opening file: %d.\n", outh);//, yaffs_error_str());
		return -1;
	}

	memset(buffer, bval, 100);

	written = 0;
	while (written < sizeOfFile) {
		i = sizeOfFile - written;
		if (i > 100)
			i = 100;

		if (yaffs_write(outh, buffer, i) != i)
			break;
		written += i;
	}

	yaffs_close(outh);

	return (written == sizeOfFile) ? 0 : -1;
}

int cmd_yaffs_write_file_dbg(char *yaffsName, char bval, int sizeOfFile)
{
	return make_a_file_dbg(yaffsName, bval, sizeOfFile);
}
#endif

int make_a_file(char *yaffsName, unsigned int addr, int sizeOfFile)
{
	int outh;
	// int i;
	int written;
	// unsigned char buffer[100];

	outh = yaffs_open(yaffsName,
				O_CREAT | O_RDWR | O_TRUNC,
				S_IREAD | S_IWRITE);
	printk("oldfile %x newfile %x\n",outh,outh);
	if (outh < 0) {
		printk("Error opening file: %d.\n", outh);//, yaffs_error_str());
		return -1;
	}

	written = yaffs_write(outh, addr, sizeOfFile);



	yaffs_close(outh);

	return (written == sizeOfFile) ? 0 : -1;
}

int cmd_yaffs_write_file(char *yaffsName, unsigned int addr, int sizeOfFile)
{
	return make_a_file(yaffsName, addr, sizeOfFile);
}

int cmd_yaffs_cp(char *oldPath,char *newPath)
{
	int oldFile,newFile;
	unsigned int sizeFile;
	unsigned char buffer[512];
	unsigned int rlen = 1,wlen;

	oldFile = yaffs_open(oldPath,O_RDWR,0);
	newFile = yaffs_open(newPath,
				O_CREAT | O_RDWR | O_TRUNC,
				S_IREAD | S_IWRITE);
	printk("oldfile %x newfile %x\n",oldFile,newFile);
	if(oldFile >= 0 && newFile >= 0) {
		do {
			rlen = yaffs_read(oldFile,buffer,512);
			if(rlen) {
				wlen = yaffs_write(newFile, buffer, rlen);	
			}
		}while(rlen && wlen);
	}



	if(oldFile >= 0) {
		yaffs_close(oldFile);
	}

	if(newFile >= 0){
		yaffs_close(newFile);
	}

	return 0;
	
}