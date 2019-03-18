/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

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
#include "embARC.h"
#include "arc_connect.h"
#include "hsdk_interrupt.h"
#include "hsdk_hardware.h"


static void hsdk_int_handler_default(void *ptr)
{
	ptr = ptr;
}

static INT_HANDLER hsdk_int_handler_table[ARC_CONNECT_NUM_INT_ALL] = {
	[0 ... ARC_CONNECT_NUM_INT_ALL-1] = hsdk_int_handler_default
};

static void arc_connect_int_isr(void *ptr)
{
	uint32_t reg;
	uint32_t i;
	uint32_t core;

	core = (_arc_lr_reg(AUX_IDENTITY) >> 8) & 0xff;

	for (i=HSDC_CREG_SW0_ISR; i<HSDK_MAX_NUM_ISR; i++) {
		reg = arc_connect_idu_read_mask(i-HSDK_MAX_NUM_EXCP);
		if (reg & 0x1) { /* check whether the interrupt is masked */
			continue;
		}
		if ( (i == HSDC_CREG_UPDATE_ISR) || (i == HSDC_CGU_PLL_LOCK_ISR) \
			|| (i == HSDC_CGU_PLL_UNLOCK_ISR)  || (i == HSDC_CGU_PLL_LOCKERROR_ISR) \
			|| (i == HSDC_TUNNEL_ISR) ) {
			//Edge triggered (creg(update) + cgu(3x))
			reg = arc_connect_idu_check_status(i-HSDK_MAX_NUM_EXCP);
			if (reg & (1<<core)) {
				arc_connect_idu_ack_cirq(i-HSDK_MAX_NUM_EXCP);
				hsdk_int_handler_table[i-HSDK_MAX_NUM_EXCP]((void *)i);
			}
		} else {
			//Level triggered
			reg = arc_connect_idu_read_mode(i-HSDK_MAX_NUM_EXCP) & 0x3U;
			if (reg == 1) {
				//Level triggered IRQ
				reg = arc_connect_idu_check_first(i-HSDK_MAX_NUM_EXCP);
				if (reg == 1) {
					//Execute handler
					hsdk_int_handler_table[i-HSDK_MAX_NUM_EXCP]((void *)i);
				}
			}
		}
	}
}

/**
 * \brief disable the specific interrupt
 *
 * \param[in] intno interrupt number
 */
int32_t int_disable(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_disable(intno);
		if (intno >= HSDK_MAX_NUM_EXCP) {
			arc_connect_idu_set_mask(intno-HSDK_MAX_NUM_EXCP, 1);
		}
		return 0;
	}

	return -1;
}

/**
 * \brief  enable the specific int
 *
 * \param[in] intno interrupt number
 */
int32_t int_enable(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_enable(intno);
		if (intno >= HSDK_MAX_NUM_EXCP) {
			arc_connect_idu_set_mask(intno-HSDK_MAX_NUM_EXCP, 0);
		}
		return 0;
	}

	return -1;
}

/**
 * \brief  check whether the specific int is enabled
 *
 * \param[in] intno interrupt number
 * \return 0 disabled, 1 enabled, < 0 error
 */
int32_t int_enabled(const uint32_t intno)
{
	int32_t enabled = 0;
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		enabled = arc_int_enabled(intno);
		if ((enabled == 1) && (intno >= HSDK_MAX_NUM_EXCP)) {
			if (arc_connect_idu_read_mask(intno-HSDK_MAX_NUM_EXCP) == 1) {
				enabled = 0;
			}
		}
	}

	return enabled;
}

/**
 * \brief  get the interrupt priority mask
 *
 * \returns interrupt priority mask, negative num
 */
int32_t int_ipm_get(void)
{
	return ((int32_t)arc_int_ipm_get() + INT_PRI_MIN);
}


/**
 * \brief  set the interrupt priority mask
 *
 * \param[in] intpri interrupt priority
 */
int32_t int_ipm_set(int32_t intpri)
{
	if (intpri >= INT_PRI_MIN && intpri <= INT_PRI_MAX) {
		intpri = intpri - INT_PRI_MIN;
		arc_int_ipm_set(intpri);
		return 0;
	}

	return  -1;
}


/**
 * \brief  get current interrupt priority mask
 *
 * \param[in] intno interrupt number
 * \return  <0 interrupt priority, 0 error
 */
int32_t int_pri_get(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return (int32_t)arc_int_pri_get(intno) + INT_PRI_MIN;
	}

	return 0;
}

/**
 * \brief set interrupt priority
 *
 * \param[in] intno interrupt number
 * \param[in] intpri interrupt priority
 */
int32_t int_pri_set(const uint32_t intno, int32_t intpri)
{
	uint32_t status;

	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		status = cpu_lock_save();
		intpri = intpri - INT_PRI_MIN;
		arc_int_pri_set(intno,(uint32_t)intpri);
		cpu_unlock_restore(status);
		return 0;
	}
	return -1;
}

