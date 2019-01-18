#if defined(BOARD_EMSK)
#include <stdint.h>
#include "board.h"
#include "spi_flash.h"
#include "arc_udma.h"
#include "dw_spi_hal.h"
#include "dw_spi.h"
#include "spi_flash_w25qxx.h"

#define _MEMORY_FENCE()				_arc_sync()
#define _DCACHE_FLUSH_MLINES(addr, size)	dcache_flush_mlines((uint32_t)(addr), (uint32_t)(size))
#define _DCACHE_INVALIDATE_MLINES(addr, size)	dcache_invalidate_mlines((uint32_t)(addr), (uint32_t)(size))
#define _ICACHE_INVALIDATE_MLINES(addr, size)	icache_invalidate_mlines((uint32_t)(addr), (uint32_t)(size))

/**
 * \name	SPI Flash Commands
 * @{
 */
#define RDID	0x9F	/*!<read chip ID */
#define RDSR	0x05	/*!< read status register */
#define WRSR	0x01	/*!< write status register */
#define WREN	0x06	/*!< write enablewaitDeviceReady */
#define WRDI	0x04	/*!< write disable */
#define READ	0x03	/*!< read data bytes */
#define SE	0x20	/*!< sector erase */
#define PP	0x02	/*!< page program */
/** @} end of name */
#define SPI_FLASH_NOT_VALID	(0xFFFFFFFF)

#define SPI_TX_UDMA_CHN		0
#define SPI_RX_UDMA_CHN		1

typedef struct spi_xfer SPI_XFER, *SPI_XFER_PTR;
/**
 * \brief	spi read and write data structure used by \ref SPI_CMD_TRANSFER
 * 	spi write then read data
 *
 */
struct spi_xfer {
	SPI_XFER *next;
	/* Set by user */
	uint8_t *tx_buf;
	uint8_t *rx_buf;
	uint32_t len;
	/* Should auto set to proper value during set buffer value */
	uint32_t tx_idx;
	uint32_t rx_idx;
};

static volatile DW_SPI_REG *spi_reg = (DW_SPI_REG *)0xF0006000U;

static dma_state_t udma;
static dma_channel_t dma_chn_tx, dma_chn_rx;

#define SPIFLASH_LINE_CS	5


int32_t spi_init(uint32_t freq)
{
	spi_reg->SSIENR = DW_SPI_SSI_DISABLE;
	/* Clear interrupts */
	uint32_t ctrl0_reg = spi_reg->ICR;
	/* Mask all interrupts */
	spi_reg->IMR = 0;

	ctrl0_reg = DW_SPI_CTRLR0_FRF_MOTOROLA | DW_SPI_TMOD_TRANSMIT_RECEIVE \
			 | 7 | DW_SPI_CTRLR0_SLV_OE_ENABLE;
	spi_reg->CTRLR0 = ctrl0_reg;
	spi_reg->CTRLR1 = 0;

	/* deselect slaves */
	spi_reg->SER = 0;
	spi_reg->DMACR = 0;
	spi_reg->DMATDLR = 0;
	spi_reg->DMARDLR = 0;

	uint32_t sck_divisor = 50000000/freq;
	spi_reg->BAUDR = sck_divisor;

	/* Set threshold values for both tx and rx */
	spi_reg->TXFTLR = 0;
	spi_reg->RXFTLR = 0;
	spi_reg->SSIENR = DW_SPI_SSI_ENABLE;
	return 0;
}

void spi_dma_prepare(void)
{
	/** Must init uDMA before use it */
	dmac_init(&udma);

	dmac_init_channel(&dma_chn_tx);
	dmac_init_channel(&dma_chn_rx);
	dmac_reserve_channel(SPI_TX_UDMA_CHN, &dma_chn_tx, DMA_REQ_PERIPHERAL);
	dmac_reserve_channel(SPI_RX_UDMA_CHN, &dma_chn_rx, DMA_REQ_PERIPHERAL);

}

#define SPI_XFER_LIST_LEN	2
#define SPI_XFER_ARB_SZ		16
void spi_xfer_callback(void *param)
{

}

void flush_xfer_data(SPI_XFER *xfer)
{
	SPI_XFER *cur_xfer = xfer;
	uint8_t i = 0;
	while (cur_xfer) {
		if (cur_xfer->tx_buf) {
			_MEMORY_FENCE();
			_DCACHE_FLUSH_MLINES((void *)(cur_xfer->tx_buf), cur_xfer->len);
		}
		if (cur_xfer->rx_buf) {
			_MEMORY_FENCE();
			_DCACHE_FLUSH_MLINES((void *)(cur_xfer->rx_buf), cur_xfer->len);
		}
		i ++;
		if (i >= SPI_XFER_LIST_LEN) {
			break;
		}
		cur_xfer = cur_xfer->next;
	}
}

