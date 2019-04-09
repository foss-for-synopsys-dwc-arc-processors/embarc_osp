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
//#define DEBUG
#include "embARC_debug.h"
#include "arc_exception.h"

/**
 * Global Variables
 */
#define RSI_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

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
int16_t rsi_spi_transfer(uint8_t *tx_buff, uint8_t *rx_buff, uint16_t transfer_length, uint8_t mode)
{
	int32_t ercd = E_OK;
	uint32_t cs_line = BOARD_RSI_SPI_CS;
	DEV_SPI_PTR rsi_spi_ptr = spi_get_dev(BOARD_RSI_SPI_ID);
	RSI_CHECK_EXP(rsi_spi_ptr != NULL, E_OBJ);

	if(mode != RSI_MODE_8BIT){
		DBG("rsi_spi_transfer mode = %d\r\n", mode);
	}

	DEV_SPI_TRANSFER spi_xfer;
	/* Master and Slave transmit */
	DEV_SPI_XFER_SET_TXBUF(&spi_xfer, tx_buff, 0, (tx_buff == NULL) ? 0 : transfer_length);
	DEV_SPI_XFER_SET_RXBUF(&spi_xfer, rx_buff, 0, (rx_buff == NULL) ? 0 : transfer_length);
	DEV_SPI_XFER_SET_NEXT(&spi_xfer, NULL);

	uint32_t status = cpu_lock_save();
	ercd = rsi_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ercd = rsi_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&spi_xfer));
	ercd = rsi_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
	cpu_unlock_restore(status);

error_exit:
	return ercd;
}



