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

#include <stdint.h>
#include <stdbool.h>
#include "arc_builtin.h"
#include "creg_hsdc.h"

void creg_hsdc_init_device(CREG_HSDC_STRUCT_PTR creg_regs)
{
}

uint32_t creg_hsdc_get_version (CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_VERSION_T v)
{
	uint32_t reg;

	reg = creg_regs->CREG_IP_VERSION;
	switch (v) {
	case CREG_HSDC_VERSION_DAY:
		reg &= VERSION_DAY;
		reg >>= VERSION_DAY_POS;
		break;

	case CREG_HSDC_VERSION_MONTH:
		reg &= VERSION_MONTH;
		reg >>= VERSION_MONTH_POS;
		break;

	case CREG_HSDC_VERSION_YEAR:
		reg &= VERSION_YEAR;
		reg >>= VERSION_YEAR_POS;
		break;

	case CREG_HSDC_VERSION_MIN:
		reg &= VERSION_TIME_MIN;
		reg >>= VERSION_TIME_MIN_POS;
		break;

	case CREG_HSDC_VERSION_HOUR:
		reg &= VERSION_TIME_HOUR;
		reg >>= VERSION_TIME_HOUR_POS;
		break;

	default:
		break;
	}

	return (reg);
}

void creg_hsdc_get_revision(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_REVISION_T *revision)
{
	*revision = (CREG_HSDC_REVISION_T)((creg_regs->CREG_IP_VERSION & VERSION_REF) >> VERSION_REF_POS);
}

/**
* Address Decoder Functions
*/
void creg_hsdc_set_slave(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_MASTER_T mst, CREG_HSDC_SLAVE_T slv, CREG_HSDC_APERTURE_T ap)
{
	uint32_t reg;
	volatile uint32_t *ptr;
	CREG_HSDC_APERTURE_T    c_ap;

	if (mst < CREG_HSDC_MASTER_NONE) {
		if (slv < CREG_HSDC_SLAVE_NONE) {

			//Point to first element
			ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

			//Calculate offset
			ptr += (mst*8);

			c_ap = ap;
			if (c_ap >= CREG_HSDC_APERTURE_8) {
				//take next register (aperture[8]..[15])
				ptr++;
				c_ap -= CREG_HSDC_APERTURE_8;
			}

			reg  = *ptr;                           //read
			reg &= ~(0x0FU << (4*(uint32_t)c_ap)); //clear
			reg |= (slv    << (4*(uint32_t)c_ap)); //set
			*ptr = reg;                            //write
			_arc_sync();
		}
	}
}

void creg_hsdc_get_slave(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_MASTER_T mst, CREG_HSDC_SLAVE_T *slv, CREG_HSDC_APERTURE_T ap)
{
	uint32_t reg;
	volatile uint32_t *ptr;
	CREG_HSDC_APERTURE_T c_ap;

	if (mst < CREG_HSDC_MASTER_NONE) {

		//Point to first element
		ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

		//Calculate offset
		ptr += (mst * 8);

		c_ap = ap;
		if (c_ap >= CREG_HSDC_APERTURE_8)
		{
			//take next register (aperture[8]..[15])
			ptr++;
			c_ap -= CREG_HSDC_APERTURE_8;
		}

		reg  = *ptr;                           //read
		reg &= (0x0FU << (4*(uint32_t)c_ap));  //mask
		reg  = (reg   >> (4*(uint32_t)c_ap));  //shift
		*slv = reg;                            //return
	}
}

void creg_hsdc_set_offset(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_MASTER_T mst, CREG_HSDC_OFFSET_T offset, CREG_HSDC_APERTURE_T ap)
{
	uint32_t reg;
	volatile uint32_t *ptr;
	CREG_HSDC_APERTURE_T c_ap;

	if (mst < CREG_HSDC_MASTER_NONE) {
		if (offset < CREG_HSDC_OFFSET_NONE) {

			//Point to first element
			ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

			//Calculate offset + 0x08
			ptr += (mst * 8) + 2;
			c_ap = ap;
			if (c_ap >= CREG_HSDC_APERTURE_8) {
				//take next register (aperture[8]..[15])
				ptr++;
				c_ap -= CREG_HSDC_APERTURE_8;
			}

			reg  = *ptr;                           //read
			reg &= ~(0x0FU << (4*(uint32_t)c_ap)); //clear
			reg |= (offset << (4*(uint32_t)c_ap)); //set
			*ptr = reg;                            //write
			_arc_sync();
		}
	}
}