void invalidate_xfer_data(SPI_XFER *xfer)
{
	SPI_XFER *cur_xfer = xfer;
	uint8_t i = 0;
	while (cur_xfer) {
		if (cur_xfer->tx_buf) {
			_MEMORY_FENCE();
			_DCACHE_INVALIDATE_MLINES((void *)(cur_xfer->tx_buf), cur_xfer->len);
		}
		if (cur_xfer->rx_buf) {
			_MEMORY_FENCE();
			_DCACHE_INVALIDATE_MLINES((void *)(cur_xfer->rx_buf), cur_xfer->len);
		}
		i ++;
		if (i >= SPI_XFER_LIST_LEN) {
			break;
		}
		cur_xfer = cur_xfer->next;
	}
}

/* Use dma interrupt is more stable than poll wait */
#define SPIFLASH_ENABLE_UDMA_INTERRUPT

int32_t spi_xfer(SPI_XFER *xfer)
{
	static __attribute__((aligned(16))) dma_desc_t dma_desc_tx[SPI_XFER_LIST_LEN];
	static __attribute__((aligned(16))) dma_desc_t dma_desc_rx[SPI_XFER_LIST_LEN];
	static dma_ctrl_t dma_ctrl_tx, dma_ctrl_rx;
	dma_callback_t xfer_cb = NULL;

	SPI_XFER *cur_xfer = xfer;
	uint8_t i = 0;
	uint8_t j = 0;
	uint32_t txtemp = 0xFFFFFFFF, rxtemp  = 0;

	while (cur_xfer) {
		DMA_CTRL_SET_OP(&dma_ctrl_tx, DMA_MANUAL_LINKED_TRANSFER);
		DMA_CTRL_SET_RT(&dma_ctrl_tx, DMA_MANUAL_REQUEST);
		DMA_CTRL_SET_DTT(&dma_ctrl_tx, DMA_MEM2MEM);
		DMA_CTRL_SET_DWINC(&dma_ctrl_tx, DMA_DW1INC1);
		DMA_CTRL_SET_ARB(&dma_ctrl_tx, SPI_XFER_ARB_SZ);
		DMA_CTRL_SET_INT(&dma_ctrl_tx, DMA_INT_DISABLE);
		if (cur_xfer->tx_buf) {
			DMA_CTRL_SET_AM(&dma_ctrl_tx, DMA_AM_SRCINC_DSTNOT);
			dmac_config_desc(&dma_desc_tx[i], (void *)(cur_xfer->tx_buf), (void *)(&spi_reg->DATAREG), cur_xfer->len, &dma_ctrl_tx);
		} else {
			DMA_CTRL_SET_AM(&dma_ctrl_tx, DMA_AM_SRCNOT_DSTNOT);
			dmac_config_desc(&dma_desc_tx[i], (void *)(&txtemp), (void *)(&spi_reg->DATAREG), cur_xfer->len, &dma_ctrl_tx);
		}
		DMA_CTRL_SET_OP(&dma_ctrl_rx, DMA_MANUAL_LINKED_TRANSFER);
		DMA_CTRL_SET_RT(&dma_ctrl_rx, DMA_MANUAL_REQUEST);
		DMA_CTRL_SET_DTT(&dma_ctrl_rx, DMA_MEM2MEM);
		DMA_CTRL_SET_DWINC(&dma_ctrl_rx, DMA_DW1INC1);
		DMA_CTRL_SET_ARB(&dma_ctrl_rx, SPI_XFER_ARB_SZ);
		DMA_CTRL_SET_INT(&dma_ctrl_rx, DMA_INT_DISABLE);
		if (cur_xfer->rx_buf) {
			DMA_CTRL_SET_AM(&dma_ctrl_rx, DMA_AM_SRCNOT_DSTINC);
			dmac_config_desc(&dma_desc_rx[i], (void *)(&spi_reg->DATAREG), (void *)(cur_xfer->rx_buf), cur_xfer->len, &dma_ctrl_rx);
		} else {
			DMA_CTRL_SET_AM(&dma_ctrl_rx, DMA_AM_SRCNOT_DSTNOT);
			dmac_config_desc(&dma_desc_rx[i], (void *)(&spi_reg->DATAREG), (void *)(&rxtemp), cur_xfer->len, &dma_ctrl_rx);
		}
		i ++;
		if (i >= SPI_XFER_LIST_LEN) {
			break;
		}
		cur_xfer = cur_xfer->next;
	}
	dmac_desc_add_linked(&dma_desc_tx[i-1], NULL);
	dmac_desc_add_linked(&dma_desc_rx[i-1], NULL);
	for (j = i; j > 1; j --) {
		dmac_desc_add_linked(&dma_desc_tx[j-2], &dma_desc_tx[j-1]);
		dmac_desc_add_linked(&dma_desc_rx[j-2], &dma_desc_rx[j-1]);
	}
	/* Init and configure dma channel transfer with transfer descriptor */
	dmac_config_channel(&dma_chn_tx, &dma_desc_tx[0]);
	dmac_config_channel(&dma_chn_rx, &dma_desc_rx[0]);

	dmac_stop_channel(&dma_chn_rx);
	dmac_stop_channel(&dma_chn_tx);

#ifdef SPIFLASH_ENABLE_UDMA_INTERRUPT
	xfer_cb = spi_xfer_callback;
#else
	arc_lock();
	xfer_cb = NULL;
#endif
	/* select device */
	spi_reg->SSIENR = DW_SPI_SSI_DISABLE;
	spi_reg->DMACR = 0;
	spi_reg->DMATDLR = 32-SPI_XFER_ARB_SZ;
	spi_reg->DMARDLR = SPI_XFER_ARB_SZ-1;
	flush_xfer_data(xfer);
	dmac_start_channel(&dma_chn_rx, xfer_cb, DMA_CHN_HIGH_PRIO);
	dmac_start_channel(&dma_chn_tx, xfer_cb, DMA_CHN_HIGH_PRIO);
	spi_reg->SER = 1<<SPIFLASH_LINE_CS;
	spi_reg->SSIENR = DW_SPI_SSI_ENABLE;
	/* enable rx and tx dma */
	spi_reg->DMACR = 3;
	dmac_wait_channel(&dma_chn_tx);
	dmac_wait_channel(&dma_chn_rx);
	/* deselect device */
	spi_reg->SSIENR = DW_SPI_SSI_DISABLE;
	spi_reg->DMACR = 0;
	spi_reg->DMATDLR = 0;
	spi_reg->DMARDLR = 0;
	spi_reg->SER = 0;
	spi_reg->SSIENR = DW_SPI_SSI_ENABLE;
#ifndef SPIFLASH_ENABLE_UDMA_INTERRUPT
	arc_unlock();
#endif
	invalidate_xfer_data(xfer);

	return 0;
}

