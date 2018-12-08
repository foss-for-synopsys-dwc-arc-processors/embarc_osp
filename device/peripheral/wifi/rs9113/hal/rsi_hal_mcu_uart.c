/**
 * @file       rsi_hal_mcu_uart.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief: HAL UART API
 *
 * @Description:
 * This file Contains all the API's related to HAL
 *
 */


/**
 * Includes
 */
#include "dev_uart.h"
#include "board.h"
#include "rsi_driver.h"
#include "embARC_error.h"

/**
 * Global Variables
 */
#define RSI_UART_ID	0
/*==================================================================*/
/**
 * @fn         int16_t rsi_uart_send(uint8_t *ptrBuf,uint16_t bufLen)
 * @param[in]  uint8 *ptrBuf, pointer to the buffer with the data to be sent/received
 * @param[in]  uint16 bufLen, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to send data to the Wi-Fi module through the UART interface.
 */
int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen)
{
	DEV_UART *uart_obj = uart_get_dev(RSI_UART_ID);
	if(uart_obj == NULL){
		return E_OBJ;
	}
	uart_obj->uart_write(ptrBuf, bufLen);
	return 0;
}


/*==================================================================*/
/**
 * @fn         int16_t rsi_uart_recv(uint8_t *ptrBuf,uint16_t bufLen)
 * @param[in]  uint8_t *ptrBuf, pointer to the buffer with the data to be sent/received
 * @param[in]  uint16_t bufLen, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @description
 * This API is used to receive data from Wi-Fi module through the UART interface.
 */


int16_t rsi_uart_recv(uint8_t *ptrBuf, uint16_t bufLen)
{
	uint32_t rd_avail;
	DEV_UART *uart_obj = uart_get_dev(RSI_UART_ID);
	if (uart_obj == NULL){
		return E_OBJ;
	}
	uart_obj->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
	bufLen = (bufLen > rd_avail)?rd_avail:bufLen;
	if (bufLen > 0) {
		return uart_obj->uart_read(ptrBuf, bufLen);
	}
	return 0;
}


// int32_t rsi_uart_init(void){
// 	int32_t ercd = E_OK;
// 	DEV_UART *uart_obj = uart_get_dev(RSI_UART_ID);
// 	if (uart_obj == NULL){
// 		return E_OBJ;
// 	}

// 	/**open uart port, if already opened, then set baudrate*/
// 	if (uart_obj->uart_open(baudrate) == E_OPNED) {
// 		ercd = uart_obj->uart_control(UART_CMD_SET_BAUD, (void *)(baudrate));
// 	}

// error_exit:
// 	return ercd;
// }

// int32_t rsi_uart_deinit(void){
// 	int32_t ercd = E_OK;
// 	DEV_UART *uart_obj = uart_get_dev(RSI_UART_ID);
// 	ercd = uart_obj->uart_close();
// 	return ercd;
// }