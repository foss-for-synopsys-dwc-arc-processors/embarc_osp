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

EFLASH_DEFINE(eflash_test, EFLASH_CRTL_BASE);

uint8_t g_test_wr_buf[SMIC_EFLASH_PAGE_SIZE];
uint8_t g_test_rd_buf[SMIC_EFLASH_PAGE_SIZE];

int main(void)
{
	uint32_t page, ofst, i, size;
	SMIC_EFLASH_INFO eflash_info;

	smic_eflash_open(eflash_test);
	smic_eflash_control(eflash_test, SMIC_EFLASH_GET_INFO, (void *)(&eflash_info));
	EMBARC_PRINTF("erase macro\n");
	smic_eflash_control(eflash_test, SMIC_EFLASH_MACRO_ERASE, NULL);
	size = eflash_info.eflash_page_size;
	for (page = 0; page < eflash_info.eflash_page_cnt; page++)
	{
		// read data from page
		EMBARC_PRINTF("read page 0x%x\n", page);
		ofst = page * eflash_info.eflash_page_size;
		smic_eflash_read(eflash_test, ofst, size, g_test_rd_buf);
		// compare data, the data should be 0xFF after erase process
		EMBARC_PRINTF("compare data in page 0x%x\n", page);
		for (i = 0; i < size; i++)
		{
			if (g_test_rd_buf[i] != 0xFF)
			{
				EMBARC_PRINTF("pos[0x%x], rd: 0x%x\n", i, g_test_rd_buf[i]);
			}
		}
	}

	// erase pages
	for (page = 0; page < eflash_info.eflash_page_cnt; page++)
	{
		// earse page
		EMBARC_PRINTF("erase page 0x%x\n", page);
		ofst = page * eflash_info.eflash_page_size;
		smic_eflash_control(eflash_test, SMIC_EFLASH_PAGE_ERASE, (void *)ofst);
		// read data from page
		EMBARC_PRINTF("read page 0x%x\n", page);
		smic_eflash_read(eflash_test, ofst, size, g_test_rd_buf);
		// compare data, the data should be 0xFF after erase process
		for (i = 0; i < size; i++)
		{
			if (g_test_rd_buf[i] != 0xFF)
			{
				EMBARC_PRINTF("pos[0x%x], rd: 0x%x fail\n", i, g_test_rd_buf[i]);
				break;
			}
		}

		for (i = 0; i < size; i++)
		{
			g_test_wr_buf[i] = i & 0xFF;
		}

		// write data to page
		EMBARC_PRINTF("write page 0x%x\n", page);
		smic_eflash_write(eflash_test, ofst, size, g_test_wr_buf);
		// read data from page
		EMBARC_PRINTF("read page 0x%x\n", page);
		smic_eflash_read(eflash_test, ofst, size, g_test_rd_buf);

		// compare
		EMBARC_PRINTF("compare data in page 0x%x\n", page);
		for (i = 0; i < size; i++)
		{
			if (g_test_wr_buf[i] != g_test_rd_buf[i])
			{
				EMBARC_PRINTF("pos[0x%x], wr: 0x%x, rd: 0x%x fail\n", i, g_test_wr_buf[i], g_test_rd_buf[i]);
				break;
			}
		}
	}


	EMBARC_PRINTF("%s\n", __func__);

	return E_OK;
}

