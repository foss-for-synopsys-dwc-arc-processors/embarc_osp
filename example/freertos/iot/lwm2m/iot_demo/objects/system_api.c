/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2017.03
 * \date 2016-07-25
 * \author Xinyi Zhao(xinyi@synopsys.com)
--------------------------------------------- */


#define RAM_BTADDRESS		0x17f00000      /*!< default ram start address of boot.bin */
#define BUF_SIZE		4096
#define BOOTIMAGE_HEAD		(0x68656164)
#define IMAGE_HEADER_LEN	(sizeof(image_t))

#if HW_VERSION == 10 || HW_VERSION == 11
#define BOOTIMAGE_START		(0x780000)
#define BOOTIMAGE_END		(0x800000)
#define BOOTIMAGE_MAX_SIZE	(BOOTIMAGE_END - BOOTIMAGE_START - IMAGE_HEADER_LEN)
#else
#define BOOTIMAGE_START		(0xD80000)
#define BOOTIMAGE_END		(0x1000000)
#define BOOTIMAGE_MAX_SIZE	(BOOTIMAGE_END - BOOTIMAGE_START - IMAGE_HEADER_LEN)
#endif


#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "spi_flash_w25qxx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <embARC.h>

typedef int (*fp_t)(void);


Inline unsigned int make_checks(unsigned int init, unsigned char *buf, unsigned int len)
{
	while(len--)
		init += *buf++;
	return init;
}

static int32_t read_spi(void * flash, uint32_t address, uint32_t size, uint32_t* checksum)
{
	uint8_t local_buf[BUF_SIZE];
	int32_t cnt, datalen, len;
	uint32_t csum;
	static void *ram = (void *)0x17f00000;
	if( checksum == NULL)
		return 0;

	w25qxx_wait_ready(flash);

	csum = *checksum;

	datalen = 0;
	do {
		cnt = w25qxx_read(flash, address, BUF_SIZE, local_buf);
		if (cnt < 0) {
			*checksum = 0;
			return 0;
		}
		len = (size < cnt) ? size : cnt;
		csum = make_checks(csum, local_buf, len);
		memcpy(ram, local_buf, len);
		ram = ram + len;
		size -= len;
		datalen += len;
		address += len;
	} while (size);

	*checksum = csum;

	return datalen;
 }

static int32_t reload_btloader(void)
{
	image_t header;
	uint32_t datalen;
	uint32_t checksum = 0;

	W25QXX_DEF(flash, BOARD_SFLASH_SPI_ID, BOARD_SFLASH_SPI_LIN, FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE);
	// read spi header
	w25qxx_init(flash, 2000000);
	datalen = w25qxx_read(flash, BOOTIMAGE_START, IMAGE_HEADER_LEN, &header);

	if(BOOTIMAGE_HEAD == header.head && \
		header.start >= BOOTIMAGE_START &&  header.start < BOOTIMAGE_START + BOOTIMAGE_MAX_SIZE && \
		header.start >= BOOTIMAGE_START &&  header.start < BOOTIMAGE_START + BOOTIMAGE_MAX_SIZE && \
		header.size > 0  && header.size <= BOOTIMAGE_MAX_SIZE ) {
		datalen = read_spi(flash, header.start, header.size, &checksum);

		if(checksum != header.checksum) {
			datalen = 0;
		}
	} else {
		datalen = 0;
	}

	return datalen;
}

static void prv_value_change(void* context, const char* uriPath, const char * value, size_t valueLength)
{
	lwm2m_uri_t uri;
	if (lwm2m_stringToUri((char *)uriPath, strlen(uriPath), &uri)) {

		lwm2m_resource_value_changed(context, &uri);
	}
}

void init_value_change(lwm2m_context_t * lwm2m)
{
	system_setValueChangedHandler(lwm2m, prv_value_change);
}

void system_reboot()
{
	fp_t fp;
	fp = (fp_t)(RAM_BTADDRESS+4);
	reload_btloader();
	cpu_lock();
	fp();   /* jump to program */
}