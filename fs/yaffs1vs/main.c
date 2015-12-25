/**
 ******************************************************************************
 * @brief	
 *		VS2008����Yaffs1��Yaffs�汾1.2.1����RAM����洢��
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		MenglongWu		2014-12-5	1.�򵥵��г��ӿڣ���Ϊ���Yaffs�汾
			̫�ϣ����´���Bug������ʱ�������/boot��/c��ִ�������/bootĿ¼
			mkdir /boot�ᱨ��Ȼ�������ļ�ϵͳ��������Ӧ��ʾ/bootĿ¼�Ѿ����ڣ�
			����û����ʾ������Ѿ�����/boot/dir1Ŀ¼��mkdir /boot/dir1û���κ����⣬
			���ٴ�����ʾĿ¼�Ѿ����ڡ�
 ******************************************************************************
*/



#include "stdio.h"
#include "yaffsfs.h"
#include "yaffs_flashif.h"
int yaffs_StartUp(void);
void cmd_yaffs_ls(const char *DirName,int ListDir);


extern unsigned char ramfs[RAM_SIZE];


//��������Ӳ��
void CreateDisk()
{
	printf("����Yaffs1.Img\n");
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


//������Ӳ��
void OpenDisk()
{
	FILE *fp;
	fp = fopen("yaffs1.img","rb");
	if(fp) {
		fread(ramfs,1,sizeof(ramfs),fp);
		fclose(fp);
	}
	else {
		printf("������yaffs1.img\n");
		CreateDisk();
	}
}

//��������Ӳ��
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
	printf("����Ŀ¼·��");
	scanf("%256s",strout);
	yaffs_mkdir(strout,0);
}

void rmdir()
{
	char strout[256];
	printf("����Ŀ¼·��");
	scanf("%256s",strout);
	yaffs_rmdir(strout);
}

void ls()
{
	char strout[256];
	printf("����Ŀ¼·��");
	scanf("%256s",strout);
	ListDir(strout,1);
}

void fnew()
{
	char strout[256];
	printf("�����ļ�·��");
	scanf("%256s",strout);
	int f;
	f = yaffs_open(strout, O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
	yaffs_close(f);
}

void fdel()
{
	char strout[256];
	printf("�����ļ�·��");
	scanf("%256s",strout);
	yaffs_unlink(strout);
}
void help()
{
	printf("�������ѡ��\n");
	printf("\thelp�����б�\n");
	printf("\tmkdir����Ŀ¼\n");
	printf("\trmdirɾ��Ŀ¼\n");
	printf("\tls�г�Ŀ¼\n");
	printf("\tfnew����һ���ļ�\n");
	printf("\tfdelɾ��һ���ļ�\n");

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

	printf("��ӡ2��������Ŀ¼\n");
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