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
#ifndef _HSDK_HARDWARE_H_
#define _HSDK_HARDWARE_H_

#include "arc_feature_config.h"

#define HSDC_KHZ(khz)				((khz)*1000)
#define HSDC_MHZ(mhz)				(HSDC_KHZ(mhz)*1000)

#define HSDC_CLK_ARC_PLL			HSDC_MHZ(500)
#define HSDC_CLK_SYS_PLL			HSDC_MHZ(400)
#define HSDC_CLK_TUNNEL_PLL			HSDC_MHZ(150)
#define HSDC_CLK_HDMI_PLL			HSDC_MHZ(200)
#define HSDC_CLK_I2S_PLL			HSDC_MHZ(100)
#define HSDC_CLK_DDR_PLL			HSDC_MHZ(666)

#define HSDC_ARC_PLL_ARC_DIV			(1)

#define HSDC_SYS_PLL_APB_DIV			(2)
#define HSDC_SYS_PLL_AXI_DIV			(1)
#define HSDC_SYS_PLL_ETH_DIV			(1)
#define HSDC_SYS_PLL_USB_DIV			(1)
#define HSDC_SYS_PLL_SDIO_DIV			(1)
#define HSDC_SYS_PLL_HDMI_DIV			(1)
#define HSDC_SYS_PLL_GFXCORE_DIV		(1)
#define HSDC_SYS_PLL_GFXDMA_DIV			(1)
#define HSDC_SYS_PLL_GFXCFG_DIV			(2)
#define HSDC_SYS_PLL_DMACORE_DIV		(1)
#define HSDC_SYS_PLL_DMACFG_DIV			(2)
#define HSDC_SYS_PLL_SDIOREF_DIV		(4)
#define HSDC_SYS_PLL_SPIREF_DIV			(12)
#define HSDC_SYS_PLL_I2CREF_DIV			(2)
#define HSDC_SYS_PLL_UARTREF_DIV		(12)
#define HSDC_SYS_PLL_EBIREF_DIV			(8)

#define HSDC_TUNNEL_PLL_TUNNEL_DIV		(3)
#define HSDC_TUNNEL_PLL_ROM_DIV			(1)
#define HSDC_TUNNEL_PLL_PWM_DIV			(2)

#define HSDC_HDMI_PLL_HDMI_DIV			(1)

#define HSDC_I2S_PLL_TX_DIV			(1)
#define HSDC_I2S_PLL_RX_DIV			(1)


/** CPU Clock Frequency definition */
#define CLK_CPU					(HSDC_CLK_ARC_PLL)
/** Peripheral Bus Reference Clock definition */
#define CLK_BUS_APB				(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_APB_DIV)

#define HSDC_CLK_SYS_PLL_APB			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_APB_DIV)
#define HSDC_CLK_SYS_PLL_AXI			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_AXI_DIV)
#define HSDC_CLK_SYS_PLL_ETH			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_ETH_DIV)
#define HSDC_CLK_SYS_PLL_USB			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_USB_DIV)
#define HSDC_CLK_SYS_PLL_SDIO			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_SDIO_DIV)
#define HSDC_CLK_SYS_PLL_HDMI			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_HDMI_DIV)
#define HSDC_CLK_SYS_PLL_GFXCORE		(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_GFXCORE_DIV)
#define HSDC_CLK_SYS_PLL_GFXDMA			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_GFXDMA_DIV)
#define HSDC_CLK_SYS_PLL_GFXCFG			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_GFXCFG_DIV)
#define HSDC_CLK_SYS_PLL_DMACORE		(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_DMACORE_DIV)
#define HSDC_CLK_SYS_PLL_DMACFG			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_DMACFG_DIV)
#define HSDC_CLK_SYS_PLL_SDIOREF		(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_SDIOREF_DIV)
#define HSDC_CLK_SYS_PLL_SPIREF			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_SPIREF_DIV)
#define HSDC_CLK_SYS_PLL_I2CREF			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_I2CREF_DIV)
#define HSDC_CLK_SYS_PLL_UARTREF		(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_UARTREF_DIV)
#define HSDC_CLK_SYS_PLL_EBIREF			(HSDC_CLK_SYS_PLL/HSDC_SYS_PLL_EBIREF_DIV)

