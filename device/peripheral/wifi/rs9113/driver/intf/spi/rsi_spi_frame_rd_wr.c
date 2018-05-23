/**
 * @file     rsi_spi_frame_rd_wr.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief SPI Control: Functions used to control SPI frame read/write functions
 *
 * @section Description
 * SPI function to read/write management descriptors frames to/from the Wi-Fi module
 *
 * @section Improvements
 * Function header is modified for frameDscRd() function
 */



/*
 * Includes
 */
#include "rsi_driver.h"

#ifdef RSI_SPI_INTERFACE
/**
 * Global Variables
 */

/*====================================================*/
/**
 * @fn          int16_t rsi_frame_read(uint8_t *pkt_buffer)
 * @brief This  function is used to read the response from module.
 * @param[in]   uint8_t *pkt_buffer, pointer to the buffer to which packet has to read
 *              which is used to store the response from the module
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @section description
 * This is a common function to read response for all the command and data from Wi-Fi module.
 */
int16_t rsi_frame_read(uint8_t *pkt_buffer)
{

	int16_t     retval;
	uint8_t     local_buffer[8];

	//! Read first 4 bytes
	retval = rsi_pre_dsc_rd(&local_buffer[0]);
	if (retval != 0x00)
	{
		return retval;
	}

	//! Read complete RX packet
	retval = rsi_pkt_rd(pkt_buffer, ((rsi_bytes2R_to_uint16(&local_buffer[2]))-4),((rsi_bytes2R_to_uint16(&local_buffer[0]))-4));
	if (retval != 0x00)
	{
		return retval;
	}

	return retval;
}

/*====================================================*/
/**
 * @fn          int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame,uint8_t *payloadparam,uint16_t size_param)
 * @brief       Common function for all the commands.
 * @param[in]   uFrameDsc uFrameDscFrame, frame descriptor
 * @param[in]   uint8_t *payloadparam, pointer to the command payload parameter structure
 * @param[in]   uint16_t size_param, size of the payload for the command
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @section description
 * This is a common function used to process a command to the Wi-Fi module.
 */
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame,uint8_t *payloadparam,uint16_t size_param)
{

#ifndef WISECONNECT_PLUS
	int16_t      retval;

	//! Write host descriptor
	retval = rsi_spi_frame_dsc_wr(uFrameDscFrame);
	if (retval != 0x00)
	{
		return retval;
	}

	//! Write payload if present
	if(size_param)
	{
		//! 4 byte align for payload size
		size_param = (size_param+3)&~3;
		retval = rsi_spi_frame_data_wr(size_param, payloadparam, 0, NULL);
		if (retval != 0x00)
		{
			return retval;
		}

	}
	return retval;


#else
	int16_t      retval;

	//! Wait for SPI Slave to become idle to take the command from master
	rsi_wait_for_spi_slave_idle();

	//! Send Write message command to slave
	rsi_send_write_message(size_param);

	//! Wait for SPI Slave to become ready to take the packet from master
	rsi_wait_for_spi_slave_ready();

	//! Write out the Command Frame Descriptor
	retval = rsi_spi_frame_dsc_wr(uFrameDscFrame);

	if (retval != 0x00)
	{
		return retval;
	}

	//! Write the Command Frame Data
	if(size_param)
	{
		retval = rsi_spi_frame_data_wr(size_param, payloadparam, 0, NULL);
		if (retval != 0x00)
		{

			return retval;
		}
	}

	return retval;
#endif
}

/*===========================================================================*/
/**
 * @fn          int16_t rsi_pre_dsc_rd(uint8_t *dbuf)
 * @brief       Reads a pre frame descriptor
 * @param[in]   uint8_t *dbuf, pointer to the buffer into which pre decriptor has to be read
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @section description
 * This function performs pre frame decriptor read.
 */
