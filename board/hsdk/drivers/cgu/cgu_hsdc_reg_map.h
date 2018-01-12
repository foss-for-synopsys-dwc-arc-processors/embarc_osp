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
#ifndef _CGU_HSDC_REG_MAP_H_
#define _CGU_HSDC_REG_MAP_H_
typedef volatile struct cgu_hsdc_reg
{
	uint32_t 	CGU_ARC_PLL_CTRL;  	// RW	0x000	ARC PLL control register
	uint32_t 	CGU_ARC_PLL_STATUS;  	// R	0x004	ARC PLL status register
	uint32_t 	CGU_ARC_PLL_FMEAS;  	// RW	0x008	ARC PLL frequency measurement register
	uint32_t 	CGU_ARC_PLL_MON;  	// RW	0x00C	ARC PLL monitor register
	uint32_t 	CGU_SYS_PLL_CTRL;  	// RW	0x010	SYS PLL control register
	uint32_t 	CGU_SYS_PLL_STATUS;  	// R	0x014	SYS PLL status register
	uint32_t 	CGU_SYS_PLL_FMEAS;  	// RW	0x018	SYS PLL frequency measurement register
	uint32_t 	CGU_SYS_PLL_MON;  	// RW	0x01C	SYS PLL monitor register
	uint32_t 	CGU_DDR_PLL_CTRL;  	// RW	0x020	DDR PLL control register
	uint32_t 	CGU_DDR_PLL_STATUS;  	// R	0x024	DDR PLL status register
	uint32_t 	CGU_DDR_PLL_FMEAS;  	// RW	0x028	DDR PLL frequency measurement register
	uint32_t 	CGU_DDR_PLL_MON;  	// RW	0x02C	DDR PLL monitor register
	uint32_t 	CGU_TUN_PLL_CTRL;  	// RW	0x030	Tunnel PLL control register
	uint32_t 	CGU_TUN_PLL_STATUS;  	// R	0x034	Tunnel PLL status register
	uint32_t 	CGU_TUN_PLL_FMEAS;  	// RW	0x038	Tunnel PLL frequency measurement register
	uint32_t 	CGU_TUN_PLL_MON;  	// RW	0x03C	Tunnel PLL monitor register
	uint32_t 	CGU_HDMI_PLL_CTRL;  	// RW	0x040	HDMI PLL control register
	uint32_t 	CGU_HDMI_PLL_STATUS;  	// R	0x044	HDMI PLL status register
	uint32_t 	CGU_HDMI_PLL_FMEAS;  	// RW	0x048	HDMI PLL frequency measurement register
	uint32_t 	CGU_HDMI_PLL_MON;  	// RW	0x04C	HDMI PLL monitor register
	uint32_t 	reserved1[12];
	uint32_t 	CGU_ARC_IDIV;  	// RW	0x080	Integer divider register for ARC HS38x4 clock
	uint32_t 	CGU_ARC_FMEAS;  	// RW	0x084	Freq measurement register for ARC HS38x4 clock
	uint32_t 	reserved2[62];
	uint32_t 	CGU_SYS_IDIV_APB;  	// RW	0x180	Integer divider register for APB clock
	uint32_t 	CGU_SYS_FMEAS_APB;  	// RW	0x184	Freq measurement register for APB clock
	uint32_t 	reserved3[2];
	uint32_t 	CGU_SYS_IDIV_AXI;  	// RW	0x190	Integer divider register for AXI clock
	uint32_t 	CGU_SYS_FMEAS_AXI;  	// RW	0x194	Freq measurement register for AXI clock
	uint32_t 	reserved4[2];
	uint32_t 	CGU_SYS_IDIV_ETH;  	// RW	0x1A0	Integer divider register for ETH clock
	uint32_t 	CGU_SYS_FMEAS_ETH;  	// RW	0x1A4	Freq measurement register for ETH clock
	uint32_t 	reserved5[2];
	uint32_t 	CGU_SYS_IDIV_USB;  	// RW	0x1B0	Integer divider register for USB clock
	uint32_t 	CGU_SYS_FMEAS_USB;  	// RW	0x1B4	Freq measurement register for USB clock
	uint32_t 	reserved6[2];
	uint32_t 	CGU_SYS_IDIV_SDIO;  	// RW	0x1C0	Integer divider register for SDIO clock
	uint32_t 	CGU_SYS_FMEAS_SDIO;  	// RW	0x1C4	Freq measurement register for SDIO clock
	uint32_t 	reserved7[2];
	uint32_t 	CGU_SYS_IDIV_HDMI;  	// RW	0x1D0	Integer divider register for HDMI clock
	uint32_t 	CGU_SYS_FMEAS_HDMI;  	// RW	0x1D4	Freq measurement register for HDMI clock
	uint32_t 	reserved8[2];
	uint32_t 	CGU_SYS_IDIV_GFX_CORE;  	// RW	0x1E0	Integer divider register for GFX core clock
	uint32_t 	CGU_SYS_FMEAS_GFX_CORE;  	// RW	0x1E4	Freq measurement register for GFX core clock
	uint32_t 	reserved9[2];
	uint32_t 	CGU_SYS_IDIV_GFX_DMA;  	// RW	0x1F0	Integer divider register for GFX dma clock
	uint32_t 	CGU_SYS_FMEAS_GFX_DMA;  	// RW	0x1F4	Freq measurement register for GFX dma clock
	uint32_t 	reserved10[2];
	uint32_t 	CGU_SYS_IDIV_GFX_CFG;  	// RW	0x200	Integer divider register for GFX config clock
	uint32_t 	CGU_SYS_FMEAS_GFX_CFG;  	// RW	0x204	Freq measurement register for GFX config clock
	uint32_t 	reserved11[2];
	uint32_t 	CGU_SYS_IDIV_DMAC_CORE;  	// RW	0x210	Integer divider register for DMAC clock
	uint32_t 	CGU_SYS_FMEAS_DMAC_CORE;  	// RW	0x214	Freq measurement register for DMAC clock
	uint32_t 	reserved12[2];
	uint32_t 	CGU_SYS_IDIV_DMAC_CFG;  	// RW	0x220	Integer divider register for DMAC config clock
	uint32_t 	CGU_SYS_FMEAS_DMAC_CFG;  	// RW	0x224	Freq measurement register for DMAC config clock
	uint32_t 	reserved13[2];
	uint32_t 	CGU_SYS_IDIV_SDIO_REF;  	// RW	0x230	Integer divider register for SDIO reference clock
	uint32_t 	CGU_SYS_FMEAS_SDIO_REF;  	// RW	0x234	Freq measurement register for SDIO reference clock
	uint32_t 	reserved14[2];
	uint32_t 	CGU_SYS_IDIV_SPI_REF;  	// RW	0x240	Integer divider register for SPI reference clock
	uint32_t 	CGU_SYS_FMEAS_SPI_REF;  	// RW	0x244	Freq measurement register for SPI reference clock
	uint32_t 	reserved15[2];
	uint32_t 	CGU_SYS_IDIV_I2C_REF;  	// RW	0x250	Integer divider register for I2C reference clock
	uint32_t 	CGU_SYS_FMEAS_I2C_REF;  	// RW	0x254	Freq measurement register for I2C reference clock
	uint32_t 	reserved16[2];
	uint32_t 	CGU_SYS_IDIV_UART_REF;  	// RW	0x260	Integer divider register for UART reference clock
	uint32_t 	CGU_SYS_FMEAS_UART_REF;  	// RW	0x264	Freq measurement register for UART reference clock
	uint32_t 	reserved17[2];
	uint32_t 	CGU_SYS_IDIV_EBI_REF;  	// RW	0x270	Integer divider register for EBI reference clock
	uint32_t 	CGU_SYS_FMEAS_EBI_REF;  	// RW	0x274	Freq measurement register for EBI reference clock
	uint32_t 	reserved18[66];
	uint32_t 	CGU_TUN_IDIV;  	// RW	0x380	Integer divider register for Tunnel clock
	uint32_t 	CGU_TUN_FMEAS;  	// RW	0x384	Freq measurement register for Tunnel clock
	uint32_t 	reserved19[62];
	uint32_t 	CGU_HDMI_IDIV_APB;  	// RW	0x480	Integer divider register for HDMI clock
	uint32_t 	CGU_HDMI_FMEAS_APB;  	// RW	0x484	Freq measurement register for HDMI clock
	uint32_t 	reserved20[62];
	uint32_t 	CGU_I2S_IDIV_TX;  	// RW	0x580	Integer divider register for I2S TX clock
	uint32_t 	CGU_I2S_FMEAS_TX;  	// RW	0x584	Freq measurement register for I2S TX clock
	uint32_t 	reserved21[2];
	uint32_t 	CGU_I2S_IDIV_RX;  	// RW	0x590	Integer divider register for I2S RX clock
	uint32_t 	CGU_I2S_FMEAS_RX;  	// RW	0x594	Freq measurement register for I2S RX clock
	uint32_t 	reserved22[186];
	uint32_t 	CGU_ARC_RST_CTRL;  	// RW	0x880	Reset control register for ARC HS38x4
	uint32_t 	reserved23[7];
	uint32_t 	CGU_SYS_RST_CTRL;  	// RW	0x8A0	Reset control register for System peripherals
	uint32_t 	reserved24[7];
	uint32_t 	CGU_DDR_RST_CTRL;  	// RW	0x8C0	Reset control register for DDR
	uint32_t 	reserved25[7];
	uint32_t 	CGU_TUN_RST_CTRL;  	// RW	0x8E0	Reset control register for Tunnel
	uint32_t 	reserved26[39];
	uint32_t 	CGU_OUT_RST_CTRL;  	// RW	0x980	Reset control register for IC reset output
	uint32_t 	reserved27[411];
	uint32_t 	CGU_IP_SW_RESET;  	// RW1C	0xFF0	CGU IP software reset register
	uint32_t 	reserved28[1];
	uint32_t 	CGU_IP_VERSION;  	// R	0xFF8	CGU IP version register
	uint32_t 	CGU_IP_TYPE;  	// R	0xFFC	CGU IP type register
}  CGU_HSDC_STRUCT, * CGU_HSDC_STRUCT_PTR;
#endif
