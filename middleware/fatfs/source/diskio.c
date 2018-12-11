/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */
#include <stddef.h>
#include "diskio.h"		/* Declarations of disk functions */
#include "ff_diskio.h"



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	FATFS_DISKIO *diskio;

	diskio = get_fatfs_diskio((uint32_t)pdrv);

	if (diskio == NULL) {
		stat = STA_NOINIT;
	} else {
		stat = (DSTATUS)(diskio->diskio_status());
	}

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	FATFS_DISKIO *diskio;

	diskio = get_fatfs_diskio((uint32_t)pdrv);

	if (diskio == NULL) {
		stat = STA_NOINIT;
	} else {
		stat = (DSTATUS)(diskio->diskio_initialize());
	}

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	FATFS_DISKIO *diskio;

	diskio = get_fatfs_diskio((uint32_t)pdrv);

	if (diskio == NULL) {
		res = RES_PARERR;
	} else {
		res = (DRESULT)(diskio->diskio_read(buff, sector, count));
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	FATFS_DISKIO *diskio;

	diskio = get_fatfs_diskio((uint32_t)pdrv);

	if (diskio == NULL) {
		res = RES_PARERR;
	} else {
		res = (DRESULT)(diskio->diskio_write(buff, sector, count));
	}

	return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	FATFS_DISKIO *diskio;

	diskio = get_fatfs_diskio((uint32_t)pdrv);

	if (diskio == NULL) {
		res = RES_PARERR;
	} else {
		res = (DRESULT)(diskio->diskio_ioctl(cmd, buff));
	}

	return res;


}

DWORD get_fattime (void)
{
	return (DWORD)diskio_get_fattime();
}

