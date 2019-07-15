#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "emsdp_hardware.h"
//dfss slyle of driver
#include "ip/designware/i2s_slave/i2s_rx_slave.h"
#include "ip/designware/i2s_slave/ss_i2s_slave.h"
#include "dw_i2s_slv_obj.h"

void dw_i2s_slv_all_install(void);

#if (USE_DW_I2S_SLV_0)
   static void dw_i2s_slv_0_tx_cb0  (void *param);
   static void dw_i2s_slv_0_rx_cb0  (void *param);
   static void dw_i2s_slv_0_err_cb0 (void *param);

   static DEV_I2S	dw_i2s_slv_0;						/*!< DW I2S SLV object */

   DW_I2S_SLV_DEV_CONTEXT dw_i2s_slv_context0 = 
   {
   	.reg_base      = EMSDP_I2S_BASE,

   	.dev_id        = 0,
   	.io_mode       = 0,

   	.int_tx_req    = 0,
   	.int_tx_err    = 0,
   	.int_rx_avil   = EMSDP_I2S_RX_DA3_INTR,
   	.int_rx_err    = EMSDP_I2S_RX_OR3_INTR,

   	.int_tx_cb     = dw_i2s_slv_0_tx_cb0,
   	.int_rx_cb     = dw_i2s_slv_0_rx_cb0,
   	.int_err_cb    = dw_i2s_slv_0_err_cb0,
   };

   static void dfss_i2s_0_isr(void *ptr);

   static int32_t dw_i2s_slv_0_open (uint32_t mode, uint32_t param)
   {
   	return ss_dw_i2s_slv_open(&dw_i2s_slv_context0, mode, param);
   }

   static int32_t dw_i2s_slv_0_close (void)
   {
   	return ss_dw_i2s_slv_close(&dw_i2s_slv_context0);
   }

   static int32_t dw_i2s_slv_0_control (uint32_t ctrl_cmd, void *param)
   {
   	return ss_dw_i2s_slv_control(&dw_i2s_slv_context0, ctrl_cmd, param);
   }

   static int32_t dw_i2s_slv_0_read (void *data, uint32_t len)
   {
   	return ss_dw_i2s_slv_read(&dw_i2s_slv_context0, (void *)data, len);
   }

   static int32_t dw_i2s_slv_0_write (const void *data, uint32_t len)
   {
   	return ss_dw_i2s_slv_write(&dw_i2s_slv_context0, (void *)data, len);
   }

   static void dw_i2s_slv_0_tx_cb0(void *param)
   {
   	ss_dw_i2s_slv_int_tx_cb(&dw_i2s_slv_context0, param);
   }

   static void dw_i2s_slv_0_rx_cb0(void *param)
   {
      ss_dw_i2s_slv_int_rx_cb(&dw_i2s_slv_context0, param);
   }

   static void dw_i2s_slv_0_err_cb0(void *param)
   {
      ss_dw_i2s_slv_int_err_cb(&dw_i2s_slv_context0, param);
   }

   static void dw_i2s_slv_0_install(void)
   {
   	DEV_I2S_PTR         dw_i2s_slv_ptr = &dw_i2s_slv_0;
   	DEV_I2S_INFO_PTR    dw_i2s_slv_info_ptr = &(dw_i2s_slv_0.i2s_info);

   	dw_i2s_slv_context0.info = dw_i2s_slv_info_ptr;

   	dw_i2s_slv_info_ptr->opn_cnt     = 0;
   	dw_i2s_slv_info_ptr->status      = DEV_DISABLED;
   	dw_i2s_slv_info_ptr->device      = I2S_DEVICE_RECEIVER;
   	dw_i2s_slv_info_ptr->mode        = DEV_SLAVE_MODE;
   	dw_i2s_slv_info_ptr->cur_state   = I2S_FREE;
   	dw_i2s_slv_info_ptr->err_state   = I2S_ERR_NONE;

   	dw_i2s_slv_ptr->i2s_open      = dw_i2s_slv_0_open;
      dw_i2s_slv_ptr->i2s_close     = dw_i2s_slv_0_close;
   	dw_i2s_slv_ptr->i2s_control   = dw_i2s_slv_0_control;
   	dw_i2s_slv_ptr->i2s_write     = dw_i2s_slv_0_write;
   	dw_i2s_slv_ptr->i2s_read      = dw_i2s_slv_0_read;
   }
#endif /* USE_DW_I2S_SLV_0 */


DEV_I2S_PTR dw_i2s_slv_get_dev(int32_t i2s_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dw_i2s_slv_all_install();
	}

	switch (i2s_id) 
   {
   #if (USE_DW_I2S_SLV_0)
		case 0:
			return &dw_i2s_slv_0;
			break;
   #endif

		default:
			break;
	}
	return NULL;
}

void dw_i2s_slv_all_install(void)
{
#if (USE_DW_I2S_SLV_0)
	dw_i2s_slv_0_install();
#endif
}



