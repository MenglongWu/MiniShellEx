/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system.
 * yaffscfg.c  The configuration for the "direct" use of yaffs.
 *
 * This file is intended to be modified to your requirements.
 * There is no need to redistribute this file.
 */

#include "yaffscfg.h"
#include "yaffsfs.h"
#include "mem.h"

// yaffs_traceMask controls printed <INFO>, see more in ydirected.h.
unsigned yaffs_traceMask = 0x11;

void yaffsfs_SetError(int err)
{
	//Do whatever to set error
	//errno = err;
}



void yaffsfs_Lock(void)
{
}

void yaffsfs_Unlock(void)
{
}

__u32 yaffsfs_CurrentTime(void)
{
	return 0;
}

void yaffsfs_LocalInitialisation(void)
{
	// Define locking semaphore.
}

// Configuration for:
// /ram  2MB ramdisk
// /boot 2MB boot disk (flash)
// /flash 14MB flash disk (flash)
// NB Though /boot and /flash occupy the same physical device they
// are still disticnt "yaffs_Devices. You may think of these as "partitions"
// using non-overlapping areas in the same device.
// 

#include "yaffs_flashif.h"

extern int nand_flash_hwr_init(void);

static yaffs_Device bootDev;
static yaffs_Device flashDev0;
static yaffs_Device flashDev1;

static yaffsfs_DeviceConfiguration yaffsfs_config[] = {

	{ "/boot", &bootDev},
	 // { "/c", &flashDev0},
	 // { "/d", &flashDev1},
	{(void *)0,(void *)0},
	{0,0}
};


int yaffs_StartUp(void)
{	
	// /boot
	bootDev.nBytesPerChunk 		= YAFFS_BYTES_PER_CHUNK;
	bootDev.nChunksPerBlock 	= YAFFS_CHUNKS_PER_BLOCK;
	bootDev.nReservedBlocks 	= 5;
	bootDev.startBlock 			= 4;//4; // Can now use block zero
	bootDev.endBlock 			= 512;//127;//127; // Last block in 2MB.	
	bootDev.useNANDECC 			= 0; // use YAFFS's ECC
	bootDev.nShortOpCaches 		= 10; // Use caches
	bootDev.genericDevice 		= (void *) 1;	// Used to identify the device in fstat.
	bootDev.writeChunkToNAND 	= yflash_WriteChunkToNAND;
	bootDev.readChunkFromNAND 	= yflash_ReadChunkFromNAND;
	bootDev.eraseBlockInNAND 	= yflash_EraseBlockInNAND;
	bootDev.initialiseNAND 		= yflash_InitialiseNAND;

	// // "/c"
	// flashDev0.nBytesPerChunk 	= YAFFS_BYTES_PER_CHUNK;
	// flashDev0.nChunksPerBlock 	= YAFFS_CHUNKS_PER_BLOCK;
	// flashDev0.nReservedBlocks 	= 5;
	// flashDev0.startBlock 		= 128; // First block after 2MB
	// flashDev0.endBlock 			= 256; // next block in 32MB
	// flashDev0.useNANDECC 		= 0; // use YAFFS's ECC
	// flashDev0.nShortOpCaches 	= 10; // Use caches
	// flashDev0.genericDevice 	= (void *) 2;	// Used to identify the device in fstat.
	// flashDev0.writeChunkToNAND 	= yflash_WriteChunkToNAND;
	// flashDev0.readChunkFromNAND = yflash_ReadChunkFromNAND;
	// flashDev0.eraseBlockInNAND 	= yflash_EraseBlockInNAND;
	// flashDev0.initialiseNAND 	= yflash_InitialiseNAND;
	
	// // "/d"
	// flashDev1.nBytesPerChunk 	= YAFFS_BYTES_PER_CHUNK;
	// flashDev1.nChunksPerBlock 	= YAFFS_CHUNKS_PER_BLOCK;
	// flashDev1.nReservedBlocks 	= 5;
	// flashDev1.startBlock 		= 257; // First block after 32MB
	// flashDev1.endBlock 			= 512; // Last block in 64MB
	// flashDev1.useNANDECC 		= 0; // use YAFFS's ECC
	// flashDev1.nShortOpCaches 	= 10; // Use caches
	// flashDev1.genericDevice 	= (void *) 3;	// Used to identify the device in fstat.
	// flashDev1.writeChunkToNAND 	= yflash_WriteChunkToNAND;
	// flashDev1.readChunkFromNAND	= yflash_ReadChunkFromNAND;
	// flashDev1.eraseBlockInNAND 	= yflash_EraseBlockInNAND;
	// flashDev1.initialiseNAND 	= yflash_InitialiseNAND;

	yaffs_initialise(yaffsfs_config);

	nand_flash_hwr_init();
	mem_init();

	return 0;
}