#define HSDC_CLK_TUNNEL_PLL_TUNNEL		(HSDC_CLK_TUNNEL_PLL/HSDC_TUNNEL_PLL_TUNNEL_DIV)
#define HSDC_CLK_TUNNEL_PLL_ROM			(HSDC_CLK_TUNNEL_PLL/HSDC_TUNNEL_PLL_ROM_DIV)
#define HSDC_CLK_TUNNEL_PLL_PWM			(HSDC_CLK_TUNNEL_PLL/HSDC_TUNNEL_PLL_PWM_DIV)

#define HSDC_CLK_HDMI_PLL_HDMI			(HSDC_CLK_HDMI_PLL/HSDC_HDMI_PLL_HDMI_DIV)

#define HSDC_CLK_I2S_PLL_TX			(HSDC_CLK_I2S_PLL/HSDC_I2S_PLL_TX_DIV)
#define HSDC_CLK_I2S_PLL_RX			(HSDC_CLK_I2S_PLL/HSDC_I2S_PLL_RX_DIV)

//APB Peripheral address map
//AXI2APB Bridge1
#define HSDC_CGU_REGBASE			(0xF0000000U)
#define HSDC_CREG_REGBASE			(0xF0001000U)
#define HSDC_GPIO_REGBASE			(0xF0003000U)
#define HSDC_UMCTL2_REGBASE			(0xF0004000U)
#define HSDC_UMCTL2_PUB_REGBASE			(0xF0004800U)
#define HSDC_UART3_REGBASE			(0xF0005000U)
#define HSDC_WDT_REGBASE			(0xF0006000U)
#define HSDC_DVFS_REGBASE			(0xF0007000U)
//AXI2APB Bridge2
#define HSDC_GMAC_CTRL_REGBASE			(0xF0008000U)
#define HSDC_GMAC_DMA_REGBASE			(0xF0009000U)
#define HSDC_SDIO_REGBASE			(0xF000A000U)
#define HSDC_PGU_REGBASE			(0xF000B000U)
//AXI2APB Bridge3
#define HSDC_HDMI_REGBASE			(0xF0010000U)
//AXI2APB Bridge4
#define HSDC_SPI0_REGBASE			(0xF0020000U)
#define HSDC_SPI1_REGBASE			(0xF0021000U)
#define HSDC_SPI2_REGBASE			(0xF0022000U)
#define HSDC_I2C0_REGBASE			(0xF0023000U)
#define HSDC_I2C1_REGBASE			(0xF0024000U)
#define HSDC_I2C2_REGBASE			(0xF0025000U)
#define HSDC_UART0_REGBASE			(0xF0026000U)
#define HSDC_UART1_REGBASE			(0xF0027000U)
#define HSDC_UART2_REGBASE			(0xF0028000U)
#define HSDC_I2S_TX_REGBASE			(0xF0029000U)
#define HSDC_I2S_RX_REGBASE			(0xF002A000U)
#define HSDC_PWM_REGBASE			(0xF002B000U)
//AXI2AHB Bridge1
#define HSDC_USB_HOST_REGBASE			(0xF0040000U)
#define HSDC_USBH_EHCI_REGBASE			(HSDC_USB_HOST_REGBASE)
#define HSDC_USBH_OHCI_REGBASE			(HSDC_USB_HOST_REGBASE + 0x20000)
//AXI2AHB Bridge2
#define HSDC_DMAC_REGBASE			(0xF0080000U)
//AXI2AHB Bridge3
#define HSDC_GFX_REGBASE			(0xF0090000U)
//AXI2AHB Bridge4
#define HSDC_EBI_REGBASE			(0xF00D0000U)


//ARC Exceptions
#define HSDC_RESET_EXCP				(0)
#define HSDC_MEMORY_ERROR_EXCP			(1)
#define HSDC_INSTRUCTION_ERROR_EXCP		(2)
#define HSDC_EVMACHINECHECK_EXCP		(3)
#define HSDC_EVITLBMISS_EXCP			(4)
#define HSDC_EVDTLBMISS_EXCP			(5)
#define HSDC_EVPROTV_EXCP			(6)
#define HSDC_EVPRIVILEGEV_EXCP			(7)
#define HSDC_EVSWI_EXCP				(8)
#define HSDC_EVTRAP_EXCP			(9)
#define HSDC_EVEXTENSION_EXCP			(10)
#define HSDC_EVDIVZERO_EXCP			(11)
#define HSDC_EVDCERROR_EXCP			(12)
#define HSDC_EVMALIGNED_EXCP			(13)

//ARC External private interrupts (low interrupts)
#define HSDC_TIMER0_EXCP			(16)
#define HSDC_TIMER1_EXCP			(17)

#define HSDC_MCIP_EXCP				(19)