/**
 * \brief	init spi flash related interface
 * \retval	0	success
 * \retval	-1	fail
 */
void spiflash_init(uint32_t freq)
{
	spi_init(freq);
	spi_dma_prepare();
}

/**
 * \brief	read spi flash identification ID
 * \return	the id of the spi flash
 */
uint32_t spiflash_read_id(void)
{
	uint32_t id = 0;
	static uint8_t local_buf[4];
	SPI_XFER cmd_xfer;

	local_buf[0] = RDID;

	cmd_xfer.tx_buf = local_buf;
	cmd_xfer.rx_buf = local_buf;
	cmd_xfer.len = 4;
	cmd_xfer.next = NULL;

	if (spi_xfer(&cmd_xfer) == 0) {
		id = (local_buf[1] << 16) | (local_buf[2] << 8) | local_buf[3];
	} else {
		id = -1;
	}

	return id;
}

/**
 * \brief	read the status of spi flash
 * \return	current status of spi flash
 */
uint32_t spiflash_read_status(void)
{
	uint8_t local_buf[2];
	SPI_XFER cmd_xfer;

	local_buf[0] = RDSR;

	cmd_xfer.tx_buf = local_buf;
	cmd_xfer.rx_buf = local_buf;
	cmd_xfer.len = 2;
	cmd_xfer.next = NULL;

	if (spi_xfer(&cmd_xfer) == 0) {
		return (uint32_t)local_buf[1];
	} else {
		return SPI_FLASH_NOT_VALID;
	}
}

/**
 * \brief	read data from flash
 * \param[in]	address		read start address of spi flash
 * \param[in]	size		read size of spi flash
 * \param[out]	data		data to store the return data
 *
 * \retval	-1		failed in read operation
 * \retval	>=0		data size of data read
 */
int32_t spiflash_read(uint32_t address, uint32_t size, void *data)
{
	uint8_t local_buf[4];
	SPI_XFER cmd_xfer;
	SPI_XFER data_xfer;
	uint32_t xfer_size, total_size = 0;
	uint8_t *xfer_buf = (uint8_t *)data;

	do {
		if (spiflash_wait_ready() != 0) {
			return -1;
		}
		xfer_size = size >= 8192 ? 8192 : size;

		local_buf[0] = READ;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] = address  & 0xff;

		cmd_xfer.tx_buf = local_buf;
		cmd_xfer.rx_buf = NULL;
		cmd_xfer.len = 4;
		cmd_xfer.next = &data_xfer;

		data_xfer.tx_buf = NULL;
		data_xfer.rx_buf = (uint8_t *)(xfer_buf);
		data_xfer.len = xfer_size;
		data_xfer.next = NULL;

		address += xfer_size;
		size -= xfer_size;
		xfer_buf += xfer_size;

		if (spi_xfer(&cmd_xfer) == 0) {
			total_size += xfer_size;
		} else {
			break;
		}
	} while (size > 0);
	if (spiflash_wait_ready() != 0) {
		return -1;
	}
	return total_size;
}

