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
#include "axs_103_interrupt.h"


typedef volatile struct dw_ictl_reg {
	uint32_t ENABLE_L;
	uint32_t ENABLE_H;
	uint32_t MASK_L;
	uint32_t MASK_H;
	uint32_t FORCE_L;
	uint32_t FORCE_H;
	uint32_t RAWSTATUS_L;
	uint32_t RAWSTATUS_H;
	uint32_t STATUS_L;
	uint32_t STATUS_H;
	uint32_t MASKSTATUS_L;
	uint32_t MASKSTATUS_H;
	uint32_t FINALSTATUS_L;
	uint32_t FINALSTATUS_H;
	/* more regs to be added according to dw_apb_ictl databook */
} DW_ICTL_REG, *DW_ICTL_REG_PTR;

typedef volatile struct ictl_gpio_reg {
	uint32_t reserved[12];
	uint32_t INTEN;		/*!< (0x30) */
	uint32_t INTMASK;	/*!< (0x34) */
	uint32_t INTTYPE_LEVEL;	/*!< (0x38) */
	uint32_t INT_POLARITY;	/*!< (0x3c) */
	uint32_t INTSTATUS;	/*!< (0x40) */
	uint32_t RAW_INTSTATUS;	/*!< (0x44) */
	uint32_t DEBOUNCE;	/*!< (0x48) */
	uint32_t PORTA_EOI;	/*!< (0x4c) */
} ICTL_GPIO_REG, *ICTL_GPIO_REG_PTR;


void dw_ictl_int_enable(DW_ICTL_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->ENABLE_L;
	val |= (1 << intno);
	reg->ENABLE_L = val;
}

void dw_ictl_int_disable(DW_ICTL_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->ENABLE_L;
	val &= ~(1 << intno);
	reg->ENABLE_L = val;
}

uint32_t dw_ictl_int_enabled(DW_ICTL_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->ENABLE_L;
	if (val & (1 << intno)) {
		return 1;
	}

	return 0;
}

uint32_t dw_ictl_int_probe(DW_ICTL_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->FINALSTATUS_L;

	if (val & (1 << intno)) {
		return 1;
	}

	return 0;

}

void dw_ictl_int_sw_trigger(DW_ICTL_REG_PTR reg, uint32_t intno)
{
	reg->FORCE_L = (1 << intno);
}


void ictl_gpio_int_enable(ICTL_GPIO_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->INTEN;
	val |= (1 << intno);
	reg->INTEN = val;
}

void ictl_gpio_int_disable(ICTL_GPIO_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->INTEN;
	val &= ~(1 << intno);
	reg->INTEN = val;
}

uint32_t ictl_gpio_int_enabled(ICTL_GPIO_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->INTEN;
	if (val & (1 << intno)) {
		return 1;
	}

	return 0;
}

uint32_t ictl_gpio_int_probe(ICTL_GPIO_REG_PTR reg, uint32_t intno)
{
	uint32_t val;

	val = reg->INTSTATUS;

	if (val & (1 << intno)) {
		return 1;
	}

	return 0;
}

void ictl_gpio_int_level_config(ICTL_GPIO_REG_PTR reg, uint32_t intno, uint32_t level)
{
	uint32_t val;
	val = reg->INTTYPE_LEVEL;
	val &= ~(1 << intno);
	val |= (level << intno);
	reg->INTTYPE_LEVEL = val;
}

void ictl_gpio_int_polarity_config(ICTL_GPIO_REG_PTR reg, uint32_t intno, uint32_t polarity)
{
	uint32_t val;
	val = reg->INT_POLARITY;
	val &= ~(1 << intno);
	val |= (polarity << intno);
	reg->INT_POLARITY = val;
}

void ictl_gpio_int_clear(ICTL_GPIO_REG_PTR reg, uint32_t intno)
{
	reg->PORTA_EOI |= (1 << intno);
}

static void axs_103_int_handler_default(void *ptr)
{
	ptr = ptr;
}


static ICTL_GPIO_REG_PTR ictl_gpio_ptr = (ICTL_GPIO_REG_PTR)(REGBASE_ICTL_CPU);
static DW_ICTL_REG_PTR dw_ictl_ptr = (DW_ICTL_REG_PTR)(REGBASE_ICTL);
static INT_HANDLER axs_103_int_handler_table[NUM_INT_ALL] = {
	[0 ... NUM_INT_ALL-1] = axs_103_int_handler_default
};


static void dw_ictl_int_isr(void *ptr)
{
	uint32_t status;
	uint32_t i;

	status = dw_ictl_ptr->FINALSTATUS_L;

	for (i=0; i < ICTL_INTNO_NUM ; i ++) {
		if (status & (1 << i)) {
			axs_103_int_handler_table[i]((void *)i);
		}
	}
}

