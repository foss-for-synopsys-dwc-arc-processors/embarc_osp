#ifndef UART_H_
#define UART_H_


#include "../ioctl.h"


/* UART control commands */
#define IO_UART_SET_LCR     		(0x20)
#define IO_UART_GET_LCR     		(0x21)
#define IO_UART_SET_DLx				(0x22)
#define IO_UART_GET_DLx				(0x23)
#define IO_UART_SET_RX_THRESHOLD	(0x24)
#define IO_UART_SET_TX_THRESHOLD	(0x25)
#define IO_UART_SET_FLOW_CONTROL  	(0x26)

extern uint32_t io_uart_open( uint32_t dev_id );
extern void io_uart_close( uint32_t dev_id );
extern void io_uart_read( uint32_t dev_id, uint8_t * data, uint32_t * size );
extern void io_uart_write( uint32_t dev_id, uint8_t * data, uint32_t * size );
extern void io_uart_poll_write(uint32_t dev_id, uint8_t *data, uint32_t *size);
extern void io_uart_poll_read(uint32_t dev_id, uint8_t *data, uint32_t *size);
extern void io_uart_rx_int(uint32_t dev_id, uint32_t enable);
extern void io_uart_tx_int(uint32_t dev_id, uint32_t enable);
/* UART valid IOCTLs
  cmd                           arg type        arg value
  IO_SET_CB_RX                  io_cb_t         Callback function invoked from ISR when read successfully finishes
  IO_SET_CB_TX                  io_cb_t         Callback function invoked from ISR when write successfully finishes
  IO_SET_CB_ERR                 io_cb_t         Callback function invoked from ISR when read/write fails
  IO_UART_SET_LCR               uint32_t        7 bits LCR[6:0] = { BC, Stick Parity, EPS, PEN, STOP, DLS[1:0] }
  IO_UART_GET_LCR               uint32_t        7 bits LCR[6:0] = { BC, Stick Parity, EPS, PEN, STOP, DLS[1:0] }
  IO_UART_SET_DLx               uint32_t        16 bits DLH[7:0];DLL[7:0]
  IO_UART_GET_DLx               uint32_t        16 bits DLH[7:0];DLL[7:0]
  IO_UART_SET_RX_THRESHOLD		uint32_t		2 bits encoding the rx-fifo (if any) threshold
  IO_UART_SET_TX_THRESHOLD		uint32_t		2 bits encoding the tx-fifo (if any) threshold
  IO_UART_SET_FLOW_CONTROL  	uint32_t 		0 - disable flow control; 1 - enable flow control
*/
extern void io_uart_ioctl( uint32_t dev_id, uint32_t cmd, void * arg );

#endif /* UART_H_ */