#define HSDK_MAX_NUM_EXCP			(24)

//ISR MCIP mapping (level sensitive except a few)
#define HSDC_CREG_SW0_ISR			(24)
#define HSDC_CREG_SW1_ISR			(25)
#define HSDC_CREG_SW2_ISR			(26)
#define HSDC_CREG_SW3_ISR			(27)
#define HSDC_TUNNEL_ISR				(28)
#define HSDC_CREG_UPDATE_ISR			(29)      //edge sensitive
#define HSDC_UART3_ISR				(30)
#define HSDC_CGU_PLL_LOCK_ISR			(31)      //edge sensitive
#define HSDC_CGU_PLL_UNLOCK_ISR			(32)      //edge sensitive
#define HSDC_CGU_PLL_LOCKERROR_ISR		(33)      //edge sensitive
#define HSDC_ETH_MAC_ISR			(34)
#define HSDC_ETH_PHY_ISR			(35)
#define HSDC_SDIO_ISR				(36)
#define HSDC_PGU_ISR				(37)
#define HSDC_HDMI_ISR				(38)
#define HSDC_USB_HOST_ISR			(39)
#define HSDC_SPI0_ISR				(40)
#define HSDC_SPI1_ISR				(41)
#define HSDC_SPI2_ISR				(42)
#define HSDC_I2C0_ISR				(43)
#define HSDC_I2C1_ISR				(44)
#define HSDC_I2C2_ISR				(45)
#define HSDC_UART0_ISR				(46)
#define HSDC_UART1_ISR				(47)
#define HSDC_UART2_ISR				(48)
#define HSDC_I2S_TX_ISR				(49)
#define HSDC_I2S_RX_ISR				(50)
#define HSDC_DMAC_ISR				(51)
#define HSDC_GFX_ISR				(52)
#define HSDC_WDT_ISR				(53)
#define HSDC_PWM_ISR				(54)
#define HSDC_DVFS_ISR				(55)
#define HSDC_GPIO0_ISR				(56)
#define HSDC_GPIO1_ISR				(57)
#define HSDC_GPIO2_ISR				(58)
#define HSDC_GPIO3_ISR				(59)
#define HSDC_GPIO4_ISR				(60)
#define HSDC_GPIO5_ISR				(61)
#define HSDC_GPIO6_ISR				(62)
#define HSDC_GPIO7_ISR				(63)
#define HSDC_GPIO8_ISR				(64)
#define HSDC_GPIO9_ISR				(65)
#define HSDC_GPIO10_ISR				(66)
#define HSDC_GPIO11_ISR				(67)
#define HSDC_GPIO12_ISR				(68)
#define HSDC_GPIO13_ISR				(69)
#define HSDC_GPIO14_ISR				(70)
#define HSDC_GPIO15_ISR				(71)
#define HSDC_GPIO16_ISR				(72)
#define HSDC_GPIO17_ISR				(73)
#define HSDC_GPIO18_ISR				(74)
#define HSDC_GPIO19_ISR				(75)
#define HSDC_GPIO20_ISR				(76)
#define HSDC_GPIO21_ISR				(77)
#define HSDC_GPIO22_ISR				(78)
#define HSDC_GPIO23_ISR				(79)
#define HSDK_MAX_NUM_ISR			(80)

#define ARC_CONNECT_NUM_INT_ALL			(HSDK_MAX_NUM_ISR-HSDC_CREG_SW0_ISR)


//DMA flow control mapping
#define HSDC_DMAC_SPI0_TX			(0)
#define HSDC_DMAC_SPI0_RX			(1)
#define HSDC_DMAC_I2C0_TX			(2)
#define HSDC_DMAC_I2C0_RX			(3)
#define HSDC_DMAC_UART0_TX			(4)
#define HSDC_DMAC_UART0_RX			(5)
#define HSDC_DMAC_I2S0_TX			(6)
#define HSDC_DMAC_I2S0_RX			(7)

#define HSDC_SRAM_SIZE				(0x00040000U)

#define HSDC_SPI_CS_0				DEV_SPI_CS_LINE_0
#define HSDC_SPI_CS_1				DEV_SPI_CS_LINE_1
#define HSDC_SPI_CS_2				DEV_SPI_CS_LINE_2
#define HSDC_SPI_CS_3				DEV_SPI_CS_LINE_3

#ifdef __cplusplus
extern "C" {
#endif

extern void hsdk_hardware_init(void);

#ifdef __cplusplus
}
#endif

#endif	/* _HSDK_HARDWARE_H_ */