/**
 * \brief 	Read status and wait while busy flag is set
 * \retval	0	success
 * \retval	-1	fail
 */
int32_t spiflash_wait_ready(void)
{
	uint32_t status = 0x01;
	do {
		status = spiflash_read_status();
		if (status == SPI_FLASH_NOT_VALID) {
			return -1;
		}
	} while (status & 0x01);
	return 0;
}

/**
 * \brief 	enable to write flash
 * \retval	0	success
 * \retval	-1	fail
 */
int32_t spiflash_write_enable(void)
{
	uint8_t local_buf[3];
	SPI_XFER cmd_xfer;

	uint32_t status = 0;
	do {
		local_buf[0] = WREN;

		cmd_xfer.tx_buf = local_buf;
		cmd_xfer.rx_buf = NULL;
		cmd_xfer.len = 1;
		cmd_xfer.next = NULL;

		if (spi_xfer(&cmd_xfer) != 0) {
			return -1;
		}
		status = spiflash_read_status();
		if (status == SPI_FLASH_NOT_VALID) {
			return -1;
		}
		// clear protection bits
		//  Write Protect. and Write Enable.
		if( (status & 0xfc) && (status & 0x02) ) {
			local_buf[0] = WRSR; // write status
			local_buf[1] = 0x00; // write status
			local_buf[2] = 0x00; // write status

			cmd_xfer.tx_buf = local_buf;
			cmd_xfer.rx_buf = NULL;
			cmd_xfer.len = 3;
			cmd_xfer.next = NULL;

			if (spi_xfer(&cmd_xfer) != 0) {
				return -1;
			}
			status = 0;
		}
	} while ( status != 0x02);

	return 0;
}


/**
 * \brief 	flash erase in sectors
 *
 * \param[in]	address		erase start address of spi flash
 * \param[in]	size		erase size
 *
 * \retval	-1 		failed in erase operation
 * \retval	>=0		sector count erased
 */
int32_t spiflash_erase(uint32_t address, uint32_t size)
{
	uint32_t last_address;
	uint32_t count = 0;
	uint8_t local_buf[4];
	SPI_XFER cmd_xfer;

	// start address of last sector
	last_address = (address + size) & (~(FLASH_SECTOR_SIZE - 1));

	// start address of first sector
	address &= ~(FLASH_SECTOR_SIZE - 1);

	do {
		if (spiflash_write_enable() != 0) {
			return -1;
		}

		if (spiflash_wait_ready() != 0) {
			return -1;
		}

		local_buf[0] = SE;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] =  address & 0xff;

		cmd_xfer.tx_buf = local_buf;
		cmd_xfer.rx_buf = NULL;
		cmd_xfer.len = 4;
		cmd_xfer.next = NULL;

		if (spi_xfer(&cmd_xfer) != 0) {
			return -1;
		}

		address += FLASH_SECTOR_SIZE;
		count++;
	} while(address <= last_address);
	if (spiflash_wait_ready() != 0) {
		return -1;
	}

	return (int32_t)count;
}

/**
 * \brief	write data to spi flash
 *
 * \param[in]	address	start address
 * \param[in]	size	data size
 * \param[in]	data	pointer to data
 *
 * \retval	>=0		written bytes number
 * \retval 	<0 		error
 */
int32_t spiflash_write(uint32_t address, uint32_t size, const void *data)
{

	uint8_t local_buf[4];
	SPI_XFER cmd_xfer;
	SPI_XFER data_xfer;
	uint8_t *xfer_buf = (uint8_t *)data;

	uint32_t first = 0;
	uint32_t size_orig = size;

	first = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));

	do {
		// send write enable command to flash
		if (spiflash_write_enable() != 0) {
			return -1;
		}
		if (spiflash_wait_ready() != 0) {
			return -1;
		}

		first = first < size ? first : size;

		local_buf[0] = PP;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] = address  & 0xff;

		cmd_xfer.tx_buf = local_buf;
		cmd_xfer.rx_buf = NULL;
		cmd_xfer.len = 4;
		cmd_xfer.next = &data_xfer;

		data_xfer.tx_buf = (uint8_t *)(xfer_buf);
		data_xfer.rx_buf = NULL;
		data_xfer.len = first;
		data_xfer.next = NULL;

		if (spi_xfer(&cmd_xfer) != 0) {
			return -1;
		}

		size -= first;
		address += first;
		xfer_buf += first;
		first = FLASH_PAGE_SIZE;

	} while (size);

	if (spiflash_wait_ready() != 0) {
		return -1;
	}

	return (int32_t)(size_orig);
}
#endif//#if defined(BOARD_EMSK)