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

#ifndef __CGU_HSDC_H__
#define __CGU_HSDC_H__

#include "cgu_hsdc_reg.h"

//Pll
typedef enum
{
	CGU_HSDC_PLL_ARC = 0,
	CGU_HSDC_PLL_SYS,
	CGU_HSDC_PLL_DDR,
	CGU_HSDC_PLL_TUNNEL,
	CGU_HSDC_PLL_HDMI,
	CGU_HSDC_PLL_NONE
} CGU_HSDC_PLL_T;

//Idiv
typedef enum
{
	CGU_HSDC_IDIV_ARC = 0,
	CGU_HSDC_IDIV_SYS,
	CGU_HSDC_IDIV_TUNNEL,
	CGU_HSDC_IDIV_HDMI,
	CGU_HSDC_IDIV_I2S,
	CGU_HSDC_IDIV_NONE
} CGU_HSDC_IDIV_T;

//Fmeas
typedef enum
{
	CGU_HSDC_FMEAS_ARC = 0,
	CGU_HSDC_FMEAS_SYS,
	CGU_HSDC_FMEAS_TUNNEL,
	CGU_HSDC_FMEAS_HDMI,
	CGU_HSDC_FMEAS_I2S,
	CGU_HSDC_FMEAS_NONE
} CGU_HSDC_FMEAS_T;

//Reset
typedef enum
{
	CGU_HSDC_RSTCTRL_ARC = 0,
	CGU_HSDC_RSTCTRL_SYS,
	CGU_HSDC_RSTCTRL_DDR,
	CGU_HSDC_RSTCTRL_TUNNEL,
	CGU_HSDC_RSTCTRL_NONE
} CGU_HSDC_RSTCTRL_T;

typedef struct
{
   uint32_t      freq;
   uint32_t      BS;
   uint32_t      F;
   uint32_t      R;
   uint32_t      OD;
   uint32_t      bypass;
   uint32_t      pd;
} CGU_REGISTER_PLL_SETTING_T;

typedef enum CGU_HSDC_PLL_FREQ
{
    CGU_HSDC_PLL_FREQ_BYPASS = 0
   ,CGU_HSDC_PLL_FREQ_100MHZ
   ,CGU_HSDC_PLL_FREQ_133MHZ
   ,CGU_HSDC_PLL_FREQ_150MHZ
   ,CGU_HSDC_PLL_FREQ_167MHZ
   ,CGU_HSDC_PLL_FREQ_200MHZ
   ,CGU_HSDC_PLL_FREQ_233MHZ
   ,CGU_HSDC_PLL_FREQ_250MHZ
   ,CGU_HSDC_PLL_FREQ_267MHZ
   ,CGU_HSDC_PLL_FREQ_300MHZ
   ,CGU_HSDC_PLL_FREQ_333MHZ
   ,CGU_HSDC_PLL_FREQ_350MHZ
   ,CGU_HSDC_PLL_FREQ_367MHZ
   ,CGU_HSDC_PLL_FREQ_400MHZ
   ,CGU_HSDC_PLL_FREQ_433MHZ
   ,CGU_HSDC_PLL_FREQ_450MHZ
   ,CGU_HSDC_PLL_FREQ_467MHZ
   ,CGU_HSDC_PLL_FREQ_500MHZ
   ,CGU_HSDC_PLL_FREQ_533MHZ
   ,CGU_HSDC_PLL_FREQ_550MHZ
   ,CGU_HSDC_PLL_FREQ_567MHZ
   ,CGU_HSDC_PLL_FREQ_600MHZ
   ,CGU_HSDC_PLL_FREQ_633MHZ
   ,CGU_HSDC_PLL_FREQ_650MHZ
   ,CGU_HSDC_PLL_FREQ_667MHZ
   ,CGU_HSDC_PLL_FREQ_700MHZ
   ,CGU_HSDC_PLL_FREQ_733MHZ
   ,CGU_HSDC_PLL_FREQ_750MHZ
   ,CGU_HSDC_PLL_FREQ_767MHZ
   ,CGU_HSDC_PLL_FREQ_800MHZ
   ,CGU_HSDC_PLL_FREQ_833MHZ
   ,CGU_HSDC_PLL_FREQ_867MHZ
   ,CGU_HSDC_PLL_FREQ_900MHZ
   ,CGU_HSDC_PLL_FREQ_933MHZ
   ,CGU_HSDC_PLL_FREQ_967MHZ
   ,CGU_HSDC_PLL_FREQ_1000MHZ
   ,CGU_HSDC_PLL_FREQ_NONE
} CGU_HSDC_PLL_FREQ_T;

