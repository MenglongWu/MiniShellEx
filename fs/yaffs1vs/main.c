/**
 ******************************************************************************
 * @brief	
 *		VS2008仿真Yaffs1，Yaffs版本1.2.1，用RAM仿真存储器
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		MenglongWu		2014-12-5	1.简单的列出接口，因为这个Yaffs版本
			太老，恐怕存在Bug，测试时候分区有/boot和/c，执行命令创建/boot目录
			mkdir /boot会报错，然后整个文件系统崩溃，理应提示/boot目录已经存在，
			但他没有提示，如果已经存在/boot/dir1目录，mkdir /boot/dir1没有任何问题，
			跟踪代码提示目录已经存在。
 ******************************************************************************
*/



#include "stdio.h"
#include "yaffsfs.h"
#include "yaffs_flashif.h"
int yaffs_StartUp(void);
void cmd_yaffs_ls(const char *DirName,int ListDir);


extern unsigned char ramfs[RAM_SIZE];


//创建虚拟硬盘
void CreateDisk()
{
	printf("创建Yaffs1.Img\n");
	unsigned char *pdata = (unsigned char *)ramfs;
	unsigned int len = sizeof(ramfs);//(512+512)*1024*4;//4MB
	while(len) {
		*pdata = 0xff;
		pdata++;
		len--;
	}
	FILE *fp;
	fp = fopen("yaffs1.img","wb");
	fwrite(ramfs,1,sizeof(ramfs),fp);
	fclose(fp);
}


//打开虚拟硬盘
void OpenDisk()
{
	FILE *fp;
	fp = fopen("yaffs1.img","rb");
	if(fp) {
		fread(ramfs,1,sizeof(ramfs),fp);
		fclose(fp);
	}
	else {
		printf("不存在yaffs1.img\n");
		CreateDisk();
	}
}

//保存虚拟硬盘
void SaveDisk()
{
	FILE *fp;
	fp = fopen("yaffs1.img","r+");
	if(fp) {
		fwrite(ramfs,1,sizeof(ramfs),fp);
		
		fclose(fp);
	}
	
}


void mkdir()
{
	char strout[256];
	printf("输入目录路径");
	scanf("%256s",strout);
	yaffs_mkdir(strout,0);
}

void rmdir()
{
	char strout[256];
	printf("输入目录路径");
	scanf("%256s",strout);
	yaffs_rmdir(strout);
}

void ls()
{
	char strout[256];
	printf("输入目录路径");
	scanf("%256s",strout);
	ListDir(strout,1);
}

void fnew()
{
	char strout[256];
	printf("输入文件路径");
	scanf("%256s",strout);
	int f;
	f = yaffs_open(strout, O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
	yaffs_close(f);
}

void fdel()
{
	char strout[256];
	printf("输入文件路径");
	scanf("%256s",strout);
	yaffs_unlink(strout);
}
void help()
{
	printf("做出你的选择\n");
	printf("\thelp命令列表\n");
	printf("\tmkdir创建目录\n");
	printf("\trmdir删除目录\n");
	printf("\tls列出目录\n");
	printf("\tfnew创建一个文件\n");
	printf("\tfdel删除一个文件\n");

}


void ListChunk()
{
	unsigned int i;
	int chunk,block,page,sector;
	printf("RAM_CHUNK_PER_PAGE %d\n",RAM_CHUNK_PER_PAGE);
	printf("RAM_PAGE_PER_BLOCK %d\n",RAM_PAGE_PER_BLOCK);
	
	chunk = block = page = sector = 0;
	for(i = 0;i < 240;++i) {
		chunk = i;
		block = chunk / (RAM_PAGE_PER_BLOCK*RAM_CHUNK_PER_PAGE);
		page = (chunk % (RAM_PAGE_PER_BLOCK*RAM_CHUNK_PER_PAGE)) / RAM_CHUNK_PER_PAGE;
		sector = chunk % (RAM_CHUNK_PER_PAGE);
		//off = 
		printf("chunk %d block %d page %d off %d\n",chunk,block,page,sector);
	}


	printf("block size %d page size %d sector size %d\n",
		RAM_BLOCK_ADDR,
		RAM_PAGE_ADDR,
		RAM_SECTOR_ADDR);
}
int main()
{
	char strout[256];
	//ListChunk();
	//return 0;
	yaffs_StartUp();

	OpenDisk();
	yaffs_mount("/boot");
	//yaffs_mount("/c");

	printf("打印2个分区根目录\n");
	cmd_yaffs_ls("/boot",1);
	cmd_yaffs_ls("/c",1);
	yaffs_rmdir("/boot/ald/fs");
	
	help();
	while(1) {
		printf(">");
		scanf("%256s",strout);
		if(strcmp(strout,"help") == 0) {
			help();
		}
		if(strcmp(strout,"mkdir") == 0) {
			mkdir();
			SaveDisk();
		}
		else if(strcmp(strout,"rmdir") == 0) {
			rmdir();
			SaveDisk();
		}
		else if(strcmp(strout,"ls") == 0) {
			ls();
			SaveDisk();
		}
		else if(strcmp(strout,"fnew") == 0) {
			fnew();
			SaveDisk();
		}
		else if(strcmp(strout,"fdel") == 0) {
			fdel();
			SaveDisk();
		}
		
	}

   	//printf("mkdir %d\n",yaffs_mkdir("/c/sk",0));
//   	printf("mkdir %d\n",yaffs_mkdir("/boot/sk2",0));
//  	printf("mkdir %d\n",yaffs_mkdir("/boot/sk3",0));
// 	yaffs_rmdir("/boot/sk2");
	
// 	yaffs_mkdir("/boot/sk4",0);
	ListDir("/boot",1);
	ListDir("/c",1);
	
	//ListDir("/c");

}