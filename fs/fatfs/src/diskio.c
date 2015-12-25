/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
// #include "usbdisk.h"	/* Example: USB drive control */
// #include "atadrive.h"	/* Example: ATA drive control */
// #include "sdcard.h"		/* Example: MMC/SDC contorl */

#include "board_config.h"
#include "board.h"
#include "nand/nand_bbt.h"


/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA drive to drive number 0 */
#define MMC		1	/* Example: Map MMC/SD card to drive number 1 */
#define USB		2	/* Example: Map USB drive to drive number 2 */
#define NAND	3

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
// return STA_NOINIT,STA_NODISK,STA_PROTECT
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	printk("\ndisk_ioctl()\n");
	DSTATUS stat;
	int result;

	// result = FR_OK;
	pdrv = NAND;
	switch (pdrv) {
	case NAND:
		result = 0;
		return stat;
	case ATA :
		// result = ATA_disk_status();

		// translate the reslut code here

		return stat;

	case MMC :
		// result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case USB :
		// result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	printk("\ndisk_initialize()\n");
	DSTATUS stat;
	int result;
	// result = FR_OK;
	pdrv = NAND;
goto _NandFlash;
	switch (pdrv) {
_NandFlash:;
	case NAND:
		result = nbbt_init();
		if(result == 0) {
			stat = 0;
		}
		else {
			stat = STA_NOINIT;
		}

		return stat;
	case ATA :
		// result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;

	case MMC :
		// result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case USB :
		// result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	printk("\ndisk_read()\n");
	DRESULT res;
	int result;
	pdrv = NAND;
	// result = FR_OK;
	switch (pdrv) {
	case NAND:
		result = nbbt_readSector(sector,buff,count);
		res = result;
		return res;
	case ATA :
		// translate the arguments here

		// result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		// result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		// result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	printk("\ndisk_write() pdrv %d sector %d count %d\n",
		pdrv,sector,count);
	DRESULT res;
	int result;
	pdrv = NAND;
	// result = FR_OK;
	switch (pdrv) {
	case NAND:
		result = nbbt_writeSector(sector,buff,count);
		res = result;
		return res;
	case ATA :
		// translate the arguments here

		// result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		// result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		// result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	printk("\ndisk_ioctl() %d %d %d %d\n",pdrv,cmd,*((DWORD*)buff) ,*((DWORD*)buff+1) );
	DRESULT res;
	int result;
	// result = FR_OK;
	
goto _NandFlash;
	switch (pdrv) {
_NandFlash:;
	case NAND:

		switch(cmd) {
		case CTRL_SYNC:	
			
			break;
		case GET_SECTOR_COUNT:	
			*(DWORD*)buff = NAND_SECTOR_COUNT;
			break;
		case GET_SECTOR_SIZE:	
			*(WORD*)buff = NAND_SECTOR_SIZE;
			break;
		case GET_BLOCK_SIZE:	
			*(DWORD*)buff = NAND_BLOCK_SIZE;
			break;
		case CTRL_ERASE_SECTOR:	
			*((DWORD*)buff) = 256;
			*((DWORD*)buff+1) = 1279;
			break;
		}
		res = RES_OK;
		// result = 0;
		return res;
	case ATA :

		// Process of the command for the ATA drive

		return res;

	case MMC :

		// Process of the command for the MMC/SD card

		return res;

	case USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}
#endif


DWORD get_fattime (void)
{
	return 0;
}