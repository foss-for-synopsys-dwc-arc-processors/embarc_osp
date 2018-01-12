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

// ----------------------------------------------
// !!! This file was automatically generated !!!
// ----------------------------------------------
#ifndef _CREG_HSDC_REG_MAP_H_
#define _CREG_HSDC_REG_MAP_H_
typedef volatile struct creg_hsdc_reg
{
	uint32_t 	CREG_AXI_M_0_SLV0;  	// RW	0x000	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_0_SLV1;  	// RW	0x004	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_0_OFFSET0;  	// RW	0x008	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_0_OFFSET1;  	// RW	0x00C	Address decoder slave offset register 1
	uint32_t 	CREG_AXI_M_0_BOOT;  	// RW	0x010	Address decoder boot register
	uint32_t 	CREG_AXI_M_0_UPDATE;  	// RW1C	0x014	Address decoder update register
	uint32_t 	reserved1[2];
	uint32_t 	CREG_AXI_M_1_SLV0;  	// RW	0x020	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_1_SLV1;  	// RW	0x024	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_1_OFFSET0;  	// RW	0x028	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_1_OFFSET1;  	// RW	0x02C	Address decoder slave offset register 1
	uint32_t 	reserved2[1];
	uint32_t 	CREG_AXI_M_1_UPDATE;  	// RW1C	0x034	Address decoder update register
	uint32_t 	reserved3[2];
	uint32_t 	CREG_AXI_M_2_SLV0;  	// RW	0x040	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_2_SLV1;  	// RW	0x044	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_2_OFFSET0;  	// RW	0x048	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_2_OFFSET1;  	// RW	0x04C	Address decoder slave offset register 1
	uint32_t 	reserved4[1];
	uint32_t 	CREG_AXI_M_2_UPDATE;  	// RW1C	0x054	Address decoder update register
	uint32_t 	reserved5[2];
	uint32_t 	CREG_AXI_M_3_SLV0;  	// RW	0x060	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_3_SLV1;  	// RW	0x064	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_3_OFFSET0;  	// RW	0x068	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_3_OFFSET1;  	// RW	0x06C	Address decoder slave offset register 1
	uint32_t 	reserved6[1];
	uint32_t 	CREG_AXI_M_3_UPDATE;  	// RW1C	0x074	Address decoder update register
	uint32_t 	reserved7[2];
	uint32_t 	CREG_AXI_M_4_SLV0;  	// RW	0x080	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_4_SLV1;  	// RW	0x084	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_4_OFFSET0;  	// RW	0x088	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_4_OFFSET1;  	// RW	0x08C	Address decoder slave offset register 1
	uint32_t 	reserved8[1];
	uint32_t 	CREG_AXI_M_4_UPDATE;  	// RW1C	0x094	Address decoder update register
	uint32_t 	reserved9[2];
	uint32_t 	CREG_AXI_M_5_SLV0;  	// RW	0x0A0	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_5_SLV1;  	// RW	0x0A4	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_5_OFFSET0;  	// RW	0x0A8	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_5_OFFSET1;  	// RW	0x0AC	Address decoder slave offset register 1
	uint32_t 	reserved10[1];
	uint32_t 	CREG_AXI_M_5_UPDATE;  	// RW1C	0x0B4	Address decoder update register
	uint32_t 	reserved11[2];
	uint32_t 	CREG_AXI_M_6_SLV0;  	// RW	0x0C0	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_6_SLV1;  	// RW	0x0C4	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_6_OFFSET0;  	// RW	0x0C8	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_6_OFFSET1;  	// RW	0x0CC	Address decoder slave offset register 1
	uint32_t 	reserved12[1];
	uint32_t 	CREG_AXI_M_6_UPDATE;  	// RW1C	0x0D4	Address decoder update register
	uint32_t 	reserved13[2];
	uint32_t 	CREG_AXI_M_7_SLV0;  	// RW	0x0E0	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_7_SLV1;  	// RW	0x0E4	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_7_OFFSET0;  	// RW	0x0E8	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_7_OFFSET1;  	// RW	0x0EC	Address decoder slave offset register 1
	uint32_t 	reserved14[1];
	uint32_t 	CREG_AXI_M_7_UPDATE;  	// RW1C	0x0F4	Address decoder update register
	uint32_t 	reserved15[2];
	uint32_t 	CREG_AXI_M_8_SLV0;  	// RW	0x100	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_8_SLV1;  	// RW	0x104	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_8_OFFSET0;  	// RW	0x108	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_8_OFFSET1;  	// RW	0x10C	Address decoder slave offset register 1
	uint32_t 	reserved16[1];
	uint32_t 	CREG_AXI_M_8_UPDATE;  	// RW1C	0x114	Address decoder update register
	uint32_t 	reserved17[2];
	uint32_t 	CREG_AXI_M_9_SLV0;  	// RW	0x120	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_9_SLV1;  	// RW	0x124	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_9_OFFSET0;  	// RW	0x128	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_9_OFFSET1;  	// RW	0x12C	Address decoder slave offset register 1
	uint32_t 	reserved18[1];
	uint32_t 	CREG_AXI_M_9_UPDATE;  	// RW1C	0x134	Address decoder update register
	uint32_t 	reserved19[2];
	uint32_t 	CREG_AXI_M_10_SLV0;  	// RW	0x140	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_10_SLV1;  	// RW	0x144	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_10_OFFSET0;  	// RW	0x148	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_10_OFFSET1;  	// RW	0x14C	Address decoder slave offset register 1
	uint32_t 	reserved20[1];
	uint32_t 	CREG_AXI_M_10_UPDATE;  	// RW1C	0x154	Address decoder update register
	uint32_t 	reserved21[2];
	uint32_t 	CREG_AXI_M_11_SLV0;  	// RW	0x160	Address decoder slave select register 0
	uint32_t 	CREG_AXI_M_11_SLV1;  	// RW	0x164	Address decoder slave select register 1
	uint32_t 	CREG_AXI_M_11_OFFSET0;  	// RW	0x168	Address decoder slave offset register 0
	uint32_t 	CREG_AXI_M_11_OFFSET1;  	// RW	0x16C	Address decoder slave offset register 1
	uint32_t 	reserved22[1];
	uint32_t 	CREG_AXI_M_11_UPDATE;  	// RW1C	0x174	Address decoder update register
	uint32_t 	reserved23[2];
	uint32_t 	CREG_PAE;  	// RW	0x180	PAE register
	uint32_t 	reserved24[4];
	uint32_t 	CREG_PAE_UPDATE;  	// RW1C	0x194	PAE update register
	uint32_t 	reserved25[26];
	uint32_t 	CREG_DDR_LATENCY;  	// RW	0x200	Latency register for DDR
	uint32_t 	CREG_SRAM_LATENCY;  	// RW	0x204	Latency register for SRAM
	uint32_t 	reserved26[126];
	uint32_t 	CREG_CPU_START;  	// RW	0x400	CPU start register
	uint32_t 	CREG_CPU_0_ENTRY;  	// RW	0x404	CPU-0 kernel entry point register
	uint32_t 	CREG_CPU_1_ENTRY;  	// RW	0x408	CPU-1 kernel entry point register
	uint32_t 	CREG_CPU_2_ENTRY;  	// RW	0x40C	CPU-2 kernel entry point register
	uint32_t 	CREG_CPU_3_ENTRY;  	// RW	0x410	CPU-3 kernel entry point register
	uint32_t 	reserved27[27];
	uint32_t 	CREG_GPIO_DBG_MUX;  	// RW	0x480	GPIO debug mux register
	uint32_t 	CREG_GPIO_MUX;  	// RW	0x484	GPIO mux register
	uint32_t 	CREG_DMA_MUX;  	// RW	0x488	DMA mux register
	uint32_t 	reserved28[1];
	uint32_t 	CREG_TUN_IO_CTRL;  	// RW	0x490	IO control register for AXI tunnel
	uint32_t 	reserved29[3];
	uint32_t 	CREG_TUN_CTRL;  	// RW	0x4A0	AXI tunnel control register
	uint32_t 	CREG_TUN_STAT;  	// R	0x4A4	AXI tunnel status register
	uint32_t 	CREG_DDR_CTRL;  	// RW	0x4A8	DDR control register
	uint32_t 	CREG_DDR_STAT;  	// R	0x4AC	DDR status register
	uint32_t 	CREG_SPI_CS_CTRL;  	// RW	0x4B0	SPI chip-select control register
	uint32_t 	CREG_GPIO_CLK_CTRL;  	// RW	0x4B4	GPIO clock control register
	uint32_t 	CREG_ARC_CLK_CTRL;  	// RW	0x4B8	ARC clock control register
	uint32_t 	CREG_I2S_CLK_CTRL;  	// RW	0x4BC	I2S clock control register
	uint32_t 	CREG_RTT_RST_CTRL;  	// RW	0x4C0	RTT reset control register
	uint32_t 	reserved30[13];
	uint32_t 	CREG_DBG_SCRATCH;  	// RW	0x4F8	Debug scratch register
	uint32_t 	CREG_DBG_PRINT;  	// RW	0x4FC	Debug print register (simulation only)
	uint32_t 	CREG_IRQ_CLR_ENABLE;  	// W	0x500	Interrupt clear enable register
	uint32_t 	CREG_IRQ_SET_ENABLE;  	// W	0x504	Interrupt set enable register
	uint32_t 	CREG_IRQ_STATUS;  	// R	0x508	Interrupt status register
	uint32_t 	CREG_IRQ_ENABLE;  	// R	0x50C	Interrupt enable register
	uint32_t 	CREG_IRQ_CLR_STATUS;  	// W	0x510	Interrupt clear status register
	uint32_t 	CREG_IRQ_SET_STATUS;  	// W	0x514	Interrupt set status register
	uint32_t 	reserved31[694];
	uint32_t 	CREG_IP_SW_RESET;  	// RW1C	0xFF0	CREG IP software reset register
	uint32_t 	reserved32[1];
	uint32_t 	CREG_IP_VERSION;  	// R	0xFF8	CREG IP version register
	uint32_t 	CREG_IP_TYPE;  	// R	0xFFC	CREG IP type register
}  CREG_HSDC_STRUCT, * CREG_HSDC_STRUCT_PTR;
#endif