void creg_hsdc_get_offset(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_MASTER_T mst, CREG_HSDC_OFFSET_T *offset, CREG_HSDC_APERTURE_T ap)
{
	uint32_t reg;
	volatile uint32_t *ptr;
	CREG_HSDC_APERTURE_T c_ap;

	if (mst < CREG_HSDC_MASTER_NONE) {

		//Point to first element
		ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

		//Calculate offset + 0x08
		ptr += (mst * 8) + 2;
		c_ap = ap;
		if (c_ap >= CREG_HSDC_APERTURE_8) {
			//take next register (aperture[8]..[15])
			ptr++;
			c_ap -= CREG_HSDC_APERTURE_8;
		}

		reg  = *ptr;
		reg &= (0x0FU << (4*(uint32_t)c_ap));  //mask
		reg  = (reg   >> (4*(uint32_t)c_ap));  //shift
		*offset = reg;                         //return
	}
}

void creg_hsdc_update(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_MASTER_T mst)
{
	volatile uint32_t *ptr;

	if (mst < CREG_HSDC_MASTER_NONE) {
		//Point to first element
		ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

		//Calculate offset + 0x14
		ptr += (mst*8) + 5;
		*ptr = A_UPDATE;
		_arc_sync();

		//wait some time
		_arc_nop();
		_arc_nop();
		_arc_nop();
		_arc_nop();
		_arc_nop();
		_arc_nop();
		_arc_nop();
		_arc_nop();
	}
}

void creg_hsdc_set_pae(CREG_HSDC_STRUCT_PTR creg_regs, uint32_t pae)
{
	volatile uint32_t       *ptr;

	//Point to first element
	ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

	//Calculate offset
	ptr += (CREG_HSDC_MASTER_NONE*8);
	*ptr = pae;
	_arc_sync();
}

void creg_hsdc_update_pae(CREG_HSDC_STRUCT_PTR creg_regs)
{
	volatile uint32_t *ptr;

	//Point to first element
	ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_SLV0;

	//Calculate offset + 0x14
	ptr += (CREG_HSDC_MASTER_NONE*8) + 5;
	*ptr = A_UPDATE;
	_arc_sync();

	//wait some time
	_arc_nop();
	_arc_nop();
	_arc_nop();
	_arc_nop();
	_arc_nop();
	_arc_nop();
	_arc_nop();
	_arc_nop();
}

/**
* General function for updating memory map
*/
volatile int creg_upd = 0;

void creg_hsdc_update_handler (void)
{
	creg_upd++;
	//DBG_print("@@ Creg-update\n\r");
}

int32_t creg_hsdc_program_aperture(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_APERTURECONFIG_T *config)
{
	if (config->mst < CREG_HSDC_MASTER_NONE) {
		int loop = 512;

		if (config->intnum != 0) {
			creg_upd = 0;
			//board_installHandler(creg_hsdc_updateHandler, config->intnum);
		}

		creg_hsdc_set_offset(creg_regs, config->mst, config->offset, config->aperture);
		creg_hsdc_set_slave (creg_regs, config->mst, config->slv, config->aperture);
		creg_hsdc_update(creg_regs, config->mst);

		if (config->intnum != 0) {
			//interrupt driven
			while ( (creg_upd == 0) && (loop > 0) ) {
				_arc_nop();
				loop--;
			}

			if (loop == 0) {
				return (1); //nok
			}
		} else {
			//polling driven
			while (loop > 0) {
				_arc_nop();
				loop--;
			}
		}

		return (0); //ok
	}

	return (1); //nok
}

/**
* Interrupt functions
*/
void creg_hsdc_clr_ena_irq(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_CORE_T core)
{
	if (core < CREG_HSDC_CORE_NONE) {
		//write only reg
		creg_regs->CREG_IRQ_CLR_ENABLE = (0x01U << core);
		_arc_sync();
	}
}