int16_t rsi_pre_dsc_rd(uint8_t *dbuf)
{

#ifndef WISECONNECT_PLUS

	int16_t     retval;
	uint8_t     c1;
	uint8_t     c2;
	uint8_t     c3;
	uint8_t     c4;

	c1 = RSI_C1FRMRD16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
	c2 = RSI_C2SPIADDR4BYTE;
#else
	c2 = RSI_C2MEMRDWRNOCARE;
#endif
	//! command frame response descriptor
	c3 = 0x04;

	//! upper bye of transfer length
	c4 = 0x00;

	//! Send C1/C2
	retval = rsi_send_c1c2(c1, c2);
	if (retval != 0)
	{
		//! exit with error if we timed out waiting for the SPI to get ready
		return retval;
	}

	// Send C3/C4
	retval = rsi_send_c3c4(c3, c4);

	// Wait for start token
	retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT,RSI_MODE_32BIT);
	if (retval != 0)
	{
		//! exit with error if we timed out waiting for the SPI to get ready
		return retval;
	}

	//! SPI read after start token
	retval = rsi_spi_transfer(NULL ,(uint8_t *)dbuf, 0x4, RSI_MODE_32BIT);

	return retval;
#else
	int16_t     retval;

	//! Wait for SPI Slave to become idle to take the command from master
	rsi_wait_for_spi_slave_idle();

	//! Send Read message command to slave
	rsi_send_read_message();

   //! Wait for SPI Slave to become ready to take the pakcet from slave
	rsi_wait_for_spi_slave_ready();

	//! Read dummy byte comming from slave(Work around for 1 byte transfer issue)
	retval = rsi_spi_transfer(NULL ,(uint8_t *)dbuf, 0x1, RSI_MODE_8BIT);

	//! Read actual 4 bytes from slave
	retval = rsi_spi_transfer(NULL ,(uint8_t *)dbuf, 0x4, RSI_MODE_8BIT);

	return retval;
#endif
}


/*===========================================================================*/
/**
 * @fn          int16_t rsi_pkt_rd(uint8_t *buf, uint16_t dummy_len, uint16_t total_len)
 * @brief       Reads frame descriptor and payload
 * @param[in]   uint8_t *dbuf, pointer to the buffer into which  decriptor and payload has to be read
 * @param[in]   uint16_t dummy_len, number of dummy bytes which can be discarded
 * @param[in]   uint16_t total_len, number of bytes to be read
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @section description
 * This function performs frame decriptor and payload read.
 */

int16_t rsi_pkt_rd(uint8_t *buf, uint16_t dummy_len, uint16_t total_len)
{

#ifndef WISECONNECT_PLUS

	int16_t     retval;
	uint8_t     c1;
	uint8_t     c2;
	uint8_t     c3;
	uint8_t     c4;
#ifdef SAPIS_BT_STACK_ON_HOST
	uint8_t     dummy_buf[150];
#else
	uint8_t     dummy_buf[8];
#endif
	uint32_t    aligned_len=0;

	aligned_len = ((total_len) + 3) & ~3;

	c1 = RSI_C1FRMRD16BIT1BYTE;
#ifdef RSI_BIT_32_SUPPORT
	c2 = RSI_C2SPIADDR1BYTE;
#else
	c2 = RSI_C2MEMRDWRNOCARE;
#endif
	//! command frame response descriptor
	c3 = aligned_len & 0xff;

	//! upper bye of transfer length
	c4 = (aligned_len & 0xff00) >> 8;

	//! Send C1/C2
	retval = rsi_send_c1c2(c1, c2);
	if (retval != 0)
	{
		//! exit with error if we timed out waiting for the SPI to get ready
		return retval;
	}

	//! Send C3/C4
	retval = rsi_send_c3c4(c3, c4);

	//! Wait for start token
	retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT,RSI_MODE_32BIT);
	if (retval != 0)
	{
		//! exit with error if we timed out waiting for the SPI to get ready
		return retval;
	}

	if(dummy_len)
	{
		//! spi recv for dummy bytes
		retval = rsi_spi_transfer(NULL,(uint8_t *)&dummy_buf[0],dummy_len,RSI_MODE_8BIT);
	}

	//! Actual spi read for descriptor and payload
	if(buf)
	{
		retval = rsi_spi_transfer(NULL, buf, (aligned_len - dummy_len),RSI_MODE_32BIT);
	}

	return retval;
#else
	int16_t     retval;

	uint8_t     dummy_buf[8];

	uint32_t    aligned_len=0;

	aligned_len = total_len;

	if(dummy_len)
	{
		//! spi recv for dummy bytes
		retval = rsi_spi_transfer(NULL,(uint8_t *)&dummy_buf[0],dummy_len,RSI_MODE_8BIT);
	}

	//! Actual spi read for descriptor and payload
	if(buf)
	{
		retval = rsi_spi_transfer(NULL, buf, (aligned_len - dummy_len),RSI_MODE_32BIT); //TODO: this support only in 8bit mode need fix it for 32bit mode
	}
	return retval;

