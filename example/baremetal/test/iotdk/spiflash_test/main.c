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
--------------------------------------------- */

#include "embARC.h"
#include "embARC_debug.h"

#include "spi_flash_w25qxx.h"


W25QXX_DEF(w25q16dv, 0, 0, 0x100, 0x1000);

#define DATA_BUF_SIZE 128
#define SPI_FLASH_ADDRESS 0x1000

uint32_t data_buf[DATA_BUF_SIZE / 4];

int main(void)
{
	uint32_t id;
	int32_t data_len;
	int32_t i;

	w25qxx_init(w25q16dv, 1000000);

	id = w25qxx_read_id(w25q16dv);
	EMBARC_PRINTF("Device ID = %x\r\n",id);

	for (i = 0; i < (DATA_BUF_SIZE / 4); i++) {
		data_buf[i] = i;
	}

	if (w25qxx_erase(w25q16dv, SPI_FLASH_ADDRESS, 1024) < 0) {
		EMBARC_PRINTF("errase error \r\n");
		while (1);
	}

	if (w25qxx_write(w25q16dv, SPI_FLASH_ADDRESS, DATA_BUF_SIZE, data_buf) < 0) {
		EMBARC_PRINTF("write error\r\n");
		while (1);
	}

	data_len = w25qxx_read(w25q16dv, SPI_FLASH_ADDRESS, DATA_BUF_SIZE, data_buf);


	if (data_len > 0) {
		EMBARC_PRINTF("read %d bytes\r\n", data_len);

		for (i = 0; i < (DATA_BUF_SIZE / 4); i += 4) {
			EMBARC_PRINTF("%x, %x, %x, %x\r\n",
				data_buf[i + 0], data_buf[i + 1],
				data_buf[i + 2], data_buf[i + 3]);

		}
	} else {
		EMBARC_PRINTF("read error %d\r\n", data_len);
	}

	while (1);
}