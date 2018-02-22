#ifndef SPI_SLAVE_H_
#define SPI_SLAVE_H_

#include "../ioctl.h"


/* SPI slave control commands */
#define IO_SPI_SLAVE_SET_SRL_MODE          (0x20)
#define IO_SPI_SLAVE_SET_TRANSFER_MODE     (0x21)
#define IO_SPI_SLAVE_SET_SCPH              (0x23)
#define IO_SPI_SLAVE_SET_SCPL              (0x24)
#define IO_SPI_SLAVE_SET_DFS               (0x25)
#define IO_SPI_SLAVE_SET_OUTPUT_ENABLE     (0x26)
#define IO_SPI_SLAVE_SET_RX_THRESHOLD      (0x28)
#define IO_SPI_SLAVE_SET_TX_THRESHOLD      (0x29)
#define IO_SPI_SLAVE_SET_PACKING	   	   (0x30)


extern uint32_t io_spi_slave_open( uint32_t dev_id );
extern void io_spi_slave_close( uint32_t dev_id );
/* I2C master valid IOCTLs
  cmd                           arg type        arg value
  IO_SET_CB_RX                  io_cb_t         Callback function invoked from ISR when read successfully finishes
  IO_SET_CB_TX                  io_cb_t         Callback function invoked from ISR when write successfully finishes
  IO_SET_CB_ERR                 io_cb_t         Callback function invoked from ISR when read/write fails
  IO_SPI_SLAVE_SET_SRL_MODE    	uint32_t        SPI shift register operation mode:
                                                0 = normal operation mode, Tx to Rx register loop disabled
                                                1 = test operation mode, Tx to Rx register loop enabled
  IO_SPI_SLAVE_SET_TRANSFER_MODE	uint32_t    SPI transfer mode:
                                                0 = transmit and receive
                                                1 = transmit only
                                                2 = receive only
  IO_SPI_SLAVE_SET_SCPH         uint32_t       	SPI closk phase: inactive state of SPI clock is low (0) or high (1)
  IO_SPI_SLAVE_SET_SCPL         uint32_t       	SPI clock polarity: SPI clock toggles in middle of first data bit (0) or at start of first data bit (1)
  IO_SPI_SLAVE_SET_DFS          uint32_t       	SPI frame size in bits
  IO_SPI_SLAVE_SET_OUTPUT_ENABLE	uint32_t    Enables (1) or disables (0) slave output 
  IO_SPI_SLAVE_SET_RX_THRESHOLD uint32_t       	threshold value for the Rx FIFO
  IO_SPI_SLAVE_SET_TX_THRESHOLD uint32_t       	threshold value for the Tx FIFO
  IO_SPI_SLAVE_SET_PACKING 		uint32_t		1|0 = enable|disable data packing in Rx/Tx buffer
  
*/

extern void io_spi_slave_ioctl( uint32_t dev_id, uint32_t cmd, void * arg );
extern void io_spi_slave_read( uint32_t dev_id, uint8_t * data, uint32_t * size );
extern void io_spi_slave_write( uint32_t dev_id, uint8_t * data, uint32_t * size );


#endif /* SPI_SLAVE_H_ */
