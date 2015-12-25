/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 * yaffs_ramdisk.h: yaffs ram disk component
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * $Id: yaffs_flashif.h,v 1.1 2003/01/21 03:32:17 charles Exp $
 */

// This provides a rma disk under yaffs.
// NB this is not intended for NAND emulation.
// Use this with dev->useNANDECC enabled, then ECC overheads are not required.

#ifndef __YAFFS_FLASH_H__
#define __YAFFS_FLASH_H__


#include "yaffs_guts.h"
int yflash_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int yflash_WriteChunkToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, yaffs_Spare *spare);
int yflash_ReadChunkFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare);
int yflash_InitialiseNAND(yaffs_Device *dev);




/*
创建一个
Page大小2K+64Byte
每个Block有8个Page
虚拟设备有128个Block

Page（2K+64Byte）划分成512Byte每chunk，16Byte每space
虚拟设备总大小是RAM_SIZE
*/

#define RAM_BYTE_PER_SPACE 16
#define RAM_BYTE_PER_CHUNK 512
#define RAM_CHUNK_PER_PAGE 4
#define RAM_PAGE_PER_BLOCK 8
#define RAM_BLOCK_COUNT    256


#define RAM_SIZE ((RAM_BYTE_PER_SPACE + RAM_BYTE_PER_CHUNK) * \
					RAM_CHUNK_PER_PAGE* \
					RAM_PAGE_PER_BLOCK*\
					RAM_BLOCK_COUNT)


#define RAM_SECTOR_ADDR (RAM_BYTE_PER_CHUNK)
#define RAM_PAGE_ADDR	(RAM_CHUNK_PER_PAGE * \
	(RAM_SECTOR_ADDR + RAM_BYTE_PER_SPACE))

#define RAM_BLOCK_ADDR (RAM_PAGE_PER_BLOCK*RAM_PAGE_ADDR)

#endif
