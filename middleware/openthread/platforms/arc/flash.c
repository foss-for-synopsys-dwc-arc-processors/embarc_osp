/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for flash.
 *
 */

#include <openthread/config.h>
#include "openthread/platform/alarm-milli.h"
#include <utils/flash.h>
#include <utils/code_utils.h>
#include "platform-arc.h"

#ifdef BOARD_EMSK
#include "spi_flash_w25qxx.h"


/**
 * EMSK HAS 128 Mbit (16 MB) SPI flash memory (Winbond W25Q128BV).
 * The Winbond W25Q128BV array is organized into 65536 programmable pages of
 * 256-byte each. Up to 256 bytes can be programmed at a time. Pages can be
 * erased in groups of 16 (4KB sector erase).
 */

/**
 * EMSK SPIflash memory map with using embARC
 * FPGA image: from 0x0000 0000
 * Secondary bootloader: from 0x0078 0000
 * Available for OpenThread: from 0x00FF D000 to 0x00FF EFFF
 */
#define OPENTHREAD_FLASH_BASE       0x00ffd000
#define OPENTHREAD_FLASH_SIZE       0x00002000

/**
 * Define SETTINGS_CONFIG_BASE_ADDRESS,
 *    SETTINGS_CONFIG_PAGE_SIZE, and
 *    SETTINGS_CONFIG_PAGE_NUM for EMSK
 */
#ifdef SETTINGS_CONFIG_BASE_ADDRESS
#undef SETTINGS_CONFIG_BASE_ADDRESS
#endif // SETTINGS_CONFIG_BASE_ADDRESS
#define SETTINGS_CONFIG_BASE_ADDRESS    0x00ffe000

#ifdef SETTINGS_CONFIG_PAGE_SIZE
#undef SETTINGS_CONFIG_PAGE_SIZE
#endif // SETTINGS_CONFIG_PAGE_SIZE
#define SETTINGS_CONFIG_PAGE_SIZE   FLASH_SECTOR_SIZE

#ifdef SETTINGS_CONFIG_PAGE_NUM
#undef SETTINGS_CONFIG_PAGE_NUM
#endif // SETTINGS_CONFIG_PAGE_NUM
#define SETTINGS_CONFIG_PAGE_NUM    1

W25QXX_DEF(w25q128bv, DW_SPI_0_ID, EMSK_SPI_LINE_SFLASH, FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE);

otError utilsFlashInit(void)
{
    w25qxx_init(w25q128bv, 1000000);
    return OT_ERROR_NONE;
}

uint32_t utilsFlashGetSize(void)
{
    return (uint32_t)(w25q128bv->sector_sz);
}

otError utilsFlashErasePage(uint32_t aAddress)
{
    otError error = OT_ERROR_NONE;
    int32_t status;

    otEXPECT_ACTION((aAddress >= OPENTHREAD_FLASH_BASE) && (aAddress < (OPENTHREAD_FLASH_BASE + OPENTHREAD_FLASH_SIZE - 1)),
                    error = OT_ERROR_INVALID_ARGS);

    /* Use 2 sectors in the implementation, cannot erase the address over the boundry */
    status = w25qxx_erase(w25q128bv, aAddress, FLASH_SECTOR_SIZE);

    otEXPECT_ACTION(status != -1, error = OT_ERROR_FAILED);

exit:
    return error;
}

otError utilsFlashStatusWait(uint32_t aTimeout)
{
    otError error = OT_ERROR_NONE;
    uint32_t start = otPlatAlarmMilliGetNow();
    bool busy = true;
    uint32_t status = 0x01;

    while (busy && ((otPlatAlarmMilliGetNow() - start) < aTimeout))
    {
        status = w25qxx_read_status(w25q128bv);
        busy =  status & 0x01;
    }

    otEXPECT_ACTION(!busy, error = OT_ERROR_BUSY);

exit:
    return error;
}

uint32_t utilsFlashWrite(uint32_t aAddress, uint8_t *aData, uint32_t aSize)
{
    int32_t written_size = 0;
    uint32_t size = 0;

    otEXPECT_ACTION((aAddress >= OPENTHREAD_FLASH_BASE) &&
                    ((aAddress + aSize) <= OPENTHREAD_FLASH_BASE + OPENTHREAD_FLASH_SIZE) &&
                    (!(aAddress & 3)) && (!(aSize & 3)), ;);

    written_size = w25qxx_write(w25q128bv, aAddress, aSize, aData);

    if (written_size < 0)
    {
        size = 0;
    }
    else
    {
        size = (uint32_t)written_size;
    }

exit:
    return size;
}

uint32_t utilsFlashRead(uint32_t aAddress, uint8_t *aData, uint32_t aSize)
{
    int32_t read_size = 0;
    uint32_t size = 0;

    otEXPECT_ACTION((aAddress >= OPENTHREAD_FLASH_BASE) &&
                    ((aAddress + aSize) <= OPENTHREAD_FLASH_BASE + OPENTHREAD_FLASH_SIZE), ;);

    read_size = w25qxx_read(w25q128bv, aAddress, aSize, aData);

    if (read_size < 0)
    {
        size = 0;
    }
    else
    {
        size = (uint32_t)read_size;
    }


exit:
    return size;
}

#endif /* BOARD_EMSK */


#ifdef BOARD_HSDK
/*
 * The HSDK flash driver is only for compiling the OpenThread code.
 */
otError utilsFlashInit(void)
{
    // Todo
    return OT_ERROR_NONE;
}

uint32_t utilsFlashGetSize(void)
{
    return 0x1000;
}

otError utilsFlashErasePage(uint32_t aAddress)
{
    (void)aAddress;
    return OT_ERROR_NONE;
}

otError utilsFlashStatusWait(uint32_t aTimeout)
{
    (void)aTimeout;
    return OT_ERROR_NONE;
}

uint32_t utilsFlashWrite(uint32_t aAddress, uint8_t *aData, uint32_t aSize)
{
    (void)aAddress;
    (void)aData;
    (void)aSize;
    return aSize;
}

uint32_t utilsFlashRead(uint32_t aAddress, uint8_t *aData, uint32_t aSize)
{
    (void)aAddress;
    (void)aData;
    (void)aSize;
    return aSize;
}

#endif /* BOARD_HSDK */
