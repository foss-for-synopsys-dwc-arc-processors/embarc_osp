/**
 * @file       rsi_hal_mcu_spi.c
 * @version    0.1
 * @date       18 sept 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief: HAL SPI API
 *
 * @Description:
 * This file Contains all the API's related to HAL
 *
 */


/**
 * Includes
 */
#include "rsi_driver.h"
#include "dev_spi.h"
#include "board.h"
#include "embARC_error.h"
#include "embARC_debug.h"

/**
 * Global Variables
 */
#define RSI_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

// int32_t rsi_spi_open(uint32_t freq, uint32_t clk_mode)
// {
// 	int32_t ercd = E_OK;
// 	DEV_SPI_PTR rsi_spi_ptr;

// 	rsi_spi_ptr = spi_get_dev(BOARD_RSI_SPI_ID);
// 	RSI_CHECK_EXP(rsi_spi_ptr != NULL, E_OBJ);

// 	DBG("rsi_spi_open DEV_MASTER_MODE= 0x%x (%d),freq= 0x%x (%d) \r\n", DEV_MASTER_MODE, DEV_MASTER_MODE, freq, freq);
// 	ercd = rsi_spi_ptr->spi_open(DEV_MASTER_MODE, freq);
// 	if ((ercd == E_OPNED) || (ercd == E_OK)) {
// 		ercd = rsi_spi_ptr->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(clk_mode));
// 	}
// error_exit:
// 	return ercd;
// }

int32_t rsi_spi_close(void)
{
	int32_t ercd = E_OK;
	DEV_SPI_PTR rsi_spi_ptr;

	rsi_spi_ptr = spi_get_dev(BOARD_RSI_SPI_ID);
	RSI_CHECK_EXP(rsi_spi_ptr != NULL, E_OBJ);

	ercd = rsi_spi_ptr->spi_close();

error_exit:
	return ercd;
}

/*==================================================================*/
/**
 * @fn         int16_t rsi_spi_transfer(uint8_t *ptrBuf,uint16_t bufLen,uint8_t *valBuf,uint8_t mode)
 * @param[in]  uint8_t *tx_buff, pointer to the buffer with the data to be transfered
 * @param[in]  uint8_t *rx_buff, pointer to the buffer to store the data received
 * @param[in]  uint16_t transfer_length, Number of bytes to send and receive
 * @param[in]  uint8_t mode, To indicate mode 8 BIT/32 BIT mode transfers.
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to tranfer/receive data to the Wi-Fi module through the SPI interface.
 */
int16_t rsi_spi_transfer(uint8_t *tx_buff, uint8_t *rx_buff, uint16_t transfer_length,uint8_t mode)
{
	int32_t ercd = E_OK;
	uint32_t tx_cnt = transfer_length;
	uint32_t rx_cnt = transfer_length;
	uint32_t cs_line = BOARD_RSI_SPI_CS;
	DEV_SPI_PTR rsi_spi_ptr = spi_get_dev(BOARD_RSI_SPI_ID);
	RSI_CHECK_EXP(rsi_spi_ptr != NULL, E_OBJ);

	if(mode != RSI_MODE_8BIT){
		DBG("rsi_spi_transfer mode = %d\r\n", mode);
	}
	if(tx_buff == NULL){
		tx_cnt = 0;
	}
	if(rx_buff == NULL){
		rx_cnt = 0;
	}
	// //! Assert the chip select pin
	// rsi_spi_cs(1);

	// for(i=0; i<transfer_length; i++)
	// {
	// 	//! Transfer the data
	// 	if(tx_buff!=NULL){
	// 		ercd = rsi_spi_ptr -> spi_write(tx_buff+i, 1);
	// 		RSI_CHECK_EXP(ercd >= E_OK, ercd);
	// 	} else {
	// 		//tx_buff is NULL, send dummy data instead
	// 		ercd = rsi_spi_ptr -> spi_write(0x00, 1);
	// 	}
	// 	//! Receive the data
	// 	if(rx_buff!=NULL){
	// 		ercd = rsi_spi_ptr -> spi_read(rx_buff+i, 1);
	// 		RSI_CHECK_EXP(ercd >= E_OK, ercd);
	// 	}
	// }

	// //! Deassert the chip select pin
	// rsi_spi_cs(0);
	// ercd = E_OK;

	DEV_SPI_TRANSFER spi_xfer;
	/* Master and Slave transmit */
	DEV_SPI_XFER_SET_TXBUF(&spi_xfer, tx_buff, 0, tx_cnt);
	DEV_SPI_XFER_SET_RXBUF(&spi_xfer, rx_buff, 0, rx_cnt);
	DEV_SPI_XFER_SET_NEXT(&spi_xfer, NULL);

	ercd = rsi_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ercd = rsi_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&spi_xfer));
	ercd = rsi_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));

error_exit:
	return ercd;
}



