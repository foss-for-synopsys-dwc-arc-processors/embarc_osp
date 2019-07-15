#include "embARC_error.h"
#include "embARC_toolchain.h"

/**
* Wrapper between dfss like driver bus DW-IP based now and emsdp board
*/
#include "ip/designware/i2s_slave/i2s_rx_slave.h"
#include "ip/designware/i2s_slave/ss_i2s_slave.h"

int32_t ss_dw_i2s_slv_open (DW_I2S_SLV_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param)
{
	uint32_t          dev_id   = ctx->dev_id;
	uint8_t           io_mode  = ctx->io_mode;
	//DEV_I2S_INFO_PTR  info     = ctx->info;
	//io_cb_t           callback;
	int32_t int_e = 0;

   //Only RX implemented
	if (io_mode == 0)
   {
		if (io_i2s_rx_slave_open(dev_id)) 
      {
			return E_SYS;
		}
      
		//callback.cb = (IO_CB_FUNC)ctx->int_rx_cb;
		int_e = int_e | int_enable(ctx->int_rx_avil);
		int_e = int_e | int_enable(ctx->int_rx_err);
		//io_i2s_rx_slave_ioctl(dev_id, IO_SET_CB_RX, &callback);
   }
   
	return (E_OK);
}

int32_t ss_dw_i2s_slv_close (DW_I2S_SLV_DEV_CONTEXT *ctx)
{
	uint32_t dev_id   = ctx->dev_id;
	uint8_t  io_mode  = ctx->io_mode;

   //Only RX implemented
	if (io_mode == 0)
   {
      io_i2s_rx_slave_close(dev_id);
   }   
   
	return (E_OK);
}

//Use local variable
uint32_t data_size_in_word;

int32_t ss_dw_i2s_slv_read (DW_I2S_SLV_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;

	if (arc_locked()) 
   {
		return E_SYS;
	}

   data_size_in_word = len;
	io_i2s_rx_slave_read(dev_id, data, &data_size_in_word);
   
	return (E_OK);
}

int32_t ss_dw_i2s_slv_write (DW_I2S_SLV_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
   //Only read supported
   return (E_SYS);
}

int32_t ss_dw_i2s_slv_control (DW_I2S_SLV_DEV_CONTEXT *ctx, uint32_t cmd, void *param)
{
	uint32_t dev_id = ctx->dev_id;

	switch (cmd)
   {
      case I2S_CMD_SET_RXCB:
         io_i2s_rx_slave_ioctl(dev_id, IO_SET_CB_RX, &param);
         break;

		case I2S_CMD_SET_ERRCB:
         io_i2s_rx_slave_ioctl(dev_id, IO_SET_CB_ERR, &param);
         break;

      case I2S_CMD_SET_RXINT:
         //still to do
         break;

      case I2S_CMD_ENA_DEV:
         //still to do
         break;

      case I2S_CMD_SLV_SET_SAMPLE_WIDTH:
         io_i2s_rx_slave_ioctl(dev_id, IO_I2S_RX_SLAVE_SET_SAMPLE_WIDTH, param);
         break;

      case I2S_CMD_SLV_SET_FIFO_THRS:
         io_i2s_rx_slave_ioctl(dev_id, IO_I2S_RX_SLAVE_SET_FIFO_THRESHOLD, param);
         break;
   }

	return (E_OK);
}

void ss_dw_i2s_slv_int_tx_cb (DW_I2S_SLV_DEV_CONTEXT *ctx, void *param)
{
	DEV_I2S_INFO_PTR info = ctx->info;
	info->i2s_cbs.tx_cb(info);
}

void ss_dw_i2s_slv_int_rx_cb (DW_I2S_SLV_DEV_CONTEXT *ctx, void *param)
{
	DEV_I2S_INFO_PTR info = ctx->info;
	info->i2s_cbs.rx_cb(info);
   
}

void ss_dw_i2s_slv_int_err_cb (DW_I2S_SLV_DEV_CONTEXT *ctx, void *param)
{
	DEV_I2S_INFO_PTR info = ctx->info;
   info->i2s_cbs.err_cb(info);
}
