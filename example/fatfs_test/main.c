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
 * specific prior done permission.

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

#define DATA_SIZE  8192
static FIL file;

uint8_t data_write[DATA_SIZE];
uint8_t data_read[DATA_SIZE];


/**
 * \brief	Test hardware board without any peripheral
 */
int main(void)
{
	uint8_t res;
	uint32_t i;
	uint32_t rw_counter;
	uint32_t done;
	uint8_t *buf;

	EMBARC_PRINTF("test start\r\n");

	for (i = 0; i < DATA_SIZE; i++) {
		data_write[i] = 'a' + i % 26;
	}

	res = f_open(&file, "test1.txt", FA_WRITE | FA_CREATE_ALWAYS);

	if (res) {
		EMBARC_PRINTF("file open err\r\n");
		while (1);
	}

	rw_counter = DATA_SIZE;
	buf = data_write;

	while (rw_counter) {
		res = f_write(&file, buf, rw_counter, &done);

		if (res) {
			EMBARC_PRINTF("file write error :%d\r\n", res);
			while (1);
		}
		rw_counter -= done;
		buf += done;
		EMBARC_PRINTF("%d bytes are written\r\n", done);
	}
	f_close(&file);

	res = f_open(&file, "test1.txt", FA_READ | FA_OPEN_EXISTING);

	if (res) {
		EMBARC_PRINTF("file open err\r\n");
		while (1);
	}

	rw_counter = DATA_SIZE;
	buf = data_read;

	while (rw_counter) {
		res = f_read(&file, buf, rw_counter, &done);

		if (res) {
			EMBARC_PRINTF("file read error :%d\r\n", res);
			while (1);
		}
		rw_counter -= done;
		buf += done;
		EMBARC_PRINTF("%d bytes are read\r\n", done);
	}

	while (1);
}
