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
#ifndef _CORE_HARDWARE_H_
#define _CORE_HARDWARE_H_

#define EMSDP_SPI0_INTR				(99)
#define EMSDP_SPI1_INTR				(100)
#define EMSDP_GPIO_INTR0			(101)
#define EMSDP_GPIO_INTR1			(102)
#define EMSDP_GPIO_INTR2			(103)
#define EMSDP_GPIO_INTR3			(104)
#define EMSDP_TIMER0_INTR0			(105)
#define EMSDP_TIMER0_INTR1			(106)
#define EMSDP_TIMER0_INTR2			(107)
#define EMSDP_TIMER0_INTR3			(108)
#define EMSDP_TIMER0_INTR4			(109)
#define EMSDP_TIMER0_INTR5			(110)
#define EMSDP_TIMER0_INTR6			(111)
#define EMSDP_TIMER0_INTR7			(112)
#define EMSDP_TIMER1_INTR0			(113)
#define EMSDP_TIMER1_INTR1			(114)
#define EMSDP_TIMER1_INTR2			(115)
#define EMSDP_TIMER1_INTR3			(116)
#define EMSDP_TIMER1_INTR4			(117)
#define EMSDP_TIMER1_INTR5			(118)
#define EMSDP_TIMER1_INTR6			(119)
#define EMSDP_TIMER1_INTR7			(120)
#define EMSDP_UART_INTR				(121)
#define EMSDP_WDT_INTR				(122)
#define EMSDP_DBG_UART_INTR			(123)
#define EMSDP_SDIO_INTR				(124)
#define EMSDP_REDPINE_SPI_INTR			(125)
#define EMSDP_REDPINE_HOST_WAKEUP		(126)

//EMSDP Peripheral Subsystem DMA assignments
#define UART_DMA_TX				(10)
#define UART_DMA_RX				(11)
#define REDPINE_SPI_DMA_TX			(12)
#define REDPINE_SPI_DMA_RX			(13)
#define FLASH_SPI_DMA_TX			(14)
#define FLASH_SPI_DMA_RX			(15)

#endif /* _CORE_HARDWARE_H_ */