#endif

}

/*===========================================================================*/
/**
 * @fn          int16_t rsi_spi_frame_dsc_wr(rsi_frame_desc_t *uFrameDscFrame)
 * @brief       Writes a Frame descriptor
 * @param[in]   uFrameDsc uFrameDscFrame, frame descriptor
 * @param[in]   uint8_t type, To indicate the type whether it is DATA or MGMT
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @section description
 * This function performs Frame Decriptor Write.
 */
int16_t rsi_spi_frame_dsc_wr(rsi_frame_desc_t *uFrameDscFrame)
{
	int16_t                             retval;
	uint8_t                             localBuf[16];
#ifndef WISECONNECT_PLUS

	uint8_t                             c1;
	uint8_t                             c2;
	uint8_t                             c3;
	uint8_t                             c4;


	c1 = RSI_C1FRMWR16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
	c2 = RSI_C2RDWR4BYTE;
#else
	c2 = RSI_C2RDWR1BYTE;
#endif
	//! frame descriptor is 16 bytes long
	c3 = RSI_FRAME_DESC_LEN;

	//! upper bye of transfer length
	c4 = 0x00;

	//! send C1/C2
	retval = rsi_send_c1c2(c1, c2);
	if (retval != 0)
	{
		//! exit with error if we timed out waiting for the SPI to get ready
		return retval;
	}

	//! send C3/C4
	retval = rsi_send_c3c4(c3, c4);
#endif
	//! SPI send
	retval = rsi_spi_transfer(uFrameDscFrame->frame_len_queue_no,localBuf , RSI_FRAME_DESC_LEN,RSI_MODE_32BIT);

	return retval;

}


/*===========================================================================*/
/**
 * @fn          int16_t rsi_spi_frame_data_wr(uint16_t bufLen, uint8_t *dBuf,uint16_t tbufLen,uint8_t *tBuf)
 * @brief       Writes a Control data frame decriptor.
 * @param[in]   uint16_t buflen length of the data buffer to write
 * @param[in]   uint8_t *dBuf, pointer to the buffer of data to write
 * @param[in]   uint16_t tbuflen length of the data fragment to write
 * @param[in]   uint8_t *tBuf, pointer to the buffer of data fragment to write
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 * @section description
 * This function performs Frame Data Write.
 */
int16_t rsi_spi_frame_data_wr(uint16_t bufLen, uint8_t *dBuf,uint16_t tbufLen,uint8_t *tBuf)
{
#ifndef WISECONNECT_PLUS

	int16_t                     retval;
	uint8_t                     c1;
	uint8_t                     c2;
	uint8_t                     c3;
	uint8_t                     c4;
	uint16_t                    tempbufLen;
	tempbufLen = bufLen + tbufLen;

	c1 = RSI_C1FRMWR16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
	c2 = RSI_C2RDWR4BYTE;
#else
	c2 = RSI_C2RDWR1BYTE;
#endif
	//! lower byte of transfer length
	c3 = (uint8_t)(tempbufLen & 0x00ff);

	//! upper byte of transfer length
	c4 = (uint8_t)((tempbufLen >> 8) & 0x00FF);

	//! send C1/C2
	retval = rsi_send_c1c2(c1, c2);
	if (retval != 0)
	{
		//! exit with error if we timed out waiting for the SPI to get ready
		return retval;
	}

	//! send C3/C4
	retval = rsi_send_c3c4(c3, c4);

	//! SPI send
	retval = rsi_spi_transfer(dBuf, NULL, bufLen, RSI_MODE_32BIT);
	if(tbufLen)
	{
		retval = rsi_spi_transfer(tBuf, NULL,tbufLen, RSI_MODE_32BIT);
	}

	return retval;

#else
	int16_t                     retval;


	retval = rsi_spi_transfer(dBuf, NULL, bufLen, RSI_MODE_32BIT);
	if(tbufLen)
	{
		retval = rsi_spi_transfer(tBuf, NULL, tbufLen, RSI_MODE_32BIT);
	}
	return retval;

#endif

}



#endif
