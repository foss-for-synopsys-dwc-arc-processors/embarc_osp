/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

#include <stddef.h>
#include "diskio.h"	/* FatFs lower layer API */
#include "ff_diskio.h"

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv	/* Physical drive nmuber (0..) */
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
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv	/* Physical drive nmuber (0..) */
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
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,	/* Physical drive nmuber (0..) */
	BYTE *buff,	/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count	/* Number of sectors to read (1..128) */
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

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count		/* Number of sectors to write (1..128) */
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

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,	/* Physical drive nmuber (0..) */
	BYTE cmd,	/* Control code */
	void *buff	/* Buffer to send/receive control data */
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
#endif


DWORD get_fattime (void)
{
	return (DWORD)diskio_get_fattime();
}