static void ictl_gpio_int_isr(void *ptr)
{
	uint32_t status;
	uint32_t i;

	status = ictl_gpio_ptr->INTSTATUS;

	for (i=0; i < ICTL_INTNO_NUM ; i ++) {
		if (status & (1 << i)) {
			axs_103_int_handler_table[i+ICTL_INTNO_NUM]((void *)i);
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) {
		/* arc interrupt controller */
		arc_int_disable(sub_intno);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		dw_ictl_int_disable(dw_ictl_ptr, sub_intno);
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		ictl_gpio_int_disable(ictl_gpio_ptr, sub_intno);
	} else {
		return -1;
	}

	return 0;
}

/**
 * \brief  enable the specific int
 *
 * \param[in] intno interrupt number
 */
int32_t int_enable(const uint32_t intno)
{
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) {
	/* arc interrupt controller */
		arc_int_enable(sub_intno);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		dw_ictl_int_enable(dw_ictl_ptr, sub_intno);
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		ictl_gpio_int_enable(ictl_gpio_ptr, sub_intno);
	} else {
		return -1;
	}

	return 0;
}

/**
 * \brief  check whether the specific int is enabled
 *
 * \param[in] intno interrupt number
 * \return 0 disabled, 1 enabled, < 0 error
 */
int32_t int_enabled(const uint32_t intno)
{
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) { /* arc interrupt controller */
		return arc_int_enabled(sub_intno);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		return dw_ictl_int_enabled(dw_ictl_ptr, sub_intno);
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		return ictl_gpio_int_enabled(ictl_gpio_ptr, sub_intno);
	}

	return -1;
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) {
		return (int32_t)arc_int_pri_get(sub_intno) + INT_PRI_MIN;
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) {
		return (int32_t)arc_int_pri_get(INTNO_ICTL_MB) + INT_PRI_MIN;
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) {
		return (int32_t)arc_int_pri_get(INTNO_ICTL_CPU) + INT_PRI_MIN;
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	intpri = intpri - INT_PRI_MIN;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) {
		arc_int_pri_set(sub_intno, (uint32_t)intpri);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) {
		arc_int_pri_set(INTNO_ICTL_MB, (uint32_t)intpri);
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) {
		arc_int_pri_set(INTNO_ICTL_CPU, (uint32_t)intpri);
	} else {
		return -1;
	}

	return 0;
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) { /* arc interrupt controller */
		return arc_int_probe(sub_intno);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) {
		return dw_ictl_int_probe(dw_ictl_ptr, sub_intno);
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) {
		return ictl_gpio_int_probe(ictl_gpio_ptr, sub_intno);
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) { /* arc interrupt controller */
		arc_int_sw_trigger(sub_intno);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		dw_ictl_int_sw_trigger(dw_ictl_ptr, sub_intno);
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		return -1;
	} else {
		return -1;
	}

	return 0;
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) { /* arc interrupt controller */
		arc_int_level_config(sub_intno, level);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		return -1;
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		ictl_gpio_int_level_config(ictl_gpio_ptr, sub_intno, level);
	} else {
		return -1;
	}

	return 0;
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) { /* arc interrupt controller */
		exc_handler_install(sub_intno, handler);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		axs_103_int_handler_table[sub_intno] = handler;
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		axs_103_int_handler_table[sub_intno + ICTL_INTNO_NUM] = handler;
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
	uint32_t ictl = intno >> 8;

	uint32_t sub_intno = intno & 0xff;

	if (ictl == 0 && sub_intno >= NUM_EXC_CPU && sub_intno < NUM_EXC_ALL) { /* arc interrupt controller */
		return exc_handler_get(sub_intno);
	} else if (ictl == INTNO_ICTL_MB && sub_intno <= ICTL_INTNO_NUM ) { /* main board ictl */
		return axs_103_int_handler_table[sub_intno];
	} else if (ictl == INTNO_ICTL_CPU && sub_intno <= CPU_ICTL_INTNO_NUM) { /* cpu card ictl */
		return axs_103_int_handler_table[sub_intno + ICTL_INTNO_NUM];
	}

	return NULL;
}


void axs_interrupt_init(void)
{

	DEV_GPIO_INT_CFG int_cfg;
	DEV_GPIO_BIT_ISR bit_isr;

	DEV_GPIO * port_ptr;

	port_ptr = gpio_get_dev(CPU_DW_GPIO_PORT_A);


	int_cfg.int_bit_mask = ICTL_GPIOA_LINE_MASK;
	int_cfg.int_bit_type = GPIO_INT_BITS_LEVEL_TRIG(ICTL_GPIOA_LINE_MASK);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_RISE_EDGE(ICTL_GPIOA_LINE_MASK);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_DIS_DEBOUNCE(ICTL_GPIOA_LINE_MASK);
	port_ptr->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));
	bit_isr.int_bit_ofs = ICTL_GPIOA_LINE;
	bit_isr.int_bit_handler = ictl_gpio_int_isr;
	port_ptr->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));
	port_ptr->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)ICTL_GPIOA_LINE_MASK);

	int_handler_install(INTNO_ICTL_CPU, dw_ictl_int_isr);
	int_enable(INTNO_ICTL_CPU);
}
