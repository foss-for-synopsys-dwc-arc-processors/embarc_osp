#ifndef I2S_RX_SLAVE_H_
#define I2S_RX_SLAVE_H_

#include "ioctl.h"

/* I2S Rx master IO control commands */
#define IO_I2S_RX_SLAVE_SET_SAMPLE_WIDTH     (0x20)
#define IO_I2S_RX_SLAVE_SET_FIFO_THRESHOLD   (0x21)
#define IO_I2S_RX_SLAVE_SET_BITCLOCK         (0x22)

extern uint32_t   io_i2s_rx_slave_open    (uint32_t dev_id);

extern void 	   io_i2s_rx_slave_close   (uint32_t dev_id);

/* Function: 		 io_i2s_rx_master_read
 * Parameters:
 *   dev_id [In]	 Identifier of I2S device instance.
 *   data   [In]     Address of input buffer where requested number of acquired samples shall be stored
 *   size   [In/Out] Address of variable that holds input buffer size specifying the number of samples to be acquired.
 *                   On invocation of callback, registerd by IO_SET_CB_RX command, the address contains the number
 *                   of samples actually acquired.
 * Returns:
 *   none
 */
extern void       io_i2s_rx_slave_read    ( uint32_t dev_id, uint32_t * data, uint32_t * size );

/* I2S valid IOCTLs
  cmd:                     				arg type:    arg value
  IO_SET_CB_RX             				io_cb_t     Callback function invoked from ISR whenever a series of samples
														         is available in the buffer provided by io_i2s_rx_master_read
  IO_SET_CB_ERR            				io_cb_t     Callback function invoked from ISR whenever input buffer overflow condition whould occur
  IO_I2S_RX_MASTER_SET_FIFO_THRESHOLD  uint32_t    threshold value for the Rx FIFO	(accepts command before first io_i2s_rx_master_read)
  IO_I2S_RX_MASTER_SET_SAMPLE_WIDTH		uint32_t 	sample width value 16 or 12 bits (accepts command before first io_i2s_rx_master_read)
  IO_I2S_RX_MASTER_SET_BITCLOCK		   uint32_t		0/1 - disable/enable i2s bit clock ans WS (accepts command before first io_i2s_rx_master_read)
*/
extern void       io_i2s_rx_slave_ioctl   ( uint32_t dev_id, uint32_t cmd, void * arg );

#endif /* I2S_RX_SLAVE_H_ */
