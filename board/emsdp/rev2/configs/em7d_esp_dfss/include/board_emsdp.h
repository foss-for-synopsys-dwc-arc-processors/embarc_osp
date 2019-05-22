
/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/** Copyright (C) 1989-2016 Synopsys, Inc.                              **/
/** All Rights Reserved.                                                **/
/**                                                                     **/
/** SYNOPSYS CONFIDENTIAL                                               **/
/**                                                                     **/
/** This is an unpublished proprietary work of Synopsys, Inc., and is   **/
/** fully protected under copyright and trade secret laws. You may not  **/
/** view, use, disclose, copy, or distribute this file or any           **/
/** information contained herein except pursuant to a valid written     **/
/** license from Synopsys.                                              **/
/**                                                                     **/
/** For more information, contact                                       **/
/** est-adm@synopsys.com or call (650) 584-1631                         **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/
#ifndef __BOARD_EMSDP_H__
#define __BOARD_EMSDP_H__


/**							     
 * Clock							     
 */
#define EMSDP_REF_CLOCK                                   (100000000)

#define EMSDP_UART_REF_CLOCK                              (100000000)
#define EMSDP_AUDIO_REF_CLOCK                              (24576000)

#if (defined(FAST_UART) )
   #define EMSDP_UART_BAUDRATE                               (6250000)
#else
   #define EMSDP_UART_BAUDRATE                               (115200)
#endif

#define EMSDP_I2C_CLOCK                                      (EMSDP_REF_CLOCK)

//APB Peripheral address map				     
#define EMSDP_CRU_BASE                                       (0xF0000000U)
#define EMSDP_CREG_BASE                                      (0xF0001000U)
#define EMSDP_GPIO_BASE                                      (0xF0002000U)
#define EMSDP_TRNG_BASE                                      (0xF0003000U)
#define EMSDP_DBG_UART_BASE                                  (0xF0004000U)
#define EMSDP_WDT_BASE                                       (0xF0005000U)
#define EMSDP_TIMERS0_BASE                                   (0xF0006000U)
#define EMSDP_TIMERS1_BASE                                   (0xF0007000U)
#define EMSDP_SPI_BASE                                       (0xF0008000U)
#define EMSDP_UART_BASE                                      (0xF0009000U)
#define EMSDP_SDIO_BASE                                      (0xF0010000U)
#define EMSDP_FLASH_BASE                                     (0xF1000000U)
#define EMSDP_EBI_BASE                                       (0xF2000000U)
#define EMSDP_PSRAM_BASE                                     (0xF2001000U)


/**
* Mapping of printf information
*/
   #define DBG_init  							            dbg_init
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
#define EMSDP_SPI0_INTR                                       (84)
#define EMSDP_SPI1_INTR                                       (85)
#define EMSDP_GPIO_INTR0                                       (86)
#define EMSDP_GPIO_INTR1                                       (87)
#define EMSDP_GPIO_INTR2                                       (88)
#define EMSDP_GPIO_INTR3                                       (89)
#define EMSDP_TIMER0_INTR0                                       (90)
#define EMSDP_TIMER0_INTR1                                       (91)
#define EMSDP_TIMER0_INTR2                                       (92)
#define EMSDP_TIMER0_INTR3                                       (93)
#define EMSDP_TIMER0_INTR4                                       (94)
#define EMSDP_TIMER0_INTR5                                       (95)
#define EMSDP_TIMER0_INTR6                                       (96)
#define EMSDP_TIMER0_INTR7                                       (97)
#define EMSDP_TIMER1_INTR0                                       (98)
#define EMSDP_TIMER1_INTR1                                       (99)
#define EMSDP_TIMER1_INTR2                                       (100)
#define EMSDP_TIMER1_INTR3                                       (101)
#define EMSDP_TIMER1_INTR4                                       (102)
#define EMSDP_TIMER1_INTR5                                       (103)
#define EMSDP_TIMER1_INTR6                                       (104)
#define EMSDP_TIMER1_INTR7                                       (105)
#define EMSDP_UART_INTR                                       (106)
#define EMSDP_WDT_INTR                                       (107)
#define EMSDP_DBG_UART_INTR                                       (108)
#define EMSDP_SDIO_INTR                                       (109)
#define EMSDP_REDPINE_SPI_INTR                                       (110)
#define EMSDP_REDPINE_HOST_WAKEUP                                       (111)
#define EMSDP_NIST_TRNG_INTR                                       (112)


//EMSDP Peripheral Subsystem DMA assignments
#define UART_DMA_TX                                       (9)
#define UART_DMA_RX                                       (10)
#define REDPINE_SPI_DMA_TX                                       (11)
#define REDPINE_SPI_DMA_RX                                       (12)
#define FLASH_SPI_DMA_TX                                       (13)
#define FLASH_SPI_DMA_RX                                       (14)

#endif //__BOARD_EMSDP_H__
