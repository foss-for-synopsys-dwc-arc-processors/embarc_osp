/* ------------------------------------------
 * Copyright (c) 2021, Synopsys, Inc. All rights reserved.

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

#define BUFFER_SIZE 4

void printbuffer(uint8_t *tx, uint8_t *rx)
{
#ifdef BOARD_EMSK
	EMBARC_PRINTF("SPI Slave Buffer\n");
#elif defined BOARD_IOTDK
	EMBARC_PRINTF("SPI Master Buffer\n");
#endif
	EMBARC_PRINTF("TX:");
	for(uint8_t i = 0; i < BUFFER_SIZE; i++){
		EMBARC_PRINTF(" 0x%x", tx[i]);
	}
	EMBARC_PRINTF("\nRX:");
	for(uint8_t i = 0; i < BUFFER_SIZE; i++){
		EMBARC_PRINTF(" 0x%x", rx[i]);
	}
	EMBARC_PRINTF("\n");
	return;
}

/* Please run EMSK (SPI Slave) first */
int main(void)
{
	unsigned int ercd = 0;
	EMBARC_PRINTF("SPI Master & Slave test " __DATE__ " / " __TIME__ " \n");

#ifdef BOARD_EMSK
	EMBARC_PRINTF("EMSK Pmod 1 (J1) lower row is working as SPI Slave\n");

	DEV_SPI_PTR pSpiSlave = spi_get_dev(DW_SPI_1_ID);

	ercd = pSpiSlave->spi_open(DEV_SLAVE_MODE, SPI_CLK_MODE_0);
	if ((ercd == E_OPNED) || (ercd == E_OK)) {
		EMBARC_PRINTF("SPI Slave is successfully opened\n");
	} else {
		EMBARC_PRINTF("SPI Slave is unable to open: error=%d\nEnd of Test\n", ercd);
		return E_SYS;
	}
	DEV_SPI_TRANSFER spi_s_xfer;
	uint8_t  spi_s_buffer_tx[BUFFER_SIZE]={0};
	uint8_t  spi_s_buffer_rx[BUFFER_SIZE]={0};
	DEV_SPI_XFER_SET_TXBUF(&spi_s_xfer, spi_s_buffer_tx, 0, BUFFER_SIZE);
	DEV_SPI_XFER_SET_RXBUF(&spi_s_xfer, spi_s_buffer_rx, 0, BUFFER_SIZE);
	DEV_SPI_XFER_SET_NEXT(&spi_s_xfer, NULL);

	while (1) {
		ercd = pSpiSlave->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&spi_s_xfer));
		printbuffer(spi_s_buffer_tx, spi_s_buffer_rx);
		// loop back the data
		for(uint8_t i = 0; i < BUFFER_SIZE; i++){
			spi_s_buffer_tx[i] = spi_s_buffer_rx[i];
		}
		board_delay_ms(500, 1);
	}
#elif defined BOARD_IOTDK
	EMBARC_PRINTF("IoTDK Pmod B upper row is working as SPI Master\n");
	DEV_SPI_PTR pSpiMaster = spi_get_dev(DFSS_SPI_1_ID);
	ercd = pSpiMaster->spi_open(DEV_MASTER_MODE, 1000000);
	if ((ercd == E_OPNED) || (ercd == E_OK)) {
		EMBARC_PRINTF("SPI Master is successfully opened\n");
		ercd = pSpiMaster->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));
	} else {
		EMBARC_PRINTF("SPI Master is unable to open: error=%d\nEnd of Test\n", ercd);
		return E_SYS;
	}
	DEV_SPI_TRANSFER spi_m_xfer;
	uint8_t  spi_m_buffer_tx[BUFFER_SIZE]={0};
	uint8_t  spi_m_buffer_rx[BUFFER_SIZE]={0};
	DEV_SPI_XFER_SET_TXBUF(&spi_m_xfer, spi_m_buffer_tx, 0, BUFFER_SIZE);
	DEV_SPI_XFER_SET_RXBUF(&spi_m_xfer, spi_m_buffer_rx, 0, BUFFER_SIZE);
	DEV_SPI_XFER_SET_NEXT(&spi_m_xfer, NULL);

	while (1) {
		ercd = pSpiMaster->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(0));
		ercd = pSpiMaster->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&spi_m_xfer));
		ercd = pSpiMaster->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(0));
		printbuffer(spi_m_buffer_tx, spi_m_buffer_rx);
		for(uint8_t i = 0; i < BUFFER_SIZE; i++){
			spi_m_buffer_tx[i]++;
		}
		board_delay_ms(3000, 1);
	}

#else
	EMBARC_PRINTF("This example is not supported under current configurations \r\n");
#endif


	return E_SYS;
}
