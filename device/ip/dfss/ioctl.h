#ifndef IOCTL_H_
#define IOCTL_H_


typedef void (* IO_CB_FUNC)( uint32_t );

typedef struct {
  IO_CB_FUNC  		cb;
} io_cb_t;

// IO devices common control commands
#define     IO_SET_CB_RX			(0x01)
#define     IO_SET_CB_TX 			(0x02)
#define     IO_SET_CB_ERR			(0x03)

#include "arc_exception.h"

#undef _Interrupt
#define _Interrupt

#define _setvecti(x, y)  int_handler_install(x, y)

#undef __Xdmac
#define __Xdmac 0

#endif
