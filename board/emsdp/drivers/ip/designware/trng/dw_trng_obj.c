/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
#include "embARC_toolchain.h"
#include "dw_trng_obj.h"
#include "emsdp/emsdp.h"


#if (USE_DW_TRNG_0)
#define DW_TRNG_0_BASE	(EMSDP_TRNG_BASE)
#define DW_TRNG_0_INTNO	(EMSDP_NIST_TRNG_INTR)

static DEV_TRNG			dw_trng_0;			/*!< designware trng object */
static DW_TRNG_CTRL		dw_trng_0_ctrl;		/*!< designware trng 0 ctrl */

static int32_t dw_trng_0_open(void){
	return dw_trng_open(&dw_trng_0);
}

static int32_t dw_trng_0_close(void){
	return dw_trng_close(&dw_trng_0);
}

static int32_t dw_trng_0_control(uint32_t ctrl_cmd, void *param){
	return dw_trng_control(&dw_trng_0, ctrl_cmd, param);
}

static int32_t dw_trng_0_read(uint32_t *data_buf){
	return dw_trng_read(&dw_trng_0, data_buf);
}

static void dw_trng_0_isr(void *ptr)
{
	dw_trng_isr(&dw_trng_0, ptr);
}

static void dw_trng_0_install(void){
	uint32_t trng_abs_base = 0;
	DEV_TRNG_PTR dw_trng_ptr = &dw_trng_0;
	DEV_TRNG_INFO_PTR dw_trng_info_ptr = &dw_trng_0.trng_info;
	DW_TRNG_CTRL_PTR dw_trng_ctrl_ptr = &dw_trng_0_ctrl;
	DW_TRNG_REG_PTR dw_trng_reg_ptr;

	/**
	 * get absolute designware base address
	 */
	trng_abs_base = (uint32_t)DW_TRNG_0_BASE;
	dw_trng_reg_ptr = (DW_TRNG_REG_PTR)trng_abs_base;

	/** trng info init */
	dw_trng_info_ptr->trng_ctrl = (void *)dw_trng_ctrl_ptr;
	dw_trng_info_ptr->opn_cnt = 0;
	dw_trng_info_ptr->length = 4;//default output length is 4 bytes
	dw_trng_info_ptr->byte_generated = 0;

	/* trng ctrl init */
	/* Variables which should be set during object implementation */
	dw_trng_ctrl_ptr->dw_trng_regs = dw_trng_reg_ptr;
	dw_trng_ctrl_ptr->intno = DW_TRNG_0_INTNO;
	dw_trng_ctrl_ptr->dw_trng_int_handler = dw_trng_0_isr;
	/* Variables which always change during trng operation */
	dw_trng_ctrl_ptr->int_status = 0;

	/** trng dev init */
	dw_trng_ptr->trng_open = dw_trng_0_open;
	dw_trng_ptr->trng_close = dw_trng_0_close;
	dw_trng_ptr->trng_control = dw_trng_0_control;
	dw_trng_ptr->trng_read = dw_trng_0_read;

}
#endif	/* USE_DW_TRNG_0 */

/** get one designware device structure */
DEV_TRNG_PTR dw_trng_get_dev(int32_t trng_id)
{
	static uint32_t install_flag = 0;

	/* install device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dw_trng_all_install();
	}

	switch (trng_id) {
#if (USE_DW_TRNG_0)
		case DW_TRNG_0_ID:
			return &dw_trng_0;
			break;
#endif
		default:
			break;
	}
	return NULL;
}

/**
 * \brief	install all trng objects
 * \note	\b MUST be called during system init
 */
void dw_trng_all_install(void)
{
#if (USE_DW_TRNG_0)
	dw_trng_0_install();
#endif
}