void creg_hsdc_set_ena_irq(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_CORE_T core)
{
	if (core < CREG_HSDC_CORE_NONE) {
		//write only reg (enable)
		creg_regs->CREG_IRQ_SET_ENABLE = (0x01U << core);
		_arc_sync();
	}
}

void creg_hsdc_set_sts_irq(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_CORE_T core)
{
	if (core < CREG_HSDC_CORE_NONE) {
		//write only reg (raise software ISR)
		creg_regs->CREG_IRQ_SET_STATUS = (0x01U << core);
		_arc_sync();
	}
}

uint32_t creg_hsdc_get_sts_irq(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_CORE_T core)
{
	uint32_t reg;

	if (core < CREG_HSDC_CORE_NONE) {

		//read only reg
		reg = creg_regs->CREG_IRQ_STATUS;
		if ( (reg & (0x01U << core)) == (0x01U << core) )
		{
			//pending
			return (1);
		}
	}

	return (0);
}

void creg_hsdc_clr_sts_irq(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_CORE_T core)
{
	if (core < CREG_HSDC_CORE_NONE) {
		//write only reg (in isr)
		creg_regs->CREG_IRQ_CLR_STATUS = (0x01U << core);
		_arc_sync();
	}
}

/**
* Tunnel functions
*/
uint32_t creg_hsdc_get_tunnel_sts(CREG_HSDC_STRUCT_PTR creg_regs)
{
	return (creg_regs->CREG_TUN_STAT);
}

void creg_hsdc_set_tunnel_priority(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_TUNNELPRIORITY_T pr)
{
	uint32_t reg;

	if (pr < CREG_HSDC_TUNNELPRIORITY_NONE) {
		reg = creg_regs->CREG_TUN_CTRL;
		reg &= ~(0x3U);
		reg |= pr;
		creg_regs->CREG_TUN_CTRL = reg;
		_arc_sync();
	}
}

void creg_hsdc_set_tunnel_pd_mode(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_TUNNELPDMODE_T pd)
{
	uint32_t reg;

	if (pd < CREG_HSDC_TUNNELPDMODE_NONE) {
		reg = creg_regs->CREG_TUN_CTRL;
		reg &= ~(0x10U);
		reg |= (pd<<4);
		creg_regs->CREG_TUN_CTRL = reg;
		_arc_sync();
	}
}

void creg_hsdc_set_tunnel_io_mode(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_TUNNELIOMODE_T iomode)
{
	uint32_t reg;
	uint32_t sr = 0;
	uint32_t odt = 0;

	if (iomode < CREG_HSDC_TUNNELIOMODE_NONE) {
		if (iomode == CREG_HSDC_TUNNELIOMODE_SSTL) {
			sr  = 0U;
			odt = 1U;
		} else if (iomode == CREG_HSDC_TUNNELIOMODE_LVCMOS) {
			sr  = 3U;
			odt = 0U;
		}

		reg = creg_regs->CREG_TUN_IO_CTRL;
		reg &= ~(0xF0000000U);
		reg |= (iomode<<31);
		reg |= (odt<<30);
		reg |= (sr<<28);
		creg_regs->CREG_TUN_IO_CTRL = reg;
		_arc_sync();
	}
}

/**
* Latency functions
*/
void creg_hsdc_set_ddr_latency(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_DDRHOSTPORT_T port, uint8_t latency)
{
	uint32_t reg;
	if (port < CREG_HSDC_DDRHOSTPORT_NONE) {
		reg = creg_regs->CREG_DDR_LATENCY;
		reg &= ~(0xFFU << (port*8));
		reg |= ((uint32_t)latency << (port*8));
		creg_regs->CREG_DDR_LATENCY = reg;
		_arc_sync();
	}
}

void creg_hsdc_set_sram_latency(CREG_HSDC_STRUCT_PTR creg_regs, uint8_t latency)
{
	uint32_t reg;

	reg = creg_regs->CREG_SRAM_LATENCY;
	reg &= ~(0xFFU);           //clr
	reg |=  (uint32_t)latency;       //set
	creg_regs->CREG_SRAM_LATENCY = reg;
	_arc_sync();
}

