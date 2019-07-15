#ifndef _SS_I2S_SLAVE_H_
#define _SS_I2S_SLAVE_H_
/* the wrapper of subsystem i2s master driver */

#include "ip/ip_hal/inc/dev_i2s.h"

typedef struct dw_i2s_slv_dev_context
{
	uint32_t reg_base;

	uint8_t	dev_id;
	uint8_t  io_mode;

	uint8_t int_tx_req;	      /* Interrupt number of TX_REQ */
	uint8_t int_tx_err;	      /* Interrupt number of TX_ERR */
	uint8_t int_rx_avil;       /* Interrupt number of RX_AVIL */
	uint8_t int_rx_err;        /* Interrupt number of RX_ERR */

	DEV_CALLBACK int_tx_cb;
	DEV_CALLBACK int_rx_cb;
	DEV_CALLBACK int_err_cb;

	volatile uint32_t flags;	/*  flag */
	DEV_I2S_INFO_PTR info;
} DW_I2S_SLV_DEV_CONTEXT;

extern int32_t ss_dw_i2s_slv_open         (DW_I2S_SLV_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param);
extern int32_t ss_dw_i2s_slv_close        (DW_I2S_SLV_DEV_CONTEXT *ctx);
extern int32_t ss_dw_i2s_slv_read         (DW_I2S_SLV_DEV_CONTEXT *ctx, void *data, uint32_t len);
extern int32_t ss_dw_i2s_slv_write        (DW_I2S_SLV_DEV_CONTEXT *ctx, void *data, uint32_t len);
extern int32_t ss_dw_i2s_slv_control      (DW_I2S_SLV_DEV_CONTEXT *ctx, uint32_t cmd, void *param);
extern void    ss_dw_i2s_slv_int_tx_cb    (DW_I2S_SLV_DEV_CONTEXT *ctx, void *param);
extern void    ss_dw_i2s_slv_int_rx_cb    (DW_I2S_SLV_DEV_CONTEXT *ctx, void *param);
extern void    ss_dw_i2s_slv_int_err_cb   (DW_I2S_SLV_DEV_CONTEXT *ctx, void *param);

#endif /* _SS_I2S_SLAVE_H_*/


