/*
 *  Copyright (C) 2014, ChaN, all right reserved.
 *
 * + The FatFs module is a free software and there is NO WARRANTY.
 * + No restriction on use. You can use, modify and redistribute it for
 *   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
 * + Redistributions of source code must retain the above copyright notice.
 **/
/**
 * Copyright (C) 2015 Synopsys, Inc. All rights reserved.
 * Synopsys modifications to the FatFs module are license for use under the same
 * terms as FatFs module. Synopsys modifications include port for ARC processors
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

#ifndef _FF_SDCARD_SPI_H_
#define _FF_SDCARD_SPI_H_

#include "stdint.h"

#include "ff_diskio.h"

extern int32_t sdcard_spi_diskio_initialize(FATFS_DISKIO_PTR diskio);
extern int32_t sdcard_spi_diskio_status(FATFS_DISKIO_PTR diskio);
extern int32_t sdcard_spi_diskio_read(FATFS_DISKIO_PTR diskio, void *buf, uint32_t sector, uint32_t count);
extern int32_t sdcard_spi_diskio_write(FATFS_DISKIO_PTR diskio, const void *buf, uint32_t sector, uint32_t count);
extern int32_t sdcard_spi_diskio_ioctl(FATFS_DISKIO_PTR diskio, uint32_t cmd, void *buf);
extern void sdcard_spi_diskio_timerproc(FATFS_DISKIO_PTR diskio);

#endif /** _FF_SDCARD_SPI_H_ */
