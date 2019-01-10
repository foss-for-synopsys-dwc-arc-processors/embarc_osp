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
#ifndef __BOARD_EMSDP_H__
#define __BOARD_EMSDP_H__

/**
 * Clock
 */
#define EMSDP_REF_CLOCK                                   (100000000)

#define EMSDP_UART_REF_CLOCK                              (100000000)
#define EMSDP_AUDIO_REF_CLOCK                             (24576000)

#if (defined(FAST_UART) )
   #define EMSDP_UART_BAUDRATE                            (6250000)
#else
   #define EMSDP_UART_BAUDRATE                            (115200)
#endif

#define EMSDP_I2C_CLOCK                                   (EMSDP_REF_CLOCK)

//APB Peripheral address map
#define EMSDP_CRU_BASE                                    (0xF0000000U)
#define EMSDP_CREG_BASE                                   (0xF0001000U)
#define EMSDP_GPIO_BASE                                   (0xF0002000U)
#define EMSDP_TRNG_BASE                                   (0xF0003000U)
#define EMSDP_DBG_UART_BASE                               (0xF0004000U)
#define EMSDP_WDT_BASE                                    (0xF0005000U)
#define EMSDP_TIMERS0_BASE                                (0xF0006000U)
#define EMSDP_TIMERS1_BASE                                (0xF0007000U)
#define EMSDP_SPI_BASE                                    (0xF0008000U)
#define EMSDP_UART_BASE                                   (0xF0009000U)
#define EMSDP_SDIO_BASE                                   (0xF0010000U)
#define EMSDP_FLASH_BASE                                  (0xF1000000U)
#define EMSDP_EBI_BASE                                    (0xF2000000U)
#define EMSDP_PSRAM_BASE                                  (0xF2001000U)
/**
* Mapping of printf information
*/
#define DBG_init  						            dbg_init
#define DBG_print                                        dbg_printf
#define DBG_getChar(x)							                                 \
{									                                                \
   DW_UART_STRUCT_PTR   uart  = (DW_UART_STRUCT_PTR)  (DBG_PORT);          \
   x = dw_uart_getChar(uart);						                              \
}
#define DBG_finish()
#define DBG_finish_with_error()
#define DBG_install(x)                                                     \
{                                                                          \
   DW_UART_STRUCT_PTR   uart  = (DW_UART_STRUCT_PTR)  (DBG_PORT);          \
   uart->IER.DATA32 = DW_UART_IER_DATA_AVAIL;                              \
   board_installHandler(x, DBG_ISR);                                       \
}


//ARC Exceptions
#define EMSDP_RESET_EXCP                                     (0)
#define EMSDP_MEMORY_ERROR_EXCP                              (1)
#define EMSDP_INSTRUCTION_ERROR_EXCP                         (2)
#define EMSDP_EVMACHINECHECK_EXCP                            (3)
#define EMSDP_EVITLBMISS_EXCP                                (4)
#define EMSDP_EVDTLBMISS_EXCP                                (5)
#define EMSDP_EVPROTV_EXCP                                   (6)
#define EMSDP_EVPRIVILEGEV_EXCP                              (7)
#define EMSDP_EVSWI_EXCP                                     (8)
#define EMSDP_EVTRAP_EXCP                                    (9)
#define EMSDP_EVEXTENSION_EXCP                               (10)
#define EMSDP_EVDIVZERO_EXCP                                 (11)
#define EMSDP_EVDCERROR_EXCP                                 (12)
#define EMSDP_EVMALIGNED_EXCP                                (13)

//ARC External private interrupts (low interrupts)
#define EMSDP_TIMER0_EXCP                                    (16)
#define EMSDP_TIMER1_EXCP                                    (17)

#define EMSDP_MAX_NUM_EXCP                                   (24)
#define EMSDP_MAX_NUM_ISR                                    (80)


//EMSDP Peripheral Subsystem Interrupt assignments
#define EMSDP_SPI0_INTR                                       (83)
#define EMSDP_SPI1_INTR                                       (84)
#define EMSDP_GPIO_INTR0                                      (85)
#define EMSDP_GPIO_INTR1                                      (86)
#define EMSDP_GPIO_INTR2                                      (87)
#define EMSDP_GPIO_INTR3                                      (88)
#define EMSDP_TIMER0_INTR0                                    (89)
#define EMSDP_TIMER0_INTR1                                    (90)
#define EMSDP_TIMER0_INTR2                                    (91)
#define EMSDP_TIMER0_INTR3                                    (92)
#define EMSDP_TIMER0_INTR4                                    (93)
#define EMSDP_TIMER0_INTR5                                    (94)
#define EMSDP_TIMER0_INTR6                                    (95)
#define EMSDP_TIMER0_INTR7                                    (96)
#define EMSDP_TIMER1_INTR0                                    (97)
#define EMSDP_TIMER1_INTR1                                    (98)
#define EMSDP_TIMER1_INTR2                                    (99)
#define EMSDP_TIMER1_INTR3                                    (100)
#define EMSDP_TIMER1_INTR4                                    (101)
#define EMSDP_TIMER1_INTR5                                    (102)
#define EMSDP_TIMER1_INTR6                                    (103)
#define EMSDP_TIMER1_INTR7                                    (104)
#define EMSDP_UART_INTR                                       (105)
#define EMSDP_WDT_INTR                                        (106)
#define EMSDP_DBG_UART_INTR                                   (107)
#define EMSDP_SDIO_INTR                                       (108)
#define EMSDP_REDPINE_SPI_INTR                                (109)
#define EMSDP_REDPINE_HOST_WAKEUP                             (110)
#define EMSDP_NIST_TRNG_INTR                                  (111)


//EMSDP Peripheral Subsystem DMA assignments
#define UART_DMA_TX                                           (9)
#define UART_DMA_RX                                           (10)
#define REDPINE_SPI_DMA_TX                                    (11)
#define REDPINE_SPI_DMA_RX                                    (12)
#define FLASH_SPI_DMA_TX                                      (13)
#define FLASH_SPI_DMA_RX                                      (14)

#endif //__BOARD_EMSDP_H__