//Arc: n
#define IDIV_ARC              0
#define IDIV_ARC_NONE         1

//Sys: n
#define IDIV_SYS_APB          0
#define IDIV_SYS_AXI          1
#define IDIV_SYS_ETH          2
#define IDIV_SYS_USB          3
#define IDIV_SYS_SDIO         4
#define IDIV_SYS_HDMI         5
#define IDIV_SYS_GFX_CORE     6
#define IDIV_SYS_GFX_DMA      7
#define IDIV_SYS_GFX_CFG      8
#define IDIV_SYS_DMAC_CORE    9
#define IDIV_SYS_DMAC_CFG     10
#define IDIV_SYS_SDIO_REF     11
#define IDIV_SYS_SPI_REF      12
#define IDIV_SYS_I2C_REF      13
#define IDIV_SYS_UART_REF     14
#define IDIV_SYS_EBI_REF      15
#define IDIV_SYS_NONE         16

//Tunnel: n
#define IDIV_TUNNEL           0
#define IDIV_ROM              1
#define IDIV_PWM              2
#define IDIV_TUNNEL_NONE      3

//Hdmi: n
#define IDIV_HDMI             0
#define IDIV_HDMI_NONE        1

//I2s: n
#define IDIV_I2S_TX           0
#define IDIV_I2S_RX           1
#define IDIV_I2S_NONE         2
//arc
#define ARC_ARC_RST        (0x1U << 16)

//sys
#define SYS_APB_RST        (0x1U << 16)
#define SYS_AXI_RST        (0x1U << 17)
#define SYS_ETH_RST        (0x1U << 18)
#define SYS_USB_RST        (0x1U << 19)
#define SYS_SDIO_RST       (0x1U << 20)
#define SYS_HDMI_RST       (0x1U << 21)
#define SYS_GFX_RST        (0x1U << 22)
#define SYS_DMAC_RST       (0x1U << 25)
#define SYS_EBI_RST        (0x1U << 31)

//ddr
#define DDR_DDR_RST        (0x1U << 0)

//tunnel
#define TUN_TUN_RST        (0x1U << 16)

typedef struct
{
	CGU_HSDC_PLL_T pll;
	uint32_t freq;
	uint32_t intnum;  //lock interrupt 0
} CGU_HSDC_PLL_CONFIG_T;


extern void cgu_hsdc_init(CGU_HSDC_STRUCT_PTR cgu_regs);

//Pll functions
extern int32_t cgu_hsdc_set_pll(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_CONFIG_T *config);
extern uint32_t cgu_hsdc_compare_pll(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_CONFIG_T *config);

//Get value from table
extern uint32_t cgu_hsdc_get_pll_freq(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_FREQ_T freq);
extern uint32_t cgu_hsdc_is_pll_in_bypass(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_T pll);

//FMeas functions
extern uint32_t cgu_hsdc_measure_pll(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_T pll, uint32_t fref);

//Idiv functions
extern int32_t cgu_hsdc_set_idiv(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_IDIV_T idiv, uint8_t n, uint8_t div);
extern uint8_t cgu_hsdc_get_idiv(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_IDIV_T idiv, uint8_t n);

//FMeas functions
extern uint32_t cgu_hsdc_measure_clk(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_FMEAS_T fmeas, uint8_t n, uint32_t fref);

//Reset IP's
extern void  cgu_hsdc_sw_reset_ctrl(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_RSTCTRL_T ctrl, uint32_t rst);
extern void  cgu_hsdc_sw_reset(CGU_HSDC_STRUCT_PTR cgu_regs, uint32_t delay);

#endif //__CGU_HSDC_H__
