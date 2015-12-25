/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 * yaffs_ramdisk.c: yaffs ram disk component
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

// This provides a YAFFS nand emulation on a file.
// THis is only intended as test code to test persistence etc.

const char *yaffs_flashif_c_version = "$Id: yaffs_fileem.c,v 1.1 2003/01/21 03:32:17 charles Exp $";


#include "ydirectenv.h"
#include "yaffs_flashif.h"
#include "yaffs_guts.h"
#include "devextras.h"

extern int nand_flash_hwr_init(void);
extern int nand_flash_erase_block(int block);
extern int nand_flash_program_buf(int chunkInNAND, const unsigned char *data, const unsigned char *spare);
extern int nand_flash_read_buf(int chunkInNAND, unsigned char *data,unsigned char *spare);




int yflash_WriteChunkToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, yaffs_Spare *spare)
{
	int err;
	unsigned char readdata [512];
	unsigned char *pdata;
	
	pdata = (unsigned char *)data;
	
	if (pdata==NULL)
	{
		nand_flash_read_buf(chunkInNAND, readdata, NULL);
		pdata = readdata;
	}

	err = nand_flash_program_buf(chunkInNAND,
								 (const unsigned char *)pdata,
								 (const unsigned char *)spare);
		
	if (err != 0)
	{
		return YAFFS_FAIL;
	}
	else
	{
		return YAFFS_OK;
	}

}


int yflash_ReadChunkFromNAND(yaffs_Device *dev, int chunkInNAND,  __u8 *data, yaffs_Spare *spare)
{
	int err;

	err = nand_flash_read_buf(chunkInNAND, (unsigned char*)data, (unsigned char *)spare);
		
	if (err != 0)
	{
		return YAFFS_FAIL;
	}
	else
	{
		return YAFFS_OK;
	}
	
//	printf("read from chunk: %d", chunkInNAND);
}


int yflash_EraseBlockInNAND(yaffs_Device *dev, int blockNumber)
{	
	int err;
	
	if(blockNumber < dev->startBlock || blockNumber >= dev->endBlock)
	{
		return YAFFS_FAIL;		
	}

	err = nand_flash_erase_block(blockNumber);
		
	if (err != 0)
	{
		return YAFFS_FAIL;
	}
	else
	{
		return YAFFS_OK;
	}

//	printf("erase block: %d", blockNumber);
}


int yflash_InitialiseNAND(yaffs_Device *dev)
{
	dev->useNANDECC = 1; // force on useNANDECC which gets faked. 
						 // This saves us doing ECC checks.
//	printf("yaffs initial...\n");
	return YAFFS_OK;
}