void creg_hsdc_set_spi_cs_ctrl(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_SPISEL_T sel, CREG_HSDC_SPICSSEL_T cs, CREG_HSDC_SPICSCTRL_T ctrl)
{
	uint32_t reg;

	if ( (sel < CREG_HSDC_SPISEL_NONE) && (cs < CREG_HSDC_SPICSSEL_NONE) ) {
		reg = creg_regs->CREG_SPI_CS_CTRL;
		reg &= ~(0x03U << ((sel*8)+(cs*2)));   //clr
		reg |=  (ctrl  << ((sel*8)+(cs*2)));   //set
		creg_regs->CREG_SPI_CS_CTRL = reg;
		_arc_sync();
	}
}

void creg_hsdc_set_i2s_clk_ctrl (CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_I2SCLKCTRL_T sel)
{
	if (sel < CREG_HSDC_I2SCLKCTRL_NONE) {
		creg_regs->CREG_I2S_CLK_CTRL = sel;
		_arc_sync();
	}
}

/**
* GPIO mux
*/
void creg_hsdc_set_gpio_mux(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_GPIOSEL_T sel, CREG_HSDC_GPIOMUX_T mux)
{
	uint32_t reg;

	if (sel < CREG_HSDC_GPIOSEL_NONE) {
		reg = creg_regs->CREG_GPIO_MUX;
		reg &= ~(0x07U << (sel*3)); //clr
		reg |=  (mux   << (sel*3)); //set
		creg_regs->CREG_GPIO_MUX = reg;
		_arc_sync();
	}
}

void creg_hsdc_set_dbg_mux(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_DBGSEL_T sel, CREG_HSDC_DBGMUX_T mux)
{
	uint32_t reg;

	if (sel < CREG_HSDC_DBGSEL_NONE) {
		reg = creg_regs->CREG_GPIO_DBG_MUX;
		reg &= ~(0x01U << sel); //clr
		reg |=  (mux   << sel); //set
		creg_regs->CREG_GPIO_DBG_MUX = reg;
		_arc_sync();
	}
}

/**
* DMA mux
*/
void creg_hsdc_set_dma_mux(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_DMAFCSEL_T sel, CREG_HSDC_DMAMUX_T mux)
{
	uint32_t reg;

	if (sel < CREG_HSDC_DMAFCSEL_NONE) {

		reg = creg_regs->CREG_DMA_MUX;
		reg &= ~(0x0FU << (sel*4)); //clr
		reg |= (mux    << (sel*4)); //set
		creg_regs->CREG_DMA_MUX = reg;
		_arc_sync();
	}
}


/**
* ARC clock control
*/
void creg_hsdc_set_arc_clk_div(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_ARCCLKDIV_T div)
{
	creg_regs->CREG_ARC_CLK_CTRL = div;
	_arc_sync();
}

CREG_HSDC_ARCCLKDIV_T creg_hsdc_get_arc_clk_div(CREG_HSDC_STRUCT_PTR creg_regs)
{
	return (CREG_HSDC_ARCCLKDIV_T)(creg_regs->CREG_ARC_CLK_CTRL);
}

/**
* ARC start
*/
void creg_hsdc_start_core(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_START_T *start)
{
	uint32_t reg;

	reg = creg_regs->CREG_CPU_START;
	reg &= ~(START_MASK | START_MODE_MASK | POLARITY_MASK | MULTI_CORE_MASK);
	reg |= ((start->start << START_POS) |
		(start->start_mode << START_MODE_POS) |
		(start->polarity << POLARITY_POS) |
		(start->multicoremode   << MULTI_CORE_POS));
	creg_regs->CREG_CPU_START = reg;
	_arc_sync();
}

//for Wdt test
void creg_hsdc_set_start_mode (CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_STARTMODE_T mode)
{
	uint32_t reg;

	reg = creg_regs->CREG_CPU_START;
	reg &= ~(START_MODE_MASK);
	reg |= (mode << START_MODE_POS);
	creg_regs->CREG_CPU_START = reg;
	_arc_sync();
}