/**
 * \brief  probe the pending status of interrupt
 *
 * \param[in] intno interrupt number
 *
 * \returns 1 pending, 0 no pending, -1 error
 */
int32_t int_probe(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return arc_int_probe(intno);
	}
	return -1;
}


/**
 * \brief  trigger the interrupt in software
 *
 * \param[in] intno interrupt number
 * \return 0 ok, -1 error
 */
int32_t int_sw_trigger(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		if (intno >= HSDK_MAX_NUM_EXCP) {
			arc_connect_idu_gen_cirq(intno-HSDK_MAX_NUM_EXCP);
		} else {
			arc_int_sw_trigger(intno);
		}
		return 0;
	}
	return -1;
}

/**
 * \brief  config the interrupt level triggered or pulse triggered
 *
 * \param[in] intno interrupt number
 * \param[in] level, 0-level trigger, 1-pluse triggered
 * \return 0 ok, -1 error
 */
int32_t int_level_config(const uint32_t intno, const uint32_t level)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		uint32_t mode = 0;
		arc_int_level_config(intno, level);
		if (intno >= HSDK_MAX_NUM_EXCP) {
			mode = arc_connect_idu_read_mode(intno-HSDK_MAX_NUM_EXCP);
			if (level == 0) {
				if (mode & (1<<4)) {
					arc_connect_idu_set_mode(intno-HSDK_MAX_NUM_EXCP, ARC_CONNECT_INTRPT_TRIGGER_LEVEL, mode & 0x3);
				}
			} else {
				if ((mode & (1<<4)) == 0) {
					arc_connect_idu_set_mode(intno-HSDK_MAX_NUM_EXCP, ARC_CONNECT_INTRPT_TRIGGER_EDGE, mode & 0x3);
				}
			}
		}
		return 0;
	}
	return -1;
}


/**
 * \brief  lock cpu, disable interrupts
 */
void cpu_lock(void)
{
	arc_lock();
}

/**
 * \brief  unlock cpu, enable interrupts to happen
 */
void cpu_unlock(void)
{
	arc_unlock();
}

/**
 * \brief  lock cpu and staus
 *
 * \returns cpu status
 */
uint32_t cpu_lock_save(void)
{
	return arc_lock_save();
}

/**
 * \brief  unlock cpu with the specific status
 *
 * \param[in] status  cpu status saved by cpu_lock_save
 */
void cpu_unlock_restore(const uint32_t status)
{
	arc_unlock_restore(status);
}

/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  install an interrupt handler
 * \param[in] intno	interrupt number
 * \param[in] handler interrupt handler to install
 */
int32_t int_handler_install(const uint32_t intno, INT_HANDLER handler)
{
	if (intno >= NUM_EXC_CPU) {
		if (intno >= HSDK_MAX_NUM_EXCP) {
			exc_handler_install(intno, arc_connect_int_isr);
			hsdk_int_handler_table[intno-HSDK_MAX_NUM_EXCP] = handler;
		} else {
			exc_handler_install(intno, handler);
		}
	} else {
		return -1;
	}

	return 0;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  get the installed an interrupt handler
 * \param[in] intno interrupt number
 * \return the installed interrupt handler or NULL
 */
INT_HANDLER int_handler_get(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU) {
		if (intno >= HSDK_MAX_NUM_EXCP) {
			return hsdk_int_handler_table[intno-HSDK_MAX_NUM_EXCP];
		} else {
			return exc_handler_get(intno);
		}
	} else {
		return NULL;
	}
}

void hsdk_interrupt_init(void)
{
	uint32_t i;
	uint32_t core;

	core = (_arc_lr_reg(AUX_IDENTITY) >> 8) & 0xff;

	for (i = HSDK_MAX_NUM_EXCP; i < HSDK_MAX_NUM_ISR; i ++) {
		if ( (i == HSDC_CREG_UPDATE_ISR) || (i == HSDC_CGU_PLL_LOCK_ISR) \
			|| (i == HSDC_CGU_PLL_UNLOCK_ISR)  || (i == HSDC_CGU_PLL_LOCKERROR_ISR) \
			|| (i == HSDC_TUNNEL_ISR) ) {
			arc_connect_init_isr(1<<core, i-HSDK_MAX_NUM_EXCP, ARC_CONNECT_INTRPT_TRIGGER_EDGE, ARC_CONNECT_DISTRI_MODE_FIRST_ACK);
		} else {
			arc_connect_init_isr(1<<core, i-HSDK_MAX_NUM_EXCP, ARC_CONNECT_INTRPT_TRIGGER_LEVEL, ARC_CONNECT_DISTRI_MODE_FIRST_ACK);
		}
	}
}