void creg_hsdc_get_debug_uart_mode(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_DEBUGUARTMODE_T *mode)
{
	*mode = (CREG_HSDC_DEBUGUARTMODE_T)((creg_regs->CREG_CPU_START & DEBUG_UART_MODE_MASK) >> DEBUG_UART_MODE_POS);
}

void creg_hsdc_get_multicore_mode(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_MULTICOREMODE_T *mode)
{
	*mode = (CREG_HSDC_MULTICOREMODE_T)((creg_regs->CREG_CPU_START & MULTI_CORE_MASK) >> MULTI_CORE_POS);
}

/**
* DDR
*/
void creg_hsdc_set_ddr_enable(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_DDRCTRL_T ctrl, uint32_t enable)
{
	uint32_t reg;

	if (ctrl < CREG_HSDC_DDRCTRL_NONE) {

		reg = creg_regs->CREG_DDR_CTRL;
		reg &= ~(0x01U << ctrl);            //clr
		reg |= ((enable & 0x01U) << ctrl);  //set
		creg_regs->CREG_DDR_CTRL = reg;
		_arc_sync();
	}
}

void creg_hsdc_wait_ddr_init_done (CREG_HSDC_STRUCT_PTR creg_regs)
{
	uint32_t reg;

	//wait till DDR init sequence is completed 0->1
	reg = creg_regs->CREG_DDR_STAT;
	while ((reg & 0x01U) == 0x00U) {
		reg = creg_regs->CREG_DDR_STAT;
	}
}

/**
* Boot Register
*/
void creg_hsdc_get_boot_mirror (CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_BOOTMIRROR_T *mirror)
{
	*mirror = (CREG_HSDC_BOOTMIRROR_T)((creg_regs->CREG_AXI_M_0_BOOT & 0x03U) >> 0);
}

void creg_hsdc_set_boot_mirror(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_BOOTMIRROR_T mirror)
{
	uint32_t reg;
	volatile uint32_t *ptr;
	ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_BOOT;
	reg  = *ptr;          //read
	reg &= 0x3FC;         //clear
	reg |= (mirror << 0); //set
	*ptr = reg;           //write
	_arc_sync();
}

void creg_hsdc_get_image_location(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_IMAGELOCATION_T *location)
{
	*location = (CREG_HSDC_IMAGELOCATION_T)((creg_regs->CREG_AXI_M_0_BOOT & 0x30U) >> 4);
}

void creg_hsdc_get_aux(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_AUX_T *aux)
{
#ifdef FPGA
	*aux = CREG_HSDC_AUX_DDR_FPGA_PUBM;
#else
	*aux = (CREG_HSDC_AUX_T)((creg_regs->CREG_AXI_M_0_BOOT & 0x3C0U) >> 6);
#endif
}

void creg_hsdc_set_aux(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_AUX_T aux)
{
	uint32_t reg;
	volatile uint32_t *ptr;

	ptr = (volatile uint32_t *)&creg_regs->CREG_AXI_M_0_BOOT;
	reg  = *ptr;        //read
	reg &= 0x3F;        //clear
	reg |= (aux << 6);  //set
	*ptr = reg;         //write
	_arc_sync();
}

void creg_hsdc_sw_reset(CREG_HSDC_STRUCT_PTR creg_regs)
{
	uint32_t reg;

	//reset
	creg_regs->CREG_IP_SW_RESET = RESET_MASK;
	_arc_sync();

	//wait till bit is cleared
	reg = creg_regs->CREG_IP_SW_RESET;
	while ( (reg & RESET_MASK) == RESET_MASK) {
		reg = creg_regs->CREG_IP_SW_RESET;
	}
}

void creg_hsdc_set_kernel_entrypoint(CREG_HSDC_STRUCT_PTR creg_regs, CREG_HSDC_CORE_T core, uint32_t address)
{
	volatile uint32_t *ptr;

	if (core < CREG_HSDC_CORE_NONE) {
		//Point to first element
		ptr = (volatile uint32_t *)&creg_regs->CREG_CPU_0_ENTRY;

		//Calculate offset (0x0, 0x4, 0x8 or 0xC)
		ptr += core;
		*ptr = address;
		_arc_sync();
	}
}